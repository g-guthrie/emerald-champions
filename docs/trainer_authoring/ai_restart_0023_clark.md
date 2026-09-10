# E0023 Clark — executable-AI restart

Historical flat-stat evidence below. Current final-Gen9 completion and native
numbers are in [the dated review](e0023_clark_gen9_2026_09_09.md); the old
stat tables and tactical results are not current-stat proof.

Status: bounded native review complete, **8.5/10 intention rating**. Clark is one completed trainer branch; Johnson remains pending. E0023's grouped catalogue record is therefore not complete. Cumulative restart coverage after the main-chat announcement: **26/516 branches, 20/468 fully completed catalogue records**, plus this completed half of E0023. No release ROM or full-playthrough claim.

## Actual encounter, not catalogue prose

Route116's Clark and Johnson scripts independently call `trainerbattle_double` at scripts.inc:282 and :340. A double encounter sets `numTrainers=2` in trainer_see.c:584–589, causing the approach scan to stop at :487–488 after collecting that one trainer. Thus even overlapping sight lines do not combine these two current doubles parties. Direct interaction also names only the selected trainer. The master requirement is corrected from “optional native pair”; identifiers stay stable. Johnson's source-only review does not certify his current AI.

## Accepted team and executable changes

All four species debut in the current authored campaign here. Their roles are distinct; no species, item, level, Nature, ability or Stat Points change is justified by the sampled evidence. All 16 moves and native abilities were reviewed against preparation/species data, not historical learnset assumptions. Archen's now-different utility slot also reduces exact set repetition with its later appearance.

| Native Normal, zero badges | HP / Atk / Def / SpA / SpD / Speed | Accepted role |
|---|---|---|
| Roggenrola, Lv12, Impish, Eviolite, Sturdy | 70 / 26 / 67 / 12 / 16 / 12 | Wide Guard, Rock Slide, Iron Defense, Body Press; 32 HP / 32 Def / 2 SpD points |
| Geodude, Lv12, Adamant, Life Orb, Rock Head | 37 / 64 / 32 / 13 / 15 / 45 | Rock Tomb, Double-Edge, Stomping Tantrum, Protect; 2 HP / 32 Atk / 32 Speed |
| Mudbray, Lv12, Adamant, Sitrus, Stamina | 74 / 70 / 27 / 17 / 21 / 19 | High Horsepower, Rock Tomb, Heavy Slam, Protect; 32 HP / 32 Atk / 2 Def |
| Archen, Lv12, Jolly, no item, Defeatist | 40 / 67 / 19 / 23 / 19 / 62 | Rock Slide, Acrobatics, **Protect replaces Dragon Claw**, Knock Off; 2 HP / 32 Atk / 32 Speed |

All IVs remain31; each spread totals66 points. Stored levels13/13/14/14 become12 under the current Normal cap/reduction logic. Native Easy/Hard lookup-fallback observations produced10/14 respectively; these are calibration checks, not difficulty balance verdicts.

The executable plan is **SETUP | PRESSURE**, retaining Conservative and the standard expert flags. Pressure changes the existing active-HP valuation, not the player's information or a forced move sequence. It was accepted only after comparing the actual reserve guards and follow-through. Archen keeps itemless110-power Acrobatics and gains a way to survive an opposing action while Mudbray attacks. Dragon Claw was largely overlapping neutral coverage, not an essential Dragon answer.

Shared fixes in battle_ai_pair.c and battle_ai_util.c:

