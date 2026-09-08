# Small common expert AI validation set

Prepared at the implementation owner's request. This is a scoped native behavior check, not a new release gate or whole-battle certificate.

## Exact source and filter selection

Run from the canonical game-source checkout (retain the build owner's existing toolchain/BUILD_ROOT overrides):

```sh
make -j4 check 'TESTS=EC expert pair:' 'TEST_SOURCE_ALLOWLIST=test/battle/ai/ai_doubles.c test/battle/ai/ai_flag_risky.c test/battle/ai/gimmick_mega.c'
```

For a separately built ELF, select the same prefix through the established export/Makefile check flow. No broad runtime runner is required. The allowlist selects three source files; the prefix selects six declarations, totaling thirteen parameterized scenarios.

## Observable contracts and fixture quality

- Helping Hand refuses a partner with only Toxic/Protect: actual chosen Scratch command and no Helping Hand message. All HP/maxHP and Speed values are explicit; no internal heuristic score assertion.
- Helping Hand works from either flank: Oranguru/Telepathy has only negligible Mud Slap damage; its Politoed partner has Surf. The two targets have 80 HP/100 SpDefense. At Level50/SpAttack150, ordinary Surf cannot KO either target, while Helping Hand's native 1.5x power boost crosses both thresholds. Actual helper target, Surf command, messages and both fainting HP bars are checked. Explicit independent speeds prevent TURN declaration order from defining the tactic.
- Beat Up/Justified works from either flank without Protect blocking it: Pangoro's Attack20 keeps ally damage small, Growlithe's Attack180 makes the two justified stages valuable, and the activator's Speed300 precedes the recipient's Speed200. Actual target/recipient attack and native +2 Attack are checked. Mold Breaker, absent Justified and 1 HP recipient controls reject ally activation. Only two eligible party members exist, so the expected two native Justified activations are explicit.
- Two Mega-eligible teammates share one owner: Mawile's Huge Power improves its physical Iron Head; Venusaur's base special attack remains useful. Both hold their legal stones. The actual selected commands must carry exactly Mawile's Mega and Venusaur's no-Mega, and the animation/final species prove the committed result. Every HP/maxHP, relevant Attack/SpAttack/Defense and Speed is specified.
- Risky/Conservative personality flags do not sacrifice a guaranteed accurate KO: Thunderbolt and Thunder both exceed 20 HP; Thunderbolt is selected under each of the three flag variants. All actors use the complete common expert profile.
- Candidate evaluation restores native state: after a real AI-controlled opening turn, a single native transaction case evaluates twice and compares the same result plus BattleMons, AI cache, protections, special statuses, battle state, field, weather, move power, item-effect battler and both RNG states. A fresh resist Berry, partially injured Water Absorb target, Helping Hand and Trick Room make the counterfactual exercise item/healing/field effects. The snapshot lives on the heap rather than the GBA's small test stack.

The transaction case is the only new declaration. It is necessary because the existing search found no native AI fixture exercising the new paired evaluator's board/cache/field/RNG boundary. Existing tests only covered isolated move-cache reuse or ordinary selected commands.

## Retired expectations

Removed four old risky tests requiring blind Counter/Mirror Coat/Revenge or personality-biased switching, and replaced the inaccurate-KO preference case with the common-profile reliable-KO contract. Removed three hard-ace exclusion tests that required withholding the best reserve. Existing positive no-other-reserve cases remain, outside this small allowlist/filter run.

## Evidence so far

The selected source files passed ARM15 production-preprocessor/cc1 syntax checking with TESTING=1 and warnings as errors. They have not yet been run in the native emulator. Any failure must first be classified as a contract, fixture, configuration, artifact freshness or implementation issue; do not loosen a valid assertion merely to pass.
