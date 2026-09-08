# CaveOfOrigin_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/CaveOfOrigin_1F/map.json) · [Scripts](../../baseline/source/data/maps/CaveOfOrigin_1F/scripts.inc)

## Current map contract

`MAP_CAVE_OF_ORIGIN_1F` · `LAYOUT_CAVE_OF_ORIGIN_1F` · `WEATHER_NONE` · `MUS_CAVE_OF_ORIGIN`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `CaveOfOrigin_1F:object_events:001` | 5,8 | [CaveOfOrigin_1F_EventScript_Carbink](../../baseline/source/data/maps/CaveOfOrigin_1F/scripts.inc#L9) — CaveOfOrigin_1F_EventScript_Carbink at (5,8); This CARBINK is guarding an old ladder. //  Its crystals echo with the energy of /  eight BADGES. Come back when they shine. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `CaveOfOrigin_1F:warp_events:001` | 11,17 | Warp from (11,17, elevation 3) to MAP_CAVE_OF_ORIGIN_ENTRANCE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `CaveOfOrigin_1F:warp_events:002` | 14,5 | Warp from (14,5, elevation 3) to MAP_CAVE_OF_ORIGIN_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `CaveOfOrigin_1F:warp_events:003` | 5,8 | Warp from (5,8, elevation 0) to MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP1 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `CaveOfOrigin_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [CaveOfOrigin_1F_OnTransition](../../baseline/source/data/maps/CaveOfOrigin_1F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls CaveOfOrigin_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
