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
    replay  witness check: re-run a finished run's seed, party, level deltas,
            ability trials and command log in a fresh run dir; report if identical

start/state/act --brief print the compact per-turn player view instead of JSON.
Nothing printed before the player commits shows the AI's pending action, and
nothing printed after `start` names the seed.

"""
import argparse
import contextlib
import hashlib
import io
import json
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'scripts' / 'playthrough'))
import native_tools
import render_emerald_champions_ui as ui
from rom_artifacts import verify_rom_elf_pair
import build_provenance as provenance
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
CONSTANTS_SCHEMA = 6  # bump whenever build_constants() gains or renames a table
# The legacy 14-message ring; the complete event log (LOG_*) supersedes it.
MSG_BASE, MSG_SIZE, MSG_COUNT = 276, 14, 14
PREV_BASE, PREV_SIZE = 472, 4
FIELD_BASE = 488
MSG_CHARS = (MSG_SIZE - 1) * 4
# Mirrors EC_AGENT_BATTLE_LOG_* in include/emerald_champions_agent_battle.h.
LOG_HEAD_WORD, LOG_SIZE_WORD, MAP_FIELD_WORD = 492, 493, 494
LAST_LIVE_WORD = 495
ROSTER_HEADER, ROSTER_MON_SIZE, ROSTER_WORDS = 9, 22, 273
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
        'BATTLE_TYPE_TWO_OPPONENTS', 'BATTLE_TYPE_MULTI', 'BATTLE_TYPE_INGAME_PARTNER',
        'BATTLE_TYPE_LEGENDARY', 'BATTLE_TYPE_FIRST_BATTLE', 'BATTLE_TYPE_IS_MASTER']),
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
BATTLE_KINDS = {'trainer': 0, 'birch_rescue': 1, 'birth_island_deoxys': 2}
# Copies of the start inputs pinned inside each run directory (see `replay`).
PARTY_COPY, SCENARIO_COPY = 'party.manifest.json', 'scenario.source.json'


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
    nature_names = re.findall(r'(?m)^#define\s+(NATURE_[A-Z0-9_]+)\b',
                              (ROOT / 'include/constants/pokemon.h').read_text())
    requests += [('constants/pokemon.h', name) for name in [*nature_names, 'NUM_NATURES']]
    resolved = resolve_defines(requests)
    for group, (_, names) in DEFINE_GROUPS.items():
        tables[group] = {name: resolved[name] for name in names}
    tables['limits'] = {'trainers': resolved['TRAINERS_COUNT'], 'partners': resolved['PARTNER_COUNT']}
    nature_values = {name: resolved[name] for name in nature_names
                     if 0 <= resolved[name] < resolved['NUM_NATURES']}
    nature_labels = {}
    for name, value in nature_values.items():
        nature_labels.setdefault(str(value), name)
    if len(nature_labels) != resolved['NUM_NATURES']:
        fail('native nature constants do not cover the whole NUM_NATURES range')
    tables['nature'] = {'names': nature_labels, 'values': nature_values}
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
        try:
            result = ui.run(command, timeout=900)
        except RuntimeError as error:
            # Large mailbox READ output must not bury the assertion summary.
            # Keep full diagnostics plus the failed state for reproduction.
            (self.dir / 'runner-failure.txt').write_text(str(error) + '\n')
            native_failure = re.findall(r'MGBA_NATIVE_FAILURE: ([^\n]+)', str(error))
            fail('native assertion/fatal failure: ' + native_failure[-1] if native_failure
                 else 'native runner failed; see ' + str(self.dir / 'runner-failure.txt'))
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
        }
        if entry['side'] == 'player':
            # This turn's choice so far, for the player's own battlers only;
            # "previous_turn" is the authoritative record of the resolved turn.
            # The opposing latch holds the AI's committed action and target
            # before the player commits, so it is never published here (see
            # native_private, logged only after the turn resolves).
            entry['choosing'] = {'action': ACTIONS.get(last & 0xFF, last & 0xFF),
                                 'move_index': (last >> 8) & 0xFF,
                                 'target': (last >> 16) & 0xFF}
        else:
            # Opposing moves are the used-move history in order of use, while
            # PP is per native slot; pairing them would publish unrelated slots'
            # PP. The battle UI shows no opposing PP at all.
            for move in entry['moves']:
                move.pop('pp', None)
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
        'board_source': 'last_live_battle' if words[LAST_LIVE_WORD] else 'current_battle',
        # AI decision timing is private telemetry (native_private), logged with
        # each act record after its turn resolves, never part of the board.
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
        # Native selection progress for the player's own battlers; opposing
        # positions are withheld (null) with the rest of the AI's pending turn.
        'selection_state': [selection[i] if i < len(actives) and actives[i]['side'] == 'player'
                            else None for i in range(4)],
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


def apply_level_cap(session, cap, expected_cap=None):
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
    expected_cap = cap if expected_cap is None else expected_cap
    if view[22] != expected_cap:
        fail(f'campaign player cap did not reach {expected_cap}; the ROM reports {view[22]}')
    return [name for name, _ in rows]


def apply_scenario(session, scenario, expected_cap):
    """Reproduce explicit source scenario state, rather than infer it from chronology.

    This fixture transport is not a reachability certificate. The calibration
    caller validates the certificate before invoking it.
    """
    flags = scenario.get('progression_flags', {})
    if isinstance(flags, list):
        flags = dict.fromkeys(flags, True)
    variables = scenario.get('progression_vars', {})
    if not isinstance(flags, dict) or not isinstance(variables, dict):
        fail('scenario progression_flags/progression_vars must be explicit mappings')
    if any(not re.fullmatch(r'FLAG_[A-Z0-9_]+', name) or type(value) is not bool
           for name, value in flags.items()):
        fail('scenario flags need FLAG_* names and boolean values')
    if any(not re.fullmatch(r'VAR_[A-Z0-9_]+', name) or type(value) is not int
           or not 0 <= value <= 65535 for name, value in variables.items()):
        fail('scenario vars need VAR_* names and 16-bit integer values')
    var_ids = resolve_defines([('constants/vars.h', name) for name in variables]) if variables else {}
    # Clear milestone flags from the generic boot fixture before applying the
    # certificate. Optional-route states need not be a prefix of the cap table.
    state_flags = {name: False for name, _ in campaign_milestones()}
    state_flags.update(flags)
    clear_ids = resolve_defines([('constants/flags.h', name) for name in state_flags])
    operations = [(6, clear_ids[name], int(value)) for name, value in state_flags.items()]
    operations += [(7, var_ids[name], value) for name, value in variables.items()]
    writes = []
    for index, (command, identifier, value) in enumerate(operations):
        frame = index * 4
        writes += [(frame, session.syms['gEcStudioArgs'], identifier),
                   (frame, session.syms['gEcStudioArgs'] + 4, value),
                   (frame, session.syms['gEcStudioCommand'], command)]
    session.run_at_frames(frames=len(operations) * 4 + 30, writes=writes)
    view, _, _ = session.read_view(advance=False)
    if view[22] != expected_cap:
        fail(f'scenario cap mismatch: expected {expected_cap}, native {view[22]}')
    return state_flags


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
    kind = getattr(args, 'battle_kind', 'trainer')
    if kind not in BATTLE_KINDS:
        fail('unsupported native battle kind')
    if kind == 'birch_rescue' and args.party:
        fail('Birch rescue uses the actual native starter-pair factory, not a prepared party')
    if kind != 'birch_rescue' and not args.party:
        fail('trainer/Deoxys fixtures need a validated prepared-party manifest')
    if kind != 'trainer' and (not args.scenario or args.trainer2 or args.partner):
        fail('scripted wild fixtures need an explicit scenario and no trainer/partner override')
    session = Session(args.run_dir)
    if session.meta_path.exists():
        fail('use a fresh --run-dir for each battle')
    # Exactly what was asked for, before a scenario fills in map/weather, so
    # `replay` can re-run the same battle. The seed itself stays in session.json.
    start_args = {key: getattr(args, key, None) for key in (
        'battle_kind', 'trainer', 'trainer2', 'partner', 'difficulty', 'cap', 'baseline',
        'map', 'weather', 'level_delta', 'ability_trial')}
    level_delta = parse_level_delta(getattr(args, 'level_delta', None))
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
    build_provenance = provenance.carry(rom, session.rom, session.elf)

    constants = build_constants()
    (session.dir / 'constants.json').write_text(json.dumps(constants) + '\n')
    ability_trial = parse_ability_trial(getattr(args, 'ability_trial', None), constants)
    session.constants = constants
    trainers = constants['trainers']
    pairings = script_pairings()

    name_a = args.trainer if kind == 'trainer' else 'TRAINER_NONE'
    if name_a not in trainers:
        fail(f'unknown trainer identifier: {name_a}')
    pairing = pairings.get(name_a) if kind == 'trainer' else None
    name_b, partner_name = args.trainer2, args.partner
    if pairing and name_b is None and partner_name is None:
        name_a, name_b, partner_name = pairing['a'], pairing['b'], pairing['partner']
    partner_name = partner_name or 'PARTNER_NONE'
    if name_b is not None and name_b not in trainers:
        fail(f'unknown trainer identifier: {name_b}')
    partners = resolve_defines([('constants/battle_partner.h', partner_name)])
    difficulty = {'easy': 0, 'medium': 1, 'normal': 1, 'hard': 2}[args.difficulty]
    source_scenario = json.loads(Path(args.scenario).read_text()) if args.scenario else None
    if kind != 'trainer' and source_scenario.get('battle_kind') != kind:
        fail('scripted wild scenario disagrees with requested native battle kind')
    if kind != 'trainer' and not any(source_scenario.get('expected_scripted_wild', {}).get(key)
                                    for key in ('allowed_teams', 'source_member_domains')):
        fail('scripted wild fixture needs source-enumerated expected native loadouts')
    certified_rescue = kind == 'birch_rescue' and source_scenario.get('producer', {}).get('kind') == 'birch_rescue'
    if certified_rescue:
        import battle_scripted_wild_arsenal as wild
        wild.certify_scenario(source_scenario)
    battle_field = (source_scenario or {}).get('battle_field', {})
    if kind != 'trainer' and not all(battle_field.get(key) for key in ('map', 'weather', 'environment')):
        fail('scripted wild fixture requires explicit source-derived map/weather/environment')
    if battle_field.get('map'):
        args.map = battle_field['map']
    if battle_field.get('weather'):
        args.weather = battle_field['weather']
    map_field = map_field_for_start(args, name_a, constants)
    if battle_field.get('environment'):
        if battle_field['environment'] not in constants['environment']['values']:
            fail('scenario battle environment is not a native constant')
        map_field['environment'] = battle_field['environment']

    session.meta = {
        'rom_sha256': rom_hash, 'elf_sha256': elf_hash, 'seed': args.seed,
        'build_provenance': build_provenance, 'build_provenance_label': provenance.label(build_provenance),
        'trainer_a': name_a, 'trainer_b': name_b, 'partner': partner_name,
        'battle_kind': kind, 'battle_id': (source_scenario or {}).get('battle_id'),
        'difficulty': args.difficulty, 'level_cap': args.cap,
        'party_manifest': str(Path(args.party).resolve()) if args.party else None,
        'party_sha256': hashlib.sha256(Path(args.party).read_bytes()).hexdigest() if args.party else None,
        'script_pairing': pairing,
        'map_field': map_field,
        'start_args': start_args,
        'level_delta': {'spec': start_args['level_delta'], 'applied': level_delta_labels(level_delta)},
        'ability_trial': {'spec': start_args['ability_trial'],
                          'applied': ability_trial_labels(ability_trial, constants)},
        'scope': ('Synthetic headless benchmark: native debug trainer lifecycle, native AI, '
                  'user-authorized stage-legal preparation. Not earned campaign play.'),
    }
    # Pin the exact inputs next to the pinned build, so a witness replay never
    # depends on a manifest or scenario file that was edited afterwards.
    for option, copy_name, key in ((args.party, PARTY_COPY, 'party_copy'),
                                   (args.scenario, SCENARIO_COPY, 'scenario_copy')):
        if option:
            source, target = Path(option).resolve(), session.dir / copy_name
            if source != target.resolve():
                shutil.copy2(source, target)
            session.meta[key] = copy_name
    if kind != 'trainer':
        session.meta['scope'] = (f'Synthetic headless {kind} fixture using actual native factory/format/callback; '
                                 'defeat-only actions. No capture, acquisition or earned campaign-progress claim.')
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
    if args.scenario:
        scenario_path = Path(args.scenario)
        scenario = source_scenario
        if scenario.get('difficulty') != args.difficulty or scenario.get('level_cap') != args.cap:
            fail('scenario difficulty/player cap disagree with start arguments')
        if kind == 'trainer' and scenario.get('trainer_id') not in {args.trainer, name_a, name_b}:
            fail('scenario trainer disagrees with requested battle')
        session.meta['scenario'] = scenario
        session.meta['scenario_sha256'] = hashlib.sha256(scenario_path.read_bytes()).hexdigest()
        session.meta['milestones'] = apply_scenario(session, scenario, args.cap)
    else:
        session.meta['milestones'] = apply_level_cap(session, args.baseline or args.cap, args.cap)
    setup_view, _, _ = session.read_view(advance=False)
    if setup_view[23] != difficulty:
        fail(f'scenario difficulty mismatch: requested {args.difficulty}, native {setup_view[23]}')
    if certified_rescue:
        gender = source_scenario['expected_player_factory']['context']['player_gender']
        values, _, _ = session.run(frames=4,
            writes=[(syms['gEcStudioArgs'], gender), (syms['gEcStudioCommand'], 11)],
            reads=[syms['gEcStudioResult']])
        if values[syms['gEcStudioResult']] != 1:
            fail('native rescue gender setup was refused')

    # 3. The existing native preparation API owns party legality.
    if kind != 'birch_rescue':
        spec, words = prepare_protocol(Path(args.party), ROOT)
        writes = [(syms[name] + offset, value) for name, offset, value in words]
        writes.append((syms['gEcAgentPrepCommand'], 1))
        reads = [syms['gEcAgentPrepResult'], syms['gEcAgentPrepErrorSlot']]
        values, _, _ = session.run(frames=PREP_FRAMES, writes=writes, reads=reads)
        if values[syms['gEcAgentPrepResult']] != 1:
            fail('native preparation rejected the manifest: ' + describe_prep_failure(
                values[syms['gEcAgentPrepResult']], values[syms['gEcAgentPrepErrorSlot']], spec))
        session.meta['prepared'] = {'encounter': spec['encounter'], 'party': len(spec['party'])}
    else:
        session.meta['prepared'] = {'factory': 'GiveEmeraldChampionsStarterPair', 'party': 2,
                                    'level': 5, 'moves': 'native natural level-five', 'items': 'none'}

    # 4. Start the battle through the native debug lifecycle, under the
    #    trainer's map weather and environment (0 keeps the room's own).
    battle_writes = [
        (syms['gEcAgentBattleTrainerA'], trainers[name_a]),
        (syms['gEcAgentBattleTrainerB'], trainers[name_b] if name_b else 0),
        (syms['gEcAgentBattlePartner'], partners[partner_name]),
        (syms['gEcAgentBattleHalted'], 0),
        (syms['gEcAgentBattleResult'], 0),
    ]
    # Per-member opposing level deltas (the tuning lever), applied natively
    # after the difficulty formula as the authored party is created. Always
    # written when the build has the array, so an unset delta is a real zero.
    if 'gEcAgentBattleLevelDelta' in syms:
        battle_writes += [(syms['gEcAgentBattleLevelDelta'] + 4 * index, value)
                          for index, value in enumerate(level_delta)]
    elif any(level_delta):
        fail('this build predates gEcAgentBattleLevelDelta; --level-delta needs a newer headless ROM')
    # Per-member trial abilities (0 keeps the authored one), checked natively
    # for species legality while the party is created.
    trial_symbols = ('gEcAgentBattleAbilityTrial', 'gEcAgentBattleTrialRejected')
    if all(name in syms for name in trial_symbols):
        battle_writes += [(syms['gEcAgentBattleAbilityTrial'] + 4 * index, value)
                          for index, value in enumerate(ability_trial)]
        battle_writes.append((syms['gEcAgentBattleTrialRejected'], 0))
    elif any(ability_trial):
        fail('this build predates gEcAgentBattleAbilityTrial; --ability-trial needs a newer headless ROM')
    if 'gEcAgentBattleKind' in syms:
        battle_writes.append((syms['gEcAgentBattleKind'], BATTLE_KINDS[kind]))
    elif kind != 'trainer':
        fail('this build predates the concrete scripted-wild bridge')
    if kind == 'birch_rescue':
        pair = source_scenario.get('opening_parameters', {})
        first, second = pair.get('first'), pair.get('second')
        if any(type(value) is not int or not 0 <= value < 3 for value in (first, second)) or first == second:
            fail('rescue scenario requires two distinct native starter indices')
        battle_writes += [(syms['gEcAgentBattleFirstStarter'], first), (syms['gEcAgentBattleSecondStarter'], second)]
    if 'gEcAgentBattleMapWeather' in syms:
        ow_weather, environment = constants['ow_weather']['values'], constants['environment']['values']
        battle_writes += [
            (syms['gEcAgentBattleMapWeather'], ow_weather[map_field['weather']] + 1),
            (syms['gEcAgentBattleEnvironment'],
             environment[map_field['environment']] + 1 if map_field.get('environment') else 0)]
    elif args.scenario:
        fail('scenario playback requires a native bridge with map weather/environment support')
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
    for attempt in range(7):
        if verdict != 0:
            break
        values, _, _ = session.run(frames=4, reads=[syms['gEcAgentBattleResult']])
        verdict = values[syms['gEcAgentBattleResult']]
    if verdict != BATTLE_START_OK:
        fail(f'the bridge refused the battle: {BATTLE_START_RESULTS.get(verdict, verdict)}')
    def check_trials():
        # A refused trial keeps the authored ability, so the fixture is not
        # the one requested and must not be played.
        if not any(ability_trial):
            return
        address = syms['gEcAgentBattleTrialRejected']
        values, _, _ = session.run(frames=1, reads=[address], advance=False)
        refused = rejected_trials(values[address], ability_trial, constants)
        session.meta['ability_trial']['rejected'] = refused
        session.meta_path.write_text(json.dumps(session.meta, indent=2) + '\n')
        if refused:
            fail('--ability-trial refused natively (not legal for that member\'s species; it would '
                 'keep its authored ability): ' + '; '.join(refused))
    # The authored party exists once the bridge accepts the battle (the roster
    # audit reads it here too); checked again at the first decision point.
    check_trials()
    if kind != 'birch_rescue':
        session.meta['opponent_identity'] = audit_opponent_roster(session, source_scenario)
    view, frames_run, stopped = advance_to_halt(
        session, [], png=(session.dir / 'start.png') if args.png else None)
    if not stopped:
        fail(f'no decision point was reached in {frames_run} frames; the battle never halted')
    check_trials()
    if kind == 'birch_rescue':
        session.meta['opponent_identity'] = audit_opponent_roster(session, source_scenario)
        if certified_rescue:
            session.meta['player_factory'] = audit_rescue_player_factory(session, source_scenario)
    state = decode_state(session, view)
    annotate_field(session, state)
    log = battle_log(session, view, 0, 0)
    session.meta['started'] = {'frames': frames_run}
    update_tracking(session, None, state, log, turn_resolved=0)
    session.meta_path.write_text(json.dumps(session.meta, indent=2) + '\n')
    session.log({'event': 'start', 'trainer_a': name_a, 'trainer_b': name_b,
                 'partner': partner_name, 'seed': args.seed, 'frames': frames_run,
                 'messages': log['messages'], 'moves': log['moves'],
                 'hp_changes': log['hp_changes'], 'popups': log['popups'],
                 'log_complete': log['log_complete'],
                 'state': state, **ai_timing(view)})
    # Nothing printed from here on names the seed: the player never needs it,
    # and knowing it would let a caller rehearse the same RNG stream.
    applied = session.meta['level_delta']['applied']
    trials = session.meta['ability_trial']['applied']
    if getattr(args, 'brief', False):
        text = render_brief(session, state, log['messages'], heading='Battle start')
        notes = ''
        if applied:
            notes += ('\nOpposing level deltas (authored member order): '
                      + ', '.join(f'{k}{v:+d}' for k, v in applied.items()))
        if trials:
            notes += ('\nOpposing ability trials (authored member order): '
                      + ', '.join(f'{k} {pretty(v)}' for k, v in trials.items()))
        print(text.replace('\nLog:', notes + '\nLog:', 1))
    else:
        # Deltas and trials are the caller's own tuning choice, not hidden
        # information about the battle.
        print(json.dumps({**state, 'level_delta': applied, 'ability_trial': trials}, indent=2))


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


def decode_opponent_roster(words, constants):
    """Private exact native inputs; never added to player-policy state."""
    if len(words) != ROSTER_WORDS or words[0] != 1:
        fail('native opponent roster audit has an unsupported/incomplete schema')
    owners = []
    for owner in range(2):
        team = []
        for slot in range(6):
            base = ROSTER_HEADER + (owner * 6 + slot) * ROSTER_MON_SIZE
            if not words[base]:
                continue
            team.append({
                'slot': slot + 1, 'species': name_of(constants['species'], words[base], 'SPECIES_'),
                'level': words[base + 1], 'item': name_of(constants['item'], words[base + 2], 'ITEM_'),
                'ability': name_of(constants['ability'], words[base + 3], 'ABILITY_'),
                'nature': name_of(constants['nature'], words[base + 4], 'NATURE_'),
                'friendship': words[base + 5], 'evs': words[base + 6:base + 12],
                'ivs': words[base + 12:base + 18],
                'moves': [name_of(constants['move'], value, 'MOVE_') for value in words[base + 18:base + 22]],
            })
        if len(team) != words[1 + owner] or not 0 <= words[1 + owner] <= 6:
            fail('native opponent roster count disagrees with published members')
        owners.append({'owner': 'A' if owner == 0 else 'B', 'trainer_id_native': words[7 + owner], 'team': team})
    return {'schema_version': 1, 'starter_generation_raw': words[3],
            'generation': words[3] if 1 <= words[3] <= 9 else 3,
            'unchosen_index': words[4], 'unchosen_species': name_of(constants['species'], words[5], 'SPECIES_'),
            'battle_type_native': words[6], 'owners': owners}


def same_species_identity(expected, actual, species_values=None):
    """Accept only identical names or compiled enum IDs, never base-species forms."""
    if expected == actual:
        return True
    if species_values is None:
        return False
    left, right = species_values.get(expected), species_values.get(actual)
    return left is not None and right is not None and left == right


def verify_opponent_identity(expected, roster, trainers, species_values=None):
    """Fail closed when a source scenario plays a different native opponent."""
    contexts = expected if isinstance(expected, list) else [expected]
    covered = set()
    for context in contexts:
        identifier = trainers.get(context.get('trainer_id'))
        found = [owner for owner in roster['owners'] if owner['trainer_id_native'] == identifier]
        if identifier is None or len(found) != 1:
            fail('expected opponent trainer is not a unique native owner')
        covered.add(found[0]['owner'])
        for key in ('generation', 'unchosen_index', 'unchosen_species'):
            if key in context and not (same_species_identity(context[key], roster[key], species_values)
                                       if key == 'unchosen_species' else context[key] == roster[key]):
                fail(f'opponent identity {key} mismatch: expected {context[key]}, native {roster[key]}')
        wanted = context.get('team')
        actual = found[0]['team']
        if not isinstance(wanted, list) or len(wanted) != len(actual):
            fail('expected opponent party size differs from native campaign factory')
        for authored, mon in zip(wanted, actual):
            for key in ('slot', 'species', 'level', 'item', 'ability', 'nature', 'friendship', 'evs', 'ivs', 'moves'):
                if key not in authored:
                    fail(f'expected opponent identity is incomplete: missing {key}')
                value = authored[key]
                if key == 'moves':
                    value = [*value, *(['MOVE_NONE'] * (4 - len(value)))]
                if not (same_species_identity(value, mon[key], species_values)
                        if key == 'species' else value == mon[key]):
                    fail(f'opponent identity slot {mon["slot"]} {key} mismatch: expected {value}, native {mon[key]}')
    if covered != {owner['owner'] for owner in roster['owners'] if owner['team']}:
        fail('expected opponent certificate does not cover every native opposing owner')


def verify_scripted_wild_identity(expected, roster, constants, scenario=None):
    """Concrete wild formats and source-enumerated variants; no trainer aliases."""
    kind = expected.get('battle_kind')
    flags = constants['battletype']
    actual = roster['battle_type_native']
    # Battle initialization adds its local master bookkeeping bit. Rescue is
    # audited after initialization; Deoxys immediately after its factory. The
    # encounter format must agree at either observation point.
    actual &= ~flags['BATTLE_TYPE_IS_MASTER']
    if actual & flags['BATTLE_TYPE_TRAINER']:
        fail('scripted wild input was generated as a trainer battle')
    if kind == 'birth_island_deoxys':
        if actual != flags['BATTLE_TYPE_LEGENDARY']:
            fail('Deoxys input must retain native single legendary format')
        count = 1
    elif kind == 'birch_rescue':
        if actual != flags['BATTLE_TYPE_FIRST_BATTLE'] | flags['BATTLE_TYPE_DOUBLE']:
            fail('rescue input must retain native first-battle doubles format')
        count = 2
    else:
        fail('unsupported scripted wild identity kind')
    owners = roster['owners']
    if owners[1]['team'] or len(owners[0]['team']) != count:
        fail('scripted wild native owner/member counts disagree')
    if 'source_member_domains' in expected:
        import battle_scripted_wild_arsenal as wild
        try:
            wild.certify_scenario(scenario or {})
            if expected != scenario['expected_scripted_wild']:
                raise ValueError('Source factory domains were substituted')
            wild.verify_members(expected['source_member_domains'], owners[0]['team'])
        except ValueError as error:
            fail(str(error))
        return {'source_domains_verified': True}
    allowed = expected.get('allowed_teams')
    if not isinstance(allowed, list) or not allowed:
        fail('scripted wild identity needs source-enumerated allowed teams')
    matches = []
    for index, team in enumerate(allowed):
        if len(team) != count:
            fail('source wild variant has the wrong member count')
        same = True
        for author, mon in zip(team, owners[0]['team']):
            for key in ('slot', 'species', 'level', 'item', 'ability', 'nature', 'friendship', 'evs', 'ivs', 'moves'):
                if key not in author:
                    fail(f'source wild identity is incomplete: missing {key}')
                value = author[key]
                if key == 'moves':
                    value = [*value, *(['MOVE_NONE'] * (4 - len(value)))]
                same &= (same_species_identity(value, mon[key], constants['species']['values'])
                         if key == 'species' else value == mon[key])
        if same:
            matches.append(index)
    if len(matches) != 1:
        fail('native scripted wild loadout does not identify one source-legal variant')
    return {'observed_variant': matches[0], 'variant_count': len(allowed)}


def audit_opponent_roster(session, scenario):
    symbol = session.syms.get('gEcAgentBattleOpponentRoster')
    expected = (scenario or {}).get('expected_opponent')
    kind = session.meta.get('battle_kind', 'trainer')
    if kind != 'trainer':
        expected = (scenario or {}).get('expected_scripted_wild')
    if symbol is None:
        if expected:
            fail('build predates exact native opponent audit; rebuild before testing regional scenarios')
        return {'status': 'unverified', 'reason': 'legacy build without native opponent audit'}
    addresses = [symbol + index * 4 for index in range(ROSTER_WORDS)]
    values, _, _ = session.run(frames=1, reads=addresses, advance=False)
    roster = decode_opponent_roster([values[address] for address in addresses], session.constants_table())
    (session.dir / 'opponent-roster.json').write_text(json.dumps(roster, indent=2) + '\n')
    if not expected:
        return {'status': 'unverified', 'reason': 'scenario has no expected opponent certificate'}
    variant = {}
    if kind == 'trainer':
        expected = expected_with_trials(expected, roster, session.constants_table()['trainers'],
                                        session.meta)
        verify_opponent_identity(expected, roster, session.constants_table()['trainers'],
                                 session.constants_table()['species']['values'])
    else:
        if expected.get('battle_kind') != kind:
            fail('source wild identity disagrees with requested battle kind')
        variant = verify_scripted_wild_identity(expected, roster, session.constants_table(), scenario)
    return {'status': 'verified', **variant, 'roster_sha256': hashlib.sha256(
        (session.dir / 'opponent-roster.json').read_bytes()).hexdigest()}


def decode_rescue_player_factory(words, constants):
    if len(words) != 57 or words[0] != 1 or words[1] != 2:
        fail('native rescue player factory audit is incomplete/unsupported')
    # Reuse the same native 22-field row decoder as the opponent audit.
    padded = [0] * ROSTER_WORDS
    padded[:4] = [1, 2, 0, words[2]]
    padded[ROSTER_HEADER:ROSTER_HEADER + 44] = words[7:51]
    team = decode_opponent_roster(padded, constants)['owners'][0]['team']
    for slot, mon in enumerate(team):
        extra = 51 + slot * 3
        mon.update(pokerus=words[extra], pp_bonuses=words[extra + 1], status=words[extra + 2])
    return {'context': dict(zip(('generation', 'first', 'second', 'player_gender', 'opening_state'), words[2:7])),
            'members': team}


def audit_rescue_player_factory(session, scenario):
    import battle_scripted_wild_arsenal as wild
    symbol = session.syms.get('gEcAgentBattlePlayerFactory')
    if symbol is None:
        fail('build predates complete rescue player factory audit')
    addresses = [symbol + index * 4 for index in range(57)]
    values, _, _ = session.run(frames=1, reads=addresses, advance=False)
    actual = decode_rescue_player_factory([values[address] for address in addresses], session.constants_table())
    expected = scenario['expected_player_factory']
    try:
        wild.certify_scenario(scenario)
        if actual['context'] != expected['context']:
            raise ValueError('Native rescue generation/ordered pair/gender context mismatch')
        wild.verify_members(expected['members'], actual['members'])
    except ValueError as error:
        fail(str(error))
    path = session.dir / 'player-factory.json'
    path.write_text(json.dumps(actual, indent=2) + '\n')
    return {'status': 'verified', 'roster_sha256': hashlib.sha256(path.read_bytes()).hexdigest()}


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
    if getattr(args, 'brief', False):
        # `state` never advances, so every log line was already printed by the
        # start/act call that produced it.
        print(render_brief(session, state, None, heading='Current board'))
    else:
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
        if battler in submitted:
            fail(f'battler {battler} received more than one command in this decision')
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
    # The events file gets two private fields the player never sees printed:
    # AI decision timing, and the opposing action the native latch already
    # held when these commands were submitted. Both are written only now,
    # after that turn resolved, for the post-battle audit (for example that
    # the AI committed before the player did).
    session.log({**events, **ai_timing(view),
                 'audit_pre_commit_opponent_latch': opponent_latch(before)})
    # The battle has already advanced: a failure in the view layer below must
    # never lose this call's report, so it falls back to the JSON record.
    output = json.dumps(events, indent=2)
    try:
        update_tracking(session, state, after, log, turn_resolved=state['turn'])
        session.meta_path.write_text(json.dumps(session.meta, indent=2) + '\n')
        if getattr(args, 'brief', False):
            drop_committed_slots(session, after)
            annotate_field(session, after)
            output = render_brief(session, after, log['messages'],
                                  heading=f'Resolved turn {state["turn"]}')
    except Exception as error:  # noqa: BLE001 - report, never lose the act
        print(f'battle_driver: warning: brief view failed ({error!r}); printing the JSON '
              'record instead', file=sys.stderr)
    print(output)
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
        if record.get('ai_decision_frames'):
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
        'battle_kind': session.meta.get('battle_kind', 'trainer'),
        'battle_id': session.meta.get('battle_id'),
        'trainer_a': session.meta['trainer_a'],
        'trainer_b': session.meta['trainer_b'],
        'partner': session.meta['partner'],
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
    # The seed stays in the run's own files (session.json, result.json) for
    # replay and audit; the printed result never names it.
    (session.dir / 'result.json').write_text(
        json.dumps({**result, 'seed': session.meta['seed']}, indent=2) + '\n')
    print(json.dumps(result, indent=2))


# ------------------------------------------------------------ level deltas

LEVEL_DELTA_RE = re.compile(r'^([AB])([0-5*])\s*=\s*([+-]?\d+)$')


def parse_level_delta(spec):
    """`A0=+2,A3=-1,B1=+1` / `A*=+1` -> 12 ints (owner A members 0-5, then B).

    Indices are the authored member order (the teams file / gTrainers party),
    not the send-out order. A wildcard sets all six of that owner's members;
    an explicit member overrides it whatever the order."""
    values = [0] * 12  # gEcAgentBattleLevelDelta: s32[PARTY_SIZE * 2]
    if not spec:
        return values
    wildcard, explicit = {}, {}
    for part in (piece.strip() for piece in spec.split(',')):
        match = LEVEL_DELTA_RE.match(part)
        if not match:
            fail(f'--level-delta: cannot read {part!r}; use A0=+2,A3=-1,B1=+1 or A*=+1')
        owner, member, delta = match[1], match[2], int(match[3])
        if not -99 <= delta <= 99:
            fail(f'--level-delta: {part!r} is outside -99..+99')
        table = wildcard if member == '*' else explicit
        key = owner if member == '*' else (owner, int(member))
        if key in table:
            fail(f'--level-delta: {owner}{member} is given twice')
        table[key] = delta
    for owner, delta in wildcard.items():
        for member in range(6):
            values[(6 if owner == 'B' else 0) + member] = delta
    for (owner, member), delta in explicit.items():
        values[(6 if owner == 'B' else 0) + member] = delta
    return values


def level_delta_labels(values):
    """{'A0': 3, ...} for the non-zero deltas, in array order."""
    return {f"{'AB'[index // 6]}{index % 6}": value for index, value in enumerate(values) if value}


TRIAL_RE = re.compile(r'^([AB])([0-5])\s*=\s*([A-Za-z0-9_]+)$')


def parse_ability_trial(spec, constants):
    """`A2=ABILITY_RAMPAGE,B0=ABILITY_SNOW_WARNING` -> 12 ability ids (0 keeps
    the authored ability), in authored member order like --level-delta."""
    values = [0] * 12  # gEcAgentBattleAbilityTrial: u32[PARTY_SIZE * 2]
    if not spec:
        return values
    known = constants['ability']['values']
    seen = set()
    for part in (piece.strip() for piece in spec.split(',')):
        match = TRIAL_RE.match(part)
        if not match:
            fail(f'--ability-trial: cannot read {part!r}; use A2=ABILITY_RAMPAGE,B0=ABILITY_SNOW_WARNING')
        owner, member = match[1], int(match[2])
        name = match[3].upper()
        name = name if name.startswith('ABILITY_') else 'ABILITY_' + name
        if (owner, member) in seen:
            fail(f'--ability-trial: {owner}{member} is given twice')
        seen.add((owner, member))
        if name not in known or name == 'ABILITY_NONE' or not known[name]:
            fail(f'--ability-trial: unknown ability {name} for {owner}{member}')
        values[(6 if owner == 'B' else 0) + member] = known[name]
    return values


def ability_trial_labels(values, constants):
    """{'A2': 'ABILITY_RAMPAGE', ...} for the requested trials, in array order."""
    return {f"{'AB'[index // 6]}{index % 6}": name_of(constants['ability'], value, 'ABILITY_')
            for index, value in enumerate(values) if value}


def rejected_trials(mask, values, constants):
    """Readable refusals from gEcAgentBattleTrialRejected (bit i = member i)."""
    labels = ability_trial_labels(values, constants)
    refused = []
    for index in range(12):
        if mask >> index & 1:
            label = f"{'AB'[index // 6]}{index % 6}"
            refused.append(f"{label} cannot take {labels.get(label, 'the requested ability')}")
    return refused


def expected_with_trials(expected, roster, trainers, meta):
    """The source identity certificate with this run's own level deltas and
    ability trials applied, so a tuning fixture is still audited exactly.

    Deltas and trials index the authored member order; the certificate's team
    is compared in native order. Where the two differ (party pools) the audit
    fails closed rather than accepting a guess."""
    deltas = parse_level_delta((meta.get('level_delta') or {}).get('spec'))
    trials = (meta.get('ability_trial') or {}).get('applied') or {}
    if not any(deltas) and not trials:
        return expected
    contexts = expected if isinstance(expected, list) else [expected]
    adjusted = []
    for context in contexts:
        context = json.loads(json.dumps(context))
        identifier = trainers.get(context.get('trainer_id'))
        owners = [owner['owner'] for owner in roster['owners'] if owner['trainer_id_native'] == identifier]
        if len(owners) == 1:
            offset = 6 if owners[0] == 'B' else 0
            for member, mon in enumerate(context.get('team') or []):
                if member >= 6:
                    break
                if 'level' in mon and deltas[offset + member]:
                    mon['level'] = max(1, min(100, mon['level'] + deltas[offset + member]))
                trial = trials.get(f"{owners[0]}{member}")
                if trial and 'ability' in mon:
                    mon['ability'] = trial
        adjusted.append(context)
    return adjusted if isinstance(expected, list) else adjusted[0]


# ------------------------------------------------------- private telemetry

def ai_timing(words):
    """AI decision timing for the pending turn: telemetry, not board state."""
    return {'ai_decision_frames': words[9], 'ai_setup_frames': words[10],
            'ai_delay_frames': words[11]}


def opponent_latch(words):
    """The opposing side's action as the native latch holds it right now.

    Read before the player's commands are written, this is the AI's already
    committed choice for the turn. It is only ever written to the events file
    after that turn resolves (the audit that the AI committed first), never
    printed to the player."""
    latch = {}
    for index in range(min(words[13], 4)):
        base = BATTLER_BASE + index * BATTLER_SIZE
        if words[base + 8] & 4:
            continue
        last = words[base + 26]
        latch[str(index)] = {'action': ACTIONS.get(last & 0xFF, last & 0xFF),
                             'move_index': (last >> 8) & 0xFF,
                             'target': (last >> 16) & 0xFF,
                             'selection': (words[31] >> (8 * index)) & 0xFF}
    return latch


# -------------------------------------------------------------- brief view

NAME_PREFIXES = ('SPECIES_', 'MOVE_', 'ITEM_', 'ABILITY_', 'TYPE_', 'SIDE_STATUS_',
                 'STATUS_FIELD_', 'B_WEATHER_', 'STATUS1_', 'TRAINER_')
STATUS_SHORT = {'STATUS1_POISON': 'PSN', 'STATUS1_BURN': 'BRN', 'STATUS1_FREEZE': 'FRZ',
                'STATUS1_PARALYSIS': 'PAR', 'STATUS1_TOXIC_POISON': 'TOX',
                'STATUS1_FROSTBITE': 'FRB'}
STAT_SHORT = {'atk': 'Atk', 'def': 'Def', 'spa': 'SpA', 'spd': 'SpD', 'spe': 'Spe',
              'acc': 'Acc', 'eva': 'Eva'}
# Nominal lengths counting the turn a condition starts. Light Clay, weather
# rocks and similar items extend them, so the brief view calls them nominal.
NOMINAL_TURNS = {'SIDE_STATUS_TAILWIND': 4, 'SIDE_STATUS_REFLECT': 5,
                 'SIDE_STATUS_LIGHTSCREEN': 5, 'SIDE_STATUS_AURORA_VEIL': 5,
                 'SIDE_STATUS_SAFEGUARD': 5, 'SIDE_STATUS_MIST': 5,
                 'STATUS_FIELD_TRICK_ROOM': 5, 'STATUS_FIELD_MAGIC_ROOM': 5,
                 'STATUS_FIELD_WONDER_ROOM': 5, 'STATUS_FIELD_GRAVITY': 5}
# The native view publishes no volatile conditions or entry hazards, so the
# brief view reads them from battle text (src/battle_message.c), exactly as a
# player learns them. (phrase, condition, set it?) for the Pokemon named last
# before the phrase.
VOLATILE_TEXT = (('snapped out of its confusion', 'confused', False),
                 ('became confused', 'confused', True),
                 ('put in a substitute', 'substitute', True),
                 ("'s substitute faded", 'substitute', False),
                 ('fell for the taunt', 'taunted', True),
                 ('shook off the taunt', 'taunted', False),
                 ('was seeded', 'leech seed', True),
                 ('fell in love', 'infatuated', True),
                 ('grew drowsy', 'drowsy', True),
                 ('is no longer disabled', 'move disabled', False),
                 ('was disabled', 'move disabled', True))
# (phrase, hazard, laid?) on the side the text names: "the opposing" or yours.
HAZARD_TEXT = (('toxic spikes were scattered', 'Toxic Spikes', True),
               ('spikes were scattered', 'Spikes', True),
               ('pointed stones float', 'Stealth Rock', True),
               ('sticky web has been laid', 'Sticky Web', True),
               ('toxic spikes disappeared', 'Toxic Spikes', False),
               ('spikes disappeared', 'Spikes', False),
               ('pointed stones disappeared', 'Stealth Rock', False),
               ('sticky web has disappeared', 'Sticky Web', False),
               ('blew away sticky web', 'Sticky Web', False))
HAZARD_LAYERS = {'Spikes': 3, 'Toxic Spikes': 2, 'Stealth Rock': 1, 'Sticky Web': 1}
TARGET_LABELS = {'both': 'both foes', 'foes_and_ally': 'all others', 'all_battlers': 'everyone',
                 'user': 'self', 'ally': 'ally', 'user_and_ally': 'own side',
                 'field': 'field', 'opponents_field': 'foe side', 'random': 'random foe'}
DIFFICULTY_NAMES = {0: 'Easy', 1: 'Medium', 2: 'Hard'}


def clean_text(text):
    """Battle text on one line: the ROM's line breaks and '~' spaces removed."""
    return ' '.join(text.replace('~', ' ').split())


