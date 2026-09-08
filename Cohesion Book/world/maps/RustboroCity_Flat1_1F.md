# RustboroCity_Flat1_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_Flat1_1F/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_Flat1_1F/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_FLAT1_1F` · `LAYOUT_RUSTBORO_CITY_FLAT1_1F` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_Flat1_1F:object_events:001` | 9,4 | [RustboroCity_Flat1_1F_EventScript_Man](../../baseline/source/data/maps/RustboroCity_Flat1_1F/scripts.inc#L4) — RustboroCity_Flat1_1F_EventScript_Man at (9,4); Every POKéMON has a special ability /  that it can use. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat1_1F:object_events:002` | 12,4 | [RustboroCity_Flat1_1F_EventScript_Woman](../../baseline/source/data/maps/RustboroCity_Flat1_1F/scripts.inc#L8) — RustboroCity_Flat1_1F_EventScript_Woman at (12,4); POKéMON are such strange creatures. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat1_1F:warp_events:001` | 6,7 | Warp from (6,7, elevation 0) to MAP_RUSTBORO_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_Flat1_1F:warp_events:002` | 7,7 | Warp from (7,7, elevation 0) to MAP_RUSTBORO_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_Flat1_1F:warp_events:003` | 2,1 | Warp from (2,1, elevation 0) to MAP_RUSTBORO_CITY_FLAT1_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
