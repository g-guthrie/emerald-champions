# AI audit handoff

Written at the end of the session that landed the Mega, switch-entry, empty-guard
and blocked-activation fixes. Everything below is either measured (stated as
such) or flagged unverified at the end.

Full history: `work/ai-uncertainty-20260915/Report.md`.

---

## 1. Y(3): HP-scaled power is fed a blend that includes non-acting branches

**Measured, on Wallace E0497 seed 1, cap 96, at 322/322 before any command:**

```
slot 0  Water Spout   484      <- chosen? no
slot 1  Ice Beam      250
slot 2  Origin Pulse  355      <- chosen
slot 3  Protect         0
```

Order confirmed from the turn-1 state JSON, not assumed. Water Spout is 36%
ahead and 100% accurate against Origin Pulse's 85%. The cached damage model is
therefore correct; the reversal is downstream.

**The two lines**, `src/battle_ai_pair.c`:

```c
3665:   u32 actionHp = hp[actor];
3990:   damage = PairHpPowerDamage(ev, actor, target, action->index, move, actionHp);
```

`hp[actor]` is a probability-weighted blend over the joint forecast. This board's
forecast contains Mega Kangaskhan's **Fake Out** — priority plus flinch. In the
branches where it lands, Kyogre is chipped *and never acts*. Those branches drag
the expected HP down, which drags Water Spout's power down, in branches where
Water Spout does not happen.

**Correct HP for an HP-scaled move:** the HP the user has *conditional on acting*
— i.e. after damage from attackers that both move before it and leave it able to
act, excluding branches where it is flinched, fully incapacitated, or dead.

**Interaction with `actionChance`:** the not-acting branches are already priced
there. Today they are counted twice — once as "you probably will not act" and
again as "and you will be weaker when you do". The fix must remove the second,
not the first. Do not reduce `actionChance` to compensate.

**Interaction with group C's conditional HP:** there is already conditional-HP
machinery for "branches where an earlier lethal move missed" (see the comments
around the weather/Tailwind/Trick Room guard in the same loop). Reuse that shape
rather than inventing a second notion of conditional HP; two different
conditionals in one trial will not stay consistent.

**Fixtures I expect to move**, and each must be re-derived rather than retargeted
by reflex:

- `test/battle/ai/power_pricing.c` — Water Spout at full health; max-power
  Reversal at 5 HP (Reversal is the same effect family, inverted).
- `test/battle/ai/joint_conflicts.c` — "the HP-scaled spread wins at full
  health" (already re-pointed at a board that fights back; it passes now and
  should keep passing).
- `test/battle/ai/setup_pricing.c` — the Endeavor low-HP case, if Endeavor
  shares the effect path.

Prediction this makes, worth checking as acceptance: Water Spout skipped at full
health *only* when a priority flincher is on the field, taken otherwise. That is
exactly the build-13 vs build-16 difference in the receipts.

---

## 2. Y(4): Kyogre turn-2 guard — short job, trace already exists

Confirmed defect: same turn, same board, builds 9, 13, 16. `738e5a8809` did not
reach it.

**Where the margin is reported:** `gAiGuardTrace[MAX_BATTLERS_COUNT]` in
`src/battle_ai_pair.c`, declared in `include/battle_ai_util.h`.

```
bit 0        guard was the chosen action
bits 16..27  margin = winning board score - best board where that body attacked
```

Margin **4095** is a sentinel meaning *no attacking board was scored at all* —
that is the `738e5a8809` defect, now fixed, and it should no longer appear.
A small margin (tens) is a guard that barely won: suspicious. A large margin
(100+) is a guard that dominated: on Wallace turn 4 both guards read 111 and 152
and are correct.

**How to read it:** build an instrumented ROM (§6 of `docs/VERIFICATION.md`),
then use `scratchpad/replay.py` — it already samples `gAiGuardTrace` for all four
battlers between turns. The Wallace line is in
`work/playtest/328-TRAINER_WALLACE/b13-a1/events.jsonl`; extract the `submitted`
blocks into the `LINE` env var (the script does the rest).