def pretty(name):
    if not name or str(name).endswith('_NONE'):
        return '-'
    name = str(name)
    for prefix in NAME_PREFIXES:
        if name.startswith(prefix):
            name = name[len(prefix):]
            break
    return name.replace('_', ' ').title()


def name_forms(species):
    """How battle text names a species: the full form name, then its base name."""
    full = pretty(species).lower()
    words = full.split()
    return [full] if len(words) < 2 or len(words[0]) < 4 else [full, words[0]]


def text_subject(text, position, candidates):
    """The battler named last before `position` ("the opposing X" for foes)."""
    best, best_at = None, -1
    for key, side, forms in candidates:
        for form in forms:
            needle = 'opposing ' + form if side == 'opponent' else form
            at = text.find(needle)
            while 0 <= at < position:
                own = side == 'opponent' or not text[:at].endswith('opposing ')
                if own and at > best_at:
                    best, best_at = key, at
                at = text.find(needle, at + 1)
    return best


def update_tracking(session, before, after, log, turn_resolved):
    """Remember what the battle has shown the player so far.

    Revealed opposing items and abilities (pop-ups and battle text), volatile
    conditions and entry hazards read from battle text, the turn each active
    Pokemon entered, and the turn each weather/terrain/room/side condition was
    first seen. Only facts the player has already been shown go in here."""
    track = session.meta.setdefault('brief', {})
    revealed = track.setdefault('revealed', {})
    volatiles = track.setdefault('volatiles', {})
    entered = track.setdefault('entered', {})
    since = track.setdefault('since', {})
    hazards = track.setdefault('hazards', {'player': {}, 'opponent': {}})
    shown = [(before, a) for a in (before or {}).get('actives', [])] + \
            [(after, a) for a in after['actives']]
    candidates, held = [], {}
    for state, active in shown:
        if active['species'] in (None, 'SPECIES_NONE'):
            continue
        key = occupant_key(state, active)
        candidates.append((key, active['side'], name_forms(active['species'])))
        if active['side'] == 'opponent':
            held.setdefault(key, set()).update(
                {('item', active['item']), ('ability', active['ability'])})
    positions = {active['battler']: occupant_key(after, active) for active in after['actives']}
    for popup in log.get('popups', []):
        key = positions.get(popup['battler'])
        if key and key.startswith('opponent'):
            for field in ('item', 'ability'):
                if field in popup:
                    revealed.setdefault(key, {})[field] = popup[field]
    for message in log.get('messages', []):
        text = clean_text(message).lower()
        for key, side, forms in candidates:
            if side == 'opponent' and any('opposing ' + form in text for form in forms):
                for field, value in held.get(key, ()):
                    if not value.endswith('_NONE') and pretty(value).lower() in text:
                        revealed.setdefault(key, {})[field] = value
        for phrase, condition, present in VOLATILE_TEXT:
            at = text.find(phrase)
            if at < 0:
                continue
            key = text_subject(text, at, candidates)
            if key:
                current = set(volatiles.get(key, []))
                (current.add if present else current.discard)(condition)
                volatiles[key] = sorted(current)
            break
        for phrase, hazard, laid in HAZARD_TEXT:
            if phrase in text:
                side = hazards.setdefault('opponent' if 'opposing' in text else 'player', {})
                layers = min(side.get(hazard, 0) + 1, HAZARD_LAYERS[hazard]) if laid else 0
                if layers:
                    side[hazard] = layers
                else:
                    side.pop(hazard, None)
                break
    alive = {occupant_key(after, a) for a in after['actives'] if a['alive']}
    was = {occupant_key(before, a) for a in before['actives'] if a['alive']} if before else set()
    for key in alive:
        if key not in was or key not in entered:
            entered[key] = after['turn']
    for table in (entered, volatiles):
        for key in [key for key in table if key not in alive]:
            del table[key]
    flags = [f'weather|{name}' for name in after['weather'] if name != 'none']
    flags += [f'field|{name}' for name in after['field'] if name != 'none']
    flags += [f'side:{side}|{name}' for side, names in after['sides'].items() for name in names]
    if after['terrain'] != 'none':
        flags.append(f"terrain|{after['terrain']}")
    for label in flags:
        since.setdefault(label, turn_resolved)
    for label in [label for label in since if label not in flags]:
        del since[label]


