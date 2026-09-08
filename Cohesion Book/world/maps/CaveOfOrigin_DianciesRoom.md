# CaveOfOrigin_DianciesRoom

**KEEP.** W-WALLACE-ROOT deliberately restores the authored one-time rain exhibition as a talk interaction off the main aisle. Diancie and the Diancite pickup remain independent discoveries.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/map.json) · [Scripts](../../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/scripts.inc)

## Current map contract

`MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM` · `LAYOUT_CAVE_OF_ORIGIN_DIANCIES_ROOM` · `WEATHER_NONE` · `MUS_CAVE_OF_ORIGIN`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `CaveOfOrigin_DianciesRoom:object_events:001` | 9,9 | [CaveOfOrigin_DianciesRoom_EventScript_Diancie](../../baseline/source/data/maps/CaveOfOrigin_DianciesRoom/scripts.inc#L52) — CaveOfOrigin_DianciesRoom_EventScript_Diancie at (9,9); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `CaveOfOrigin_DianciesRoom:object_events:002` | 7,9 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DIANCITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_DIANCITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `CaveOfOrigin_DianciesRoom:warp_events:001` | 9,6 | Warp from (9,6, elevation 0) to MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP3 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
