import json,sys,re,copy,hashlib
from pathlib import Path
R=Path(__file__).resolve().parents[2]; S=R/'baseline/source'; O=R/'battles/early'
sys.path.insert(0,str(S/'scripts'))
import emerald_champions_teams as teams
import implement_emerald_champions_master_battles as native
bs=json.loads((O/'baseline.json').read_text()); notes=json.loads((R/'review/early-battles.notes.json').read_text()); by={b['trainer']:b for b in bs}
plans={}
for raw in (O/'plan-updates.txt').read_text().splitlines():
 if raw.strip():
  k,v=raw.split('|',1);plans[k]=v
for k,v in plans.items():notes[k].update(proposed_plan=v,decision='REPAIR')
# Explicit approved changes; all nonlisted fields and slots remain exact baseline.
changes={
'TRAINER_ROXANNE_1':[{'slot':5,'replace':{'species':'NOSEPASS','item':'EVIOLITE','ability':'STURDY','nature':'SASSY','points':'32/0/2/0/32/0','offset':4,'moves':['POWER_GEM','EARTH_POWER','WIDE_GUARD','PROTECT']}}],
'TRAINER_CRISTIAN':[{'slot':1,'update':{'nature':'JOLLY','points':'2/32/0/0/0/32'}}],
'TRAINER_MARY':[{'slot':2,'update':{'ability':'HUSTLE'}}]
}
notes['TRAINER_ROXANNE_1'].update(decision='REVISE',iconic_restoration='Restore Nosepass in slot 5, replacing Onix; other five slots unchanged. Its Wide Guard protects the actual Rock offense and Eviolite/Sturdy preserves a survival role.',proposed_plan='Carbink establishes Trick Room beside Choice Band Relicanth. Guts Larvitar and Rock Head Sudowoodo sustain physical pressure. Iconic Nosepass uses Eviolite and Sturdy to survive, Wide Guard to protect its partner against spread damage, and special Rock/Ground coverage to remain useful against physical mitigation. Bombirdier remains the last-slot ace.',proposed_crack='Carbink is the only Trick Room setter and Mental Herb absorbs its first applicable disruption. Remove it, deny it repeatedly, use your own slow attackers or manage the five-turn field. Nosepass Wide Guard blocks spread attacks but not single-target pressure; Bombirdier changes late defensive matchups. Clever ordinary teams must retain multiple winning approaches.')
notes['TRAINER_CRISTIAN'].update(decision='REPAIR',assessment='The existing six-member Justified plan is sound, but Falinks is too slow for the advertised same-turn opening boost: baseline Speed 41 at level 20 versus Gallade 77 at level 21. Changing only Falinks to Jolly and 2/32/0/0/0/32 gives Speed 80, preceding Gallade 77 under the snapshot Champions formula. This trades HP and some Attack for the explicitly intended activation timing; the other five loadouts remain exact.',proposed_plan='Jolly, Speed-invested Falinks can Beat Up its Justified Gallade partner before Gallade attacks at the intended cap. Lucario is an alternative Justified recipient when active. Coaching is a lower-damage support option when appropriate. Hitmonchan supplies coverage; Machamp and Annihilape remain the last two locked aces. Ally survival and actual turn order always take precedence over automatic activation.',proposed_crack='Disrupt Falinks or its recipient before the activation pays off, use appropriate redirection or speed control, or withstand and punish the boosted attacker. Beat Up also works on active Justified Lucario, so removing Gallade does not universally disable it. Account for Machamp and Annihilape as the final reserves.',acceptance='Reproduce Speed 41/77 before and 80/77 after at levels 20/21; check Normal floor levels 18/19 and later live-cap floors. Trace all Beat Up hits, recipient survival and +6 Justified before Gallade attacks; reject lethal ally hits and re-evaluate after paralysis, Trick Room or redirection.')
notes['TRAINER_MARY'].update(decision='REPAIR',proposed_plan='Berserk Drampa gains one Special Attack stage when a qualifying surviving hit crosses half HP. Eviolite Rufflet uses Hustle to amplify its physical attacks at an explicit accuracy cost. Guts Obstagoon, Coil Dudunsparce and flexible Silvally preserve the retaliation theme, with Mega Lopunny providing fast finishing pressure.',proposed_crack='Knocking Drampa out prevents a surviving Berserk payoff, but merely taking it below half HP in one hit still activates Berserk. Account for Rufflet Hustle accuracy, Obstagoon Guts and Mega Lopunny typing/ability; use precise physical mitigation and coverage rather than a blanket Normal-type assumption.')
companions=[
{'species':'PIKACHU','item':'LIGHT_BALL','ability':'LIGHTNING_ROD','nature':'TIMID','points':'2/0/0/32/0/32','offset':-1,'moves':['FAKE_OUT','THUNDERBOLT','ELECTROWEB','PROTECT']},
{'species':'SHROOMISH','item':'EVIOLITE','ability':'EFFECT_SPORE','nature':'BOLD','points':'32/0/32/0/2/0','offset':-1,'moves':['SPORE','LEECH_SEED','GIGA_DRAIN','PROTECT']},
{'species':'TAILLOW','item':'TOXIC_ORB','ability':'GUTS','nature':'JOLLY','points':'2/32/0/0/0/32','offset':-1,'moves':['FACADE','BRAVE_BIRD','QUICK_ATTACK','PROTECT']}]
for b in bs:
 if b['encounter']==1:
  k=b['trainer']; role={'TREECKO':'physical Grass offense with Swords Dance and priority','MUDKIP':'immediate physical Water offense and coverage','TORCHIC':'physical Fire offense with Swords Dance and recoil'}[b['mons'][0]['species']]
  notes[k].update(decision='REVISE',assessment=f"The historical baseline is a one-Pokémon single battle. The user superseded that exception: this branch becomes a four-Pokémon doubles fight after the player receives TWO different regional starters. The preserved Hoenn slot-0 preset supplies {role}; special Light Ball Pikachu supplies Fake Out and Electroweb in the lead. Shroomish offers sleep/sustain and Guts Taillow independent offense in reserve. Rival slot 0 is the unchosen third regional starter, never either owned family.",proposed_plan='The rival leads the unchosen regional starter beside Light Ball Pikachu. Fake Out can create a safe setup or attack window, while Electroweb supports turn order when it produces more value. Shroomish supplies Spore, Leech Seed and recovery damage; Toxic Orb Taillow creates independent Guts offense. Preserve four legal loadouts after all generation substitutions and select actions jointly for the current board.',proposed_crack='Prepare around the actual unchosen starter, Pikachu speed control and the available reserves. Ground immunity, Fake Out denial, powder immunity, defensive positioning and calculated offensive pressure are distinct tools. The player can catch and prepare a broader party before this battle; neither of the two chosen starter families is imposed as a mandatory solution.',final_format='double',acceptance='Run all 54 ordered two-starter choices across nine generations, for both rival identities. Assert four opponents, two active battlers, slot 0 equals the unchosen regional starter and neither owned family, three exact companions, and Normal/Medium levels never below live cap minus 2. Exercise Fake Out plus setup, Electroweb order, Spore immunity, Guts activation, loss/Retry and legal starter-preset substitution.',shared_ai_requirements=['RIVAL','INTRO_TWO_STARTERS','PRIORITY','SPEED','POWDER','RESERVES'])
  changes[k]=[{'append':copy.deepcopy(companions)}]
