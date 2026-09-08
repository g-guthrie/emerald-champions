#!/usr/bin/env python3
import pathlib,json,re,collections,copy
BOOK=pathlib.Path(__file__).resolve().parents[2];SRC=BOOK/'baseline/source';INV=BOOK/'inventory';OUT=pathlib.Path(__file__).resolve().parent
S=json.loads((INV/'species.json').read_text());A=json.loads((INV/'species-aliases.json').read_text());R=json.loads((INV/'acquisition-roots.json').read_text());W=json.loads((INV/'wild.json').read_text());P=json.loads((BOOK/'review/wild-proposals.json').read_text());FC=json.loads((INV/'form-changes.json').read_text())
def canonical(k):
 seen=set()
 while k in A and k not in seen:seen.add(k);k=A[k]
 return k
def extract_field(block,field):
 m=re.search(r'\.'+field+r'\s*=\s*',block)
 if not m:return None
 start=m.end();depth=0;quote=False;escape=False
 for i in range(start,len(block)):
  c=block[i]
  if quote:
   if escape:escape=False
   elif c=='\\':escape=True
   elif c=='"':quote=False
   continue
  if c=='"':quote=True;continue
  if c in '([{':depth+=1
  elif c in ')]}':depth-=1
  elif c==',' and depth==0:return re.sub(r'\s+',' ',block[start:i]).strip()
 return None
raw=(INV/'species-preprocessed.txt').read_text();positions=list(re.finditer(r'^\s*\[(SPECIES_\w+)\]\s*=',raw,re.M));META={}
for i,m in enumerate(positions):
 block=raw[m.end():positions[i+1].start() if i+1<len(positions) else len(raw)];d={}
 for field in ['weight','height','genderRatio','eggGroups','levelUpLearnset','teachableLearnset','speciesName','formSpeciesIdTable','formChangeTable']:
  x=extract_field(block,field)
  if x is not None:d[field]=x
 META[m.group(1)]=d
# All configured flags and evolutions are read. Closed-form proposals below are additions to, not edits of, the root wild plan.
EXTRA=[]
def addslot(name,method,old,new):
 rows=[row for row in W if row['name']==name and method in row['methods']];assert len(rows)==1,(name,method,len(rows))
 slots=[s for s in rows[0]['methods'][method]['slots'] if canonical(s['species'])==canonical(old)];assert len(slots)==1,(name,method,old,slots)
 slot=slots[0];EXTRA.append({'id':f'FORM-WILD-{len(EXTRA)+1:02d}','name':name,'map':rows[0]['map'],'method':method,'table_field':rows[0]['methods'][method]['table_field'],'slot':slot['slot'],'from':slot['species'],'to':new,'weight':slot['weight'],'min_level':slot['min_level'],'max_level':slot['max_level']})
addslot('MtPyre_2F','land_mons','SPECIES_SINISTEA_PHONY','SPECIES_SINISTEA_ANTIQUE')
addslot('MtPyre_5F','land_mons','SPECIES_POLTCHAGEIST_COUNTERFEIT','SPECIES_POLTCHAGEIST_ARTISAN')
addslot('Underwater_SeafloorCavern','land_mons','SPECIES_KINGDRA','SPECIES_TATSUGIRI_DROOPY')
addslot('Underwater_SeafloorCavern','land_mons','SPECIES_WISHIWASHI','SPECIES_TATSUGIRI_STRETCHY')
addslot('Route118','super_rod','SPECIES_BASCULIN','SPECIES_BASCULIN_BLUE_STRIPED')
addslot('SafariZone_Northwest','land_mons','SPECIES_DITTO','SPECIES_SQUAWKABILLY_YELLOW')
addslot('Route121','land_mons','SPECIES_KOMALA','SPECIES_MAUSHOLD_THREE')
addslot('DesertUnderpass','land_mons','SPECIES_ESPATHRA','SPECIES_DUDUNSPARCE_THREE_SEGMENT')
# actual map name printed if this expected location differs
for row in W:
 if 'submar' in row['name'].lower():print('SUBMARINE',row['name'],row['methods'])
(OUT/'metadata.json').write_text(json.dumps(META,indent=2)+'\n')
(OUT/'wild-additions.json').write_text(json.dumps(EXTRA,indent=2)+'\n')
print('EXTRA',len(EXTRA))
# Explicit cosmetic groups; no namespace-wide 'same National Dex' conversion.
STYLE={}
def group(name,keys):
 keys=[canonical('SPECIES_'+k) if not k.startswith('SPECIES_') else canonical(k) for k in keys];assert len(keys)==len(set(keys));assert all(k in S for k in keys)
 STYLE[name]=keys
for prefix in ['FURFROU','SHELLOS','GASTRODON','SCATTERBUG','SPEWPA','VIVILLON','DEERLING','SAWSBUCK','FLABEBE','FLORGES']:
 group(prefix,[k for k in S if k.startswith('SPECIES_'+prefix+'_')])
