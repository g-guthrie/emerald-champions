# TrainerHill_Entrance

**REVISE.** FAC-02 preserves Time Attack access, timer, doors and prizes while replacing legacy opponent construction. Any change to the field pair must preserve shared floor completion and loss/withdraw return behavior.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/TrainerHill_Entrance/map.json) · [Scripts](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc)

## Current map contract

`MAP_TRAINER_HILL_ENTRANCE` · `LAYOUT_TRAINER_HILL_ENTRANCE` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `TrainerHill_Entrance:object_events:001` | 11,6 | [TrainerHill_Entrance_EventScript_Attendant](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L107) — TrainerHill_Entrance_EventScript_Attendant at (11,6); I hope you give it your best. / Thank you for playing! | **REVISE** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · FAC-02 |
| `TrainerHill_Entrance:object_events:002` | 4,9 | [TrainerHill_Entrance_EventScript_Nurse](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L99) — TrainerHill_Entrance_EventScript_Nurse at (4,9); shared behavior FACILITY | **REPAIR** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · FAC-02, W-CENTER-FIRST-VISIT |
| `TrainerHill_Entrance:object_events:003` | 14,9 | [TrainerHill_Entrance_EventScript_Clerk](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L241) — TrainerHill_Entrance_EventScript_Clerk at (14,9); shared behavior FACILITY | **REVISE** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · FAC-02 |
| `TrainerHill_Entrance:object_events:004` | 5,14 | [TrainerHill_Entrance_EventScript_Girl](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L232) — TrainerHill_Entrance_EventScript_Girl at (5,14); Do you see the Time Board over there? //  My friends and I are trying to see who /  can reach the top in the least time. / Do you know when they're opening /  this place up? //  I'm waiting here to be the first /  challenger ever! | **REVISE** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · FAC-02 |
| `TrainerHill_Entrance:object_events:005` | 14,15 | [TrainerHill_Entrance_EventScript_Man](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L223) — TrainerHill_Entrance_EventScript_Man at (14,15); Who knows what sort of TRAINERS /  and POKéMON combos are ahead? //  All I know is that I'll knock aside /  anyone that stands in my way! / I heard tough TRAINERS come to this /  TRAINER HILL from all over. //  I can't wait to test the waters! //  I'll knock aside anyone that stands /  in my way! | **REVISE** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · FAC-02 |
| `TrainerHill_Entrance:coord_events:001` | 9,6 | [TrainerHill_Entrance_EventScript_EntryTrigger](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L121) — Coordinate trigger at (9,6); VAR_TRAINER_HILL_IS_ACTIVE == 0 invokes TrainerHill_Entrance_EventScript_EntryTrigger. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · FAC-02 |
| `TrainerHill_Entrance:bg_events:001` | 8,10 | [TrainerHill_Entrance_EventScript_Records](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L215) — TrainerHill_Entrance_EventScript_Records at (8,10); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · FAC-02 |
| `TrainerHill_Entrance:warp_events:001` | 9,16 | Warp from (9,16, elevation 3) to MAP_ROUTE111 warp 4. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-02 |
| `TrainerHill_Entrance:warp_events:002` | 10,16 | Warp from (10,16, elevation 3) to MAP_ROUTE111 warp 4. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-02 |
| `TrainerHill_Entrance:warp_events:003` | 9,1 | Warp from (9,1, elevation 3) to MAP_TRAINER_HILL_1F warp 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · FAC-02 |
| `TrainerHill_Entrance:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [TrainerHill_Entrance_OnResume](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L18) — MAP_SCRIPT_ON_RESUME calls TrainerHill_Entrance_OnResume. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-02 |
| `TrainerHill_Entrance:map_scripts:002` | MAP_SCRIPT_ON_RETURN_TO_FIELD | [TrainerHill_Entrance_OnReturn](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L36) — MAP_SCRIPT_ON_RETURN_TO_FIELD calls TrainerHill_Entrance_OnReturn. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-02 |
| `TrainerHill_Entrance:map_scripts:003` | MAP_SCRIPT_ON_TRANSITION | [TrainerHill_Entrance_OnTransition](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L9) — MAP_SCRIPT_ON_TRANSITION calls TrainerHill_Entrance_OnTransition. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-02 |
| `TrainerHill_Entrance:map_scripts:004` | MAP_SCRIPT_ON_LOAD | [TrainerHill_Entrance_OnLoad](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L43) — MAP_SCRIPT_ON_LOAD calls TrainerHill_Entrance_OnLoad. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-02 |
| `TrainerHill_Entrance:map_scripts:005` | MAP_SCRIPT_ON_FRAME_TABLE | [TrainerHill_Entrance_OnFrame](../../baseline/source/data/maps/TrainerHill_Entrance/scripts.inc#L51) — MAP_SCRIPT_ON_FRAME_TABLE calls TrainerHill_Entrance_OnFrame. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-02 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
