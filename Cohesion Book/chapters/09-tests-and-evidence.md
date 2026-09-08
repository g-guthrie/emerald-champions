# 09 — Tests, gates and honest evidence

The user explicitly warned that many existing tests and gates are outdated, unnecessary or hostile to creative changes. Treat them as code and claims to examine. A failure is evidence to investigate, not authority to restore old design.

## TEST-01 — Decide the contract before changing the game

For any failing or inherited check:

1. Identify the current user requirement it is supposed to protect.
2. Read the actual assertion and its fixture/configuration, not only the test name.
3. Confirm the source/artifact versions, battle format, actual form/ability, effective levels and Champions Points calculation.
4. Determine whether it fails for a real behavior defect, stale assumptions, broken setup or an obsolete preference.
5. Keep a meaningful check, update a superseded contract, or retire a redundant/invalid check with its reason recorded.

Do not weaken a valid save/transaction/battle invariant merely to get green output. Equally, do not change good teams, restore singles, remove intended move access or preserve arbitrary species placement to satisfy a historical gate.

## Disposition of relevant existing checks

|Check or family|Final disposition|Reason and exact treatment|
|---|---|---|
|Authored teams → master → native trainer source agreement|KEEP/UPDATE|Protect the current authoring pipeline. Regenerate from the new intended authoring; do not compare the new game to the old snapshot as a permanent requirement. Update opening format and regional-pair handling.|
|Configured species/ability validity|KEEP|A requested ability that falls back to another slot changes observable battle behavior. Validate starting and transformed forms correctly.|
|66 total/32 per-stat preparation budget and display/engine order|KEEP|These are actual current builder rules. Use the native flat-Points calculation and effective IV31, not old EV/IV conventions.|
|New Medium cap-minus-two floor|UPDATE|Existing gym exemptions and the old singleton opening are superseded. Test actual runtime levels at entry and revisit, without double-reducing facility opponents.|
|Old opening singles/minimum party exceptions|RETIRE/UPDATE|INTRO-01 is explicitly doubles with two starters; Route103 has four opponents. Preserve full 6+6 and half 3+3 multi semantics instead of forcing one old count.|
|Map/label/warp/geometry integrity|KEEP narrowly|Dangling references, invalid coordinates and wrong source assets can break the game. A static graph does not prove stateful traversal. Do not freeze every historic object position or dialogue sentence.|
|Wild table shape, configured species, level bounds and current 5%policy|KEEP/UPDATE|These protect usable distribution. No fixed twelve-unique-species quota, old slot weight requirement, or fixed roster count. Reflect explicit new fishing overrides.|
|Mega reward transaction and coverage of actually enabled stones|KEEP/UPDATE|Protect real rewards, flags and bag-full retries. Derive enabled items from current data. Do not require exactly 99 forever or one particular past location.|
|Legendary caught/unlocked identity and state capacity|KEEP/UPDATE|Exact forms, permanent captures and finite bit capacity are real requirements. Append the proposed Galar bird IDs safely; retire old mandatory-Devon/permanent-loss expectations.|
|Source/ROM/ELF stamp and save integrity checks|KEEP|These protect against testing stale artifacts or corrupting player data. They do not certify game quality.|
|Pinned external learnset comparison|ADVISORY under the expanded-native policy|The current game deliberately supplies expanded preset moves and has special form mappings. Record differences; do not mass-remove moves solely because of this reference.|
|Repeated team fingerprints or mandatory every-Mega/every-legendary showcases|RETIRE as blockers|`audit_emerald_champions_master_battles.py` includes these checks around 968–982. Intentional thematic repetition or a targeted nostalgia substitution must not be forbidden by an old showcase quota.|
|Five consecutive strategies / repeated species across neighboring teams|ADVISORY, not blockers|The same audit includes such quotas around 1029–1046. Judge actual battles and their context. Identical labels can hide different puzzles; different labels do not prove variety.|
|Broad competitive-quality heuristics|ADVISORY|`audit_battle_set_quality.py` already describes optional heuristics. Lower power, specialized investment, unusual moves or an intentional risk cannot be rejected without team context.|
|Exact old prose, NPC totals, screenshot hashes and historical corpus floors|RETIRE unless they protect a distinct current failure|Render new text and inspect changed scenes for actual layout/access problems. Do not require old wording or revive a deleted test corpus.|
|Existing working Room, Instruct, Sitrus, Palafin, item and party regressions|KEEP relevant cases|Audit fixture quality first, then preserve observable behaviors that are still required. Avoid duplicating equivalent tests around every trainer ID.|

