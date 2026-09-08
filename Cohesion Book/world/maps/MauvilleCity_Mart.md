# MauvilleCity_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/MauvilleCity_Mart/map.json) · [Scripts](../../baseline/source/data/maps/MauvilleCity_Mart/scripts.inc)

## Current map contract

`MAP_MAUVILLE_CITY_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MauvilleCity_Mart:object_events:001` | 1,3 | [MauvilleCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/MauvilleCity_Mart/scripts.inc#L4) — MauvilleCity_Mart_EventScript_Clerk at (1,3); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MauvilleCity_Mart:object_events:002` | 1,4 | [MauvilleCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/MauvilleCity_Mart/scripts.inc#L4) — MauvilleCity_Mart_EventScript_Clerk at (1,4); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MauvilleCity_Mart:object_events:003` | 5,4 | [MauvilleCity_Mart_EventScript_ExpertM](../../baseline/source/data/maps/MauvilleCity_Mart/scripts.inc#L30) — MauvilleCity_Mart_EventScript_ExpertM at (5,4); Trainer battles do not allow BAG items. /  This shop stocks specialized POKé BALLS /  instead, so you can build more teams. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MauvilleCity_Mart:object_events:004` | 5,5 | [MauvilleCity_Mart_EventScript_Man](../../baseline/source/data/maps/MauvilleCity_Mart/scripts.inc#L34) — MauvilleCity_Mart_EventScript_Man at (5,5); Choose a move, switch a partner, or /  protect a position… //  The TRAINER's decisions determine how /  battles turn out. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MauvilleCity_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_MAUVILLE_CITY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MauvilleCity_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_MAUVILLE_CITY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
