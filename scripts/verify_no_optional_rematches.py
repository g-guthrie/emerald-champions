#!/usr/bin/env python3
"""Reject optional Hoenn rematch entrypoints while preserving campaign reentry."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
BANNED = re.compile(r'\b(?:ShouldTryRematchBattle|register_matchcall|MULTI_REMATCH_BATTLE_MODE|FLAG_DAILY_REMATCH_\w+|trainerbattle_rematch\w*)\b')


def source(relative):
    return re.sub(r'@[^\n]*', '', (ROOT / relative).read_text())


def main():
    errors = []
    for path in (ROOT / 'data/maps').glob('*/scripts.inc'):
        for number, line in enumerate(re.sub(r'@[^\n]*', '', path.read_text()).splitlines(), 1):
            if BANNED.search(line):
                errors.append(f'{path.relative_to(ROOT)}:{number}: optional rematch hook: {line.strip()}')
    for area, trainer in [('MeteorFalls_StevensCave', 'STEVEN'), ('LilycoveCity_CoveLilyMotel_2F', 'BUFFEL')]:
        text = source(f'data/maps/{area}/scripts.inc')
        guard = text.find(f'goto_if_defeated TRAINER_{trainer},')
        battle = text.find(f'trainerbattle_no_intro_double TRAINER_{trainer},')
        if not 0 <= guard < battle:
            errors.append(f'{area}: one-time boss needs its permanent defeated guard')
    rival = source('data/maps/Route103/scripts.inc')
    if re.search(r'trainerbattle[^\n]*TRAINER_(?:MAY|BRENDAN)_LILYCOVE_', rival):
        errors.append('Route103 still dispatches a postgame rival rematch')
    if 'giveitem ITEM_EON_TICKET' not in rival or 'FLAG_ENABLE_SHIP_SOUTHERN_ISLAND' not in rival:
        errors.append('Route103 lost Southern Island access')
    league = source('data/maps/EverGrandeCity_PokemonLeague_1F/scripts.inc')
    if 'goto_if_set FLAG_ENTERED_ELITE_FOUR, EverGrandeCity_PokemonLeague_1F_EventScript_GoForth' not in league:
        errors.append('Normal Elite Four replay entry is missing')
    if errors:
        raise SystemExit('\n'.join(errors))
    print('PASS: no Hoenn rematch hooks/registrations/selectors; one-time bosses, Eon Ticket and League reentry retained')


if __name__ == '__main__':
    main()