def status_text(status):
    out = []
    for entry in status:
        if entry.startswith('sleep:'):
            out.append(f"SLP({entry.split(':')[1]})")
        elif entry in STATUS_SHORT:
            out.append(STATUS_SHORT[entry])
    return (' ' + '/'.join(out)) if out else ''


def stages_text(stages):
    raised = [f'{STAT_SHORT[name]}{value:+d}' for name, value in stages.items()
              if value and name in STAT_SHORT]
    return (' [' + ' '.join(raised) + ']') if raised else ''


def types_text(types):
    return '/'.join(dict.fromkeys(pretty(t) for t in types if t not in ('TYPE_MYSTERY', 'TYPE_NONE')))


def targets_text(move, actor):
    kind = move['target_type']
    if kind in FIXED_TARGET_MOVES:
        return f"@{move['targets'][0]} ({TARGET_LABELS.get(kind, kind)})" if move['targets'] else ''
    foes = [t for t in move['targets'] if t % 2 != actor % 2]
    ally = [t for t in move['targets'] if t % 2 == actor % 2 and t != actor]
    text = ('@' + '/'.join(map(str, foes))) if foes else ''
    if ally:
        text += f" (ally @{ally[0]})"
    return text or ('@' + '/'.join(map(str, move['targets'])))


