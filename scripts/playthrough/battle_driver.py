#!/usr/bin/env python3
"""Headless per-turn battle driver for the authored campaign trainers.

Synthetic benchmark evidence only. This starts one trainer battle in the
EC_HEADLESS_FIXTURES ROM through the native debug battle lifecycle, prepares a
user-authorized stage-legal party through the existing native preparation API,
and then answers every player decision point from a memory mailbox. No buttons,
no screenshots, no story receipts. It never earns campaign progress.

    start   boot, prepare the party, begin the battle, stop at the first decision
    state   print the current semantic state (read-only; never advances)
    act     validate and submit this decision point's commands, advance, report
    result  print the final outcome

See docs/VERIFICATION.md, "Headless per-turn battle driver".
"""
import argparse
import hashlib
import json
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'scripts' / 'playthrough'))
import native_tools
import render_emerald_champions_ui as ui
from rom_artifacts import verify_rom_elf_pair
from prepare_party import protocol as prepare_protocol

RTC_EPOCH = '946684800'
BOOT_FRAMES = 2400
PREP_FRAMES = 240
TURN_FRAMES = 10000
MAX_CHUNKS = 8  # long multi-battle faint/exit sequences need more than one chunk
# Mechanical text/animation advance; the runner accepts at most 512 key events.
TEXT_PULSE_EVERY = 20
TEXT_PULSE_SPAN = 10000

VIEW_WORDS = 500
BATTLER_BASE, BATTLER_SIZE = 32, 28
PARTY_BASE, PARTY_SIZE_W = 144, 12
FOE_BASE, FOE_SIZE = 216, 3
LEGAL_BASE, LEGAL_SIZE = 252, 6
MSG_BASE, MSG_SIZE, MSG_COUNT = 276, 14, 14
PREV_BASE, PREV_SIZE = 472, 4
MSG_CHARS = (MSG_SIZE - 1) * 4

PHASES = ['idle', 'starting', 'running', 'await_action', 'await_switch', 'ended']
ACTIONS = {0: 'use_move', 1: 'use_item', 2: 'switch', 3: 'run', 10: 'exec_script',
           13: 'nothing_fainted', 0xFF: 'none'}
GIMMICKS = ['none', 'mega', 'ultra_burst', 'z_move', 'dynamax', 'tera']
OUTCOMES = {0: 'ongoing', 1: 'won', 2: 'lost', 3: 'drew', 4: 'ran', 5: 'player_teleported',
            6: 'mon_fled', 7: 'caught', 8: 'no_safari_balls', 9: 'forfeited', 10: 'mon_teleported'}
TERRAINS = ['none', 'grassy', 'misty', 'electric', 'psychic']
MOVE_TARGETS = ['none', 'selected', 'smart', 'depends', 'opponent', 'random', 'both', 'user',
                'ally', 'user_and_ally', 'user_or_ally', 'foes_and_ally', 'field',
                'opponents_field', 'all_battlers']
# gBattleCommunication[battler] >= this means the action for this turn is locked in.
ACTION_CONFIRMED = 4
STAT_NAMES = ['hp', 'atk', 'def', 'spe', 'spa', 'spd', 'acc', 'eva']

DEFINE_GROUPS = {
    'weather': ('constants/battle.h', ['B_WEATHER_RAIN_NORMAL', 'B_WEATHER_RAIN_PRIMAL',
        'B_WEATHER_RAIN_DOWNPOUR', 'B_WEATHER_SUN_NORMAL', 'B_WEATHER_SUN_PRIMAL',
        'B_WEATHER_SANDSTORM', 'B_WEATHER_HAIL', 'B_WEATHER_SNOW', 'B_WEATHER_FOG',
        'B_WEATHER_STRONG_WINDS']),
    'field': ('constants/battle.h', ['STATUS_FIELD_MAGIC_ROOM', 'STATUS_FIELD_TRICK_ROOM',
        'STATUS_FIELD_WONDER_ROOM', 'STATUS_FIELD_MUDSPORT', 'STATUS_FIELD_WATERSPORT',
        'STATUS_FIELD_GRAVITY', 'STATUS_FIELD_ION_DELUGE', 'STATUS_FIELD_FAIRY_LOCK']),
    'status': ('constants/battle.h', ['STATUS1_SLEEP', 'STATUS1_POISON', 'STATUS1_BURN',
        'STATUS1_FREEZE', 'STATUS1_PARALYSIS', 'STATUS1_TOXIC_POISON', 'STATUS1_FROSTBITE']),
    'side': ('constants/battle.h', ['SIDE_STATUS_REFLECT', 'SIDE_STATUS_LIGHTSCREEN',
        'SIDE_STATUS_SAFEGUARD', 'SIDE_STATUS_MIST', 'SIDE_STATUS_TAILWIND',
        'SIDE_STATUS_AURORA_VEIL', 'SIDE_STATUS_LUCKY_CHANT',
        'SIDE_STATUS_DAMAGE_NON_TYPES', 'SIDE_STATUS_RAINBOW',
        'SIDE_STATUS_SEA_OF_FIRE', 'SIDE_STATUS_SWAMP']),
    'battletype': ('constants/battle.h', ['BATTLE_TYPE_DOUBLE', 'BATTLE_TYPE_TRAINER',
        'BATTLE_TYPE_TWO_OPPONENTS', 'BATTLE_TYPE_MULTI', 'BATTLE_TYPE_INGAME_PARTNER']),
    'limitation': ('constants/battle_util.h', ['MOVE_LIMITATION_ZEROMOVE', 'MOVE_LIMITATION_PP',
        'MOVE_LIMITATION_DISABLED', 'MOVE_LIMITATION_TORMENTED', 'MOVE_LIMITATION_TAUNT',
        'MOVE_LIMITATION_IMPRISON', 'MOVE_LIMITATION_ENCORE', 'MOVE_LIMITATION_CHOICE_ITEM',
        'MOVE_LIMITATION_ASSAULT_VEST', 'MOVE_LIMITATION_GRAVITY', 'MOVE_LIMITATION_HEAL_BLOCK',
        'MOVE_LIMITATION_BELCH', 'MOVE_LIMITATION_THROAT_CHOP', 'MOVE_LIMITATION_STUFF_CHEEKS',
        'MOVE_LIMITATION_CANT_USE_TWICE', 'MOVE_LIMITATION_UNUSABLE']),
}

