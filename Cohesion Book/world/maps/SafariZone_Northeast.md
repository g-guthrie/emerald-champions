# SafariZone_Northeast

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/SafariZone_Northeast/map.json) · [Scripts](../../baseline/source/data/maps/SafariZone_Northeast/scripts.inc)

## Current map contract

`MAP_SAFARI_ZONE_NORTHEAST` · `LAYOUT_SAFARI_ZONE_NORTHEAST` · `WEATHER_NONE` · `MUS_SAFARI_ZONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SafariZone_Northeast:object_events:001` | 8,20 | [SafariZone_Northeast_EventScript_Boy](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L140) — SafariZone_Northeast_EventScript_Boy at (8,20); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_Northeast:object_events:002` | 30,22 | [SafariZone_Northeast_EventScript_Girl](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L148) — SafariZone_Northeast_EventScript_Girl at (30,22); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_Northeast:object_events:003` | 11,11 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (11,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_Northeast:object_events:004` | 13,35 | [SafariZone_Northeast_EventScript_Woman](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L144) — SafariZone_Northeast_EventScript_Woman at (13,35); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_Northeast:object_events:005` | 8,13 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (8,13); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_Northeast:object_events:006` | 9,7 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (9,7); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_Northeast:object_events:007` | 8,10 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (8,10); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_Northeast:object_events:008` | 12,8 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (12,8); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_Northeast:object_events:009` | 8,17 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_NUGGET; root Common_EventScript_FindItem; flag FLAG_ITEM_SAFARI_ZONE_NORTH_EAST_NUGGET. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SafariZone_Northeast:bg_events:001` | 31,35 | Hidden ITEM_ULTRA_BALL at (31,35); persistent flag FLAG_HIDDEN_ITEM_SAFARI_ZONE_NORTH_EAST_ULTRA_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `SafariZone_Northeast:bg_events:002` | 21,5 | Hidden ITEM_BIG_NUGGET at (21,5); persistent flag FLAG_HIDDEN_ITEM_SAFARI_ZONE_NORTH_EAST_BIG_NUGGET. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `SafariZone_Northeast:connections:001` | left | left connection to MAP_SAFARI_ZONE_NORTH, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_Northeast:connections:002` | down | down connection to MAP_SAFARI_ZONE_SOUTHEAST, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
