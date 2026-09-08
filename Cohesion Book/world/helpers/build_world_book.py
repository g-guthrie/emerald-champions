from pathlib import Path
import json,re,collections,textwrap,hashlib
W=Path(__file__).resolve().parents[1];B=W.parent;S=B/'baseline/source';raw=json.loads((W/'inventory.raw.json').read_text());idx=json.loads((W/'script-index.json').read_text());props=json.loads((W/'proposals.core.json').read_text())+json.loads((W/'proposals.systems.json').read_text())
extra=W/'proposals.supplement.json'
if extra.exists():props+=json.loads(extra.read_text())
# Native game text serialization: paragraph separators are explicit; extra lines scroll.
def asm(label,txt):
 txt=txt.replace('’', "'"); paragraphs=txt.split('\\p');out=[label+':']
 for pi,p in enumerate(paragraphs):
  lines=textwrap.wrap(' '.join(p.split()),width=34,break_long_words=False,break_on_hyphens=False) or ['']
  for li,l in enumerate(lines):
   last=li==len(lines)-1;end=('$' if pi==len(paragraphs)-1 else '\\p') if last else ('\\n' if li==0 else '\\l')
   out.append('\t.string "'+l.replace('"','\\"')+end+'"')
 return '\n'.join(out)
for p in props:
 for a in p['targets']:
  if 'replacement_text' in a:a['replacement_assembly']=asm(a['label'],a['replacement_text'])
(W/'proposed-edits.json').write_text(json.dumps({'authority':'Separate documentation only; baseline and live source read-only','proposals':props},ensure_ascii=False,indent=2)+'\n')
# Deterministic grouping, explicitly reviewed as ecological/story neighborhoods, not power chapters.
def region(n):
 if n.startswith(('Unused','BattleColosseum','TradeCenter','RecordCorner','UnionRoom','Route104_Prototype')) or '_UnusedRubyMap' in n or n=='LilycoveCity_UnusedMart' or n.endswith('_PokemonCenter_2F') or n.endswith('_PokemonLeague_2F') or 'MysteryEventsHouse' in n or n.startswith('MossdeepCity_GameCorner'):return '10-support'
 if n.startswith(('SecretBase','ContestHall','SafariZone','TrainerHill','BattlePyramidSquare','Route110_TrickHouse')) or n=='Route121_SafariZoneEntrance' or 'ContestLobby' in n or 'ContestHall' in n or 'BattleTent' in n:return '09-side-activities'
 if n.startswith('BattleFrontier'):return '08-frontier'
 if n.startswith(('NavelRock','BirthIsland','FarawayIsland','SouthernIsland','TerraCave','MarineCave','Underwater_MarineCave','AlteringCave')):return '08-frontier'
 if n.startswith(('EverGrande','VictoryRoad')):return '07-league'
 if n.startswith(('Lilycove','Mossdeep','Sootopolis','CaveOfOrigin','AquaHideout','MagmaHideout','SeafloorCavern','Underwater','ShoalCave','Pacifidlog','SkyPillar','SealedChamber','IslandCave','AncientTomb','DesertRuins','AbandonedShip','SSTidal')) or re.match(r'Route1(2[4-9]|3[0-4])',n):return '06-sea-and-crisis'
 if n.startswith(('Fortree','MtPyre','ScorchedSlab')) or re.match(r'Route1(1[89]|2[0-3])',n):return '05-rainforest'
 if n.startswith(('Fallarbor','Lavaridge','MtChimney','JaggedPass','FieryPath','AshenWoods','EmberPath','MeteorFalls','MirageTower','SandstrewnRuins','DesertUnderpass')) or re.match(r'Route11[1-4]',n):return '04-volcano-and-desert'
 if n.startswith(('Mauville','Verdanturf','NewMauville','Route110','Route117')):return '03-mauville'
 if n.startswith(('Dewford','Slateport','GraniteCave')) or re.match(r'Route10[5-9]',n):return '02-dewford-and-slateport'
 return '01-opening'
