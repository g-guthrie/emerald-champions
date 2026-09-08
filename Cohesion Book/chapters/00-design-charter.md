# 00 — Governing design and authoring contract

## Authority and scope

The user requested a separate, exhaustive implementation book before changes to the game. This book's files are the only output being authored. `baseline/source/` is an evidentiary snapshot, not a second implementation checkout. Never edit it or the live source to make a proposal appear verified.

The current game is the authored baseline. Later user instructions override snapshot AGENTS.md where they conflict, specifically the abolished opening singles exception and the universal Medium level floor. Existing teams are substantial finished content. Inspect every trainer branch individually and preserve its composition, moves, investment, items, abilities, levels, lead order and personality unless a concrete defect or an explicit cohesion objective warrants a documented revision. A new idea alone is not evidence that the existing authoring is inferior.

The final specification must distinguish source facts, design judgments, exact proposed changes, and acceptance evidence still required after implementation.

## Non-negotiable experience

1. Every campaign trainer encounter is an expert-level puzzle from the opening onward. Do not design beginner stages, delayed access to strategic concepts, or deliberately weak AI for ordinary trainers.
2. All tactical capabilities—priority, sleep, redirection, Fake Out, speed control, field control, switching and partner coordination—belong throughout the game. Every trainer's composition, moves, investment and AI should pursue the strongest coherent, clever strategy for its selected pieces. Do not simplify strategy by trainer rank, confuse prose repairs with adequate tactical review, or claim mathematical optimality from source inspection.
3. Difficulty is primarily calibrated with effective species strength, levels, and party size. Effective strength includes ability, item, form, investment and role rather than raw base-stat total alone.
4. **There are no singles trainer competitions in the proposed game.** All six opening rival branches become four-Pokémon doubles. Birch's preceding rescue is a specifically requested scripted doubles encounter in which the player selects and keeps two distinct starters. Ordinary wild and one-off legendary captures remain singles. Standard trainer teams must never have fewer than four Pokémon. Preserve explicitly authored multi-party sizes: Half layouts contribute3+3=6; Full two-trainer layouts can contribute6+6=12 (E0127). Judge the combined opponent side rather than inflating each half tofour or trimming fullparties tosix. Native legacy singles challenge paths must be converted or retired explicitly.
5. Easy/Normal/Hard retain the shared-team/shared-AI model and their existing global level calibration. **On Medium, every normal campaign opponent Pokémon must be at least the live player level cap minus two, including gym trainers.** Central contract DIFF-01 will implement this consistently; existing gym exemptions are not the proposed behavior. Preserve authored offsets and stronger levels rather than mass-editing them to simulate a shared runtime floor. This book does not silently adopt the illustrative new level bands from earlier discussion.
6. Player choice and convenient preparation are central. Preserve free nature, ability, Stat Point, legal move and held-item preparation, cap-respecting Leveler access, healing, Retry and separate Reload Save. A clever solution is allowed to win cleanly.
7. There is no rule that each Pokémon needs its own chapter, that early exotic Pokémon are inherently undesirable, or that the game should progress from weak species to strong species. Preserve good early options; justify changes against actual battle and habitat needs.
8. Preserve broad access to babies and useful unevolved/early-evolving Pokémon. Evolution requirements must be practical and clearly explained. Do not revive egg-only baby restrictions or breeding-stat grind.
9. Habitat identity and nostalgia matter. Retain recognizable residents, meaningful regional discoveries and iconic trainer Pokémon. In particular, Glacia's Walrein must return with a substantive competitive role. Audit other principal trainers for missing signature species; document exact team consequences.
10. Mega Stones remain exploration, NPC and modest berry-exchange rewards. Preserve the Ring's existing acquisition and the free evolution-item archive. No universal free Mega archive.
11. Ordinary species encounter chance remains at least 5% per method, with the existing Feebas exception separately evaluated. Geographic discovery should not become repetitive random grinding.
12. Legendary guidance at Devon is optional. Retain deliberate field landmarks, appropriate existing story/badge gates, sparse meaningful species prerequisites, retry after revisiting an area, and permanent caught/reward state. Older mandatory research and permanent failure rules are superseded.
13. Magnemite retains New Mauville's technological association, Duskull retains Mt. Pyre's identity, and Dondozo/Tatsugiri retain their underwater/submarine relationship. Habitat improvements should preserve player options and account for displaced families globally.
14. Treat every advertised supported battle-relevant species/form as an acquisition obligation. Distinguish a family appearing somewhere from the actual requested form being obtainable in a single save.

## Required entry format

Every reviewed object or system needs a traceable disposition. Use **KEEP**, **REVISE**, **REPAIR**, or **INERT/EXCLUDED**. A preserve decision must identify what was examined and why it fits; it must not be an unexamined default used to inflate completion.

- Source: exact snapshot file and label/identifier, with useful line references.
- Current: the actual existing behavior or authored loadout.
- Assessment: coherence, access, identity, strategic purpose and failure conditions.
- Final specification: the entire proposed result or an explicit KEEP of the reproduced baseline.
- Dependencies: other entries or shared rules affected.
- Acceptance: the smallest meaningful source, runtime, traversal or battle evidence required.

## Parallel ownership and integration

- Early battle reviewer owns trainer groups through encounter E0250.
- Late battle reviewer owns trainer groups after E0250, including League/postgame and explicit iconic restoration proposals.
- World reviewer owns maps, NPCs, objects, warps, connections, coordinate/background events, story handoffs and interaction prose.
- Lead editor owns wild availability, forms/evolutions, Mega/legendary rewards, shared AI, global reconciliation, source binding, final index and book validation.

Battle and world reviewers may recommend shared changes, but shared AI and acquisition rules receive one final specification under lead-editor ownership. No reviewer may modify another reviewer's output without coordination. No agent may change live game files, baseline source, ROMs, saves, installed apps, git history or external services.

## Evidence boundaries

Catalog coverage, textual analysis and graph checks are not empirical playtesting. Historical test results retain their recorded source boundary. A table of intended strategies is not proof the live AI executes them. Geometry alone does not solve all stateful movement and story reachability. Record concrete source conclusions and exact downstream verification requirements without inventing results.

Keep implementation tests focused on observable behavior and demonstrated failure modes. Do not install prose locks, fixed strategy quotas, universal species-count quotas or historical encounter-count floors in the game.

## Completion contract

The book is ready for implementation review when all in-scope inventory entries have traceable dispositions; every trainer branch has been individually examined; every proposed roster has a complete final loadout; every source-backed repair and approved nostalgia objective has an exact specification; cross-chapter conflicts are resolved; source links and coverage reconcile; and remaining implementation-time evidence is clearly listed. Completion does not claim the proposed game has been built or all battles have been won.
