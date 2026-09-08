from pathlib import Path
import re,json
B=Path(__file__).resolve().parents[1];R=B/'baseline/source'
names=['FLAG_EC_GIFT_INDEEDEE_F','FLAG_EC_GIFT_COSPLAY_PIKACHU','FLAG_EC_GIFT_BATTLE_BOND_GRENINJA','FLAG_EC_GIFT_PARTNER_PIKACHU','FLAG_EC_GIFT_PARTNER_EEVEE','FLAG_EC_GIFT_SECOND_KUBFU','FLAG_EC_GIFT_SECOND_COSMOG','FLAG_EC_TRICK_FINAL_TENT_RECEIVED','FLAG_EC_TRICK_FINAL_ALAKAZITE_RECEIVED','FLAG_EC_TRICK_FINAL_STATE_INITIALIZED']
flags=(R/'include/constants/flags.h').read_text();vars=(R/'include/constants/vars.h').read_text();rows=[]
paths=[p for root in ['src','include','data','asm','scripts'] for p in (R/root).rglob('*') if p.is_file() and p.suffix in ['.c','.h','.inc','.s','.json','.py']]
texts={p:p.read_text(errors='replace') for p in paths}
for k,name in enumerate(names):
 value=0x2AB+k;old=f'FLAG_UNUSED_0x{value:X}'
 assert re.search(r'^#define\s+'+old+r'\s+0x'+format(value,'X')+r'\b',flags,re.M),(name,old)
 refs=[str(p.relative_to(R)) for p,t in texts.items() if re.search(r'\b'+old+r'\b',t)]
 assert refs==['include/constants/flags.h'],(name,refs)
 rows.append({'symbol':name,'address':f'0x{value:03X}','replaces':old,'baseline_references':refs,'initial_value':0,'initialization':'Version4 migration explicitly initializes reused receipt bits before use; Trick initializer then reconstructs its legacy entitlements under the world contract. New games start clear.'})
vs=[]
for value,name,old in [(0x40E1,'VAR_EC_SECOND_STARTER','VAR_EC_RETIRED_HATCH_SPECIES_1'),(0x40E2,'VAR_EC_OPENING_STATE','VAR_EC_RETIRED_HATCH_SPECIES_2'),(0x40E3,'VAR_EC_CIRCUIT_BEST_WINS','VAR_EC_RETIRED_HATCH_SPECIES_3')]:
 refs=[str(p.relative_to(R)) for p,t in texts.items() if re.search(r'\b'+old+r'\b',t)]
 assert refs==['include/constants/vars.h'],(name,refs)
 vs.append({'symbol':name,'address':f'0x{value:X}','replaces':old,'baseline_references':refs})
j={'scope':'Proposed book allocation only. No headers or saves changed.','flags':rows,'variables':vs,'preserve':['VAR_STEVEN_STARTER_STONE_DELIVERY at 0x40E0: on v4 migration field &= 0x3 preserves old bits 0/1 and initializes new second-family bits 2/3; do not repeat on v4 loads','LegendarySignIDs82/83/84 use existingword5bits2/3/4 within96bitcapacity; clearonlythese newlyallocatedbits onv4migration, preserveold82andearnedArceus','All other map/object/trainer identifiers and save layout'],'evidence_limits':'Declaration-only symbolic scans plus source-state-owner review. A coincident0x2AC graphics baseBlock is not a flag reference. Recheck allocation against current live source before implementation.'}
(B/'review/state-allocations.json').write_text(json.dumps(j,indent=2)+'\n')
md=['# Proposed persistent state allocation','','No save structure is enlarged. These are reservations for the book\'s implementation, not changes to the current game. The frozen-source symbolic scan found only the retired/unused declarations for each reclaimed name. Recheck against live source when implementing.','','|New symbol|Address|Replaced declaration|','|---|---|---|']+[f"|`{x['symbol']}`|`{x['address']}`|`{x['replaces']}`|" for x in vs+rows]
md += ['', '## Migration order', '']
for paragraph in ['One version 4 migration owns all reclamation. Run it before the existing version 3 early return can bypass new initialization. Preserve saved parties, boxes, catches, difficulty, original scene flags and actual earned receipts. Clear retired nursery values from the three reused variables, then populate opening/best state from the specific trustworthy old fields. Initialize the ten reused flags to zero exactly once. Then reconstruct Trick House legacy receipt state according to its ambiguity-aware world contract; do not clear its initializer on later loads.', 'Existing starter-stone delivery bits 0/1 remain first-family receipts. On version 4 migration, mask the old delivery field with `0x3` to preserve those actual receipts and initialize the newly assigned bits 2/3 to zero before the second-family entitlement is calculated. Bits 2/3 then record second-family receipts; absent stones are not owed. The 85-Sign revision uses additional bits within the existing 96-bit words: clear only new caught/unlocked bits 82–84 on version 4 initialization, preserve the original 82 Sign entries and earned Arceus, and never shift previous Sign IDs.', 'Best recorded initializes from observed active current Circuit wins only; zero means no best has been recorded yet. Never copy old nursery species or lifetime wins into it. Subsequent loads must not reset new receipts or best.', 'Acceptance covers new games, pre-version-4 saves at all opening phases, completed version 3 games, interrupted gifts/storage failure, already-owned first-starter stones, pending second-starter stones, ambiguous final Trick prizes, caught base/Galarian birds, and idempotence on the next ordinary load.']:
 md += [paragraph, '']
(B/'appendices/state-allocation.md').write_text('\n'.join(md)+'\n');print('Reservedvars',len(vs),'receiptflags',len(rows))