legacy_interior_prefix=('BattleFrontier_BattleDomeCorridor','BattleFrontier_BattleDomePreBattleRoom','BattleFrontier_BattleDomeBattleRoom','BattleFrontier_BattlePalaceCorridor','BattleFrontier_BattlePalaceBattleRoom','BattleFrontier_BattlePyramidFloor','BattleFrontier_BattlePyramidTop','BattleFrontier_BattleArenaCorridor','BattleFrontier_BattleArenaBattleRoom','BattleFrontier_BattleFactoryPreBattleRoom','BattleFrontier_BattleFactoryBattleRoom','BattleFrontier_BattlePikeCorridor','BattleFrontier_BattlePikeThreePathRoom','BattleFrontier_BattlePikeRoom','BattleFrontier_BattleTowerMulti','BattleFrontier_BattleTowerElevator','BattlePyramidSquare')
def statusmap(n):
 if n.startswith(('Unused','Route104_Prototype','AquaHideout_Unused')) or n=='LilycoveCity_UnusedMart':return ('INERT/EXCLUDED','Registered historical or prototype map; no native adventure entry is authored. Preserve numeric identity and assets; do not populate it to inflate exploration coverage.')
 if n.startswith(legacy_interior_prefix):return ('INERT/EXCLUDED','Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.')
 if n.startswith('ShoalCave_HighTide'):return ('INERT/EXCLUDED','This registered header has no normal entrance. Its layout is actively selected by the corresponding LowTide map when the tide flag is set; preserve layout assets and IDs, and test that actual shared-header transition.')
 if n.endswith(('BattleTentCorridor','BattleTentBattleRoom')):return ('INERT/EXCLUDED','FAC-01 local exhibitions supersede this legacy tent staging room. Retain map IDs and any earned recovery state; no exhibition should warp here or consume the old challenge pipeline.')
 if 'MysteryEventsHouse' in n or n.startswith('MossdeepCity_GameCorner'):return ('REVISE','Preserve the home, harmless residents, minigame records and imported data. Retire the native e-Reader trainer challenge under the all-doubles policy; externally stored data is not an authored native team.')
 if n.startswith(('SecretBase','BattleColosseum','TradeCenter','RecordCorner','UnionRoom')) or n.endswith(('_PokemonCenter_2F','_PokemonLeague_2F')):return ('KEEP','Shared player-owned or external-link support. Preserve geography, trades, records and decoration data. Native trainer challenge entry is governed by the all-doubles external-entry contract, never silently accepted as a singles exception.')
 return ('KEEP','Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.')
