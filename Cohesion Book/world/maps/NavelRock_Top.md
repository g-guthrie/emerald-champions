# NavelRock_Top

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/NavelRock_Top/map.json) · [Scripts](../../baseline/source/data/maps/NavelRock_Top/scripts.inc)

## Current map contract

`MAP_NAVEL_ROCK_TOP` · `LAYOUT_NAVEL_ROCK_TOP` · `WEATHER_SHADE` · `MUS_RG_SEVII_CAVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `NavelRock_Top:object_events:001` | 12,4 | Passive/staged OBJ_EVENT_GFX_HOOH at (12,4); visibility flag FLAG_HIDE_HO_OH; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `NavelRock_Top:coord_events:001` | 12,10 | [NavelRock_Top_EventScript_HoOh](../../baseline/source/data/maps/NavelRock_Top/scripts.inc#L33) — Coordinate trigger at (12,10); VAR_TEMP_1 == 0 invokes NavelRock_Top_EventScript_HoOh. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `NavelRock_Top:bg_events:001` | 12,9 | Hidden ITEM_SACRED_ASH at (12,9); persistent flag FLAG_HIDDEN_ITEM_NAVEL_ROCK_TOP_SACRED_ASH. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `NavelRock_Top:warp_events:001` | 13,20 | Warp from (13,20, elevation 3) to MAP_NAVEL_ROCK_UP4 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `NavelRock_Top:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [NavelRock_Top_OnTransition](../../baseline/source/data/maps/NavelRock_Top/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls NavelRock_Top_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `NavelRock_Top:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [NavelRock_Top_OnResume](../../baseline/source/data/maps/NavelRock_Top/scripts.inc#L23) — MAP_SCRIPT_ON_RESUME calls NavelRock_Top_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
