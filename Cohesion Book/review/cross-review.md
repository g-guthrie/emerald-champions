# Cross-volume QA — competition, opening and world contracts

This was a bounded independent reconciliation pass over the final charter, opening and competition chapters, both battle volumes, the world proposal/event ledgers and all fifteen quiz questions. The active wild, form and Mega authoring chapters were not re-audited wholesale. The second-starter/Mega handoff was examined as a specific shared-state dependency with its owner.

All seven concrete findings below were resolved **in the implementation specification** during this pass. This is not a claim that the game has been changed or that the repaired behavior has run. No game source, frozen baseline, ROM or save was modified.

## Findings and their resolutions

### QA-01 — Settled encounter formats and full two-trainer parties

**P1 · Resolved in book.** The charter initially still called wild-format clarification pending and described every multi opposing side as six. The user had explicitly settled ordinary wild/legendary captures as singles and Birch's rescue as doubles. E0127 also deliberately brings six Courtney Pokémon plus six grunt Pokémon.

Evidence: [configured full teams](../baseline/source/include/config/battle.h:403), [party capping predicate](../baseline/source/src/battle_setup.c:2301), [Meteor Falls two-trainer call](../baseline/source/data/maps/MeteorFalls_1F_1R/scripts.inc:56), and the absence of `Multi Party: Half` in its two native party blocks. The six E0054/E0060/E0061 contributions and the two E0409 contributions are explicitly three-member Half components; they are not standalone undersized trainer teams.

Resolution: [charter rule 4](../chapters/00-design-charter.md:16) and [TEAM-01](../chapters/02-competition-rules.md:44) now distinguish 3+3 Half layouts from 6+6 Full layouts, retain the explicit capture exception, and prohibit new trainer singles. Do not flatten E0127 to six or inflate every Half component to four.

### QA-02 — A failed early supply grant could retrigger the lab every frame

**P1 · Resolved in book.** The first opening specification kept the new opening state at 2 after a full Bag/PC while promising the player could leave. It had not explicitly advanced the old `VAR_BIRCH_LAB_STATE`, whose value 2 automatically invokes `GiveStarterEvent` on the lab's OnFrame callback. Releasing without changing that old state can immediately lock the player again.

Evidence: [lab OnFrame and receipt entry](../baseline/source/data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc:121).

Resolution: [the opening handoff](../chapters/01-opening-and-starters.md) explicitly sets **old lab state 3 before any failure can release control**, while **new opening state 2** keeps supplies owed. Birch/aide interaction resumes the transaction; Route103 still requires new state 3. The source-authoring helper and [opening JSON](opening.json) include this same contract. Acceptance now exercises several free frames, lab exit/re-entry, storage recovery and exactly one successful packet.

### QA-03 — Eviolite quiz had two truthful answers

**P2 · Resolved in book.** Mechadoll 3 question 3 asked which Pokémon can receive Eviolite's benefit, with both “Any baby Pokémon” and “One that can still evolve” offered. Ordinary baby Pokémon can evolve, making the first answer a truthful subset rather than a clean distractor. Matching the coded index alone would not detect this ambiguity.

Resolution: [W-TRICK-QUIZ](../world/changes.md) now uses “One with no evolution” as option 0 and keeps “One that can still evolve” at correct index 1. All fifteen questions were read for mechanical meaning and answer uniqueness; their other correct answers are consistent with the stated ordinary/configured conditions. This was a semantic review, not execution of all forty-five menu branches.

### QA-04 — Native link/Secret Base singles lacked exact retirement instructions

**P1 · Resolved in book.** The generic W-C-LINK policy did not itself replace concrete singles entrypoints. The imported Secret Base root reaches a C constructor that sets `TRAINER | SECRET_BASE` without `DOUBLE`; wired/wireless menus explicitly offer Single Battle.

Exact sources:

- [Secret Base shared root](../baseline/source/data/scripts/secret_base.inc:284) and [battle entry](../baseline/source/data/scripts/secret_base.inc:638).
- [Secret Base battle flags](../baseline/source/src/secret_base.c:1162).
- [Wired selection and SingleBattleMode](../baseline/source/data/scripts/cable_club.inc:284).
- [Wireless selection and WirelessSingleBattle](../baseline/source/data/scripts/cable_club.inc:992).
- [Shared battle-mode menu](../baseline/source/src/data/script_menu.h:568).

