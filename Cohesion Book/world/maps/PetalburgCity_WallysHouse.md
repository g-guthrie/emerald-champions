# PetalburgCity_WallysHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/PetalburgCity_WallysHouse/map.json) · [Scripts](../../baseline/source/data/maps/PetalburgCity_WallysHouse/scripts.inc)

## Current map contract

`MAP_PETALBURG_CITY_WALLYS_HOUSE` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_PETALBURG`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PetalburgCity_WallysHouse:object_events:001` | 3,4 | [PetalburgCity_WallysHouse_EventScript_WallysDad](../../baseline/source/data/maps/PetalburgCity_WallysHouse/scripts.inc#L30) — PetalburgCity_WallysHouse_EventScript_WallysDad at (3,4); You're… /  Ah, you must be {PLAYER}{KUN}, right? //  Thank you for playing with WALLY a /  little while ago. //  He's been frail and sickly ever /  since he was a baby. //  We've sent him to stay with my relatives /  in VERDANTURF TOWN for a while. //  The air is a lot cleaner there /  than it is here. //  What's that? Where's WALLY? /  He's already left, our WALLY. //  I wonder where he could have /  gotten by now? / I wonder how our WALLY is doing? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PetalburgCity_WallysHouse:object_events:002` | 7,5 | [PetalburgCity_WallysHouse_EventScript_WallysMom](../../baseline/source/data/maps/PetalburgCity_WallysHouse/scripts.inc#L62) — PetalburgCity_WallysHouse_EventScript_WallysMom at (7,5); WALLY was really happy when he told /  us that he caught a POKéMON. //  It's been ages since I've seen him /  smile like that. / I want you to keep this a secret /  from my husband… //  But our WALLY left VERDANTURF TOWN /  without telling anyone. //  You know, WALLY is frail, but /  he's surprisingly strong-willed. //  I'm sure that he'll come back safe /  and sound one day! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PetalburgCity_WallysHouse:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_PETALBURG_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity_WallysHouse:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_PETALBURG_CITY warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity_WallysHouse:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [PetalburgCity_WallysHouse_OnFrame](../../baseline/source/data/maps/PetalburgCity_WallysHouse/scripts.inc#L15) — MAP_SCRIPT_ON_FRAME_TABLE calls PetalburgCity_WallysHouse_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `PetalburgCity_WallysHouse:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [PetalburgCity_WallysHouse_OnWarp](../../baseline/source/data/maps/PetalburgCity_WallysHouse/scripts.inc#L6) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls PetalburgCity_WallysHouse_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
