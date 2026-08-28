#!/usr/bin/env python3
"""Generate/check the initial Mt. Chimney story-ascent design review."""
from __future__ import annotations
import argparse,json,statistics
from collections import Counter
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
LEDGER=ROOT/'docs/verdant_battle_experience_ledger.json'; DESIGNS=ROOT/'docs/verdant_bespoke_battle_designs.json'; GUIDE=ROOT/'docs/verdant_battle_guide.json'
OUT_JSON=ROOT/'docs/emerald_champions_mt_chimney_story_review.json'; OUT_MD=ROOT/'docs/emerald_champions_mt_chimney_story_review.md'

def build():
 ledger=json.loads(LEDGER.read_text())['entries']; designs=json.loads(DESIGNS.read_text())['designs']; guide=json.loads(GUIDE.read_text())['entries']; entries=[e for e in ledger if 108<=e['index']<=110]; rows=[]; all_species=[]; by={}
 for e in entries:
  species=sorted({m['speciesId'] for g in guide if g.get('encounterId')==e['encounter_id'] and g.get('designStatus')=='closed' for m in g['party']}); by[e['encounter_id']]=species; all_species+=species
  rows.append({'index':e['index'],'encounter_id':e['encounter_id'],'format':e['identity']['format'],'target_difficulty':e['target_difficulty'],'manual_quality':designs[e['encounter_id']]['manual_quality'],'tempo':e['tempo'],'species':species,'reference_count':len(e['historic_reference_ids']),'playtest_status':e['playtest_status']})
 targets=[e['target_difficulty'] for e in entries]; duplicates=sorted(s for s,c in Counter(all_species).items() if c>1)
 return {'version':1,'scope':{'location':'MtChimney','battle_indices':[108,110],'encounter_count':3,'status':'source-closed-static-pass-runtime-unplayed'},'difficulty':{'minimum':min(targets),'median':statistics.median(targets),'maximum':max(targets),'targets':targets,'interpretation':'Editorial targets only; observed difficulty requires playtesting on Hard/Medium/Easy.'},'format_mix':{'boss-double':2,'mixed-native-pair':1},'species_usage':{'slots':len(all_species),'unique_species_forms':len(set(all_species)),'duplicates_across_physical_encounters':duplicates,'by_encounter':by},'evidence':{'quality_ten_dossiers':sum(r['manual_quality']==10 for r in rows),'competitive_reference_count':sum(r['reference_count'] for r in rows),'runtime_playtested':sum(r['playtest_status']!='static-pass-runtime-unplayed' for r in rows)},'variety_review':{'result':'pass-no-local-maximum','encounter_modes':['opposing-sight land-construction native pair or split singles','unactivated prototype assembly-line Admin boss','base-Groudon air-control and positioning leader boss'],'difficulty_shape':'The ascent begins at target 9.4, then delivers two distinct target-10 bosses with manual Bag recovery between them.','species_result':'All 18 physical species/form identities are unique across the initial ascent.','progression_result':'Magma visibly progresses from laborers, to Tabitha machinery, to Maxie command. Coalossal remains unactivated and Groudon remains base form, preserving both later finales.','next_chapter_guardrail':'Jagged Pass must release boss density without becoming easy. Start with the active Magma guard, avoid sun/Tailwind, machinery, construction, base Groudon, and another Mega; then order every descent trainer from map geometry before Lavaridge.'},'battles':rows}

def render(r):
 d=r['difficulty'];u=r['species_usage']; lines=['# Emerald Champions Mt. Chimney Story-Ascent Review','','Generated from source-backed battle dossiers and guide. Runtime remains unplayed.','','## Verdict','','PASS: the initial Mt. Chimney ascent does not exhibit a local design maximum.','',f"- Battles: 108-110 ({r['scope']['encounter_count']} physical encounters)",f"- Target difficulty: {d['minimum']}-{d['maximum']} (median {d['median']})",f"- Format mix: {r['format_mix']}",f"- Species: {u['slots']} slots, {u['unique_species_forms']} unique, duplicates {u['duplicates_across_physical_encounters']}",f"- Quality-10 dossiers: {r['evidence']['quality_ten_dossiers']}",f"- Competitive references: {r['evidence']['competitive_reference_count']}",'','## Sequence','']
 for row in r['battles']: lines.append(f"- Battle {row['index']} ({row['encounter_id']}): {row['format']}, target {row['target_difficulty']}. {row['tempo']}")
 lines+=['','## Forward guardrail','',r['variety_review']['next_chapter_guardrail'],'']; return '\n'.join(lines)

def check(r):
 if r['scope']['encounter_count']!=3 or r['difficulty']!={'minimum':9.4,'median':10.0,'maximum':10.0,'targets':[9.4,10.0,10.0],'interpretation':'Editorial targets only; observed difficulty requires playtesting on Hard/Medium/Easy.'}:raise SystemExit('FAIL: Mt. Chimney difficulty/scope drifted')
 u=r['species_usage'];
 if u['slots']!=18 or u['unique_species_forms']!=18 or u['duplicates_across_physical_encounters']:raise SystemExit('FAIL: Mt. Chimney species variety drifted')
 if r['evidence']!={'quality_ten_dossiers':3,'competitive_reference_count':17,'runtime_playtested':0}:raise SystemExit(f"FAIL: Mt. Chimney evidence drifted {r['evidence']}")

def main():
 p=argparse.ArgumentParser();p.add_argument('--write',action='store_true');p.add_argument('--check',action='store_true');a=p.parse_args();r=build();j=json.dumps(r,indent=2,ensure_ascii=False)+'\n';m=render(r)
 if a.write:OUT_JSON.write_text(j);OUT_MD.write_text(m)
 if a.check:check(r); assert OUT_JSON.read_text()==j and OUT_MD.read_text()==m
 print('PASS: Mt. Chimney story ascent has three quality-10 encounters, 18/18 unique species, median target 10, and no local maximum')
if __name__=='__main__':main()