# All later rival prose respects the new two-choice state, not the obsolete one-choice matchup claim.
for b in bs:
 if b['cls']=='rival' and b['encounter']!=1 and ('BRENDAN_' in b['trainer'] or 'MAY_' in b['trainer']):
  n=notes[b['trainer']];n['proposed_crack']='Assess the actual regional starter and reserve composition after the two-choice starter resolver runs. Lightning Rod can protect Swellow from single-target Electric attacks, Poison Heal changes Breloom status choices, and priority/redirection rules depend on the field. Use available coverage, disruption, speed control and positioning; no single owned starter is the prescribed answer.';n['decision']='REPAIR'
  n.setdefault('dependencies',[]).append('Opening chapter: all later rival slot replacements follow the unchosen third regional starter and its appropriate evolution stage.')
# Exact proposed plan for notes whose inherited adjacent-trainer prose is not specific enough.
meta={}; master=(S/'data/emerald_champions/emerald_champions_master_battle_design.txt').read_text(); native_text=(S/'src/data/trainers.party').read_text()
for number,block in teams.split_encounters(master)[1]:
 if number<=250:
  meta[number]={key:teams.line_value(block,key) for key in ['location','chapter','strict_cap','requirement','physical_group_id','campaign_order']}
  meta[number]['line']=master[:master.index(f'=== ENCOUNTER {number:04d} ===')].count('\n')+1
  for tr in teams.BRANCH_RE.finditer(block):
   start=tr.end(); end=block.find('--- BRANCH ',start); seg=block[start:] if end<0 else block[start:end];meta[number][tr.group(1)]=teams.line_value(seg,'format')
