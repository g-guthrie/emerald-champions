# ContestHall

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/ContestHall/map.json) · [Scripts](../../baseline/source/data/maps/ContestHall/scripts.inc)

## Current map contract

`MAP_CONTEST_HALL` · `LAYOUT_CONTEST_HALL` · `WEATHER_NONE` · `MUS_CONTEST`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ContestHall:object_events:001` | 6,4 | Passive/staged OBJ_EVENT_GFX_WOMAN_3 at (6,4); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:002` | 7,4 | Passive/staged OBJ_EVENT_GFX_CONTEST_JUDGE at (7,4); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:003` | 4,7 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (4,7); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:004` | 6,7 | Passive/staged OBJ_EVENT_GFX_VAR_1 at (6,7); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:005` | 8,7 | Passive/staged OBJ_EVENT_GFX_VAR_2 at (8,7); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:006` | 2,6 | Passive/staged OBJ_EVENT_GFX_VAR_4 at (2,6); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:007` | 4,2 | Passive/staged OBJ_EVENT_GFX_VAR_6 at (4,2); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:008` | 5,2 | Passive/staged OBJ_EVENT_GFX_VAR_7 at (5,2); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:009` | 9,2 | Passive/staged OBJ_EVENT_GFX_VAR_8 at (9,2); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:010` | 10,2 | Passive/staged OBJ_EVENT_GFX_VAR_9 at (10,2); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:011` | 12,5 | Passive/staged OBJ_EVENT_GFX_VAR_A at (12,5); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:012` | 2,5 | Passive/staged OBJ_EVENT_GFX_VAR_5 at (2,5); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:013` | 7,5 | Passive/staged OBJ_EVENT_GFX_ITEM_BALL at (7,5); visibility flag FLAG_HIDE_CONTEST_POKE_BALL; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:014` | 10,7 | Passive/staged OBJ_EVENT_GFX_VAR_3 at (10,7); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:object_events:015` | 12,6 | Passive/staged OBJ_EVENT_GFX_ARTIST at (12,6); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `ContestHall:map_scripts:001` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [ContestHall_OnWarp](../../baseline/source/data/maps/ContestHall/scripts.inc#L61) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls ContestHall_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `ContestHall:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [ContestHall_OnFrame](../../baseline/source/data/maps/ContestHall/scripts.inc#L57) — MAP_SCRIPT_ON_FRAME_TABLE calls ContestHall_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `ContestHall:map_scripts:003` | MAP_SCRIPT_ON_TRANSITION | [ContestHall_OnTransition](../../baseline/source/data/maps/ContestHall/scripts.inc#L13) — MAP_SCRIPT_ON_TRANSITION calls ContestHall_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `ContestHall:map_scripts:004` | MAP_SCRIPT_ON_RESUME | [ContestHall_OnResume](../../baseline/source/data/maps/ContestHall/scripts.inc#L49) — MAP_SCRIPT_ON_RESUME calls ContestHall_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `ContestHall:map_scripts:005` | MAP_SCRIPT_ON_RETURN_TO_FIELD | [ContestHall_OnReturn](../../baseline/source/data/maps/ContestHall/scripts.inc#L9) — MAP_SCRIPT_ON_RETURN_TO_FIELD calls ContestHall_OnReturn. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
