# Authored Return friendship repair — 2026-09-10

Found while reviewing E0106 Trent: Stoutland repeatedly chose coverage/support
instead of its intended Normal attack. This is a **trainer-data materialization
defect**, not an AI inability to recognize Return damage.

## Source chain and correction

`scripts/implement_emerald_champions_master_battles.py::render_party` previously
omitted happiness. `tools/trainerproc/main.c` only emits `.friendship` when the
trainer party contains a `Happiness:` property; unspecified C fields are zero.
`src/trainer_util.c` copies that field into the created Pokémon. Native
`src/battle_util.c` gives Return floor(10*friendship/25) base power, then clamps
zero to1. Thus the omitted field produced **1-BP Return**, not102-BP Return.

The canonical materializer now emits **Happiness:255 only for Return users**.
This is the existing trainerproc field, not a new trainer struct, runtime
exception, or player friendship rule. Frustration users keep zero friendship
and their102-BP attack. No stats/IVs/EVs change: B_FRIENDSHIP_BOOST is FALSE.
No change is made to Frustration, affection, Return's native formula or the AI
damage evaluator. The field already exists in the compiled struct; no new
runtime state/search is introduced, and no measured ROM-size claim is made.

Current canonical scan: **one Return user**, TRAINER_TRENT_1/Stoutland;
**three Frustration users**, TRAINER_ALLEN/Bunnelby,
TRAINER_JOHNSON/Bunnelby and TRAINER_DAWSON/Wooloo. No other prior completed
battle is newly affected by this materialization change. Other trainer data
outside the canonical campaign is not silently normalized.

The first patch used the wrong text key `Friendship:`; trainerproc rejected it
before execution. Corrected to its supported `Happiness:` spelling. That build
failure was an authoring-tool syntax error, not a native gameplay result.

## Native verification

`work/trent-gen9-final-complete.log` passes all four actual generated-party
friendship checks:255 for Return,0 for Frustration. It also demonstrates live
Return selection, consumed PP, damage and knockouts. The complete Trent review
passes4 groups/43 cases; this field check alone is not battle completion.
After removing its scratch include, `work/ai-shared-through-trent-final.log`
passes57/57 retained shared groups. Canonical trainer/shared-set comparisons pass.

The regular canonical comparison covers the newly emitted property. No giant
per-battle test corpus or unrelated battle revalidation gate is introduced.
No release build or Downloads replacement is part of this repair.
