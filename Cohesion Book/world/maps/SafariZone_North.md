# SafariZone_North

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/SafariZone_North/map.json) · [Scripts](../../baseline/source/data/maps/SafariZone_North/scripts.inc)

## Current map contract

`MAP_SAFARI_ZONE_NORTH` · `LAYOUT_SAFARI_ZONE_NORTH` · `WEATHER_NONE` · `MUS_SAFARI_ZONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SafariZone_North:object_events:001` | 22,9 | [SafariZone_North_EventScript_Fisherman](../../baseline/source/data/maps/SafariZone_North/scripts.inc#L4) — SafariZone_North_EventScript_Fisherman at (22,9); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_North:object_events:002` | 6,29 | [SafariZone_North_EventScript_Man](../../baseline/source/data/maps/SafariZone_North/scripts.inc#L8) — SafariZone_North_EventScript_Man at (6,29); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_North:object_events:003` | 25,10 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (25,10); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_North:object_events:004` | 25,13 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (25,13); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_North:object_events:005` | 28,14 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (28,14); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_North:object_events:006` | 23,6 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (23,6); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_North:object_events:007` | 20,7 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (20,7); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_North:object_events:008` | 27,7 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (27,7); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SafariZone_North:object_events:009` | 7,6 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_NUGGET; root Common_EventScript_FindItem; flag FLAG_ITEM_SAFARI_ZONE_NORTH_NUGGET. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SafariZone_North:connections:001` | left | left connection to MAP_SAFARI_ZONE_NORTHWEST, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_North:connections:002` | down | down connection to MAP_SAFARI_ZONE_SOUTH, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_North:connections:003` | right | right connection to MAP_SAFARI_ZONE_NORTHEAST, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
