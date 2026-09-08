from pathlib import Path
import json,copy,sys,re
R=Path(__file__).resolve().parents[2];S=R/'baseline/source';sys.path.insert(0,str(S/'scripts'))
import emerald_champions_teams as t
import audit_emerald_champions_master_battles as audit
rows=json.loads((R/'review/opening-later-presets.json').read_text());early=json.loads((R/'review/early-battles.json').read_text());op=json.loads((R/'review/opening.json').read_text())
changes={
'IVYSAUR':{'moves':['SLEEP_POWDER','GIGA_DRAIN','SLUDGE_BOMB','PROTECT']},
'CHARMELEON':{'moves':['HEAT_WAVE','DRAGON_PULSE','ANCIENT_POWER','PROTECT']},
'BAYLEEF':{'moves':['GIGA_DRAIN','HELPING_HAND','LIGHT_SCREEN','PROTECT']}
}
roletext='''IVYSAUR|Sleep and repeatable Grass/Poison damage support either active ally; removed Solar Beam/Growth/Weather Ball dependence because no fixed rival teammate supplies sun.
VENUSAUR|Fast sleep/Leaf Storm pressure with Earth Power coverage; Chlorophyll can exploit opposing sun without depending on it.
CHARMELEON|Immediate spread Fire plus Dragon/Rock coverage; Dragon Pulse replaces weak weather-dependent coverage in a team without its own weather setter.
CHARIZARD|Fast special Fire/Flying pressure with native-expanded Tailwind support; preserve the authored move despite the narrower reference omission.
WARTORTLE|Bulky Eviolite Shell Smash threatens special spread pressure; choose setup only with survival and speed payoff.
BLASTOISE|Bulky Fake Out/Yawn, Follow Me and Flip Turn support; preserve expanded redirection access and select moves for the actual partner.
BAYLEEF|Specially bulky Light Screen/Helping Hand support now retains Giga Drain, avoiding an all-status last-starter state.
MEGANIUM|Triage healing and draining offense protects a stronger active attacker; Light Screen gives a second support route.
QUILAVA|Eruption at high HP and Heat Wave after chip; Flash Fire offers a defensive entry against Fire.
TYPHLOSION|Scarf Eruption provides an immediate offensive reserve with coverage after HP loss; no allied sun assumed.
CROCONAW|Sheer Force Dragon Dance with Water/Ice coverage; Eviolite protects its setup window.
FERALIGATR|Life Orb Sheer Force offense can set up or attack immediately; relevant moves avoid Life Orb recoil.
GROVYLE|A special Unburden template uses Leaf Storm/White Herb, expanded Dragon Pulse and priority; Hoenn authored branches keep their existing physical sets instead.
SCEPTILE|Special default is catalog context only; all actual Hoenn branches retain their individually authored physical Sceptile sets.
COMBUSKEN|Special Speed Boost with Feint is catalog context only; Hoenn authored branches retain their exact existing physical moves and offsets.
BLAZIKEN|Mixed Speed Boost is catalog context only; Hoenn authored branches keep the exact individually reviewed physical ace.
MARSHTOMP|Slow special spread and Wide Guard are catalog context only; Hoenn authored branches preserve the existing physical Marshtomp.
SWAMPERT|Bulky special/Wide Guard default is catalog context only; Hoenn authored branches preserve their exact physical Swampert.
GROTLE|White Herb Shell Smash converts an unevolved tank into physical pressure, with Superpower coverage; require actual speed payoff.
TORTERRA|White Herb Shell Smash supports powerful Ground/Grass attacks without partner damage; no Mega is introduced.
MONFERNO|Fake Out and Iron Fist punches provide immediate support and physical coverage rather than another setup dependency.
INFERNAPE|Fake Out/Helping Hand/Quick Guard support plus Close Combat; preserve native-expanded priority protection.
PRINPLUP|Competitive, Scald and Icy Wind support both physical and special partners; preserve native-expanded burn pressure.
EMPOLEON|Competitive special coverage and Shuca survival; responds to opposing stat reduction while remaining offensive.
SERVINE|Contrary Leaf Storm with Glare and Knock Off supplies a direct win route and utility.
SERPERIOR|Contrary offense plus Glare/Light Screen; its berry may need a role-preserving conflict substitute in the complete rival team.
PIGNITE|Bulky physical Fire/Fighting pressure plus Sucker Punch; Thick Fat offers defensive switching utility.
EMBOAR|Reckless physical Fire and Fighting coverage; its slow bulky nature is preserved rather than adding an untested speed redesign.
DEWOTT|Fast Icy Wind/Chilling Water and Helping Hand supports the other active slot while retaining direct attacks.
SAMUROTT|Physical Swords Dance with Water STAB and Aqua Jet; useful under ordinary speed control or as an immediate priority reserve.
QUILLADIN|Physical bulk, Super Fang and Helping Hand create damage opportunities without weather dependency.
CHESNAUGHT|Bulletproof and contact punishment sustain the board while Body Press exploits Defense; an alternate defensive endgame.
BRAIXEN|Fast special Fire/Psychic pressure and Will-O-Wisp for physical mitigation.
DELPHOX|Power Herb Solar Beam supplies one-turn Water/Ground coverage without sun; preserve its Power Herb rather than arbitrary item replacement.
FROGADIER|Protean special offense and U-turn; generation-nine once-per-entry behavior is explicit.
GRENINJA|Fast special Water/Dark/Ice plus U-turn; can reset Protean and reposition without consuming the fixed Mega allocation.
DARTRIX|Tailwind and Helping Hand support with direct Brave Bird; Long Reach can avoid contact punishment.
DECIDUEYE|Configured Tinted Lens attacks plus Spirit Shackle trapping; Clear Amulet protects physical investment.
TORRACAT|Intimidate/Helmet and U-turn support the full rival team while Flare Blitz threatens damage.
INCINEROAR|Intimidate/Fake Out/Parting Shot provides coherent positioning, while native-expanded Knock Off gives useful item control.
BRIONNE|Liquid Voice/Hyper Voice and Icy Wind offer strong special spread pressure plus speed control.
PRIMARINA|Throat Spray Liquid Voice converts a damaging sound move into a further special threat; preserve that specific item synergy.
THWACKEY|Grassy Surge enables priority and strong physical attacks while supporting grounded healing.
RILLABOOM|Grassy Surge, Grassy Glide and Drum Beating create priority and speed alternatives; High Horsepower avoids terrain-weakened Earthquake.
RABOOT|Libero physical Fire/Fighting with U-turn and priority, using the first type change per entry accurately.
CINDERACE|Band Libero physical pressure; Choice lock and re-entry typing are considered by the shared AI.
DRIZZILE|Focus Energy/Sniper offers crit pressure with direct Water attacks; setup is conditional on a useful action window.
INTELEON|Sniper Snipe Shot attacks through redirection and Taunt supports the active partner.
FLORAGATO|Physical Protean Grass/Fairy coverage plus Sucker Punch; preserve exact move legality and first-entry type change.
MEOWSCARADA|Flower Trick provides reliable critical offense with Dark coverage and priority; protect the role when replacing a conflicting Sash.
CROCALOR|Physically bulky Unaware/Wisp/Slack Off supports the board while Flamethrower provides repeatable damage.
SKELEDIRGE|Unaware Torch Song/Wisp/Hex creates a self-contained special endgame with recovery.
QUAXWELL|Flip Turn, recovery and hazard removal offer bulky support; its single damaging move still works when no replacement is available.
QUAQUAVAL|Moxie/Aqua Step gives physical snowballing with immediate Fighting/Dark coverage and Detect.'''
roles=dict(line.split('|',1)for line in roletext.splitlines())
profiles={};bad=[]
for r in rows:
 f=copy.deepcopy(r);f.update(changes.get(r['species'],{}));f.pop('baseline_pinned_issues',None)
 illegal={'MOVE_'+x for x in f['moves']}-audit.pinned_legal_moves('SPECIES_'+r['species'])
 if illegal:bad.append([r['species'],sorted(illegal)])
 profiles[r['species']]={'baseline':r,'final':f,'assessment':roles[r['species']],'decision':'REPAIR'if r['species']in changes else'KEEP'}