group('FLOETTE_COMMON',[f'FLOETTE_{c}' for c in ['RED','YELLOW','ORANGE','BLUE','WHITE']])
group('MINIOR_METEOR_COLORS',[k for k in S if k.startswith('SPECIES_MINIOR_METEOR_')])
group('ALCREMIE_ORDINARY',[k for k in S if k.startswith('SPECIES_ALCREMIE_') and 'GMAX' not in k])
group('MAGEARNA_PAINT',['MAGEARNA','MAGEARNA_ORIGINAL'])
group('ZARUDE_SCARF',['ZARUDE','ZARUDE_DADA'])
group('SQUAWKABILLY_GUTS',['SQUAWKABILLY_GREEN','SQUAWKABILLY_BLUE'])
group('SQUAWKABILLY_SHEER_FORCE',['SQUAWKABILLY_YELLOW','SQUAWKABILLY_WHITE'])
STYLE_REPORT=[]
def fingerprint(k):
 sp=S[k];me=META[k]
 return {'combat_flags':sorted(sp['flags']),'stats':sp['stats'],'types':sp['types'],'abilities':sp['abilities'],'weight':me.get('weight'),'genderRatio':me.get('genderRatio'),'eggGroups':me.get('eggGroups'),'levelUpLearnset':me.get('levelUpLearnset'),'teachableLearnset':me.get('teachableLearnset'),'evolution_rules':[(e['method'],e['parameter'],tuple(e['conditions']),S.get(canonical(e['target']),{}).get('dex')) for e in sp['evolutions']]}
for name,keys in STYLE.items():
 ref=fingerprint(keys[0]);diff={k:[field for field in ref if fingerprint(k)[field]!=ref[field]] for k in keys[1:]};diff={k:v for k,v in diff.items() if v}
 STYLE_REPORT.append({'group':name,'species':keys,'equal_properties':not diff,'differences':diff,'reference':ref})
print('STYLE GROUPS',[(r['group'],len(r['species']),r['differences']) for r in STYLE_REPORT])
(OUT/'style-whitelist.json').write_text(json.dumps(STYLE_REPORT,indent=2)+'\n')
# Proposed direct roots rebuilt from final exact slot tables, retaining all non-wild source records.
FINAL_ROOTS=collections.defaultdict(list)
for k,roots in R.items():
 for root in roots:
  if root['kind']!='wild':FINAL_ROOTS[canonical(k)].append(root)
modified=copy.deepcopy(W)
applied={}
for change in P+EXTRA:
 signature=(change['map'],change['method'],change['slot'])
 if signature in applied:
  assert applied[signature]==canonical(change['to']),('conflicting proposal',signature)
  continue
 applied[signature]=canonical(change['to'])
 rows=[r for r in modified if r['map']==change['map'] and change['method'] in r['methods']];assert len(rows)==1,change
 slot=[s for s in rows[0]['methods'][change['method']]['slots'] if s['slot']==change['slot']];assert len(slot)==1;slot=slot[0]
 assert canonical(slot['species'])==canonical(change['from']),(change,slot)
 slot['species']=change['to'];slot['proposal']=change['id']
for row in modified:
 for method,data in row['methods'].items():
  for slot in data['slots']:
   FINAL_ROOTS[canonical(slot['species'])].append({'kind':'wild','map':row['map'],'method':method,'slot':slot['slot'],'percent':slot['weight'],'level':[slot['min_level'],slot['max_level']],'proposal':slot.get('proposal')})
newgifts={'SPECIES_INDEEDEE_F':'FORM-03','SPECIES_GRENINJA_BATTLE_BOND':'FORM-09','SPECIES_PIKACHU_STARTER':'FORM-09','SPECIES_EEVEE_STARTER':'FORM-09','SPECIES_PIKACHU_COSPLAY':'FORM-09-COSTUME'}
for k,id in newgifts.items():FINAL_ROOTS[k].append({'kind':'proposed-prepared-gift','contract':id})
for idx,name,mapname,badges in [(82,'ARTICUNO','ROUTE120',6),(83,'ZAPDOS','ROUTE112',3),(84,'MOLTRES','MT_PYRE_EXTERIOR',6)]:FINAL_ROOTS[f'SPECIES_{name}_GALAR'].append({'kind':'proposed-form-specific-Sign','contract':'FORM-06','sign_id':idx,'map':'MAP_'+mapname,'badges':badges,'flag':f'FLAG_BADGE{badges:02d}_GET','offset':2})
# Explicit excluded engine/event categories, distinct from transient forms that are actively usable in battle.
COSTUMES=['SPECIES_PIKACHU_'+x for x in ['COSPLAY','ROCK_STAR','BELLE','POP_STAR','PHD','LIBRE']]
INACTIVE={}
for k,s in S.items():
 if k=='SPECIES_NONE':INACTIVE[k]='Sentinel, not a Pokémon.'
 elif 'isGigantamax' in s['flags'] or 'isUltraBurst' in s['flags'] or 'isTeraForm' in s['flags'] or k in ['SPECIES_ETERNATUS_ETERNAMAX','SPECIES_TERAPAGOS_STELLAR']:
  INACTIVE[k]='Inactive gimmick form under the approved Mega-only battle rules; no normal acquisition or generic form-service bypass.'
 elif 'isTotem' in s['flags'] or 'TOTEM' in k:INACTIVE[k]='Legacy Totem encounter template; the current game does not advertise a separate obtainable Totem-power mode.'
 elif k=='SPECIES_GRENINJA_ASH':INACTIVE[k]='Legacy pre-Gen9 Battle Bond transformation; current Battle Bond boosts stats once instead of changing into Ash form.'
 elif k=='SPECIES_PICHU_SPIKY_EARED' or (k.startswith('SPECIES_PIKACHU_') and k not in ['SPECIES_PIKACHU_STARTER','SPECIES_PIKACHU_GMAX']+COSTUMES):
  INACTIVE[k]='Legacy promotional costume/cap template with no distinct positive battle-stat/ability capability over ordinary Pichu/Pikachu; gender, breeding and evolution restrictions mean it is not an unrestricted cosmetic conversion target.'