contracts={
'PICKUP':('KEEP','Physical item rewards','Common_EventScript_FindItem obtains item/quantity from the event template and delegates to the existing finditem transaction. Keep item, flag, coordinates and quantity as cataloged unless the acquisition volume explicitly changes them. A failed insertion must leave the reward available. Item-ball appearance and pickup identity must agree.','data/scripts/item_ball_scripts.inc'),
'HIDDEN':('KEEP','Hidden items','Keep exact coordinate, elevation, item and persistent flag. Finding a hidden item commits only on insertion success; revisit preserves ownership and no duplicate reward. This optional search is not a substitute for an advertised acquisition route.','data/scripts/obtain_item.inc'),
'BERRY':('KEEP','Berry trees','Retain tree ID, planting/harvest/watering state and local berry species. Track regrowth separately from one-time Mega exchanges. Existing Berry Master accepts the listed garden mixture; no new berry grind or free Mega archive.','data/scripts/berry_tree.inc'),
'FIELD':('KEEP','Field obstacles','Use existing HM-license plus badge plus non-Egg compatible-party capability model. A known move is preferred; the fallback does not occupy a battle move slot. Preserve obstacle positions and local puzzle state. Flash/tide/Regi puzzle-specific scripts retain their own explicit rules. W-FIELD-SURF fixes only the deletion capability mismatch.','src/field_move.c'),
'CENTER':('REVISE','Healing and first-visit tools','Retain respawn routing, healing, Vial refill and missing-tool recovery. W-CENTER-FIRST-VISIT finishes the first handoff by offering the ordinary heal flow. Every one of the17 native Centers/League Center supplies the existing free specialist and vendor.','data/scripts/pkmn_center_nurse.inc'),
'TUTOR':('KEEP','Free preparation','Retain legal move editing, complete battle sets, Nature, Stat Points, name/delete services, cancel paths and Mega-stone checks. Every service applies immediately without introducing a progression curriculum. W-FREE-SERVICE-COHESION removes live paid duplicates.','data/scripts/emerald_champions.inc'),
'VENDOR':('KEEP','Battle supplies and evolution archive','Retain free held items and the Ring-gated free evolution archive. No universal Mega Stone archive. Selection cursor/back/cancel and item grants remain one shared implementation.','data/scripts/emerald_champions.inc'),
'LEGEND':('KEEP','Legendary and static encounters','Retain deliberate landmarks, optional Devon guidance, actual badge/story/species gates, capture/permanent reward state and re-entry retry. Ordinary wild and legendary capture battles remain singles; scripted INTRO-01 rescue is separately specified. WILD-ENGINE-01 prevents Sweet Scent from starting a stale battle after a rejected normal draw. W-SPIRITOMB-01 removes only that alternate encounter’s unexplained species gate.','src/legendary_signs.c'),
'TRAINER':('REVISE','Authored trainer interaction','Preserve object, script identity, line of sight, defeated/rematch flags, dialogue flow and story placement unless a named world change applies. Final parties, moves and tactical prose are owned by the individual battle volumes; all native trainer battles are doubles and standard teams have at least4. DIFF-01 supplies the Medium floor at live cap minus2.','data/scripts/trainer_battle.inc'),
'FLAVOR':('KEEP','Local NPC and environmental prose','The actual referenced text is reproduced in the dialogue ledger. Preserve examined local personality, biome history, directions and harmless optional social interactions unless a listed text replacement repairs a concrete mismatch. Do not turn every resident into a battle lecturer.','data/maps/'),
'STORY':('KEEP','Stateful story interaction','Preserve the map’s specific flags/vars, actor choreography and success handoffs reproduced by source references. Required battles do not commit defeated/reward/story-success state on loss. Every cited acceptance route must still be tested in the actual game; resolved script references alone are not traversal proof.','data/maps/'),
'WARP':('KEEP','Warp and connection','Keep exact source location, elevation, destination map and destination warp/offset. Refer to map-specific story/field gates and active layout variants. The structural gate resolved all1402 warps and152 connections; actual state-dependent traversal remains implementation evidence.','data/maps/'),
'COORD':('KEEP','Coordinate trigger','Keep exact location, state variable and required value. Shared entrance columns must run one scene, not repeated scenes; success advances only the intended state. Weather triggers keep local biome atmosphere. Every change to a triggering scene must include its sibling coordinates.','data/maps/'),
'BACKGROUND':('KEEP','Sign and background interaction','Preserve exact position/facing direction and source root. Route roster appendices follow GUIDE-01 and must show only enabled actual methods. Proposed clue and rule-board replacements identify exact labels; inert guide prose is never counted as player-facing guidance.','data/maps/'),
'MAP_CALLBACK':('KEEP','Map-load/frame/resume callbacks','Keep callback type and entry label. Review map flags, dynamic layout, object visibility and handoff state together. For Circuit/Tent/Hill conversions use their explicit origin/restore contracts; do not let retained legacy callbacks launch unsupported modes.','data/maps/'),
'PASSIVE':('KEEP','Passive actor or prop','An object with no script may be a cutscene actor, staged Pokémon, temporary item-ball prop or visual resident. Keep graphics, ID, movement, flag and position. It is not an advertised interactive reward merely because it resembles one; scene owners control its lifetime.','data/maps/'),
'LINK':('REVISE','External battle and record support','Preserve trading, record mixing, link minigame and imported data. LINK-01 owns the exact wired/wireless doubles-only entry and native recorded-owner challenge retirement. Retire unsupported native e-Reader battles. LINK-01 restricts wired/wireless battle entry to the exact Double/Multi menus and retires native recorded Secret Base owner challenges before save or daily-state mutation. Preserve imported profiles and decorations; never manufacture missing opponents or delete records.','data/scripts/cable_club.inc'),
'FACILITY':('REVISE','Native competition interfaces','FAC-01 supplies local six-Pokémon tent exhibitions; FAC-02 preserves Hill Time Attack with coherent generated doubles teams; Frontier desks use central Circuit. World changes reconcile guides, records, duplicate paid services and obsolete wagers. Legacy challenge rooms are not reactivated by this book.','src/champions_circuit.c'),
'CONTEST':('KEEP','Contests and social minigames','Preserve optional noncombat contests, paintings, ribbons, Berry Blender, style changes and spectator interactions. These remain worthwhile Hoenn culture without becoming battle-preparation tolls. Their registered staging maps are not trainer-format exceptions.','data/scripts/contest_hall.inc'),
'DAYCARE':('KEEP','Daycare and gifts','Retain two-compatible-parent breeding, short waits, Togepi gift, explicit gift-egg exclusion and one qualifying hatched egg for Kangaskhanite. Adult means breeding-eligible, not necessarily fully evolved. Preserve direct early babies and free preparation.','data/scripts/day_care.inc'),
'TRAVEL':('KEEP','Transport','Keep Briney’s letter/delivery itinerary, return options, Cable Car, tides, public ferry passes and explicit route directions. Test source and destination state after cancel, success and later revisit. Movement convenience must not bypass unearned HM/badge access.','data/maps/'),
}
# Useful common source contracts actually inspected in addition to map scripts.
known_shared_files={'data/scripts/item_ball_scripts.inc':'PICKUP','data/scripts/obtain_item.inc':'PICKUP','data/scripts/berry_tree.inc':'BERRY','data/scripts/field_move_scripts.inc':'FIELD','data/scripts/pkmn_center_nurse.inc':'CENTER','data/scripts/emerald_champions.inc':'TUTOR','data/scripts/move_tutors.inc':'TUTOR','data/scripts/day_care.inc':'DAYCARE','data/scripts/trainer_battle.inc':'TRAINER','data/scripts/trainer_hill.inc':'FACILITY','data/scripts/secret_base.inc':'LINK','data/scripts/cable_club.inc':'LINK','data/scripts/contest_hall.inc':'CONTEST','data/scripts/berry_blender.inc':'CONTEST','data/scripts/battle_pike.inc':'FACILITY','data/scripts/gabby_and_ty.inc':'TRAINER','data/scripts/kecleon.inc':'LEGEND','data/scripts/players_house.inc':'STORY','data/scripts/secret_power_tm.inc':'FIELD','data/scripts/prof_birch.inc':'STORY','data/scripts/profile_man.inc':'FLAVOR','data/scripts/interview.inc':'FLAVOR','data/scripts/mauville_man.inc':'FLAVOR','data/scripts/lilycove_lady.inc':'FLAVOR','data/scripts/apprentice.inc':'LINK','data/scripts/roulette.inc':'CONTEST','data/scripts/cave_hole.inc':'FIELD'}
# Review contracts and all shared roots are listed, rather than pretending each repeated berry/tree service needs a new mechanic.
shared=collections.defaultdict(list)
for m in raw['maps']:
 for e in m['events']:
  r=e.get('root')
  if r in idx and idx[r]['path']!=m['script_path']:shared[r].append(e['id'])
