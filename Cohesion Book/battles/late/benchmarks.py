#!/usr/bin/env python3
"""Read-only source arithmetic for the book; not a native battle simulation."""
import json,pathlib,re
HERE=pathlib.Path(__file__).resolve().parent;BOOK=HERE.parents[1];SRC=BOOK/'baseline/source'
species=json.loads((BOOK/'inventory/species.json').read_text());reviews=json.loads((HERE/'review.json').read_text())
fields=['baseHP','baseAttack','baseDefense','baseSpAttack','baseSpDefense','baseSpeed']
natures={'MODEST':(3,1),'JOLLY':(5,3),'TIMID':(5,1),'CALM':(4,1),'ADAMANT':(1,3)}
def stats(key,level,points,nature):
 base=species['SPECIES_'+key]['stats'];p=list(map(int,points.split('/')));up,down=natures[nature];out=[]
 for i,field in enumerate(fields):
  v=((2*int(base[field])+31)*level)//100
  if i==0:v+=level+10+p[i]
  else:
   v+=5+p[i]
   if i==up:v=v*110//100
   if i==down:v=v*90//100
  out.append(v)
 return out
rows=[]
for trainer,slot,key in [('TRAINER_GLACIA',4,'WALREIN'),('TRAINER_PHOEBE',3,'DUSCLOPS'),('TRAINER_WALLACE',5,'STARMIE_MEGA')]:
 r=reviews[trainer];m=r['final_team'][slot];cap=int(r['metadata']['strict_cap'])
 for difficulty,adjust in [('Hard',0),('Medium',-2),('Easy',-4)]:
  level=min(100,max(cap+m['offset'],cap)+adjust)
  vals=stats(key,level,m['points'],m['nature']);row={'trainer':trainer,'species':key,'difficulty':difficulty,'level':level,'points':m['points'],'nature':m['nature'],'stats_HP_Atk_Def_SpA_SpD_Spe':vals}
  if key=='STARMIE_MEGA':row['Huge_Power_effective_Attack']=vals[1]*2
  if key=='WALREIN':row['Tailwind_effective_Speed_before_other_modifiers']=vals[5]*2
  rows.append(row)