ENUM_FILES = {
    'species': ('constants/species.h', 'SPECIES_'),
    'move': ('constants/moves.h', 'MOVE_'),
    'ability': ('constants/abilities.h', 'ABILITY_'),
    'item': ('constants/items.h', 'ITEM_'),
    'type': ('constants/pokemon.h', 'TYPE_'),
}


def fail(message):
    raise SystemExit(f'battle_driver: {message}')


# ------------------------------------------------------------------- constants

def parse_enum(path, prefix):
    """Name<->value for a packed engine enum; the first real spelling of a value wins.

    Count sentinels (MOVES_COUNT_GEN2 and friends) share a value with a real
    member, so they are resolvable but never become a display name."""
    names, values, counter = {}, {}, 0
    for line in path.read_text().splitlines():
        line = line.strip()
        if line.startswith('#') or line.startswith('//'):
            continue
        match = re.match(r'([A-Z][A-Z0-9_]*)\s*(?:=\s*([^,]+?))?\s*,', line)
        if not match:
            continue
        name, expression = match[1], match[2]
        if expression is None:
            value = counter
        else:
            expression = expression.split('//')[0].strip()
            if re.fullmatch(r'\d+', expression):
                value = int(expression)
            elif re.fullmatch(r'0[xX][0-9a-fA-F]+', expression):
                value = int(expression, 16)
            elif expression in values:
                value = values[expression]
            else:
                continue
        counter = value + 1
        values[name] = value
        if name.startswith(prefix) and 'COUNT' not in name:
            names.setdefault(value, name)
    return names, values


def resolve_defines(names_by_header):
    """Resolve preprocessor constants by compiling them, exactly as the party
    preparation helper resolves species/move constants."""
    headers = sorted({header for header, _ in names_by_header})
    wanted = [name for _, name in names_by_header]
    # constants/battle.h names the Type enum, so pull it in first.
    headers = ['constants/pokemon.h'] + [h for h in headers if h != 'constants/pokemon.h']
    source = '#include <stdio.h>\n#define TRUE 1\n#define FALSE 0\n'
    source += ''.join(f'#include "{header}"\n' for header in headers)
    source += 'int main(void) {\n'
    source += ''.join(f'printf("{name} %u\\n", (unsigned)({name}));\n' for name in wanted)
    source += 'return 0; }\n'
    with tempfile.TemporaryDirectory(prefix='ec-battle-') as tmp:
        exe = str(Path(tmp) / 'constants')
        subprocess.run([shutil.which('cc'), '-Iinclude', '-x', 'c', '-', '-o', exe],
                       input=source, cwd=ROOT, text=True, check=True, capture_output=True)
        out = subprocess.run([exe], text=True, check=True, capture_output=True).stdout
    return {name: int(value) for name, value in (line.split() for line in out.splitlines())}


def build_constants():
    tables = {}
    for key, (relative, prefix) in ENUM_FILES.items():
        names, values = parse_enum(ROOT / 'include' / relative, prefix)
        tables[key] = {'names': {str(k): v for k, v in names.items()}, 'values': values}
    requests = []
    for group, (header, names) in DEFINE_GROUPS.items():
        requests += [(header, name) for name in names]
    requests += [('constants/opponents.h', 'TRAINERS_COUNT'),
                 ('constants/battle_partner.h', 'PARTNER_COUNT')]
    resolved = resolve_defines(requests)
    for group, (_, names) in DEFINE_GROUPS.items():
        tables[group] = {name: resolved[name] for name in names}
    tables['limits'] = {'trainers': resolved['TRAINERS_COUNT'], 'partners': resolved['PARTNER_COUNT']}
    tables['trainers'] = parse_trainer_ids()
    tables['charmap'] = parse_charmap()
    return tables