all_source=teams.read_teams(S/'data/emerald_champions/emerald_champions_battle_teams.txt')
compiled=teams.compile_master(all_source,master)
rewritten,ncount,problems=native.implement(250,S/'data/emerald_champions/emerald_champions_master_battle_design.txt',S/'src/data/trainers.party')
import audit_emerald_champions_master_battles as audit
legality=[]
for k,cs in changes.items():
 fs=copy.deepcopy(by[k]['mons'])
 for c in cs:
  if 'append' in c:fs+=c['append']
  elif 'replace' in c:fs[c['slot']-1]=c['replace']
  else:fs[c['slot']-1].update(c['update'])
 for m in fs:
  bad={'MOVE_'+x for x in m['moves']}-audit.pinned_legal_moves('SPECIES_'+m['species'])
  if bad:legality.append([k,m['species'],sorted(bad)])
validation={'changed_team_move_legality_issues':legality,'authored_branches':len(bs),'encounters':len({b['encounter'] for b in bs}),'pokemon_slots':sum(len(b['mons']) for b in bs),'all_individually_reviewed':set(notes)==set(by),'master_compile_identical':compiled==master,'native_early_rewrite_identical':rewritten==native_text,'native_examined':ncount,'native_problems':problems,'runtime_tested':False}
records={}
dialogues=json.loads((O/'gym-dialogue.json').read_text())
for e in dialogues:
 n=notes[e['trainer']]; n.setdefault('native_text_changes',[]).append(e)
 if n['decision']=='KEEP':n['decision']='REPAIR'
for b in bs:
 k=b['trainer'];n=notes[k]; m=meta[b['encounter']]; final=copy.deepcopy(b['mons'])
 for ch in changes.get(k,[]):
  if 'append'in ch:final+=ch['append'];continue
  if 'replace'in ch:final[ch['slot']-1]=ch['replace']
  if 'update'in ch:final[ch['slot']-1].update(ch['update'])
 match=re.search(r'^=== '+re.escape(k)+r' ===$',native_text,re.M)
 if not match:match=re.search(r'^'+re.escape(k)+r'\b',native_text,re.M)
 nline=native_text[:match.start()].count('\n')+1 if match else None
 records[k]={**n,'reviewed':True,'encounter':b['encounter'],'location':m['location'],'chapter':m['chapter'],'strict_cap':int(m['strict_cap']),'requirement':m['requirement'],'source':{'authoring':'data/emerald_champions/emerald_champions_battle_teams.txt','line':b['line'],'master_line':m['line'],'native_line':nline},'baseline':b,'baseline_format':m[k],'final_format':n.get('final_format',m[k]),'proposed_mon_changes':changes.get(k,[]),'final_team':final,'final_ai_traits':b['ai'],'final_plan':n.get('proposed_plan',b['plan']),'final_crack':n.get('proposed_crack',b['crack']),'dependencies':n.get('dependencies',[])+['Shared expert-capability AI applies to every trainer class; traits are situational preferences, never a license for knowingly losing play.','Shared runtime level floor applies to all trainer fights including gyms: Normal/Medium must be at least the live player cap minus 2. Preserve authored offsets as inputs; do not bake the floor into each team.','Acquisition/preparation chapters must make multiple practical counterstrategies accessible before the encounter.','Native narrative labels and optional guidance must describe this actual final roster without prescribing one observed winning method.'],'evidence':'Individually inspected source and proposed specification; implementation, battle execution and empirical difficulty validation remain pending.'}
