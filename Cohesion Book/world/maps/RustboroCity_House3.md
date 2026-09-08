# RustboroCity_House3

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_House3/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_House3/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_HOUSE3` · `LAYOUT_RUSTBORO_CITY_HOUSE` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_House3:object_events:001` | 4,5 | [RustboroCity_House3_EventScript_OldMan](../../baseline/source/data/maps/RustboroCity_House3/scripts.inc#L4) — RustboroCity_House3_EventScript_OldMan at (4,5); For my own POKéMON, I give them /  perfectly suited nicknames! //  It's my expression of, uh… /  originality, yes, that's it! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_House3:object_events:002` | 7,5 | [RustboroCity_House3_EventScript_OldWoman](../../baseline/source/data/maps/RustboroCity_House3/scripts.inc#L8) — RustboroCity_House3_EventScript_OldWoman at (7,5); But giving the name PEKACHU to /  a PIKACHU? It seems pointless. //  I suppose it is good to use a name /  that's easy to understand, but… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_House3:object_events:003` | 4,4 | [RustboroCity_House3_EventScript_Pekachu](../../baseline/source/data/maps/RustboroCity_House3/scripts.inc#L13) — RustboroCity_House3_EventScript_Pekachu at (4,4); PEKACHU: Peka! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_House3:warp_events:001` | 5,8 | Warp from (5,8, elevation 0) to MAP_RUSTBORO_CITY warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_House3:warp_events:002` | 6,8 | Warp from (6,8, elevation 0) to MAP_RUSTBORO_CITY warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