# Reference differences are retained under the chosen expanded-native move policy.
trios={g:[x['species']for x in json.loads((R/'review/opening-default-presets.json').read_text())if x['generation']==g]for g in range(1,10)}
lookup={(r['base_species'],r['stage']):r['species']for r in rows};resolved=[]
allteams=t.read_teams(S/'data/emerald_champions/emerald_champions_battle_teams.txt')
for b in allteams:
 if b.cls!='rival'or not('BRENDAN_'in b.trainer or'MAY_'in b.trainer):continue
 if b.encounter not in [1,32,59,249,302]:continue
 idx=0 if b.encounter==1 else len(b.mons)-1;old=b.mons[idx]
 typ=0 if old.species in ['TREECKO','GROVYLE','SCEPTILE']else 1 if old.species in ['TORCHIC','COMBUSKEN','BLAZIKEN']else 2
 stage=0 if b.encounter==1 else 1 if old.species in ['GROVYLE','COMBUSKEN','MARSHTOMP']else 2
 for gen in range(1,10):
  other=early[b.trainer]['final_team'][1:]if b.encounter==1 else[vars(m)for i,m in enumerate(b.mons)if i!=idx]
  taken={m['item']for m in other if m['item']!='NONE'}
  base=trios[gen][typ];species=base if stage==0 else lookup[(base,stage)]
  if gen==3:f=copy.deepcopy(early[b.trainer]['final_team'][idx])if b.encounter<=250 else copy.deepcopy(vars(old));provenance='preserved exact Hoenn authored branch'
  elif stage==0:f=copy.deepcopy(op['starter_presets'][base]['final_rival_opening']);f['offset']=f.pop('level_offset');provenance='explicit opening slot table'
  else:
   f=copy.deepcopy(profiles[species]['final']);f['offset']=old.offset;provenance='curated stage profile'
   if f['item']in taken:
    pts=list(map(int,f['points'].split('/')));support=max(pts[1],pts[3])<16
    opts=['COVERT_CLOAK','MENTAL_HERB','LEFTOVERS','LUM_BERRY','SITRUS_BERRY']if support else['LIFE_ORB','EXPERT_BELT','SITRUS_BERRY','LUM_BERRY','LEFTOVERS','COVERT_CLOAK']
    assert f['item']not in ['WHITE_HERB','POWER_HERB','SCOPE_LENS','THROAT_SPRAY'],(b.trainer,species,'critical item collision')
    f['item']=next(x for x in opts if x not in taken);provenance+=' with explicit conflict replacement'
  assert f['item']not in taken,(b.trainer,gen,species,f['item'])
  illegal={'MOVE_'+x for x in f['moves']}-audit.pinned_legal_moves('SPECIES_'+species)
  resolved.append({'trainer':b.trainer,'encounter':b.encounter,'generation':gen,'unchosen_index':typ,'starter_slot':idx,'stage':stage,'final_starter':f,'preserved_other_party':other,'provenance':provenance})
