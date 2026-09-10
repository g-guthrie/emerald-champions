# Devon and Dewford chapter review

Chapter pass completed at source-review and scoped-native-verification level. This does not mean every optional choice, rematch, gift-storage outcome, or multiplayer interaction was played through. The full-game goal remains active.

## Scope

Post-Roxanne Route 116/Rusturf rescue, Devon's three floors, Briney's cottage and outbound sailing, Dewford town/Center/houses/Hall/Meadow/Manor, Granite Cave, Route 106's local conversations, the Brawly-in-Slateport dependency, Dewford Gym and Steven's handoff. Later Surf-only route encounters and shared Cable Club services remain separate full-game scope; their early-map entry points were identified here.

Source review traced mandatory gates, actor scripts, reward state, repeat conversations and current legendary-state owners. Current C trainer parties match the named species/Ability/item lessons in the Gym and Rusturf grunt dialogue. This is dialogue/mechanics consistency review, not tactical AI certification.

## Changes

- Steven now acknowledges Cobalion already caught.
- Meadow warden explains caught residents leaving the encounter pool and has a completed-Meloetta response.
- Early HM explanations explicitly distinguish automatic Flash lighting.
- Fossil scientist tells players how the short wait actually completes: another researcher or re-entering the floor.
- Hall no longer assumes the trendy phrase names a move.
- Five Hall lines that overflowed with long word pairs were rewritten. The longest measured two-token trend is 165 pixels; revised substitutions fit within 216 pixels.

## Native evidence

Existing story-handoff fixtures were extended with bounded starts. Native engine scripts and movement ran; trainer battles use existing traversal automation.

- Cobalion uncaught/caught: distinct Steven farewells, departure flag set, controls released.
- Warden without Sing, with Sing, and after Meloetta capture: appropriate research/song/completion dialogue and controls released.
- Rusturf rescue: goods recovered, native Peeko/Briney reunion, controls released.
- First sailing: Dewford arrival observed, followed by a return trip caused by continued test input; no movement lock.
- Brawly: badge awarded and controls released. Initial repeated A input reopened the NPC dialogue; B dismissal verified the actual ending.
- Devon president: escorted scene reaches state 1 with no active script or movement lock.
- Fossil pickup: seeded Old Amber revival moves from pending through ready/receipt to state 0; party count increases from 2 to 3 and controls release. Initial fossil selection/consumption is source-reviewed, not exercised by this pickup scenario.

Evidence is under `work/dialogue-dewford-20260910/`, particularly `native-discoveries.log`, `native-brawly-finish.log`, `native-handoffs-badge-boat.log`, `native-devon.log`, and `native-fossil-receipt.log`, with scene screenshots.

## Layout and delivery

Separate source scans cover Devon (62 blocks), Rusturf/Route116/cottage/Gym (79 blocks), and Dewford/Granite Cave (124 blocks; overlap is intentional). Dynamic Hall substitutions were checked separately rather than counted as resolved empty variables. No measured final line exceeds 216 pixels. These scans supplement scoped screenshots; they are not screenshots of every page.

Latest gameplay release: `pokeemerald-dewford-audit-20260910-release.gba`. Its build/input stamp/release gates passed after the Hall fixes. Subsequent edits in this continuation only extend headless fixtures and audit records. No fresh production gameplay changes were made after that delivery.

Full-game follow-up: shared multiplayer services, broader field/encounter traversal, future badge/revisit states, Surf-only coastal routes, and final integrated verification. Preserve these limitations in the final campaign report.
