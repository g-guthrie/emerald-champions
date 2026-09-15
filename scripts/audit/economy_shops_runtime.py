#!/usr/bin/env python3
"""Exercise economy shops through native clerk/menu input with synthetic prerequisites."""
import argparse
import hashlib
import json
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import render_emerald_champions_ui as ui
import run_emerald_champions_campaign as c
from rom_artifacts import verify_rom_elf_pair


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=False)
    rom, elf = out / 'scene.gba', out / 'scene.elf'
    verify_rom_elf_pair(ROOT / 'pokeemerald-headless.gba', ROOT / 'pokeemerald-headless.elf')
    for src, dst in [('pokeemerald-headless.gba', rom), ('pokeemerald-headless.elf', elf),
                     ('pokeemerald-headless.inputs.json', out / 'scene.inputs.json')]:
        shutil.copy2(ROOT / src, dst)
    constants = c.parse_numeric_constants()
    enum = (ROOT / 'include/emerald_champions_headless.h').read_text().split('enum EmeraldChampionsHeadlessScenario', 1)[1].split('};', 1)[0]
    scenario = re.findall(r'EC_HEADLESS_SCENARIO_\w+', enum).index('EC_HEADLESS_SCENARIO_ECONOMY_SHOPS')
    symbols = ui.native_tools.symbols(elf, ROOT, first=True)
    addresses = {name: symbols[name] for name in (*c.TELEMETRY_SYMBOLS, 'gEcHeadlessFixtureScenario', 'gEcHeadlessFixtureParam')}
    runner = ui.build_runner()
    state = out / 'current.ss1'
    common = dict(runner=runner, rom=rom, state=state, addresses=addresses)
    trace, panels = [], []
    report = dict(status='running', scope='Synthetic shop prerequisites; native NPCs, menus and transactions. No earned traversal or battle acceptance.',
                  build={f'{kind}_sha256': hashlib.sha256(path.read_bytes()).hexdigest() for kind, path in [('rom', rom), ('elf', elf)]}, trace=trace)

    def step(label, key=None, frames=100, panel=False):
        shot = out / f'{len(trace):03d}-{label}.png'
        telemetry, _ = c.run_state_chunk(**common, frames=frames, keys=[(0, 2, key)] if key else None, screenshot=shot)
        trace.append(dict(label=label, key=key, frames=frames, screenshot=str(shot), telemetry=telemetry))
        if panel:
            panels.append(dict(path=str(shot), label=label.replace('-', ' ')))
        return telemetry

    def check(kind, item, expected):
        telemetry, _ = c.run_state_chunk(**common, frames=2, writes=[
            (0, 4, addresses['gEcHeadlessCampaignQueryId'], constants.get(item, item)),
            (0, 4, addresses['gEcHeadlessCampaignQueryKind'], kind),
            (0, 4, addresses['gEcHeadlessCampaignQueryValue'], 0xFFFFFFFF)])
        actual = telemetry['gEcHeadlessCampaignQueryValue']
        trace.append(dict(query=kind, item=item, actual=actual, expected=expected))
        if actual != expected:
            raise AssertionError(f'query {kind} {item}: {actual} != {expected}')

    def boot(param):
        trace.append(dict(fixture='ECONOMY_SHOPS', param=param))
        ui.run([str(runner), '--rom', str(rom), '--rtc', '946684800', '--frames', '320', '--state-out', str(state),
                '--write', f"59:4:0x{addresses['gEcHeadlessFixtureParam']:x}:{param}",
                '--write', f"60:4:0x{addresses['gEcHeadlessFixtureScenario']:x}:{scenario}"])
        step(f'boot-{param}')
        step('face-clerk', 'DOWN' if param in (7, 8) else 'UP', 32)

    def open_shop(param, pages=2):
        boot(param)
        for page in range(pages):
            step(f'clerk-{param}-page-{page + 1}', 'A', panel=True)
        step('shop-options', 'A')
        step(f'stock-{param}', 'A', panel=True)

    def exit_shop():
        step('exit-stock', 'B')
        step('exit-shop', 'B')
        telemetry = step('farewell', 'A')
        if c.is_stable_overworld(telemetry):
            return
        for _ in range(4):
            telemetry = step('return-controls')
            if c.is_stable_overworld(telemetry):
                return
        raise AssertionError('Shop did not return field controls')

    try:
        open_shop(1)
        step('select-mega', 'A')
        step('cancel-confirmation', 'B', panel=True)
        check(9, 0, 20000); check(4, 'ITEM_PIDGEOTITE', 0)
        step('select-mega-again', 'A')
        step('purchase-one-mega', 'A', panel=True)
        check(9, 0, 0); check(4, 'ITEM_PIDGEOTITE', 1)
        step('return-to-stock', 'A')
        step('owned-mega-refused', 'A', panel=True)
        check(9, 0, 0); check(4, 'ITEM_PIDGEOTITE', 1)
        step('dismiss-refusal', 'A')
        exit_shop()
        for param, label in [(3, 'pc-owned'), (4, 'party-held'), (5, 'full-bag'), (6, 'insufficient-money')]:
            open_shop(param)
            step(label + '-select', 'A', panel=param != 5)
            if param == 5:
                step('full-bag-confirm', 'A', panel=True)
            check(9, 0, 19999 if param == 6 else 20000)
            check(4, 'ITEM_PIDGEOTITE', 0)
            if param == 3:
                check(5, 'ITEM_PIDGEOTITE', 1)
        boot(2)
        step('bracelet-required', 'A', panel=True)
        step('steven-guidance', 'A', panel=True)
        telemetry = step('no-bracelet-return', 'A')
        if not c.is_stable_overworld(telemetry):
            raise AssertionError('Bracelet gate failed to return controls')
        check(9, 0, 20000); check(4, 'ITEM_PIDGEOTITE', 0)
        open_shop(0)
        stones = ('FIRE', 'WATER', 'THUNDER', 'LEAF', 'MOON', 'SUN')
        for index, stone in enumerate(stones):
            step('select-' + stone.lower(), 'A')
            step('quantity-one', 'A')
            step('buy-' + stone.lower(), 'A', panel=index == 5)
            check(4, 'ITEM_' + stone + '_STONE', 1)
            check(9, 0, 6000 - (index + 1) * 500)
            step('return-to-evolution-stock', 'A')
            step('next-evolution-item', 'DOWN')
        step('select-reusable-cord', 'A', panel=True)
        step('buy-reusable-cord', 'A', panel=True)
        check(4, 'ITEM_LINKING_CORD', 1); check(9, 0, 0)
        step('return-to-cord', 'A')
        step('owned-cord-refused', 'A', panel=True)
        check(4, 'ITEM_LINKING_CORD', 1); check(9, 0, 0)
        step('dismiss-cord-refusal', 'A')
        exit_shop()
        open_shop(7)
        step('select-armor', 'A')
        step('armor-quantity', 'A')
        step('buy-armor', 'A', panel=True)
        check(4, 'ITEM_AUSPICIOUS_ARMOR', 1); check(9, 0, 19000)
        step('return-to-tools', 'A')
        exit_shop()
        open_shop(8, pages=1)
        exit_shop()
        for item, price in [('ITEM_LEAF_STONE', 500), ('ITEM_METAL_COAT', 1000),
                            ('ITEM_LINKING_CORD', 3000), ('ITEM_SCROLL_OF_WATERS', 3000)]:
            check(8, item, price)
        for item in ('ITEM_PIDGEOTITE', 'ITEM_AMPHAROSITE', 'ITEM_MANECTITE', 'ITEM_ALTARIANITE'):
            check(8, item, 20000)
            check(7, item, 0)
        report['status'] = 'pass'
        print('PASS: native Mega transactions and failure cases; six different stones plus reusable Cord for starting6000; Lilycove tools/stone menus and armor purchase; actual prices and zero Mega resale; returned controls.')
    except Exception as error:
        report.update(status='fail', error=str(error))
        raise
    finally:
        (out / 'result.json').write_text(json.dumps(report, indent=2) + '\n')
        (out / 'panels.json').write_text(json.dumps(dict(title='Economy shops: native transaction evidence', scope=report['scope'], build=report['build'], panels=panels), indent=2) + '\n')


if __name__ == '__main__':
    main()