# Native Sitrus uses ceil-half; Gluttony flavor berries use floor-half.
berry=[]
for level in (68,70,78,80,98,100):
 vals=stats('LINOONE',level,'2/32/0/0/0/32','ADAMANT');hp=vals[0];after=hp-hp//2
 berry.append({'level':level,'max_hp':hp,'hp_after_full_hp_belly_drum':after,'sitrus_threshold':(hp+1)//2,'figy_gluttony_threshold':hp//2,'sitrus_triggers':after<=(hp+1)//2,'figy_triggers':after<=hp//2})
assert berry[0]=={'level':68,'max_hp':207,'hp_after_full_hp_belly_drum':104,'sitrus_threshold':104,'figy_gluttony_threshold':103,'sitrus_triggers':True,'figy_triggers':False}
base=reviews['TRAINER_WALLACE']['baseline']['team'][5];oldstats=stats('STARMIE_MEGA',86,base['points'],base['nature']);newstats=next(r['stats_HP_Atk_Def_SpA_SpD_Spe'] for r in rows if r['trainer']=='TRAINER_WALLACE' and r['difficulty']=='Medium')
assert oldstats[5]==newstats[5]==297
assert newstats[1]*2==470
assert next(r['stats_HP_Atk_Def_SpA_SpD_Spe'] for r in rows if r['species']=='WALREIN' and r['difficulty']=='Medium')==[326,143,191,220,177,135]
result={'method':'Exact configured flat-Points integer stat arithmetic; no native battle execution','formula_source':'src/pokemon.c:1424-1430 and1452-1457','berry_source':'src/battle_util.c:5529-5548','rows':rows,'wallace_baseline_Medium86_stats':oldstats,'jack_sitrus_retention':berry,'assertions_passed':True}
(BOOK/'review/late-battles-benchmarks.json').write_text(json.dumps(result,indent=2)+'\n')
md=['# Native Points arithmetic and narrow revision validation','These calculations use the snapshot\'s configured **fixed IV31 and flat Stat Points**, with integer Nature rounding. They are source arithmetic, not measured emulator damage or proof of an optimal spread. All point strings are HP/Attack/Defense/Special Attack/Special Defense/Speed, which differs from the engine enum order.','## Formula',f'[Native stat calculation](<{SRC}/src/pokemon.c:1424>) applies `floor((2 × base + 31) × level / 100) + 5 + Points` before the integer Nature multiplier for non-HP stats. HP uses `floor((2 × baseHP + 31) × level / 100) + level + 10 + HP Points`. Zero stored IV does not produce a zero-IV stat under this configuration.','The benchmark cap is80 for the three League changes. At these positive offsets, the common floor does not bind: Hard uses cap+offset, Medium uses cap+offset−2, Easy cap+offset−4. Future revisits must use the live cap and the central floor rule. No proposed loadout encodes a fixed level or quietly changes its Points by difficulty.','## Proposed signature and Mega builds','| Trainer / Pokémon | Setting | Level | HP | Atk | Def | SpA | SpD | Spe | Additional effect |\n|---|---|---:|---:|---:|---:|---:|---:|---:|---|']
for r in rows:
 vals=r['stats_HP_Atk_Def_SpA_SpD_Spe'];effect=f"Huge Power Attack {r['Huge_Power_effective_Attack']}" if 'Huge_Power_effective_Attack' in r else f"Tailwind Speed {r['Tailwind_effective_Speed_before_other_modifiers']}" if 'Tailwind_effective_Speed_before_other_modifiers' in r else 'Eviolite applies in battle; shown stats exclude item modifier'
 md.append(f"| {r['trainer'].replace('TRAINER_','')} / {r['species']} | {r['difficulty']} | {r['level']} | "+' | '.join(map(str,vals))+f' | {effect} |')
md.extend(['Walrein at Medium level81 has **326HP, 191Defense, 220Special Attack and 135Speed**; Articuno\'s Tailwind gives270 Speed before other modifiers. Its bulk/coverage/control role is intentional. These numbers do not claim it outdamages the displaced Glastrier. Snow, Veil, Thick Fat and Chien-Pao\'s allied Defense effect belong in actual damage calculations, not in an unsupported promise of survivability.',f"Mega Starmie at Medium level86 retains **297Speed** with the new Jolly spread, matching the baseline Timid spread. Its Attack becomes235 before Huge Power and **470 after Huge Power**, compared with the baseline's287 Special Attack for its all-special set. This is a useful structural reason for the physical revision, not a direct damage ratio: Liquidation/Psycho Cut/Aqua Jet have different power, defenses, priority, accuracy and coverage from Hydro Pump/Thunderbolt/Ice Beam.",'Dusclops\'s Eviolite modifies its defenses in battle and Pressure changes PP use only for applicable targets. Its proposed Helping Hand/Pain Split role must be validated against actual partner attacks and HP states. No unsupported Trick Room or zero-IV adjustment is added.','## Existing Sitrus choice retained after checking the actual contract',f'[Native Berry threshold](<{SRC}/src/battle_util.c:5529>) gives Sitrus a rounded-up half-HP threshold. Gluttony\'s flavor-Berry condition uses floor-half. This makes a tempting Figy substitution unreliable at odd HP, despite its larger healing fraction. Keep Jack\'s existing Sitrus, unchanged Points and current moves.','| Linoone level | Max HP | After full-HP Drum | Sitrus threshold | Figy + Gluttony threshold | Sitrus activates | Figy activates |\n|---:|---:|---:|---:|---:|---|---|'])
for r in berry:md.append('| '+ ' | '.join(str(r[k]) for k in ['level','max_hp','hp_after_full_hp_belly_drum','sitrus_threshold','figy_gluttony_threshold','sitrus_triggers','figy_triggers'])+' |')
md.extend(['At the important Medium level68 case, maxHP207 becomes104 after Drum. Sitrus activates at104; Gluttony Figy would require103 or lower. No new implicit HP normalization is proposed to rescue that item change. This is why a source-grounded review preserves a seemingly suboptimal existing item.','## Exact changed-set checks','The volume renderer verified all proposed moves for Ronald\'s Raichu, Phoebe\'s Dusclops, Glacia\'s Walrein, Wallace\'s Starmie, Quincy\'s Power Construct Zygarde and Colton\'s Eternal Floette against the snapshot\'s pinned `showdown_champions_learnsets.json`, and all proposed base abilities against the configured species inventory. Each final team retains legal66-total/32-per-stat Points, distinct non-NONE held items, its authored party size and unchanged offsets.','Ronald\'s Thunder is present in the pinned Raichu learnset. Mega Raichu Y has No Guard in the configured species data; Thunder therefore has stronger immediate power/status pressure after Mega at lower PP. The native battle check must still cover pre-Mega rain dependence, reciprocal No Guard accuracy and whether spending a setup turn is actually profitable.','## Evidence still required','- Materialize the six exact revised teams through the existing authoring pipeline and compare full source/native loadouts.','- Verify the proposed roles with actual damage, speed, item, weather, terrain and AI decisions under each effective difficulty level.','- For restored Walrein/Dusclops, play meaningful favorable and unfavorable boards and more than one player solution; preserve the encounter\'s intended severity without assuming a nostalgia change is automatically balanced.','- Verify Mega Starmie receives Huge Power and uses physical attacks/priority correctly, and that the shared Mega AI handles form tradeoffs and Steven\'s two eligible candidates.','- [Machine-readable arithmetic](../../review/late-battles-benchmarks.json). No ROM or test ELF was built for this appendix.'])
(HERE/'benchmarks.md').write_text(re.sub(r'(?m)(^\|[^\n]*\n)\n(?=\|)',r'\1','\n\n'.join(md))+'\n')
print(json.dumps({'rows':len(rows),'berry_cases':len(berry),'assertions_passed':True,'wallace_baseline':oldstats,'wallace_final':newstats}))
