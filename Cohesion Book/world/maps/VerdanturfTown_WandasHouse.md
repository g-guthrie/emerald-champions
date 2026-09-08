# VerdanturfTown_WandasHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/VerdanturfTown_WandasHouse/map.json) · [Scripts](../../baseline/source/data/maps/VerdanturfTown_WandasHouse/scripts.inc)

## Current map contract

`MAP_VERDANTURF_TOWN_WANDAS_HOUSE` · `LAYOUT_VERDANTURF_TOWN_WANDAS_HOUSE` · `WEATHER_NONE` · `MUS_VERDANTURF`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VerdanturfTown_WandasHouse:object_events:001` | 14,5 | [VerdanturfTown_WandasHouse_EventScript_Wally](../../baseline/source/data/maps/VerdanturfTown_WandasHouse/scripts.inc#L4) — VerdanturfTown_WandasHouse_EventScript_Wally at (14,5); WALLY: I lost to you, {PLAYER}, but I'm /  not feeling down anymore. //  Because I have a new purpose in life. /  Together with my RALTS, I'm going /  to challenge POKéMON GYMS and become /  a great TRAINER. //  Please watch me, {PLAYER}. /  I'm going to be stronger than you. //  When I do, I'm going to challenge you /  to another battle. / WALLY: Please watch me, {PLAYER}. /  I'm going to get stronger than you. //  When I do, I'm going to challenge you /  to another battle. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `VerdanturfTown_WandasHouse:object_events:002` | 5,4 | [VerdanturfTown_WandasHouse_EventScript_WandasBoyfriend](../../baseline/source/data/maps/VerdanturfTown_WandasHouse/scripts.inc#L37) — VerdanturfTown_WandasHouse_EventScript_WandasBoyfriend at (5,4); Thanks to you, I can see my girlfriend /  every day. /  Happy? You bet I am! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown_WandasHouse:object_events:003` | 7,2 | [VerdanturfTown_WandasHouse_EventScript_WallysUncle](../../baseline/source/data/maps/VerdanturfTown_WandasHouse/scripts.inc#L18) — VerdanturfTown_WandasHouse_EventScript_WallysUncle at (7,2); UNCLE: Oh! {PLAYER}{KUN}! /  WALLY's next door. //  But, boy, there's something I have to /  tell you. //  This natural environment is doing /  wonders for WALLY's health. //  Maybe it's not just the environment. /  It could be POKéMON that are giving /  the boy hope. / WALLY's gone away… /  He slipped off on his own… | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `VerdanturfTown_WandasHouse:object_events:004` | 2,4 | [VerdanturfTown_WandasHouse_EventScript_MegaGift_GARDEVOIRITE](../../baseline/source/data/maps/VerdanturfTown_WandasHouse/scripts.inc#L170) — VerdanturfTown_WandasHouse_EventScript_MegaGift_GARDEVOIRITE at (2,4); My daughter's boyfriend is a very /  driven and passionate sort of person. //  He's been digging a tunnel nonstop /  just so he can see my daughter. //  My daughter's a little concerned, /  so she goes out to the tunnel a lot. / It's amazing. My daughter's boyfriend /  was digging the tunnel by hand! //  It's so incredible! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `VerdanturfTown_WandasHouse:object_events:005` | 5,5 | [VerdanturfTown_WandasHouse_EventScript_Wanda](../../baseline/source/data/maps/VerdanturfTown_WandasHouse/scripts.inc#L41) — VerdanturfTown_WandasHouse_EventScript_Wanda at (5,5); WANDA: You are? /  Oh, right, I get it! //  You're the {PLAYER} who WALLY was /  telling me about. //  I'm WALLY's cousin. /  Glad to meet you! //  I think WALLY's become a lot more lively /  and healthy since he came here. / WANDA: Don't worry about WALLY. /  He'll be just fine. //  I know my little cousin, and he has /  POKéMON with him, too. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `VerdanturfTown_WandasHouse:warp_events:001` | 7,7 | Warp from (7,7, elevation 0) to MAP_VERDANTURF_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown_WandasHouse:warp_events:002` | 8,7 | Warp from (8,7, elevation 0) to MAP_VERDANTURF_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