def parse_trainer_ids():
    """TRAINER_* identifiers, including the campaign's aliases."""
    text = (ROOT / 'include/constants/opponents.h').read_text()
    raw = dict(re.findall(r'^#define\s+(TRAINER_[A-Z0-9_]+)\s+(\S+)\s*$', text, re.M))
    resolved = {}

    def value_of(name, seen=()):
        if name in resolved:
            return resolved[name]
        if name in seen or name not in raw:
            return None
        expression = raw[name]
        if re.fullmatch(r'\d+', expression):
            resolved[name] = int(expression)
        elif re.fullmatch(r'0[xX][0-9a-fA-F]+', expression):
            resolved[name] = int(expression, 16)
        else:
            inner = value_of(expression, seen + (name,))
            if inner is None:
                return None
            resolved[name] = inner
        return resolved[name]

    for name in raw:
        value_of(name)
    return resolved


def parse_charmap():
    chars = {}
    for line in (ROOT / 'charmap.txt').read_text().splitlines():
        match = re.match(r"^'(.*)'\s*=\s*([A-Fa-f0-9]{2})\s*(?:@.*)?$", line.strip())
        if match:
            text = match[1].replace(r"\'", "'").replace(r'\"', '"')
            if not text.startswith('\\'):
                chars.setdefault(int(match[2], 16), text)
    return {str(k): v for k, v in chars.items()}


def decode_text(raw, charmap):
    out, i = [], 0
    while i < len(raw):
        value = raw[i]
        i += 1
        if value == 0xFF:
            break
        if value in (0xFA, 0xFB, 0xFE):
            out.append('\n')
            continue
        if value in (0xFC, 0xFD):
            i += 1
            continue
        out.append(charmap.get(str(value), '?'))
    return ''.join(out).strip()


def flags_of(value, table):
    return [name for name, bit in table.items() if bit and (value & bit) == bit]


# ---------------------------------------------------------------- session I/O

class Session:
    def __init__(self, run_dir):
        self.dir = Path(run_dir).resolve()
        self.meta_path = self.dir / 'session.json'
        self.rom = self.dir / 'scene.gba'
        self.elf = self.dir / 'scene.elf'
        self.state_file = self.dir / 'current.ss1'
        self.events = self.dir / 'events.jsonl'
        self.meta = json.loads(self.meta_path.read_text()) if self.meta_path.exists() else {}
        self.constants = None
        self._syms = None

    @property
    def syms(self):
        if self._syms is None:
            self._syms = native_tools.symbols(self.elf, ROOT)
        return self._syms

    def constants_table(self):
        if self.constants is None:
            cache = self.dir / 'constants.json'
            if cache.exists():
                self.constants = json.loads(cache.read_text())
            else:
                self.constants = build_constants()
                cache.write_text(json.dumps(self.constants) + '\n')
        return self.constants

    def check_artifacts(self):
        for path, key in ((self.rom, 'rom_sha256'), (self.elf, 'elf_sha256')):
            if hashlib.sha256(path.read_bytes()).hexdigest() != self.meta[key]:
                fail(f'artifact mismatch, this run needs its own saved build: {path}')

    def run(self, *, frames, writes=(), reads=(), until=None, advance=True, png=None,
            advance_text=False):
        runner = ui.build_runner()
        target = self.dir / 'next.ss1' if advance else None
        command = [str(runner), '--rom', str(self.rom), '--rtc', RTC_EPOCH,
                   '--frames', str(frames), '--state-in', str(self.state_file)]
        if target is not None:
            command += ['--state-out', str(target)]
        if png is not None:
            command += ['--screenshot', str(png)]
        for address, value in writes:
            command += ['--write', f'0:4:0x{address:x}:{value & 0xffffffff}']
        for address in reads:
            command += ['--read', f'4:0x{address:x}']
        if advance_text:
            # Mechanical text advance only. Every decision is served from the
            # mailbox; these pulses never select an action, target or party slot.
            for frame in range(0, min(frames, TEXT_PULSE_SPAN), TEXT_PULSE_EVERY):
                command += ['--key', f'{frame}:1:A']
        if until is not None:
            address, mask, value = until
            command += ['--until', f'4:0x{address:x}:0x{mask:x}:0x{value:x}']
        result = ui.run(command, timeout=900)
        if advance:
            target.replace(self.state_file)
        values = {int(a, 16): int(v, 16) for a, v in re.findall(
            r'READ width=\d+ address=([0-9a-f]+) value=([0-9a-f]+)', result.stdout)}
        frames_run = int(re.search(r'RESULT frames=(\d+)', result.stdout).group(1))
        stopped = 'stop_matched=1' in result.stdout
        return values, frames_run, stopped

    def read_view(self, *, advance=False, frames=1, writes=(), until=None, png=None,
                  advance_text=False):
        base = self.syms['gEcAgentBattleView']
        addresses = [base + 4 * i for i in range(VIEW_WORDS)]
        values, frames_run, stopped = self.run(frames=frames, writes=writes, reads=addresses,
                                               until=until, advance=advance, png=png,
                                               advance_text=advance_text)
        return [values[address] for address in addresses], frames_run, stopped

    def log(self, record):
        with self.events.open('a') as handle:
            handle.write(json.dumps(record) + '\n')


