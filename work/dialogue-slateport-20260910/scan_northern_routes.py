from pathlib import Path
import re,json
ROOT=Path(__file__).resolve().parents[2]
src=(ROOT/'src/fonts.c').read_text()
widths=[int(x) for x in re.findall(r'\d+',re.search(r'gFontNormalLatinGlyphWidths\[\] = \{(.*?)\};',src,re.S)[1])]
cmap={}
for line in (ROOT/'charmap.txt').read_text().splitlines():
 m=re.match(r"^'(.*)'\s*=\s*([0-9A-Fa-f]+)\s*(?:@.*)?$",line)
 if m:cmap[m[1]]=int(m[2],16)
def width(t):
 pos=0
 for part in re.split(r'(\x01\d+\x02)',t):
  if part.startswith('\x01'):pos=max(pos,int(part[1:-1]))
  else:pos+=sum(widths[cmap[c]] if c in cmap and cmap[c]<len(widths) else 6 for c in part)
 return pos
paths=[ROOT/'data/maps'/n/'scripts.inc' for n in ['Route111','Route111_WinstrateFamilysHouse','Route112','FieryPath','AshenWoods','JaggedPass','EmberPath','Route113','Route113_GlassWorkshop','FallarborTown','FallarborTown_CozmosHouse','FallarborTown_Mart','FallarborTown_PokemonCenter_1F','FallarborTown_MoveRelearnersHouse','FallarborTown_BattleTentLobby','FallarborTown_BattleTentCorridor','FallarborTown_BattleTentBattleRoom','Route114','Route114_LanettesHouse','Route114_FossilManiacsHouse','Route114_FossilManiacsTunnel','MeteorFalls_1F_1R','MeteorFalls_1F_2R','MeteorFalls_B1F_2R','MeteorFalls_JirachisRoom','MtChimney','LavaridgeTown','LavaridgeTown_HerbShop','LavaridgeTown_House','LavaridgeTown_Mart','LavaridgeTown_PokemonCenter_1F','LavaridgeTown_PokemonCenter_2F','LavaridgeTown_Gym_1F','LavaridgeTown_Gym_B1F','PetalburgCity_Gym','PetalburgCity','PetalburgCity_WallysHouse','NewMauville_Entrance','NewMauville_Inside','Route118','Route119','Route119_House','Route119_WeatherInstitute_1F','Route119_WeatherInstitute_2F','Route120','FortreeCity','FortreeCity_Gym','FortreeCity_House1','FortreeCity_House2','FortreeCity_House3','FortreeCity_House4','FortreeCity_House5','FortreeCity_Mart','FortreeCity_DecorationShop','FortreeCity_PokemonCenter_1F','FortreeCity_PokemonCenter_2F','ScorchedSlab','ScorchedSlab_B1F','ScorchedSlab_B2F','ScorchedSlab_HeatransRoom']]
paths += [ROOT/'data/maps'/n/'scripts.inc' for n in ['RustboroCity_Mart','SlateportCity_Mart','LilycoveCity_DepartmentStore_4F','LilycoveCity_DepartmentStore_1F','SlateportCity','ShoalCave_LowTideEntranceRoom','BattleFrontier_ExchangeServiceCorner']]
paths += [ROOT/'data/maps'/n/'scripts.inc' for n in ['Route121','Route122','Route121_SafariZoneEntrance','SafariZone_South','SafariZone_Southwest','SafariZone_Northwest','SafariZone_North','SafariZone_Northeast','SafariZone_Southeast','SafariZone_RestHouse']]
paths += [ROOT/'data/scripts/safari_zone.inc']
paths += [ROOT/'data/text/shoal_cave.inc',ROOT/'data/text/trainers.inc', ROOT/'data/text/berries.inc']
paths += [ROOT/'data/text/lottery_corner.inc',ROOT/'data/text/event_ticket_1.inc',ROOT/'data/text/event_ticket_2.inc',ROOT/'data/text/tv.inc']
paths += [p for p in sorted((ROOT/'data/maps').glob('LilycoveCity*/scripts.inc')) if p not in paths]
paths += [ROOT/'data/scripts/contest_hall.inc', ROOT/'data/scripts/berry_blender.inc']
paths += sorted((ROOT/'data/maps').glob('MtPyre*/scripts.inc'))
paths += sorted((ROOT/'data/maps').glob('MagmaHideout*/scripts.inc'))
paths += sorted((ROOT/'data/maps').glob('AquaHideout*/scripts.inc'))
paths += [ROOT/'data/maps/SlateportCity_Harbor/scripts.inc']
paths += sorted((ROOT/'data/maps').glob('MossdeepCity*/scripts.inc'))
paths += [ROOT/'data/maps/Route124/scripts.inc',ROOT/'data/maps/Route124_DivingTreasureHuntersHouse/scripts.inc',ROOT/'data/text/cable_club.inc']
paths += [ROOT/'data/text/move_tutors.inc',ROOT/'data/maps/Route125/scripts.inc']
paths += [p for p in sorted((ROOT/'data/maps').glob('ShoalCave*/scripts.inc')) if p not in paths]
paths += [ROOT/'data/maps'/n/'scripts.inc' for n in ['Route126','Route127','Route128','Underwater_SeafloorCavern']]
paths += sorted((ROOT/'data/maps').glob('SeafloorCavern*/scripts.inc'))
paths += sorted((ROOT/'data/maps').glob('SootopolisCity*/scripts.inc'))
paths += sorted((ROOT/'data/maps').glob('CaveOfOrigin*/scripts.inc'))
paths += sorted((ROOT/'data/maps').glob('SkyPillar*/scripts.inc'))
paths += sorted((ROOT/'data/maps').glob('Pacifidlog*/scripts.inc'))
paths += [ROOT/'data/maps'/n/'scripts.inc' for n in ['Route129','Route130','Route131','Route132','Route133','Route134','Underwater_Route134','Underwater_SealedChamber','SealedChamber_OuterRoom','SealedChamber_InnerRoom','DesertRuins','IslandCave','AncientTomb']]
paths += [ROOT/'data/maps'/n/'scripts.inc' for n in ['EverGrandeCity','EverGrandeCity_PokemonCenter_1F','EverGrandeCity_PokemonCenter_2F','EverGrandeCity_PokemonLeague_1F','EverGrandeCity_PokemonLeague_2F','VictoryRoad_1F','VictoryRoad_B1F','VictoryRoad_B2F']]
paths += [p for p in sorted((ROOT/'data/maps').glob('EverGrandeCity*/scripts.inc')) if p not in paths]
paths += [ROOT/'data/scripts/elite_four.inc', ROOT/'data/text/pokedex_rating.inc']
paths += [p for p in sorted((ROOT/'data/maps').glob('LittlerootTown*/scripts.inc')) if p not in paths]
paths += [ROOT/'data/maps/SSTidalCorridor/scripts.inc']
paths += [p for p in sorted((ROOT/'data/maps').glob('SSTidal*/scripts.inc')) if p not in paths]
paths += [p for prefix in ['SouthernIsland','BirthIsland','FarawayIsland','NavelRock'] for p in sorted((ROOT/'data/maps').glob(prefix+'*/scripts.inc')) if '_Frlg' not in p.parent.name and p not in paths]
paths += [ROOT/'data/maps'/n/'scripts.inc' for n in ['BattleFrontier_OutsideWest','BattleFrontier_OutsideEast','BattleFrontier_ReceptionGate','BattleFrontier_ScottsHouse','BattleFrontier_PokemonCenter_1F','BattleFrontier_PokemonCenter_2F','BattleFrontier_Mart','BattleFrontier_BattleDomeLobby','BattleFrontier_BattleTowerLobby'] if ROOT/'data/maps'/n/'scripts.inc' not in paths]
paths += [p for p in sorted((ROOT/'data/maps').glob('BattleFrontier_Lounge*/scripts.inc')) if p not in paths]
paths += [ROOT/'data/maps/BattleFrontier_RankingHall/scripts.inc', ROOT/'data/scripts/apprentice.inc', ROOT/'data/maps/BattleFrontier_BattleTowerBattleRoom/scripts.inc']
paths += [p for p in sorted((ROOT/'data/maps').glob('BattleFrontier_*Lobby/scripts.inc')) if p not in paths]
frontier_npc_labels=set()
for p in (ROOT/'work/dialogue-slateport-20260910').glob('BattleFrontier_*Lobby-npc-source.txt'):
 frontier_npc_labels.update(re.findall(r'^(\w+):',p.read_text(),re.M))
