# SafariZone_Southeast

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/SafariZone_Southeast/map.json) · [Scripts](../../baseline/source/data/maps/SafariZone_Southeast/scripts.inc)

## Current map contract

`MAP_SAFARI_ZONE_SOUTHEAST` · `LAYOUT_SAFARI_ZONE_SOUTHEAST` · `WEATHER_NONE` · `MUS_SAFARI_ZONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SafariZone_Southeast:object_events:001` | 7,7 | [SafariZone_Southeast_EventScript_RichBoy](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L136) — SafariZone_Southeast_EventScript_RichBoy at (7,7); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_Southeast:object_events:002` | 20,30 | [SafariZone_Southeast_EventScript_FatMan](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L132) — SafariZone_Southeast_EventScript_FatMan at (20,30); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_Southeast:object_events:003` | 8,26 | [SafariZone_Southeast_EventScript_LittleGirl](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L128) — SafariZone_Southeast_EventScript_LittleGirl at (8,26); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_Southeast:object_events:004` | 31,15 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BIG_PEARL; root Common_EventScript_FindItem; flag FLAG_ITEM_SAFARI_ZONE_SOUTH_EAST_BIG_PEARL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SafariZone_Southeast:object_events:005` | 12,16 | [SafariZone_Southeast_EventScript_ExpansionZoneAttendant](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L120) — SafariZone_Southeast_EventScript_ExpansionZoneAttendant at (12,16); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_Southeast:bg_events:001` | 19,36 | Hidden ITEM_PP_UP at (19,36); persistent flag FLAG_HIDDEN_ITEM_SAFARI_ZONE_SOUTH_EAST_PP_UP. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `SafariZone_Southeast:bg_events:002` | 32,33 | Hidden ITEM_FULL_RESTORE at (32,33); persistent flag FLAG_HIDDEN_ITEM_SAFARI_ZONE_SOUTH_EAST_FULL_RESTORE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `SafariZone_Southeast:connections:001` | left | left connection to MAP_SAFARI_ZONE_SOUTH, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_Southeast:connections:002` | up | up connection to MAP_SAFARI_ZONE_NORTHEAST, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