def render_brief(session, state, messages, heading):
    """The compact per-turn view an agent reads (~1k tokens).

    Player Pokemon show exact HP; foes show the battle's percentage, revealed
    items/abilities only, and moves seen. Everything is what the battle has
    shown the player; the opposing pending action is never included."""
    track = session.meta.get('brief', {})
    revealed, volatiles = track.get('revealed', {}), track.get('volatiles', {})
    entered, since = track.get('entered', {}), track.get('since', {})
    hazards = track.get('hazards', {})
    kinds = state['battle_type']
    doubles = 'BATTLE_TYPE_DOUBLE' in kinds
    form = 'Doubles' if doubles else 'Singles'
    if 'BATTLE_TYPE_INGAME_PARTNER' in kinds or 'BATTLE_TYPE_MULTI' in kinds:
        form += ' multi (AI partner)'
    foes = ' & '.join(pretty(name) for name in (session.meta.get('trainer_a'),
                                                 session.meta.get('trainer_b'))
                      if name and name != 'TRAINER_NONE') or pretty(session.meta.get('battle_kind'))
    lines = [f"== {heading} | now turn {state['turn']} {state['phase']} | {form} vs {foes} | "
             f"{DIFFICULTY_NAMES.get(state['difficulty'], state['difficulty'])} cap {state['level_cap']}"]
    if messages is None:
        lines.append('Log: nothing new (state never advances; start/act printed every line).')
    else:
        text = [clean_text(message) for message in messages if clean_text(message)]
        lines.append('Log:' + (''.join('\n  ' + line for line in text) if text else ' (no new lines)'))
    if state['phase'] == 'ended':
        lines.append(f"BATTLE OVER: {state['outcome'].upper()} (your faints {state['player_faints']}, "
                     f"foe faints {state['opponent_faints']}). Run `result`.")
        return '\n'.join(lines)

    field = []
    for name in state['weather']:
        if name != 'none':
            field.append(f"{pretty(name)} since T{since.get('weather|' + name, '?')}")
    if state['terrain'] != 'none':
        left = state['terrain_turns']
        field.append(f"{state['terrain'].title()} Terrain "
                     + (f'({left} turns left)' if left else '(permanent)'))
    for name in state['field']:
        if name != 'none':
            nominal = NOMINAL_TURNS.get(name)
            field.append(f"{pretty(name)} since T{since.get('field|' + name, '?')}"
                         + (f' (nominal {nominal}t)' if nominal else ''))
    for side, label in (('player', 'yours'), ('opponent', 'foe')):
        for name in state['sides'].get(side, []):
            nominal = NOMINAL_TURNS.get(name)
            field.append(f"{pretty(name)}[{label}] since T{since.get(f'side:{side}|' + name, '?')}"
                         + (f' (nominal {nominal}t)' if nominal else ''))
        for hazard, layers in sorted(hazards.get(side, {}).items()):
            field.append(f"{hazard}{' x' + str(layers) if layers > 1 else ''}[{label}]")
    lines.append('Field: ' + (' | '.join(field) if field else 'clear'))

    lines.append('Active:')
    for active in sorted(state['actives'], key=lambda a: a['battler']):
        battler, key = active['battler'], occupant_key(state, active)
        mine = active['side'] == 'player'
        who = ('YOU' if active['agent_controlled'] else 'PARTNER') if mine else 'FOE'
        if not active['alive']:
            gone = 'fainted' if active['hp'] == 0 and active['species'] not in (None, 'SPECIES_NONE') else 'empty'
            lines.append(f"  [{battler}] {who} {pretty(active['species'])} {gone}")
            continue
        extra = volatiles.get(key, [])
        tail = (f" | {', '.join(extra)}" if extra else '') + f" | in since T{entered.get(key, '?')}"
        if mine:
            health = f"{active['hp']}/{active['max_hp']}"
            gear = f"@{pretty(active['item'])} {pretty(active['ability'])}"
            if active.get('illusion_disguise'):
                gear += f" (disguised as {pretty(active['illusion_disguise'])})"
            pending = any(entry['battler'] == battler for entry in state['pending_decision'])
            if not pending and active['moves']:
                gear += ' | ' + ', '.join(f"{pretty(m['move'])} {m['pp']}pp" for m in active['moves'])
        else:
            percent = 0 if not active['hp'] else max(1, round(100 * active['hp'] / max(active['max_hp'], 1)))
            health = f'{percent}%'
            shown = revealed.get(key, {})
            item = shown.get('item')
            item_text = '?' if not item else pretty(item) + (' (used/lost)' if active['item'] != item else '')
            gear = f"item {item_text}, ability {pretty(shown['ability']) if shown.get('ability') else '?'}"
            seen = [pretty(m['move']) for m in active['moves']]
            gear += ' | seen: ' + (', '.join(seen) if seen else '-')
        mega = ' MEGA' if active['mega_evolved'] else ''
        lines.append(f"  [{battler}] {who} {pretty(active['species'])}{mega} L{active['level']} {health}"
                     f"{status_text(active['status'])}{stages_text(active['stat_stages'])} "
                     f"{types_text(active['types'])} | {gear}{tail}")

    on_field = {a['party_slot'] for a in state['actives']
                if a['side'] == 'player' and a['agent_controlled'] and a['alive']}
    bench = []
    for mon in state['player_reserves']:
        if mon['slot'] in on_field:
            continue
        condition = 'FNT' if not mon['hp'] else f"{mon['hp']}/{mon['max_hp']}{status_text(mon['status'])}"
        bench.append(f"{mon['slot']} {pretty(mon['species'])} {condition} @{pretty(mon['item'])}")
    lines.append('Your bench: ' + ('; '.join(bench) if bench else 'none'))
    party, unseen = [], 0
    for mon in state['opponent_party']:
        if not mon['revealed']:
            unseen += 1
            continue
        party.append(f"{pretty(mon['species'])} L{mon['level']}{' FNT' if mon['fainted'] else ''}"
                     + (f" ({mon['owner']})" if 'BATTLE_TYPE_TWO_OPPONENTS' in kinds else ''))
    lines.append('Foe party: ' + (', '.join(party) or '-') + f'; unseen {unseen}')

    names = {mon['slot']: pretty(mon['species']) for mon in state['player_reserves']}
    actives = {a['battler']: a for a in state['actives']}
    lines.append('Decide:' if state['pending_decision'] else 'Decide: nothing pending')
    for entry in state['pending_decision']:
        battler = entry['battler']
        now = '' if entry.get('awaiting_now', True) else ' (include in this call)'
        switches = ', '.join(f"switch{slot} {names.get(slot, slot)}" for slot in entry['switch_slots'])
        if entry['replacing']:
            lines.append(f"  {battler}: replace fainted {pretty(entry.get('fainted_species'))}{now}"
                         f" -> {switches or 'no reserve'}")
            continue
        pp = {m['index']: m.get('pp') for m in actives[battler]['moves']}
        options = []
        for move in entry['moves']:
            label = f"move{move['index']} {pretty(move['move'])} {pp.get(move['index'], '?')}pp"
            if not move['legal']:
                reasons = ','.join(b.replace('MOVE_LIMITATION_', '').lower() for b in move['blocked_by'])
                options.append(f'{label} BLOCKED({reasons or "blocked"})')
            else:
                options.append(f"{label} {targets_text(move, battler)}")
        if entry['must_struggle']:
            options = [f'{battler}:struggle only']
        lines.append(f"  {battler} {pretty(entry['species'])}{now}: " + ' | '.join(options))
        if entry['switch_blocked_by']:
            switch_text = f"switch blocked ({entry['switch_blocked_by'][0]})"
        elif entry['may_switch']:
            switch_text = switches or 'no reserve to switch to'
        else:
            switch_text = 'cannot switch'
        active = actives[battler]
        if active['usable_gimmick'] == 'mega' and not active['mega_already_used']:
            switch_text += ' | Mega available: append ,mega to a move'
        lines.append(f'     {switch_text}')
    yours = '0/2' if doubles else '0'
    lines.append(f"Commands (one act call, one per battler under Decide): B:move<i>@<target>[,mega] | "
                 f"B:switch<slot> | B:struggle. Your battlers {yours}, foes {'1/3' if doubles else '1'}.")
    return '\n'.join(lines)


