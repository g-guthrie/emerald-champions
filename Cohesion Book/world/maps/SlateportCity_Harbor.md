# SlateportCity_Harbor

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/SlateportCity_Harbor/map.json) · [Scripts](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc)

## Current map contract

`MAP_SLATEPORT_CITY_HARBOR` · `LAYOUT_HARBOR` · `WEATHER_NONE` · `MUS_SLATEPORT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SlateportCity_Harbor:object_events:001` | 8,10 | [SlateportCity_Harbor_EventScript_FerryAttendant](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L157) — SlateportCity_Harbor_EventScript_FerryAttendant at (8,10); I beg your pardon? /  You're looking for a ship? //  I'm sorry, the ferry service isn't /  available at present… / Hello, are you here for the ferry? /  May I confirm your ferry pass? | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `SlateportCity_Harbor:object_events:002` | 4,12 | [SlateportCity_Harbor_EventScript_Sailor](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L263) — SlateportCity_Harbor_EventScript_Sailor at (4,12); A journey to the bottom of the sea… /  I wonder what it'd be like? //  I'd love to go deep underwater like /  that someday. / For a ship to sail safely, we need to /  know about the weather! //  Speaking of weather, I heard something /  from a guy at the WEATHER INSTITUTE. //  He was saying abnormal weather has /  been reported all over the place! //  You should visit the WEATHER INSTITUTE /  and ask around! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SlateportCity_Harbor:object_events:003` | 1,12 | [SlateportCity_Harbor_EventScript_FatMan](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L284) — SlateportCity_Harbor_EventScript_FatMan at (1,12); I wanted to go with CAPT. STERN on /  the ocean floor exploration. //  But the sub's too small for me. //  If I squeezed in, there wouldn't be /  any room for the CAPTAIN… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SlateportCity_Harbor:object_events:004` | 6,13 | [SlateportCity_Harbor_EventScript_CaptStern](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L288) — SlateportCity_Harbor_EventScript_CaptStern at (6,13); CAPT. STERN: Those thugs… //  They're the same lot who tried to rob /  the DEVON GOODS at the MUSEUM. / CAPT. STERN: They have my submarine. /  The cavern charts are aboard, too. //  ARCHIE named LILYCOVE. Reach the city /  by heading east from ROUTE 121. //  Find LILYCOVE's eastern beach. SURF /  north to TEAM AQUA's cave hideout. //  Stop their launch before they reach /  the sleeping POKéMON on the seafloor. | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FIELD-CLUES |
| `SlateportCity_Harbor:object_events:005` | 8,9 | Passive/staged OBJ_EVENT_GFX_SS_TIDAL at (8,9); visibility flag FLAG_HIDE_SLATEPORT_CITY_HARBOR_SS_TIDAL; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SlateportCity_Harbor:object_events:006` | 7,10 | Passive/staged OBJ_EVENT_GFX_AQUA_MEMBER_M at (7,10); visibility flag FLAG_HIDE_SLATEPORT_CITY_HARBOR_AQUA_GRUNT; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SlateportCity_Harbor:object_events:007` | 8,10 | Passive/staged OBJ_EVENT_GFX_ARCHIE at (8,10); visibility flag FLAG_HIDE_SLATEPORT_CITY_HARBOR_ARCHIE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SlateportCity_Harbor:object_events:008` | 7,9 | Passive/staged OBJ_EVENT_GFX_SUBMARINE_SHADOW at (7,9); visibility flag FLAG_HIDE_SLATEPORT_CITY_HARBOR_SUBMARINE_SHADOW; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SlateportCity_Harbor:coord_events:001` | 8,11 | [SlateportCity_Harbor_EventScript_AquaEscapeTrigger0](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L23) — Coordinate trigger at (8,11); VAR_SLATEPORT_HARBOR_STATE == 1 invokes SlateportCity_Harbor_EventScript_AquaEscapeTrigger0. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `SlateportCity_Harbor:coord_events:002` | 8,12 | [SlateportCity_Harbor_EventScript_AquaEscapeTrigger1](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L29) — Coordinate trigger at (8,12); VAR_SLATEPORT_HARBOR_STATE == 1 invokes SlateportCity_Harbor_EventScript_AquaEscapeTrigger1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `SlateportCity_Harbor:coord_events:003` | 8,13 | [SlateportCity_Harbor_EventScript_AquaEscapeTrigger2](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L35) — Coordinate trigger at (8,13); VAR_SLATEPORT_HARBOR_STATE == 1 invokes SlateportCity_Harbor_EventScript_AquaEscapeTrigger2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `SlateportCity_Harbor:coord_events:004` | 8,14 | [SlateportCity_Harbor_EventScript_AquaEscapeTrigger3](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L41) — Coordinate trigger at (8,14); VAR_SLATEPORT_HARBOR_STATE == 1 invokes SlateportCity_Harbor_EventScript_AquaEscapeTrigger3. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `SlateportCity_Harbor:warp_events:001` | 11,14 | Warp from (11,14, elevation 0) to MAP_SLATEPORT_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SlateportCity_Harbor:warp_events:002` | 12,14 | Warp from (12,14, elevation 0) to MAP_SLATEPORT_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SlateportCity_Harbor:warp_events:003` | 19,15 | Warp from (19,15, elevation 0) to MAP_SLATEPORT_CITY warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SlateportCity_Harbor:warp_events:004` | 20,15 | Warp from (20,15, elevation 0) to MAP_SLATEPORT_CITY warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SlateportCity_Harbor:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [SlateportCity_Harbor_OnTransition](../../baseline/source/data/maps/SlateportCity_Harbor/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls SlateportCity_Harbor_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
