# LilycoveCity_Harbor

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_Harbor/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_Harbor/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_HARBOR` · `LAYOUT_HARBOR` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_Harbor:object_events:001` | 8,10 | [LilycoveCity_Harbor_EventScript_FerryAttendant](../../baseline/source/data/maps/LilycoveCity_Harbor/scripts.inc#L9) — LilycoveCity_Harbor_EventScript_FerryAttendant at (8,10); I beg your pardon? /  You're looking for a ship? //  I'm sorry, the ferry service isn't /  available at present… / Hello, are you here for the ferry? /  May I confirm your ferry pass? | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `LilycoveCity_Harbor:object_events:002` | 8,9 | Passive/staged OBJ_EVENT_GFX_SS_TIDAL at (8,9); visibility flag FLAG_HIDE_LILYCOVE_HARBOR_SSTIDAL; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `LilycoveCity_Harbor:object_events:003` | 3,13 | [LilycoveCity_Harbor_EventScript_Sailor](../../baseline/source/data/maps/LilycoveCity_Harbor/scripts.inc#L427) — LilycoveCity_Harbor_EventScript_Sailor at (3,13); Until they finish making the ferry, /  we sailors have nothing to do… //  I wish they'd get a move on, the folks /  at the SHIPYARD in SLATEPORT. / CHAMPION! STERN and DEVON approved /  four old exploration passes for you. //  They chart SOUTHERN ISLAND, NAVEL ROCK, /  BIRTH ISLAND, and FARAWAY ISLAND. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_Harbor:object_events:004` | 8,10 | Passive/staged OBJ_EVENT_GFX_SAILOR at (8,10); visibility flag FLAG_HIDE_LILYCOVE_HARBOR_FERRY_SAILOR; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `LilycoveCity_Harbor:object_events:005` | 8,10 | Passive/staged OBJ_EVENT_GFX_EXPERT_M at (8,10); visibility flag FLAG_HIDE_LILYCOVE_HARBOR_EVENT_TICKET_TAKER; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `LilycoveCity_Harbor:warp_events:001` | 11,14 | Warp from (11,14, elevation 0) to MAP_LILYCOVE_CITY warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_Harbor:warp_events:002` | 12,14 | Warp from (12,14, elevation 0) to MAP_LILYCOVE_CITY warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_Harbor:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LilycoveCity_Harbor_OnTransition](../../baseline/source/data/maps/LilycoveCity_Harbor/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls LilycoveCity_Harbor_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
