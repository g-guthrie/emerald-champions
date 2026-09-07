# Campaign tuning: route conversations and coordinated gyms

The September 5, 2026 user direction keeps the opening demanding. Difficulty settings remain global level offsets; they do not select different teams or AI. These changes reduce repeated interruptions and improve coordination, without lowering trainer levels.

## Route pacing

Converted **45 of 223 eligible ordinary outdoor-route encounter groups (20.2%)** into conversations. This is not a 20% cut to the whole campaign. Gyms, rivals, route aces/experts, story fights, family gauntlets, and indoor Trick House/Seashore challenges are excluded.

The active campaign now contains **468 physical encounter groups and 516 trainer branches**, down from 513/561. IDs retain gaps to preserve existing references. Retired party rows are removed; old-save compatibility is intentionally outside this change.

Every retired NPC keeps its original object ID, location, sprite, movement and visibility flag. Only trainer sight behavior and dialogue change. Their scripts terminate in a conversation, including old rematch entry points.

All **782 species** previously showcased in authored battles remain showcased. All **92 Mega Stone showcases** and **76 legendary showcase requirements** remain represented. Distinctive early encounters were deliberately spared rather than meeting a per-route percentage quota. Dale's Focus Energy/Scope Lens/Sniper Octillery strategy moves to Alex, whose existing Togekiss provides redirection support; no party slot or level was added.

| Route | NPCs now offering conversation |
|---|---|
| Route103 | Andrew, Marcos, Pete, Isabelle |
| Route105 | Dominik |
| Route106 | Douglas |
| Route107 | Beth |
| Route108 | Jerome, Matthew |
| Route109 | Huey, Gwen, Elijah |
| Route110 | Dale |
| Route111 | Irene, Becky |
| Route112 | Carol |
| Route114 | Nancy |
| Route115 | Nob, Kyra, Helene |
| Route116 | Karen |
| Route117 | Dylan |
| Route118 | Wade, Barny |
| Route119 | Hugh, Dayton |
| Route120 | Robert, Keigo, Riley, Callie |
| Route121 | Vanessa |
| Route123 | Kindra, Jonas |
| Route124 | Spencer, Roland, Chad |
| Route125 | Nolen, Sharon |
| Route126 | Leonardo, Isobel |
| Route127 | Jonah, Aidan |
| Route128 | Wayne |
| Route133 | Franklin |
| Route134 | Hitoshi |

The exact retirement ledger, including original object records, is `data/emerald_champions/route_conversations.json`. `verify_route_conversations.py` checks the resulting playable contracts rather than imposing a permanent encounter-count quota.

## Gym coordination

All eight gyms retain six Pokémon, the same species, and the same level offsets.

| Gym | Change |
|---|---|
| Roxanne | Partner-safe Ground coverage; Larvitar can Protect to activate Guts; the documented counterplay acknowledges Carbink's Mental Herb. |
| Brawly | Fast Cobalion adds Coaching while retaining Quick Guard, complementing the physical attackers. |
| Wattson | Electrode's Discharge activates Electivire's Motor Drive; both leads can Protect, and Electivire no longer Earthquakes its partner. Suicide-oriented AI extras retire with Explosion. |
| Flannery | Chlorophyll After You into Torkoal's Eruption; Heat Wave avoids burning Lilligant; Incineroar supplies Fake Out and Parting Shot. |
| Norman | Porygon2 adds Icy Wind, Slaking uses partner-safe Ground coverage, and Mega Staraptor can Protect. |
| Winona | Altaria supports with Helping Hand instead of erasing boosts; Enamorus invests in physical Contrary attacks with special coverage; Mega Skarmory adds Iron Defense for Body Press. |
| Tate and Liza | Consistent low-Speed/bulky spreads and a second Trick Room setter; Icy Wind no longer works against the team's own speed plan. |
| Juan | Partner-safe Water attacks, Tailwind/Snarl support, Acid Spray instead of ally-resetting Haze, and Protect for Mega Gyarados. Rain is correctly described as finite. |

These changes are intended to improve tactical execution. They are not a measured claim that every matchup became harder. Full gym difficulty and alternate player solutions remain playtest questions.

## Verification and test review

- Active team materialization matches the authoring byte-for-byte.
- Reviewed legality, runtime-coherence and encounter-quality checks pass; the encounter-audit self-test rejects its 13 negative/positive controls.
- Retained trainer rosters and levels, global caps/difficulty, and numeric trainer IDs were compared against the pre-edit snapshot and remain unchanged.
- Route conversation regressions check disabled sight battles, valid dialogue, preserved coordinates/identity, remaining species showcases.
- Inclement map reference changes are restricted to the approved trainer-sight semantics on 25 maps. Visual objects, geometry and assets are unchanged. The visual special-call digest update removes four rematch queries and one registration query; no visual-special call was removed.
- Old requirements for contiguous encounter IDs and an 83–87% doubles share were removed. Active format matching, story chronology, unique IDs, legality, and showcase coverage remain checked. The checkpoint-count regression now uses a small independent fixture with gaps instead of freezing historical campaign totals.
- The native Flannery test imports generated campaign data explicitly: the standard battle harness replaces `gTrainers` with mock trainers. It now tests her actual lead species, moves, abilities, items, levels and AI routing against passive opponents. It does not simulate a complete gym match or import every authored stat investment.
- The After You turn-order test formerly depended on Stone Edge KOing a full-health Pidgeot under a particular generation's stats. Explicit faint/survival conditions now isolate the turn-order behavior it claims to test.

All 23 selected native tests passed: six dynamic AI scenarios (including the real Flannery opening), partner absorption, six After You cases, eight Coaching cases, and two exhaustive preset application checks. All 90 new dialogue lines were measured with the actual font widths; the widest is 183 pixels within the 216-pixel window.

Builds and native validation use the isolated snapshot `/private/tmp/ec-campaign-tuning-build-0905`, with the installed Arm GNU 15.2 toolchain. Shared root ROM/ELF files are not overwritten. The separate Astra Medium playthrough task owns progression testing from the preserved Mauville checkpoint. Traversal autowin remains separate from combat-difficulty evidence.

Final result: the isolated native release build passes every configured release gate, including the source-content stamp, ROM checksum and memory limits. The validated ROM, ELF, stamp and verification logs are in `release/2026-09-05-route-gym-tuning/`.