# ------------------------------------------------------------------- the state

def name_of(table, value, prefix):
    return table['names'].get(str(value)) or f'{prefix}{value}'


def decode_state(session, words):
    c = session.constants_table()
    species_t, move_t, ability_t, item_t, type_t = (c['species'], c['move'], c['ability'],
                                                    c['item'], c['type'])
    phase = PHASES[words[1]] if words[1] < len(PHASES) else str(words[1])
    battlers_count = words[13]
    battle_flags = words[4]

    def battler(index):
        base = BATTLER_BASE + index * BATTLER_SIZE
        flags = words[base + 8]
        gimmick = words[base + 25]
        last = words[base + 26]
        return {
            'battler': index,
            'side': 'player' if flags & 4 else 'opponent',
            'agent_controlled': bool(flags & 8),
            'alive': bool(flags & 1),
            'absent': bool(flags & 2),
            'species': name_of(species_t, words[base + 0], 'SPECIES_'),
            'level': words[base + 1],
            'hp': words[base + 2],
            'max_hp': words[base + 3],
            'status': flags_of(words[base + 4], c['status']),
            'item': name_of(item_t, words[base + 5], 'ITEM_'),
            'ability': name_of(ability_t, words[base + 6], 'ABILITY_'),
            'party_slot': words[base + 7],
            'stat_stages': {STAT_NAMES[i]: words[base + 9 + i] - 6 for i in range(8)},
            'types': [name_of(type_t, (words[base + 27] >> (8 * i)) & 0xFF, 'TYPE_')
                      for i in range(3)],
            'moves': [{'index': i,
                       'move': name_of(move_t, words[base + 17 + i], 'MOVE_'),
                       'pp': words[base + 21 + i]}
                      for i in range(4) if words[base + 17 + i]],
            'active_gimmick': GIMMICKS[gimmick & 0xFF] if (gimmick & 0xFF) < len(GIMMICKS) else '?',
            'usable_gimmick': (GIMMICKS[(gimmick >> 8) & 0xFF]
                               if ((gimmick >> 8) & 0xFF) < len(GIMMICKS) else '?'),
            'mega_already_used': bool(gimmick & 0x10000),
            # This turn's choice so far; "previous_turn" is the authoritative
            # record of the turn that has actually resolved.
            'choosing': {'action': ACTIONS.get(last & 0xFF, last & 0xFF),
                         'move_index': (last >> 8) & 0xFF,
                         'target': (last >> 16) & 0xFF},
        }

    def party(slot):
        base = PARTY_BASE + slot * PARTY_SIZE_W
        if not words[base]:
            return None
        pp = words[base + 11]
        return {
            'slot': slot,
            'species': name_of(species_t, words[base + 0], 'SPECIES_'),
            'level': words[base + 1],
            'hp': words[base + 2],
            'max_hp': words[base + 3],
            'status': flags_of(words[base + 4], c['status']),
            'item': name_of(item_t, words[base + 5], 'ITEM_'),
            'ability': name_of(ability_t, words[base + 6], 'ABILITY_'),
            'moves': [{'index': i, 'move': name_of(move_t, words[base + 7 + i], 'MOVE_'),
                       'pp': (pp >> (8 * i)) & 0xFF}
                      for i in range(4) if words[base + 7 + i]],
        }

    def foes():
        out = []
        for index in range(12):
            base = FOE_BASE + index * FOE_SIZE
            flags = words[base + 2]
            if not flags & 1:
                continue
            owner = 'A' if index < 6 else 'B'
            entry = {'owner': owner, 'slot': index % 6, 'revealed': bool(flags & 2),
                     'fainted': bool(flags & 4)}
            if flags & 2:
                entry['species'] = name_of(species_t, words[base + 0], 'SPECIES_')
                entry['level'] = words[base + 1]
            out.append(entry)
        return out

    actives = [battler(i) for i in range(min(battlers_count, 4))]
    need_mask = words[8]

    def decision(index):
        base = LEGAL_BASE + index * LEGAL_SIZE
        limits = words[base]
        switch = words[base + 5]
        moves = []
        for i in range(4 if actives[index]['alive'] else 0):
            mask = words[base + 1 + i]
            if not actives[index]['moves'] or i >= len(actives[index]['moves']):
                pass
            move_name = name_of(move_t, words[BATTLER_BASE + index * BATTLER_SIZE + 17 + i],
                                'MOVE_')
            if move_name == 'MOVE_NONE':
                continue
            target_type = (mask >> 8) & 0xFF
            moves.append({
                'index': i, 'move': move_name,
                'legal': not (limits >> i) & 1,
                'blocked_by': flags_of(limits, c['limitation']) if (limits >> i) & 1 else [],
                'target_type': (MOVE_TARGETS[target_type] if target_type < len(MOVE_TARGETS)
                                else str(target_type)),
                'targets': sorted({t for t in range(4) if (mask >> t) & 1} | {index}),
            })
        return {
            'battler': index,
            'species': actives[index]['species'],
            'may_switch': bool(switch & 0x100),
            'switch_slots': [s for s in range(6) if (switch >> s) & 1],
            'moves': moves,
        }

    selection = [(words[31] >> (8 * i)) & 0xFF for i in range(4)]
    pending = []
    if phase in ('await_action', 'await_switch'):
        for index in range(min(battlers_count, 4)):
            if not actives[index]['agent_controlled']:
                continue
            if (need_mask >> index) & 1 or (phase == 'await_action' and actives[index]['alive']
                                            and selection[index] < ACTION_CONFIRMED):
                pending.append(decision(index))

    return {
        'phase': phase,
        'awaiting_now': [i for i in range(4) if (need_mask >> i) & 1],
        'turn': words[3],
        'battle_type': flags_of(battle_flags, c['battletype']),
        'weather': flags_of(words[5], c['weather']) or ['none'],
        'field': flags_of(words[6], c['field']) or ['none'],
        'terrain': TERRAINS[words[29]] if words[29] < len(TERRAINS) else str(words[29]),
        'terrain_turns': words[30],
        'sides': {'player': flags_of(words[17], c['side']),
                  'opponent': flags_of(words[18], c['side'])},
        'outcome': OUTCOMES.get(words[7], words[7]),
        'player_faints': words[14],
        'opponent_faints': words[15],
        'level_cap': words[22],
        'difficulty': words[23],
        'ai_decision_frames': words[9],
        'ai_setup_frames': words[10],
        'ai_delay_frames': words[11],
        'actives': actives,
        'player_reserves': [p for p in (party(s) for s in range(6)) if p],
        'opponent_party': foes(),
        'pending_decision': pending,
        'message_serial': words[16],
        'selection_state': selection,
        'previous_turn': [
            {'battler': i,
             'action': ACTIONS.get(words[PREV_BASE + i * PREV_SIZE], words[PREV_BASE + i * PREV_SIZE]),
             'move_index': words[PREV_BASE + i * PREV_SIZE + 1],
             'target': words[PREV_BASE + i * PREV_SIZE + 2],
             'move': name_of(move_t, words[PREV_BASE + i * PREV_SIZE + 3], 'MOVE_')}
            for i in range(min(battlers_count, 4))],
    }


