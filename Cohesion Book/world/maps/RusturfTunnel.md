# RusturfTunnel

**REVISE.** Keep the sound-sensitive tunnel, hostage rescue, lovers’ reunion and meaningful shortcut. Habitat restoration is decided globally; do not change party access or silently revive retired tunnel trainers.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RusturfTunnel/map.json) · [Scripts](../../baseline/source/data/maps/RusturfTunnel/scripts.inc)

## Current map contract

`MAP_RUSTURF_TUNNEL` · `LAYOUT_RUSTURF_TUNNEL` · `WEATHER_FOG_HORIZONTAL` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RusturfTunnel:object_events:001` | 23,5 | [RusturfTunnel_EventScript_WandasBoyfriend](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L30) — RusturfTunnel_EventScript_WandasBoyfriend at (23,5); … … //  Why can't they keep digging? /  Is the bedrock too hard? //  My beloved awaits me in VERDANTURF /  TOWN just beyond here… //  If RUSTBORO and VERDANTURF were /  joined by this tunnel, I could visit /  her every day… //  But this… /  What am I to do? / To get from RUSTBORO to VERDANTURF, /  you need to go to DEWFORD, then pass /  through SLATEPORT and MAUVILLE… | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RusturfTunnel:object_events:002` | 24,5 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (24,5); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `RusturfTunnel:object_events:003` | 3,1 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_POKE_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_RUSTURF_TUNNEL_POKE_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `RusturfTunnel:object_events:004` | 30,2 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAX_ETHER; root Common_EventScript_FindItem; flag FLAG_ITEM_RUSTURF_TUNNEL_MAX_ETHER. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `RusturfTunnel:object_events:005` | 5,4 | Passive/staged OBJ_EVENT_GFX_EXPERT_M at (5,4); visibility flag FLAG_HIDE_RUSTURF_TUNNEL_BRINEY; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `RusturfTunnel:object_events:006` | 14,5 | [RusturfTunnel_EventScript_Grunt](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L300) — RusturfTunnel_EventScript_Grunt at (14,5); Grah, keelhaul it all! //  The hostage POKéMON turned out to be a /  useless little bird, and now I'm stuck /  in a tunnel to nowhere! //  Hey! You! You want the package? Come /  and take it from DONDOZO! / Urrrggh! My career in crime comes to /  a dead end! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `RusturfTunnel:object_events:007` | 14,4 | [RusturfTunnel_EventScript_Peeko](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L290) — RusturfTunnel_EventScript_Peeko at (14,4); PEEKO: Pii pihyoh! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RusturfTunnel:object_events:008` | 24,4 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (24,4); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `RusturfTunnel:object_events:009` | 32,13 | [RusturfTunnel_EventScript_Mike](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L432) — RusturfTunnel_EventScript_Mike at (32,13); What do you call a wild man up in the /  mountains? A mountain man, right? //  So why don't they call a POKéMON in /  the mountains a mountain POKéMON? / My POKéMON… /  Ran out of power… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `RusturfTunnel:object_events:010` | 25,4 | [RusturfTunnel_EventScript_Wanda](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L20) — RusturfTunnel_EventScript_Wanda at (25,4); On the other side of this rock… /  My boyfriend is there. //  He… He's not just digging the tunnel /  to come see me. //  He works his hands raw and rough /  for the benefit of everyone. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RusturfTunnel:coord_events:001` | 23,4 | [RusturfTunnel_EventScript_TunnelBlockagePos1](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L258) — Coordinate trigger at (23,4); TRIGGER_RUN_IMMEDIATELY == 0 invokes RusturfTunnel_EventScript_TunnelBlockagePos1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `RusturfTunnel:coord_events:002` | 9,4 | [RusturfTunnel_EventScript_AquaGruntBackUp](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L270) — Coordinate trigger at (9,4); VAR_RUSTURF_TUNNEL_STATE == 2 invokes RusturfTunnel_EventScript_AquaGruntBackUp. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `RusturfTunnel:coord_events:003` | 9,5 | [RusturfTunnel_EventScript_AquaGruntBackUp](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L270) — Coordinate trigger at (9,5); VAR_RUSTURF_TUNNEL_STATE == 2 invokes RusturfTunnel_EventScript_AquaGruntBackUp. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `RusturfTunnel:coord_events:004` | 25,4 | [RusturfTunnel_EventScript_TunnelBlockagePos2](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L262) — Coordinate trigger at (25,4); TRIGGER_RUN_IMMEDIATELY == 0 invokes RusturfTunnel_EventScript_TunnelBlockagePos2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `RusturfTunnel:coord_events:005` | 25,5 | [RusturfTunnel_EventScript_TunnelBlockagePos3](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L266) — Coordinate trigger at (25,5); TRIGGER_RUN_IMMEDIATELY == 0 invokes RusturfTunnel_EventScript_TunnelBlockagePos3. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `RusturfTunnel:warp_events:001` | 4,10 | Warp from (4,10, elevation 3) to MAP_ROUTE116 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RusturfTunnel:warp_events:002` | 29,16 | Warp from (29,16, elevation 3) to MAP_VERDANTURF_TOWN warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RusturfTunnel:warp_events:003` | 18,20 | Warp from (18,20, elevation 3) to MAP_ROUTE116 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RusturfTunnel:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [RusturfTunnel_OnTransition](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L11) — MAP_SCRIPT_ON_TRANSITION calls RusturfTunnel_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `RusturfTunnel:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [RusturfTunnel_OnFrame](../../baseline/source/data/maps/RusturfTunnel/scripts.inc#L6) — MAP_SCRIPT_ON_FRAME_TABLE calls RusturfTunnel_OnFrame. | **REPAIR** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · W-FIELD-CLUES |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
