# OldaleTown_House2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/OldaleTown_House2/map.json) · [Scripts](../../baseline/source/data/maps/OldaleTown_House2/scripts.inc)

## Current map contract

`MAP_OLDALE_TOWN_HOUSE2` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_OLDALE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `OldaleTown_House2:object_events:001` | 4,4 | [OldaleTown_House2_EventScript_Woman](../../baseline/source/data/maps/OldaleTown_House2/scripts.inc#L4) — OldaleTown_House2_EventScript_Woman at (4,4); POKéMON grow from battles, but you /  never need to grind. //  A POKéMON CENTER LEVELER trains your /  whole party to the current limit. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `OldaleTown_House2:object_events:002` | 7,4 | [OldaleTown_House2_EventScript_Man](../../baseline/source/data/maps/OldaleTown_House2/scripts.inc#L8) — OldaleTown_House2_EventScript_Man at (7,4); Raw levels alone won't take you far. //  Build different partners for different /  battles, then change plans when needed. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `OldaleTown_House2:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_OLDALE_TOWN warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown_House2:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_OLDALE_TOWN warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
