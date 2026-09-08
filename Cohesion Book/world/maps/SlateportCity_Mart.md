# SlateportCity_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/SlateportCity_Mart/map.json) · [Scripts](../../baseline/source/data/maps/SlateportCity_Mart/scripts.inc)

## Current map contract

`MAP_SLATEPORT_CITY_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SlateportCity_Mart:object_events:001` | 1,3 | [SlateportCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/SlateportCity_Mart/scripts.inc#L4) — SlateportCity_Mart_EventScript_Clerk at (1,3); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_Mart:object_events:002` | 1,4 | [SlateportCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/SlateportCity_Mart/scripts.inc#L4) — SlateportCity_Mart_EventScript_Clerk at (1,4); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_Mart:object_events:003` | 4,2 | [SlateportCity_Mart_EventScript_BlackBelt](../../baseline/source/data/maps/SlateportCity_Mart/scripts.inc#L27) — SlateportCity_Mart_EventScript_BlackBelt at (4,2); The MARKET has unusual merchandise. //  The MART is still a reliable place to /  restock everyday field supplies. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_Mart:object_events:004` | 5,5 | [SlateportCity_Mart_EventScript_Man](../../baseline/source/data/maps/SlateportCity_Mart/scripts.inc#L31) — SlateportCity_Mart_EventScript_Man at (5,5); A GREAT BALL is better than a POKé BALL /  at catching POKéMON. //  With this, I should be able to get that /  elusive POKéMON… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_SLATEPORT_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SlateportCity_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_SLATEPORT_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
