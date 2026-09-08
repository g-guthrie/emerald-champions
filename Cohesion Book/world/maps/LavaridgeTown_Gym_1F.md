# LavaridgeTown_Gym_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/map.json) · [Scripts](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc)

## Current map contract

`MAP_LAVARIDGE_TOWN_GYM_1F` · `LAYOUT_LAVARIDGE_TOWN_GYM_1F` · `WEATHER_FOG_HORIZONTAL` · `MUS_GYM`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LavaridgeTown_Gym_1F:object_events:001` | 13,9 | [LavaridgeTown_Gym_1F_EventScript_Flannery](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L46) — LavaridgeTown_Gym_1F_EventScript_Flannery at (13,9); I'm FLANNERY, LAVARIDGE's GYM LEADER! //  I used to think Fire meant attacking /  harder. MT. CHIMNEY taught me timing. //  My slowest POKéMON can move first in two /  different ways. Cool the engine early! / You weakened the eruptions, broke the /  Balloon, and changed pace with my heat. //  No type-chart shortcut. Take the /  HEAT BADGE! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_Gym_1F:object_events:002` | 3,14 | [LavaridgeTown_Gym_1F_EventScript_Cole](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L103) — LavaridgeTown_Gym_1F_EventScript_Cole at (3,14); Owowowowow! /  Yikes, it's hot! //  Touch us, and you will feel it! / I'm blinded by sweat in my eyes… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_Gym_1F:object_events:003` | 2,15 | [LavaridgeTown_Gym_1F_EventScript_Gerald](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L129) — LavaridgeTown_Gym_1F_EventScript_Gerald at (2,15); Can your POKéMON withstand /  392-degree heat? //  SUNNY DAY first. Then we run! / It didn't burn hotly enough… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_Gym_1F:object_events:004` | 3,10 | [LavaridgeTown_Gym_1F_EventScript_Axle](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L114) — LavaridgeTown_Gym_1F_EventScript_Axle at (3,10); I'm trying to relieve my stress. /  Don't come along and stress me out! //  Hit me hard. I only grow! / I hope FLANNERY flames you good! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_Gym_1F:object_events:005` | 5,2 | [LavaridgeTown_Gym_1F_EventScript_Danielle](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L124) — LavaridgeTown_Gym_1F_EventScript_Danielle at (5,2); Um… /  Okay, I'll battle with you. //  Dance, and ORICORIO dances too! / Oh, but you're too strong. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_Gym_1F:object_events:006` | 12,16 | [LavaridgeTown_Gym_1F_EventScript_GymGuide](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L149) — LavaridgeTown_Gym_1F_EventScript_GymGuide at (12,16); Hey, how's it going, CHAMPION- /  bound {PLAYER}? //  LAVARIDGE's GYM LEADER FLANNERY /  uses FIRE-type POKéMON. //  Her passion for POKéMON burns stronger /  and hotter than a volcano. //  Her sun weakens WATER and speeds SOLAR /  BEAM, while TRICK ROOM reverses tempo. //  Protect your WATER answer and disrupt /  the field before attacking. / Yow! That was a scorching-hot battle! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LavaridgeTown_Gym_1F:bg_events:001` | 10,15 | [LavaridgeTown_Gym_1F_EventScript_LeftGymStatue](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L162) — LavaridgeTown_Gym_1F_EventScript_LeftGymStatue at (10,15); LAVARIDGE TOWN POKéMON GYM / LAVARIDGE TOWN POKéMON GYM //  FLANNERY'S CERTIFIED TRAINERS: /  {PLAYER} | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LavaridgeTown_Gym_1F:bg_events:002` | 16,15 | [LavaridgeTown_Gym_1F_EventScript_RightGymStatue](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L168) — LavaridgeTown_Gym_1F_EventScript_RightGymStatue at (16,15); LAVARIDGE TOWN POKéMON GYM / LAVARIDGE TOWN POKéMON GYM //  FLANNERY'S CERTIFIED TRAINERS: /  {PLAYER} | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LavaridgeTown_Gym_1F:warp_events:001` | 13,18 | Warp from (13,18, elevation 3) to MAP_LAVARIDGE_TOWN warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:002` | 14,18 | Warp from (14,18, elevation 3) to MAP_LAVARIDGE_TOWN warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:003` | 10,18 | Warp from (10,18, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:004` | 8,9 | Warp from (8,9, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:005` | 4,18 | Warp from (4,18, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:006` | 5,14 | Warp from (5,14, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:007` | 0,17 | Warp from (0,17, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:008` | 5,9 | Warp from (5,9, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:009` | 2,15 | Warp from (2,15, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:010` | 3,14 | Warp from (3,14, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:011` | 1,14 | Warp from (1,14, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:012` | 0,10 | Warp from (0,10, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:013` | 3,10 | Warp from (3,10, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:014` | 0,6 | Warp from (0,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:015` | 3,6 | Warp from (3,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:016` | 5,6 | Warp from (5,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 13. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:017` | 2,3 | Warp from (2,3, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 14. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:018` | 5,2 | Warp from (5,2, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 15. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:019` | 7,2 | Warp from (7,2, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 16. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:020` | 8,6 | Warp from (8,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 17. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:021` | 10,6 | Warp from (10,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 18. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:022` | 4,16 | Warp from (4,16, elevation 0) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 20. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:023` | 12,3 | Warp from (12,3, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 19. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:024` | 14,6 | Warp from (14,6, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 23. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:025` | 13,17 | Warp from (13,17, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 22. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:warp_events:026` | 12,12 | Warp from (12,12, elevation 3) to MAP_LAVARIDGE_TOWN_GYM_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Gym_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LavaridgeTown_Gym_1F_OnTransition](../../baseline/source/data/maps/LavaridgeTown_Gym_1F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls LavaridgeTown_Gym_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