# Route graph: only actual active change-table edges, never the complete form-species enum table.
EDGES=[]
for k,s in S.items():
 for e in s['evolutions']:
  target=canonical(e['target'])
  if e['method'] in ['EVO_NONE','EVO_TRADE']:continue
  if any(c.startswith('IF_REGION, REGION_') and not c.endswith(('REGION_HOENN','REGION_KANTO')) for c in e['conditions']):continue
  if target in S and target not in INACTIVE:EDGES.append({'from':k,'to':target,'kind':'evolution','definition':e,'contract':'FORM-11'})
 table=s['change_table']
 if table in FC:
  for row in FC[table]['entries']:
   m=re.search(r'\{(FORM_CHANGE_\w+)\s*,\s*(SPECIES_\w+)(.*)\}',row)
   if not m:continue
   method,target,conditions=m.groups();target=canonical(target)
   if target==k or target not in S or target in INACTIVE:continue
   EDGES.append({'from':k,'to':target,'kind':'native-form','method':method,'conditions':conditions.strip(', '),'table':table,'source_line':FC[table]['line'],'contract':'FORM-08' if 'ZYGARDE' in k else 'FORM-12' if 'isMegaEvolution' in S[target]['flags'] else 'FORM-02'})
# Derive only legitimate breedable offspring using the actual backward species algorithm + explicit native exceptions.
def eggspecies(k):
 cur=k
 for _ in range(5):
  found=None
  for parent,s in S.items():
   if any(canonical(e['target'])==cur for e in s['evolutions']):found=parent;break
  if found is None:break
  cur=found
 if cur=='SPECIES_MANAPHY':return 'SPECIES_PHIONE'
 if cur.startswith('SPECIES_ROTOM'):return 'SPECIES_ROTOM'
 if cur.startswith('SPECIES_SCATTERBUG'):return 'SPECIES_SCATTERBUG_FANCY'
 if cur.startswith('SPECIES_FURFROU'):return 'SPECIES_FURFROU_NATURAL'
 if cur=='SPECIES_SINISTEA_ANTIQUE':return 'SPECIES_SINISTEA_PHONY'
 if cur=='SPECIES_POLTCHAGEIST_ARTISAN':return 'SPECIES_POLTCHAGEIST_COUNTERFEIT'
 return cur
for k,s in S.items():
 if k in INACTIVE:continue
 eggs=META.get(k,{}).get('eggGroups','')
 if 'EGG_GROUP_NO_EGGS_DISCOVERED' in eggs and k!='SPECIES_MANAPHY':continue
 if 'isMegaEvolution' in s['flags'] or 'isPrimalReversion' in s['flags']:continue
 egg=eggspecies(k)
 if egg in S and egg!=k and egg not in INACTIVE:EDGES.append({'from':k,'to':egg,'kind':'breeding','contract':'FORM-11','requirements':'Compatible Ditto/parent; daycare access. Foreign regional offspring needs Everstone on the intended regional parent. Native special offspring overrides apply.'})
for report in STYLE_REPORT:
 if not report['equal_properties']:continue
 keys=report['species']
 for source in keys:
  for target in keys:
   if target!=source:EDGES.append({'from':source,'to':target,'kind':'cosmetic-styling','contract':'FORM-05','group':report['group']})
for source in COSTUMES:
 for target in COSTUMES:
  if target!=source:EDGES.append({'from':source,'to':target,'kind':'costume-lesson','contract':'FORM-09-COSTUME','requirements':'Owned female Cosplay Pikachu; explicit confirmation of costume and signature-move exchange, preserving unrelated moves and identity.'})
# Unown letter entries are sprite projections from the actual base Unown personality.
for k in S:
 if k.startswith('SPECIES_UNOWN_'):EDGES.append({'from':'SPECIES_UNOWN','to':k,'kind':'personality-presentation','contract':'FORM-05','requirements':'Native random Unown PID letter, not a stored-form menu mutation.'})
