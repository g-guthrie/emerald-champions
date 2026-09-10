# Chapter 2 dialogue audit: Petalburg through the Stone Badge

September 10, 2026. Continues the opening review in `2026-09-10-opening-dialogue.md`.

## Scope and conclusion

Reviewed the executable dialogue, NPC scripts, map object/coordinate events and relevant C consumers along Route 102, Petalburg, Route 104, Petalburg Woods and Rustboro through Roxanne's first badge. Included accessible houses, shops, Center staff, trainer conversations, signs, optional services, and repeat/decline branches. Also read the initial Route 116/rest-house/tunnel-side conversations for players exploring east before the badge, and the immediate post-badge stolen-goods scene as the next chapter's handoff.

The chapter's source and targeted native checks are in good shape after the fixes below. This is not an exhaustive fresh-save playthrough, battle balance certification, or whole-game sign-off. Existing trainer authoring and battle difficulty were preserved.

## Findings fixed

1. **Scott's unclear first appearance.** His old interruption judged the player's clean clothes, questioned whether they were a trainer, then apologized for taking their time. It is Scott's introduction as a scout for talented trainers, leading into school and later campaign appearances. He now gives his name, describes his interest in how partners work together, and points to the Rustboro school as the next meeting. His first post-badge school greeting now follows that introduction rather than introducing himself again. Encounter state and all four exit paths are unchanged.
2. **Wally's parents knew the future.** Both house NPCs could thank a first-time visitor for a catch that had not happened. Before Petalburg Gym state 2 they now explain that Wally has gone to ask Norman for help. They only discuss his catch after the tutorial. The thanks message now refers directly to helping him catch his first Pokémon.
3. **Norman's welcome lagged behind the story.** He now welcomes the player as someone whose journey has already begun, acknowledges the partners and gives parental encouragement, rather than predicting that the player will become a trainer later.
4. **Premature Gym room instructions.** The Petalburg Gym guide previously explained challenge rooms while they were still closed. Early visits now explain Norman's four-badge requirement and invite the player to speak with him at the entrance. Challenge-room guidance remains for the actual Gym challenge.
5. **Outdated capture/storage advice.** Petalburg's gentleman said a seventh catch automatically entered storage. `Cmd_givecaughtmon` and `B_CATCH_SWAP_INTO_PARTY` actually offer a party swap. His dialogue now describes both choices.
6. **Shaymin sighting after capture.** Route 102's boy now recognizes the caught Shaymin instead of continuing to advertise its 3% encounter rate after it has left the wild pool.
7. **Unhelpful gender explanation.** The Rustboro Center no longer says nobody knows how male/female Pokémon differ. It gives the concrete Burmy evolution example, checked against the species evolution data.
8. **Pacing and layout polish.** Tightened Wally's request and catching dialogue, the Devon researcher's Shroomish detour, the Woods preparation sign, the reflection NPC and the flower-shop learning prompts. Kept the researcher distracted by Shroomish so his later “no more looking today” exit still pays off.

## NPC coverage ledger

“Reviewed” here means source review, not an individual emulator recording for each NPC.

