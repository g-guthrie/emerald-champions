#!/usr/bin/env python3
"""Native Steven/Museum edge cases with explicit synthetic inventory setup."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import render_emerald_champions_ui as ui
import run_emerald_champions_campaign as campaign
from rom_artifacts import verify_rom_elf_pair


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('rom', 'elf', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    rom, elf, out = args.rom.resolve(), args.elf.resolve(), args.out.resolve()
    verify_rom_elf_pair(rom, elf)
    out.mkdir(parents=True, exist_ok=True)
    require(not (out / 'result.json').exists(), 'use a new evidence directory')
    constants = campaign.parse_numeric_constants()
    enum = (ROOT / 'include/emerald_champions_headless.h').read_text().split('enum EmeraldChampionsHeadlessScenario', 1)[1].split('};', 1)[0]
    scenario = re.findall(r'EC_HEADLESS_SCENARIO_\w+', enum).index('EC_HEADLESS_SCENARIO_STORY_HANDOFF')
    addresses = {name: ui.resolve_symbol(elf, name) for name in (*campaign.TELEMETRY_SYMBOLS,
                 'gEcHeadlessFixtureScenario', 'gEcHeadlessFixtureParam', 'gEcHeadlessFixtureTrigger')}
    runner = ui.build_runner()
    state, rows = out / 'current.ss1', []
    result = {'status': 'running', 'rom_sha256': hashlib.sha256(rom.read_bytes()).hexdigest(),
              'elf_sha256': hashlib.sha256(elf.read_bytes()).hexdigest(),
              'setup': 'Synthetic two-mon party. Missing Letter, PC Letter without Badge 2, or Ring-owned Charmander with full Mega pocket and PC. Trigger 2/3 frees one PC slot and reloads; trigger 1 reloads only. Museum starts without Parts and with both grunts hidden.',
              'scope': 'Real map interactions and persistent inventory/flag state; not fresh-save progression.', 'trace': rows}
    with tempfile.TemporaryDirectory(prefix='ec-handoff-runtime-') as scratch:
        scratch_rom = Path(scratch) / 'handoff.gba'
        shutil.copy2(rom, scratch_rom)
        common = dict(runner=runner, rom=scratch_rom, state=state, addresses=addresses)

        def advance(label, frames=240, keys=None, writes=None):
            v, _ = campaign.run_state_chunk(**common, frames=frames, keys=keys, writes=writes,
                    screenshot=out / f'{len(rows):03d}-{label}.png')
            rows.append({'label': label, 'telemetry': v})
            return v

        def query(kind, name):
            value, _ = campaign.query_campaign_value(kind=kind, identifier=constants[name], **common)
            rows.append({'query_kind': kind, 'name': name, 'value': value})
            return value

        def actor(local_id, expected):
            active, position, _ = campaign.query_campaign_object(local_id=local_id, **common)
            rows.append({'actor': local_id, 'active': active, 'position': position})
            require(active == expected, f'actor {local_id} active={active}, expected {expected}')
            if local_id == 1 and expected:
                require(position == (7, 8), f'Steven moved while reward pending: {position}')

        def boot(param):
            ui.run([str(runner), '--rom', str(scratch_rom), '--rtc', '946684800', '--frames', '300',
                    '--state-out', str(state), '--write', f"59:4:0x{addresses['gEcHeadlessFixtureParam']:x}:{param}",
                    '--write', f"60:4:0x{addresses['gEcHeadlessFixtureScenario']:x}:{scenario}"])
            ready = advance(f'boot-{param}', 2)
            require(campaign.is_stable_overworld(ready), f'fixture {param} not settled')
            require((ready['gEcHeadlessCampaignPlayerX'], ready['gEcHeadlessCampaignPlayerY']) ==
                    ((12,6) if param == 3 else (7,9)), 'fixture started at wrong position')

        def interact(label, facing='UP'):
            advance(label + '-face', 32, keys=[(0, 16, facing)])
            for _ in range(60):
                v = advance(label, 40, keys=[(0, 2, 'A')])
                if campaign.is_stable_overworld(v):
                    require(v['gEcHeadlessCampaignBattleSerial'] == 0, 'unexpected battle')
                    return
            raise RuntimeError(label + ' did not settle')

        def reload(command):
            v = advance(f'fixture-storage-request-{command}', 180,
                        writes=[(0, 4, addresses['gEcHeadlessFixtureTrigger'], command)])
            require(campaign.is_stable_overworld(v), 'reload failed to settle')

        def stones(bits, x, y, visible):
            require(query(2, 'VAR_STEVEN_STARTER_STONE_DELIVERY') == bits, 'wrong delivery ledger')
            require(query(5, 'ITEM_CHARIZARDITE_X') == x, 'wrong PC Charizardite X quantity')
            require(query(5, 'ITEM_CHARIZARDITE_Y') == y, 'wrong PC Charizardite Y quantity')
            require(query(4, 'ITEM_MEGA_RING') == 1, 'Ring lost or duplicated')
            actor(1, visible)

        try:
            boot(0)
            interact('missing-letter')
            require(query(1, 'FLAG_DELIVERED_STEVEN_LETTER') == 0, 'missing Letter advanced story')
            require(query(4, 'ITEM_MEGA_RING') == 0, 'missing Letter granted Ring')
            actor(1, True)
            boot(1)
            require(query(5, 'ITEM_LETTER') == 1, 'PC Letter fixture missing')
            interact('pc-letter')
            require(query(5, 'ITEM_LETTER') == 0, 'PC Letter not consumed')
            require(query(1, 'FLAG_DELIVERED_STEVEN_LETTER') == 1, 'real Letter handoff not recorded')
            require(query(4, 'ITEM_MEGA_RING') == 0, 'Ring granted without Badge 2')
            actor(1, True)
            boot(2)
            interact('full-storage')
            stones(0, 0, 0, True)
            reload(2)
            interact('one-pc-slot')
            stones(1, 1, 0, True)
            reload(1)
            interact('partial-reentry')
            stones(1, 1, 0, True)
            reload(3)
            interact('second-pc-slot')
            stones(3, 1, 1, False)
            reload(1)
            stones(3, 1, 1, False)
            boot(3)
            actor(3, False)
            actor(4, False)
            interact('museum-missing-parts', 'RIGHT')
            require(query(1, 'FLAG_DELIVERED_DEVON_GOODS') == 0, 'missing Parts advanced story')
            actor(3, False)
            actor(4, False)
            result['status'] = 'pass'
        except Exception as error:
            result.update(status='fail', error=str(error))
            raise
        finally:
            (out / 'result.json').write_text(json.dumps(result, indent=2) + '\n')
    print('PASS: missing/PC Letter, durable two-stone partial/reentry delivery, Museum missing-Parts preflight')


if __name__ == '__main__':
    main()
