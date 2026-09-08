from pathlib import Path
import re,json,copy
R=Path(__file__).resolve().parents[2];S=R/'baseline/source';O=R/'battles/early'
s=(R/'inventory/species-preprocessed.txt').read_text();marks=list(re.finditer(r'^[ \t]*\[(SPECIES_\w+)\] =\s*\{',s,re.M));bases={}
fields=['HP','Attack','Defense','SpAttack','SpDefense','Speed']
def const(expr):
 expr=expr.strip()
 while expr.startswith('(') and expr.endswith(')'):
  dep=0;whole=True
  for j,c in enumerate(expr):
   dep+=int(c=='(')-int(c==')')
   if dep==0 and j<len(expr)-1:whole=False;break
  if not whole:break
  expr=expr[1:-1].strip()
 if '?'in expr:
  cond,rest=expr.split('?',1);yes,no=rest.split(':',1)
  return const(yes if const(cond) else no)
 if not re.fullmatch(r'[0-9() +*/<>=!&|.%-]+',expr):raise ValueError(expr)
 return int(eval(expr.replace('||',' or ').replace('&&',' and '),{'__builtins__':{}},{}))
for i,m in enumerate(marks):
 b=s[m.end():marks[i+1].start()if i+1<len(marks)else len(s)];v={}
 for f in fields:
  mt=re.search(r'\.base'+f+r'\s*=\s*([^,]+)',b)
  if mt:
   try:v[f]=const(mt[1])
   except ValueError:pass
 if len(v)==6:bases[m[1][8:]]=v
p=(S/'src/pokemon.c').read_text();nm=list(re.finditer(r'\[NATURE_(\w+)\] =\s*\{',p));natures={}
mapstat={'HP':'HP','ATK':'Attack','DEF':'Defense','SPATK':'SpAttack','SPDEF':'SpDefense','SPEED':'Speed'}
for i,m in enumerate(nm):
 b=p[m.end():nm[i+1].start()if i+1<len(nm)else len(p)];up=re.search(r'\.statUp = STAT_(\w+)',b);down=re.search(r'\.statDown = STAT_(\w+)',b)
 if up and down:natures[m[1]]=(mapstat[up[1]],mapstat[down[1]])
def calc(species,level,nature,points,stored_iv=31):
 ps=list(map(int,points.split('/')));assert len(ps)==6 and sum(ps)<=66 and max(ps)<=32,(species,ps)
 r={};pre={};base=bases[species];up,down=natures[nature]
 for f,sp in zip(fields,ps):
  if f=='HP':r[f]=1 if species=='SHEDINJA'else((2*base[f]+31)*level)//100+level+10+sp;pre[f]=r[f]
  else:
   v=((2*base[f]+31)*level)//100+5+sp;pre[f]=v
   r[f]=v*110//100 if up!=down and f==up else v*90//100 if up!=down and f==down else v
 return {'species':species,'level':level,'nature':nature,'points':points,'stored_iv_fixture':stored_iv,'effective_iv':31,'base_stats':base,'pre_nature':pre,'stats':r}
def monrow(m,level):return calc(m['species'],level,m['nature'],m['points'])
battles=json.loads((R/'review/early-battles.json').read_text());opening=json.loads((R/'review/opening.json').read_text())
fal=[]
for mode,lf,lg in [('Easy',16,17),('Normal/Medium',18,19),('Hard',20,21),('Normal/Medium revisit at live cap30',28,28)]:
 old=monrow(battles['TRAINER_CRISTIAN']['baseline']['mons'][0],lf);new=monrow(battles['TRAINER_CRISTIAN']['final_team'][0],lf);g=monrow(battles['TRAINER_CRISTIAN']['final_team'][1],lg)
 fal.append({'mode':mode,'baseline_falinks':old,'final_falinks':new,'gallade':g,'old_before':old['stats']['Speed']>g['stats']['Speed'],'new_before':new['stats']['Speed']>g['stats']['Speed']})
pika=battles['TRAINER_BRENDAN_ROUTE_103_MUDKIP']['final_team'][1];pi12=monrow(pika,12);pi14=monrow(pika,14)
initial=[]
for s,v in opening['starter_presets'].items():
 player=v['final_player_initial'];riv=v['final_rival_opening'];initial.append({'species':s,'player_rescue':monrow(player,5),'player_at_cap14':monrow(player,14),'rival_normal':monrow(riv,12),'rival_hard':monrow(riv,14),'player_faster_than_normal_pikachu_before_modifiers':monrow(player,14)['stats']['Speed']>pi12['stats']['Speed']})
