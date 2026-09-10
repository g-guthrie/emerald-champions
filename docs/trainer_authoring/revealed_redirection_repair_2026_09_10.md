# Revealed redirection forecast repair — 2026-09-10

Found during E0111 Jaylen's native opposing-rain review. Politoed/Volt Absorb
Pachirisu face Cryogonal/Magnezone. Pachirisu uses Follow Me; Magnezone selects
Thunderbolt toward Politoed on turns1–4, repeatedly healing Pachirisu instead.
This is a demonstrated repeated wasted-action defect, not a request for perfect
prediction. `work/jaylen-gen9-followthrough.log` preserves the pre-fix sequence.

## Cause and bounded correction

`src/battle_ai_pair.c::ChooseJointFoeForecast` chooses threatening opposing
damage pairs. It did not forecast a repeated support move even after seeing
Follow Me. The existing action evaluator already resolves redirection, recipient
immunity, priority, powder immunity and bypasses, but its foe forecasts supplied
only attacks. Therefore the AI kept valuing damage against the wrong recipient.

Keep the strongest damaging forecast. If a living foe with a partner last used
Follow Me or Rage Powder and can still legally use it, use the existing second
forecast slot for that possibility. The foe's choices already enforce PP,
Taunt and other move limitations; incapacity is additionally excluded. The
normal alternate damage-target forecast remains when no revealed redirector
qualifies. At most one revealed redirector is selected for this alternative.

This is uncertainty about a revealed move, not knowledge of the user's current
command. A predicted redirector may attack instead; that primary damaging
possibility is retained. A partner may KO the redirector before a later attack,
and the existing action-order evaluation still handles that. No blanket ban on
Electric attacks, no trainer-specific immunity exception and no forced response.
The first unseen Follow Me can still catch the AI.

No new forecast slots, recursive turn search, heap allocation or per-trial
native damage recalculation. A small scan of existing choices is added once
per evaluated board. Actual release ROM/RAM growth has not been measured.

## Evidence and retained checks

- `work/revealed-redirection-before.log`: the frozen three-turn native board
  fails the no-repeat Thunderbolt PP assertion before the repair.
- `work/ai-and-jaylen-redirection-candidate.log`: new regression passes; the
  rain sequence changes from repeated healing to Blizzard/Body Press progress
  and a turn4 finish. An older Leech Seed assertion fails because Ferroseed
  now switches to Cyclizar, not because another Seed was wasted.
- The Leech Seed check retains actual successful seed state and exactly one
  Seed PP expenditure. It reads the original setter's party PP after switching
  or active PP otherwise. Removed unrelated active-species/Gyro Ball locks:
  the regression protects failed repetition, not a particular tactical script.
- `work/ai-and-jaylen-final.log`:63/63 groups pass, including60 retained shared
  groups and3 temporary Jaylen groups/36 cases. The new retained regression
  permits an initial Thunderbolt but rejects repeated use after revelation.

Jaylen's original sampled complete decisions remain48 full/18 isolated frames.
Post-revelation timing and the clean scratch-free shared run are recorded in
his final encounter document. This is not an all516-battle replay, an exhaustive
redirection-mechanics certification or a new release build. Prior bounded
trainer results are historical execution evidence, not renewed campaign-wide
behavior guarantees after a shared policy change.
