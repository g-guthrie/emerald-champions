import json
from pathlib import Path

root = Path(__file__).resolve().parents[2]
folder = root / 'work/wild-habitat-20260908'
# These are individually authored judgments after reading every current method
# roster. No score is computed from richness, overlap, rarity, or prior reviews.
raw = '''
ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS|10|Frillish, Jellicent and Dhelmise make this feel haunted. Hooking Dhelmise at 30% is a worthwhile wreck expedition.
ABANDONED_SHIP_ROOMS_B1F|9|Golisopod, Qwilfish and Barbaracle belong in a broken hull. I would put Lapras outside the ship, though.
ALTERING_CAVE|3|After the League, walking into 100% Zubat is a letdown. The default population does nothing with this location.
ALTERING_CAVE_1F|7|Lucario, Shiinotic and Blissey are useful catches, but most of this postgame roster was already available elsewhere.
ALTERING_CAVE_B1F|7|Guzzlord is exciting. Another cave mix plus a pond containing Poliwhirl and Basculin does not make the whole basement special.
ARTISAN_CAVE_1F|9|Smeargle at 56% fixes the main problem. Aron and Woobat still feel like remnants of the old filler list.
ARTISAN_CAVE_B1F|8|The Smeargle colony is better, but every species here also appears on 1F. This floor still lacks its own attraction.
ASHEN_WOODS|10|Hisuian Growlithe, Buzzwole, Brute Bonnet and Bloodmoon Ursaluna make this a place I would deliberately explore and revisit.
CAVE_OF_ORIGIN_1F|8|Walking Wake is a great ancient-cave discovery. Sableye, Mawile, Carbink and Boldore still occupy almost half the encounters.
CAVE_OF_ORIGIN_DIANCIES_ROOM|10|Carbink, Sableye, Mawile and Glimmora make a convincing gem chamber. I would keep this focused four-species roster.
CAVE_OF_ORIGIN_ENTRANCE|7|Sableye and Mawile again, plus Noibat, Golbat and Noivern. Too much of this sacred cave feels like earlier caves.
CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP1|5|Every species is already in the entrance or main floor. I see little reason to hunt in this optional room.
CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP2|6|Archaludon helps, but the other eleven species repeat the main floor. The room deserves more than one distinguishing catch.
CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP3|7|Clefable and Metang finally suggest a celestial chamber. Most remaining slots still repeat the previous rooms.
DESERT_UNDERPASS|10|Four fossil hybrids, Ditto and three-segment Dudunsparce are discoveries worth making. They suit an unusual buried passage.
DEWFORD_MANOR_1F|9|Gastly, Drowzee, Galarian Slowpoke and Sinistea give the manor character. I would feature Sableye more prominently than Hoothoot.
DEWFORD_MEADOW|10|The pollinators make sense, Butterfree and Ribombee are useful now, and Pheromosa is a thrilling find. I would keep it.
DEWFORD_TOWN|8|Old Rod Mantyke is excellent. A Good Rod dominated by Luvdisc is a disappointing follow-up for this fishing town.
EMBER_PATH|9|Blacephalon, Blaze Tauros and Chandelure are worth coming for. The 10% Noibat slot feels wasted beside them.
EVER_GRANDE_CITY|7|Its Super Rod roster exactly copies Route124. Wailmer and Pyukumuku do not make this feel like the final coastal stop.
FIERY_PATH|10|Torkoal, Koffing, Charcadet and Slither Wing give useful, different team options while still feeling native to a hot tunnel.
GRANITE_CAVE_1F|10|Makuhita, Abra, Aron and Beldum make the first cave visit rewarding. Glimmet and Nacli fit without replacing its Hoenn residents.
GRANITE_CAVE_B1F|9|Sableye, Mawile and Meditite give the descent real value. Timburr is less of a discovery now that it is on Route102.
GRANITE_CAVE_B2F|8|Tinkatink, Duraludon and Deino are great reasons to continue. Too much of the rest repeats B1F, including its common residents.
GRANITE_CAVE_STEVENS_ROOM|9|Beldum, Metang and Lucario make Steven's room rewarding. I would make Beldum, rather than Aron, the leading encounter.
JAGGED_PASS|8|Primeape, Gligar and Mudbray suit the slopes. Bagon, Deino, Jangmo-o and Hakamo-o together feel like excess dragon storage.
LILYCOVE_CITY|8|Wailmer and Cramorant suit the busy bay. The stronger rods largely repeat shellfish and predators found along the surrounding coast.
MAGMA_HIDEOUT_1F|8|Numel, Slugma and the mining Pokémon make a sensible entrance population. I want the deeper rooms to develop this cast.
MAGMA_HIDEOUT_2F_1R|5|Ten of twelve species repeat 1F. Geodude becoming Graveler and Zubat becoming Noibat barely changes the experience.
MAGMA_HIDEOUT_2F_2R|8|Koffing, Grimer and Weezing suggest a toxic chamber. I like that direction, but the Noibat slot adds little.
MAGMA_HIDEOUT_2F_3R|7|Houndour, Houndoom and Salazzle could define this room. Numel, Slugma, Noibat and Graveler repeatedly dilute that idea.
MAGMA_HIDEOUT_3F_1R|8|Camerupt, Darmanitan and Volcarona make this feel more dangerous. Noibat and Graveler still take 21% of the table.
MAGMA_HIDEOUT_3F_2R|7|Chandelure is welcome, but this largely repeats the previous chamber's Camerupt, Magcargo, Weezing and other evolved Fire types.
MAGMA_HIDEOUT_3F_3R|7|Salazzle and Larvesta suggest a different hot ecosystem. Most encounters still replay the same Camerupt/Magcargo/Noibat mix.
MAGMA_HIDEOUT_4F|10|Gouging Fire, Camerupt, Coalossal, Volcarona and Chandelure deliver the intensity I want at the hideout's end.
METEOR_FALLS_1F_1R|9|Lunatone, Solrock, Clefairy and Minior make the cave memorable. The Super Rod's Golduck/Lombre/Quagsire mix is much less distinctive.
METEOR_FALLS_1F_2R|9|Scream Tail and Ferrothorn are worthwhile deeper finds. Fishing still feels like an ordinary river transplanted into the cave.
METEOR_FALLS_B1F_1R|9|Dratini, Dragonair and Kingdra make Surf excellent here. The Super Rod undercuts that with Poliwhirl, Sharpedo and Octillery.
METEOR_FALLS_B1F_2R|9|Roaring Moon, Salamence and Dragonite justify the journey. Finding Lombre and Golduck on the Super Rod is comparatively deflating.
METEOR_FALLS_STEVENS_CAVE|10|The Beldum family, Steven's steels, Iron Crown and Iron Boulder belong together. Almost every catch could start a new team.
MIRAGE_TOWER_1F|10|Sandshrew and Trapinch among Baltoy, Golett, Yamask and Honedge make this feel like a buried ruin with useful early finds.
MIRAGE_TOWER_2F|8|Dwebble and Orthworm are good additions, but Yamask, Golett and Bronzor make most encounters feel like the floor below.
MIRAGE_TOWER_3F|8|Golurk, Claydol and Bronzong sell the old guardians. The mix still spends too much time repeating the lower-floor population.
MIRAGE_TOWER_4F|10|Claydol, Golurk, Bronzong and Cofagrigus finally feel like an assembly of ruin guardians. Darmanitan and Krookodile add muscle.
MIRAGE_TOWER_B1F|9|Chest Gimmighoul is exactly the treasure I want here. The otherwise familiar Sandshrew/Trapinch/relic mix could emphasize that treasure chamber more.
MOSSDEEP_CITY|9|Staryu and Starmie beside the Space Center feel right. I would give Starmie a more prominent Super Rod position.
MT_PYRE_1F|10|Duskull at 22% and Shuppet at 20% make the tower belong to its ghosts. The smaller ghost discoveries complement them.
MT_PYRE_2F|8|Antique Sinistea is a good find, but almost everything else repeats 1F. I would develop the antique-and-candle idea further.
MT_PYRE_3F|10|Litwick and Lampent together at 32% make this feel like a candle-filled floor, with other ghosts supporting the scene.
MT_PYRE_4F|10|Banette and Dusclops at 20% each make the climb feel more dangerous. Mimikyu and the rarer evolved ghosts add useful choices.
MT_PYRE_5F|8|Artisan Poltchageist and Drifblim interest me. Noibat does not, and the rest repeats too much of 4F.
MT_PYRE_6F|10|Sinistcha, Houndstone, Dusknoir and Chandelure give the upper floor the finished, powerful ghost roster it deserves.
MT_PYRE_EXTERIOR|10|Hisuian Zorua and Galarian Corsola belong beside a graveyard. Chimecho, Vulpix and Drifloon complete the atmosphere.
MT_PYRE_SUMMIT|9|Flutter Mane is a superb summit discovery. Growlithe and Vulpix still make too much of the climb feel like the exterior again.
NEW_MAUVILLE_ENTRANCE|9|Magnemite, Voltorb, Klink and Porygon make a convincing power station. Magnezone and Iron Thorns reward checking it properly.
NEW_MAUVILLE_INSIDE|8|Iron Hands is exciting, but most residents repeat the entrance. I would make the machinery and generator population more distinct inside.
PACIFIDLOG_TOWN|9|Alomomola and the fishing birds fit life around the rafts. Super Rod Shellder feels modest for a return visit this late.
PETALBURG_CITY|5|Marill fits these ponds. Tentacruel, Corsola, Dragalge and Sharpedo make the rest feel like a misplaced ocean roster.
PETALBURG_WOODS|8|Shroomish, Slakoth, Caterpie and Ferroseed are good woodland choices. Aipom as the leading Rock Smash encounter feels wrong to me.
PETALBURG_WOODS_2|6|Caterpie, Weedle, Kakuna and Metapod consume 46%. I want stronger bugs in the deeper woods, not so many stages of two families.
PETALBURG_WOODS_3|9|Croagunk, Murkrow, Kartana and Goomy make exploration worthwhile. The pond's Lanturn/Octillery fishing mix is much less convincing.
ROUTE101|8|Mienfoo substantially improves it. Zigzagoon, Poochyena, Sentret and Lillipup still occupy 41%, crowding the more interesting opening choices.
ROUTE102|9|Timburr and Pachirisu alongside Lotad, Marill and Ralts are excellent. The later Super Rod's Lombre is an underwhelming return reward.
ROUTE103|8|Shellos and Growlithe are excellent early tools. Shinx, Mareep, Electrike, Blitzle, Yamper, Toxel and Grubbin crowd seven Electric evolution lines together.
ROUTE104|6|Azurill and Budew help, but Pikipek, Pidgey, Pidove and Fletchling take 36%. After moving Mienfoo, I left this route too bird-heavy.
ROUTE105|8|Crabrawler, Inkay and the shore birds give the land character. Luvdisc leading the Super Rod table is a weak reason to return.
ROUTE106|8|Makuhita, Crabrawler and Hawlucha make this a believable fighting coast. The Luvdisc-led Super Rod roster does not match that excitement.
ROUTE107|9|Arrokuda and the Barraskewda rod discovery give this stretch a clear fast-fish identity. The middle catches could follow that idea more closely.
ROUTE108|9|Frillish and Dhelmise make the approaching wreck feel present. Super Rod Luvdisc and Bruxish contribute much less to that experience.
ROUTE109|9|Sandygast and Pincurchin from beach rocks are discoveries I like. The stronger fishing still repeats nearby Kingler, Shellder and Staryu.
ROUTE110|8|Gulpin, Varoom, Shroodle and Trubbish fit Cycling Road's outskirts. The fishing does not follow the electrical/industrial character nearly as well.
ROUTE111|9|Cacnea, Bramblin, Hippopotas, Gible and Galarian Yamask make a strong desert. The oasis's Octillery-led Super Rod feels borrowed.
ROUTE111_RUINS_EXTERIOR|8|Xatu, Farigiraf and Sandy Shocks are interesting here. The Rockruff/Skiploom/Jumpluff portion feels less connected to the ruins.
ROUTE112|10|Numel and Ponyta establish the volcanic slope; Mankey, Primeape, Tyrogue and Combat Tauros supply useful Fighting choices.
ROUTE113|8|Spinda, Skarmory and Spoink give this ash route its character. Another Mienfoo and assorted fighters draw attention away from them.
ROUTE114|9|Swablu, Zangoose and Seviper preserve the route's appeal. Phanpy and Dratini add good reasons to investigate both cliffs and water.
ROUTE115|7|Tangela, Pancham and Minior interest me, but Jigglypuff, Taillow, Skwovet and the other mammals do not add up to a convincing north shore.
ROUTE116|10|Mime Jr., Munchlax, Riolu and Throh offer different answers before Roxanne. Dreepy remains an appealing long-term investment beside them.
ROUTE117|8|Ditto, Audino, Tandemaus and the unusual Old Rod roster suit the Daycare. Floette, Sunkern and Sunflora coming from rocks do not.
ROUTE118|8|Passimian, Manectric and Duraludon are useful discoveries. Lickitung, Carnivine and Komala make the land feel less deliberately placed.
ROUTE119|10|Tropius, Goomy, Oranguru, Comfey and Amoonguss belong in this rainforest. Raging Bolt and Feebas make it worth exploring thoroughly.
ROUTE120|8|Absol, Shiftry, Morelull and the Pumpkaboo sizes sell the woods. Watchog taking 10% is a slot I would spend differently.
ROUTE121|7|Skuntank, Mabosstiff and Zoroark suggest a good ominous route. Furfrou, Elgyem and Maushold pull it in several unrelated directions.
ROUTE122|9|Frillish and Dhelmise make sense around Mt. Pyre. Kingler and Shellder on the Super Rod still feel like ordinary beach fishing.
ROUTE123|6|My Patrat move preserved the family, but I dislike the result here. Applin, Smoliv and Karrablast/Shelmet deserve more attention than the common mammals.
ROUTE124|8|Mantine and Clamperl make a good approach to Dive. Its Super Rod list is exactly the same as Ever Grande's.
ROUTE125|8|Spheal, Dewgong and Walrein make the cold current believable. Super Rod Luvdisc and Corsola pull the fishing back toward a generic warm coast.
ROUTE126|8|Aqua Tauros is a catch I would want. The surrounding Alomomola/Wishiwashi sea is pleasant, but the rods add little unique to this crater ring.
ROUTE127|9|Mareanie, Toxapex, Pyukumuku and Bruxish make a convincing reef. The Super Rod largely abandons that idea for Kingler and Shellder.
ROUTE128|8|Sharpedo and Jellicent suit the cavern approach. The Wailmer/Wailord/Clawitzer fishing still resembles several neighboring stretches.
ROUTE129|9|Wailmer and Wailord feel like pods here. The Super Rod's Seadra and Octillery do less to establish a deep-sea route.
ROUTE130|9|The Wynaut colony finally makes Mirage Island special. Frillish and Jellicent suit its waters; the two Qwilfish forms dominate the Super Rod too heavily.
ROUTE131|8|Relicanth and Clamperl make a believable old sea. The Super Rod drops that idea for another Qwilfish/Octillery/Seadra list.
ROUTE132|7|Barraskewda fits the rushing current. Surf Magikarp at 25% and a Seadra/Relicanth-led Super Rod do not deliver the same energy.
ROUTE133|7|Spheal and Dragalge suggest a cold current carrying kelp. Clamperl and Luvdisc take 45% of the Super Rod and weaken the return visit.
ROUTE134|8|Clamperl, Huntail and Gorebyss give the trench a real population. Fishing should reinforce them instead of exactly repeating Route131's Super Rod.
RUSTURF_TUNNEL|9|Bagon, Larvitar, Machop and Drilbur make this an exciting early detour. Whismur could be easier to find without sacrificing those options.
SAFARI_ZONE_NORTH|10|Kangaskhan, Scyther and the Alolan forms are catches I would travel for. The larger herbivores make them feel part of a reserve.
SAFARI_ZONE_NORTHEAST|8|Miltank, Tauros and Forretress make sense together. Houndour and Ledian are less compelling headliners for an optional late hunting area.
SAFARI_ZONE_NORTHWEST|9|Rhyhorn/Rhydon, Chansey and the regional finds justify visiting. The pond's Lombre/Golduck fishing is less special than the land.
SAFARI_ZONE_SOUTH|10|Alolan Raichu, regional Meowth, Mr. Mime, Smeargle and Chansey offer a terrific collection of different team-building tools.
SAFARI_ZONE_SOUTHEAST|8|Gligar, Ambipom and Ursaring interest me. Flaaffy and Ariados occupy too much space before the more exciting evolved catches.
SAFARI_ZONE_SOUTHWEST|10|Karrablast/Shelmet and their evolutions belong together. Palpitoad, Croagunk and Goomy make the surrounding marsh feel coherent.
SANDSTREWN_RUINS|9|Great Tusk, Unown, Honedge and the relic Pokémon are well placed. Swanna in the buried cistern is the bit I do not buy.
SANDSTREWN_RUINS_2F|8|Iron Treads and Orthworm suit the buried machinery. Too many ordinary encounters repeat Yamask, Golett and Honedge from earlier rooms.
SANDSTREWN_RUINS_3F|7|After the Stakataka chamber, another common Yamask plus Tinkatink and Gabite feels like a step backward in the discoveries.
SANDSTREWN_RUINS_B1F|10|Stakataka looks like part of the ruin itself. Cofagrigus, Golurk, Bronzong and Aegislash make convincing inhabitants around it.
SCORCHED_SLAB_B1F|6|Noibat, Gurdurr and Machoke do not strongly establish a hot cave. Its Super Rod even duplicates Route102 exactly.
SCORCHED_SLAB_B2F|8|Magmar, Turtonator and Iron Moth make this feel hotter and stranger. Noibat and the ordinary cave fighters still take unnecessary space.
SCORCHED_SLAB_HEATRANS_ROOM|7|Magcargo and Coalossal fit. Boldore, Dugtrio and Slugma make Heatran's chamber feel less impressive than its location promises.
SEAFLOOR_CAVERN_ENTRANCE|8|Relicanth on the Good Rod suits the depths. Magikarp in the Surf table feels like a missed chance to establish the cavern.
SEAFLOOR_CAVERN_ROOM1|7|Golisopod and Toxapex work here. Concentrating Dragalge while retaining Barraskewda and Kingdra on the walking floor was not my best choice.
SEAFLOOR_CAVERN_ROOM2|6|Sharpedo, Basculegion and Kingdra feel misplaced on the cave floor. Zubat and Floatzel do not turn the mix into a distinct room.
SEAFLOOR_CAVERN_ROOM3|10|Dhelmise, Claydol, Bronzong and Sableye make this feel like a strange relic chamber. The narrower roster works.
SEAFLOOR_CAVERN_ROOM4|7|Malamar is a good resident to emphasize. Sharpedo, Barraskewda and Kingdra make too much of the rest feel like a fishing list.
SEAFLOOR_CAVERN_ROOM5|6|I increased Kingdra to 34%, but this is a walking cave floor. I would move that concentration into an actual pool.
SEAFLOOR_CAVERN_ROOM6|8|Barbaracle on the floor and Frillish/Jellicent in the pool distinguish the methods well. The remaining land fish dilute that improvement.
SEAFLOOR_CAVERN_ROOM7|7|Toxapex and the Clamperl/Huntail/Gorebyss pool work. Mawile and Good Rod Luvdisc feel like leftovers from unrelated populations.
SEAFLOOR_CAVERN_ROOM8|7|Golisopod and Clobbopus belong on the floor. Kingdra and Dragalge together at 46% overwhelm those more convincing residents.
SEASPRAY_CAVE|8|Chinchou, Tynamo and the two Stunfisk are interesting cave-water finds. Wailord and the routine beach fishing weaken the setting.
SEASPRAY_CAVE_B1F|10|Swinub, Snover, Sneasel and Frigibax offer different reasons to descend. Smoochum and Cryogonal make the ice pocket more than one evolution line.
SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM|6|Spheal and Seel are right. Surf Snom and rod-caught Cetoddle are not placements I like, and Zubat leads the land roster.
SHOAL_CAVE_LOW_TIDE_ICE_ROOM|10|Alolan Vulpix, Hisuian Sneasel, Galarian Mr. Mime and Iron Bundle make this a proper hidden-room reward.
SHOAL_CAVE_LOW_TIDE_INNER_ROOM|7|Galarian Darumaka is exciting. Cetoddle dominating Surf and reappearing on rods makes the water feel poorly considered.
SHOAL_CAVE_LOW_TIDE_LOWER_ROOM|8|Alolan Sandshrew deserves this room. The other eleven residents repeat too much of the surrounding ice cave.
SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM|6|This offers almost the same Spheal/Swinub/Snorunt/Sneasel mix as the other rooms, without a comparable new discovery.
SKY_PILLAR_1F|8|Claydol, Golurk and Minior fit the old tower. Sableye and Mawile leading yet another cave roster make the entrance too familiar.
SKY_PILLAR_3F|9|Altaria, Noivern, Dragonair and Salamence make the climb worthwhile. I would let the airborne residents define more of the encounters.
SKY_PILLAR_5F|8|The top largely repeats 3F. I want more Altaria, Noivern and Minior here, rather than Banette and Claydol leading again.
SLATEPORT_CITY|9|Clauncher and Skrelp under the docks, with Dhelmise offshore, feel deliberate. The Super Rod could develop those families more convincingly.
SOOTOPOLIS_CITY|8|Gyarados belongs in this crater lake. The Super Rod's Bruxish/Kingler/Qwilfish list feels disconnected from that particular place.
UNDERWATER_ROUTE124|10|Lanturn, Jellicent, Starmie and Dondozo make descending feel like entering another world. Every slot contributes something worth catching.
UNDERWATER_ROUTE126|10|Corsola/Cursola, Clamperl's evolutions and Feebas/Milotic make this feel like an ecosystem, with Palafin as a welcome surprise.
UNDERWATER_SEAFLOOR_CAVERN|10|Dondozo and all three Tatsugiri forms make this a team-building destination. Nihilego and Iron Bundle add the right deep-water strangeness.
VERDANTURF_MEADOW|10|Cottonee, Indeedee and Hatenna offer real team engines. Iron Leaves and Eternal Floette are memorable rewards among the flowers.
VICTORY_ROAD_1F|8|Iron Valiant, Metagross and the final dragons are exciting. Noibat and the several middle evolutions weaken the final-challenge feeling.
VICTORY_ROAD_B1F|8|Iron Jugulis and Rock Smash Tyranitar add value. Eleven land species still repeat 1F, so this does not feel sufficiently different.
VICTORY_ROAD_B2F|7|Even after my rod improvement, Shellos, Noibat and Metang feel modest here when stronger finished Pokémon were already available upstairs.
'''
rows = []
for line in raw.strip().splitlines():
    key, score, reason = line.split('|', 2)
    rows.append({'id': 'MAP_' + key, 'rating': int(score), 'reason': reason})
source = json.loads((folder / 'current-area-rosters.json').read_text())
assert len(rows) == len({r['id'] for r in rows}) == 138
assert {r['id'] for r in rows} == {r['id'] for r in source}
assert all(1 <= r['rating'] <= 10 for r in rows)
(folder / 'ratings_personal.json').write_text(json.dumps(rows, indent=2) + '\n')
print('Authored and coverage-checked all 138 personal ratings')
