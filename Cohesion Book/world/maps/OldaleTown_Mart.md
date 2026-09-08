# OldaleTown_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/OldaleTown_Mart/map.json) · [Scripts](../../baseline/source/data/maps/OldaleTown_Mart/scripts.inc)

## Current map contract

`MAP_OLDALE_TOWN_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `OldaleTown_Mart:object_events:001` | 1,3 | [OldaleTown_Mart_EventScript_Clerk](../../baseline/source/data/maps/OldaleTown_Mart/scripts.inc#L4) — OldaleTown_Mart_EventScript_Clerk at (1,3); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `OldaleTown_Mart:object_events:002` | 5,5 | [OldaleTown_Mart_EventScript_Woman](../../baseline/source/data/maps/OldaleTown_Mart/scripts.inc#L38) — OldaleTown_Mart_EventScript_Woman at (5,5); The clerk says they're all sold out. /  I can't buy any POKé BALLS. / I'm going to buy a bunch of POKé BALLS /  and catch a bunch of POKéMON! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `OldaleTown_Mart:object_events:003` | 9,4 | [OldaleTown_Mart_EventScript_Boy](../../baseline/source/data/maps/OldaleTown_Mart/scripts.inc#L51) — OldaleTown_Mart_EventScript_Boy at (9,4); If a POKéMON gets hurt and loses its HP /  and faints, it won't be able to battle. //  To prevent your POKéMON from fainting, /  restore its HP with a POTION. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `OldaleTown_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_OLDALE_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_OLDALE_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