pairs=[]
for x in opening['ordered_choice_matrix']:
 pair=[monrow(m,14)for m in x['player_initial_party']];rival=monrow(x['rival_final_party'][0],12)
 pairs.append({'generation':x['generation'],'first_index':x['first_index'],'second_index':x['second_index'],'player_pair':pair,'rival_starter':rival,'pikachu':pi12,'speed_order_without_priority_field_or_abilities':sorted([{'name':'player_'+m['species'],'speed':m['stats']['Speed']}for m in pair]+[{'name':'rival_'+rival['species'],'speed':rival['stats']['Speed']},{'name':'rival_PIKACHU','speed':pi12['stats']['Speed']}],key=lambda y:-y['speed'])})
roxy=[]
for mode,delta in [('Normal/Medium',-2),('Hard',0)]:
 row={'mode':mode}
 for name,slot in [('carbink',0),('relicanth',1),('nosepass',4)]:
  m=battles['TRAINER_ROXANNE_1']['final_team'][slot];row[name]=monrow(m,14+m['offset']+delta)
 row['old_onix']=monrow(battles['TRAINER_ROXANNE_1']['baseline']['mons'][4],18+delta);roxy.append(row)
shrooms=[calc('SHROOMISH',14,'BOLD','32/0/32/0/2/0'),calc('SHROOMISH',14,'BOLD','27/0/32/0/2/5'),calc('SHROOMISH',14,'TIMID','29/0/32/0/2/3'),calc('SHROOMISH',14,'BOLD','25/0/32/0/2/7'),calc('SHROOMISH',14,'TIMID','27/0/32/0/2/5')]
tree=[calc('TREECKO',14,'ADAMANT','32/32/2/0/0/0'),calc('TREECKO',14,'ADAMANT','25/32/2/0/0/7'),calc('TREECKO',14,'JOLLY','28/32/2/0/0/4'),calc('TREECKO',14,'ADAMANT','23/32/2/0/0/9')]
for x in tree:x['unburden_speed']=x['stats']['Speed']*2
assert fal[1]['baseline_falinks']['stats']['Speed']==37
assert fal[1]['final_falinks']['stats']['Speed']==75 and fal[1]['gallade']['stats']['Speed']==73
assert fal[2]['baseline_falinks']['stats']['Speed']==41 and fal[2]['final_falinks']['stats']['Speed']==80 and fal[2]['gallade']['stats']['Speed']==77
assert pi12['stats']['Speed']==68 and pi14['stats']['Speed']==72
iv0=calc('FALINKS',20,'JOLLY','2/32/0/0/0/32',0);iv31=calc('FALINKS',20,'JOLLY','2/32/0/0/0/32',31);assert iv0['stats']==iv31['stats']
data={'source':'snapshot configured species metadata plus src/pokemon.c:1425 and1452 and4807','formula':'NonHP=floor(((2B+31)*L)/100)+5+StatPoints, then nature floor(n*110/100) or floor(n*90/100). HP=floor(((2B+31)*L)/100)+L+10+HPpoints; Shedinja handling stays1.','display_order':fields,'engine_order':['HP','Attack','Defense','Speed','SpAttack','SpDefense'],'limits':{'per_stat':32,'total':66},'iv_zero_same_as_31':iv0['stats']==iv31['stats'],'falinks_gallade':fal,'pikachu_normal':pi12,'pikachu_hard':pi14,'starter_benchmarks':initial,'ordered_opening_pairs':pairs,'roxanne':roxy,'shroomish_reallocations':shrooms,'treecko_unburden_reallocations':tree,'validation':{'computed':True,'engine_executed':False,'full_damage_simulated':False,'all54pairs_included':len(pairs)==54}}
(O/'points-benchmarks.json').write_text(json.dumps(data,indent=2)+'\n')
def statsline(r):return '/'.join(str(r['stats'][f])for f in fields)
md=['# Early battle Stat Point benchmarks','','**Source calculation, not runtime victory evidence.** These numbers use the configured snapshot species metadata and the exact current Champions stat formula. They make the point/nature levers concrete for the opening, Cristian and Roxanne. All source stats below exclude transient battle modifiers unless the row explicitly says otherwise.','','## The actual formula and storage order','','[src/pokemon.c:1425](../../baseline/source/src/pokemon.c:1425) adds Stat Points after level scaling and before nature. [HP calculation](../../baseline/source/src/pokemon.c:1452) follows the same fixed-point principle. [Nature rounding](../../baseline/source/src/pokemon.c:4807) uses integer multiplication/division.','','```text','Non-HP before nature = floor((2 × baseStat + 31) × level / 100) + 5 + points','Nature raised stat    = floor(beforeNature × 110 / 100)','Nature lowered stat   = floor(beforeNature × 90 / 100)','HP                    = floor((2 × baseHP + 31) × level / 100) + level + 10 + HPpoints','```','','The engine forces effective IV31 in the Champions branch. A stored Speed IV0 does not make a slow Room Pokémon slower in this build. There is no friendship stat bonus (`B_FRIENDSHIP_BOOST=FALSE`). HP does not receive nature changes, and Shedinja’s special HP rule remains1.','','The builder budget is [66 total and32 per stat](../../baseline/source/include/constants/emerald_champions.h:12). Do not use252/510 EV assumptions: those legacy constants still exist but do not describe the Champions preparation contract. A point adds one before nature at level5 just as it does at level100.','','The authored/display order is **HP/Atk/Def/SpA/SpD/Spe**. Native `STAT_*` storage order is **HP/Atk/Def/Spe/SpA/SpD**. Use [gEmeraldChampionsStatPointOrder](../../baseline/source/src/emerald_champions_battle_sets.c:548) or `EC_STAT_POINT_DATA`; never write the display vector directly into sequential engine fields.','','## Cristian: repairing the actual Beat Up order','','|Difficulty/scenario|Falinks level|Gallade level|Old Falinks Speed|Final Falinks Speed|Gallade Speed|Result|','|---|---:|---:|---:|---:|---:|---|']
for x in fal:md.append(f"|{x['mode']}|{x['final_falinks']['level']}|{x['gallade']['level']}|{x['baseline_falinks']['stats']['Speed']}|{x['final_falinks']['stats']['Speed']}|{x['gallade']['stats']['Speed']}|Final Falinks acts first at equal priority|")
md+=['','At Hard levels20/21: old Falinks Speed is `floor(181×20/100)+5 =41`; final Jolly/32-Speed Falinks is `floor((36+5+32)×110/100)=80`; Gallade is `floor(191×21/100)+5+32=77`. Normal/Medium levels18/19 become37→75 against73. The shared floor does not erase positive authored level offsets, and a later revisit can place both at the live floor.','','The exact repair changes Falinks Adamant/PB `32/32/2/0/0/0` to Jolly/PS `2/32/0/0/0/32`. It loses30 HP points,2 Defense points and the Attack-raising nature, gaining32 Speed points plus a Speed nature. That is a deliberate activation trade, not free stats. Gallade and the other five party fields remain unchanged.','','This proves unmodified equal-priority order only. Beat Up damage, Gallade survival, all Justified activations, redirection, paralysis, opposing priority and Trick Room must be tested in battle. The AI may use Coaching or an attack instead when the actual board makes Beat Up worse.','','## Route103: Light Ball Pikachu and both player starters','','|Opponent|Level|Raw HP/Atk/Def/SpA/SpD/Spe|Light Ball offensive values|','|---|---:|---|---|',f"|Normal/Medium Pikachu|12|{statsline(pi12)}|Atk {pi12['stats']['Attack']*2}, SpA {pi12['stats']['SpAttack']*2}; Speed unchanged|",f"|Hard Pikachu|14|{statsline(pi14)}|Atk {pi14['stats']['Attack']*2}, SpA {pi14['stats']['SpAttack']*2}; Speed unchanged|",'','Pikachu is Timid `2/0/0/32/0/32`, with Fake Out/Thunderbolt/Electroweb/Protect. Light Ball is a battle offensive modifier, not extra displayed points or Speed. The exact regional starter partner and both owned starters vary by the54-choice table; all54 raw-speed orders are included in [the JSON](points-benchmarks.json).','','|Starter|Player Speed at rescue L5|Player Speed at cap14|Rival Speed at Medium L12|Rival Speed at Hard L14|','|---|---:|---:|---:|---:|']
for x in initial:md.append(f"|{x['species']}|{x['player_rescue']['stats']['Speed']}|{x['player_at_cap14']['stats']['Speed']}|{x['rival_normal']['stats']['Speed']}|{x['rival_hard']['stats']['Speed']}|")
md+=['','The player columns use the exact initial-grant sets; they are editable through the preparation system. The rival columns use the explicit regional Route103 set, including the preserved different Hoenn authored builds. These columns do not assume that the player remains at level5 before the first trainer: the Leveler and preparation services are available beforehand.','','A useful example is bulky initial Treecko. Its Berry Juice/Unburden set has Speed28 at level14, rising to56 after Unburden—still below the Medium rival Pikachu’s68. Having a speed-doubling ability is not itself proof of moving first.','','|Treecko nature/points|HP|Attack|Raw Speed|After Unburden|Interpretation|','|---|---:|---:|---:|---:|---|']
for x,label in zip(tree,['Initial default; slower than Medium Pikachu','Reallocate7HP points to Speed; beats Medium68','Jolly plus4Speed points; beats Medium68, gives up Adamant Attack','Reallocate9HP points to Speed; beats Hard72']):md.append(f"|{x['nature']} {x['points']}|{x['stats']['HP']}|{x['stats']['Attack']}|{x['stats']['Speed']}|{x['unburden_speed']}|{label}|")
md+=['','The retained player Torchic initial set has Speed58 at cap14; one Speed Boost reaches87 before other modifiers. Treecko’s alternative investments illustrate choice, not a compulsory solution. Ties are not guaranteed order, and Tailwind, paralysis, stat stages or terrain can change the comparison.','','## Roxanne: sleep timing, Carbink and iconic Nosepass','','|Mode / Pokémon|Level|HP/Atk/Def/SpA/SpD/Spe|','|---|---:|---|']
for x in roxy:
 for key in ['carbink','relicanth','nosepass','old_onix']:md.append(f"|{x['mode']} / {key}|{x[key]['level']}|{statsline(x[key])}|")