def decode_messages(session, words, since):
    charmap = session.constants_table()['charmap']
    total = words[16]
    out = []
    for serial in range(max(since, total - MSG_COUNT), total):
        base = MSG_BASE + (serial % MSG_COUNT) * MSG_SIZE
        header = words[base]
        if (header >> 8) != serial:
            continue
        length = min(header & 0xFF, MSG_CHARS)
        raw = bytearray()
        for i in range(MSG_SIZE - 1):
            word = words[base + 1 + i]
            raw += bytes([word & 0xFF, (word >> 8) & 0xFF, (word >> 16) & 0xFF, (word >> 24) & 0xFF])
        text = decode_text(raw[:length], charmap)
        if text:
            out.append(text)
    return out


# ---------------------------------------------------------------- subcommands

def scenario_index(name):
    header = (ROOT / 'include/emerald_champions_headless.h').read_text()
    block = header.split('enum EmeraldChampionsHeadlessScenario')[1].split('};')[0]
    return re.findall(r'EC_HEADLESS_SCENARIO_\w+', block).index('EC_HEADLESS_SCENARIO_' + name)


MULTI_RE = re.compile(
    r'^\s*multi_2_vs_2\s+(TRAINER_\w+)\s*,[^,]+,\s*(TRAINER_\w+)\s*,[^,]+,\s*(PARTNER_\w+)', re.M)
TWO_RE = re.compile(
    r'^\s*trainerbattle_double_two_trainers\s+(TRAINER_\w+)\s*,[^,]+,\s*(TRAINER_\w+)', re.M)


def script_pairings():
    """How the map scripts actually start the authored two-owner encounters."""
    pairs = {}
    for path in sorted((ROOT / 'data/maps').glob('*/scripts.inc')):
        text = path.read_text()
        for a, b, partner in MULTI_RE.findall(text):
            for key in (a, b):
                pairs[key] = {'a': a, 'b': b, 'partner': partner, 'script': str(path)}
        for a, b in TWO_RE.findall(text):
            for key in (a, b):
                pairs[key] = {'a': a, 'b': b, 'partner': 'PARTNER_NONE', 'script': str(path)}
    return pairs


def advance_to_halt(session, writes, png=None):
    """Run in bounded chunks until the ROM parks at the next decision or the end.

    The runner caps key events per invocation, so a long faint/exit sequence is
    continued across chunks rather than by inflating one frame budget."""
    syms = session.syms
    total = 0
    for chunk in range(MAX_CHUNKS):
        view, frames_run, stopped = session.read_view(
            advance=True, frames=TURN_FRAMES, writes=writes if chunk == 0 else (),
            until=(syms['gEcAgentBattleHalted'], 1, 1), advance_text=True,
            png=png if chunk == 0 else None)
        total += frames_run
        if stopped:
            return view, total, True
    return view, total, False


