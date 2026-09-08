# SlateportCity_SternsShipyard_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/SlateportCity_SternsShipyard_1F/map.json) · [Scripts](../../baseline/source/data/maps/SlateportCity_SternsShipyard_1F/scripts.inc)

## Current map contract

`MAP_SLATEPORT_CITY_STERNS_SHIPYARD_1F` · `LAYOUT_SLATEPORT_CITY_STERNS_SHIPYARD_1F` · `WEATHER_NONE` · `MUS_SLATEPORT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SlateportCity_SternsShipyard_1F:object_events:001` | 5,5 | [SlateportCity_SternsShipyard_1F_EventScript_Dock](../../baseline/source/data/maps/SlateportCity_SternsShipyard_1F/scripts.inc#L4) — SlateportCity_SternsShipyard_1F_EventScript_Dock at (5,5); The hull fits. The engine fits. /  So why won't these measurements agree? //  If I move this here… /  No, then that won't fit! / Hm? Oh, sorry. I'm DOCK. /  CAPT. STERN asked me to design a ferry. //  Are those the DEVON GOODS? /  He's been waiting for them. //  He's at the OCEANIC MUSEUM. /  Take the parcel straight to him. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SlateportCity_SternsShipyard_1F:object_events:002` | 10,7 | [SlateportCity_SternsShipyard_1F_EventScript_Scientist1](../../baseline/source/data/maps/SlateportCity_SternsShipyard_1F/scripts.inc#L53) — SlateportCity_SternsShipyard_1F_EventScript_Scientist1 at (10,7); The seasons, the weather, where /  the moon sits in the sky… //  These and other conditions make /  the sea change its expression. //  That's right! /  The sea is like a living thing! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_SternsShipyard_1F:object_events:003` | 18,8 | [SlateportCity_SternsShipyard_1F_EventScript_Scientist2](../../baseline/source/data/maps/SlateportCity_SternsShipyard_1F/scripts.inc#L57) — SlateportCity_SternsShipyard_1F_EventScript_Scientist2 at (18,8); I get seasick real easily. /  So I get to help out here instead. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_SternsShipyard_1F:object_events:004` | 12,11 | [SlateportCity_SternsShipyard_1F_EventScript_Briney](../../baseline/source/data/maps/SlateportCity_SternsShipyard_1F/scripts.inc#L61) — SlateportCity_SternsShipyard_1F_EventScript_Briney at (12,11); MR. BRINEY: Ah, {PLAYER}{KUN}! /  It's been too long! //  Aye, since I met you, this old sea dog's /  been feeling frisky! //  So I've decided to help DOCK make /  a ferry. //  Aye, after all, a ferry would be able /  to carry a lot of people. //  But, you know, that DOCK is really /  something special. //  With his knack for technology and /  my experience, I'm sure that we can /  build one great ship, aye! | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `SlateportCity_SternsShipyard_1F:warp_events:001` | 2,14 | Warp from (2,14, elevation 0) to MAP_SLATEPORT_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SlateportCity_SternsShipyard_1F:warp_events:002` | 3,14 | Warp from (3,14, elevation 0) to MAP_SLATEPORT_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SlateportCity_SternsShipyard_1F:warp_events:003` | 3,1 | Warp from (3,1, elevation 0) to MAP_SLATEPORT_CITY_STERNS_SHIPYARD_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