paths += [p for p in sorted((ROOT/'data/maps').glob('AbandonedShip*/scripts.inc')) if p not in paths]
paths += [ROOT/'data/maps'/n/'scripts.inc' for n in ['Route105','Route107','Route108'] if ROOT/'data/maps'/n/'scripts.inc' not in paths]
rows=[]
for p in paths:
 s=p.read_text()
 for m in re.finditer(r'^(\w+):{1,2}\s*\n((?:\s*\.string[^\n]*\n)+)',s,re.M):
  if p.parent.name == 'text' and p.name not in ['shoal_cave.inc','pokedex_rating.inc'] and not re.match(r'((Route11[123489]|Route12[01456789]|Route13[01234])_Text_|LilycoveCity_|MossdeepCity_|SootopolisCity_|MoveTutor_Text_(DynamicPunch|Explosion|ThisMoveCanOnlyBeLearnedOnce)|SouthernIsland_|BirthIsland_|NavelRock_|FarawayIsland_|EventTicket_Text_)',m[1]):continue
  if p.name=='pokedex_rating.inc' and not m[1].startswith('gBirchDexRatingText_'):continue
  if p.parent.name.startswith('BattleFrontier_Battle') and p.parent.name.endswith('Lobby') and not (m[1] in frontier_npc_labels or m[1].startswith('BattleFrontier_BattleTowerLobby_Text_Circuit') or m[1].startswith('BattleFrontier_OutsideWest_') or m[1] in ['BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules','BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord']):continue
  if p.parent.name=='BattleFrontier_BattleTowerBattleRoom' and not m[1].startswith('BattleFrontier_BattleTowerBattleRoom_Text_Circuit'):continue
  if p.name=='apprentice.inc' and m[1]!='Apprentice_Text_PreparingForCircuit':continue
  text=''.join(re.findall(r'\.string\s+"(.*)"',m[2]))
  row={'file':str(p.relative_to(ROOT)), 'label':m[1], 'line':s[:m.start()].count('\n')+1,'text':text,'lines':[]}
  for t in re.split(r'\\[nlp]|\$',text):
   if not t:continue
   unknown=[]
   def expand(k):
    name=k[1]
    if re.fullmatch(r'PAUSE \d+',name):return ''
    if name in ['PLAYER','RIVAL']:return 'W'*7
    if name=='KUN':return ''
    if name.startswith('CLEAR_TO '):return '\x01'+name.split()[1]+'\x02'
    if name=='PLUS':return '+'
    if name=='REGION':return 'HOENN'
    if name=='POKEBLOCK':return 'W'*5
    if name.endswith('ARROW'):return 'W'
    if name.startswith('STR_VAR'):
     if m[1]=='BattleFrontier_ScottsHouse_Text_ObtainedXBattlePoints':return '4'
     if m[1]=='BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord':return '65535'
     if m[1]=='BattleFrontier_BattleTowerBattleRoom_Text_CircuitBattlePoints':return '40'
     if m[1].startswith('BattleFrontier_BattleTowerLobby_Text_Circuit'):
      if name=='STR_VAR_2':return '65536'
      if name=='STR_VAR_3':return '255'
      if m[1].endswith('Opponent'):return 'a balanced doubles team'
      return 'CELESTEELA'
     if m[1].startswith('BattleFrontier_Lounge6_'):return 'SKITTY' if name=='STR_VAR_1' else 'TYPE: NULL'
     if m[1].startswith('BattleFrontier_Lounge2_'):return 'MULTI LINK'
     if m[1].startswith('BattleFrontier_Lounge3_'):return '65535'
     if m[1].startswith('BattleFrontier_Lounge7_'):return 'THUNDER WAVE' if name=='STR_VAR_1' else '48'
     if m[1].startswith('LittlerootTown_ProfessorBirchsLab_'):return 'W'*13
     if m[1] in ['RivalsHouse_1F_Text_LikeChildLikeFather']:return 'daughter'
     if m[1] in ['RivalsHouse_1F_Text_DoYouHavePokemon','LittlerootTown_Text_CanYouGoSeeWhatsHappening']:return 'big guy'
     if m[1].startswith('gBirchDexRatingText_'):return '65535'
     if m[1].startswith('PacifidlogTown_House2_'):return '65535'
     if m[1].startswith('PacifidlogTown_House3_'):return 'BAGON' if name=='STR_VAR_1' else 'CYCLIZAR'
     if m[1].startswith('SootopolisCity_LotadAndSeedotHouse_'):
      return 'W'*7 if 'Biggest' in m[1] and name=='STR_VAR_2' else '99.9 m'
     if m[1].startswith('SootopolisCity_MysteryEventsHouse_'):return 'W'*7
     if m[1].startswith('RS_MysteryEventsHouse_'):return 'W'*7
     if m[1].startswith('MossdeepCity_House1_'):return 'W'*13 if 'HmmYourPokemon' in m[1] else 'YELLOW '+ 'W'*5
     if m[1].startswith('MossdeepCity_House4_'):return 'W'*25
     if m[1].startswith('MossdeepCity_SpaceCenter_1F_'):return '65535'
     if m[1].startswith('Route124_DivingTreasureHuntersHouse_'):return 'YELLOW SHARD' if name=='STR_VAR_1' else 'THUNDER STONE'
     if m[1].startswith('BerryBlender_Text_'):return 'W'*7
     if p.name=='contest_hall.inc':
      if 'GettingStarted' in m[1]:return 'W'*(6 if name=='STR_VAR_3' else 17)
      if 'EntryXTrainersMon' in m[1]:return '4' if name=='STR_VAR_2' else 'W'*(7 if name=='STR_VAR_1' else 12)
      if 'CongratsTrainerXandMon' in m[1]:return '4' if name=='STR_VAR_2' else 'W'*(12 if name=='STR_VAR_1' else 7)
      if 'PutRibbonOnMon' in m[1]:return 'W'*12
      if 'EntryNumX' in m[1]:return '4'

     if m[1] in ['LilycoveCity_ContestLobby_Text_WhatImageWhenYouHearX','LilycoveCity_ContestLobby_Text_ThatsAllForInterview']:return 'W'*6
     if m[1].startswith('LilycoveCity_PokemonTrainerFanClub_'):return 'W'*7
     if m[1]=='LilycoveCity_ContestLobby_Text_PutTheRibbonOnMon':return 'W'*12
     if m[1].startswith('LilycoveCity_DepartmentStoreRooftop_Text_'):return max(['FRESH WATER','SODA POP','LEMONADE'],key=width)
     if m[1].startswith('LilycoveCity_DepartmentStore_1F_Text_'):
      if 'TicketNumber' in m[1]:return '65535'
      if 'TicketMatch' in m[1]:return 'W'*12
      return 'Master Ball'
     if m[1].startswith('SafariZone_Text_Pokeblock'):return 'GOLD '+ 'W'*5
     if m[1].startswith('BattleFrontier_ExchangeServiceCorner_Text_'):return 'W'*13
     if m[1].startswith('ShoalCave_Text_'):return 'GLALITITE'
     if 'FortreeCity_House1' in m[1]:return 'BOMBIRDIER'
     if 'WeatherInstitute' in m[1] and name == 'STR_VAR_1':return 'ROUTE 129'
     if 'GlassWorkshop_Text_' in m[1]:
      if name=='STR_VAR_2': return '9999'
      if 'CollectedAshes' in m[1]: return '9999'
      if 'Milestone' in m[1]:return 'Houndoominite'
      if 'NotEnoughAshNeedX' in m[1]:return '250'
      return max(['BLUE FLUTE','YELLOW FLUTE','RED FLUTE','WHITE FLUTE','BLACK FLUTE','PRETTY CHAIR','PRETTY DESK'],key=width)
     if 'MauvilleCity_GameCorner_Text_' in m[1]:return 'W'*13
     if any(x in m[1] for x in ['EvsAlreadyReady','MonMostImpressiveGiveItThis']):return 'W'*13
     if 'FurfrouComplete' in m[1]:return 'W'*12
     if 'ExchangeBerryPowderForItem' in m[1]:return 'W'*13
     if 'EeveeResearcher' in m[1]:return 'W'*12
     if 'Fossil' in m[1] or 'ReceivedMonFromResearcher' in m[1]:return 'W'*13
     if 'Flat1_2F' in m[1]:return 'W'*15
     if 'House1_Text' in m[1]:return 'W'*12
     if 'PetalburgCity_PokemonCenter' in m[1]:return 'W'*12
     if 'StarterRegionChosen' in m[1]:return 'Sinnoh'
     if 'OhYoureTheNewNeighbor' in m[1]:return 'daughter'
     if any(x in m[1] for x in ['YourTwoPartners','NameOpeningPartner']):return 'W'*10
     if 'NameRatersHouse_Text' in m[1]:return 'W'*12
     if 'MoveDeletersHouse_Text' in m[1]:return 'W'*(12 if name=='STR_VAR_1' else 16)
     if 'WhichXmove' in m[1]:return 'move'
     if 'Nature' in m[1] and name=='STR_VAR_2':return 'W'*7
     if m[1].startswith('EmeraldChampions_Text_'):
      return 'W'*(12 if name=='STR_VAR_1' else 20)
     unknown.append(name);return '' 
    unknown.append(name);return ''
   expanded=re.sub(r'\{([^}]+)\}',expand,t)
   row['lines'].append({'text':t,'width':width(expanded),'unresolved':unknown})
  rows.append(row)
(ROOT/'work/dialogue-slateport-20260910/northern-route-text-layout.json').write_text(json.dumps(rows,indent=2,ensure_ascii=False)+'\n')
issues=[]
for r in rows:
 for l in r['lines']:
  if l['width']>200:issues.append({'file':r['file'],'label':r['label'],'width':l['width'],'text':l['text']})
print('blocks',len(rows),'lines',sum(len(x['lines']) for x in rows),'over200',len(issues))
for r in issues:print(json.dumps(r,ensure_ascii=False))