def command_start(args):
    session = Session(args.run_dir)
    if session.meta_path.exists():
        fail('use a fresh --run-dir for each battle')
    session.dir.mkdir(parents=True, exist_ok=True)
    # A concurrent session may rebuild the root ROM at any moment, so a stamped
    # snapshot directory can stand in for it.
    build = Path(args.build_dir).resolve() if args.build_dir else ROOT
    rom, elf = build / 'pokeemerald-headless.gba', build / 'pokeemerald-headless.elf'
    verify_rom_elf_pair(rom, elf)
    stamp = build / 'pokeemerald-headless.inputs.json'
    if not stamp.exists():
        fail('stamp pokeemerald-headless.inputs.json before driving a battle')
    evidence = json.loads(stamp.read_text())['artifacts']
    rom_hash = hashlib.sha256(rom.read_bytes()).hexdigest()
    elf_hash = hashlib.sha256(elf.read_bytes()).hexdigest()
    if evidence.get('pokeemerald-headless.gba') != rom_hash \
       or evidence.get('pokeemerald-headless.elf') != elf_hash:
        fail('the headless ROM/ELF do not match their input stamp; rebuild and restamp')
    shutil.copy2(rom, session.rom)
    shutil.copy2(elf, session.elf)
    shutil.copy2(stamp, session.dir / 'inputs.json')

    constants = build_constants()
    (session.dir / 'constants.json').write_text(json.dumps(constants) + '\n')
    session.constants = constants
    trainers = constants['trainers']
    pairings = script_pairings()

    name_a = args.trainer
    if name_a not in trainers:
        fail(f'unknown trainer identifier: {name_a}')
    pairing = pairings.get(name_a)
    name_b, partner_name = args.trainer2, args.partner
    if pairing and name_b is None and partner_name is None:
        name_a, name_b, partner_name = pairing['a'], pairing['b'], pairing['partner']
    partner_name = partner_name or 'PARTNER_NONE'
    if name_b is not None and name_b not in trainers:
        fail(f'unknown trainer identifier: {name_b}')
    partners = resolve_defines([('constants/battle_partner.h', partner_name)])
    difficulty = {'easy': 0, 'medium': 1, 'normal': 1, 'hard': 2}[args.difficulty]

    session.meta = {
        'rom_sha256': rom_hash, 'elf_sha256': elf_hash, 'seed': args.seed,
        'trainer_a': name_a, 'trainer_b': name_b, 'partner': partner_name,
        'difficulty': args.difficulty, 'level_cap': args.cap,
        'party_manifest': str(Path(args.party).resolve()),
        'party_sha256': hashlib.sha256(Path(args.party).read_bytes()).hexdigest(),
        'script_pairing': pairing,
        'scope': ('Synthetic headless benchmark: native debug trainer lifecycle, native AI, '
                  'user-authorized stage-legal preparation. Not earned campaign play.'),
    }
    session.meta_path.write_text(json.dumps(session.meta, indent=2) + '\n')
    session.check_artifacts()
    syms = session.syms

    # 1. Clean boot into the agent-battle scenario at the requested cap/difficulty.
    runner = ui.build_runner()
    param = (args.cap & 0xFF) | ((difficulty & 0xFF) << 8)
    boot = [str(runner), '--rom', str(session.rom), '--rtc', RTC_EPOCH,
            '--frames', str(BOOT_FRAMES), '--state-out', str(session.state_file),
            '--write', f'59:4:0x{syms["gEcAgentBattleSeed"]:x}:{args.seed & 0xffffffff}',
            '--write', f'59:4:0x{syms["gEcHeadlessFixtureParam"]:x}:{param}',
            '--write', f'60:4:0x{syms["gEcHeadlessFixtureScenario"]:x}:'
                       f'{scenario_index("AGENT_BATTLE")}',
            '--until', f'4:0x{syms["gEcStudioState"]:x}:0x1:0x1']
    output = ui.run(boot, timeout=900)
    if 'stop_matched=1' not in output.stdout:
        fail('the headless boot never reached an unlocked field state\n' + output.stdout[-2000:])

    # 2. The existing native preparation API owns party legality.
    spec, words = prepare_protocol(Path(args.party), ROOT)
    writes = [(syms[name] + offset, value) for name, offset, value in words]
    writes.append((syms['gEcAgentPrepCommand'], 1))
    reads = [syms['gEcAgentPrepResult'], syms['gEcAgentPrepErrorSlot']]
    values, _, _ = session.run(frames=PREP_FRAMES, writes=writes, reads=reads)
    if values[syms['gEcAgentPrepResult']] != 1:
        fail(f'native preparation rejected the manifest: result='
             f'{values[syms["gEcAgentPrepResult"]]} slot={values[syms["gEcAgentPrepErrorSlot"]]}')
    session.meta['prepared'] = {'encounter': spec['encounter'], 'party': len(spec['party'])}

    # 3. Start the battle through the native debug lifecycle.
    battle_writes = [
        (syms['gEcAgentBattleTrainerA'], trainers[name_a]),
        (syms['gEcAgentBattleTrainerB'], trainers[name_b] if name_b else 0),
        (syms['gEcAgentBattlePartner'], partners[partner_name]),
        (syms['gEcAgentBattleHalted'], 0),
        (syms['gEcAgentBattleResult'], 0),
        (syms['gEcAgentBattleCommand'], 1),
    ]
    view, frames_run, stopped = advance_to_halt(
        session, battle_writes, png=(session.dir / 'start.png') if args.png else None)
    if not stopped:
        fail(f'no decision point was reached in {frames_run} frames; the battle never halted')
    state = decode_state(session, view)
    session.meta['started'] = {'frames': frames_run}
    session.meta_path.write_text(json.dumps(session.meta, indent=2) + '\n')
    session.log({'event': 'start', 'trainer_a': name_a, 'trainer_b': name_b,
                 'partner': partner_name, 'seed': args.seed, 'frames': frames_run,
                 'messages': decode_messages(session, view, 0), 'state': state})
    print(json.dumps(state, indent=2))