Resolution: [LINK-01](../chapters/02-competition-rules.md:132) specifies Double/Multi/Info/Exit indices, exact protocol values, guarded obsolete singles labels, safe old-session cleanup, and retirement of native Secret Base challenge dispatch before save/daily-owner writes. Imported data and trading remain intact. The four-member link admission check must be scoped to that interface/peer validation; it must **not** redefine the common two-usable-Pokémon guard used by ordinary story battles and the new rival.

### QA-05 — Legacy second-starter receipt skipped the new Dex/Ball transaction

**P1 · Resolved in book.** A v3 save at old lab state 3 has already received its one starter but has not received the old post-rival Dex/Balls. The first migration text moved new state 5 directly to new state 3 after the supplemental Pokémon, falsely making such a save ready for the four-Pokémon rival without the new capture access.

Evidence: [old post-rival Dex and Ball handoff](../baseline/source/data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc:393).

Resolution: legacy old lab states **2/3/4** now process the supplement, remain freely escapable at **old lab state 3**, enter **new opening state 2**, then use the shared early supply transaction. Only after success may new state 3 be set; an already-defeated rival flag queues the old state-4 debrief instead of replaying the fight. Only fully wrapped old lab state 5 plus the Dex receipt skips directly to new state 4. [Opening JSON](opening.json) includes phase-by-phase storage/cancel/reload cases.

### QA-06 — Previously departed Steven could not deliver new second-family stones

**P1 · Resolved in book, coordinated with Mega owner.** A v3 save may already have hidden Steven after its old one-family stone handoff. Extending the receipt bits and saying “do not depart while pending” does not restore an actor who has already departed.

