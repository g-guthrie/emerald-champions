# AbandonedShip_CaptainsOffice

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AbandonedShip_CaptainsOffice/map.json) · [Scripts](../../baseline/source/data/maps/AbandonedShip_CaptainsOffice/scripts.inc)

## Current map contract

`MAP_ABANDONED_SHIP_CAPTAINS_OFFICE` · `LAYOUT_ABANDONED_SHIP_CAPTAINS_OFFICE` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AbandonedShip_CaptainsOffice:object_events:001` | 3,4 | [AbandonedShip_CaptainsOffice_EventScript_CaptSternAide](../../baseline/source/data/maps/AbandonedShip_CaptainsOffice/scripts.inc#L4) — AbandonedShip_CaptainsOffice_EventScript_CaptSternAide at (3,4); I'm investigating this ship on behalf /  of CAPT. STERN. //  He also asked me to find a SCANNER, /  but I haven't had any success… / Oh! That's a SCANNER! //  Listen, can I get you to deliver that /  to CAPT. STERN? //  I want to investigate this ship a /  little more. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `AbandonedShip_CaptainsOffice:object_events:002` | 0,6 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_STORAGE_KEY; root Common_EventScript_FindItem; flag FLAG_ITEM_ABANDONED_SHIP_CAPTAINS_OFFICE_STORAGE_KEY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AbandonedShip_CaptainsOffice:warp_events:001` | 7,6 | Warp from (7,6, elevation 3) to MAP_ABANDONED_SHIP_DECK warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_CaptainsOffice:warp_events:002` | 8,6 | Warp from (8,6, elevation 3) to MAP_ABANDONED_SHIP_DECK warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
