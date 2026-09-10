# Ability engine paths and shared-AI coverage — September 9, 2026

This is a targeted source trace of the current implementation for fifteen
self/ally ability themes plus Simple Beam. It maps mechanics and shared AI only;
trainer/team placement is owned by the concurrent team audit. Runtime code,
evaluator code, and play evidence are three different things:

- **Runtime exists** means the battle engine has an executable mechanic.
- **Evaluator handles it** means current shared AI gives the tactic explicit
  scoring or simulates its payoff. A heuristic mention is not the same as a
  stateful joint forecast.
- **Proven play** requires an executed native battle scenario. The procedural
  trace ran no battles. A subsequent main-review Steam Engine correction and
  its narrowly scoped native check are explicitly recorded below; they do not
  establish every listed ability engine or any completed trainer battle.

## Active generation configuration

`GEN_LATEST` is `GEN_CHAMPIONS`, one step after Gen 9
(`include/config/general.h:71-74`). Relevant current settings are all
`GEN_LATEST`:

- modern Beat Up behavior: `include/config/battle.h:128`;
- Gen 8+ Rattled interaction with Intimidate: `include/config/battle.h:202`;
- Gen 5+ Lightning Rod/Storm Drain immunity and ally redirection:
  `include/config/battle.h:208-209`;
- Weak Armor's two-stage Speed increase: `include/config/battle.h:213`;
- Gen 8+ Dancer ordering by current modified Speed:
  `include/config/battle.h:229`.

The ordinary doubles scorer is `AI_DoubleBattle` in
`src/battle_ai_main.c`. Smart campaign doubles also use the bounded joint
decision path when both active allies have `AI_FLAG_SMART_MON_CHOICES`
(`src/battle_ai_pair.c:2963-2970`). That path evaluates the two allied actions,
limited switches and forms together (`:3010-3099`) with a one-turn fast forecast
(`:2666-2746`). Its own comment describes only a small number of explicitly
cached partner stat interactions (`src/battle_ai_pair.c:77-83`). Therefore a
bonus in `battle_ai_main.c` can help a move enter consideration while its actual
same-turn stat payoff is still absent from the final pair simulation.

## Per-theme implementation map

### 1. Steam Engine

Runtime: after surviving a damaging Fire- or Water-type hit, including an ally
hit, Steam Engine raises Speed by six stages
(`src/battle_util.c:4302-4312`). It is not an immunity.

AI: the ordinary doubles scorer checks friendly-fire survival and usable Speed,
then rewards intentional Fire/Water ally targeting
(`src/battle_ai_main.c:3814-3828`; trigger eligibility at
`src/battle_ai_util.c:6471-6516`). The joint evaluator applies allied damage,
models Weakness Policy first, then adds six Speed stages before a later action
(`src/battle_ai_pair.c:2170-2194`). Main review found and corrected an overshoot:
the old forecast added the numeric stage ceiling (12), incorrectly turning a
prior -2 into +6 rather than the native +4. This is one
of the better-covered engines, although the source trace is not play proof.

Main-review verification after the six-stage correction: the existing Tabitha
native group passes all three parameters (useful activation, injured recipient,
unneeded activation). Log: `work/steam-engine-stage-review.log`. Those scenarios
start at neutral Speed; the negative-stage correction is supported by direct
runtime/forecast formula comparison, not claimed as a played negative-stage
decision boundary. No extra search states or cache fields were added.
After removing temporary diagnostic includes, all 46 retained `EC ` groups
also pass against the rebuilt shared tree. The existing warmed full-party pair
probe reports 30 GBA frames, not complete cold-decision or campaign latency
certification. Log: `work/ai-shared-current-20260909.log`.

### 2. Justified

Current encounter evidence: [E0038 Cristian](e0038_cristian_gen9_2026_09_09.md)
now executes six-hit allied Beat Up before Gallade's boosted attack, followed
by another attack rather than repeated activation. The Lucario fallback and
a living status-affected donor each demonstrate five-hit activation. Corrected
EV ordering and Focus Sash, not new shared search, enabled these sequences.
This supersedes source-only confidence for this trainer, not every holder.

Runtime: each surviving damaging Dark-type hit raises Attack one stage
(`src/battle_util.c:3909-3920`), so each eligible Beat Up strike can activate it.