**The open question:** the receipt notes Parental Bond Double-Edge is a genuine
threat to Kyogre, so a mixed forecast could legitimately defend the guard. The
margin settles it — a defensible guard wins by a lot, a mispriced one by a hair.
Read the number before deciding it is a bug.

---

## 3. Mega Milotic Ice Beam

Wallace E0497, build-16 rerun, **turn 8** (`work/playtest/328-TRAINER_WALLACE/`
receipt-b16.json, run dir `b16-a1`). Mega Milotic chose Ice Beam into an
**Assault Vest Kingambit** — Ice is 0.5x into Dark/Steel, and it is a special
attack into a special wall, with its own partner beside it. Untouched by me.
Use the per-move damage trace (`gAiMoveDamageTrace[battler][slot]`, added this
session) to see whether the damage model already knows Ice Beam is bad there; if
it does, the defect is in what beats it, not in the damage.

---

## 4. J4: the ten `ai_doubles.c` reds

That file was last edited Sept 14, before omniscience was removed, so much of it
encodes the old model. **Method that worked:** reproduce, then probe whether a
*decisive* version of the board flips the choice. The probe is what separates a
stale fixture from a pricing bug — without it, "the new choice is defensible" is
just a story told about a passing test.

**Resolved (4):**

| Fixture | Outcome |
| --- | --- |
| Quiver Dance 2/2 | retargeted — probe: a certain-kill attacker does not change the choice, so it rests on target uncertainty |
| Acid Spray 2/3 | retargeted — in-fixture control (Covert Cloak, drop cannot land) still attacks |
| Cotton Guard 2/3 | retargeted; **3/3 was a real bug** → defensive-boost category rule, `e73cec83d7` |
| Iron Defense 2/2 | retargeted — probe: removing Sturdy does not change the choice either |

**Parked (3), all "expected Protect, got an attack":**

- Rock Tomb 2/2 (`ai_doubles.c:309`) — with Covert Cloak the drop fails, so the
  player moves first and kills the 65-HP partner *before it acts*; the fixture
  expected the AI to guard that partner, which the old model knew to do.
- damage-based recoil 2/3 (`:928`)
- offensive drops 3/6 (`:593`)

**The question that unparks them:** does the empty-guard rerun split show
*needed* guards being missed (as opposed to empty ones being taken)? If the
reruns show the AI now guards when it should, these three are stale fixtures and
retarget. If they show needed guards are being skipped, they are the same real
defect and must be fixed, not retargeted. Do not decide this from the fixtures
alone — that is how I would have got it wrong.

**Left (3), not yet examined:**

- `candidate Trace 2/3` (`:707`) — "Expected MOVE, got SWITCH/SEND_OUT". A switch
  shape, different family; plausibly real.
- `preserve the Plus Minus partner 1/2` (`:2908`) — HP equality, follows
  whichever action changed.
- `prevent a priority hit from activating Defeatist 1/3` (`:401`) — HP equality,
  same.

---

## 5. Groups L through V, in my intended order

One line each; all from playtest receipts, all still open unless noted.

- **L(1)** authored setup/support never chosen across five teams — *largely
  closed*: Tailwind and Dragon Dance fire on rebuilt boards; re-run the rest
  before touching any constant.
- **L(2)/O(4)/Q(3)/Y(2)** switch-in forecast — **closed** by `ab8a849663`
  (entry that a revealed move kills costs more than a body); Y(2) confirmed
  quiet on build-13.
- **L(3)/O(5)/Q(5)/R(3)** Wide Guard scored on the user rather than the partner
  — does not reproduce; pinned green.
- **M** Weather Ball type (withdrawn), Snipe Shot redirection, Breloom Spore,
  Toxicroak/Scizor conflict, Foul Play on the target's Attack (pinned), Salazzle
  Fake Out, Golbat Taunt (**fixed**, Taunt ignores a Protect-only target).