| Area | NPCs and branches reviewed |
|---|---|
| Route 102 | Grass-height boy; Shaymin boy before/after capture; Calvin, Rick, Allen, Tiana, their introduction/defeat/repeat text and Match Call gates; both route signs and berries. |
| Petalburg exterior | Gym escort boy and approach paths; Scott's four paths; water-reflection boy; storage gentleman; woman waiting for Wally; town/Gym/house signs. Later Surf pickups are outside this chapter. |
| Petalburg houses | Adventurous couple; Norman's neighbor and former challenger; Wally's father and mother before/after the catch. Later Surf and Victory Road branches remain for their chapters. |
| Petalburg Gym | Norman's first meeting, Wally's arrival/loan/catch/return/departure, Norman's zero- and one-badge advice; entrance guide and statues. Later room trainers are not accessible before four badges. |
| Petalburg Mart | Clerk and opening inventory, love/growth woman, Repel boy and Antidote man. Poison advice matches the no-overworld-damage rule. |
| Petalburg Center | Nurse/shared services, storage man, Oran Berry youngster, starter-type woman, held-item vendor, move specialist and local legendary guide; upstairs attendants use the shared dialogue reviewed in Chapter 1. Starter advice traces both selected starters through `IsStarterInParty`, including evolution roots. |
| Route 104 | Cottage/sea/catching/theft/walking NPCs; Leaf Stone boy; Gina and Mia; Ivan, Billy, Haley, Winston, Cindy and Darian; gifts/repeats, battle-party rejection and Match Call gates; signs and berry plots. Florist sapling/decorations state is later. Rival and sailing scenes are gated until the Devon chapter. |
| Briney's cottage | Confirmed Briney and Peeko are hidden before the rescue; their thank-you/sailing dialogue is not a pre-badge conversation. |
| Flower shop | Owner's first/repeat/yes/no berry explanations; Wailmer Pail girl; Venusaurite/daily-berry girl and repeat dialogue. Decoration sales are a later visit. |
| Petalburg Woods | Devon researcher and Aqua grunt, both approach variants and post-battle exit; Lyle and James; grass-advice boy; Burmy/Dartrix boy; preparation and deeper-forest signs; Tart Apple girl reachable after Cut. Read the deeper areas' ranger/sign text as a post-badge side-trip boundary; no deeper-area playthrough claimed. |
| Rustboro exterior | Fire-type challenger, Devon explanation man, school employee, badge admirer, Briney boy, young explorers, evolution pair, trade/obedience man, doubles boy and outdoor Roxanne; signs. Checked stolen-goods scene and request point east to Route 116 as the next story objective. |
| Rustboro houses/apartments | Trade NPC and hiker; school-admiring family; nickname couple and Pikachu cry; Ability couple; Walda's parents, phrase input/decline/cancel/repeat and wallpaper responses; Devon resident and Skitty cry; old man, Premier Ball child, Metal Coat giver and replacement hiker; president/son stone collectors. |
| Cutter's house | Cutter gift/repeat explanation and daughter. HM ownership, Stone Badge and compatible-party use agree with the field-move behavior; Cut need not occupy a battle slot. |
| Rustboro Mart/Center | Mart clerk/stock, False Swipe NPC and three shoppers; nurse, Nature man, gender boy, Cut girl, specialized Ball clerk, held-item vendor, move specialist and local legendary guide; upstairs shared attendants. The Center Ball clerk sells a distinct specialized list. |
| Rustboro school | Teacher's Sun Stone, classroom walk paths, students trading/holding items/confusion/berries, notebook, all five blackboard topics and cancel, Scott before/after badge and repeats. |
| Devon 1F | Greeter, employee, stair guard and displays before/after theft. Upper floors are blocked until goods are returned, so upper-floor story/service review belongs to the next chapter. |
| Rustboro Gym | Guide approach/gift/advice/repeat; Josh, Tommy, Marc and Roxanne; statues; badge/rewards and onward directions. Advice about Carbink's Trick Room, Nosepass's Wide Guard, Josh's Sturdy, Tommy's Storm Drain and Marc's Aurora Veil matches current compiled teams. |
| Optional Route 116 preview | Initial digger, stone-search NPC and local Karen conversation; trainer introduction/defeat/repeat text; three rest-house workers and initial tunnel blockage. The post-badge Peeko/goods rescue, Devon return and travel arrangements are the next full chapter. |

## Verification and limits

- Source font scan: 540 blocks / 2,042 nonempty lines in the regional/shared **superset**, including unused and later dialogue. No measured line exceeded the standard 216-pixel dialogue window; no extracted segment contained multiple unscrolled newlines. Seven-character player names, fifteen-character Walda phrases and relevant species substitutions were included. Unresolved fossil/Eevee substitutions belong to blocked Devon upper floors and remain for the next chapter. This is not a guarantee for every dynamic message in the ROM.
- Native mGBA: Scott completed from all four approach rows; encounter flag advanced to 1 and field/script controls unlocked.
- Native mGBA: Wally's father rendered the new pre-catch response at state 0 and the thanks response at state 2, with a seven-character player name. Both conversations returned control normally. The mother's shared pre-catch branch was source-reviewed.
- Native mGBA: Norman's first meeting, Wally's unautomated catching tutorial and return to the Gym completed at Gym state 2 with controls unlocked. This is scene progression evidence, not a trainer battle balance test.
- Native mGBA: Roxanne victory handoff passed the existing runtime assertions for first badge, both reward flags/items, defeated trainer, level cap 20, Rustboro state 1 and unlocked controls. Combat was automatically resolved for this progression fixture.
- The original fixed-frame Roxanne renderer sampled a reopened repeat conversation and reported failure. Re-running with the same assertions and stopping on successful observation passed at frame 2,662; no gameplay or assertion was relaxed. Likewise, Scott's first probe only turned the player; a normal walking input exercised the trigger on the subsequent run.
- Visual inspection: Scott introduction, Norman welcome, Wally-family before/after text, post-tutorial Gym and Roxanne send-off. Not every NPC page was individually rendered.
- The final late-added early-Gym-guide branch was source/font reviewed and included in the final release build; the earlier headless screenshots precede that addition.
- Release input/artifact identity, ROM header/checksum and memory gates: `work/dialogue-badge1-20260910/verification.log`.

Evidence, source snapshots and temporary diagnostics are under `work/dialogue-badge1-20260910/`. No generic permanent text-test framework was added. The existing story-handoff fixture was extended for the specific scenes above.

## Next full chapter

Start at the theft on leaving Rustboro Gym: Route 116, Rusturf Tunnel and Peeko, returning the Devon Goods, Devon's accessible floors/services, Mr. Stone and Match Call, the rival's optional rematch, Briney's sailing and Dewford through Brawly. Revisit hometown callback dialogue when its state changes. The prior opening release remains a separate preserved artifact; the new first-badge ROM includes the opening fixes.
