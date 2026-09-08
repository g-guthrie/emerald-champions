# BirthIsland_Harbor

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BirthIsland_Harbor/map.json) · [Scripts](../../baseline/source/data/maps/BirthIsland_Harbor/scripts.inc)

## Current map contract

`MAP_BIRTH_ISLAND_HARBOR` · `LAYOUT_ISLAND_HARBOR` · `WEATHER_NONE` · `MUS_NONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BirthIsland_Harbor:object_events:001` | 8,5 | [BirthIsland_Harbor_EventScript_Sailor](../../baseline/source/data/maps/BirthIsland_Harbor/scripts.inc#L4) — BirthIsland_Harbor_EventScript_Sailor at (8,5); shared behavior TRAVEL | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `BirthIsland_Harbor:object_events:002` | 8,7 | Passive/staged OBJ_EVENT_GFX_SS_TIDAL at (8,7); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BirthIsland_Harbor:warp_events:001` | 8,2 | Warp from (8,2, elevation 0) to MAP_BIRTH_ISLAND_EXTERIOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