The snapshot release path has already been reduced. Its current deterministic gates focus on source/materialization/structure; the existence of broader standalone scripts does not mean their every assertion should be reinstated as a release condition.

## TEST-02 — Small evidence sets with independent value

Use shared cases for shared behavior, with the actual affected trainer scenarios as integration evidence. The following groups each protect a distinct failure mode:

- **Opening and persistent transactions:** two different starters inserted atomically;54 selection outcomes; pre-rival supplies; partial/full PC; legacy migration; second-starter Mega receipts and Steven reappearance. The 54-case matrix covers the actual current selection domain, not a permanent future test-count floor.
- **Levels and Points:** Medium floor at gyms/routes/revisits; source stat formula; nature/priority/Speed; a precise timing repair and actual berry thresholds. Fixtures specify their stats and do not accidentally bypass native calculation through testing-only fixed-stat flags.
- **AI state correctness:** one switch-entry hazard/self-effect application, candidate-order independence, truthful first-hit Berry damage, actual base/Mega simulation and independent owner budgets.
- **AI action selection:** a useful support pair, a disrupted setup, deliberate ally activation, an ace needed early, Palafin before its last switch opportunity, and a replacement that must survive both opponents. Do not count forced scripted moves as autonomous AI success.
- **Acquisition and reward state:** a rejected Sweet Scent draw cannot launch a stale enemy party; exact Galar bird identities; one-save irreversible evolution alternatives; independent gift/stone/prize delivery after full-bag/PC failure; no free protected-relic bypass.
- **World and format:** mandatory story handoffs, two-starter rescue choreography, no native trainer singles entry, local Tent returns, Hill timer/party cleanup, and reachable pending reward NPCs.
- **UI and source binding:** readable two-name starter confirmation and relevant menus, correct displayed stats/methods, and the specific ROM/ELF/input stamp used for any runtime result.

A useful implementation test should be able to fail for the behavior it protects. Prefer production functions and real battle/field paths over a second implementation that simply reproduces the proposed answer. Test-build bypasses are explicit: for example `CanMegaEvolve` skips the player's Ring check under TESTING, so that environment cannot prove the production no-Ring gate by itself.

## TEST-03 — What book validation does and does not prove

The book's inventory, source-link, no-overlap, shape and coverage checks establish that the document accounts for its frozen input and that its proposed records are internally consistent. They are not new game gates.

The source snapshot contains 516 authored branches,540 Hoenn maps,138 wild-header map IDs and 99 stone items. Those counts are useful for detecting an omitted page in this book. A future creative revision can legitimately change any inventory size. The document must update with that revision rather than constrain it.

The book was written without a ROM build or source implementation. Mathematical Points benchmarks are calculations; historical tests retain their own source boundary; source analysis is not a campaign playthrough. Actual implementation still needs the stated new evidence. Do not describe a catalogue export, a static pass or a traversal harness that auto-wins battles as proof that every fight is challenging, optimal or fun.

## Implementation reporting

For each patch, report the intended behavior, exact changed scope, what evidence was obtained, what failed and why, and what the next check establishes. Once relevant checks pass, move forward; do not repeat unchanged suites without a new reason. Design review, runtime battle quality, fresh-save traversal and release/artifact integrity remain distinct evidence.