1. **Rock Tomb** now uses the existing native-eligibility/accuracy Speed-drop forecast. It respects Substitute, secondary-effect denial, native stat protection, Contrary/White Herb and action ordering through that existing path. No new speed matrix.
2. **Iron Defense** uses the existing immediate self-Defense cache and action-time mitigation. It does not retroactively reduce faster damage. Native adjusted stages, caps, Snatch, paralysis and White Herb follow the already supported setup path; Body Press already consumes modeled Defense offensively.
3. **Stamina** raises modeled Defense after a surviving single damaging hit, physical or special, before a later action. It excludes Substitute and miss-only survival. Effective Stamina is not incorrectly suppressed by Mold Breaker; Shield Dust/Cloak are not Stamina immunities. Existing stages/modifiers and bounded effect pass are reused.
4. **Itemless Defeatist** uses two native-rounded offensive damage anchors selected at action-time HP. Existing disjoint conditional-boost storage is reused—no new matrix or per-candidate native damage calls. The helper restores HP and cached HP percentage; the native calculation restores its scratch/RNG. HP-powered attacks, Flail, Final Gambit, Endeavor, unsupported multi-hit and Max/Z contexts are excluded.

Rock Head stops Double-Edge's move recoil, **not Life Orb's payment**. Body Press uses Defense/stages but does not treat Eviolite's defensive modifier as an offensive bonus. Defeatist applies at **half HP or less**, not only below half.

## Actual native decisions and comparison evidence

Player sets were native-generated atLv14 with current species stats and legal early preparation. No trainer command was forced; player commands were not fed to the evaluator. Promoted-reserve scenarios explicitly fainted the original leads and moved the genuine reserves into active positions. Injury controls are constructed states, not fabricated claims of an observed entry sequence.

| Board | Observed accepted outcome |
|---|---|
| Eevee/Timburr physical lead pressure | Body Press + Geodude Protect first; second turn Body Press/Rock Tomb removes Eevee. Roggenrola then uses Iron Defense while low-HP Geodude guards. |
| Pachirisu/Lotad single-target lead pressure | Iron Defense/Protect first. **Rock Tomb lowers Lotad17→11, letting Roggenrola12 land boosted Body Press before Giga Drain on turn2.** Both leads survive18/34 HP; before the Rock Tomb fix, Geodude fainted and Lotad survived1 HP. The accepted pressure plan then voluntarily pivots Roggenrola to Mudbray on turn3 while Geodude attacks. |
| Mienfoo + Muddy Water Lotad | First turn leaves Roggenrola16 HP after setup; on turn2 Rock Tomb/Body Press removes Lotad before its spread attack. Before the repair both leads fell on turn2. |
| Lotad/Shellos double Muddy Water | Wide Guard blocks both spreads; Geodude initially guards, then Double-Edge attacks behind repeated Wide Guard. Geodude37→34 is Life Orb payment, not Rock Head failure. |
| Mienfoo Fake Out/Taunt + single-target Giga Drain | Fast Fake Out interrupts Roggenrola; Taunt subsequently prevents its chosen setup and Geodude falls, bringing Archen in. This remains useful counterplay, not an immunity to delete. |
| Healthy Mudbray/Archen vs Eevee/Timburr | Heavy Slam/Acrobatics removes Timburr on turn1; High Horsepower/Acrobatics finishes Eevee on turn2. Mudbray finishes31 HP and +3 Defense; Sitrus was consumed. |
| Reserves vs Pachirisu/Lotad | Acrobatics removes Lotad turn1, Archen ends4 HP. Accepted Protect + pressure plan lets Mudbray attack on turn2 instead of the candidate's former double Protect. On turn3 Archen's repeated Protect fails, but Mudbray finishes Pachirisu; this is a win with a remaining preservation weakness, not flawless play. |
| Half-HP Archen vs physical pair | Protect first, then Acrobatics while Mudbray attacks. Eevee falls turn2 and Timburr turn3; Mudbray15 HP/+5 Defense and Archen20 HP remain. |
| Faster Mienfoo/Pachirisu focus on Archen | Protect blocks opening Fake Out/Thunderbolt. Turn2 Acrobatics + High Horsepower removes Mienfoo; both reserves survive58/4 HP. Without Protect, Archen fainted before contributing and Mienfoo remained72 HP. Turn3 Protect lets Mudbray keep attacking Pachirisu. |

