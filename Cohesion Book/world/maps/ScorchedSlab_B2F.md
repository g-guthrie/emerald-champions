# ScorchedSlab_B2F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/ScorchedSlab_B2F/map.json) · [Scripts](../../baseline/source/data/maps/ScorchedSlab_B2F/scripts.inc)

## Current map contract

`MAP_SCORCHED_SLAB_B2F` · `LAYOUT_SCORCHED_SLAB_B2F` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ScorchedSlab_B2F:object_events:001` | 28,22 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (28,22); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `ScorchedSlab_B2F:object_events:002` | 29,22 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (29,22); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `ScorchedSlab_B2F:object_events:003` | 29,25 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (29,25); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `ScorchedSlab_B2F:object_events:004` | 29,24 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (29,24); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `ScorchedSlab_B2F:object_events:005` | 23,22 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BEAST_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_SCORCHED_BEAST_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ScorchedSlab_B2F:object_events:006` | 18,28 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (18,28); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `ScorchedSlab_B2F:object_events:007` | 19,28 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (19,28); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `ScorchedSlab_B2F:object_events:008` | 12,15 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (12,15); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `ScorchedSlab_B2F:warp_events:001` | 17,15 | Warp from (17,15, elevation 0) to MAP_SCORCHED_SLAB_B1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ScorchedSlab_B2F:warp_events:002` | 18,4 | Warp from (18,4, elevation 0) to MAP_SCORCHED_SLAB_HEATRANS_ROOM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
