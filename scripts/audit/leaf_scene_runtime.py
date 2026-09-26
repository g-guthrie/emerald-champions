#!/usr/bin/env python3
"""Exercise Leaf's real coordinate event in a dedicated fixture ROM.

Inventory, party and prior-save setup are synthetic. Movement, script branches,
reward delivery and map reloads run in libmGBA. First-win combat is auto-resolved
and proves scene traversal only. Uses scratch saves and exact ROM/ELF identity.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import render_emerald_champions_ui as ui
from rom_artifacts import verify_rom_elf_pair
import build_provenance as provenance

PREFIX = 'gEcHeadless'
FIELDS = ['FixtureScenario', 'FixtureParam', 'FixtureTrigger', 'FixtureSetupResult',
          'FixtureObservedResult', 'FixtureFlashLevel',
          'CampaignPlayerX', 'CampaignPlayerY', 'CampaignControlsLocked',
          'CampaignScriptEnabled', 'CampaignInBattle', 'CampaignBattleSerial',
          'CampaignQueryKind', 'CampaignQueryId', 'CampaignQueryObjectActive',
          'CampaignQueryObjectX', 'CampaignQueryObjectY',
          'LeafRewardOwned', 'LeafCompleted', 'LeafTrainerDefeated']


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rom', type=Path, required=True)
    parser.add_argument('--elf', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    rom, elf, out = args.rom.resolve(), args.elf.resolve(), args.out.resolve()
    verify_rom_elf_pair(rom, elf)
    out.mkdir(parents=True, exist_ok=True)
    require(not (out / 'result.json').exists(), 'use a new output directory to preserve prior evidence')
    header = (ROOT / 'include/emerald_champions_headless.h').read_text()
    enum = header.split('enum EmeraldChampionsHeadlessScenario', 1)[1].split('};', 1)[0]
    scenarios = re.findall(r'EC_HEADLESS_SCENARIO_\w+', enum)
    scenario = scenarios.index('EC_HEADLESS_SCENARIO_LEAF_SCENE')
    nm = ui.run([shutil.which('arm-none-eabi-nm') or 'arm-none-eabi-nm', '-S', str(elf)]).stdout
    symbols = {parts[-1]: int(parts[0], 16) for line in nm.splitlines()
               if len(parts := line.split()) >= 3 and re.fullmatch('[0-9a-fA-F]+', parts[0])}
    addresses = {name: symbols[PREFIX + name] for name in FIELDS}
    runner = ui.build_runner()
    trace = []
    manifest = {'rom_sha256': hashlib.sha256(rom.read_bytes()).hexdigest(),
                'elf_sha256': hashlib.sha256(elf.read_bytes()).hexdigest(),
                'build_provenance': provenance.verify(rom, elf),
                'evidence': 'real script traversal; synthetic setup; auto-resolved first battle',
                'setup': {
                    'leaf': 'Badge 2 and HM Flash; parameterized completion/trainer/item capacity and one/two usable mons',
                    'secret_base': 'Local owner slot 0, Red Cave 1, established entrance, small chair at (5,5); native decoration initialization',
                    'new_mauville': 'Voltorb 2 previously defeated and hidden; Upgrade already collected; healthy level-100 Zigzagoon; 250 native Repel steps',
                },
                'status': 'running', 'trace': trace}
    with tempfile.TemporaryDirectory(prefix='ec-leaf-runtime-') as scratch:
        scratch_rom = Path(scratch) / 'leaf.gba'
        shutil.copy2(rom, scratch_rom)
        state = out / 'current.ss1'

        def advance(label, frames=240, *, keys=(), writes=(), boot=False):
            next_state = out / 'next.ss1'
            command = [str(runner), '--rom', str(scratch_rom), '--rtc', '946684800',
                       '--frames', str(frames), '--state-out', str(next_state)]
            if not boot:
                command += ['--state-in', str(state)]
            for frame, duration, key in keys:
                command += ['--key', f'{frame}:{duration}:{key}']
            for frame, name, value in writes:
                command += ['--write', f'{frame}:4:0x{addresses[name]:x}:{value}']
            for address in addresses.values():
                command += ['--read', f'4:0x{address:x}']
            screenshot = out / f'{len(trace):03d}-{label}.png'
            command += ['--screenshot', str(screenshot)]
            result = ui.run(command)
            next_state.replace(state)
            reads = {int(a, 16): int(v, 16) for a, v in ui.READ_PATTERN.findall(result.stdout)}
            values = {name: reads[address] for name, address in addresses.items()}
            trace.append({'label': label, 'frames': frames, 'telemetry': values,
                          'screenshot': str(screenshot)})
            (out / 'result.json').write_text(json.dumps(manifest, indent=2) + '\n')
            return values

        def stable(v):
            return v['FixtureSetupResult'] == 1 and not any(v[k] for k in
                ('CampaignControlsLocked', 'CampaignScriptEnabled', 'CampaignInBattle'))

        def position(v, x, y):
            require((v['CampaignPlayerX'], v['CampaignPlayerY']) == (x, y),
                    f'expected player {(x,y)}, got {v}')

        def settle(label):
            for _ in range(50):
                v = advance(label, keys=[(f, 2, 'A') for f in range(0, 240, 24)])
                if stable(v):
                    return v
            raise RuntimeError(f'{label}: scene did not settle')

        def boot(param):
            v = advance(f'boot-{param}', 300, boot=True, writes=[
                (59, 'FixtureParam', param), (59, 'CampaignQueryId', 11),
                (59, 'CampaignQueryKind', 3), (60, 'FixtureScenario', scenario)])
            require(stable(v), f'fixture not ready: {v}')
            require(v['FixtureFlashLevel'] == 1, f'native Flash unlock not applied: {v}')
            position(v, 21, 20)
            return v

        def cross(label):
            advance(label + '-up', 32, keys=[(0, 16, 'UP')])
            return settle(label)

        def reload(label, clear_bag=False):
            v = advance(label, 180, writes=[(0, 'FixtureTrigger', 2 if clear_bag else 1)])
            require(stable(v), f'reload not ready: {v}')
            position(v, 21, 20)

        try:
            boot(0)
            for index in range(2):
                v = cross('completed')
                position(v, 21, 19)
                require(v['LeafCompleted'] == 1 and v['CampaignBattleSerial'] == 0
                        and v['CampaignQueryObjectActive'] == 0, f'completed replay: {v}')
                if index == 0:
                    reload('completed-reentry')

            boot(2)
            v = cross('insufficient')
            position(v, 21, 19)
            require(v['CampaignBattleSerial'] == 0 and v['LeafCompleted'] == 0
                    and (v['CampaignQueryObjectX'], v['CampaignQueryObjectY']) == (21, 16),
                    f'insufficient party moved Leaf or completed battle: {v}')

            boot(1)
            for index in range(2):
                v = cross('full-bag')
                position(v, 21, 19)
                require(v['LeafRewardOwned'] == 0 and v['LeafCompleted'] == 0
                        and v['CampaignBattleSerial'] == 0
                        and (v['CampaignQueryObjectX'], v['CampaignQueryObjectY']) == (21, 16),
                        f'full bag did not restore pending scene: {v}')
                if index == 0:
                    advance('step-away', 32, keys=[(0, 16, 'DOWN')])
            reload('pending-reentry')
            v = cross('full-bag-after-reentry')
            require(v['LeafCompleted'] == 0 and v['CampaignBattleSerial'] == 0,
                    f'pending reentry lost reward state: {v}')
            reload('make-room-reentry', clear_bag=True)
            v = cross('reward-retry')
            position(v, 20, 18)
            require(v['LeafCompleted'] == 1 and v['LeafRewardOwned'] == 1
                    and v['CampaignBattleSerial'] == 0 and v['CampaignQueryObjectActive'] == 0,
                    f'earned reward retry failed: {v}')
            reload('reward-completed-reentry')
            v = cross('reward-completed')
            require(v['CampaignBattleSerial'] == 0 and v['CampaignQueryObjectActive'] == 0,
                    f'reward completion replayed: {v}')

            boot(3)
            v = cross('first-battle')
            require(v['CampaignBattleSerial'] == 1 and v['LeafTrainerDefeated'] == 1
                    and v['LeafCompleted'] == 1 and v['LeafRewardOwned'] == 1,
                    f'first battle traversal failed: {v}')

            v = advance('secret-base-established', 300, boot=True, writes=[
                (60, 'FixtureScenario', scenarios.index('EC_HEADLESS_SCENARIO_SECRET_BASE_ESTABLISHED'))])
            require(stable(v) and v['FixtureObservedResult'] == 1,
                    f'established Secret Base map/position/decoration not observed: {v}')
            position(v, 6, 5)

            v = advance('new-mauville-ready', 300, boot=True, writes=[
                (60, 'FixtureScenario', scenarios.index('EC_HEADLESS_SCENARIO_NEW_MAUVILLE_BUTTONS'))])
            require(stable(v), f'New Mauville fixture not settled: {v}')
            position(v, 6, 12)
            v = cross('new-mauville-blue')
            position(v, 6, 11)
            require(v['FixtureObservedResult'] == 1, f'blue button did not swap passages: {v}')
            # Walk along the actual corridor to the green button; no state patch.
            advance('new-mauville-down', 32, keys=[(0, 16, 'DOWN')])
            for index in range(4):
                v = advance(f'new-mauville-left-{index}', 32, keys=[(0, 16, 'LEFT')])
            position(v, 2, 12)
            v = cross('new-mauville-green')
            position(v, 2, 11)
            require(v['FixtureObservedResult'] == 2, f'green button did not reverse passages: {v}')
            manifest['status'] = 'pass'
        except Exception as exc:
            manifest.update(status='fail', error=str(exc))
            raise
        finally:
            (out / 'result.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print('PASS: illuminated Leaf/retries, established decorated Secret Base, New Mauville blue/green switches')


if __name__ == '__main__':
    main()