# ----------------------------------------------------------- witness replay

# Timing and private audit fields; everything else in an act record must match.
REPLAY_IGNORED = {'frames', 'ai_decision_frames', 'ai_setup_frames', 'ai_delay_frames',
                  'audit_pre_commit_opponent_latch'}


def commands_of(submitted):
    """The act command strings that produced an act record's `submitted`."""
    commands = []
    for battler, command in submitted.items():
        if command['action'] == 'switch':
            commands.append(f"{battler}:switch{command['slot']}")
        elif 'index' not in command:
            commands.append(f'{battler}:struggle')
        else:
            commands.append(f"{battler}:move{command['index']}@{command['target']}"
                            + (',mega' if command.get('mega') else ''))
    return commands


def replay_start_args(meta, source, target):
    """(start arguments, basis, party path, scenario path) for replaying `source`."""
    recorded = meta.get('start_args')
    if recorded:
        arguments, basis = dict(recorded), 'recorded start arguments'
    else:
        basis = 'start arguments rebuilt from session.json (run predates recorded arguments)'
        kind = meta.get('battle_kind', 'trainer')
        arguments = {'battle_kind': kind, 'trainer': meta['trainer_a'] if kind == 'trainer' else None,
                     'trainer2': None, 'partner': None, 'difficulty': meta['difficulty'],
                     'cap': meta['level_cap'], 'baseline': None, 'map': None, 'weather': 'map'}
        if not meta.get('script_pairing'):
            arguments['trainer2'] = meta.get('trainer_b')
            if meta.get('partner') not in (None, 'PARTNER_NONE'):
                arguments['partner'] = meta['partner']
        weather_basis = (meta.get('map_field') or {}).get('weather_basis', '')
        if weather_basis.startswith('--weather '):
            arguments['weather'] = weather_basis.split()[1]
    arguments['weather'] = arguments.get('weather') or 'map'
    arguments.setdefault('battle_kind', meta.get('battle_kind', 'trainer'))
    party = None
    if meta.get('party_sha256'):
        copy = source / meta.get('party_copy', PARTY_COPY)
        party = copy if copy.is_file() else Path(meta.get('party_manifest') or '')
        if not party.is_file() or hashlib.sha256(party.read_bytes()).hexdigest() != meta['party_sha256']:
            fail('the original party manifest is missing or changed since that battle started')
    scenario = None
    if meta.get('scenario') is not None:
        copy = source / meta.get('scenario_copy', SCENARIO_COPY)
        if copy.is_file():
            scenario = copy
        else:
            scenario = target / 'replay-scenario.json'
            scenario.write_text(json.dumps(meta['scenario'], indent=2) + '\n')
    return arguments, basis, party, scenario