# Correct fusion prerequisites are conjunctions, never simple base-species reachability.
FUSIONS=[('KYUREM','RESHIRAM','KYUREM_WHITE','DNA_SPLICERS'),('KYUREM','ZEKROM','KYUREM_BLACK','DNA_SPLICERS'),('NECROZMA','SOLGALEO','NECROZMA_DUSK_MANE','N_SOLARIZER'),('NECROZMA','LUNALA','NECROZMA_DAWN_WINGS','N_LUNARIZER'),('CALYREX','GLASTRIER','CALYREX_ICE','REINS_OF_UNITY'),('CALYREX','SPECTRIER','CALYREX_SHADOW','REINS_OF_UNITY')]
ROUTE={k:{'kind':'root','sources':r} for k,r in FINAL_ROOTS.items() if k in S and k not in INACTIVE and k!='SPECIES_PHIONE'}
changed=True
while changed:
 changed=False
 for e in EDGES:
  if e['from'] in ROUTE and e['to'] not in ROUTE and e['to'] not in INACTIVE:ROUTE[e['to']]=e;changed=True
 for a,b,t,item in FUSIONS:
  a,b,t='SPECIES_'+a,'SPECIES_'+b,'SPECIES_'+t
  if a in ROUTE and b in ROUTE and t not in ROUTE:ROUTE[t]={'kind':'fusion','from':a,'component':b,'item':'ITEM_'+item,'contract':'FORM-08'};changed=True
MISSING=[k for k in S if k not in ROUTE and k not in INACTIVE]
print('ROUTES',len(ROUTE),'INACTIVE',len(INACTIVE),'MISSING',len(MISSING),MISSING)
(OUT/'strict-routes.json').write_text(json.dumps({'routes':ROUTE,'inactive':INACTIVE,'missing':MISSING,'edges':EDGES,'final_roots':FINAL_ROOTS,'style_whitelist':STYLE_REPORT,'extra_wild':EXTRA},indent=2)+'\n')
# Exact proposed gift builds. Point order is the public preparation order, not the engine stat enum.
GIFTS=[
 {'id':'FORM-GIFT-INDEEDEE','species':'SPECIES_INDEEDEE_F','flag':'FLAG_EC_GIFT_INDEEDEE_F','contract':'FORM-03','npc':'WallysAunt after existing Gardevoirite/story wrapper','gate':'opening complete; gift not received','item':'ITEM_PSYCHIC_SEED','ability':'ABILITY_PSYCHIC_SURGE','nature':'NATURE_CALM','points':[32,0,2,0,32,0],'moves':['FOLLOW_ME','HELPING_HAND','PSYCHIC','PROTECT'],'preset_reference_line':12764},
 {'id':'FORM-GIFT-COSPLAY','species':'SPECIES_PIKACHU_COSPLAY','flag':'FLAG_EC_GIFT_COSPLAY_PIKACHU','contract':'FORM-09-COSTUME','npc':'Lilycove Contest Lobby Girl','gate':'gift not received; no contest victory requirement','item':'ITEM_LIGHT_BALL','ability':'ABILITY_LIGHTNING_ROD','nature':'NATURE_TIMID','points':[2,0,0,32,0,32],'moves':['THUNDERBOLT','GRASS_KNOT','KNOCK_OFF','PROTECT'],'preset_reference_line':11154},
 {'id':'FORM-GIFT-BATTLE-BOND','species':'SPECIES_GRENINJA_BATTLE_BOND','flag':'FLAG_EC_GIFT_BATTLE_BOND_GRENINJA','contract':'FORM-09','npc':'shared Birch/Lab Aide ResearchPartners','gate':'FLAG_SYS_GAME_CLEAR; gift not received','item':'ITEM_LIFE_ORB','ability':'ABILITY_BATTLE_BOND','nature':'NATURE_TIMID','points':[2,0,0,32,0,32],'moves':['HYDRO_PUMP','DARK_PULSE','WATER_SHURIKEN','PROTECT'],'preset_reference_line':11804},
 {'id':'FORM-GIFT-PARTNER-PIKACHU','species':'SPECIES_PIKACHU_STARTER','flag':'FLAG_EC_GIFT_PARTNER_PIKACHU','contract':'FORM-09','npc':'shared Birch/Lab Aide ResearchPartners','gate':'FLAG_SYS_GAME_CLEAR; gift not received','item':'ITEM_LIGHT_BALL','ability':'ABILITY_LIGHTNING_ROD','nature':'NATURE_TIMID','points':[2,0,0,32,0,32],'moves':['FAKE_OUT','THUNDERBOLT','GRASS_KNOT','PROTECT'],'preset_reference_line':1554},
 {'id':'FORM-GIFT-PARTNER-EEVEE','species':'SPECIES_EEVEE_STARTER','flag':'FLAG_EC_GIFT_PARTNER_EEVEE','contract':'FORM-09','npc':'shared Birch/Lab Aide ResearchPartners','gate':'FLAG_SYS_GAME_CLEAR; gift not received','item':'ITEM_SITRUS_BERRY','ability':'ABILITY_ADAPTABILITY','nature':'NATURE_ADAMANT','points':[32,32,2,0,0,0],'moves':['DOUBLE_EDGE','YAWN','HELPING_HAND','PROTECT'],'rationale':'Non-evolving enhanced Eevee must not receive ordinary Eevee Eviolite fallback; mixed offense/support uses its real ability.'},
 {'id':'FORM-GIFT-SECOND-KUBFU','species':'SPECIES_KUBFU','flag':'FLAG_EC_GIFT_SECOND_KUBFU','contract':'FORM-07','npc':'shared Birch/Lab Aide ResearchPartners','gate':'IsLegendarySignCaught(LEGENDARY_SIGN_KUBFU); gift not received','item':'ITEM_EVIOLITE','ability':'ABILITY_INNER_FOCUS','nature':'NATURE_JOLLY','points':[2,32,0,0,0,32],'moves':['CLOSE_COMBAT','ICE_PUNCH','U_TURN','PROTECT'],'preset_reference_line':10684},
 {'id':'FORM-GIFT-SECOND-COSMOG','species':'SPECIES_COSMOG','flag':'FLAG_EC_GIFT_SECOND_COSMOG','contract':'FORM-07','npc':'shared Birch/Lab Aide ResearchPartners','gate':'IsLegendarySignCaught(LEGENDARY_SIGN_COSMOG); gift not received','item':'ITEM_EVIOLITE','ability':'ABILITY_UNAWARE','nature':'NATURE_ADAMANT','points':[32,32,2,0,0,0],'moves':['DOUBLE_EDGE','TELEPORT','MIMIC','NONE'],'preset_reference_line':9664,'rationale':'Preserve current expanded useful moves, omit empty Splash, and use a durable slow support spread.'}
]
for gift in GIFTS:
 gift['level_rule']='min(GetCurrentLevelCap(),25)';gift['receipt']='Set only the named flag after MON_GIVEN_TO_PARTY or MON_GIVEN_TO_PC. Decline/full storage/failure changes no entitlement. Shared Birch/Aide handler has one flag, not two gifts.'
 gift['preparation_policy']='Use the exact final build; retain existing expanded move access. Pinned-reference differences are advisory, not automatic illegality.'
 assert gift['ability'] in S[gift['species']]['abilities'],gift
 assert sum(gift['points'])==66 and max(gift['points'])<=32,gift