md+=['','The proposed Nosepass keeps Onix’s Eviolite, Sturdy and +4offset but changes its role to special bulk, special Rock/Ground attacks and Wide Guard. Its lower raw offensive damage is an explicit cost for partner protection and the requested signature restoration. Eviolite’s in-battle defense multiplier is not included in the raw table. Compare whole-pair outcomes rather than judging that replacement from one Attack number.','','Roxanne’s Carbink has no Overcoat or Safety Goggles. Mental Herb handles eligible mental disruption, not sleep immunity. **Spore’s priority0 already precedes Trick Room’s−7** if the user gets to act; raising Speed is not required merely to precede Carbink choosing Trick Room. Speed can matter against Relicanth’s ordinary-priority attack that might hit the sleeper first. Powder legality, existing status, immunity and damage survival remain separate questions.','','|Level14 Shroomish nature/points|HP|Defense|Speed|Purpose|','|---|---:|---:|---:|---|']
for x,label in zip(shrooms,['Bulky baseline;19Speed','Bold +5Speed points beats Medium Relicanth23','Timid +3Speed points beats Medium23, losing Bold defense bonus','Bold +7Speed points beats Hard Relicanth25','Timid +5Speed points beats Hard25, losing Bold defense bonus']):md.append(f"|{x['nature']} {x['points']}|{x['stats']['HP']}|{x['stats']['Defense']}|{x['stats']['Speed']}|{label}|")
md+=['','Each example still totals66points and respects32perstat. The player pays for Speed through fewer HP points, a different nature, or both. None of these rows proves Shroomish survives Head Smash, wins the battle, or is the best sleep option. Those are damage and paired-action questions to test with the actual available roster.','','## Priority is a different lever from Speed','','|Move|Configured priority|Implication|','|---|---:|---|','|Helping Hand|+5|Can support an ally before most attacks regardless of its raw Speed.|','|Protect|+4|Normally precedes Fake Out and attacks, subject to its other rules.|','|Fake Out / Wide Guard|+3|Their relative order can depend on Speed; Fake Out still requires its entry turn.|','|Extreme Speed|+2|Outruns ordinary moves by priority, not merely a high Speed stat.|','|Quick Attack|+1|Lower priority than Extreme Speed.|','|Spore / Electroweb|0|Ordinary-priority order follows current Speed/field rules.|','|Trick Room|−7|Usually acts after ordinary attacks; once active it reverses Speed order within priority brackets.|','','Move priorities are read from `src/data/moves_info.h`; Fake Out/Extreme Speed use the configured modern conditional values. Priority blockers and Psychic Terrain can invalidate moves against applicable targets; a larger Speed number does not bypass them.','','## Acceptance boundary','','The appendix independently calculates source-defined stats, checks66/32budgets and confirms the quoted arithmetic and all54matrix rows. It does not execute `CalculateMonStats`, compute full damage ranges, simulate enemy AI or prove a winning strategy. Implementation should add focused engine comparisons for these exact vectors and levels, including storedIV0versus31, display-to-engine order, nature rounding, and the post-floor party stats. Runtime scenarios must then verify action order, activation costs, item effects and alternate player solutions.','']
(O/'points-benchmarks.md').write_text('\n'.join(md))
print(json.dumps({'Pikachu':pi12['stats'],'FalinksMedium':fal[1]['final_falinks']['stats'],'RoxanneMedium':{k:roxy[0][k]['stats']for k in ['carbink','relicanth','nosepass']},'rows':len(pairs)},indent=2))