AI: `ShouldBeatUpForJustified` requires a surviving target, Dark damaging move,
available Attack stage and physical payoff
(`src/battle_ai_util.c:3300-3317`). The ordinary doubles scorer has explicit
Beat Up partner targeting and Protect-order handling
(`src/battle_ai_main.c:3366-3373,3939-3960,4012-4020`). The joint evaluator uses
the actual eligible party count, not a fixed four or six, and applies that many
Attack stages before a later physical action
(`src/battle_ai_util.c:813-857`; `src/battle_ai_pair.c:2160-2169`). This is strong
source-level coverage.

### 3. Stamina

Runtime: every surviving damaging hit from another battler raises Defense one
stage (`src/battle_util.c:3945-3955`). Multi-hit moves can therefore activate it
repeatedly.

AI: the joint evaluator forecasts one Defense stage after a surviving
single-hit attack and can run a second immediate-effect pass
(`src/battle_ai_pair.c:2233-2251,2582-2642`). It deliberately excludes multi-hit,
Beat Up and Parental Bond sequences, stating that their internal
damage/Defense ordering is not represented (`:2143-2144,2237-2245`). Stamina is
also absent from the ordinary ally-trigger switch in
`src/battle_ai_util.c:6471-6516`. Thus basic single-hit defensive value is
partially modeled, but the requested multihit-to-Defense-to-Body Press engine is
not represented as such.

### 4. Weak Armor

Runtime: after surviving physical damage, Weak Armor lowers Defense one stage
and, under current configuration, raises Speed two
(`src/battle_util.c:3957-3971`; `include/config/battle.h:213`).

AI: there is no Weak Armor case in the ordinary partner-ability scoring block
(`src/battle_ai_main.c:3688-3935`), no trigger eligibility in
`src/battle_ai_util.c:6471-6516`, and no Weak Armor state transition in the joint
evaluator. An allied physical hit is therefore primarily seen as friendly-fire
damage, even if Weakness Policy is separately recognized. The core tactical
payoff is missing from shared AI.

### 5. Anger Point

Runtime: a surviving critical hit sets Attack to maximum
(`src/battle_util.c:4044-4054`). The condition does not exclude an ally.

AI: the ordinary doubles scorer recognizes guaranteed-critical partner moves,
requires the partner to survive and the attacker to act first, and rewards the
activation (`src/battle_ai_main.c:3408-3420,3693-3713`). The joint evaluator does
not apply the resulting maximum Attack stage before the recipient's action.
This is explicit heuristic coverage without full joint-payoff simulation.

### 6. Storm Drain

Runtime: selected Water moves can redirect to the earliest Storm Drain battler
under current ally-redirection configuration
(`src/battle_move_resolution.c:835-897`). On its own Water hit, Storm Drain
absorbs that hit and raises Special Attack one stage
(`src/battle_util.c:2332-2362`). A spread Surf is still processed for its other
targets; Storm Drain absorbs its own target hit rather than deleting the whole
move.

AI: opponent targeting recognizes partner redirection
(`src/battle_ai_main.c:1705-1717`; `src/battle_ai_util.c:601-627`), smart switching
can select a Water absorber (`src/battle_ai_switch.c:991-1014`), and ordinary
ally targeting values the Special Attack payoff
(`src/battle_ai_main.c:3766-3794`). The joint evaluator zeroes the recipient's
Water damage and raises Special Attack for a later special action
(`src/battle_ai_pair.c:2120-2128`). This is strong source-level coverage.

### 7. Lightning Rod

Runtime mirrors Storm Drain for Electric moves: selected moves redirect
(`src/battle_move_resolution.c:835-897`), and the recipient is immune and gains
one Special Attack stage (`src/battle_util.c:2355-2358`).

AI mirrors Storm Drain: opponent target denial, absorbing switches, ally-trigger
scoring and a same-turn joint Special Attack forecast are implemented at the
same paths (`src/battle_ai_main.c:1705-1717,3714-3742`;
`src/battle_ai_switch.c:991-1014`; `src/battle_ai_pair.c:2120-2128`). This is
strong source-level coverage, not executed battle evidence.

### 8. Sap Sipper

Runtime: a Grass hit is absorbed and raises Attack one stage
(`src/battle_util.c:2363-2366`).