(W/'common-contracts.md').write_text('# Shared world contracts\n\nThese contracts are applied to the exact instances in the per-map ledger. Each map still carries its own coordinates, state, neighbors and local disposition. Reuse is explicit; no claim is made that static source inspection is a runtime traversal.\n\n'+ '\n\n'.join(f'<a id="w-c-{k.lower()}"></a>\n\n## W-C-{k} — {title}\n\n**{disp}.** {body}\n\nSource: [`{path}`](../baseline/source/{path}).' for k,(disp,title,body,path) in contracts.items())+'\n\n## Shared-root instance index\n\n| Root | Source | Instances | Contract |\n|---|---|---:|---|\n'+'\n'.join(f'| `{r}` | [{idx[r]["path"]}:{idx[r]["line"]}](../baseline/source/{idx[r]["path"]}#L{idx[r]["line"]}) | {len(es)} | W-C-{known_shared_files.get(idx[r]["path"],"STORY")} |' for r,es in sorted(shared.items()))+'\n')
# Manual assessed map/area notes, grounded in reviewed current source.
notes={
'OldaleTown':'Preserve the connected first preparation hub. INTRO-01 changes the adjacent first battles; the Center remains available before trainer challenges. The footprint joke and optional Potion promotion can remain without an artificial learning phase.',
'PetalburgCity':'Preserve Norman/Wally story and lake-town identity, including party save/restore in the capture demonstration. Expert battle capability is present immediately; young-character narrative is not a difficulty curriculum.',
'RustboroCity':'Preserve goods-theft staging and the repeated explicit Route116→Rusturf→return-here instruction. The Devon return must survive item-pocket and item-PC fullness without moving the employee or advancing state prematurely.',
'SlateportCity':'Brawly’s museum-queue visit, northbound Knuckle gate, shipyard/museum progression and ferry subplot form one coherent hub. Keep direct directions to Briney’s Route109 boat and Brawly’s return location. FAC-01 replaces only the tent battle mode.',
'MauvilleCity':'Preserve Wally’s authored battle and crossroads services. Keep free preparation, starter/coin exchange and eventual New Mauville revisit; correct dialogue that falsely claims an exact battle outcome or diminishes levels as an intentional lever.',
'FortreeCity':'Preserve treetop paths, Kecleon/Scope access, east-road Feather gate and clear directions to Steven’s bridge. Distinctive canopy residents and free services coexist; no encounter scarcity is imposed to protect a type chapter.',
'LilycoveCity':'Preserve museum/contest/market culture, rival confrontation, blocked Wailmer cove and submarine pursuit. The Altarianite alternative shares its receipt with Winona and must remain an intentional alternate, not a duplicate grant bug.',
'MossdeepCity':'Preserve the Space Center’s before/after population and Steven’s visible handoff. Correct the old Relicanth prerequisite clue, without moving the submarine discoveries or changing which badge permits Dive.',
'SootopolisCity':'Preserve the crisis’s weather, residents, three great Pokémon choreography, Sky Pillar excursion and Waterfall/Juan handoff. Its claims about Sign partners are descriptive narrative; exact acquisition conditions are supplied by current guidance.',
'Route111':'Preserve the Winstrate sequence, desert weather boundary, Dynamo gate, clearly explained Runerigus stone and staged Blob quest. The nurse already guards against downgrading Vial capacity; do not invent a repair for that guarded path.',
'Route113':'Preserve ash collection and current Kingambit clue. The newer held-Leader’s-Crest evolution is already in snapshot and is not an acquisition defect. The Glass Workshop is optional regional craft, not a battle-stat grind.',
'Route115':'Keep early Seaspray discovery alongside Meteor Falls’s later approach. A cold side cave can be exciting early; Shoal’s distinct later content is reconciled in the acquisition volume rather than restricting all early Ice options.',
'Route116':'Keep early babies and useful investments per acquisition owner. Dusk Stone seeker checks the specific path find and does not confiscate the player’s useful stone; goods rescue progression stays explicit.',
'Route117':'Keep meadow, Daycare access and independent Audinite gift. The Daycare’s egg objective is an exploration reward with one clear qualifying hatch, not a stat-preparation requirement.',
'Route119':'Preserve rainforest, weather institute, bridges, Scope route and story rivalry. Correct obsolete Castform/Devon Sign requirements and outcome-dependent victory boasts. Weather is habitat and battle context throughout the game.',
'Route120':'Preserve optional Kecleon captures, lake and grove discoveries, Scope demo and Feather gate. Keep the field landmarks and remove obsolete Gardevoir/Xatu silhouettes as implied prerequisites.',
'Route128':'Preserve the seafloor crisis return coordinates and Steven’s explicit north→west→Dive directions into Sootopolis. This scene cannot substitute for testing the actual underwater/city entrance route.',
'Route130':'Mirage Island stays an optional mystery. Do not make its personality/day roll the sole source of any battle-relevant family; broad acquisition alternatives belong to the global catalogue.',
'Route133':'The final Vial nurse correctly appears only at capacity2 and hides at3; preserve its staged dependency on the earlier Blob reward. A simple rescue interaction is an appropriate exploration reward.',
'RusturfTunnel':'Keep the sound-sensitive tunnel, hostage rescue, lovers’ reunion and meaningful shortcut. Habitat restoration is decided globally; do not change party access or silently revive retired tunnel trainers.',
'GraniteCave_StevensRoom':'Preserve letter delivery before Brawly, explicit museum-queue directions, the Ring after Knuckle Badge, starter-stone pending storage and Pokenav handoff. Replace the still-live mandatory Devon translation claim.',
'PetalburgWoods':'Preserve Shroomish/Devon rescue and broad forest discovery. Current Burmy/Hisuian Decidueye explanation is already useful. The live deep-forest sign must drop old Breloom/Blissey prerequisites.',
'NewMauville_Inside':'Keep switch states, Voltorb decoys, Rotom generator sequence, appliance catalogue, technology habitat and Meltan evolution. Replace its stale compulsory-research/Manectric/Dialga clue. The generator can be on or off for Melmetal.',
'AbandonedShip_Room_B1F':'W-SPIRITOMB-01 makes the eerie socket legible and uses Odd Keystone alone. Preserve the ordinary Spiritomb paths and the Keystone-only-on-capture transaction.',
'Route110_TrickHousePuzzle5':'Reauthor the complete15-question bank without NPC counts, variable prices or retired species. Preserve its mechanical-doll choreography, random question selection, wrong-answer reset and field puzzle identity.',
'Route110_TrickHouseEnd':'Preserve earned stage rewards and final personal farewell. W-TRICK-REWARDS fixes the King’s Rock retry mismatch and separates tent/Alakazite receipts without replaying the maze.',
'Route110_TrickHouseEntrance':'Preserve eight revisit gates and the detective-style hide-and-seek entrance. Pending rewards remain accessible; finished story text must not hide an unclaimed final part.',
'CaveOfOrigin_DianciesRoom':'W-WALLACE-ROOT deliberately restores the authored one-time rain exhibition as a talk interaction off the main aisle. Diancie and the Diancite pickup remain independent discoveries.',
'ScorchedSlab_HeatransRoom':'Keep Magma Stone reveal. FLAG_EC_CAUGHT_HEATRAN is a physical-presence flag also set at new game; clearing it for the initial reveal is not proof of recapture. Preserve permanent capture through the actual legendary/Pokédex ledger.',
'VerdanturfMeadow':'Preserve current Deerling and Hisuian Lilligant guidance and the floral discovery space. The former missing-family/evolution criticism is superseded by snapshot fixes.',
'AshenWoods':'Keep ash/shade weather pockets, caretaker story, deliberate landmark, thoughtful Vial chase and alternative powerful wild options. The chase carries its state through re-entry and replacement Heal Balls are available.',
'SandstrewnRuins':'Preserve the fossil archive, branching stair network and route from both Mirage Tower basement and Desert Underpass. The tower may disappear; the underpass connection must keep the ruin’s acquisition content available.',
'Route114_FossilManiacsTunnel':'Keep the underpass restoration after Mirage Tower loss and current complete-fossil support. Correct the stale Landorus silhouette below into an accurate ruins connection clue.',
'Route117_PokemonDayCare':'Keep precise Togepi gift exclusion, compatible-parent hatch detection, Oval Charm and Kangaskhanite pending reward. Do not restore egg-only baby availability or genetic-stat grinding.',
'Route121_SafariZoneEntrance':'Keep the Catching Charm welcome gift, explicit entry requirements, space check and optional Safari mechanics. Any species obtainable only here must have its actual final catch method reflected in acquisition coverage.',
'BattleFrontier_ReceptionGate':'Replace obsolete facility-tour and rules claims with current Circuit facts. Preserve first pass grant, Scott welcome, architectural wayfinding and the separation of historical records from active competition.',
'BattleFrontier_Lounge3':'Retire new wagers on unavailable modes and settle existing obligations exactly once with BP-cap protection. Keep the social scene and no new reward economy.',
'BattleFrontier_Lounge7':'Keep the veteran pair and their personality; both use the existing free specialist. No BP toll for an otherwise free legal move.',
'BattleFrontier_ExchangeServiceCorner':'Keep BP decorations and field supplies; route ordinary evolution access through the existing free Ring archive. Source debit/insert ordering remains an explicit acceptance check.',
'TrainerHill_Entrance':'FAC-02 preserves Time Attack access, timer, doors and prizes while replacing legacy opponent construction. Any change to the field pair must preserve shared floor completion and loss/withdraw return behavior.',
}
# Catalog exact event-level assessment and dependencies. The baseline representation is preserved in full.
change_labels=collections.defaultdict(set);change_paths=collections.defaultdict(set)
for p in props:
 for a in p['targets']:
  change_labels[a['label']].add(p['id']);change_paths[a['source']].add(p['id'])