def command_replay(args):
    """Re-run a finished run's seed, party and command log in a fresh run
    directory, on the original run's own pinned ROM/ELF, and report whether
    every act record and the outcome are identical: the witness check."""
    source, target = Path(args.source).resolve(), Path(args.run_dir).resolve()
    if not (source / 'session.json').is_file() or not (source / 'events.jsonl').is_file():
        fail(f'{source} is not a battle run directory')
    if (target / 'session.json').exists():
        fail('use a fresh --run-dir for the replay')
    meta = json.loads((source / 'session.json').read_text())
    acts = [record for record in map(json.loads, (source / 'events.jsonl').read_text().splitlines())
            if record.get('event') == 'act']
    if not acts:
        fail('the original run has no act records to replay')
    target.mkdir(parents=True, exist_ok=True)
    build = target / 'witness-build'
    build.mkdir(exist_ok=True)
    for name, pinned in (('scene.gba', 'pokeemerald-headless.gba'),
                         ('scene.elf', 'pokeemerald-headless.elf'),
                         ('inputs.json', 'pokeemerald-headless.inputs.json'),
                         ('scene.provenance.json', 'pokeemerald-headless.provenance.json')):
        if (source / name).is_file():
            clone_file(source / name, build / pinned)
    arguments, basis, party, scenario = replay_start_args(meta, source, target)
    start = SimpleNamespace(**arguments, seed=meta['seed'], run_dir=str(target),
                            party=str(party) if party else None,
                            scenario=str(scenario) if scenario else None,
                            build_dir=str(build), png=False, brief=False)
    failure, step = None, 'start'
    with contextlib.redirect_stdout(io.StringIO()):
        try:
            command_start(start)
            for index, record in enumerate(acts):
                step = f'act {index}'
                command_act(SimpleNamespace(commands=commands_of(record['submitted']),
                                            run_dir=str(target), png=False, brief=False))
        except SystemExit as error:
            failure = {'step': step, 'reason': str(error)}
    replayed = []
    if (target / 'events.jsonl').is_file():
        replayed = [record for record in map(json.loads, (target / 'events.jsonl').read_text().splitlines())
                    if record.get('event') == 'act']
    semantic = lambda record: {key: value for key, value in record.items() if key not in REPLAY_IGNORED}
    divergence = failure
    for index, (original, again) in enumerate(zip(acts, replayed)):
        if divergence:
            break
        left, right = semantic(original), semantic(again)
        if left != right:
            divergence = {'step': f'act {index}', 'fields': sorted(
                key for key in set(left) | set(right) if left.get(key) != right.get(key))}
    if not divergence and len(replayed) != len(acts):
        divergence = {'step': f'act {len(replayed)}', 'reason': 'replay produced a different number of acts'}
    rosters = [path / 'opponent-roster.json' for path in (source, target)]
    identical = divergence is None
    report = {
        'witness_replay': 'identical' if identical else 'DIVERGED',
        'identical': identical,
        'winning_witness': identical and acts[-1].get('outcome') == 'won',
        'acts_original': len(acts), 'acts_replayed': len(replayed),
        'outcome_original': acts[-1].get('outcome'),
        'outcome_replay': replayed[-1].get('outcome') if replayed else None,
        'first_divergence': divergence,
        'opponent_roster_identical': (rosters[0].read_bytes() == rosters[1].read_bytes()
                                      if all(path.is_file() for path in rosters) else None),
        'start_arguments': basis,
        'level_delta': (meta.get('level_delta') or {}).get('applied', {}),
        'ability_trial': (meta.get('ability_trial') or {}).get('applied', {}),
        'build': "the original run's pinned ROM/ELF (" + str(meta.get('rom_sha256', ''))[:12] + ')',
        'original_run': str(source), 'replay_run': str(target),
    }
    (target / 'witness.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
    if not identical:
        raise SystemExit(1)


BRIEF_HELP = ('print the compact per-turn player view (~1k tokens) instead of JSON')


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    subparsers = parser.add_subparsers(dest='command', required=True)

    start = subparsers.add_parser('start')
    start.add_argument('--battle-kind', choices=list(BATTLE_KINDS), default='trainer',
                       help='only concrete native trainer/rescue/Deoxys entries; no generic wild fabrication')
    start.add_argument('--trainer')
    start.add_argument('--trainer2', help='second owner; normally taken from the map script')
    start.add_argument('--partner', help='PARTNER_* for a multi; normally from the map script')
    start.add_argument('--party', help='validated preparation manifest; omit for actual level-five rescue starters')
    start.add_argument('--seed', type=lambda v: int(v, 0), required=True)
    start.add_argument('--difficulty', default='medium',
                       choices=['easy', 'medium', 'normal', 'hard'])
    start.add_argument('--cap', type=int, required=True, help='campaign player level cap')
    start.add_argument('--baseline', type=int, help='opponent campaign milestone baseline, '
                       'when difficulty changes the post-League player cap')
    start.add_argument('--scenario', help='explicit source scenario flags/vars; the caller '
                       'must separately validate reachability')
    start.add_argument('--run-dir', required=True)
    start.add_argument('--build-dir',
                       help='directory holding a stamped headless ROM/ELF/inputs triple')
    start.add_argument('--map', help='map the fight happens on; normally found from the map '
                                     'script that starts the battle')
    start.add_argument('--weather', default='map',
                       help='overworld weather to open under: map (default: the trainer\'s own '
                            'map, tile and region), none, rain, downpour, thunderstorm, sun, '
                            'sandstorm, snow, fog, or a WEATHER_* name')
    start.add_argument('--level-delta', dest='level_delta',
                       help='opposing per-member level deltas in AUTHORED member order, applied '
                            'after the difficulty formula: e.g. A0=+2,A3=-1,B1=+1; A*=+1 sets all '
                            'of owner A (explicit members override a wildcard)')
    start.add_argument('--ability-trial', dest='ability_trial',
                       help='opposing per-member trial abilities in AUTHORED member order: e.g. '
                            'A2=ABILITY_RAMPAGE,B0=ABILITY_SNOW_WARNING; refused natively (and the '
                            'start fails) when not legal for that species')
    start.add_argument('--png', action='store_true')
    start.add_argument('--brief', action='store_true', help=BRIEF_HELP)
    start.set_defaults(func=command_start)

    for name, func in (('state', command_state), ('result', command_result)):
        sub = subparsers.add_parser(name)
        sub.add_argument('--run-dir', required=True)
        sub.add_argument('--png', action='store_true')
        if name == 'state':
            sub.add_argument('--brief', action='store_true', help=BRIEF_HELP)
        sub.set_defaults(func=func)

    act = subparsers.add_parser('act')
    act.add_argument('commands', nargs='+')
    act.add_argument('--run-dir', required=True)
    act.add_argument('--png', action='store_true')
    act.add_argument('--brief', action='store_true', help=BRIEF_HELP)
    act.set_defaults(func=command_act)

    replay = subparsers.add_parser('replay', help='witness check: re-run a finished run in a fresh '
                                   'run directory and compare every act record and the outcome')
    replay.add_argument('--run-dir', required=True, help='fresh directory for the replay')
    replay.add_argument('--from', dest='source', required=True, help='the original run directory')
    replay.set_defaults(func=command_replay)

    args = parser.parse_args()
    args.func(args)


if __name__ == '__main__':
    main()