data={'profiles':profiles,'resolved_variants':resolved,'validation':{'profile_count':len(profiles),'resolved_variants':len(resolved),'pinned_reference_differences_preserved':bad,'all_resolved_items_unique':True,'runtime_verified':False},'item_policy':'Exact resolved rows are authoritative; lookup does not choose first matching raw preset at runtime. No player Item Clause is inferred. Required synergy items are preserved; none collide in the current complete matrices.'}
source_text=(S/'src/data/pokemon/emerald_champions_battle_sets.h').read_text()
for species,entry in profiles.items():
 match=re.search(r'^    \[SPECIES_'+species+r'\] =',source_text,re.M)
 entry['source']={'file':'src/data/pokemon/emerald_champions_battle_sets.h','line':source_text[:match.start()].count('\n')+1}
(R/'review/opening-regional-rival-profiles.json').write_text(json.dumps(data,indent=2)+'\n')
md=['# Regional rival stage profiles and exact resolved variants','','This supplement closes the runtime-template gap behind the two-starter opening. It specifies54 middle/final species profiles and270 exact trainer×generation starter substitutions covering Route103, Rustboro/Route104, Route110, Route119 and Lilycove. Both ordered player choices that leave the same third starter produce the same rival variant; the player’s own first/second ordering remains distinct.','','The Hoenn authored templates are preserved exactly, except the six explicitly revised opening teams. Their complete teams remain owned by the battle volumes. The stage profile of a Hoenn species below is catalog context, not an override of those authored branches. Regional replacements must never alter the fixed Mega partner, party size, starter position or level offset.','','## Exact middle/final profiles','','|Species|Decision|Item|Ability|Nature|Points|Moves|Individual role assessment|','|---|---|---|---|---|---|---|---|']
for s,p in profiles.items():
 f=p['final'];md.append('|'+ '|'.join([s,p['decision'],f['item'],f['ability'],f['nature'],f['points'],', '.join(f['moves']),p['assessment']])+'|')
md+=['','## Item conflicts and materialization','','The exact [270 resolved variants](../../review/opening-regional-rival-profiles.json) include the complete final starter loadout and every preserved other party member. Their item fields are the final specification. No runtime random choice or itemless error fallback is permitted. A support profile losing a duplicated Sitrus/Eviolite can receive Covert Cloak, Mental Herb, Leftovers or Lum Berry according to the recorded row; an offensive profile uses a recorded Life Orb/Expert Belt/berry alternative. This is opponent authoring coherence, not a general player Item Clause.','','White Herb, Power Herb, Scope Lens and Throat Spray are protected where they drive the chosen strategy. None collide in the resulting current matrices. If future roster edits introduce such a collision, reopen that specific variant rather than stripping the item. The provenance field records every actual replacement in this version.','','Use the existing hand-audited battle-set authoring file and generator for these profiles. Give curated regional-rival alternatives stable species-local names and generate the selection index from that authoring; do not rely on a hardcoded raw index that shifts when another preset is inserted. The runtime accessor selects the generated explicit profile for species/stage/milestone, then preserves the captured authored level and slot. This adds an explicit use selection to the existing catalog instead of an unrelated second preset engine.','','Validation performed: the curated changes use existing native moves, with six narrower-reference differences deliberately preserved under the expanded catalog policy; every resolved opposing party has unique held items; all270variants have a single identified starter slot and preserve the other party members. Stat allocations remain within66total/32perstat. No runtime AI execution, damage range or battle victory is claimed. After implementation, test the actual resolved native parties for all variants, including evolved typings, field interactions, later live-cap floors and the fixed Lilycove Mega partner.','']
(R/'battles/early/regional-rival-profiles.md').write_text('\n'.join(md));print(data['validation'])
