import json
from pathlib import Path

root = Path(__file__).resolve().parents[2]
p = root / 'src/data/wild_encounters.json'
data = json.loads(p.read_text())
group = next(g for g in data['wild_encounter_groups'] if g['label'] == 'gWildMonHeaders')
rows = {}
for row in group['encounters']:
    rows.setdefault(row['map'], row)

# Keep every former opening species before Roxanne; add immediately usable
# offense on Route 101 and doubles support on Route 102.
opening = {
    ('ROUTE101', 4): ('PIDGEY', 'MIENFOO'),
    ('ROUTE102', 6): ('KRICKETOT', 'PACHIRISU'),
    ('ROUTE104', 2): ('MIENFOO', 'PIDGEY'),
    ('PETALBURG_WOODS', 2): ('WURMPLE', 'KRICKETOT'),
}
for (name, index), (old, new) in opening.items():
    mon = rows['MAP_' + name]['land_mons']['mons'][index]
    assert mon['species'] == 'SPECIES_' + old
    mon['species'] = 'SPECIES_' + new

layouts = {
    'ARTISAN_CAVE_1F':
        'SMEARGLE SMEARGLE SMEARGLE SMEARGLE SMEARGLE WOOBAT GLIGAR SABLEYE MAWILE ARON CARBINK WOBBUFFET',
    'ARTISAN_CAVE_B1F':
        'SMEARGLE SMEARGLE SMEARGLE SABLEYE MAWILE CARBINK WOBBUFFET SMEARGLE SMEARGLE SMEARGLE SABLEYE MAWILE',
    'CAVE_OF_ORIGIN_DIANCIES_ROOM':
        'CARBINK CARBINK CARBINK SABLEYE MAWILE CARBINK GLIMMORA CARBINK CARBINK SABLEYE MAWILE GLIMMORA',
    'ROUTE130':
        'WYNAUT WYNAUT WYNAUT WOBBUFFET GIRAFARIG DODUO NATU WYNAUT XATU TROPIUS WOBBUFFET KECLEON',
    # Keep the desert entrance rosters and their strong early finds. Accents
    # on later rooms preserve Darmanitan and Krookodile in their current slots.
    'MIRAGE_TOWER_4F':
        'CLAYDOL GOLURK BRONZONG SIGILYPH COFAGRIGUS DARMANITAN HONEDGE SANDACONDA KROOKODILE SPIRITOMB COFAGRIGUS BRONZONG',
    'SANDSTREWN_RUINS_B1F':
        'COFAGRIGUS BRONZONG GOLURK DARMANITAN DOUBLADE SIGILYPH STAKATAKA GOLURK CLAYDOL KROOKODILE AEGISLASH COFAGRIGUS',
    'MT_PYRE_1F':
        'SHUPPET DUSKULL GASTLY DUSKULL LITWICK MISDREAVUS HAUNTER SHUPPET GREAVARD SINISTEA BANETTE DUSCLOPS',
    'MT_PYRE_3F':
        'SHUPPET DUSKULL HAUNTER LITWICK MISDREAVUS LITWICK LAMPENT PHANTUMP LAMPENT SINISTEA BANETTE DUSCLOPS',
    'MT_PYRE_4F':
        'BANETTE DUSCLOPS LAMPENT MIMIKYU HAUNTER DUSCLOPS MISDREAVUS BANETTE GREAVARD POLTCHAGEIST DUSKNOIR CHANDELURE',
    'MT_PYRE_6F':
        'BANETTE DUSCLOPS LAMPENT SINISTCHA DUSKNOIR CHANDELURE HOUNDSTONE POLTCHAGEIST SINISTCHA HOUNDSTONE DUSKNOIR CHANDELURE',
    'SEAFLOOR_CAVERN_ROOM1':
        'DRAGALGE GOLISOPOD DRAGALGE TOXAPEX DRAGALGE MALAMAR BARRASKEWDA GRAPPLOCT GOLISOPOD CLAWITZER KINGDRA SHARPEDO',
    'SEAFLOOR_CAVERN_ROOM3':
        'DHELMISE CLAYDOL SABLEYE BRONZONG DHELMISE DRAGALGE TOXAPEX GOLISOPOD DHELMISE SABLEYE CLAYDOL BRONZONG',
    'SEAFLOOR_CAVERN_ROOM5':
        'KINGDRA GOLISOPOD KINGDRA KINGDRA GOLISOPOD DRAGALGE DRAGALGE TOXAPEX CROBAT BARRASKEWDA BASCULEGION GRAPPLOCT',
    'SEAFLOOR_CAVERN_ROOM8':
        'KINGDRA DRAGALGE DRAGALGE KINGDRA GOLISOPOD GOLISOPOD CLAWITZER CLAWITZER CROBAT CROBAT BASCULEGION CLOBBOPUS',
}
for name, roster in layouts.items():
    species = roster.split()
    mons = rows['MAP_' + name]['land_mons']['mons']
    assert len(species) == len(mons) == 12
    for mon, s in zip(mons, species):
        mon['species'] = 'SPECIES_' + s
