#!/usr/bin/env python3
"""Native full-Bag refusal and retry for all four Rustboro guide approaches.

Party and full Medicine pocket are fixture setup. Actual coordinate events,
movement, inventory delivery and state completion execute in libmGBA.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import struct
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
    header = (ROOT / 'include/emerald_champions_headless.h').read_text()
    enum = header.split('enum EmeraldChampionsHeadlessScenario', 1)[1].split('};', 1)[0]
    scenario = re.findall(r'EC_HEADLESS_SCENARIO_\w+', enum).index('EC_HEADLESS_SCENARIO_RUSTBORO_GUIDE_RETRY')
    addresses = {name: ui.resolve_symbol(elf, name) for name in (*campaign.TELEMETRY_SYMBOLS,
                 'gEcHeadlessFixtureScenario', 'gEcHeadlessFixtureParam', 'gEcHeadlessFixtureTrigger')}
    runner = ui.build_runner()
    state = out / 'current.ss1'
    rows = []
    result = {'status': 'running', 'rom_sha256': hashlib.sha256(rom.read_bytes()).hexdigest(),
              'elf_sha256': hashlib.sha256(elf.read_bytes()).hexdigest(),
              'setup': 'Healthy level-5 Zigzagoon; actual Fresh Water pocket filled via slot APIs; trigger 2 clears fixture Bag only',
              'trace': rows}
    map_data = json.loads((ROOT / 'data/maps/RustboroCity_Gym/map.json').read_text())
    triggers = map_data['coord_events']
    layouts = json.loads((ROOT / 'data/layouts/layouts.json').read_text())['layouts']
    layout = next(row for row in layouts if row['id'] == map_data['layout'])
    blocks = (ROOT / layout['blockdata_filepath']).read_bytes()
    with tempfile.TemporaryDirectory(prefix='ec-guide-runtime-') as scratch:
        scratch_rom = Path(scratch) / 'guide.gba'
        shutil.copy2(rom, scratch_rom)
        common = dict(runner=runner, rom=scratch_rom, state=state, addresses=addresses)

        def advance(label, frames=240, keys=None, writes=None):
            v, _ = campaign.run_state_chunk(**common, frames=frames, keys=keys, writes=writes,
                  screenshot=out / f'{len(rows):03d}-{label}.png')
            rows.append({'label': label, 'telemetry': v})
            return v

        def cross(label, direction):
            advance(label + '-step', 32, keys=[(0, 16, direction)])
            for _ in range(20):
                v = advance(label, keys=[(f, 2, 'A') for f in range(0, 240, 24)])
                if campaign.is_stable_overworld(v):
                    return v
            raise RuntimeError('guide scene failed to settle')

        def check(v, expected_state, expected_item, trigger):
            require((v['gEcHeadlessCampaignPlayerX'], v['gEcHeadlessCampaignPlayerY']) ==
                    (trigger['x'], trigger['y']), 'did not cross intended guide trigger')
            active, pos, _ = campaign.query_campaign_object(local_id=4, **common)
            require(active and pos == (3, 18), f'guide failed to return: {active}, {pos}')
            value, _ = campaign.query_campaign_value(kind=2, identifier=constants['VAR_RUSTBORO_GYM_GUIDE_STATE'], **common)
            require(value == expected_state, f'guide completion state {value} != {expected_state}')
            quantity, _ = campaign.query_campaign_value(kind=4, identifier=constants['ITEM_FRESH_WATER'], **common)
            require(quantity == expected_item, f'Fresh Water count {quantity} != {expected_item}')
            require(v['gEcHeadlessCampaignBattleSerial'] == 0, 'unexpected battle')
            rows.append({'guide_position': pos, 'guide_state': value, 'fresh_water': quantity})

        try:
            for index, trigger in enumerate(triggers):
                direction, reverse = [('LEFT', 'RIGHT'), ('UP', 'DOWN'), ('UP', 'DOWN'), ('RIGHT', 'LEFT')][index]
                command = [str(runner), '--rom', str(scratch_rom), '--rtc', '946684800', '--frames', '300',
                           '--state-out', str(state), '--write', f"59:4:0x{addresses['gEcHeadlessFixtureParam']:x}:{index}",
                           '--write', f"60:4:0x{addresses['gEcHeadlessFixtureScenario']:x}:{scenario}"]
                ui.run(command)
                ready = advance(f'entry-{index+1}-ready', 2)
                x, y = ready['gEcHeadlessCampaignPlayerX'], ready['gEcHeadlessCampaignPlayerY']
                require(campaign.is_stable_overworld(ready), 'fixture did not settle')
                require(0 <= x < layout['width'] and 0 <= y < layout['height'], 'fixture started outside the map')
                require((struct.unpack_from('<H', blocks, 2*(y*layout['width']+x))[0] >> 10) & 3 == 0,
                        'fixture started on a collision tile')
                for attempt in range(2):
                    v = cross(f'entry-{index+1}-full-{attempt+1}', direction)
                    check(v, 0, 0, trigger)
                    advance('step-away', 32, keys=[(0, 16, reverse)])
                advance('make-room', 2, writes=[(0, 4, addresses['gEcHeadlessFixtureTrigger'], 2)])
                v = cross(f'entry-{index+1}-retry', direction)
                check(v, 1, 5, trigger)
                advance('completed-step-away', 32, keys=[(0, 16, reverse)])
                v = cross(f'entry-{index+1}-completed', direction)
                check(v, 1, 5, trigger)
            result['status'] = 'pass'
        except Exception as error:
            result.update(status='fail', error=str(error))
            raise
        finally:
            (out / 'result.json').write_text(json.dumps(result, indent=2) + '\n')
    print('PASS: all four Rustboro guide paths restore on full Bag and grant exactly five waters on retry')


if __name__ == '__main__':
    main()