FORM_ITEMS=['ITEM_ROTOM_CATALOG','ITEM_GRACIDEA','ITEM_PRISON_BOTTLE','ITEM_RED_NECTAR','ITEM_YELLOW_NECTAR','ITEM_PINK_NECTAR','ITEM_PURPLE_NECTAR','ITEM_ADAMANT_CRYSTAL','ITEM_LUSTROUS_GLOBE','ITEM_GRISEOUS_CORE','ITEM_DOUSE_DRIVE','ITEM_SHOCK_DRIVE','ITEM_BURN_DRIVE','ITEM_CHILL_DRIVE']
# Explicit displaced-family preservation evidence.
for x in EXTRA:
 old=canonical(x['from']);survivors=FINAL_ROOTS.get(old,[])
 x['source']='src/data/wild_encounters.json';x['displaced_direct_roots']=survivors
 if old=='SPECIES_ESPATHRA':x['displaced_family_alternative']={'species':'SPECIES_FLITTLE','map':'MAP_DESERT_UNDERPASS','percent':10,'evolution':S['SPECIES_FLITTLE']['evolutions']}
 x['acceptance']='Preserve native slot weight/level and all other slots; verify actual route/method access and exact captured form; do not rely on a stale generated table.'
 x['rationale']={
 'FORM-WILD-01':'Antique tea discovery on Mt.Pyre2F; Phony retains other floors; Chipped Pot already free.',
 'FORM-WILD-02':'Artisan tea discovery on Mt.Pyre5F; Counterfeit retains other floors; Masterpiece Teacup already free.',
 'FORM-WILD-03':'Add the distinct Droopy Commander option beside Curly/Dondozo without moving either10%anchor.',
 'FORM-WILD-04':'Add the distinct Stretchy Commander option in the same submarine habitat; preserve6%chance.',
 'FORM-WILD-05':'Blue Basculin supplies Rock Head; Red stays18%Surf on the same river route.',
 'FORM-WILD-06':'Yellow Squawkabilly supplies the Sheer Force color group in imported Safari fauna; Ditto remainsRoute117.',
 'FORM-WILD-07':'A practical discovery for the rare three-member family, avoiding100-capture phenotype grind; Komala remainsRoute118.',
 'FORM-WILD-08':'A deep burrowing discovery for three-segment Dudunsparce; same-cave Flittle still supplies Espathra.'}[x['id']]
