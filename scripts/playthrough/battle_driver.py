#!/usr/bin/env python3
"""Headless per-turn battle driver for the authored campaign trainers.

Synthetic benchmark evidence only. This starts one trainer battle in the
EC_HEADLESS_FIXTURES ROM through the native debug battle lifecycle, prepares a
user-authorized stage-legal party through the existing native preparation API,
and then answers every player decision point from a memory mailbox. No buttons,
no screenshots, no story receipts. It never earns campaign progress.

    start   boot, prepare the party, begin the battle under the trainer's map weather,
            stop at the first decision
    state   print the current semantic state (read-only; never advances)
    act     validate and submit this decision point's commands, advance, report
    result  print the final outcome

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
from prepare_party import PREP_RESULTS, protocol as prepare_protocol

RTC_EPOCH = '946684800'
BOOT_FRAMES = 2400
PREP_FRAMES = 240
BASE_LEVEL_CAP = 14  # GetCurrentLevelCap with no milestone flag set
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
CONSTANTS_SCHEMA = 3  # bump whenever build_constants() gains or renames a table
# The legacy 14-message ring; the complete event log (LOG_*) supersedes it.
MSG_BASE, MSG_SIZE, MSG_COUNT = 276, 14, 14
PREV_BASE, PREV_SIZE = 472, 4
FIELD_BASE = 488
MSG_CHARS = (MSG_SIZE - 1) * 4
# Mirrors EC_AGENT_BATTLE_LOG_* in include/emerald_champions_agent_battle.h.
LOG_HEAD_WORD, LOG_SIZE_WORD, MAP_FIELD_WORD = 492, 493, 494
LOG_TEXT, LOG_MOVE, LOG_HP, LOG_POPUP = 1, 2, 3, 4
READS_PER_CALL = 500  # the runner accepts 512 --read requests

PHASES = ['idle', 'starting', 'running', 'await_action', 'await_switch', 'ended']
ACTIONS = {0: 'use_move', 1: 'use_item', 2: 'switch', 3: 'run', 10: 'exec_script',
           13: 'nothing_fainted', 0xFF: 'none'}
GIMMICKS = ['none', 'mega', 'ultra_burst', 'z_move', 'dynamax', 'tera']
OUTCOMES = {0: 'ongoing', 1: 'won', 2: 'lost', 3: 'drew', 4: 'ran', 5: 'player_teleported',
            6: 'mon_fled', 7: 'caught', 8: 'no_safari_balls', 9: 'forfeited', 10: 'mon_teleported'}
# IsPlayerDefeated (src/battle_setup.c) whites the player out for these, so a
# double KO that empties both sides at once is a loss, exactly as in play.
PLAYER_DEFEATED = {'lost', 'drew', 'forfeited'}
TERRAINS = ['none', 'grassy', 'misty', 'electric', 'psychic']
# Bit order of AuthoredFieldMask in src/emerald_champions_agent_battle.c.
AUTHORED_FIELD = {
    0: 'electric_terrain', 1: 'electric_terrain_temporary',
    2: 'misty_terrain', 3: 'misty_terrain_temporary',
    4: 'grassy_terrain', 5: 'grassy_terrain_temporary',
    6: 'psychic_terrain', 7: 'psychic_terrain_temporary',
    8: 'trick_room', 9: 'magic_room', 10: 'wonder_room',
    16: 'sun', 17: 'sun_temporary', 18: 'rain', 19: 'rain_temporary',
    20: 'sandstorm', 21: 'hail', 22: 'snow', 23: 'fog',
}
# Mirrors enum EmeraldChampionsAgentSwitchBlock; index 0 means the switch is legal.
SWITCH_BLOCKS = ['', 'battle_arena', 'commander', 'trapped', 'ability_prevents_escape']
MOVE_TARGETS = ['none', 'selected', 'smart', 'depends', 'opponent', 'random', 'both', 'user',
                'ally', 'user_and_ally', 'user_or_ally', 'foes_and_ally', 'field',
                'opponents_field', 'all_battlers']
# Target types with a native default rather than a user-selected target.
# The target byte still matters to spread iteration.
FIXED_TARGET_MOVES = {'user', 'user_and_ally', 'ally', 'both', 'foes_and_ally', 'field',
                      'opponents_field', 'all_battlers', 'random'}
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
    # STATUS1_SLEEP and STATUS1_TOXIC_COUNTER are multi-bit counters, not flags;
    # decode_status owns them. Every other group here is single-bit.
    'status': ('constants/battle.h', ['STATUS1_SLEEP', 'STATUS1_POISON', 'STATUS1_BURN',
        'STATUS1_FREEZE', 'STATUS1_PARALYSIS', 'STATUS1_TOXIC_POISON', 'STATUS1_FROSTBITE',
        'STATUS1_TOXIC_COUNTER']),
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
# Enums that share their header with others: (header, enum name, prefix).
ENUM_BLOCKS = {
    'ow_weather': ('constants/weather.h', 'OverworldWeather', 'WEATHER_'),
    'environment': ('constants/battle.h', 'BattleEnvironments', 'BATTLE_ENVIRONMENT_'),
}


def fail(message):
    raise SystemExit(f'battle_driver: {message}')


def clone_file(source, target):
    """Copy the ROM/ELF into the run directory without spending the space.

    Every run pins its own build so a concurrent rebuild cannot invalidate it,
    but a plain copy costs ~68MB per battle and a playtest wave is hundreds of
    battles. A copy-on-write clone is instant, shares the blocks until something
    writes, and is an ordinary independent file afterwards. Falls back to a real
    copy on a filesystem that cannot clone."""
    for flag in ('--reflink=auto', '-c'):  # GNU coreutils, then macOS
        try:
            subprocess.run(['cp', flag, str(source), str(target)],
                           check=True, capture_output=True)
            return
        except (subprocess.CalledProcessError, FileNotFoundError):
            continue
    shutil.copy2(source, target)


# ------------------------------------------------------------------- constants

def parse_enum(path, prefix, text=None):
    """Name<->value for a packed engine enum; the first real spelling of a value wins.

    Count sentinels (MOVES_COUNT_GEN2 and friends) share a value with a real
    member, so they are resolvable but never become a display name."""
    names, values, counter = {}, {}, 0
    for line in (path.read_text() if text is None else text).splitlines():
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
        # Skip only the plural sentinels (MOVES_COUNT, MOVES_COUNT_GEN2), never a
        # real member whose name merely starts that way, such as MOVE_COUNTER.
        if name.startswith(prefix) and not re.search(r'_COUNT(_|$)', name):
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
    for key, (relative, enum, prefix) in ENUM_BLOCKS.items():
        text = (ROOT / 'include' / relative).read_text()
        block = text.split(f'enum {enum}')[1].split('{', 1)[1].split('};')[0]
        names, values = parse_enum(None, prefix, text=block)
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
    tables['schema'] = CONSTANTS_SCHEMA
    for group in ('weather', 'field', 'side', 'battletype', 'limitation'):
        for name, bit in tables[group].items():
            if bit & (bit - 1):
                fail(f'{name} is a multi-bit mask; flags_of cannot decode it')
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
    """Single-bit flag names only. A multi-bit mask would need every bit set."""
    return [name for name, bit in table.items() if bit and (value & bit) == bit]


def decode_status(value, table):
    """STATUS1 mixes single-bit conditions with two counters.

    Sleep lives in the low bits as turns remaining, and the toxic counter in
    bits 8-11, so decoding either as a flag drops it for every value but a full
    mask - a sleeping battler read as no status at all."""
    out = []
    for mask, label in ((table['STATUS1_SLEEP'], 'sleep'),):
        counter = value & mask
        if counter:
            out.append(f'{label}:{counter // (mask & -mask)}')
    for name in ('STATUS1_POISON', 'STATUS1_BURN', 'STATUS1_FREEZE', 'STATUS1_PARALYSIS',
                 'STATUS1_TOXIC_POISON', 'STATUS1_FROSTBITE'):
        if value & table[name]:
            out.append(name)
    toxic_mask = table['STATUS1_TOXIC_COUNTER']
    toxic = value & toxic_mask
    if toxic:
        out.append(f'toxic_counter:{toxic // (toxic_mask & -toxic_mask)}')
    return out


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
        self._sizes = None

    @property
    def syms(self):
        if self._syms is None:
            self._syms = native_tools.symbols(self.elf, ROOT)
        return self._syms

    def constants_table(self):
        if self.constants is None:
            cache = self.dir / 'constants.json'
            if cache.exists() and json.loads(cache.read_text()).get('schema') == CONSTANTS_SCHEMA:
                self.constants = json.loads(cache.read_text())
            else:
                self.constants = build_constants()
                cache.write_text(json.dumps(self.constants) + '\n')
        return self.constants

    def check_artifacts(self):
        for path, key in ((self.rom, 'rom_sha256'), (self.elf, 'elf_sha256')):
            if hashlib.sha256(path.read_bytes()).hexdigest() != self.meta[key]:
                fail(f'artifact mismatch, this run needs its own saved build: {path}')

    def symbol_sizes(self):
        """{name: (address, size)} from nm -S, for address-range containment."""
        if self._sizes is None:
            out = ui.run([shutil.which('arm-none-eabi-nm'), '-S', str(self.elf)]).stdout
            self._sizes = {}
            for line in out.splitlines():
                parts = line.split()
                if len(parts) >= 4 and re.fullmatch('[0-9a-fA-F]+', parts[0]) \
                   and re.fullmatch('[0-9a-fA-F]+', parts[1]):
                    self._sizes[parts[-1]] = (int(parts[0], 16), int(parts[1], 16))
        return self._sizes

    def crashed(self, png):
        """True if the ROM is sitting in its crash handler.

        The fixture ROM's assertf failures stop the game there, which otherwise
        looks exactly like a battle that will not reach a decision point: the
        driver spends its whole frame budget and reports no halt. The crash
        screen carries the assertion text, so capture it as evidence."""
        sizes = self.symbol_sizes()
        ranges = [sizes[name] for name in ('CrashScreen', 'HandleExceptionEntry')
                  if name in sizes]
        if not ranges:
            return False
        runner = ui.build_runner()
        command = [str(runner), '--rom', str(self.rom), '--rtc', RTC_EPOCH,
                   '--frames', '2', '--state-in', str(self.state_file),
                   '--screenshot', str(png)]
        out = ui.run(command, timeout=300).stdout
        match = re.search(r'pc=([0-9a-f]+)', out)
        if not match:
            return False
        pc = int(match.group(1), 16) & ~1
        return any(start <= pc < start + size for start, size in ranges)

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

    def run_at_frames(self, *, frames, writes):
        """Like run(), but each write carries its own frame so a command mailbox
        that is consumed once per frame can be driven several times in one go."""
        runner = ui.build_runner()
        target = self.dir / 'next.ss1'
        command = [str(runner), '--rom', str(self.rom), '--rtc', RTC_EPOCH,
                   '--frames', str(frames), '--state-in', str(self.state_file),
                   '--state-out', str(target)]
        for frame, address, value in writes:
            command += ['--write', f'{frame}:4:0x{address:x}:{value & 0xffffffff}']
        ui.run(command, timeout=900)
        target.replace(self.state_file)

    def read_view(self, *, advance=False, frames=1, writes=(), until=None, png=None,
                  advance_text=False):
        base = self.syms['gEcAgentBattleView']
        addresses = [base + 4 * i for i in range(VIEW_WORDS)]
        halted_address = self.syms['gEcAgentBattleHalted']
        values, frames_run, stopped = self.run(frames=frames, writes=writes,
                                               reads=addresses + [halted_address],
                                               until=until, advance=advance, png=png,
                                               advance_text=advance_text)
        # A plain state read can stop mid-publication even without --until.
        # Wait for a complete decision snapshot; do not invent switch legality
        # from a partially cleared buffer or bypass the engine's switch checks.
        for attempt in range(16):
            words = [values[address] for address in addresses]
            if words[1] not in (3, 4, 5) or values[halted_address]:
                return words, frames_run, stopped
            values, extra, matched = self.run(frames=8 * (attempt + 1),
                reads=addresses + [halted_address], advance=advance,
                until=(halted_address, 1, 1))
            frames_run += extra
            stopped |= matched
        fail('native decision snapshot did not finish publication')

    def read_log(self, words, start):
        """The event-log bytes written since `start`, or None if the ROM has no
        log or the ring wrapped past `start` (then only the legacy ring is left).

        Read after the halt without advancing: a parked battle writes nothing,
        and outside a battle the bridge does not log."""
        size, end = words[LOG_SIZE_WORD], words[LOG_HEAD_WORD]
        if not size or 'gEcAgentBattleLog' not in self.syms:
            return None
        if end < start or end - start > size:
            return None
        base = self.syms['gEcAgentBattleLog']
        first = start - start % 4
        positions = list(range(first, end, 4))
        data = bytearray()
        for chunk in range(0, len(positions), READS_PER_CALL):
            addresses = [base + pos % size for pos in positions[chunk:chunk + READS_PER_CALL]]
            values, _, _ = self.run(frames=1, reads=addresses, advance=False)
            for address in addresses:
                data += values[address].to_bytes(4, 'little')
        return bytes(data[start - first:end - first])

    def log(self, record):
        with self.events.open('a') as handle:
            handle.write(json.dumps(record) + '\n')


# ------------------------------------------------------------------- the state

def name_of(table, value, prefix):
    return table['names'].get(str(value)) or f'{prefix}{value}'


def decode_state(session, words):
    if words[0] not in (0, 3):
        fail('This battle uses the obsolete target/switch adapter. Rebuild and start a fresh benchmark.')
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
        entry = {
            'battler': index,
            'side': 'player' if flags & 4 else 'opponent',
            'agent_controlled': bool(flags & 8),
            'alive': bool(flags & 1),
            'absent': bool(flags & 2),
            # As displayed: an opposing Illusion shows its disguise's species,
            # types, ability and item until the disguise breaks.
            'species': name_of(species_t, words[base + 0], 'SPECIES_'),
            'level': words[base + 1],
            'hp': words[base + 2],
            'max_hp': words[base + 3],
            'status': decode_status(words[base + 4], c['status']),
            'item': name_of(item_t, words[base + 5], 'ITEM_'),
            'ability': name_of(ability_t, words[base + 6], 'ABILITY_'),
            'party_slot': words[base + 7] & 0xFF,
            'stat_stages': {STAT_NAMES[i]: words[base + 9 + i] - 6 for i in range(8)},
            'types': [name_of(type_t, (words[base + 27] >> (8 * i)) & 0xFF, 'TYPE_')
                      for i in range(3)],
            'moves': [{'index': i,
                       'move': name_of(move_t, words[base + 17 + i], 'MOVE_'),
                       'pp': words[base + 21 + i]}
                      for i in range(4) if words[base + 17 + i]],
            # species already carries the Mega form once the engine applies it
            # (SPECIES_ALAKAZAM_MEGA and friends resolve), but state it outright
            # so "no Mega happened" is never read as a naming gap.
            'mega_evolved': (gimmick & 0xFF) == GIMMICKS.index('mega'),
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
        disguise = words[base + 7] >> 8
        if disguise:
            # Only the player's own battlers publish this: the truth plus the
            # Pokemon the opponent sees.
            entry['illusion_disguise'] = name_of(species_t, disguise, 'SPECIES_')
        return entry

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
            'status': decode_status(words[base + 4], c['status']),
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
            target_name = (MOVE_TARGETS[target_type] if target_type < len(MOVE_TARGETS)
                           else str(target_type))
            moves.append({
                'index': i, 'move': move_name,
                'legal': not (limits >> i) & 1,
                # The reason bits ride in the high half of the move's word;
                # `limits` is a slot mask and must never be decoded as reasons.
                'blocked_by': flags_of((mask >> 16) & 0xFFFF, c['limitation']),
                'target_type': target_name,
                # Match the native UI's default target for fixed-target moves.
                # Spread attacks must start at a foe, not the acting battler.
                'targets': ([(mask >> 4) & 3] if target_name in FIXED_TARGET_MOVES
                            else [t for t in range(4) if (mask >> t) & 1]),
            })
        blocker = (switch >> 16) & 0xFF
        refused = (switch >> 24) & 0xFF
        return {
            'battler': index,
            'species': actives[index]['species'],
            # Every slot is blocked (an Encored move later Disabled, an Encored
            # Fake Out past the first turn, no PP left): the engine answers
            # Fight with Struggle, so "N:struggle" is the move command.
            'must_struggle': bool(moves) and not any(m['legal'] for m in moves),
            'may_switch': bool(switch & 0x100),
            # The engine's own switch gate: Shadow Tag and friends, the trapping
            # moves and volatiles, Battle Arena and Commander. Empty means legal.
            'switch_blocked_by': ([SWITCH_BLOCKS[blocker]] if blocker else []),
            # Set when the engine refused the switch this driver last submitted.
            'switch_last_refused': SWITCH_BLOCKS[refused] if refused else None,
            'switch_slots': [s for s in range(6) if (switch >> s) & 1],
            'moves': moves,
        }

    selection = [(words[31] >> (8 * i)) & 0xFF for i in range(4)]
    pending = []
    if phase in ('await_action', 'await_switch'):
        for index in range(min(battlers_count, 4)):
            if not actives[index]['agent_controlled']:
                continue
            awaiting = bool((need_mask >> index) & 1)
            # The engine asks for faint replacements one battler at a time, but
            # the mailbox holds a command for each, so every battler that owes a
            # replacement is listed and may be commanded in the same act call.
            owes_replacement = (phase == 'await_switch' and not actives[index]['alive'])
            chooses_action = (phase == 'await_action' and actives[index]['alive']
                              and selection[index] < ACTION_CONFIRMED)
            if not (awaiting or owes_replacement or chooses_action):
                continue
            entry = decision(index)
            entry['awaiting_now'] = awaiting
            entry['replacing'] = owes_replacement or (awaiting and not actives[index]['alive'])
            if entry['replacing']:
                # The species here is the battler that fainted, not an incoming
                # one; committed is the slot its partner has already been sent.
                entry['fainted_species'] = entry.pop('species')
                entry['species'] = None
            pending.append(entry)


    native_outcome = OUTCOMES.get(words[7], words[7])
    has_log = bool(words[LOG_SIZE_WORD])
    map_field = words[MAP_FIELD_WORD]
    state = {
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
        'outcome': 'lost' if native_outcome in PLAYER_DEFEATED else native_outcome,
        'player_faints': words[14],
        'opponent_faints': words[15],
        'level_cap': words[22],
        'difficulty': words[23],
        'ai_decision_frames': words[9],
        'ai_setup_frames': words[10],
        'ai_delay_frames': words[11],
        'actives': actives,
        # Always present, so its presence says nothing about this board.
        'display_note': ('Opposing species, types, ability and item are what the battle '
                         'displays: a Pokemon under Illusion shows its disguise (and stays '
                         'unrevealed in opponent_party) until the disguise breaks. Your own '
                         'Illusion user shows the truth plus illusion_disguise.'),
        'player_reserves': [p for p in (party(s) for s in range(6)) if p],
        'opponent_party': foes(),
        'pending_decision': pending,
        'message_serial': words[16],
        'log_head': words[LOG_HEAD_WORD],
        'selection_state': selection,
        # Where the field state came from. The engine's only setup-side channel
        # is the trainer's authored startingStatus; this game has no map or Gym
        # field table, so an empty authored list means the encounter itself
        # asked for nothing and a bare field at turn 0 is faithful, not dropped.
        'field_source': {
            'authored_by_trainer_a': [name for bit, name in AUTHORED_FIELD.items()
                                      if (words[FIELD_BASE] >> bit) & 1],
            'authored_by_trainer_b': [name for bit, name in AUTHORED_FIELD.items()
                                      if (words[FIELD_BASE + 1] >> bit) & 1],
            'terrain_at_turn_0': (TERRAINS[words[FIELD_BASE + 2]]
                                  if words[FIELD_BASE + 2] < len(TERRAINS)
                                  else str(words[FIELD_BASE + 2])),
            'weather_at_turn_0': flags_of(words[FIELD_BASE + 3], c['weather']) or ['none'],
            'terrain_is_permanent': bool(words[29]) and words[30] == 0,
            # The trainer's map weather stood in for the headless room's sky
            # when the battle opened (see resolve_map_field and `start`).
            'from_map': bool(map_field & 0x10000),
            'map_weather': (name_of(c['ow_weather'], (map_field & 0xFF) - 1, 'WEATHER_')
                            if has_log and map_field & 0xFF else None),
            'environment': (name_of(c['environment'], map_field >> 24, 'BATTLE_ENVIRONMENT_')
                            if has_log else None),
        },
        'previous_turn': [
            {'battler': i,
             'action': ACTIONS.get(words[PREV_BASE + i * PREV_SIZE], words[PREV_BASE + i * PREV_SIZE]),
             'move_index': words[PREV_BASE + i * PREV_SIZE + 1],
             'target': words[PREV_BASE + i * PREV_SIZE + 2],
             'move': name_of(move_t, words[PREV_BASE + i * PREV_SIZE + 3], 'MOVE_'),
             'target_kind': target_kind(i, words[PREV_BASE + i * PREV_SIZE + 2],
                                        min(battlers_count, 4))}
            for i in range(min(battlers_count, 4))],
    }
    if native_outcome != state['outcome']:
        state['outcome_native'] = native_outcome
    return state


def decode_log(session, data):
    """(messages, moves, hp_changes, popups) from event-log bytes.

    moves, hp_changes and popups carry message_index: how many of this call's
    messages came before them, so each can be placed against the text."""
    c = session.constants_table()
    messages, moves, changes, popups = [], [], [], []
    i = 0
    while i + 2 <= len(data):
        kind, length = data[i], data[i + 1]
        payload = data[i + 2:i + 2 + length]
        i += 2 + length
        if kind == LOG_TEXT:
            text = decode_text(payload, c['charmap'])
            if text:
                messages.append(text)
        elif kind == LOG_MOVE and length >= 4:
            moves.append({'user': payload[0], 'target': payload[1],
                          'move': name_of(c['move'], payload[2] | (payload[3] << 8), 'MOVE_'),
                          'message_index': len(messages), 'damage': {}, 'healing': {},
                          'self_hp_change': 0})
        elif kind == LOG_HP and length >= 9:
            battler, attacker = payload[0], payload[1]
            before, after = payload[2] | (payload[3] << 8), payload[4] | (payload[5] << 8)
            move = name_of(c['move'], payload[6] | (payload[7] << 8), 'MOVE_')
            # The engine clears gCurrentMove between actions and at the end of
            # the turn, so a change with no current move is residual (weather,
            # status, items, hazards). One inside the latest logged move by the
            # same attacker belongs to that move.
            current = (moves[-1] if moves and moves[-1]['user'] == attacker
                       and moves[-1]['move'] == move else None)
            cause = ('residual' if move == 'MOVE_NONE'
                     else 'move' if current is not None else 'other')
            change = {'battler': battler, 'hp_before': before, 'hp_after': after,
                      'change': after - before, 'cause': cause, 'message_index': len(messages)}
            if move != 'MOVE_NONE':
                change['attacker'] = attacker
                change['move'] = move
                # Charge it to the move in progress: damage and healing dealt
                # to others, and the user's own recoil, Life Orb or drain.
                if current is not None:
                    if battler == attacker:
                        current['self_hp_change'] += after - before
                    else:
                        # Losses and gains apart: a target's Sitrus Berry
                        # firing mid-move must not shrink the move's damage.
                        key, bucket = str(battler), ('damage' if after < before else 'healing')
                        current[bucket][key] = current[bucket].get(key, 0) + abs(after - before)
            changes.append(change)
        elif kind == LOG_POPUP and length >= 4:
            # Berries, Life Orb-like items and many abilities show only a
            # pop-up, never a message.
            value = payload[2] | (payload[3] << 8)
            popups.append({'battler': payload[0], 'message_index': len(messages),
                           **({'item': name_of(c['item'], value, 'ITEM_')} if payload[1]
                              else {'ability': name_of(c['ability'], value, 'ABILITY_')})})
    return messages, moves, changes, popups


def battle_log(session, words, since_log, since_serial):
    """Every message (plus move and HP records) since log position `since_log`,
    falling back to the legacy 14-message ring (from message `since_serial`)
    on an older ROM or if more than the whole ring was written in one call."""
    data = session.read_log(words, since_log)
    if data is None:
        legacy = decode_messages(session, words, since_serial)
        return {'messages': legacy, 'moves': [], 'hp_changes': [], 'popups': [],
                'log_complete': words[16] - since_serial <= len(legacy)}
    messages, moves, changes, popups = decode_log(session, data)
    return {'messages': messages, 'moves': moves, 'hp_changes': changes, 'popups': popups,
            'log_complete': True}


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
    r'^\s*multi_2_vs_2\s+(TRAINER_\w+)\s*,[^,]+,\s*(TRAINER_\w+)\s*,[^,]+,\s*(?:PARTNER_)?(\w+)', re.M)
TWO_RE = re.compile(
    r'^\s*trainerbattle_double_two_trainers\s+(TRAINER_\w+)\s*,[^,]+,\s*(TRAINER_\w+)', re.M)


def script_pairings():
    """How the map scripts actually start the authored two-owner encounters."""
    pairs = {}
    for path in sorted((ROOT / 'data/maps').glob('*/scripts.inc')):
        text = path.read_text()
        for a, b, partner in MULTI_RE.findall(text):
            partner = 'PARTNER_' + partner  # maps may use the unprefixed aliases
            for key in (a, b):
                pairs[key] = {'a': a, 'b': b, 'partner': partner, 'script': str(path)}
        for a, b in TWO_RE.findall(text):
            for key in (a, b):
                pairs[key] = {'a': a, 'b': b, 'partner': 'PARTNER_NONE', 'script': str(path)}
    return pairs


# ------------------------------------------------------------ the map's field
#
# The game opens every trainer battle with the overworld weather the player is
# standing in (B_OVERWORLD_WEATHER_OVERRIDE is GEN_8, so it is not locked), and
# with the environment of the tile under the player. The headless room is the
# Oldale Pokemon Center, so both come from the trainer's own map instead: the
# map header, the map's ON_TRANSITION weather script evaluated at the trainer's
# tile, and the nearest weather trigger the player walks across to reach them.

BATTLE_CALL_RE = re.compile(r'^\s*(trainerbattle\w*|multi_2_vs_2)\b(.*)$')
LABEL_RE = re.compile(r'^([A-Za-z_]\w*)::?(?:\s*@.*)?$')
# What the engine's FIELD_EFFECT_OVERWORLD_WEATHER/TERRAIN cases make of each
# overworld weather (src/battle_util.c, B_OVERWORLD_FOG GEN_LATEST,
# B_OVERWORLD_SNOW GEN_LATEST, B_THUNDERSTORM_TERRAIN TRUE).
BATTLE_EFFECT_OF_WEATHER = {
    'WEATHER_RAIN': 'rain', 'WEATHER_DOWNPOUR': 'rain',
    'WEATHER_RAIN_THUNDERSTORM': 'rain + electric terrain',
    'WEATHER_SANDSTORM': 'sandstorm', 'WEATHER_DROUGHT': 'sun', 'WEATHER_SNOW': 'snow',
    'WEATHER_FOG_HORIZONTAL': 'misty terrain', 'WEATHER_FOG_DIAGONAL': 'misty terrain',
}
WEATHER_ALIASES = {
    'none': 'WEATHER_NONE', 'clear': 'WEATHER_NONE', 'rain': 'WEATHER_RAIN',
    'downpour': 'WEATHER_DOWNPOUR', 'thunderstorm': 'WEATHER_RAIN_THUNDERSTORM',
    'sun': 'WEATHER_DROUGHT', 'drought': 'WEATHER_DROUGHT', 'sand': 'WEATHER_SANDSTORM',
    'sandstorm': 'WEATHER_SANDSTORM', 'snow': 'WEATHER_SNOW', 'fog': 'WEATHER_FOG_HORIZONTAL',
}
CYCLE_TABLES = {'WEATHER_ROUTE119_CYCLE': 'sWeatherCycleRoute119',
                'WEATHER_ROUTE123_CYCLE': 'sWeatherCycleRoute123'}


def parse_script_file(path):
    """(instructions, label->index) for one map's scripts.inc, in file order,
    so a label that does not end falls through into the next one."""
    code, labels = [], {}
    if not path.exists():
        return code, labels
    for raw in path.read_text().splitlines():
        line = raw.split('@')[0].rstrip() if not raw.lstrip().startswith('.') else ''
        match = LABEL_RE.match(line.strip()) if line and not line[0].isspace() else None
        if match:
            labels[match[1]] = len(code)
            continue
        parts = line.strip().split(None, 1)
        if parts:
            args = [a.strip() for a in parts[1].split(',')] if len(parts) > 1 else []
            code.append((parts[0], args))
    return code, labels


def run_weather_script(code, labels, entry, xy=None, limit=2000):
    """Evaluate the weather a map script leaves, as the overworld would.

    A tiny interpreter for the commands the weather scripts are written in:
    getplayerxy/compare/goto_if_*/call_if_*/goto/call/return/end/setweather.
    Anything it cannot know (story vars and flags) is taken as not taken, and a
    setweather reachable only through such a branch is returned separately as
    conditional, so the caller can report it without applying it."""
    tests = {'lt': lambda a, b: a < b, 'le': lambda a, b: a <= b, 'gt': lambda a, b: a > b,
             'ge': lambda a, b: a >= b, 'eq': lambda a, b: a == b, 'ne': lambda a, b: a != b}
    variables, weather, conditional = {}, None, set()
    compared, stack, steps = None, [], 0
    pc = labels.get(entry)

    def scan(label, depth=0):
        """Weathers a skipped branch could set, without evaluating it."""
        found, i = set(), labels.get(label)
        while i is not None and i < len(code) and depth < 4:
            op, args = code[i]
            if op == 'setweather' and args:
                found.add(args[0])
            elif op in ('call', 'goto') or op.startswith(('call_if', 'goto_if')):
                if args and args[-1] in labels and args[-1] != label:
                    found |= scan(args[-1], depth + 1)
            if op in ('end', 'return', 'goto'):
                break
            i += 1
        return found

    while pc is not None and pc < len(code) and steps < limit:
        steps += 1
        op, args = code[pc]
        pc += 1
        if op == 'getplayerxy' and xy is not None and len(args) == 2:
            variables[args[0]], variables[args[1]] = xy
        elif op == 'compare' and len(args) == 2:
            value = variables.get(args[0])
            try:
                operand = int(args[1], 0)
            except ValueError:
                operand = None
            compared = (value, operand) if value is not None and operand is not None else None
        elif op.startswith(('goto_if_', 'call_if_')) and args:
            kind = op.split('_if_')[1]
            target = args[-1]
            taken = False
            if kind in tests and compared is not None:
                taken = tests[kind](*compared)
            elif kind in tests or kind in ('set', 'unset', 'defeated', 'not_defeated'):
                conditional |= scan(target)
            if taken and target in labels:
                if op.startswith('call'):
                    stack.append(pc)
                pc = labels[target]
        elif op == 'goto' and args:
            pc = labels.get(args[0])
        elif op == 'call' and args:
            if args[0] in labels:
                stack.append(pc)
                pc = labels[args[0]]
        elif op == 'return':
            pc = stack.pop() if stack else None
        elif op == 'end':
            pc = None
        elif op == 'setweather' and args:
            weather = args[0]
    return weather, conditional - ({weather} if weather else set())


def find_trainer_script(trainer):
    """(map name, enclosing label) of the map script that starts this battle."""
    token = re.compile(r'\b' + re.escape(trainer) + r'\b')
    for path in sorted((ROOT / 'data/maps').glob('*/scripts.inc')):
        label = None
        for raw in path.read_text().splitlines():
            match = LABEL_RE.match(raw.split('@')[0].strip()) if raw and not raw[0].isspace() else None
            if match:
                label = match[1]
                continue
            battle = BATTLE_CALL_RE.match(raw)
            if battle and token.search(battle[2]):
                return path.parent.name, label
    return None, None


def trainer_position(map_json, code, labels, label):
    """The tile the trainer stands on: the object event whose script reaches the
    battle label, directly, by goto/call, or by falling through into it."""
    starts = sorted((start, name) for name, start in labels.items())
    owners, cursor, owner = [], 0, None
    for index in range(len(code)):
        while cursor < len(starts) and starts[cursor][0] <= index:
            owner = starts[cursor][1]
            cursor += 1
        owners.append(owner)
    callers = {}
    for index, (op, args) in enumerate(code):
        if (op in ('goto', 'call', 'case') or op.startswith(('goto_if', 'call_if'))) \
           and args and owners[index]:
            callers.setdefault(args[-1], set()).add(owners[index])
    for start, name in starts:
        if 0 < start <= len(code) and code[start - 1][0] not in ('end', 'return', 'goto', 'releaseall_end'):
            if owners[start - 1] and owners[start - 1] != name:
                callers.setdefault(name, set()).add(owners[start - 1])
    wanted, frontier = {label}, [label]
    for _ in range(4):
        frontier = [c for name in frontier for c in callers.get(name, ()) if c not in wanted]
        wanted |= set(frontier)
    objects = [o for o in map_json.get('object_events', []) if o.get('script') in wanted]
    trainers = [o for o in objects if o.get('trainer_type', 'TRAINER_TYPE_NONE') != 'TRAINER_TYPE_NONE']
    for group in (trainers, objects):
        if group:
            return group[0]['x'], group[0]['y']
    for event in map_json.get('coord_events', []):
        if event.get('script') in wanted:
            return event['x'], event['y']
    return None


def layout_grid(map_json):
    layouts = json.loads((ROOT / 'data/layouts/layouts.json').read_text())['layouts']
    layout = next((l for l in layouts if l.get('id') == map_json['layout']), None)
    if layout is None or 'blockdata_filepath' not in layout:
        return None
    data = (ROOT / layout['blockdata_filepath']).read_bytes()
    width, height = layout['width'], layout['height']
    words = [data[i] | (data[i + 1] << 8) for i in range(0, min(len(data), 2 * width * height), 2)]
    return width, height, words


def nearest_trigger_weather(map_json, code, labels, xy):
    """The weather of the nearest weather trigger by walking distance.

    Weather regions are fenced by trigger tiles (a sunny row outside, a
    sandstorm row inside), so the last trigger crossed on the way to a trainer
    is the nearest one reachable from its tile through passable ground."""
    triggers = {}
    for event in map_json.get('coord_events', []):
        weather = None
        if event.get('type') == 'weather':
            weather = event.get('weather', '').replace('COORD_EVENT_', '')
        elif event.get('type') == 'trigger' and event.get('script') in labels:
            weather, _ = run_weather_script(code, labels, event['script'])
        if weather:
            triggers[(event['x'], event['y'])] = weather
    if not triggers or xy is None:
        return None, None
    grid = layout_grid(map_json)
    if grid is None:
        return None, None
    width, height, words = grid
    start = tuple(xy)
    seen, queue = {start: 0}, [start]
    for x, y in queue:
        if (x, y) in triggers:
            return triggers[(x, y)], seen[(x, y)]
        for nx, ny in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
            if 0 <= nx < width and 0 <= ny < height and (nx, ny) not in seen:
                block = words[ny * width + nx]
                if ((block >> 10) & 3) == 0 or (nx, ny) in triggers:
                    seen[(nx, ny)] = seen[(x, y)] + 1
                    queue.append((nx, ny))
    return None, None


def weather_cycle(name):
    text = (ROOT / 'src/field_weather_effect.c').read_text()
    block = text.split(CYCLE_TABLES[name])[1].split('{')[1].split('}')[0]
    return re.findall(r'WEATHER_\w+', block)


def anomaly_visitors(map_name):
    """Weather-anomaly visitors whose home is this map (src/weather_anomaly.c).
    Anomalies roll on player steps, so they are never applied by default."""
    text = (ROOT / 'src/data/pokemon/legendary_signs.h').read_text()
    map_json = json.loads((ROOT / 'data/maps' / map_name / 'map.json').read_text())
    home = map_json.get('id', '').removeprefix('MAP_')
    out = []
    for mon, _, flag, _, where, _, weather in re.findall(
            r'^VISITOR\((\w+),\s*(\d+),\s*(\w+),\s*(\d+),\s*(\w+),\s*(\w+),\s*(\w+)\)', text, re.M):
        if where == home:
            out.append({'species': 'SPECIES_' + mon, 'weather': 'WEATHER_' + weather, 'gate': flag})
    return out


def resolve_map_field(trainer, map_override=None):
    """Everything start needs to open the battle under the trainer's own sky."""
    map_name, label = find_trainer_script(trainer)
    source = 'battle script'
    if map_override:
        if map_name != map_override:
            label = None
        map_name, source = map_override, '--map'
    info = {'map': map_name, 'map_source': source, 'script_label': label}
    if not map_name or not (ROOT / 'data/maps' / map_name / 'map.json').exists():
        info.update(weather='WEATHER_NONE', weather_basis='no map found for this trainer',
                    environment=None)
        return info
    map_json = json.loads((ROOT / 'data/maps' / map_name / 'map.json').read_text())
    code, labels = parse_script_file(ROOT / 'data/maps' / map_name / 'scripts.inc')
    # The shared scripts (Common_EventScript_SetAbnormalWeather and friends)
    # are reachable from every map; append them after an 'end' so nothing
    # falls through into them.
    shared_code, shared_labels = parse_script_file(ROOT / 'data/event_scripts.s')
    code.append(('end', []))
    labels.update({name: start + len(code) for name, start in shared_labels.items()
                   if name not in labels})
    code += shared_code
    xy = trainer_position(map_json, code, labels, label) if label else None
    info['trainer_xy'] = list(xy) if xy else None
    header = map_json.get('weather', 'WEATHER_NONE')
    weather, basis = header, 'map header'
    conditional = set()
    transition = next((a[1] for op, a in code if op == 'map_script' and len(a) == 2
                       and a[0] == 'MAP_SCRIPT_ON_TRANSITION'), None)
    if transition:
        scripted, conditional = run_weather_script(code, labels, transition, xy)
        if scripted and (xy is not None or not any(op == 'getplayerxy' for op, _ in code)):
            weather, basis = scripted, 'ON_TRANSITION script at the trainer tile'
    triggered, distance = nearest_trigger_weather(map_json, code, labels, xy)
    if triggered:
        info['trigger_weather'] = {'weather': triggered, 'steps': distance}
        if triggered != weather:
            basis = (f'nearest weather trigger ({distance} steps); '
                     f'{basis} alone says {weather}')
        else:
            basis += f'; nearest weather trigger agrees ({distance} steps)'
        weather = triggered
    if weather in CYCLE_TABLES:
        cycle = weather_cycle(weather)
        effects = [BATTLE_EFFECT_OF_WEATHER.get(w, 'none') for w in cycle]
        modal = max(cycle, key=lambda w: (effects.count(BATTLE_EFFECT_OF_WEATHER.get(w, 'none')),
                                          -cycle.index(w)))
        info['weather_cycle'] = cycle
        basis += (f'; {weather} changes daily {cycle}, the most frequent battle weather '
                  f'({modal}) is used; pass --weather to choose another day')
        weather = modal
    elif weather == 'WEATHER_DYNAMIC':
        basis += '; WEATHER_DYNAMIC follows the daily seed and is not reproduced'
        weather = 'WEATHER_NONE'
    info['weather'] = weather
    info['weather_basis'] = basis
    if conditional and xy is None and any(op == 'getplayerxy' for op, _ in code):
        # Without the trainer's tile the map's own region test cannot run.
        info['position_dependent_weather'] = sorted(conditional)
        info['weather_basis'] += '; the trainer tile is unknown and this map sets weather by position'
    elif conditional:
        # Weather a story flag or var switches on (the Groudon/Kyogre storm's
        # WEATHER_ABNORMAL, for one). Not applied; --weather reproduces it.
        info['story_conditional_weather'] = sorted(conditional)
    visitors = anomaly_visitors(map_name)
    if visitors:
        info['possible_anomaly_weather'] = visitors
    # BattleSetup_GetEnvironmentId, without the tile metatile behaviour: Route
    # 113 and a sandstorm are sand, then the map type decides.
    map_type = map_json.get('map_type', '')
    on_water = False
    if xy:
        grid = layout_grid(map_json)
        if grid:
            width, _, words = grid
            on_water = ((words[xy[1] * width + xy[0]] >> 12) & 0xF) == 1
    if map_name == 'Route113' or weather == 'WEATHER_SANDSTORM':
        environment = 'BATTLE_ENVIRONMENT_SAND'
    elif map_type == 'MAP_TYPE_UNDERGROUND':
        environment = 'BATTLE_ENVIRONMENT_POND' if on_water else 'BATTLE_ENVIRONMENT_CAVE'
    elif map_type in ('MAP_TYPE_INDOOR', 'MAP_TYPE_SECRET_BASE'):
        environment = 'BATTLE_ENVIRONMENT_BUILDING'
    elif map_type == 'MAP_TYPE_UNDERWATER':
        environment = 'BATTLE_ENVIRONMENT_UNDERWATER'
    elif on_water:
        environment = 'BATTLE_ENVIRONMENT_WATER'
    else:
        environment = 'BATTLE_ENVIRONMENT_PLAIN'
    info['environment'] = environment
    return info


