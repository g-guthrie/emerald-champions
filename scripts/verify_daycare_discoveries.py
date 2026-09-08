#!/usr/bin/env python3
"""Check early baby access and the one bred-and-hatched egg nursery reward."""
from pathlib import Path
import json
from verify_mega_stone_rewards import world_reward_sources

ROOT = Path(__file__).resolve().parents[1]
EARLY_ROUTES = (
    ('MAP_ROUTE102', 'land_mons', 'HAPPINY'),
    ('MAP_ROUTE116', 'land_mons', 'MUNCHLAX'),
    ('MAP_ROUTE116', 'land_mons', 'MIME_JR'),
    ('MAP_RUSTURF_TUNNEL', 'land_mons', 'CHINGLING'),
    ('MAP_DEWFORD_TOWN', 'fishing_mons', 'MANTYKE'),
    ('MAP_ROUTE115', 'land_mons', 'SKWOVET'),
    ('MAP_ROUTE117', 'land_mons', 'MEOWTH'),
    ('MAP_ROUTE117', 'land_mons', 'WOOLOO'),
    ('MAP_ROUTE117', 'land_mons', 'DITTO'),
    ('MAP_ROUTE112', 'land_mons', 'TYROGUE'),
)


def verify(root=ROOT):
    wild = json.loads((root / 'src/data/wild_encounters.json').read_text())
    hoenn = next(g for g in wild['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
    fields = {f['type'] for f in hoenn['fields']}
    for map_id, field, name in EARLY_ROUTES:
        assert field in fields, field
        table = next(e[field]['mons'] for e in hoenn['encounters'] if e['map'] == map_id)
        if name == 'MANTYKE':
            table = table[:2]  # Surf and stronger rods do not satisfy early access.
        assert any(m['species'] == 'SPECIES_' + name for m in table), (map_id, field, name)
    rewards = world_reward_sources(root)
    for item, location in [('KANGASKHANITE', 'Route117_PokemonDayCare'), ('AUDINITE', 'Route117:')]:
        sources = rewards['ITEM_' + item]
        assert len(sources) == 1 and location in sources[0], sources
    npc = (root / 'data/maps/Route117_PokemonDayCare/scripts.inc').read_text()
    assert 'call_if_set FLAG_EC_HATCHED_DAYCARE_EGG, Route117_PokemonDayCare_EventScript_HatchReward' in npc
    reward = npc.split('Route117_PokemonDayCare_EventScript_HatchReward::', 1)[1].split('\nRoute117_PokemonDayCare_EventScript_HatchRewardReturn::', 1)[0]
    assert 'goto_if_eq VAR_RESULT, FALSE' in reward
    for flag in ['FLAG_RECEIVED_WINSTRATE_KANGASKHANITE', 'FLAG_ITEM_ROUTE_120_KANGASKHANITE']:
        assert reward.index('goto_if_set ' + flag) < reward.index('giveitem ITEM_KANGASKHANITE')
        assert reward.index('giveitem ITEM_KANGASKHANITE') < reward.index('goto_if_eq VAR_RESULT, FALSE') < reward.index('setflag ' + flag)
    daycare = (root / 'src/daycare.c').read_text()
    gift = daycare.split('void CreateEgg(', 1)[1].split('static void SetInitialEggData(', 1)[0]
    bred = daycare.split('static void SetInitialEggData(struct Pokemon *mon, enum Species species, struct DayCare *daycare)\n{', 1)[1].split('\n}', 1)[0]
    assert 'METLOC_DAYCARE_EGG' not in gift
    assert 'metloc_u8_t origin = METLOC_DAYCARE_EGG;' in bred
    assert 'SetMonData(mon, MON_DATA_MET_LOCATION, &origin);' in bred
    hatch = (root / 'src/egg_hatch.c').read_text().split('static void AddHatchedMonToParty(u8 id)\n{', 1)[1].split('\n}', 1)[0]
    assert 'if (GetMonData(mon, MON_DATA_MET_LOCATION) == METLOC_DAYCARE_EGG)\n        FlagSet(FLAG_EC_HATCHED_DAYCARE_EGG);' in hatch
    assert hatch.index('FlagSet(FLAG_EC_HATCHED_DAYCARE_EGG)') < hatch.index('SetMonData(mon, MON_DATA_MET_LOCATION, &metLocation)')
    print('PASS: early babies, displaced families, gift exclusion and exclusive bred-egg Kangaskhanite reward')


if __name__ == '__main__':
    verify()
