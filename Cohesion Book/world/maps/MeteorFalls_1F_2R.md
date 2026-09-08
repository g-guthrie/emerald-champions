# MeteorFalls_1F_2R

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/MeteorFalls_1F_2R/map.json) · [Scripts](../../baseline/source/data/maps/MeteorFalls_1F_2R/scripts.inc)

## Current map contract

`MAP_METEOR_FALLS_1F_2R` · `LAYOUT_METEOR_FALLS_1F_2R` · `WEATHER_NONE` · `MUS_CAVE_OF_ORIGIN`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MeteorFalls_1F_2R:object_events:001` | 13,2 | [MeteorFalls_1F_2R_EventScript_Nicolas](../../baseline/source/data/maps/MeteorFalls_1F_2R/scripts.inc#L14) — MeteorFalls_1F_2R_EventScript_Nicolas at (13,2); This is where we DRAGON users do our /  training. //  The CHAMPION even visits. /  Now do you see how special it is here? / Urgh! /  I didn't expect you to be so strong! | **REPAIR** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01, W-SIGN-LOCAL |
| `MeteorFalls_1F_2R:object_events:002` | 6,12 | [MeteorFalls_1F_2R_EventScript_John](../../baseline/source/data/maps/MeteorFalls_1F_2R/scripts.inc#L35) — MeteorFalls_1F_2R_EventScript_John at (6,12); JOHN: We've always battled POKéMON /  together as a twosome. /  We've confidence in ourselves. / JOHN: Oh, my. /  We've lost, dear wife. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MeteorFalls_1F_2R:object_events:003` | 7,12 | [MeteorFalls_1F_2R_EventScript_Jay](../../baseline/source/data/maps/MeteorFalls_1F_2R/scripts.inc#L54) — MeteorFalls_1F_2R_EventScript_Jay at (7,12); JOHN: Young TRAINER, may we trade /  notes on teamwork through MATCH CALL? / JAY: We've been married for /  fifty years. //  The bond we share as a couple could /  never be broken. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MeteorFalls_1F_2R:object_events:004` | 11,13 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MASTER_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_METEOR_FALLS_1F_2R_MASTER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MeteorFalls_1F_2R:object_events:005` | 18,2 | [MeteorFalls_1F_2R_EventScript_MoveSpecialist](../../baseline/source/data/maps/MeteorFalls_1F_2R/scripts.inc#L4) — MeteorFalls_1F_2R_EventScript_MoveSpecialist at (18,2); I once guarded the secret of /  DRAGON ASCENT here. //  The POKéMON CENTER specialist now /  teaches every legal move. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MeteorFalls_1F_2R:object_events:006` | 14,28 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (14,28); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `MeteorFalls_1F_2R:warp_events:001` | 10,29 | Warp from (10,29, elevation 3) to MAP_METEOR_FALLS_1F_1R warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_1F_2R:warp_events:002` | 4,14 | Warp from (4,14, elevation 3) to MAP_METEOR_FALLS_B1F_1R warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_1F_2R:warp_events:003` | 7,20 | Warp from (7,20, elevation 3) to MAP_METEOR_FALLS_B1F_1R warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_1F_2R:warp_events:004` | 21,23 | Warp from (21,23, elevation 3) to MAP_METEOR_FALLS_B1F_1R warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