(OUT/'wild-additions.json').write_text(json.dumps(EXTRA,ensure_ascii=False,indent=2)+'\n')
TRANSIENT=set()
for k in S:
 if (k in ['SPECIES_CASTFORM_SUNNY','SPECIES_CASTFORM_RAINY','SPECIES_CASTFORM_SNOWY','SPECIES_CHERRIM_SUNSHINE','SPECIES_DARMANITAN_ZEN','SPECIES_DARMANITAN_GALAR_ZEN','SPECIES_AEGISLASH_BLADE','SPECIES_MELOETTA_PIROUETTE','SPECIES_WISHIWASHI_SCHOOL','SPECIES_MIMIKYU_BUSTED','SPECIES_EISCUE_NOICE','SPECIES_CRAMORANT_GULPING','SPECIES_CRAMORANT_GORGING','SPECIES_PALAFIN_HERO','SPECIES_XERNEAS_ACTIVE','SPECIES_ZYGARDE_COMPLETE','SPECIES_MORPEKO_HANGRY','SPECIES_TERAPAGOS_TERASTAL'] or k.startswith('SPECIES_MINIOR_CORE_')):TRANSIENT.add(k)
stylegroup={k:r['group'] for r in STYLE_REPORT for k in r['species']}
dexgroups=collections.defaultdict(list)
for k,s in S.items():dexgroups[s['dex']].append(k)
firstdex={dex:keys[0] for dex,keys in dexgroups.items()}
def contract_for_edge(e,target):
 if e.get('kind')=='root':
  proposed=[q for q in e.get('sources',[]) if q.get('contract')]
  if proposed:return proposed[0]['contract'].replace('FORM-09-COSTUME','FORM-09')
  if any(q.get('proposal') for q in e.get('sources',[])):return 'FORM-04'
 if target in stylegroup:return 'FORM-05'
 if 'isMegaEvolution' in S[target]['flags']:return 'FORM-12'
 if target.startswith('SPECIES_DEOXYS'):return 'FORM-10'
 if target.startswith('SPECIES_ZYGARDE') or e.get('kind')=='fusion':return 'FORM-08'
 if e.get('kind')=='cosmetic-styling':return 'FORM-05'
 if e.get('kind')=='costume-lesson':return 'FORM-09'
 if e.get('kind') in ('evolution','breeding'):return 'FORM-11'
 return e.get('contract','FORM-08')
def route_text(k):
 if k in INACTIVE:return INACTIVE[k]
 e=ROUTE[k];kind=e['kind']
 if kind=='root':
  roots=e['sources'];wild=[r for r in roots if r['kind']=='wild'];new=[r for r in roots if r['kind'].startswith('proposed')]
  if new:return '; '.join(r.get('contract','FORM-06')+' '+r['kind'] for r in new)
  if wild:
   q=wild[0];return f"{q['map']} / {q['method']} {q['percent']}%, level {q['level'][0]}–{q['level'][1]}"+(f" ({q['proposal']})" if q.get('proposal') else '')+(f'; {len(wild)-1} other wild slots' if len(wild)>1 else '')
  q=roots[0]
  if q['kind']=='legendary':return q.get('id','Legendary Sign')+'; preserve its actual map/source, badge, flag and discovery requirements'
  return q['kind']+': '+str(q.get('map',q.get('source',q.get('name','see complete root record'))))
 if kind=='evolution':
  d=e['definition'];return e['from']+' → '+d['method']+' '+d['parameter']+('; '+'; '.join(d['conditions']) if d['conditions'] else '')
 if kind=='native-form':return e['from']+' → '+e['method']+(' ['+e['conditions']+']' if e['conditions'] else '')
 if kind=='fusion':return e['from']+' + '+e['component']+' + '+e['item']+'; actual owned components and free unfusion slot'
 if kind=='cosmetic-styling':return 'Owned '+e['from']+'; FORM-05 same-group styling '+e['group']
 if kind=='costume-lesson':return 'Owned Cosplay-family Pikachu; FORM-09 confirmed costume/signature exchange'
 if kind=='breeding':return e['from']+' + compatible parent/Ditto; native egg rules, Everstone for foreign regional offspring'
 if kind=='personality-presentation':return 'Base Unown personality renders this letter; no independent stored-form acquisition or PID rewrite'
 return str(e)