Evidence: [Steven's existing delivery/departure path](../baseline/source/data/maps/GraniteCave_StevensRoom/scripts.inc:24) and [current two-slot receipt loop](../baseline/source/src/field_specials.c:633).

Resolution: second-family slots become earned only after actual successful second-Pokémon delivery. At migration completion and supplemental-gift commit, reconcile newly populated unreceipted slots. Once the old letter/Knuckle Badge/Ring handoff is complete, clear **only** Steven's hide flag when such a real entitlement is pending. His return enters pending stone delivery without replaying the letter, Ring or first-family gifts. Empty/fulfilled slots and an undelivered second Pokémon do not wake him. Full storage preserves the pending receipt and actor availability. The opening helper/JSON and Mega-owner contract now agree.

### QA-07 — A legacy supplement should not wait for Birch's daily return

**P2 · Resolved in book.** Birch is outside the lab on state values 2–5, with a daily state update. A lab-only professor handoff can delay a basic compatibility gift for days before the League.

Evidence: [Birch location routing](../baseline/source/data/scripts/prof_birch.inc:1), [daily update](../baseline/source/src/time_events.c:116), and the always-present `LOCALID_BIRCHS_LAB_AIDE` with visibility flag 0 in the lab map.

Resolution: Birch and the aide call the same handler and share receipts. Priority is **state-5 owed second starter → state-2 early supplies → optional ResearchPartners menu → ordinary dialogue**. The aide can fulfill the legacy gift while Birch is on Route101/103, without changing his location flags or timer. The form reviewer confirmed the same priority for optional second Kubfu/Cosmog and postgame form gifts.

## Independent checks that passed

- The early and late review key sets have no overlap and exactly cover the **516 snapshot trainer IDs**. No expected ID is missing and no extra ID is disguised as baseline coverage.
- The proposed catalog has **2,339 Pokémon slots**, reflecting the eighteen additional first-rival companions. Every final point vector is six values, at most 32 per stat and at most 66 total. No duplicate move slot or duplicate non-NONE held item was found within a final authored opponent party.
- The only final parties below four are the eight explicitly paired three-member Half contributions. The six opening aliases all have four Pokémon and doubles format. E0127 preserves both full six-member parties.
- The nine late Walrein/Dusclops/Mega Starmie benchmark rows were independently recomputed from the configured species inventory and native integer formula; all matched. The early appendix explicitly separates priority from Speed, uses forced IV31, and does not count Light Ball or Eviolite as displayed Stat Points.
- Iconic constraints remain concrete: Roxanne gains Nosepass, Phoebe gains Dusclops, Glacia gains Walrein; Norman's Slaking, Winona's Altaria, Wallace's Milotic and Steven's Metagross remain meaningful roster members. Secondary nostalgic choices are explicitly considered rather than silently replacing additional authored roles.
- The world ledger accounts for **540 registered maps and 6,322 baseline event/callback records**. Its own scope accurately describes source/digest/common-contract review and structural coverage, not instruction-by-instruction runtime execution. The 54 opening choices and 270 regional rival variants are derived specification matrices, not claims of that many manually played battles.
- Expanded native move access remains authoritative. Squirtle Follow Me, Cyndaquil Nature Power, Piplup Scald and Scorbunny Blaze Kick are retained. Narrower pinned-reference omissions are documented as policy differences, not used to purge existing moves.

These are bounded static/book checks. They do not establish every AI action, acquisition route, map traversal, damage threshold or battle difficulty in an implemented ROM.

## Relevant test and gate dispositions

The user explicitly required tests to be questioned as code, especially where old design assumptions constrain good authoring. The following dispositions belong in the shared test audit and implementation plan, not as additional blanket gates.

| Check or assumption | Disposition | Reason and useful replacement |
|---|---|---|
| Authored source → master → native loadout equality | **KEEP** | Protects deliberate user-authored fields. Bind all inputs/artifacts to the same source revision; do not pass against a stale generated party. |
| Trainer IDs, event references, warp destinations and script return contracts | **KEEP** | Structural integrity has a real failure contract. Distinguish inactive/source-only roots from accessible content. |
| Old opening singleton and gym floor exemptions | **UPDATE** | Replace with the approved two-starter rescue/four-mon rival and universal Medium floor, preserving ordinary wild singles and explicit Half/Full ownership. |
| Pokémon preparation point checks | **UPDATE** | Use the actual 66/32 contract, display-to-engine mapping, fixed IV31 and native nature rounding. Retire fixtures that pretend zero stored Speed IV changes production stats. |
| Pinned-reference moves as universal legality law | **UPDATE** | Compare against the chosen expanded native access policy. Keep the narrower reference comparison as advisory; an omission alone does not establish an illegal in-game move. |
| Exact team fingerprint uniqueness | **RETIRE as design blocker** | [The existing audit hard-fails duplicates](../baseline/source/scripts/audit_emerald_champions_master_battles.py:968), but repeated characters/contexts can justify a repeated composition. Keep duplicate **trainer-ID** protection separately. |
| Every Mega and legendary must appear on an opposing team | **RETIRE as design blocker** | [Showcase coverage checks](../baseline/source/scripts/audit_emerald_champions_master_battles.py:972) can force unrelated swaps after an explicitly desired restoration. Keep collection/acquisition completeness as a separate real requirement. |
| Five repeated primary-strategy labels or more than 35 local species repeats | **RETIRE as design blocker** | [These fixed quotas](../baseline/source/scripts/audit_emerald_champions_master_battles.py:1029) are subjective design restrictions. Report patterns for contextual review rather than mechanically weakening theme or strong teams. |
| Prose keyword/strategy-description locks | **RETIRE as mechanics proof** | Match actual moves, abilities, forms and outcomes. A slogan containing “Trick Room” does not prove a setter or legal action exists. |
| Book counts 516/54/270/540/6322 | **KEEP for this book's snapshot reconciliation only** | These establish that this finite inventory was accounted for. They must not become future gameplay count floors or prevent legitimate additions/deletions. |
| Focused production-function and battle fixtures | **KEEP/UPDATE proportionally** | Preserve tests that can fail for the actual defect: wrong order, failed receipt, premature story commit, illegal target, stale party or discarded reward. Audit fixture state before changing game behavior; do not require a predetermined winning team or a fixed solution script. |

The review does not recommend deleting useful independent behavioral tests because a broad historical quota is obsolete. Nor does it propose a new registry of rigid team-strength, rarity, strategy or prose quotas.
