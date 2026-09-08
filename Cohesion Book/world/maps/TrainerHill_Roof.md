# TrainerHill_Roof

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/TrainerHill_Roof/map.json) · [Scripts](../../baseline/source/data/maps/TrainerHill_Roof/scripts.inc)

## Current map contract

`MAP_TRAINER_HILL_ROOF` · `LAYOUT_TRAINER_HILL_ROOF` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `TrainerHill_Roof:object_events:001` | 12,7 | [TrainerHill_Roof_EventScript_Owner](../../baseline/source/data/maps/TrainerHill_Roof/scripts.inc#L6) — TrainerHill_Roof_EventScript_Owner at (12,7); shared behavior FACILITY | **REVISE** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · FAC-02 |
| `TrainerHill_Roof:warp_events:001` | 9,5 | Warp from (9,5, elevation 3) to MAP_TRAINER_HILL_4F warp 1. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-02 |
| `TrainerHill_Roof:warp_events:002` | 15,5 | Warp from (15,5, elevation 0) to MAP_TRAINER_HILL_ELEVATOR warp 1. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-02 |
| `TrainerHill_Roof:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [TrainerHill_OnResume](../../baseline/source/data/scripts/trainer_hill.inc#L1) — MAP_SCRIPT_ON_RESUME calls TrainerHill_OnResume. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-02 |
| `TrainerHill_Roof:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [TrainerHill_OnFrame](../../baseline/source/data/scripts/trainer_hill.inc#L20) — MAP_SCRIPT_ON_FRAME_TABLE calls TrainerHill_OnFrame. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-02 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