for name, s in [('ROOM2', 'SHARPEDO'), ('ROOM4', 'MALAMAR'), ('ROOM6', 'BARBARACLE'), ('ROOM7', 'TOXAPEX')]:
    for mon in rows['MAP_SEAFLOOR_CAVERN_' + name]['land_mons']['mons']:
        if mon['species'] == 'SPECIES_DHELMISE':
            mon['species'] = 'SPECIES_' + s
p.write_text(json.dumps(data, indent=2) + '\n')

p = root / 'data/emerald_champions/wild_route_sheet.json'
sheet = json.loads(p.read_text())
# Dhelmise remains a wreck/haunted-water/deep-chamber find. Rod access is
# unchanged; replacements match their habitats and supply ready battle roles.
replacements = {
    'DEWFORD_TOWN': 'MANTINE',
    'METEOR_FALLS_B1F_1R': 'KINGDRA',
    'MOSSDEEP_CITY': 'STARMIE',
    'ROUTE107': 'BARRASKEWDA',
    'ROUTE111': 'WHISCASH',
    'ROUTE120': 'LUDICOLO',
    'ROUTE121': 'JELLICENT',
    'SAFARI_ZONE_SOUTHEAST': 'AZUMARILL',
    'SANDSTREWN_RUINS': 'RELICANTH',
}
for name, species in replacements.items():
    mons = sheet['MAP_' + name]['super_rod']
    assert mons[-1] == 'DHELMISE'
    mons[-1] = species
# Give neighboring seas different residents instead of sharing the same
# rotated filler. Strong forms stay available; all underwater rosters stay put.
water = {
    'ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS': {
        'surf': 'FRILLISH DRAGALGE JELLICENT TENTACRUEL DHELMISE',
        'super_rod': 'DHELMISE DRAGALGE SHARPEDO LANTURN JELLICENT',
    },
    'ABANDONED_SHIP_ROOMS_B1F': {
        'surf': 'MAREANIE TENTACRUEL GOLISOPOD QWILFISH LAPRAS',
        'super_rod': 'CLAWITZER OCTILLERY QWILFISH BARBARACLE DHELMISE',
    },
    'PETALBURG_CITY': {'old_rod': 'MAGIKARP MARILL'},
    'METEOR_FALLS_B1F_1R': {'surf': 'DRATINI BASCULIN DRAGONAIR POLIWHIRL KINGDRA'},
    'ROUTE125': {'surf': 'SPHEAL DEWGONG PELIPPER WALREIN LAPRAS'},
    'ROUTE127': {'surf': 'MAREANIE PYUKUMUKU TOXAPEX BRUXISH LAPRAS'},
    'ROUTE129': {'surf': 'WAILMER SHARPEDO WAILORD MANTINE LAPRAS'},
    'ROUTE130': {'surf': 'FRILLISH WISHIWASHI JELLICENT ALOMOMOLA LAPRAS'},
    'ROUTE131': {'surf': 'CLAMPERL TENTACRUEL RELICANTH LUMINEON GYARADOS'},
    'ROUTE132': {'surf': 'CARVANHA MAGIKARP BARRASKEWDA WAILORD FINIZEN'},
    'ROUTE133': {'surf': 'SPHEAL PELIPPER DRAGALGE MANTINE LAPRAS'},
    'ROUTE134': {'surf': 'SEEL CLAMPERL HUNTAIL GOREBYSS GYARADOS'},
    'SEAFLOOR_CAVERN_ROOM6': {'surf': 'FRILLISH DRAGALGE JELLICENT GOLISOPOD LAPRAS'},
    'SEAFLOOR_CAVERN_ROOM7': {'surf': 'CLAMPERL WISHIWASHI HUNTAIL GOREBYSS FINIZEN'},
    'VICTORY_ROAD_B2F': {'super_rod': 'DREDNAW CRAWDAUNT GOLDUCK BASCULIN DRAGONAIR'},
}
for name, methods in water.items():
    for method, roster in methods.items():
        mons = roster.split()
        assert len(mons) == len(sheet['MAP_' + name][method])
        sheet['MAP_' + name][method] = mons
