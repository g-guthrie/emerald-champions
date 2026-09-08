from pathlib import Path
import json,collections,copy,re
B=Path(__file__).resolve().parents[1];out=B/'appendices/wild';out.mkdir(parents=True,exist_ok=True)
w=json.loads((B/'inventory/wild.json').read_text());changes=json.loads((B/'review/wild-proposals.json').read_text())
# Additional form-access edits are merged here only after the owning reviewer supplies exact rows.
extra=B/'review/wild-form-additions.json'
if extra.exists():changes+=json.loads(extra.read_text())
notes=dict(l.split('\t',1) for l in (B/'review/wild-assessments.tsv').read_text().splitlines() if l.strip())
by=collections.defaultdict(list)
for e in w:by[e['map']].append(e)
assert set(notes)=={m.removeprefix('MAP_') for m in by}
raw=(B/'baseline/source/src/data/wild_encounters.json').read_text().splitlines();src_lines={}
for i,l in enumerate(raw,1):
 m=re.search(r'"map": "(MAP_\w+)"',l)
 if m:src_lines.setdefault(m[1],i)
ledger={};index=['# Complete wild encounter review','','Every registered Hoenn map with an ordinary encounter header is represented below. Each page reproduces all ordinary method slots, gives a map-specific assessment, and specifies the complete final table for changed methods. There are138unique mapIDs,146headerrows includingAlteringCave selectors, and380methodrecords when thethree rods are counted separately.','','These are source/catalogue records, not380independent walks or battle tests. Dormant Hidden rows are shown separately and excluded from active acquisition claims (GUIDE-01). Ordinary wild/capture formats remain singles.','','|Map|Decision|Methods|Specific assessment|','|---|---|---:|---|']
def tbl(t):
 return '\n'.join(['|Slot (zero-based)|Species|Chance|Authored levels|','|---:|---|---:|---|']+[f"|{p['slot']}|{p['species']}|{p['weight']}%|{p['min_level']}–{p['max_level']}|" for p in t['slots']])
for mid,es in by.items():
 name=es[0]['name']; cs=[c for c in changes if c['map']==mid];note=notes[mid.removeprefix('MAP_')];decision='REVISE' if cs else 'KEEP';p=out/(name+'.md')
 page=[f'# {name} — wild distribution', '',f'**Decision: {decision}.** {note}', '', f'[World pathways and interactions](../../world/maps/{name}.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L{src_lines[mid]})', '', '## Common final behavior', '', 'Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.', '', 'Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.', '']
 rec={'map':mid,'name':name,'decision':decision,'assessment':note,'source_line':src_lines[mid],'changes':cs,'header_rows':[]}
 for ix,e in enumerate(es):
  page+=[f'## Header row {ix+1}'+(' — legacy selector variant' if len(es)>1 else ''),'']
  if len(es)>1:page+=['`VAR_ALTERING_CAVE_WILD_SET` selects a single one of these nine contiguous rows; out-of-range values fall back to0. Their union must not be presented as simultaneous normal availability. Preserve legacy selector compatibility; root0 is the ordinary default.','']
  h={'base_label':e['base_label'],'methods':{}}
  for method,t in e['methods'].items():
   final=copy.deepcopy(t);mc=[c for c in cs if c['method']==method]
   for c in mc:
    hits=[p for p in final['slots'] if p['slot']==c['slot']];assert len(hits)==1,(mid,c)
    assert hits[0]['species']==c['from'],(mid,c,hits)
    hits[0]['species']=c['to']
   page +=[f'### {method}', '',f'Native field `{t["table_field"]}`; encounter-rate value `{t["encounter_rate"]}`.', '', '**Current slots**', '',tbl(t),'']
   if mc:
    page+=['**Final slots**','',tbl(final),'']
    for c in mc:page+=[f'- **{c["id"]}:** {c["rationale"]}']
    page+=['']
   else:page+=['**Final: KEEP the complete current slots above.**','']
   h['methods'][method]={'baseline':t,'final':final,'decision':'REVISE' if mc else 'KEEP'}
  if e['inactive_hidden']:
   page += ['### Dormant Hidden source data','','These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.','', '|Species|Authored levels|','|---|---|']+[f"|{x['species']}|{x['min_level']}–{x['max_level']}|" for x in e['inactive_hidden']['mons']]+['']
  rec['header_rows'].append(h)
 page+=['## Implementation and acceptance','','- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.','- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.','- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.','- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.','- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.','']
 p.write_text('\n'.join(page)+'\n');ledger[mid]=rec
 index.append(f'|[{name}](wild/{name}.md)|{decision}|{sum(len(e["methods"]) for e in es)}|{note}|')
(B/'appendices/wild-encounter-catalog.md').write_text('\n'.join(index)+'\n');(B/'review/wild-distribution.json').write_text(json.dumps(ledger,indent=2,ensure_ascii=False)+'\n')
print('wildpages',len(ledger),'revisedmaps',sum(v['decision']=='REVISE' for v in ledger.values()),'slotchanges',len(changes))