MILESTONE_RE = re.compile(r'\{\s*(FLAG_[A-Z0-9_]+)\s*,\s*(\d+)\s*(?:,\s*\d+\s*)?\}')  # {flag, cap[, stipend]}


def campaign_milestones():
    """The one cap table, read from its owner rather than copied.

    GetCurrentLevelCap walks sCampaignMilestones in src/caps.c, so a requested
    cap is reproduced by setting exactly the flags at or below it. Parsing keeps
    caps.c canonical and keeps this bridge out of a file the trainer side owns."""
    text = (ROOT / 'src/caps.c').read_text()
    block = text.split('sCampaignMilestones[] =')[1].split('};')[0]
    rows = [(name, int(cap)) for name, cap in MILESTONE_RE.findall(block) if int(cap)]
    if not rows:
        fail('could not read sCampaignMilestones from src/caps.c')
    return rows


def apply_level_cap(session, cap):
    """Set the milestone flags for `cap` through the existing Studio command."""
    syms = session.syms
    rows = [(name, value) for name, value in campaign_milestones() if value <= cap]
    known = {value for _, value in campaign_milestones()}
    if cap not in known and cap != BASE_LEVEL_CAP:
        fail(f'--cap {cap} is not a campaign milestone cap; choose one of '
             f'{sorted(known | {BASE_LEVEL_CAP})}')
    flags = resolve_defines([('constants/flags.h', name) for name, _ in rows]) if rows else {}
    writes = []
    for index, (name, _) in enumerate(rows):
        frame = index * 4
        writes.append((frame, syms['gEcStudioArgs'], flags[name]))
        writes.append((frame, syms['gEcStudioArgs'] + 4, 1))
        writes.append((frame, syms['gEcStudioCommand'], 6))
    session.run_at_frames(frames=len(rows) * 4 + 30, writes=writes)
    view, _, _ = session.read_view(advance=False)
    if view[22] != cap:
        fail(f'campaign cap did not reach {cap}; the ROM reports {view[22]}')
    return [name for name, _ in rows]

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
    png = session.dir / 'crash.png'
    if session.crashed(png):
        fail(f'the ROM stopped in its crash handler after {total} frames. This is a '
             f'native assertion failure, not a stuck battle; the assertion text is on '
             f'the captured screen: {png}')
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
    clone_file(rom, session.rom)
    clone_file(elf, session.elf)
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
    map_field = map_field_for_start(args, name_a, constants)

    session.meta = {
        'rom_sha256': rom_hash, 'elf_sha256': elf_hash, 'seed': args.seed,
        'trainer_a': name_a, 'trainer_b': name_b, 'partner': partner_name,
        'difficulty': args.difficulty, 'level_cap': args.cap,
        'party_manifest': str(Path(args.party).resolve()),
        'party_sha256': hashlib.sha256(Path(args.party).read_bytes()).hexdigest(),
        'script_pairing': pairing,
        'map_field': map_field,
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

    # 2. Put the campaign at the requested cap before preparing the party, so
    #    native species caps and authored level offsets both read the real value.
    session.meta['milestones'] = apply_level_cap(session, args.cap)

    # 3. The existing native preparation API owns party legality.
    spec, words = prepare_protocol(Path(args.party), ROOT)
    writes = [(syms[name] + offset, value) for name, offset, value in words]
    writes.append((syms['gEcAgentPrepCommand'], 1))
    reads = [syms['gEcAgentPrepResult'], syms['gEcAgentPrepErrorSlot']]
    values, _, _ = session.run(frames=PREP_FRAMES, writes=writes, reads=reads)
    if values[syms['gEcAgentPrepResult']] != 1:
        fail('native preparation rejected the manifest: ' + describe_prep_failure(
            values[syms['gEcAgentPrepResult']], values[syms['gEcAgentPrepErrorSlot']], spec))
    session.meta['prepared'] = {'encounter': spec['encounter'], 'party': len(spec['party'])}

    # 4. Start the battle through the native debug lifecycle, under the
    #    trainer's map weather and environment (0 keeps the room's own).
    battle_writes = [
        (syms['gEcAgentBattleTrainerA'], trainers[name_a]),
        (syms['gEcAgentBattleTrainerB'], trainers[name_b] if name_b else 0),
        (syms['gEcAgentBattlePartner'], partners[partner_name]),
        (syms['gEcAgentBattleHalted'], 0),
        (syms['gEcAgentBattleResult'], 0),
    ]
    if 'gEcAgentBattleMapWeather' in syms:
        ow_weather, environment = constants['ow_weather']['values'], constants['environment']['values']
        battle_writes += [
            (syms['gEcAgentBattleMapWeather'], ow_weather[map_field['weather']] + 1),
            (syms['gEcAgentBattleEnvironment'],
             environment[map_field['environment']] + 1 if map_field.get('environment') else 0)]
    else:
        print('battle_driver: warning: this build predates map weather; the battle opens under '
              'the headless room\'s sky', file=sys.stderr)
    battle_writes.append((syms['gEcAgentBattleCommand'], 1))
    # The bridge answers the start command on the next frame. Read its verdict
    # first: a refused start never halts, and would otherwise burn the whole
    # frame budget looking like a hung battle.
    values, _, _ = session.run(frames=4, writes=battle_writes,
                               reads=[syms['gEcAgentBattleResult']])
    verdict = values[syms['gEcAgentBattleResult']]
    if verdict != BATTLE_START_OK:
        fail(f'the bridge refused the battle: {BATTLE_START_RESULTS.get(verdict, verdict)}')
    view, frames_run, stopped = advance_to_halt(
        session, [], png=(session.dir / 'start.png') if args.png else None)
    if not stopped:
        fail(f'no decision point was reached in {frames_run} frames; the battle never halted')
    state = decode_state(session, view)
    annotate_field(session, state)
    log = battle_log(session, view, 0, 0)
    session.meta['started'] = {'frames': frames_run}
    session.meta_path.write_text(json.dumps(session.meta, indent=2) + '\n')
    session.log({'event': 'start', 'trainer_a': name_a, 'trainer_b': name_b,
                 'partner': partner_name, 'seed': args.seed, 'frames': frames_run,
                 'messages': log['messages'], 'moves': log['moves'],
                 'hp_changes': log['hp_changes'], 'popups': log['popups'],
                 'log_complete': log['log_complete'],
                 'state': state})
    print(json.dumps(state, indent=2))


BATTLE_START_OK = 1
# enum EmeraldChampionsAgentBattleResult (include/emerald_champions_agent_battle.h).
BATTLE_START_RESULTS = {
    0: 'pending (the ROM never read the start command)',
    2: 'bad command',
    3: 'bad trainer: unknown trainer id or an empty authored party',
    4: 'bad party: the prepared party is empty',
    5: 'not ready: the overworld was not idle (a script or battle was running)',
}


def map_field_for_start(args, trainer, constants):
    """The trainer's map weather/environment, or the --weather override."""
    field = resolve_map_field(trainer, args.map)
    if args.weather != 'map':
        name = WEATHER_ALIASES.get(args.weather.lower(), args.weather.upper())
        if name not in constants['ow_weather']['values']:
            fail(f'--weather {args.weather}: use map, none, rain, downpour, thunderstorm, sun, '
                 f'sandstorm, snow, fog or a WEATHER_* name')
        field['map_weather'] = field['weather']
        field['weather'] = name
        field['weather_basis'] = f'--weather {args.weather} (the map gives {field["map_weather"]})'
    if field['weather'] not in constants['ow_weather']['values']:
        fail(f'map weather {field["weather"]} is not an overworld weather constant')
    field['battle_effect'] = BATTLE_EFFECT_OF_WEATHER.get(field['weather'], 'none')
    if not field.get('map'):
        print(f'battle_driver: warning: no map script starts {trainer}; the battle opens with '
              f'no map weather. Pass --map <MapName> (the order file has it).', file=sys.stderr)
    return field


def annotate_field(session, state):
    """Put the start-time map resolution next to what the battle actually shows."""
    field = session.meta.get('map_field')
    if field:
        state['field_source']['map'] = {key: field.get(key) for key in (
            'map', 'trainer_xy', 'weather', 'battle_effect', 'weather_basis', 'weather_cycle',
            'story_conditional_weather', 'position_dependent_weather', 'possible_anomaly_weather') if field.get(key)}


def describe_prep_failure(result, slot, spec):
    name = PREP_RESULTS.get(result, f'result {result}')
    party = spec.get('party', [])
    where = f' at slot {slot} ({party[slot]["species"]})' if 0 <= slot < len(party) else ''
    return name + where


def command_state(args):
    session = Session(args.run_dir)
    session.check_artifacts()
    view, _, _ = session.read_view(advance=False,
                                   png=(session.dir / 'state.png') if args.png else None)
    state = decode_state(session, view)
    annotate_field(session, state)
    drop_committed_slots(session, state)
    session.log({'event': 'state', 'state': state})
    print(json.dumps(state, indent=2))


def drop_committed_slots(session, state):
    """A replacement already submitted for one battler is no longer available.

    The engine only marks the slot occupied once the switch-in resolves, so
    between the two halves of a double faint both battlers would otherwise be
    offered the same reserve. The native menu hides it the same way, by passing
    the partner's monToSwitchIntoId into the party screen."""
    record = session.meta.get('replacement_commits') or {}
    if record.get('turn') != state['turn'] or state['phase'] != 'await_switch':
        return {}
    commits = {int(k): v for k, v in record.get('slots', {}).items()}
    for entry in state['pending_decision']:
        taken = {slot for battler, slot in commits.items() if battler != entry['battler']}
        entry['switch_slots'] = [s for s in entry['switch_slots'] if s not in taken]
    return commits


def record_commits(session, state, submitted):
    """Remember replacements submitted during this await_switch chain."""
    pending = {entry['battler']: entry for entry in state['pending_decision']}
    slots = {}
    if (session.meta.get('replacement_commits') or {}).get('turn') == state['turn']:
        slots = dict((session.meta['replacement_commits'] or {}).get('slots', {}))
    for battler, command in submitted.items():
        if command['action'] == 'switch' and pending.get(battler, {}).get('replacing'):
            slots[str(battler)] = command['slot']
    session.meta['replacement_commits'] = {'turn': state['turn'], 'slots': slots}
    session.meta_path.write_text(json.dumps(session.meta, indent=2) + '\n')

def target_kind(actor, target, battlers):
    """How the recorded target relates to the actor.

    Says nothing about the move; it classifies the battler the engine recorded,
    so an ally-targeted move reads "ally" rather than looking like a mis-aimed
    attack. Player-side indices are even, opponent-side odd."""
    if target is None or target >= battlers:
        return 'unknown'
    if target == actor:
        return 'self'
    return 'ally' if (target % 2) == (actor % 2) else 'foe'

def occupant_key(state, active):
    """A stable identity for the Pokemon standing in a slot.

    A battler index is a position, not a Pokemon: after a switch or a faint
    replacement the same index holds someone else. Keying on the owner's party
    index instead follows the individual mon. In a multi battle the partner
    indexes its own party and the second opposing owner has its own too, so the
    owner has to be part of the key."""
    if active['side'] == 'player':
        owner = 'player' if active['agent_controlled'] else 'partner'
    elif 'BATTLE_TYPE_TWO_OPPONENTS' in state['battle_type'] and active['battler'] == 3:
        owner = 'opponent_b'
    else:
        owner = 'opponent_a'
    return f"{owner}:{active['party_slot']}"


def occupants(state):
    """{identity: (species, hp)} for everything whose HP this state can see.

    Player reserves are included, so a Pokemon that switched out mid-turn still
    has its damage attributed to it rather than to whoever replaced it."""
    seen = {}
    for active in state['actives']:
        seen[occupant_key(state, active)] = (active['species'], active['hp'], active['battler'])
    for mon in state['player_reserves']:
        seen.setdefault(f"player:{mon['slot']}", (mon['species'], mon['hp'], None))
    return seen

COMMAND_RE = re.compile(r'^(\d+)\s*:\s*(?:move(\d+)@(\d+)(,mega)?|switch(\d+)|(struggle))$')


def command_act(args):
    session = Session(args.run_dir)
    session.check_artifacts()
    syms = session.syms
    before, _, _ = session.read_view(advance=False)
    state = decode_state(session, before)
    if state['phase'] not in ('await_action', 'await_switch'):
        fail(f'not at a decision point (phase={state["phase"]})')

    drop_committed_slots(session, state)
    pending = {entry['battler']: entry for entry in state['pending_decision']}
    writes = []
    submitted = {}
    for text in args.commands:
        match = COMMAND_RE.match(text.strip())
        if not match:
            fail(f'unparsable command: {text!r} (use "0:move1@3" or "0:move0@1,mega" or "2:switch3" '
                 f'or "0:struggle")')
        battler = int(match[1])
        if battler not in pending:
            fail(f'battler {battler} does not need a command here; pending={sorted(pending)}')
        entry = pending[battler]
        if match[5] is not None:
            slot = int(match[5])
            if slot not in entry['switch_slots']:
                fail(f'battler {battler} cannot switch to slot {slot}; '
                     f'legal={entry["switch_slots"]}')
            if state['phase'] == 'await_action' and entry['switch_blocked_by']:
                fail(f'battler {battler} cannot switch this turn: '
                     f'{entry["switch_blocked_by"][0]}')
            if state['phase'] == 'await_action' and not entry['may_switch']:
                fail(f'battler {battler} cannot switch this turn')
            duplicate = next((b for b, c in submitted.items()
                              if c['action'] == 'switch' and c['slot'] == slot), None)
            if duplicate is not None:
                fail(f'battler {battler} and battler {duplicate} cannot both switch to '
                     f'slot {slot}; one reserve can only be sent out once')
            writes += [(syms['gEcAgentBattleSwitchSlot'] + 4 * battler, slot),
                       (syms['gEcAgentBattleAction'] + 4 * battler, 2)]
            submitted[battler] = {'action': 'switch', 'slot': slot}
        elif match[6] is not None:
            if state['phase'] == 'await_switch':
                fail(f'battler {battler} must be replaced with a switch, not a move')
            if not entry.get('must_struggle'):
                fail(f'battler {battler} still has a legal move; Struggle is only for '
                     f'a battler whose every move is blocked')
            # Fight with nothing selectable: the engine substitutes Struggle
            # and never asks which move, so index and target are unused.
            writes += [(syms['gEcAgentBattleMoveIndex'] + 4 * battler, 0),
                       (syms['gEcAgentBattleTarget'] + 4 * battler, 0),
                       (syms['gEcAgentBattleMega'] + 4 * battler, 0),
                       (syms['gEcAgentBattleAction'] + 4 * battler, 1)]
            submitted[battler] = {'action': 'move', 'move': 'MOVE_STRUGGLE', 'mega': False}
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
    # Only the battler the engine is actually asking must be answered now; a
    # command for the other half of a double faint is held in the mailbox.
    required = {b for b, entry in pending.items() if entry.get('awaiting_now', True)}
    missing = sorted(required - set(submitted))
    if missing:
        fail(f'these battlers still need a command: {missing}')
    record_commits(session, state, submitted)
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

    before_occupants, after_occupants = occupants(state), occupants(after)
    damage = {}
    for key, (species, hp, _) in after_occupants.items():
        if key not in before_occupants:
            continue
        was_species, was_hp, _ = before_occupants[key]
        if was_species == species and was_hp != hp:
            damage[key] = was_hp - hp
    fainted = sorted(
        key for key, (species, hp, _) in after_occupants.items()
        if hp == 0 and key in before_occupants and before_occupants[key][1] > 0
        and before_occupants[key][0] == species)
    occupant_changed = {}
    for active in after['actives']:
        key = occupant_key(after, active)
        previous = next((a for a in state['actives'] if a['battler'] == active['battler']), None)
        if previous is not None and occupant_key(state, previous) != key:
            occupant_changed[active['battler']] = {
                'from': f"{previous['species']} ({occupant_key(state, previous)})",
                'to': f"{active['species']} ({key})",
            }

    log = battle_log(session, view, state['log_head'], state['message_serial'])
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
        # Keyed by the Pokemon, not the slot it stood in. Negative means healed.
        'damage': damage,
        'occupant_changed': occupant_changed,
        # Also by identity: a slot whose occupant was replaced mid-turn holds a
        # live Pokemon afterwards and would otherwise hide the faint.
        'faints': fainted,
        'weather': after['weather'],
        'field': after['field'],
        # Every message of the call, in order. moves[] is each move as it was
        # used: user and target battlers, HP it took from (damage) and gave to
        # (healing) each other battler while it resolved, and the user's own HP
        # change (recoil, Life Orb, drain);
        # hp_changes[] is every HP change with its cause; popups[] the ability
        # and item pop-ups that print no text (Sitrus Berry, Intimidate).
        # message_index places each record before messages[message_index].
        'messages': log['messages'],
        'moves': log['moves'],
        'hp_changes': log['hp_changes'],
        'popups': log['popups'],
        'log_complete': log['log_complete'],
        'player_faints': after['player_faints'],
        'opponent_faints': after['opponent_faints'],
        'outcome': after['outcome'],
        'phase': after['phase'],
    }
    if 'outcome_native' in after:
        events['outcome_native'] = after['outcome_native']
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
    live_faints = None
    for line in session.events.read_text().splitlines():
        record = json.loads(line)
        if record.get('event') != 'act':
            continue
        decisions += 1
        turns = max(turns, record['turn_after'])
        if record['ai_decision_frames']:
            ai_frames.append(record['ai_decision_frames'])
        if record['phase'] != 'ended':
            live_faints = (record['player_faints'], record['opponent_faints'])
    # The engine clears an owner's party during the end-of-battle teardown, so a
    # ROM without the native guard reports that side's faints as zero once the
    # battle is over. Fall back to the last reading taken while it was live.
    player_faints, opponent_faints = state['player_faints'], state['opponent_faints']
    if live_faints is not None:
        player_faints = max(player_faints, live_faints[0])
        opponent_faints = max(opponent_faints, live_faints[1])
    result = {
        'trainer_a': session.meta['trainer_a'],
        'trainer_b': session.meta['trainer_b'],
        'partner': session.meta['partner'],
        'seed': session.meta['seed'],
        'difficulty': session.meta['difficulty'],
        'level_cap': state['level_cap'],
        'phase': state['phase'],
        'outcome': state['outcome'],
        'outcome_native': state.get('outcome_native', state['outcome']),
        'map_weather': (session.meta.get('map_field') or {}).get('weather'),
        'turns': turns,
        'decisions': decisions,
        'player_faints': player_faints,
        'opponent_faints': opponent_faints,
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
    start.add_argument('--map', help='map the fight happens on; normally found from the map '
                                     'script that starts the battle')
    start.add_argument('--weather', default='map',
                       help='overworld weather to open under: map (default: the trainer\'s own '
                            'map, tile and region), none, rain, downpour, thunderstorm, sun, '
                            'sandstorm, snow, fog, or a WEATHER_* name')
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