notes = {
    'PETALBURG_CITY': 'A sheltered town pond. Marill joins Magikarp on the Old Rod; Surf and stronger rods broaden the waterside team options.',
    'DEWFORD_TOWN': 'Old Rod Mantyke remains a village specialty. Mantine is the stronger-rod return visit, with Krabby, Staryu and Corsola around the pier and reef.',
    'METEOR_FALLS_B1F_1R': 'The dragon pool: Dratini and Dragonair share the deep water, with Kingdra as a ready partner. Basculin and Poliwhirl keep the river identity.',
    'MOSSDEEP_CITY': 'Staryu tide pools and Cramorant pilings. Starmie rewards a return to the sea beside the Space Center with the Super Rod.',
    'ROUTE107': 'Fast swimmers and clear shallows: Arrokuda, Lumineon and Staryu, with Barraskewda on the Super Rod.',
    'ROUTE111': 'Desert oasis: Wooper and Barboach mud life, with Whiscash on the Super Rod. Dratini remains the unusual Surf find.',
    'ROUTE120': 'Reed marsh: Wooper, Masquerain and Chewtle. Ludicolo is the distinctive Super Rod find among the freshwater residents.',
    'ROUTE121': 'Mt. Pyre foreshore: Frillish and Swanna over still water, with Jellicent on the Super Rod.',
    'SAFARI_ZONE_SOUTHEAST': 'Safari lowland stream: Marill, Lotad and Goldeen families, with Azumarill as the Super Rod prize.',
    'SANDSTREWN_RUINS': 'Buried cistern: Wooper and Whiscash in still water. An ancient Relicanth rewards fishing in the ruins.',
    'ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS': 'Flooded wreck: Frillish and Jellicent haunt the corridors; Dhelmise is the leading Super Rod catch and a Surf discovery.',
    'ABANDONED_SHIP_ROOMS_B1F': 'Flooded cabins: Mareanie, Golisopod and Qwilfish shelter in the hull; Clauncher and Clawitzer occupy the pipes. Dhelmise remains in the wreck.',
    'ROUTE125': 'Shoal approach: Spheal, Dewgong and Walrein follow the cold current, with Pelipper overhead and Lapras offshore.',
    'ROUTE127': 'Toxic reef: Mareanie, Toxapex, Pyukumuku and Bruxish, with Lapras beyond the reef.',
    'ROUTE129': 'Deep-water crossing: Wailmer and Wailord pods, Sharpedo lanes and Mantine riding the current.',
    'ROUTE130': 'Mirage Island waters: Frillish and Jellicent shadows among Wishiwashi schools. The island grass is a Wynaut colony with birds and forest visitors.',
    'ROUTE131': 'Sky Pillar approach: Clamperl beds, Relicanth grounds, Tentacruel and Gyarados in the old sea.',
    'ROUTE132': 'Fast current: Carvanha and Barraskewda hunt the channel; Magikarp, Wailord and Finizen retain their existing surface roles.',
    'ROUTE133': 'Cold current meets kelp: Spheal drift south, Dragalge shelter in the weeds, and Mantine and Lapras cross below Pelipper.',
    'ROUTE134': 'Sealed Chamber approach: Seel and Clamperl, then Huntail and Gorebyss along the trench. Gyarados remains the surface discovery.',
    'SEAFLOOR_CAVERN_ENTRANCE': 'Cavern pool: Clamperl, Tentacruel and Sharpedo. The Dondozo and Tatsugiri discoveries are in the separate submerged submarine chamber.',
    'SEAFLOOR_CAVERN_ROOM6': 'Dark inner pool: Frillish, Jellicent, Dragalge and Golisopod, with Basculegion on the Super Rod.',
    'SEAFLOOR_CAVERN_ROOM7': 'Shell beds and deep channels: Clamperl, Huntail, Gorebyss and Wishiwashi. Finizen and the existing rod discoveries remain.',
    'VICTORY_ROAD_B2F': 'Underground river: Basculin and Floatzel at the surface, with Crawdaunt and Dragonair among the ready Super Rod choices before the League.',
}
for name, why in notes.items():
    sheet['MAP_' + name]['why'] = why
p.write_text(json.dumps(sheet, indent=2) + '\n')
print('Authored',len(opening) + len(layouts) + 4,'land maps and',len(set(replacements) | set(water)),'water maps')
