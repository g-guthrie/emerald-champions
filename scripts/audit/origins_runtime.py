#!/usr/bin/env python3
"""Execute C48 observations and the authored Diancite pickup with libmGBA.

Setup is synthetic. Win/capture/loss controls exercise native script boundaries;
they do not assess combat. The native Run command is used for flee cases.
Every state directory retains its matching ROM/ELF and input stamp.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import native_tools
import render_emerald_champions_ui as ui
import run_emerald_champions_campaign as campaign
from rom_artifacts import verify_rom_elf_pair


def require(ok, message):
    if not ok:
        raise RuntimeError(message)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


class Scene:
    def __init__(self, evidence, name, param, symbols, runner, scenario, constants):
        self.out = evidence / name
        self.out.mkdir()
        for filename in ('scene.gba', 'scene.elf', 'inputs.json'):
            os.link(evidence / filename, self.out / filename)
        self.symbols, self.runner = symbols, runner
        self.scenario, self.constants = scenario, constants
        self.state = self.out / 'current.ss1'
        self.trace = []
        self.fields = ['FixtureSetupResult', 'FixtureParam', 'FixtureActiveScenario',
            'CampaignMapId', 'CampaignPlayerX', 'CampaignPlayerY', 'CampaignPlayerFacing',
            'CampaignControlsLocked', 'CampaignScriptEnabled', 'CampaignInBattle',
            'CampaignBattleSerial', 'CampaignCaptureSerial', 'CampaignLastResolution',
            'CampaignLastCaptureResult', 'CampaignCaptureBookkeepingValid',
            'CampaignQueryValue', 'CampaignQueryObjectActive']
        self.advance('entry', 300, boot=param)

    def advance(self, label, frames=48, keys=(), writes=(), boot=None, captures=()):
        i = len(self.trace)
        image_path = self.out / f'{i:03d}-{label}.png'
        next_state = self.out / 'next.ss1'
        cmd = [str(self.runner), '--rom', str(self.out / 'scene.gba'), '--rtc', '946684800',
            '--frames', str(frames), '--state-out', str(next_state), '--screenshot', str(image_path),
            '--save-out', str(self.out / 'current.sav')]
        if boot is None:
            cmd += ['--state-in', str(self.state)]
        else:
            cmd += ['--write', f'59:4:0x{self.symbols["gEcHeadlessFixtureParam"]:x}:{boot}',
                    '--write', f'60:4:0x{self.symbols["gEcHeadlessFixtureScenario"]:x}:{self.scenario}']
        for at, duration, key in keys:
            cmd += ['--key', f'{at}:{duration}:{key}']
        for name, value in writes:
            cmd += ['--write', f'0:4:0x{self.symbols["gEcHeadless" + name]:x}:{value}']
        reads = {name: (4, self.symbols['gEcHeadless' + name]) for name in self.fields}
        reads['battle_outcome'] = (1, self.symbols['gBattleOutcome'])
        reads['player_controller'] = (4, self.symbols['gBattlerControllerFuncs'])
        reads['start_cursor'] = (1, self.symbols['sStartMenuCursorPos'])
        reads['save_counter'] = (4, self.symbols['gSaveCounter'])
        for word in range(4):
            reads[f'start_order{word}'] = (4, self.symbols['sCurrentStartMenuActions'] + word * 4)
        for slot in range(16):
            reads[f'task{slot}'] = (4, self.symbols['gTasks'] + slot * 40)
            reads[f'active{slot}'] = (1, self.symbols['gTasks'] + slot * 40 + 4)
        for width, address in reads.values():
            cmd += ['--read', f'{width}:0x{address:x}']
        for at, tag in captures:
            cmd += ['--screenshot-at', f'{at}:{self.out / f"{i:03d}-{label}-{tag}.png"}']
        result = ui.run(cmd)
        next_state.replace(self.state)
        values = {int(a, 16): int(v, 16) for a, v in re.findall(
            r'READ width=\d+ address=([0-9a-f]+) value=([0-9a-f]+)', result.stdout)}
        telemetry = {name: values[address] for name, (_, address) in reads.items()}
        yesno = self.symbols['Task_HandleYesNoInput'] & ~1
        telemetry['yesno'] = any(telemetry[f'active{s}'] and (telemetry[f'task{s}'] & ~1) == yesno for s in range(16))
        telemetry['levitate_active'] = any(telemetry[f'active{s}'] and (telemetry[f'task{s}'] & ~1)
            == (self.symbols['ApplyLevitateMovement'] & ~1) for s in range(16))
        # Several controller translation units use this same static symbol.
        # Inspect the player's live callback against every linked definition.
        telemetry['choose_action'] = (telemetry['player_controller'] & ~1) in self.symbols['_action_handlers']
        telemetry['campaign_telemetry_live'] = telemetry['FixtureActiveScenario'] == self.scenario
        cursor = telemetry['start_cursor']
        telemetry['start_action'] = ((telemetry[f'start_order{cursor // 4}'] >> (8 * (cursor % 4))) & 255) if cursor < 16 else -1
        telemetry = {name: value for name, value in telemetry.items() if not name.startswith(('task', 'active'))}
        self.trace.append({'label': label, 'frames': frames, 'keys': keys, 'writes': writes,
                           'telemetry': telemetry, 'screenshot': image_path.name})
        (self.out / 'trace.json').write_text(json.dumps(self.trace, indent=2) + '\n')
        self.last = telemetry
        return telemetry

    def key(self, key, label=None, frames=48):
        return self.advance(label or key.lower(), frames, keys=[(0, 2, key)],
            captures=[(16, 'frame16'), (32, 'frame32')] if label == 'observation' else ())

    def walk(self, key, label, frames=48):
        return self.advance(label, frames, keys=[(0, 16, key)])

    def face(self, key, label):
        direction = {'UP': 'DIR_NORTH', 'DOWN': 'DIR_SOUTH', 'LEFT': 'DIR_WEST', 'RIGHT': 'DIR_EAST'}[key]
        if self.last['CampaignPlayerFacing'] != self.constants[direction]:
            self.key(key, label)

    def stable(self):
        return not any(self.last[name] for name in ('CampaignControlsLocked', 'CampaignScriptEnabled', 'CampaignInBattle'))

    def query(self, kind, ident=0):
        value = self.constants.get(ident, ident)
        return self.advance(f'query-{kind}-{ident}', 2,
            writes=[('CampaignQueryKind', kind), ('CampaignQueryId', value)])['CampaignQueryValue']

    def menu(self):
        for _ in range(80):
            if self.last['yesno']:
                return
            require(not self.last['CampaignInBattle'], 'unexpected battle before accepting the offer')
            self.key('A', 'observation')
        raise RuntimeError('battle offer never appeared')

    def settle(self, label='resolved'):
        for _ in range(160):
            if self.stable():
                return
            self.key('B' if self.last['yesno'] else 'A', label)
        raise RuntimeError('native scene did not release field controls')

    def observe(self):
        self.key('UP', 'face-actor')
        self.menu()

    def start_action(self, action):
        self.key('START', 'start-menu')
        for _ in range(16):
            if self.last['start_action'] == self.symbols['_start_actions'].index(action):
                if action == 'MENU_ACTION_SAVE':
                    # Keep the native flash transaction in one emulator run.
                    # Recreating the core from snapshots during flash writes can
                    # strand the emulated busy state; it is not a game Retry.
                    self.advance('native-save-uninterrupted', 1200,
                        keys=[(at,2,'A') for at in (0,120,240,360)])
                else:
                    self.key('A', action.lower())
                self.settle(action.lower() + '-confirm')
                return
            self.key('DOWN', 'start-menu-row')
        raise RuntimeError(action + ' missing from native Start menu')

    def origins_roundtrip(self, area):
        """Leave and return through the actual chamber doorway after native Run."""
        origin = area == 'origin'
        room = self.symbols['_maps']['MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM' if origin else 'MAP_METEOR_FALLS_JIRACHIS_ROOM']
        for _ in range(6):
            self.walk('UP' if origin else 'DOWN', 'leave-for-retry', 100)
            if self.last['CampaignMapId'] != room:
                break
        require(self.last['CampaignMapId'] != room, 'retry did not leave the chamber')
        self.walk('DOWN', 'step-away-from-door', 100)
        for _ in range(4):
            self.walk('UP', 'return-through-door', 100)
            if self.last['CampaignMapId'] == room:
                break
        require(self.last['CampaignMapId'] == room, 'retry doorway did not return to chamber')
        target_y = 8 if origin else 7
        for _ in range(6):
            y = self.last['CampaignPlayerY']
            if y == target_y:
                break
            self.walk('DOWN' if y < target_y else 'UP', 'retry-approach', 100)
        require(self.last['CampaignPlayerY'] == target_y, 'retry did not reach actor')
        self.face('DOWN' if origin else 'UP', 'face-returned-actor')

    def save_reload(self):
        before_save = self.last['save_counter']
        position = (self.last['CampaignPlayerX'], self.last['CampaignPlayerY'])
        self.start_action('MENU_ACTION_SAVE')
        require(self.last['save_counter'] > before_save, 'native Save did not write a new save')
        self.walk('DOWN', 'walk-after-completed-save')
        require((self.last['CampaignPlayerX'], self.last['CampaignPlayerY']) != position, 'reload control did not move')
        self.start_action('MENU_ACTION_RELOAD_SAVE')
        self.advance('native-completed-reload', 300, writes=[('FixtureActiveScenario', self.scenario)])
        require((self.last['CampaignPlayerX'], self.last['CampaignPlayerY']) == position, 'native Reload lost saved position')

    def shoal_roundtrip(self, label):
        require((self.last['CampaignPlayerX'], self.last['CampaignPlayerY']) == (12, 12), 'unexpected Shoal pickup approach')
        for step in range(9):
            self.walk('LEFT', label + '-exit-corridor', 100 if step == 8 else 48)
        require(self.last['CampaignMapId'] == self.symbols['_maps']['MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM'], 'Shoal side pickup blocked exit')
        require((self.last['CampaignPlayerX'], self.last['CampaignPlayerY']) == (38, 15), 'wrong Shoal exit anchor')
        self.walk('RIGHT', label + '-step-off-ladder')
        self.walk('LEFT', label + '-return-ladder', 100)
        require(self.last['CampaignMapId'] == self.symbols['_maps']['MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM'], 'Shoal return ladder failed')
        for step in range(9):
            self.walk('RIGHT', label + '-return-corridor')
        require((self.last['CampaignPlayerX'], self.last['CampaignPlayerY']) == (12, 12), 'wrong return to pickup corner')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--case', nargs='+', choices=['decline', 'reverse', 'missing', 'fallback', 'win', 'capture', 'full-storage', 'loss', 'flee', 'flee-retry', 'win-reload', 'tickets-full-retry', 'devon', 'league', 'champion', 'save-reload', 'diancite', 'diancite-full', 'diancite-duplicate', 'diancite-claimed', 'meteor-door', 'meteor-door-full'])
    args = p.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=False)
    rom, elf = ROOT / 'pokeemerald-headless.gba', ROOT / 'pokeemerald-headless.elf'
    verify_rom_elf_pair(rom, elf)
    stamp = ROOT / 'pokeemerald-headless.inputs.json'
    inputs = json.loads(stamp.read_text())
    require(inputs['artifacts']['pokeemerald-headless.gba'] == digest(rom)
        and inputs['artifacts']['pokeemerald-headless.elf'] == digest(elf), 'input stamp does not bind this artifact pair')
    shutil.copy2(rom, out / 'scene.gba')
    shutil.copy2(elf, out / 'scene.elf')
    shutil.copy2(stamp, out / 'inputs.json')
    shutil.copy2(Path(__file__), out / 'origins_runtime.py')
    source_base = native_tools.run(['git', '-C', str(ROOT), 'rev-parse', 'HEAD']).stdout.strip()
    (out / 'source.patch').write_text(native_tools.run(['git', '-C', str(ROOT), 'diff', '--binary', 'HEAD']).stdout)
    symbols = native_tools.symbols(elf, ROOT)
    symbol_text = native_tools.run([native_tools.find_nm(ROOT), '-S', str(elf)]).stdout
    symbols['_action_handlers'] = [int(row.split()[0], 16) & ~1 for row in symbol_text.splitlines()
        if row.split() and row.split()[-1] == 'HandleInputChooseAction']
    start_enum = (ROOT / 'src/start_menu.c').read_text().split('// Menu actions', 1)[1].split('};', 1)[0]
    symbols['_start_actions'] = re.findall(r'MENU_ACTION_\w+', start_enum)
    symbols['_maps'] = campaign.parse_map_ids()
    runner = ui.build_runner()
    enum = (ROOT / 'include/emerald_champions_headless.h').read_text().split('enum EmeraldChampionsHeadlessScenario', 1)[1].split('};', 1)[0]
    scenario = re.findall(r'EC_HEADLESS_SCENARIO_\w+', enum).index('EC_HEADLESS_SCENARIO_BOOK_RESEARCH')
    constants = campaign.parse_numeric_constants()
    direction_enum = (ROOT / 'include/constants/global.h').read_text().split('Direction\n{', 1)[1].split('CARDINAL_DIRECTION_COUNT', 1)[0]
    constants.update({name: value for value, name in enumerate(re.findall(r'DIR_\w+', direction_enum))})
    constants['B_OUTCOME_RAN'] = int(re.search(r'^#define B_OUTCOME_RAN\s+(\d+)',
        (ROOT / 'include/constants/battle.h').read_text(), re.MULTILINE)[1])
    result = {'status': 'running', 'rom_sha256': digest(rom), 'elf_sha256': digest(elf),
        'source_base': source_base, 'source_patch_sha256': digest(out / 'source.patch'),
        'driver_sha256': digest(Path(__file__)), 'runner_sha256': digest(runner),
        'scope': 'Synthetic setup; real scripts, menus and map exits. Forced win/capture/loss are exit-boundary evidence. Flee uses native Run. Human visual acceptance and fresh-save traversal are not claimed.', 'cases': []}
    cases = args.case if args.case else ['decline', 'reverse', 'missing', 'fallback', 'win', 'capture', 'full-storage', 'loss', 'flee', 'flee-retry', 'win-reload', 'tickets-full-retry', 'devon', 'league', 'champion', 'save-reload', 'diancite', 'diancite-full', 'diancite-duplicate', 'diancite-claimed', 'meteor-door', 'meteor-door-full']
    try:
        for area, base, flag, other, caught in [
            ('origin', 21, 'FLAG_EC_SURVEYED_ORIGIN_CHAMBER', 'FLAG_EC_SURVEYED_METEOR_CHAMBER', 'FLAG_EC_CAUGHT_DIANCIE'),
            ('meteor', 22, 'FLAG_EC_SURVEYED_METEOR_CHAMBER', 'FLAG_EC_SURVEYED_ORIGIN_CHAMBER', 'FLAG_EC_CAUGHT_JIRACHI')]:
            for case in cases:
                if case.startswith(('diancite', 'meteor-door', 'tickets-')):
                    continue
                param = base | {'reverse': 0x1000, 'missing': 0x200, 'capture': 0x800,
                    'full-storage': 0x100, 'devon': 0x3000, 'league': 0x5000,
                    'champion': 0x9000, 'flee': 0x10000, 'flee-retry': 0x10000}.get(case, 0)
                if case == 'fallback':
                    param = base + 2
                s = Scene(out, area + '-' + case, param, symbols, runner, scenario, constants)
                require(s.stable(), 'fixture entry is not stable')
                require(s.query(1, flag) == 0, 'room entry recorded an observation')
                require(s.query(9) == 6000, 'fixture starting money differs')
                if case == 'fallback':
                    s.walk('UP' if area == 'origin' else 'DOWN', 'entrance-prompt')
                    s.menu()
                    s.key('B', 'decline-observation')
                    s.settle()
                    require(s.query(1, flag) == 0, 'declining recorded observation')
                    s.walk('DOWN' if area == 'origin' else 'UP', 'leave-prompt-tile')
                    s.walk('UP' if area == 'origin' else 'DOWN', 'recross-prompt-tile')
                    require(s.stable(), 'declined fallback retriggered in same visit')
                    # Exit and re-enter via the actual adjacent warp.
                    s.walk('UP' if area == 'origin' else 'DOWN', 'exit-chamber', 100)
                    s.walk('DOWN', 'step-off-return-warp', 100)
                    s.walk('UP', 'return-chamber', 100)
                    if s.last['CampaignPlayerY'] == (6 if area == 'origin' else 10):
                        s.walk('DOWN' if area == 'origin' else 'UP', 'return-entrance', 40)
                    s.menu()
                    s.key('A', 'accept-observation')
                    s.settle()
                elif case == 'missing':
                    s.face('UP', 'face-blocked-actor')
                    s.key('A', 'badge-prerequisite')
                    s.settle('badge-prerequisite-return')
                    require(s.last['CampaignBattleSerial'] == 0, 'missing badge started a battle')
                else:
                    s.observe()
                    require(s.query(1, flag) == (case != 'missing'), 'wrong observation at battle offer')
                    require(s.query(1, other) == (case in ['reverse', 'devon', 'league', 'champion']), 'observation changed the other chamber')
                    if case in ['decline', 'reverse', 'missing', 'full-storage', 'devon', 'league', 'champion', 'save-reload']:
                        s.key('B', 'decline-battle')
                        s.settle()
                        require(s.last['CampaignBattleSerial'] == 0, 'decline started a battle')
                        s.observe()
                        s.key('B', 'repeat-decline')
                        s.settle()
                        if case == 'save-reload':
                            before_save = s.last['save_counter']
                            saved_position = (s.last['CampaignPlayerX'], s.last['CampaignPlayerY'])
                            s.start_action('MENU_ACTION_SAVE')
                            require(s.last['save_counter'] > before_save, 'native Save did not write a new save')
                            s.walk('DOWN', 'walk-after-save')
                            require((s.last['CampaignPlayerX'], s.last['CampaignPlayerY']) != saved_position, 'save-reload control did not move')
                            s.start_action('MENU_ACTION_RELOAD_SAVE')
                            # ReloadSave legitimately clears EWRAM, including test
                            # telemetry. Re-arm observation without a fixture boot.
                            s.advance('native-reload-return', 300, writes=[
                                ('FixtureActiveScenario', scenario), ('FixtureParam', param)])
                            require((s.last['CampaignPlayerX'], s.last['CampaignPlayerY']) == saved_position, 'native Reload did not restore saved position')
                            require(s.query(1, flag) == 1, 'native Reload lost the observation')
                    else:
                        if case == 'loss':
                            s.advance('synthetic-loss-control', 2, writes=[('CampaignForceLoss', 1)])
                        if case in ('flee', 'flee-retry'):
                            s.advance('native-battle-control', 2, writes=[('FixtureActiveScenario', 0)])
                        s.key('A', 'accept-battle')
                        if case in ('flee', 'flee-retry'):
                            s.advance('native-command-menu', 600)
                            for _ in range(40):
                                if s.last['choose_action']:
                                    break
                                s.key('A', 'native-battle-intro')
                            require(s.last['choose_action'], 'native command menu did not become ready')
                            s.key('RIGHT', 'run-column')
                            s.key('DOWN', 'run-row')
                            s.key('A', 'native-run')
                            s.advance('run-outcome', 180)
                            require(s.last['battle_outcome'] == constants['B_OUTCOME_RAN'], 'native Run did not flee')
                            s.advance('restore-telemetry', 2, writes=[('FixtureActiveScenario', scenario)])
                        s.settle()
                        require(s.query(1, flag) == 1, 'battle exit revoked observation')
                        require(s.query(1, caught) == (case == 'capture'), 'wrong caught flag after battle')
                resolved = 'FLAG_EC_RESOLVED_DIANCIE' if area == 'origin' else 'FLAG_EC_RESOLVED_JIRACHI'
                if case == 'flee-retry':
                    require(s.query(1, resolved) == 0, 'flee completed the required challenge')
                    s.origins_roundtrip(area)
                    s.menu()
                    s.key('A', 'retry-battle-after-native-flee')
                    s.settle('retry-victory')
                    require(s.query(1, resolved) == 1, 'returned encounter could not resolve')
                if case == 'win-reload':
                    require(s.query(1, resolved) == 1, 'victory did not resolve the required challenge')
                    s.save_reload()
                    require(s.query(1, resolved) == 1, 'native Reload revoked the completed challenge')
                if case in ('flee', 'loss', 'decline', 'save-reload', 'missing'):
                    require(s.query(1, resolved) == 0, 'unsuccessful encounter completed the challenge')
                require(s.query(1, flag) == (case != 'missing'), 'wrong final observation flag')
                require(not s.last['levitate_active'], 'awakening left its movement task running')
                if case != 'loss':
                    require(s.query(9) == 6000, 'observation paid an unexpected reward')
                result['cases'].append({'name': area + '-' + case, 'status': 'pass', 'last': s.last})
                print('PASS:', area, case, flush=True)
        if 'tickets-full-retry' in cases:
            s = Scene(out, 'tickets-full-retry', 28 | 0x100, symbols, runner, scenario, constants)
            papers = [('SS_TICKET','FLAG_RECEIVED_SS_TICKET','FLAG_EC_EARNED_SS_TICKET'),
                      ('EON_TICKET','FLAG_EC_RECEIVED_EON_TICKET','FLAG_EC_EARNED_EON_TICKET'),
                      ('OLD_SEA_MAP','FLAG_RECEIVED_OLD_SEA_MAP','FLAG_EC_EARNED_OLD_SEA_MAP'),
                      ('AURORA_TICKET','FLAG_RECEIVED_AURORA_TICKET','FLAG_EC_EARNED_AURORA_TICKET'),
                      ('MYSTIC_TICKET','FLAG_RECEIVED_MYSTIC_TICKET','FLAG_ENABLE_SHIP_NAVEL_ROCK')]
            s.face('UP', 'face-center-nurse')
            s.menu()
            s.key('A', 'heal-despite-full-key-pocket')
            s.settle('full-bag-healing')
            for item, received, earned in papers:
                require(s.query(4,'ITEM_'+item) == 0 and s.query(1,received) == 0, 'full pocket consumed document receipt')
                require(s.query(1,earned) == 1, 'full pocket revoked earned passage')
            s.advance('synthetic-room-in-key-pocket', 2, writes=[('FixtureTrigger',2)])
            s.menu()
            s.key('A', 'heal-after-pending-delivery')
            s.settle('pending-delivered')
            s.save_reload()
            s.face('UP','face-nurse-after-reload')
            s.menu()
            s.key('A','repeat-healing')
            s.settle('repeat-no-duplicate-documents')
            for item, received, earned in papers:
                require(s.query(4,'ITEM_'+item) == 1 and s.query(1,received) == 1, 'document was missing or duplicated after retry/reload')
                require(s.query(1,earned) == 1, 'retry/reload revoked earned passage')
            result['cases'].append({'name':'tickets-full-retry','status':'pass','last':s.last})
            print('PASS: tickets-full-retry', flush=True)
        for case in (case for case in cases if case.startswith('diancite')):
            param = 26 | {'diancite-full': 0x100, 'diancite-duplicate': 0x200,
                'diancite-claimed': 0x400}.get(case, 0)
            s = Scene(out, case, param, symbols, runner, scenario, constants)
            require(s.stable(), 'pickup approach not stable')
            claimed = case == 'diancite-claimed'
            require(s.query(3, 1) == (not claimed), 'wrong pickup visibility')
            s.shoal_roundtrip('unclaimed' if not claimed else 'claimed')
            require(s.query(3, 1) == (not claimed), 'roundtrip changed pickup visibility')
            require(s.query(9) == 6000, 'bypass triggered a reward')
            s.face('RIGHT', 'face-pickup')
            s.key('A', 'pick-up')
            s.settle('pickup-result')
            if case == 'diancite-full':
                require(s.query(1, 'FLAG_EC_MEGA_REWARD_DIANCITE') == 0, 'full pocket consumed receipt')
                require(s.query(3, 1) == 1, 'full pocket removed pickup')
                require(s.query(9) == 6000, 'full pocket paid unearned money')
                s.key('A', 'full-pocket-repeat')
                s.settle('full-pocket-repeat-result')
                require(s.query(1, 'FLAG_EC_MEGA_REWARD_DIANCITE') == 0, 'repeat consumed pending receipt')
                s.advance('make-pocket-room', 2, writes=[('FixtureTrigger', 2)])
                s.key('A', 'retry-pickup')
                s.settle('retry-result')
            require(s.query(1, 'FLAG_EC_MEGA_REWARD_DIANCITE') == 1, 'pickup receipt missing')
            require(s.query(3, 1) == 0, 'claimed pickup still visible')
            expected_money = 9000 if case == 'diancite-duplicate' else 6000
            require(s.query(9) == expected_money, 'wrong finite payout')
            require(s.query(4, 'ITEM_DIANCITE') == (case not in ['diancite-duplicate', 'diancite-claimed']), 'wrong delivered Diancite quantity')
            if case == 'diancite-duplicate':
                require(s.query(5, 'ITEM_DIANCITE') == 1, 'duplicate conversion changed owned PC stone')
            s.shoal_roundtrip('after-collection')
            s.face('RIGHT', 'face-collected-anchor')
            s.key('A', 'collected-anchor-repeat')
            s.settle('collected-anchor-result')
            require(s.query(9) == expected_money, 're-entry repeated finite payout')
            require(s.query(1, 'FLAG_EC_MEGA_REWARD_DIANCITE') == 1, 're-entry lost receipt')
            result['cases'].append({'name': case, 'status': 'pass', 'last': s.last})
            print('PASS:', case, flush=True)
        for case in (case for case in cases if case.startswith('meteor-door')):
            s = Scene(out, case, 25 | (0x100 if case.endswith('full') else 0), symbols, runner, scenario, constants)
            require(s.query(3, 1) == 0, 'obsolete doorway pickup still exists')
            s.walk('UP', 'unclaimed-door-approach')
            s.walk('UP', 'unclaimed-door-entry', 100)
            require(s.last['CampaignMapId'] == symbols['_maps']['MAP_METEOR_FALLS_JIRACHIS_ROOM'], 'required Jirachi door still blocked')
            require(s.query(1, 'FLAG_EC_SURVEYED_METEOR_CHAMBER') == 0, 'door entry recorded an observation')
            require(s.query(4, 'ITEM_BEAST_BALL') == 0 and s.query(9) == 6000, 'door passage triggered an unwanted reward')
            result['cases'].append({'name': case, 'status': 'pass', 'last': s.last})
            print('PASS:', case, flush=True)
        result['status'] = 'pass'
    except Exception as error:
        result.update(status='fail', error=str(error))
        raise
    finally:
        (out / 'result.json').write_text(json.dumps(result, indent=2) + '\n')


if __name__ == '__main__':
    main()