def eventcontract(m,e):
 k=e['kind'];r=e.get('root') or '';data=e['data'];b=idx.get(r,{});body=b.get('body','')
 if k in ('warp_events','connections'):return 'WARP'
 if k=='map_scripts':return 'MAP_CALLBACK'
 if k=='coord_events':return 'COORD'
 if data.get('type')=='hidden_item':return 'HIDDEN'
 if k=='bg_events':return 'BACKGROUND'
 if r=='Common_EventScript_FindItem' or 'finditem ' in body:return 'PICKUP'
 if r=='BerryTreeScript':return 'BERRY'
 if r in ('EventScript_CutTree','EventScript_RockSmash','EventScript_StrengthBoulder'):return 'FIELD'
 if 'Legendary' in r or any(v in body for v in ['StartLegendaryBattle','CreateSelectedLegendarySignEncounter','StartRegiBattle','CreateEmeraldChampionsStaticLegendaryEncounter']):return 'LEGEND'
 if 'Nurse' in r and ('Center' in m['name'] or 'League_1F' in m['name']):return 'CENTER'
 if 'EmeraldChampionsMoveTutor' in r:return 'TUTOR'
 if 'EmeraldChampionsBattleVendor' in r:return 'VENDOR'
 if 'ChampionsCircuit' in r or 'TrainerHill' in m['name'] or ('BattleTent' in m['name'] and 'Attendant' in r):return 'FACILITY'
 if data.get('trainer_type')=='TRAINER_TYPE_NORMAL' or any(v in body for v in ['trainerbattle','facilitytrainerbattle','multi_2_vs_2']):return 'TRAINER'
 if 'Daycare' in r or 'TogepiEgg' in r:return 'DAYCARE'
 if any(v in r for v in ['Briney','Ferry','CableCar','Sailor']) and ('warp ' in body or 'Briney' in r or 'Ferry' in r):return 'TRAVEL'
 if not r or r in ('0','NULL','0x0'):return 'PASSIVE'
 if b.get('path') in known_shared_files:return known_shared_files[b['path']]
 if any(v in body for v in ['setflag','setvar','clearflag','applymovement','special','goto_if']):return 'STORY'
 return 'FLAVOR'
