# Marc / Stonjourner Stealth Rock — playtest diagnosis

## Repair and proactive follow-up

The original diagnosis below is historical. Current code now routes
TARGET_OPPONENTS_FIELD to a living foe, not the actor. The only four native
move-table consumers are Stealth Rock, Spikes, Toxic Spikes and Sticky Web;
all four execution commands use the selected target's side. Correct the stale
enum comment that incorrectly described Teatime here. Teatime actually uses
all-battler iteration; Court Change explicitly swaps sides. The adjacent
user/field/all-battler target categories were not blindly reassigned.

One shared `AI_IsHazardAtCapacity` owner now supplies hazard capacity to bad-move
rejection and setup rewards. Pure status hazards at capacity score zero rather
than a small penalty that later bonuses can outweigh. Stone Axe and Ceaseless
Edge retain their damaging actions without an already-filled hazard bonus.

The proactive native check found another real defect: two allies both chose
Stealth Rock on an empty side, consuming two PP for only one successful cast.
The existing pair-setup duplication rule now rejects redundant Rock/Web pairs
and two Spikes/Toxic Spikes casts when fewer than two layers remain. Two useful
layers are still allowed. This is bounded scoring, not additional search.

Verified `work/hazard-targeting-final.log`: four shared groups/16 cases pass:
all four hazards on both actor flanks, four two-setter cases, Magic Bounce
avoidance on either foe flank, and continued damaging Stone Axe/Ceaseless Edge
after capacity. These are four retained regressions for the demonstrated
failure modes and adjacent exceptions, not a per-trainer test corpus.
Some early scratch scaffolding failed because of missing explicit fixture Speed,
bit-field assertion typing and incorrect parameter enumeration; these were
fixture defects, not gameplay failures. A Normal Smeargle also legitimately
preferred STAB Tackle to an unboosted Stone Axe: the damage-preservation control
now uses actual Sharpness/STAB users instead of changing gameplay for that test.

`work/hazard-shared-and-marc-final.log`:52/52 groups pass, including the actual
Marc three-turn reproduction and complete timing. Marc's corrected sequence:
turn1 Veil/Stealth Rock on the PLAYER side, turn2 Blizzard/High Horsepower,
turn3 Blizzard/Rock Slide. Stealth Rock PP stays19 after its single cast,
opponent-side rocks stay absent, and the saturated move score is zero.
Complete cold decision39GBA frames (~0.65seconds), warm33, matching the earlier
Marc timing sample. After removing both temporary Marc includes,
`work/ai-shared-hazard-clean.log` passes50/50 retained groups.

The adjacent target categories were inspected, not exhaustively simulated.
Magic Bounce checks demonstrate AI avoidance, not every possible reflection
interaction. A forced/only-available move can still fail; saturated hazards
are not banned from the game's move system. No claim of perfect removal,
interruption, or simultaneous opposing-side prediction in every position.

New full-ROM build `emerald-champions-hazards-20260909-2058` passes release
verification and is copied to `playtests/20260909-2058-hazards/` and Downloads.
ROM hash0ade80ebe0ec0e671de4d83f5eb71a4ec9778f03a76b2f15bbc678c1e4131d33;
used ROM27,901,672bytes,224 fewer than1848 after all intervening authoring.
EWRAM/IWRAM unchanged. Original1848 download remains preserved. Marc's9/10
recheck and Ned's8.5/10 completion are in main chat at2026-09-10 02:01:43.411 UTC,
P:29871; ledger44/516 branches,31/468 encounter records. E0034 Elliot is next.

## Original diagnosis

User reports the Stonehenge-looking opponent immediately before Roxanne
repeatedly attempting unsuccessful Stealth Rock in the frozen1848 playtest.
Actual canonical match: Marc's Stonjourner, which knows Rock Slide,
High Horsepower, Wide Guard and Stealth Rock. Pause sequential authoring;
Ned's otherwise verified work remains pending main-chat completion.

## Reproduced defect

`work/marc-hazard-reproduction.log` executes current native Marc with six native
level14 player members. The two active player members attempt Protect, while
four available reserves make hazards relevant. This is a diagnostic board,
not a reconstruction of the user's exact party or an optimal-play test.

- Turn1: Aurora Veil / Stealth Rock. Opponent-side rocks1, player-side rocks0;
  Stonjourner's Stealth Rock PP19.
- Turn2: Blizzard / Stealth Rock. Rocks still on opponent side only; PP18.
  The second cast fails because the wrong side already has rocks.
- Turn3: Blizzard / Rock Slide. The reproduction proves a repeated failed cast,
  not that every board repeats indefinitely. Its Stealth Rock score against
  the player is still112 because that side remains empty.

The first scratch attempt had an incomplete copied declaration and failed to
compile; it yielded no gameplay evidence. That fixture-only error was corrected.
The successful three-snapshot diagnostic is read-only with respect to gameplay
code. Its scratch include is removed; no production repair or new ROM yet.

## Exact ownership mismatch

`PairTargetIsLegal` in `src/battle_ai_pair.c` groups TARGET_OPPONENTS_FIELD with
TARGET_USER/TARGET_FIELD and accepts only `target == actor`. `BuildPairActions`
scores that action against the opposing battler, but final action selection
stores the original self target in `gAiBattleData->chosenTarget`.
Stealth Rock's native table explicitly uses TARGET_OPPONENTS_FIELD.
`Cmd_setstealthrock` in `src/battle_script_commands.c` uses
`GetBattlerSide(gBattlerTarget)` and fails if that side already has rocks.
The evaluator and execution therefore disagree about which side is affected.

The frozen1848 ELF independently contains this defect, not just later source:
`PairTargetIsLegal.lto_priv.0` is at0x08018c08. Its switch uses target-type minus4;
the table at0x08a61e4c has entry9 (TARGET_OPPONENTS_FIELD=13) pointing to0x08018cce,
which compares the target register with the actor register. The same build's
`Cmd_setstealthrock.lto_priv.0` is at0x080d6dc0. The preserved artifact is bound
to ROM SHA256098d3a3fe471ae5c917d6350798d797bfa24a7c27e4e66d2db63ab4088d1c94c.

There is a second policy weakness to inspect during repair: existing rocks
receive only a small bad-move score penalty, while `AI_ShouldSetUpHazards`
does not itself check existing hazard capacity before other scoring layers
reward setup. Correct targeting must be accompanied by a focused check that
an already-established hazard is not repeatedly attempted. Preserve valid
Spikes/Toxic Spikes layers and the damage of Stone Axe/Ceaseless Edge.

The proper repair target is shared opponent-field targeting and saturated
hazard eligibility, not deleting Stonjourner's move or adding a trainer-only
exception. Inspect other consumers of TARGET_OPPONENTS_FIELD and exercise
correct side placement, no failed repeat, valid layering and reflection as
appropriate before declaring it resolved. No unbounded search is needed.

## Coverage correction

Earlier Marc main-strategy scenarios had only two active player Pokémon and
no reserves; hazard incentives were consequently not exercised. Their PASS
did not certify this behavior. Reopen Marc and revise the ledger from43 to42
closed branches (30 to29 encounter records). Do not erase historical ratings,
pretend the prior review caught the problem, or infer all hazards are correct
from a generic shared-suite pass. No full campaign reaudit gate is added.

Frozen Downloads ROM is unchanged. The current user request is to inspect the
code; this checkpoint diagnoses the failure without claiming a delivered fix.