RECORDS={}
for k,s in S.items():
 if k in INACTIVE:cat='inactive-gimmick-or-legacy';decision='INERT/EXCLUDED';contract='FORM-01';r=INACTIVE[k]
 else:
  e=ROUTE[k];kind=e['kind'];contract=contract_for_edge(e,k)
  if 'isMegaEvolution' in s['flags']:cat='Mega battle form'
  elif 'isPrimalReversion' in s['flags']:cat='Primal automatic battle form'
  elif k in TRANSIENT:cat='temporary native battle state'
  elif kind=='personality-presentation':cat='personality-rendered appearance'
  elif k in COSTUMES:cat='functional costume form'
  elif kind=='fusion' or k in ['SPECIES_KYUREM_WHITE','SPECIES_KYUREM_BLACK','SPECIES_NECROZMA_DUSK_MANE','SPECIES_NECROZMA_DAWN_WINGS','SPECIES_CALYREX_ICE','SPECIES_CALYREX_SHADOW']:cat='fusion requiring two owned components'
  elif any(f in s['flags'] for f in ['isAlolanForm','isGalarianForm','isHisuianForm','isPaldeanForm']):cat='regional functional form'
  elif k in stylegroup:cat='explicit cosmetic group member'
  elif firstdex[s['dex']]!=k:cat='persistent functional/phenotype variant'
  elif kind=='evolution':cat='ordinary evolved species'
  else:cat='ordinary acquisition species'
  decision='KEEP'
  if kind in ['cosmetic-styling','costume-lesson'] or k in newgifts:decision='REPAIR'
  if kind=='root' and any(q.get('proposal') or q['kind'].startswith('proposed') for q in e['sources']):decision='REVISE'
  if kind=='native-form' and any(item in e.get('conditions','') for item in FORM_ITEMS):decision='REPAIR'
  r=route_text(k)
  # Evolution/form routes can depend on revised parents while preserving their existing transition.
  if k in ['SPECIES_MAUSHOLD_THREE','SPECIES_MAUSHOLD_FOUR']:contract='FORM-11';decision='REPAIR' if kind!='root' else decision
  if k in ['SPECIES_TOXTRICITY_AMPED','SPECIES_TOXTRICITY_LOW_KEY']:contract='FORM-11';decision='REPAIR'
 RECORDS[k]={'species':k,'national_dex':s['dex'],'source':s['source'],'category':cat,'disposition':decision,'contract':contract,'native_properties':{'types':s['types'],'abilities':s['abilities'],'stats':s['stats'],'flags':s['flags'],'weight':META.get(k,{}).get('weight'),'gender_ratio':META.get(k,{}).get('genderRatio'),'egg_groups':META.get(k,{}).get('eggGroups')},'baseline_roots':R.get(k,[]),'final_route':ROUTE.get(k),'access_summary':r,'evolution_definitions':s['evolutions'],'form_change_table':s['change_table'],'form_table_reference_only':s['form_table'],'cosmetic_group':stylegroup.get(k),'verification':'Source-specific route/condition or deliberate exclusion. Runtime acquisition, state persistence and battle-form behavior must be verified at implementation; this ledger is not a fresh playthrough.'}
# Preserve both nonbreedable branches in the resource ledger rather than claiming independent graph reach proves copies exist.
RESOURCE_LEDGER=[{'family':'Kubfu','original_copies':1,'supplemental_copies':1,'receipt_flag':'FLAG_EC_GIFT_SECOND_KUBFU','branches':['SPECIES_URSHIFU_SINGLE_STRIKE','SPECIES_URSHIFU_RAPID_STRIKE'],'contract':'FORM-07','requirements':'Original Sign caught; explicit scroll choice for each specimen. No resetting Sign state or generic legendary conversion.'},{'family':'Cosmog','original_copies':1,'supplemental_copies':1,'receipt_flag':'FLAG_EC_GIFT_SECOND_COSMOG','branches':['SPECIES_SOLGALEO','SPECIES_LUNALA'],'contract':'FORM-07','requirements':'Original Sign caught; Cosmog43→Cosmoem53; day/night selects final branch. PREP02 permits ready evolution atcap100; both actual components can then be used sequentially in Necrozma fusions.'}]
SUMMARY={'configured_entries_including_none':len(S),'actual_species_forms':len(S)-1,'national_dex_identities_excluding_none':len(dexgroups)-1,'specified_active_paths':len(ROUTE),'inactive_entries_including_none':len(INACTIVE),'unresolved_active_entries':MISSING,'style_groups':len(STYLE),'style_members':sum(len(x) for x in STYLE.values()),'extra_wild_changes':len(EXTRA),'prepared_gifts':len(GIFTS),'added_form_tools':len(FORM_ITEMS),'proposed_sign_count':85,'source_coverage_complete':not MISSING,'native_playthrough_performed':False}
FINAL={'summary':SUMMARY,'scope':'Sequential one-save functional roster, not simultaneous living dex; current14boxes420slots unchanged. Paths retain conditions and are not runtime traversal proof.','contracts':['FORM-'+str(i).zfill(2) for i in range(1,13)],'species':RECORDS,'extra_wild':EXTRA,'gifts':GIFTS,'new_form_tools':FORM_ITEMS,'cosmetic_whitelist':STYLE_REPORT,'costume_whitelist':COSTUMES,'costume_moves':{'PIKACHU_COSPLAY':'NONE','PIKACHU_ROCK_STAR':'METEOR_MASH','PIKACHU_BELLE':'ICICLE_CRASH','PIKACHU_POP_STAR':'DRAINING_KISS','PIKACHU_PHD':'ELECTRIC_TERRAIN','PIKACHU_LIBRE':'FLYING_PRESS'},'nonbreedable_resource_ledger':RESOURCE_LEDGER,'fusion_requirements':FUSIONS,'dependency_contracts':['PREP-02','PREP-02-B','PREP-06','WATER-AUTHOR-01','WILD-ENGINE-01','MEGA-FORM-LABELS','state-allocation-v4'],'policy':'Retain existing expanded preparation access. Pinned differences are advisory; tests validate current intended state/item/party contracts rather than historical count/prose/strategy quotas.'}
(BOOK/'review/acquisition-forms.json').write_text(json.dumps(FINAL,ensure_ascii=False,indent=2)+'\n')
def src_link(source):
 if not source:return 'sentinel/data-only entry'
 return f"[{source['file']}:{source['line']}](<{SRC/source['file']}:{source['line']}>)"