AI: smart switching treats Sap Sipper as a Grass absorber
(`src/battle_ai_switch.c:1011-1014`), and ordinary doubles scoring rewards an
ally Grass trigger when the recipient has a physical payoff
(`src/battle_ai_main.c:3867-3881`; `src/battle_ai_util.c:6484-6489`). The joint
evaluator does not apply the Attack stage in its zero-damage ally-absorption
branch, which only handles Storm Drain and Lightning Rod
(`src/battle_ai_pair.c:2120-2141`). The final pair forecast therefore lacks the
same-turn physical payoff.

### 9. Well-Baked Body

Runtime: a Fire hit is absorbed and raises Defense two stages
(`src/battle_util.c:2367-2370`).

AI: smart switching recognizes the Fire immunity
(`src/battle_ai_switch.c:991-996`); ordinary doubles scoring rewards the ally
trigger when Defense can rise (`src/battle_ai_main.c:3851-3866`;
`src/battle_ai_util.c:6510-6512`). The joint evaluator neither applies the
Defense stages nor propagates them into a same-turn Body Press forecast. This is
heuristic-only tactical coverage.

### 10. Thermal Exchange

Runtime: after surviving Fire damage, Thermal Exchange raises Attack one stage
(`src/battle_util.c:4343-4353`). It also blocks burn application and clears burn
when acquired (`src/battle_util.c:5387-5405,9049-9056`). It does not absorb Fire
damage.

AI: ordinary doubles scoring checks damage, survival, Attack usefulness and
action order before rewarding an allied Fire hit
(`src/battle_ai_main.c:3829-3850`; `src/battle_ai_util.c:6484-6489`). The joint
evaluator applies the damage but not the Attack increase, so later physical
damage in the pair is undervalued. Tactical handling is partial.

### 11. Water Compaction

Runtime: after surviving Water damage, Water Compaction raises Defense two
stages (`src/battle_util.c:3933-3944`). It is not a Water immunity.

AI: ordinary doubles scoring explicitly requires friendly-fire survival and
available Defense, and rewards qualifying ally Water moves
(`src/battle_ai_main.c:3795-3813`; `src/battle_ai_util.c:6510-6512`). The joint
evaluator does not apply the Defense change or its Body Press payoff. Tactical
handling is partial and must not be confused with absorption.

### 12. Rattled

Runtime: each surviving damaging Dark, Bug or Ghost hit raises Speed one stage
(`src/battle_util.c:3921-3932`). Current Gen 8+ configuration also lets
Intimidate activate Rattled (`include/config/battle.h:202`;
`src/battle_script_commands.c:12038-12047`).

AI: ordinary scoring avoids donating boosts to enemies
(`src/battle_ai_main.c:1662-1666`) and can reward an ally trigger when Speed can
rise (`:3904-3919`; `src/battle_ai_util.c:6503-6505`). However, the specialized
Beat Up intent helper only admits Justified and Rage Fist
(`src/battle_ai_main.c:3366-3373`; `src/battle_ai_util.c:2933-2953`), and the
joint evaluator does not apply Rattled Speed stages or eligible Beat Up hit
count. The repeated-Speed engine is missing from final pair evaluation.

### 13. Costar

Runtime: on entry in doubles, Costar copies the living ally's stat stages and
critical-hit boosts (`src/battle_util.c:4800-4815`).

AI: switch-candidate simulation copies the same state
(`src/battle_ai_switch.c:3313-3346`). However, healthy reserve admission
explicitly recognizes weather/terrain restoration and Commander, but not
Costar (`src/battle_ai_pair.c:2851-2880`). Unless the side is already pressured
or an authored partner-tactics entry independently admits the reserve, a
proactive Costar pivot may never reach the otherwise-correct switch simulation.
This is partial coverage with a concrete candidate-generation gap.

### 14. Commander

Runtime: a base Tatsugiri beside Dondozo becomes semi-invulnerable and gives
Dondozo +2 Attack, Defense, Special Attack, Special Defense and Speed, subject to
double-battle, same-trainer and action/switch constraints
(`src/battle_util.c:4750-4785`). Commander state is cleared when Dondozo faints
(`src/battle_move_resolution.c:5195-5206`).