def command_state(args):
    session = Session(args.run_dir)
    session.check_artifacts()
    view, _, _ = session.read_view(advance=False,
                                   png=(session.dir / 'state.png') if args.png else None)
    state = decode_state(session, view)
    session.log({'event': 'state', 'state': state})
    print(json.dumps(state, indent=2))


COMMAND_RE = re.compile(r'^(\d+)\s*:\s*(?:move(\d+)@(\d+)(,mega)?|switch(\d+))$')


def command_act(args):
    session = Session(args.run_dir)
    session.check_artifacts()
    syms = session.syms
    before, _, _ = session.read_view(advance=False)
    state = decode_state(session, before)
    if state['phase'] not in ('await_action', 'await_switch'):
        fail(f'not at a decision point (phase={state["phase"]})')

    pending = {entry['battler']: entry for entry in state['pending_decision']}
    writes = []
    submitted = {}
    for text in args.commands:
        match = COMMAND_RE.match(text.strip())
        if not match:
            fail(f'unparsable command: {text!r} (use "0:move1@3" or "0:move0@1,mega" or "2:switch3")')
        battler = int(match[1])
        if battler not in pending:
            fail(f'battler {battler} does not need a command here; pending={sorted(pending)}')
        entry = pending[battler]
        if match[5] is not None:
            slot = int(match[5])
            if slot not in entry['switch_slots']:
                fail(f'battler {battler} cannot switch to slot {slot}; '
                     f'legal={entry["switch_slots"]}')
            if state['phase'] == 'await_action' and not entry['may_switch']:
                fail(f'battler {battler} is trapped and cannot switch this turn')
            writes += [(syms['gEcAgentBattleSwitchSlot'] + 4 * battler, slot),
                       (syms['gEcAgentBattleAction'] + 4 * battler, 2)]
            submitted[battler] = {'action': 'switch', 'slot': slot}
        else:
            index, target, mega = int(match[2]), int(match[3]), match[4] is not None
            if state['phase'] == 'await_switch':
                fail(f'battler {battler} must be replaced with a switch, not a move')
            option = next((m for m in entry['moves'] if m['index'] == index), None)
            if option is None:
                fail(f'battler {battler} has no move at index {index}')
            if not option['legal']:
                fail(f'battler {battler} move{index} ({option["move"]}) is blocked: '
                     f'{option["blocked_by"]}')
            if target not in option['targets']:
                fail(f'battler {battler} move{index} ({option["move"]}) cannot target '
                     f'{target}; legal={option["targets"]}')
            active = state['actives'][battler]
            if mega and (active['usable_gimmick'] != 'mega' or active['mega_already_used']):
                fail(f'battler {battler} cannot Mega Evolve here')
            writes += [(syms['gEcAgentBattleMoveIndex'] + 4 * battler, index),
                       (syms['gEcAgentBattleTarget'] + 4 * battler, target),
                       (syms['gEcAgentBattleMega'] + 4 * battler, 1 if mega else 0),
                       (syms['gEcAgentBattleAction'] + 4 * battler, 1)]
            submitted[battler] = {'action': 'move', 'index': index, 'move': option['move'],
                                  'target': target, 'mega': mega}
    missing = sorted(set(pending) - set(submitted))
    if missing:
        fail(f'these battlers still need a command: {missing}')
    writes.append((syms['gEcAgentBattleHalted'], 0))

    view, frames_run, stopped = advance_to_halt(
        session, writes,
        png=(session.dir / f'turn-{state["turn"]:03d}.png') if args.png else None)
    after = decode_state(session, view)

    # A forced replacement, and the action that ends the battle, do not advance
    # the turn counter, so the native previous-turn latch still holds the last
    # completed turn. Only the commands this call actually submitted are
    # authoritative here; the native record is added only when a turn resolved,
    # and only for battlers this call did not command and that were alive when
    # it started (a battler KOed earlier in the same turn keeps a stale latch).
    turn_advanced = after['turn'] != state['turn']
    chosen = [dict(submitted[battler], battler=battler, source='submitted',
                   species=state['actives'][battler]['species'])
              for battler in submitted]
    if turn_advanced:
        for record in after['previous_turn']:
            battler = record['battler']
            if battler in submitted or record['action'] == 'none':
                continue
            if battler >= len(state['actives']) or not state['actives'][battler]['alive']:
                continue
            chosen.append(dict(record, source='native',
                               species=state['actives'][battler]['species']))
    chosen.sort(key=lambda entry: entry['battler'])

    events = {
        'event': 'act',
        'turn_before': state['turn'],
        'turn_after': after['turn'],
        'turn_advanced': turn_advanced,
        'submitted': submitted,
        'frames': frames_run,
        'halted': stopped,
        'ai_decision_frames': after['ai_decision_frames'],
        'ai_setup_frames': after['ai_setup_frames'],
        'ai_delay_frames': after['ai_delay_frames'],
        'chosen': chosen,
        'hp_before': {b['battler']: [b['hp'], b['max_hp']] for b in state['actives']},
        'hp_after': {b['battler']: [b['hp'], b['max_hp']] for b in after['actives']},
        'damage': {b['battler']: state['actives'][b['battler']]['hp'] - b['hp']
                   for b in after['actives'] if b['battler'] < len(state['actives'])},
        'faints': [b['battler'] for b in after['actives']
                   if not b['alive'] and state['actives'][b['battler']]['alive']],
        'weather': after['weather'],
        'field': after['field'],
        'messages': decode_messages(session, view, state['message_serial']),
        'player_faints': after['player_faints'],
        'opponent_faints': after['opponent_faints'],
        'outcome': after['outcome'],
        'phase': after['phase'],
    }
    session.log(events)
    print(json.dumps(events, indent=2))
    if not stopped and after['phase'] != 'ended':
        print(f'battle_driver: warning: no new decision point within {frames_run} frames; '
              'run "state" or "act" again to continue', file=sys.stderr)