(R/'review/early-battles.json').write_text(json.dumps(records,indent=2)+'\n');(R/'review/early-battles.validation.json').write_text(json.dumps(validation,indent=2)+'\n')
def source_link(path,line,label):return f'[{label}](../../baseline/source/{path}'+(f':{line}'if line else '')+')'
def table(mons):
 out=['|Slot|Species|Item|Ability|Nature|Stat Points HP/Atk/Def/SpA/SpD/Spe|Cap offset|Moves|','|---:|---|---|---|---|---|---:|---|']
 for i,m in enumerate(mons,1):out.append(f"|{i}|{m['species']}|{m['item']}|{m['ability']}|{m['nature']}|{m['points']}|{m['offset']:+d}|{', '.join(m['moves'])}|")
 return '\n'.join(out)
chunks=[]
for lo in range(1,251,20):
 hi=min(lo+19,250); these=[r for r in records.values() if lo<=r['encounter']<=hi]
 if not these:continue
 name=f'{lo:04d}-{hi:04d}.md';chunks.append((name,len(these)))
 out=[f'# Early battle volume — E{lo:04d}–E{hi:04d}', '', '[Volume contract and index](README.md). Entries follow source chronology, which is an authoring order; map/story reachability remains authoritative.','', 'Each loadout is complete. Baseline text is reproduced as historical evidence, including errors explicitly repaired below. A KEEP retains exact Pokémon fields and order while adopting the book’s shared expert AI and runtime level floor. No gameplay files were edited.','']
 for r in these:
  b=r['baseline'];src=r['source'];k=b['trainer'];out +=[f"## E{r['encounter']:04d} — {k}",'',f"**{r['decision']}** — {r['location']}; {r['chapter']}; authored cap {r['strict_cap']}; baseline {r['baseline_format']}; final {r['final_format']}; class `{b['cls']}`.",'',source_link(src['authoring'],src['line'],'Authored branch')+' · '+source_link('data/emerald_champions/emerald_champions_master_battle_design.txt',src['master_line'],'Encounter metadata')+' · '+source_link('src/data/trainers.party',src['native_line'],'Native trainer source'),'',f"Baseline traits: {', '.join(b['ai']) or 'none beyond class profile'}. Source requirement: {r['requirement'] or 'not stated in master'}.",'', '**Complete baseline**','',table(b['mons']),'',f"Baseline plan: {b['plan']}",'',f"Baseline counterplay note: {b['crack']}",'','**Individual assessment**','',r['assessment'],'','**Final specification**','']
  if r['decision']=='KEEP':out+=['KEEP every field and the exact party order in the complete baseline above. Its plan and counterplay remain authored inputs subject to the shared field-aware AI contract.']
  else:
   out+=['Complete final party (all omitted-from-diff details are explicitly reproduced):','',table(r['final_team']),'',f"Final plan: {r['final_plan']}",'',f"Final counterplay note: {r['final_crack']}"]
  if r.get('iconic_restoration'):out+=['',r['iconic_restoration']]
  if r.get('native_text_changes'):out+=['','Exact native text changes are specified in [gym dialogue repairs](gym-dialogue.md): '+', '.join('`'+e['label']+'`' for e in r['native_text_changes'])+'.']
  out+=['',f"Final trainer traits: {', '.join(r['final_ai_traits']) or 'none beyond the shared expert profile'}. No class may omit tactical capabilities.",'','**Dependencies and acceptance**','',*['- '+x for x in r['dependencies']],'',f"Specific acceptance: {r['acceptance']}",'',f"Shared AI requirements: {', '.join(r['shared_ai_requirements']) or 'joint board-aware decision and replacement selection'}.",'',r['evidence'],'']
 (O/name).write_text('\n'.join(out))
