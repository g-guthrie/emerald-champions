# BirthIsland_Exterior

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BirthIsland_Exterior/map.json) · [Scripts](../../baseline/source/data/maps/BirthIsland_Exterior/scripts.inc)

## Current map contract

`MAP_BIRTH_ISLAND_EXTERIOR` · `LAYOUT_BIRTH_ISLAND_EXTERIOR` · `WEATHER_NONE` · `MUS_NONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BirthIsland_Exterior:object_events:001` | 15,12 | [BirthIsland_Exterior_EventScript_Triangle](../../baseline/source/data/maps/BirthIsland_Exterior/scripts.inc#L41) — BirthIsland_Exterior_EventScript_Triangle at (15,12); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `BirthIsland_Exterior:object_events:002` | 15,3 | Passive/staged OBJ_EVENT_GFX_DEOXYS at (15,3); visibility flag FLAG_HIDE_DEOXYS; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BirthIsland_Exterior:warp_events:001` | 15,24 | Warp from (15,24, elevation 0) to MAP_BIRTH_ISLAND_HARBOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BirthIsland_Exterior:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [BirthIsland_Exterior_OnTransition](../../baseline/source/data/maps/BirthIsland_Exterior/scripts.inc#L11) — MAP_SCRIPT_ON_TRANSITION calls BirthIsland_Exterior_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BirthIsland_Exterior:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [BirthIsland_Exterior_OnResume](../../baseline/source/data/maps/BirthIsland_Exterior/scripts.inc#L31) — MAP_SCRIPT_ON_RESUME calls BirthIsland_Exterior_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BirthIsland_Exterior:map_scripts:003` | MAP_SCRIPT_ON_RETURN_TO_FIELD | [BirthIsland_Exterior_OnReturnToField](../../baseline/source/data/maps/BirthIsland_Exterior/scripts.inc#L7) — MAP_SCRIPT_ON_RETURN_TO_FIELD calls BirthIsland_Exterior_OnReturnToField. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
