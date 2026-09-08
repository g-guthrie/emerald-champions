from pathlib import Path
import json
B=Path(__file__).resolve().parents[1]
w=json.loads((B/'inventory/wild.json').read_text());out=[]
changes=[
('ROUTE118','land_mons','RATICATE','ZIGZAGOON_GALAR','Add the missing Galarian raccoon line on the settled river corridor. Ordinary Rattata is available in Dewford Manor and can already evolve; preserve Linoone/Manectric as route anchors.'),
('ROUTE118','land_mons','PORYGON','SQUAWKABILLY_GREEN','Add the missing flocking parrot beside a human travel corridor. Porygon retains New Mauville, Safari and other sources; no technological family disappears.'),
('SAFARI_ZONE_SOUTH','land_mons','PORYGON','MEOWTH_GALAR','The imported-fauna collection gains the missing Galarian Meowth/Perrserker line. New Mauville retains Porygon at the same Surf access tier.'),
('ROUTE117','old_rod','MAGIKARP','WOOPER_PALDEA','The Daycare pond gains a mud-dwelling regional form and immediate Clodsire option. Keep Tympole as the other Old Rod species. Magikarp remains widely available from the earlier Dewford Old Rod.'),
('VERDANTURF_MEADOW','land_mons','ALCREMIE','PONYTA_GALAR','The meadow gains the missing Galarian Ponyta line. Milcery remains here and evolves at30, retaining practical Alcremie access at this cap.'),
('ROUTE121','land_mons','HYPNO','GRIMER_ALOLA','The Lilycove approach gains an imported urban scavenger and Alolan Muk path. Drowzee remains in early Dewford Manor, preserving Hypno through evolution.'),
('SHOAL_CAVE_LOW_TIDE_LOWER_ROOM','land_mons','SEEL','SANDSHREW_ALOLA','Add the missing Ice/Steel line without removing early Seel access or the Shoal entrance population. This lower cold chamber becomes distinct from the entrance.'),
('SHOAL_CAVE_LOW_TIDE_INNER_ROOM','land_mons','ZUBAT','DARUMAKA_GALAR','Introduce the missing Galarian Darumaka branch while bats remain at the entrance and other caves. Preserve Spheal/Snorunt and the existing water population.'),
('ROUTE119','land_mons','KOMMO_O','VOLTORB_HISUI','The wet forest gains a wooden, Grass/Electric regional form. Jangmo-o remains on Route114/JaggedPass and can evolve before this cap; direct Kommo-o remains elsewhere.'),
('METEOR_FALLS_1F_2R','land_mons','CLEFAIRY','SCREAM_TAIL','Add the missing ancient lunar Fairy while retaining Clefairy in the front chamber and other Falls floors. Existing back-room physical access is retained.'),
('ASHEN_WOODS','land_mons','TRUMBEAK','BRUTE_BONNET','The ash forest gains an ancient mushroom. Pikipek remains on Route104 and evolves at14, so the Trumbeak family remains early and plentiful.'),
('FIERY_PATH','land_mons','LARVESTA','SLITHER_WING','A sun-associated ancient insect gives the hot path a distinct find. Larvesta remains in the deeper Petalburg forest, Ashen Woods and volcanic interiors.'),
('ROUTE111_RUINS_EXTERIOR','land_mons','CLAYDOL','SANDY_SHOCKS','The desert ruins gain the missing ancient magnet. Baltoy/Claydol remain in the desert and ruin interiors; no early player family is removed.'),
('SANDSTREWN_RUINS_2F','land_mons','CLAYDOL','IRON_TREADS','Add a missing mechanical excavator to a ruin excavation floor. Claydol remains on adjacent ruin floors and can be evolved from Baltoy.'),
('VICTORY_ROAD_B1F','land_mons','NOIBAT','IRON_JUGULIS','The middle Victory Road floor gains a distinct late opponent-building option. Noibat remains from Rusturf onward and on the other Victory Road floors.'),
('SCORCHED_SLAB_B2F','land_mons','SLUGMA','IRON_MOTH','A missing artificial fire insect distinguishes the hot deep slab. Slugma remains at Fiery Path and multiple neighboring volcanic sites.'),
('NEW_MAUVILLE_ENTRANCE','land_mons','EELEKTROSS','IRON_THORNS','The power station gains a missing mechanical Electric species. Tynamo stays in the station and evolves through the free item archive; Magnemite/Voltorb/Klink identity remains.'),
('VERDANTURF_MEADOW','land_mons','FLOETTE_WHITE','IRON_LEAVES','Add an exceptional mechanical meadow guardian while keeping White Flabebe, Eternal Floette and the ordinary White Floette evolution at19. This is added choice, not a forced late-game power gate.'),
('MAGMA_HIDEOUT_4F','land_mons','MAGMAR','GOUGING_FIRE','Add a missing ancient fire species near the volcanic climax. Magby and Magmar remain in multiple earlier fire habitats.'),
('METEOR_FALLS_STEVENS_CAVE','land_mons','DURALUDON','IRON_BOULDER','The deepest steel/stone collection gains a missing future mineral species. Duraludon retains Granite Cave, Route118 and other caves.'),
('ASHEN_WOODS','land_mons','NOCTOWL','URSALUNA_BLOODMOON','Add the otherwise unattainable Bloodmoon form as a distinctive forest discovery. Hoothoot is available on Route103/Dewford Manor and evolves at20; the existing bear family is preserved.'),
]
for idx,(mapid,method,old,new,why) in enumerate(changes,1):
 e=next(e for e in w if e['map']=='MAP_'+mapid);m=e['methods'][method]; hits=[p for p in m['slots'] if p['species']=='SPECIES_'+old];assert len(hits)==1,(mapid,old,hits)
 x=hits[0]
 out.append({'id':f'WILD-{idx:02}','map':e['map'],'name':e['name'],'method':method,'table_field':m['table_field'],'slot':x['slot'],'from':'SPECIES_'+old,'to':'SPECIES_'+new,'weight':x['weight'],'min_level':x['min_level'],'max_level':x['max_level'],'decision':'REVISE','rationale':why,'source':'src/data/wild_encounters.json','authoring_dependency':'For Route117 fishing add a validated optional two-species old_rod_species override to the existing route sheet/builder; all other entries are native land-table edits.','acceptance':'Reconcile configured species; preserve slot weights/levels and all other slots; ordinary-method chances >=5%; verify local encounter terrain/method access; check displaced-family remaining source before materializing.'})
(B/'review/wild-proposals.json').write_text(json.dumps(out,indent=2,ensure_ascii=False)+'\n')
print('Proposed exact wild-slot revisions',len(out))