AI: the Circuit generator rejects Commander without Dondozo and recognizes the
pair (`src/champions_circuit.c:1227,1311-1312`). Shared pair reserve admission
explicitly considers Tatsugiri/Dondozo formation
(`src/battle_ai_pair.c:2861-2879`), and switch simulation installs the hidden
state and all five +2 stages (`src/battle_ai_switch.c:3313-3333`). Commander is
one of the best-covered engines in current source.

### 15. Dancer

Runtime: after a successful dance move, all other Dancer battlers are queued
(`src/battle_move_resolution.c:3462-3485`). `TryDancer` chooses activations by
current modified Speed under the active configuration, prevents recursion, and
executes the copied move with corrected ally targeting
(`src/battle_util.c:2997-3046`; completion hook at
`src/battle_move_resolution.c:5217-5225`).

AI: no Dancer-specific scoring or copied-move transition appears in
`battle_ai_main.c`, `battle_ai_util.c`, `battle_ai_pair.c`, or
`battle_ai_switch.c`. The joint evaluator therefore cannot value the extra
setup or attack, choose a dance for that payoff, or forecast Dancer's target and
order. Runtime exists; tactical evaluator coverage is missing.

### Simple Beam

Runtime: Simple Beam is a 100%-accurate selected status move whose overwrite
argument is `ABILITY_SIMPLE` (`src/data/moves_info.h:13237-13252`). Its script
rejects unoverwritable/same abilities and Ability Shield, then replaces the
target's live and overwritten Ability fields
(`data/battle_scripts_1.s:1159-1173`;
`src/battle_script_commands.c:8696-8720`). Simple doubles stat-stage changes in
the common stat-change engine (`src/battle_stat_change.c:770-784`).

AI: legality checks understand overwrite immunity and Ability Shield
(`src/battle_ai_util.c:6524-6642`). Ally targeting routes through
`AbilityChangeScore` (`src/battle_ai_main.c:3939-3960,5467-5492`), and
`BattlerBenefitsFromAbilityScore` considers Simple good when the recipient has
any self-raising move (`src/battle_ai_util.c:6659-6705,6824-6829`). Generic stat
scoring also knows existing Simple doubles stages
(`src/battle_ai_util.c:5448-5453`). The joint pair evaluator does not simulate
Simple Beam changing the ally's Ability before that ally acts, so it cannot
apply doubled same-turn setup or quantify which stat payoff matters. This is
generic heuristic coverage, not a complete Simple Beam engine.

## Highest-value actionable evaluator gaps

These are mechanics/evaluator gaps only; they do not prescribe trainer roster
changes.

1. **Add one bounded ally-hit transition shared by ordinary scoring and the
   joint forecast.** It should apply Weak Armor, Anger Point, Thermal Exchange,
   Water Compaction and Rattled after actual surviving damage, and Sap Sipper /
   Well-Baked Body after absorption. Preserve action order, Mold Breaker,
   Substitute and stat caps. This closes six currently heuristic-only or absent
   payoffs without trainer-specific scripts.
2. **Model repeated-hit ability activation where the tactic depends on it.** Use
   the existing eligible Beat Up count for Rattled, and represent Stamina's
   per-hit Defense sequence rather than explicitly discarding multi-hit moves.
   Recompute a later Body Press from the forecast Defense stage. Keep the model
   bounded; it need not branch every damage roll.
3. **Simulate Simple Beam before the partner's action.** Apply the same overwrite
   legality as runtime, temporarily replace the ally Ability, then pass doubled
   stages into the existing immediate-effect forecast. The current “has any
   self-raise” rating cannot distinguish a real same-turn payoff from a dead
   transfer.
4. **Represent Dancer's one copied action.** Reuse runtime dance classification
   and modified-Speed ordering, prevent recursive copies, and score the copied
   move's actual target/setup/damage once. Presently the defining engine is
   invisible to shared AI.
5. **Admit proactive Costar switch candidates.** When the active ally has
   copyable boosts, let Costar reach the existing correct switch-in simulation
   even if the side is healthy and no trainer-specific partner-tactics entry is
   present.

Steam Engine, Justified, Storm Drain, Lightning Rod and Commander already have
the most complete source-level paths. They still require encounter-specific
native execution before any team using them can be called tactically verified.