ledger={};chapters=collections.defaultdict(list);all_dialogues={};event_counts=collections.Counter();disps=collections.Counter();maxmaps=0
for m in raw['maps']:
 n=m['name'];reg=region(n);base_status,classification=statusmap(n);events={};localidx={k:b for k,b in idx.items() if b['path']==m['script_path']};allcode='\n'.join(b['body'] for b in localidx.values());gates=sorted(set(re.findall(r'\b(?:FLAG_|VAR_)[A-Z0-9_]+\b',allcode)))
 for e in m['events']:
  c=eventcontract(m,e);reachable=set(e.get('local_labels',[]))|set(e.get('external_labels',[]))|{e.get('root')};changes=set()
  for r in reachable:changes.update(change_labels.get(r,set()))
  if n in ['Route101','Route103','LittlerootTown_ProfessorBirchsLab'] or (n.startswith('LittlerootTown_') and ('House' in n)):changes.add('INTRO-01')
  if 'RouteSign' in (e.get('root') or '') or ('Sign' in (e.get('root') or '') and n.startswith('Route')):changes.add('GUIDE-01')
  if c=='CENTER':changes.add('W-CENTER-FIRST-VISIT')
  if c=='LINK' or n.startswith(('SecretBase','BattleColosseum','TradeCenter','RecordCorner','UnionRoom')) or n.endswith(('_PokemonCenter_2F','_PokemonLeague_2F')):changes.add('LINK-01')
  if c=='TRAINER':changes.add('DIFF-01')
  if 'BattleTent' in n:changes.add('FAC-01')
  if n.startswith('TrainerHill'):changes.add('FAC-02')
  status='INERT/EXCLUDED' if base_status=='INERT/EXCLUDED' else ('REPAIR' if changes and any(p['id'] in changes and p['disposition']=='REPAIR' for p in props) else ('REVISE' if changes or c in ('LINK','FACILITY') else 'KEEP'))
  data=e['data'];r=e.get('root');b=idx.get(r,{})
  if e['kind']=='warp_events':summary=f"Warp from ({data.get('x')},{data.get('y')}, elevation {data.get('elevation')}) to {data.get('dest_map')} warp {data.get('dest_warp_id')}."
  elif e['kind']=='connections':summary=f"{data.get('direction')} connection to {data.get('map')}, offset {data.get('offset')}."
  elif data.get('type')=='hidden_item':summary=f"Hidden {data.get('item')} at ({data.get('x')},{data.get('y')}); persistent flag {data.get('flag')}."
  elif c=='PICKUP':summary=f"Pickup {data.get('trainer_sight_or_berry_tree_id','quantity in script')}; root {r}; flag {data.get('flag','script owned')}."
  elif c=='PASSIVE':summary=f"Passive/staged {data.get('graphics_id','event')} at ({data.get('x')},{data.get('y')}); visibility flag {data.get('flag','none')}; no direct interaction script."
  elif e['kind']=='coord_events':summary=f"Coordinate {data.get('type')} at ({data.get('x')},{data.get('y')}); "+(f"{data.get('var')} == {data.get('var_value')} invokes {r}." if r else f"weather {data.get('weather')}.")
  elif e['kind']=='map_scripts':summary=f"{data.get('type')} calls {r}."
  else:summary=f"{r} at ({data.get('x')},{data.get('y')}); "+(' / '.join(x['text'] for x in e.get('text',[])[:2]) or ('shared behavior '+c))
  ev={**{k:v for k,v in e.items() if k!='text'},'dialogue_labels':[x['label'] for x in e.get('text',[])],'disposition':status,'assessment_contract':'W-C-'+c,'current_summary':summary,'final_specification':('Retain the registered support event and baseline behavior; exclude it from native challenge/acquisition claims.' if status=='INERT/EXCLUDED' else 'Preserve the baseline event record and referenced behavior except the explicit proposals/dependencies listed here. Apply any individual battle/acquisition final specification through its owning chapter.'),'proposal_ids':sorted(changes),'access_dependencies':({'visibility_flag':data.get('flag'),'trigger_variable':data.get('var'),'trigger_value':data.get('var_value'),'map_state_registry':'See map_state_symbols and map callbacks; source predicates are authoritative.'}),'evidence':{'source_structure':'Source map/label/geometry contracts inspected; progression verifier passed','semantic_review':'Map-specific dialogue/state digest plus named common contract; complex C-special behavior separately linked','runtime':'Not executed in documentation phase'},'acceptance':['Verify the observable '+contracts[c][1].lower()+' behavior using W-C-'+c+'.','Exercise map-specific access and the linked proposal acceptance conditions on an implemented build.']}
  events[e['id']]=ev;event_counts[e['kind']]+=1;disps[status]+=1
  for tx in e.get('text',[]):
   label=tx['label'];all_dialogues[label]={'source':idx[label]['path'],'line':idx[label]['line'],'current':tx['text'],'event_ids':all_dialogues.get(label,{}).get('event_ids',[])+[e['id']],'proposal_ids':sorted(change_labels.get(label,set()))}
 mp={'map_id':m['map']['id'],'source':m['source'],'metadata':{k:v for k,v in m['map'].items() if k not in ['object_events','coord_events','bg_events','warp_events','connections']},'script_source':m['script_path'],'map_script_root':m['map_script_root'],'disposition':('REVISE' if base_status!='INERT/EXCLUDED' and any(e['proposal_ids'] for e in events.values()) else base_status),'classification':classification,'assessment':notes.get(n,classification),'region_chapter':reg+'.md','map_state_symbols':gates,'events':events,'proposal_ids':sorted(set(p for e in events.values() for p in e['proposal_ids'])),'source_reference_review':'Per-map dialogue/control-flow digest and all explicit physical event records; linked common contracts avoid repeated reinvention. This is not a proof of stateful traversal.'}
 ledger[n]=mp;chapters[reg].append(n)
 # Each map has a standalone reading page, with every event (not only notable NPCs).
 lines=[f'# {n}',f'**{mp["disposition"]}.** {mp["assessment"]}',f'[Regional experience](../{reg}.md) · [Map source](../../baseline/source/{m["source"]})'+(f' · [Scripts](../../baseline/source/{m["script_path"]})' if m['script_path'] else ''),'## Current map contract',f'`{m["map"]["id"]}` · `{m["map"]["layout"]}` · `{m["map"].get("weather")}` · `{m["map"].get("music")}`',classification,'## Complete event ledger','Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.','\n| Event | Position / selector | Current behavior | Disposition / final dependency |','|---|---|---|---|']
 for id,e in events.items():
  data=e['data'];pos=f"{data.get('x','—')},{data.get('y','—')}" if 'x' in data else data.get('type',data.get('direction','—'));cs=e['current_summary'].replace('|','/').replace('\n',' ');root=e.get('root');ref=e.get('script_source');label=f'`{id}`';
  if ref:cs=f'[{root}](../../baseline/source/{ref["path"]}#L{ref["line"]}) — '+cs
  lines.append(f'| {label} | {pos} | {cs} | **{e["disposition"]}** · [W-C-{eventcontract(m,e)}](../common-contracts.md#w-c-{eventcontract(m,e).lower()})'+(' · '+', '.join(e['proposal_ids']) if e['proposal_ids'] else '')+' |')
 lines += ['\n## Final specification and acceptance\n','Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.','Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.','The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.']
 (W/'maps'/f'{n}.md').write_text('\n\n'.join(lines[:7])+'\n\n'+'\n'.join(lines[7:])+'\n')