def command_result(args):
    session = Session(args.run_dir)
    session.check_artifacts()
    view, _, _ = session.read_view(advance=False)
    state = decode_state(session, view)
    turns, decisions, ai_frames = 0, 0, []
    for line in session.events.read_text().splitlines():
        record = json.loads(line)
        if record.get('event') != 'act':
            continue
        decisions += 1
        turns = max(turns, record['turn_after'])
        if record['ai_decision_frames']:
            ai_frames.append(record['ai_decision_frames'])
    result = {
        'trainer_a': session.meta['trainer_a'],
        'trainer_b': session.meta['trainer_b'],
        'partner': session.meta['partner'],
        'seed': session.meta['seed'],
        'difficulty': session.meta['difficulty'],
        'level_cap': state['level_cap'],
        'phase': state['phase'],
        'outcome': state['outcome'],
        'turns': turns,
        'decisions': decisions,
        'player_faints': state['player_faints'],
        'opponent_faints': state['opponent_faints'],
        'ai_decision_frames_max': max(ai_frames) if ai_frames else 0,
        'ai_decision_seconds_max': round(max(ai_frames) / 59.7275, 3) if ai_frames else 0.0,
        'ai_decision_frames': ai_frames,
        'rom_sha256': session.meta['rom_sha256'],
        'elf_sha256': session.meta['elf_sha256'],
        'scope': session.meta['scope'],
    }
    session.log({'event': 'result', 'result': result})
    (session.dir / 'result.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    subparsers = parser.add_subparsers(dest='command', required=True)

    start = subparsers.add_parser('start')
    start.add_argument('--trainer', required=True)
    start.add_argument('--trainer2', help='second owner; normally taken from the map script')
    start.add_argument('--partner', help='PARTNER_* for a multi; normally from the map script')
    start.add_argument('--party', required=True, help='handoff/benchmarks/*.json manifest')
    start.add_argument('--seed', type=lambda v: int(v, 0), required=True)
    start.add_argument('--difficulty', default='medium',
                       choices=['easy', 'medium', 'normal', 'hard'])
    start.add_argument('--cap', type=int, required=True, help='campaign player level cap')
    start.add_argument('--run-dir', required=True)
    start.add_argument('--build-dir',
                       help='directory holding a stamped headless ROM/ELF/inputs triple')
    start.add_argument('--png', action='store_true')
    start.set_defaults(func=command_start)

    for name, func in (('state', command_state), ('result', command_result)):
        sub = subparsers.add_parser(name)
        sub.add_argument('--run-dir', required=True)
        sub.add_argument('--png', action='store_true')
        sub.set_defaults(func=func)

    act = subparsers.add_parser('act')
    act.add_argument('commands', nargs='+')
    act.add_argument('--run-dir', required=True)
    act.add_argument('--png', action='store_true')
    act.set_defaults(func=command_act)

    args = parser.parse_args()
    args.func(args)


if __name__ == '__main__':
    main()