The pressure-plan comparison reduced the special reserve's turn2 double guard to High Horsepower/Protect. Retaining just Protect without pressure left Pachirisu45 HP; accepted pressure leaves32. Healthy physical execution stayed intact. Original Dragon Claw was retained in a counterfactual scratch comparison only, then the accepted move was materialized in the actual trainer input and regenerated header.

## Distinct regressions, audited with negative probes

| Isolated generic boundary | Native/fixed result | Disabled-forecast negative |
|---|---|---|
| Stamina between partner chip and follow-up | Quick Attack deals2. Waterfall deals24 against Own Tempo but14 after Stamina; Ice Beam deals20 after Stamina. AI now chooses Waterfall for Own Tempo and Ice Beam for Stamina. | Disable only the new Stamina application: selects Waterfall and fails the decision assertion. |
| Rock Tomb ordering / Covert Cloak | Smeargle100 slows Tauros80 below Oranguru60: Rock Tomb/Psychic gets the KO and preserves partner. Cloak control chooses Strength/Protect. | Remove only Rock Tomb from the Speed cache: Strength replaces Rock Tomb and assertion fails. |
| Iron Defense timing | Slow physical opponent: Iron Defense leaves30/70 HP at+2 Defense. Faster otherwise identical opponent: Body Press,12 HP, no boost. | Remove only Iron Defense from its cache: slow case chooses Body Press and fails. |
| Defeatist after priority chip | Native11 damage takes60/100→49; Acrobatics falls from21 to10 and no longer KOs20-HP target. AI instead removes the priority threat with Quick Attack. Already-injured40-HP control also uses Quick Attack; healthy100-HP control retains Acrobatics KO at89 HP. | Disable only Defeatist anchors:60-HP case incorrectly uses Acrobatics, fails KO/decision expectation. |

The Stamina passive partner was changed from Pachirisu to a Ghost in the isolated diagnostic: otherwise the repaired AI could avoid triggering Stamina by redirecting its chip, obscuring the intended follow-up boundary. That is not a Clark loadout edit. The retained tests are four compact **generic shared-mechanic** groups, with no authored trainer locks. Forced native controls and actual-team diagnostics were not retained in the build.

Final accepted coverage: **29 actual-party parameters** (25 battle observations including seven transition follow-ups, plus four timing/calibration parameters), **15 temporary generic parameters**, and **38 clean shared groups**. Final scopes were run separately: the initial three-turn healthy-reserve fixture incorrectly requested a third turn after both player Pokémon had already fainted on turn2. That impossible extra parameter was removed, not “fixed” by altering gameplay, and the remaining seven transition parameters passed. No native game freeze was inferred from this fixture failure.

Warm/rebuilt paired queries with four and six distinct native player members were **31/37 GBA frames**, approximately0.52/0.62 seconds. Initial samples were28/34 frames. These are sampled decision costs, not a whole-game worst-case guarantee. The new storage reuses existing matrices; the extra Stamina effect pass can still cost time.

Canonical/master/party projection verified all516 branches; this is synchronization evidence, not review credit for those branches. All Clark scratch includes were removed, shared38 passed on the clean build, and diff whitespace checks passed. No commit/publication/release build was made.

## Remaining opportunities

**Improve switching and preservation against single-target Water/Grass pressure, and avoid repeated Protect when Archen is already in Defeatist range.** More exact threshold probabilities and lower worst-case latency would also help.

Stamina's within-multihit Defense sequence is still outside this bounded forecast. Stage-to-damage modifiers are approximate. Defeatist endpoints are native-rounded, but endpoint selection uses conditional-alive aggregate HP, not an exact distribution across damage rolls, misses and healing branches that straddle half HP. Item-bearing Defeatist is not newly modeled. Existing unrelated item-loss/other-mechanic approximations remain; these changes do not certify them.

The physical/guard/Speed/fast-reserve partnership is now demonstrated and supports the8.5 judgment. Single-target special pressure, faster denial and risky repeated guards keep it below10. Continue with **Johnson separately**, then close the grouped E0023 record only after his own review and main-chat confirmation.