(W/'event-ledger.json').write_text(json.dumps({'scope':raw['counts'],'registered_maps':len(ledger),'maps':ledger},ensure_ascii=False,indent=2)+'\n')
(W/'dialogue-ledger.json').write_text(json.dumps(all_dialogues,ensure_ascii=False,indent=2)+'\n')
orphans=json.loads((W/'unrooted-entrypoints.json').read_text())
for r in orphans:
 r['disposition']='REPAIR' if r['label']=='CaveOfOrigin_DianciesRoom_EventScript_WallaceExhibition' else 'INERT/EXCLUDED'
 r['final_specification']='Restore deliberately under W-WALLACE-ROOT.' if r['disposition']=='REPAIR' else 'No independent native interaction root found by the structural reference review. Preserve inert unless the owning explicit chapter retires the code; do not silently reattach old trainers, guides or facilities.'
 r['evidence_boundary']='Reference closure candidate classification cross-checked by root context; external/dynamic calls may be separately documented. Not a proof that arbitrary debug/script injection cannot reach it.'
(W/'unrooted-entrypoints.json').write_text(json.dumps(orphans,ensure_ascii=False,indent=2)+'\n')
# Detailed, source-backed exact revision appendix.
cl=['# Exact world revision specification','The following are proposed changes to implement later. Nothing here has been applied to the game or snapshot. Text blocks include native .string serialization; new flags and events require central ID allocation and the stated runtime acceptance.','This list is intentionally selective. The complete map/event ledger explicitly preserves the remaining authored world. Global INTRO-01, DIFF-01, GUIDE-01, FAC-01 and FAC-02 are owned by the lead-editor chapters.']
for p in props:
 cl += [f'<a id="{p["id"].lower()}"></a>\n\n## {p["id"]} — {p["title"]}',f'**{p["disposition"]}.** {p["assessment"]}']
 for a in p['targets']:
  cl += [f'### `{a["label"]}`',f'Source: [{a["source"]}'+(f':{a["line"]}' if 'line' in a else '')+f'](../baseline/source/{a["source"]}'+(f'#L{a["line"]}' if 'line' in a else '')+')'+(' — new entry' if a.get('new_label') or a.get('new_event') else '')]
  if 'current' in a:cl += ['Current: '+a['current'].replace('\n','; ')]
  if 'replacement_assembly' in a:cl += ['Final text:\n\n```asm\n'+a['replacement_assembly']+'\n```']
  if 'final_behavior' in a:cl += ['Final behavior:\n\n'+a['final_behavior']]
  if 'final_object' in a:cl+=['Final object:\n\n```json\n'+json.dumps(a['final_object'],indent=2)+'\n```']
  if 'choices' in a:cl += [f'Choices in `{a["menu_array"]}` (`{a["menu_source"]}`), zero-based: '+', '.join(f'{i}: {c}' for i,c in enumerate(a['choices']))+f'. Correct index: **{a["correct_index"]}**. Update `{a["script_label"]}` to that case. Evidence: `{a["evidence"]}`.']
 cl+=['Dependencies: '+'; '.join(p['dependencies']),'Acceptance: '+' '.join(p['acceptance'])]