- **N** field-effect value: Lava Plume/Frost Breath ACTIVATE (**fixed** — the
  recipient's own guard blocked it, `d50e8337c8`), terrain removal enabling an
  ally's item, Defog behind mist, Eruption at full HP, Darmanitan at −6.
- **O** setup starvation (nine printed setup moves, two fired), spread scored
  below single-target with Telepathy partners, Choice lock into a resisted move,
  Gliscor's second Swords Dance.
- **P** big multi-stat setup declined on a bought turn (Shell Smash + White
  Herb/Simple, Shift Gear), Talonflame Tailwind, Drifblim consecutive guard,
  **Huntail into its own Storm Drain (fixed)**, Hippowdon empty guard,
  **Prankster Encore into Dark (fixed)**, Garbodor Aftermath.
- **Q** guard not discounted for a defender whose ability/item wants the hit
  (Water Compaction, Steam Engine, Weakness Policy, Flash Fire, Lightning Rod,
  Water Absorb); Weezing's entry-turn guard; Orthworm pivot; Ninetales Heat Wave
  under enemy rain; four authored Wide Guards; Stakataka Trick Room latch.
- **R** ally-targeted support resolving to a foe (**fixed**, `cef54e57f9` +
  `4b72954cd3` for the per-battler path); denied setup treated as refuted
  (Lapras Rain Dance after a flinch); Toxapex Surf three turns; Sharpedo's
  Speed Boost Protect.
- **S** Cresselia Helping Hand onto a guarding partner (**fixed**, `141b8201e8`);
  Lunatone/Gardevoir switch-ins; Seviper's Coil; Klinklang/Sandy Shocks into
  Swampert; Zoroark/Kingambit decoy inversion.
- **T** Rabsca Revival Blessing never selected; Xatu Tailwind at 1 HP; Exeggutor
  consecutive guard; entry-turn empty guards; Ally Switch never selected.
- **U** stat move at +6 (Belly Drum, Nasty Plot, Swords Dance, Coil) must be
  excluded outright; **tactic actor/recipient resolved backwards** (Lumineon/
  Gastrodon — worth checking against my `d50e8337c8` change); Sharpedo Mega vs
  banked Speed Boost; Gyarados Dragon Dance into a 4x Electric; Qwilfish four
  Spikes; Alomomola Wish with no recipient.
- **V** last-slot Megas dying un-evolved (**probably closed** by the Mega fix —
  reruns pending); Xerneas Geomancy; Shiinotic into a powder-immune target;
  Kingambit resisted Iron Head; Pinsir switch-in.
- **W/X/Y** mostly addressed this session; Y(1) second-Mega gate is **held for
  the user** (canon rule, engine files, do not touch).

---

## 6. The authored-fixture board sweep

**The rule:** a fixture whose board cannot fight back does not test a pair
decision. With no threat, the AI has no competing use for the turn, so every
joint conflict — guards, wasted support, blocked activations — disappears. Boards
made of Magikarp using Splash and Wobbuffet using Celebrate are the tell.

**Already re-pointed (2):**

- Aisha's activation — new fixture `EC authored strategy: Aisha's activation
  survives a board that can fight back`. **Re-pointing changed the verdict**: it
  failed immediately and exposed the blocked-activation bug (`d50e8337c8`).
- The Water Spout pin in `joint_conflicts.c` — now has attackers opposite. It
  passes either way, so re-pointing did **not** change the verdict, but it also
  means the original was never evidence for the live claim.

**Known still built on boards that cannot fight back** (not yet re-pointed):

- `EC Gym: Wattson's Discharge activates Motor Drive before Electivire acts`
  (`emerald_champions_plans.c`) — the comparison case for the whole ACTIVATE
  family; re-point this first.
- `EC authored strategy: Aisha's Frost Breath activates Anger Point` and
  `Georgia's Shadow Sneak arms the Weakness Policy` — the original harmless
  versions; keep them, but do not cite them as live evidence.
- `EC Gym: Jocelyn dances on a safe board with the Dancer relay up` — harmless
  by design and says so, which is fine as long as nobody reads it as proof the
  relay fires in play.
- Much of `setup_pricing.c`, `next_turn_values.c` and `support_starvation.c`
  uses Celebrate/Splash bodies. Those test *pricing in isolation*, which is
  legitimate; the risk is only in citing them as evidence about live boards.

**Two of my own pins this rule has overturned:**

1. Group F's "Wattson's Discharge activation fires on his real board" — measured
   on a board with nothing to fight, so it is not evidence the interaction works
   in play.
2. Y(3)'s "at full health Kyogre takes Water Spout" — true on my board, false on
   Wallace's, which is the whole of §1.

---

## 7. Unverified beliefs

Stated plainly because they have shaped my choices and none is measured:

- **The empty-guard fix probably closes the consecutive-Protect family**
  (Camerupt, Drifblim, Noctowl, Runerigus, Exeggutor). Reasoning: a second guard
  chosen against nothing is the same defect one turn later. Three rooms have gone
  quiet; nine outstanding. **Unverified.**
- **The Mega fix probably closes K and V's un-evolved Megas** (Kangaskhan,
  Machamp, Absol, Falinks, Raichu X) — all are Megas never *considered* on boards
  heavy enough to exhaust the clock. Reruns pending. **Unverified.**
