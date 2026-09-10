from pathlib import Path
base=Path('data/maps')
for mapname,targets in {
 'SlateportCity':['SlateportCity_EventScript_SternsShipyardFerryComplete','SlateportCity_EventScript_HarborSignFerryComplete'],
 'LilycoveCity':['LilycoveCity_EventScript_HarborSignFerryReady'],
 'SlateportCity_SternsShipyard_1F':['SlateportCity_SternsShipyard_1F_EventScript_FerryReady'],
}.items():
 p=base/mapname/'scripts.inc';s=p.read_text()
 for target in targets:
  old='goto_if_set FLAG_SYS_GAME_CLEAR, '+target
  assert old in s; s=s.replace(old,'goto_if_set FLAG_BADGE06_GET, '+target)
 p.write_text(s)
p=base/'MossdeepCity_SpaceCenter_2F/scripts.inc';s=p.read_text()
for who in ['Scientist','Gentleman','RichBoy']:
 old=f'goto_if_set FLAG_SYS_GAME_CLEAR, MossdeepCity_SpaceCenter_2F_EventScript_{who}Normal'
 assert old in s;s=s.replace(old,f'goto_if_ge VAR_MOSSDEEP_CITY_STATE, 3, MossdeepCity_SpaceCenter_2F_EventScript_{who}Normal')
p.write_text(s)
