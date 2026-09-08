# MeteorFalls_1F_1R

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/MeteorFalls_1F_1R/map.json) · [Scripts](../../baseline/source/data/maps/MeteorFalls_1F_1R/scripts.inc)

## Current map contract

`MAP_METEOR_FALLS_1F_1R` · `LAYOUT_METEOR_FALLS_1F_1R` · `WEATHER_NONE` · `MUS_CAVE_OF_ORIGIN`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MeteorFalls_1F_1R:object_events:001` | 2,4 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_AGGRONITE; root Common_EventScript_FindItem; flag FLAG_ITEM_METEOR_FALLS_1F_1R_AGGRONITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MeteorFalls_1F_1R:object_events:002` | 2,14 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MOON_STONE; root Common_EventScript_FindItem; flag FLAG_ITEM_METEOR_FALLS_1F_1R_MOON_STONE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MeteorFalls_1F_1R:object_events:003` | 27,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_METAGROSSITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_ROUTE124_METAGROSSITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MeteorFalls_1F_1R:object_events:004` | 26,32 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SALAMENCITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_SALAMENCITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MeteorFalls_1F_1R:object_events:005` | 11,20 | Passive/staged OBJ_EVENT_GFX_MAGMA_MEMBER_M at (11,20); visibility flag FLAG_HIDE_METEOR_FALLS_TEAM_MAGMA; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `MeteorFalls_1F_1R:object_events:006` | 12,21 | Passive/staged OBJ_EVENT_GFX_MAGMA_MEMBER_F at (12,21); visibility flag FLAG_HIDE_METEOR_FALLS_TEAM_MAGMA; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `MeteorFalls_1F_1R:object_events:007` | 4,21 | Passive/staged OBJ_EVENT_GFX_ARCHIE at (4,21); visibility flag FLAG_HIDE_METEOR_FALLS_TEAM_AQUA; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `MeteorFalls_1F_1R:object_events:008` | 4,20 | Passive/staged OBJ_EVENT_GFX_AQUA_MEMBER_M at (4,20); visibility flag FLAG_HIDE_METEOR_FALLS_TEAM_AQUA; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `MeteorFalls_1F_1R:object_events:009` | 4,21 | Passive/staged OBJ_EVENT_GFX_AQUA_MEMBER_M at (4,21); visibility flag FLAG_HIDE_METEOR_FALLS_TEAM_AQUA; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `MeteorFalls_1F_1R:object_events:010` | 11,22 | [MeteorFalls_1F_1R_EventScript_ProfCozmo](../../baseline/source/data/maps/MeteorFalls_1F_1R/scripts.inc#L286) — MeteorFalls_1F_1R_EventScript_ProfCozmo at (11,22); I'm PROF. COZMO. MAGMA asked me to map /  the METEORITE's resonance here. //  I thought it was geological research. /  They took the sample and my readings. //  The pattern matches MT. CHIMNEY's fault /  line and those strange marks in HOENN. //  If they amplify it at the summit, the /  whole network may answer at once. / PROF. COZMO: The METEORITE and the /  fault line share one resonance. //  Please stop MAGMA before they force the /  entire network awake. //  ROUTE 112's cable car will take you /  to MT. CHIMNEY's summit. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MeteorFalls_1F_1R:object_events:011` | 14,21 | [MeteorFalls_1F_1R_EventScript_RivalTalkAfterBattle](../../baseline/source/data/maps/MeteorFalls_1F_1R/scripts.inc#L5) — MeteorFalls_1F_1R_EventScript_RivalTalkAfterBattle at (14,21); MAGMA took the METEORITE toward /  MT. CHIMNEY. We have to follow. //  Take ROUTE 112's cable car /  up to the summit. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MeteorFalls_1F_1R:coord_events:001` | 14,18 | [MeteorFalls_1F_1R_EventScript_MagmaStealsMeteoriteScene](../../baseline/source/data/maps/MeteorFalls_1F_1R/scripts.inc#L29) — Coordinate trigger at (14,18); VAR_METEOR_FALLS_STATE == 0 invokes MeteorFalls_1F_1R_EventScript_MagmaStealsMeteoriteScene. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MeteorFalls_1F_1R:warp_events:001` | 27,18 | Warp from (27,18, elevation 4) to MAP_ROUTE114 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_1F_1R:warp_events:002` | 6,39 | Warp from (6,39, elevation 3) to MAP_ROUTE115 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_1F_1R:warp_events:003` | 10,3 | Warp from (10,3, elevation 3) to MAP_METEOR_FALLS_1F_2R warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_1F_1R:warp_events:004` | 5,4 | Warp from (5,4, elevation 3) to MAP_METEOR_FALLS_B1F_1R warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_1F_1R:warp_events:005` | 26,28 | Warp from (26,28, elevation 3) to MAP_METEOR_FALLS_B1F_1R warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_1F_1R:warp_events:006` | 4,2 | Warp from (4,2, elevation 0) to MAP_METEOR_FALLS_STEVENS_CAVE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MeteorFalls_1F_1R:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [MeteorFalls_1F_1R_OnLoad](../../baseline/source/data/maps/MeteorFalls_1F_1R/scripts.inc#L15) — MAP_SCRIPT_ON_LOAD calls MeteorFalls_1F_1R_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