- **The self-target clamp probably explains every "0x into an immune target"
  report from E0409.** Rhyperior, Great Tusk and Mega Heatran were aiming at
  themselves; a self-aimed attack produces no damage line, which reads exactly
  like an immune target in a receipt. I never read `moveTarget` back to confirm
  the clamp field-by-field. **Unverified.**
- **The switch-entry charge may be too blunt.** It costs 220 — more than a body —
  whenever a revealed move would kill the incoming reserve. That is deliberately
  dominant, and it could suppress a switch that is right for other reasons (a
  Regenerator pivot, a sacrifice to preserve a sweeper). No fixture covers that
  case. **Unverified.**
- **`PAIR_WORK_MAX`/`PAIR_WORK_MIN` (96/24) are not tuned**, only checked against
  the Dancer board at 60 frames and my suite. Heavier boards than the ones I have
  may still truncate in ways I have not seen. **Unverified.**
- **The ten `ai_doubles.c` reds are mostly stale rather than broken.** Four of
  the first four went that way, but that is a small sample and the fifth
  (Cotton Guard 3/3) was real. **Unverified for the remaining six.**
- **`gAiPairBudgetTruncated` is sticky for the battle**, by design, so any trace
  reading it per-decision is wrong. Use `gAiPairDecisionTruncated`. I made this
  mistake once and it cost a misread.

---

## Things that cost me time; save yourself the same

- `git checkout -- <path>` restores from the **index**. If an experiment was ever
  `git add`ed, that is what comes back. Use `git checkout HEAD -- <path>`, and
  read the first column of `git status --short`.
- The stamp races constantly against other agents rewriting
  `src/data/trainers.party`. `touch <elf> && stamp && check` in a tight retry
  loop, not `touch; sleep; stamp`.
- A build that dies on a half-written file in a shared tree is not your bug;
  wait and rebuild. `campaign cap did not reach N; the ROM reports 0` means the
  ROM was built against an inconsistent data state — rebuild, do not debug.
- Build an instrumented headless ROM under a different `BUILD_NAME` and move it
  into your own directory as `pokeemerald-headless.*`; never write the root
  triple. One line in `docs/VERIFICATION.md` describes it.
- **When a report survives a fixture, go to the replay.** Four constructed boards
  found nothing on the switch family; one instrumented replay found it in a
  build. Same for Mega, empty guards and Y(3).