def esc(s):return str(s).replace('|','\\|').replace('\n',' ')
md=['# Complete species and form disposition index',f"The snapshot contains **{SUMMARY['actual_species_forms']} actual species/forms and {SUMMARY['national_dex_identities_excluding_none']} National Dex identities**. All configured entries, including the sentinel, receive a disposition below. **{len(ROUTE)} entries have a specified active access/presentation path; {len(INACTIVE)-1} actual entries are deliberately inactive legacy/gimmick templates.** This is complete design/source coverage, not a claim of1,515 newly played acquisitions.",'Read [the acquisition/evolution/form chapter](../chapters/04-acquisition-evolutions-forms.md) first. [The complete JSON](../review/acquisition-forms.json) retains all root records, native conditions, exact proposed gifts, form tables, the full cosmetic whitelist and the two-parent fusion/resource ledger. A final path through a proposed parent can preserve an existing evolution while still depending on the earlier repair.','## Explicit cosmetic whitelist','The service uses **same-group membership**, never a same-Dex or any-whitelisted-species rule. Current equal-property comparisons cover stats/types/ability slots/weight/gender/egg groups/native learnset pointers and equivalent evolution conditions. Expanded preparation additions remain preserved. Cosmetic Minior groups include meteor colors only; core states follow native Shields Down. Functional costumes are a separate action.']
for report in STYLE_REPORT:
 md += [f"### {report['group']}",' · '.join(k.replace('SPECIES_','') for k in report['species']),f"Source comparison: {'equal checked properties' if report['equal_properties'] else report['differences']}. No item or Pokémon is granted by a cosmetic change."]
md += ['## Functional costume whitelist',' · '.join(k.replace('SPECIES_','') for k in COSTUMES),'Only a separately received Cosplay-family Pikachu may use these lessons. Signature exchanges require explicit confirmation and preserve unrelated moves; ordinary/cap/enhanced partner Pikachu is not a valid source.','## Full configured index','Rows follow configured family/source order. The immediate path is concise; the JSON includes every alternative root and exact transition conditions. Levels are source encounter levels, not an asserted earliest story access or a promise that all evolution conditions are already fulfilled.','| Exact species/form | Disposition / category | Final path and requirements | Contract | Source |','|---|---|---|---|---|']
for k,r in RECORDS.items():
 source=src_link(r['source']);contract=r['contract'];link=f"[{contract}](../chapters/04-acquisition-evolutions-forms.md#{contract.lower()})"
 md.append(f"| {k} | {r['disposition']} — {r['category']} | {esc(r['access_summary'])} | {link} | {source} |")
md += ['## Verification limits','A connected graph can still lie about one-time resources, gender-specific egg species, weather/time/location conditions, held-item ownership, Mega eligibility, fusion components or full storage. The main chapter supplies concrete repairs and native acceptance cases for those boundaries. The index is not an excuse to freeze current counts, force a living dex or remove expanded moves merely to satisfy a pinned comparison.']
text='\n\n'.join(md);text=re.sub(r'(?m)(^\|[^\n]*\n)\n(?=\|)',r'\1',text)
(BOOK/'appendices/species-and-forms.md').write_text(text+'\n')
print('FINAL SUMMARY',SUMMARY)

# Attach human-authored final contract prose after catalog rendering.
chapter=(BOOK/'chapters/04-acquisition-evolutions-forms.md').read_text()
parts=re.split(r'(?m)^## (FORM-\d+)\s*$',chapter)
FINAL['system_contracts']={}
for i in range(1,len(parts),2):
 body=parts[i+1].split('\n## Implementation and evidence boundary')[0].strip()
 match=re.search(r'\*\*Disposition: (KEEP|REPAIR|REVISE)',body)
 FINAL['system_contracts'][parts[i]]={'disposition':match.group(1) if match else 'REPAIR','specification_markdown':body}
FINAL['validation']={'configured_set_matches_index':set(RECORDS)==set(S),'same_group_property_comparisons_passed':all(g['equal_properties'] for g in STYLE_REPORT),'unique_receipt_flags':len({g['flag'] for g in GIFTS}),'full_active_design_coverage':not MISSING,'native_battles_or_traversal_run':False,'current_evolution_archive_items':45,'supplemental_form_items':14}
(BOOK/'review/acquisition-forms.json').write_text(json.dumps(FINAL,ensure_ascii=False,indent=2)+'\n')