counts={x:sum(r['decision']==x for r in records.values()) for x in ['KEEP','REPAIR','REVISE']}
readme=['# Early battles — complete individual review','','This volume covers every snapshot trainer branch through E0250: **266 branches, 224 encounter groups, 1,146 baseline Pokémon slots**. Stable encounter IDs have gaps; none are silently invented to fill them. Every branch has an individually written assessment, complete baseline, exact disposition, shared AI dependencies and specific acceptance scenarios. Automated rendering reproduces loadouts and links; it does not substitute for those individual reviews.','',f"Dispositions: {counts}. **257 of266 complete baseline Pokémon loadouts are preserved exactly.** Roster edits are limited to the six newly authorized opening branches, Roxanne's iconic Nosepass restoration, Cristian's measured Falinks Speed repair, and Mary's inactive Rufflet ability repair. Other REPAIR entries correct mechanical claims, party-order explanations or dialogue while preserving exact Pokémon fields.",'','## Superseding user decisions','','The first rival is now FOUR-Pokémon DOUBLES, after choosing TWO distinct regional starters and a doubles Birch rescue. The rival receives the unchosen third starter beside Pikachu. Ordinary wild/legendary encounters remain singles. All trainer classes use the most coherent available strategy; rank cannot disable tactical competence. Normal/Medium opponent levels never fall below the current live cap minus 2, including gyms. Historical baseline singles and negative offsets are evidence only, not the final specification.','', 'The two-starter opening and 54-choice resolver are specified in [the opening chapter](../../chapters/01-opening-and-starters.md).','', '## Review blocks','']
readme += [f'- [E{name[:-3]}]({name}) — {n} branches'for name,n in chunks]
readme+=['','## How to validate implementation','','For a changed entry, first check exact final materialization, species/form/move/item/ability legality against this configured build and the shared party/floor contract. Then run the entry’s specific paired-action and replacement scenarios at the actual cap. Include valid alternative player lines, immunity/field changes and a disrupted opening. A team is not proven correct merely because its intended combo succeeds in one friendly fixture. Preserve successful clever solutions.','', 'The source alignment comparison is read-only: `emerald_champions_teams.compile_master` on the full snapshot and native materialization through E0250 were compared as strings, without running write commands. Results: '+json.dumps(validation)+'.','', 'Read [regional rival profiles](regional-rival-profiles.md) for54stage profiles and270exact resolved variants. Read [exact Stat Point benchmarks](points-benchmarks.md) for the opening, Cristian and Roxanne. Read [gym dialogue repairs](gym-dialogue.md) for exact native text replacements. [Review JSON](../../review/early-battles.json) is keyed by trainer ID and contains complete baseline/final teams. [Validation JSON](../../review/early-battles.validation.json) records scoped static evidence.','', 'Stat Points are fixed bonuses before nature in the snapshot Champions formula (`src/pokemon.c:1425`), so low-level Speed cannot be inferred from familiar EV-scaled expectations. Cristian’s repair specifically uses this formula. Ability names and move effects are drawn from the configured source; current simulator reputations are not substituted for this build.','', 'No ROM was built, no game source or save was changed, and no fresh runtime wins are claimed.']
(O/'README.md').write_text('\n'.join(readme)+'\n')
print(json.dumps({'counts':counts,'validation':validation,'chapters':len(chunks),'final_slots':sum(len(r['final_team'])for r in records.values())},indent=2))
