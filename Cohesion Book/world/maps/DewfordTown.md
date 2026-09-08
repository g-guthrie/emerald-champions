# DewfordTown

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/DewfordTown/map.json) · [Scripts](../../baseline/source/data/maps/DewfordTown/scripts.inc)

## Current map contract

`MAP_DEWFORD_TOWN` · `LAYOUT_DEWFORD_TOWN` · `WEATHER_SUNNY` · `MUS_DEWFORD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `DewfordTown:object_events:001` | 7,12 | [DewfordTown_EventScript_Woman](../../baseline/source/data/maps/DewfordTown/scripts.inc#L75) — DewfordTown_EventScript_Woman at (7,12); The meadow west of town reopened after /  the cave began shining. //  An abandoned manor there holds DEVON's /  oldest records of MEGA EVOLUTION. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `DewfordTown:object_events:002` | 12,9 | [DewfordTown_EventScript_Briney](../../baseline/source/data/maps/DewfordTown/scripts.inc#L24) — DewfordTown_EventScript_Briney at (12,9); MR. BRINEY: Have you delivered your /  LETTER? //  Or were you meaning to sail back to /  PETALBURG? / MR. BRINEY: PETALBURG it is, then! //  Anchors aweigh! /  PEEKO, we're setting sail, my darling! | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `DewfordTown:object_events:003` | 12,14 | [DewfordTown_EventScript_OldRodFisherman](../../baseline/source/data/maps/DewfordTown/scripts.inc#L93) — DewfordTown_EventScript_OldRodFisherman at (12,14); This is a renowned fishing spot. /  Are you getting the itch to fish? / I hear you, and I like what /  you're saying! //  I'll give you one of my fishing RODS. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `DewfordTown:object_events:004` | 12,8 | Passive/staged OBJ_EVENT_GFX_MR_BRINEYS_BOAT at (12,8); visibility flag FLAG_HIDE_MR_BRINEY_BOAT_DEWFORD_TOWN; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `DewfordTown:object_events:005` | 1,6 | [DewfordTown_EventScript_TrendyPhraseBoy](../../baseline/source/data/maps/DewfordTown/scripts.inc#L610) — DewfordTown_EventScript_TrendyPhraseBoy at (1,6); I like what's hip, happening, and trendy. /  I'm always checking it out. //  Listen, have you heard about this new /  “{STR_VAR_1}”? //  That's right! /  Of course you know! //  I mean, sheesh, /  “{STR_VAR_1}”… /  It's the hottest thing in cool! //  Wherever you're from, /  “{STR_VAR_1}” /  is the biggest happening thing, right? / Hunh? /  It's not the hip and happening thing? //  Well, hey, you have to tell me, /  what's new and what's “in”? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `DewfordTown:object_events:006` | 8,18 | [DewfordTown_EventScript_GymGuide](../../baseline/source/data/maps/DewfordTown/scripts.inc#L5) — DewfordTown_EventScript_GymGuide at (8,18); Hey, Champ-to-be! //  BRAWLY, the GYM LEADER here, went /  to SLATEPORT's OCEANIC MUSEUM. //  Look for him in the queue outside. /  He needs a nudge to come home! //  MR. BRINEY can sail you there once /  you deliver STEVEN's LETTER. //  Find STEVEN in GRANITE CAVE, /  northwest of DEWFORD. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `DewfordTown:bg_events:001` | 10,10 | [DewfordTown_EventScript_TownSign](../../baseline/source/data/maps/DewfordTown/scripts.inc#L81) — DewfordTown_EventScript_TownSign at (10,10); DEWFORD TOWN /  “A tiny island in the blue sea.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `DewfordTown:bg_events:002` | 11,16 | [DewfordTown_EventScript_GymSign](../../baseline/source/data/maps/DewfordTown/scripts.inc#L85) — DewfordTown_EventScript_GymSign at (11,16); DEWFORD TOWN POKéMON GYM /  LEADER: BRAWLY /  “A big wave in fighting!” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `DewfordTown:bg_events:003` | 4,10 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (4,10); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `DewfordTown:bg_events:004` | 3,10 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (3,10); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `DewfordTown:bg_events:005` | 2,4 | [DewfordTown_EventScript_HallSign](../../baseline/source/data/maps/DewfordTown/scripts.inc#L89) — DewfordTown_EventScript_HallSign at (2,4); DEWFORD HALL /  “Everyone's information exchange!” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `DewfordTown:warp_events:001` | 3,3 | Warp from (3,3, elevation 0) to MAP_DEWFORD_TOWN_HALL warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordTown:warp_events:002` | 2,10 | Warp from (2,10, elevation 0) to MAP_DEWFORD_TOWN_POKEMON_CENTER_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordTown:warp_events:003` | 8,17 | Warp from (8,17, elevation 0) to MAP_DEWFORD_TOWN_GYM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordTown:warp_events:004` | 17,14 | Warp from (17,14, elevation 0) to MAP_DEWFORD_TOWN_HOUSE1 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordTown:warp_events:005` | 8,8 | Warp from (8,8, elevation 0) to MAP_DEWFORD_TOWN_HOUSE2 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordTown:connections:001` | up | up connection to MAP_ROUTE106, offset -60. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordTown:connections:002` | right | right connection to MAP_ROUTE107, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordTown:connections:003` | left | left connection to MAP_DEWFORD_MEADOW, offset 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordTown:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [DewfordTown_OnTransition](../../baseline/source/data/maps/DewfordTown/scripts.inc#L20) — MAP_SCRIPT_ON_TRANSITION calls DewfordTown_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