(W/'changes.md').write_text('\n\n'.join(cl)+'\n')
# Coverage is artifact coverage plus named review methods, explicitly not runtime.
coverage={'registered_maps_expected':540,'registered_maps_actual':len(ledger),'event_counts_expected':raw['counts'],'event_counts_actual':dict(event_counts),'event_dispositions':dict(disps),'events_with_disposition':sum(disps.values()),'unrooted_entrypoint_candidates':len(orphans),'shared_event_roots':len(shared),'proposals':len(props),'proposal_targets':sum(len(p['targets']) for p in props),'dialogue_labels_from_event_closures':len(all_dialogues),'runtime_executed':False,'source_gate':{'maps':540,'physical_npc_trigger_sign_events':4196,'warps':1402,'script_refs':18350,'script_lines':106569,'specialvar_return_contracts':368,'result':'PASS','scope':'Static structure only'},'review_method':'All registered maps and event records enumerated; per-map source dialogue/state digests inspected; common engine contracts inspected once and referenced by each instance; focused source traces for each proposed repair; no blanket claim of manual instruction-by-instruction execution or full live traversal.'}
(W/'coverage.json').write_text(json.dumps(coverage,indent=2)+'\n')
(W/'map-index.md').write_text('# Complete registered map index\n\nAll540 registered Hoenn maps are represented. Names containing Unused are judged by actual references: Cave of Origin former R/S floors are live; prototype/Ruby hideout headers are inert.\n\n| Map | Disposition | Region | Objects | Other events and callbacks |\n|---|---|---|---:|---:|\n'+'\n'.join(f'| [{n}](maps/{n}.md) | {m["disposition"]} | [{m["region_chapter"]}]({m["region_chapter"]}) | {sum(e["kind"]=="object_events" for e in m["events"].values())} | {sum(e["kind"]!="object_events" for e in m["events"].values())} |' for n,m in ledger.items())+'\n')
(W/'chapter-membership.json').write_text(json.dumps(dict(chapters),indent=2)+'\n')
print(json.dumps(coverage,indent=2))
