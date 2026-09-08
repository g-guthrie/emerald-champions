# VerdanturfMeadow

**KEEP.** Preserve current Deerling and Hisuian Lilligant guidance and the floral discovery space. The former missing-family/evolution criticism is superseded by snapshot fixes.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/VerdanturfMeadow/map.json) · [Scripts](../../baseline/source/data/maps/VerdanturfMeadow/scripts.inc)

## Current map contract

`MAP_VERDANTURF_MEADOW` · `LAYOUT_VERDANTURF_MEADOW` · `WEATHER_NONE` · `MUS_RG_SEVII_45`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VerdanturfMeadow:object_events:001` | 7,10 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_FLOETTITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_VERDANTURF_FLOETTITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `VerdanturfMeadow:object_events:002` | 12,9 | [VerdanturfMeadow_EventScript_Warden](../../baseline/source/data/maps/VerdanturfMeadow/scripts.inc#L10) — VerdanturfMeadow_EventScript_Warden at (12,9); DEERLING graze among these flowers. /  They evolve into SAWSBUCK at Lv. 34. //  A SUN STONE used on PETILIL in this /  meadow yields Hisuian LILLIGANT. /  Use it elsewhere for the usual form. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfMeadow:object_events:003` | 2,10 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (2,10); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `VerdanturfMeadow:bg_events:001` | 4,15 | Hidden ITEM_PINK_NECTAR at (4,15); persistent flag FLAG_EC_HIDDEN_ITEM_VERDANTURF_PINK_NECTAR. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `VerdanturfMeadow:bg_events:002` | 10,15 | Hidden ITEM_PURPLE_NECTAR at (10,15); persistent flag FLAG_EC_HIDDEN_ITEM_VERDANTURF_PURPLE_NECTAR. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `VerdanturfMeadow:connections:001` | up | up connection to MAP_VERDANTURF_TOWN, offset 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfMeadow:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [VerdanturfMeadow_OnTransition](../../baseline/source/data/maps/VerdanturfMeadow/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls VerdanturfMeadow_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
