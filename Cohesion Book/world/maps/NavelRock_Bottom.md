# NavelRock_Bottom

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/NavelRock_Bottom/map.json) · [Scripts](../../baseline/source/data/maps/NavelRock_Bottom/scripts.inc)

## Current map contract

`MAP_NAVEL_ROCK_BOTTOM` · `LAYOUT_NAVEL_ROCK_BOTTOM` · `WEATHER_NONE` · `MUS_RG_SEVII_CAVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `NavelRock_Bottom:object_events:001` | 11,13 | [NavelRock_Bottom_EventScript_Lugia](../../baseline/source/data/maps/NavelRock_Bottom/scripts.inc#L30) — NavelRock_Bottom_EventScript_Lugia at (11,13); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `NavelRock_Bottom:warp_events:001` | 14,19 | Warp from (14,19, elevation 0) to MAP_NAVEL_ROCK_DOWN11 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `NavelRock_Bottom:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [NavelRock_Bottom_OnTransition](../../baseline/source/data/maps/NavelRock_Bottom/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls NavelRock_Bottom_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `NavelRock_Bottom:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [NavelRock_Bottom_OnResume](../../baseline/source/data/maps/NavelRock_Bottom/scripts.inc#L20) — MAP_SCRIPT_ON_RESUME calls NavelRock_Bottom_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
