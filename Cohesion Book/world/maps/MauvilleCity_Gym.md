# MauvilleCity_Gym

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/MauvilleCity_Gym/map.json) · [Scripts](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc)

## Current map contract

`MAP_MAUVILLE_CITY_GYM` · `LAYOUT_MAUVILLE_CITY_GYM` · `WEATHER_NONE` · `MUS_GYM`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MauvilleCity_Gym:object_events:001` | 5,2 | [MauvilleCity_Gym_EventScript_Wattson](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L76) — MauvilleCity_Gym_EventScript_Wattson at (5,2); Wahahahah! I'm WATTSON! //  Electric teams can race forward or close /  the circuit and reverse move order. //  My opening is fixed. My reserves are not. /  Read the board each time it changes! / Wahahah! You solved the circuit instead /  of memorizing it! Take the DYNAMO BADGE! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MauvilleCity_Gym:object_events:002` | 7,8 | [MauvilleCity_Gym_EventScript_Shawn](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L218) — MauvilleCity_Gym_EventScript_Shawn at (7,8); You saved your EARTHQUAKE for me. /  Nothing here stands on the ground. //  Try again, from the air! / You broke our live connection… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MauvilleCity_Gym:object_events:003` | 1,16 | [MauvilleCity_Gym_EventScript_Vivian](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L228) — MauvilleCity_Gym_EventScript_Vivian at (1,16); PLUSLE and MINUN amplify each other /  just by standing side by side. //  Split us, or lose to both! / You found a path around every immunity! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MauvilleCity_Gym:object_events:004` | 5,10 | [MauvilleCity_Gym_EventScript_Ben](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L223) — MauvilleCity_Gym_EventScript_Ben at (5,10); Send current -- LANTURN drinks it. /  Send water -- HELIOLISK drinks that. //  What is left in your bag? / You reversed my reverse current! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MauvilleCity_Gym:object_events:005` | 1,13 | [MauvilleCity_Gym_EventScript_Kirk](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L213) — MauvilleCity_Gym_EventScript_Kirk at (1,13); PINCURCHIN floors the whole stage, /  and TOGEDEMARU takes your bolts. //  Find the current's blind spot! / You shut down my amplifier! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MauvilleCity_Gym:object_events:006` | 7,20 | [MauvilleCity_Gym_EventScript_GymGuide](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L238) — MauvilleCity_Gym_EventScript_GymGuide at (7,20); Hey, how's it going, CHAMPION- /  bound {PLAYER}? //  WATTSON, the LEADER of MAUVILLE /  GYM, uses ELECTRIC-type POKéMON. //  If you challenge him with WATER-type /  POKéMON, he'll zap them! Bzzt! //  And, he's put in switch-controlled /  doors all over his GYM! Eccentric! //  Hey, go for it! / Whoa, you're electrifying! /  You've powered the door open! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MauvilleCity_Gym:object_events:007` | 7,10 | [MauvilleCity_Gym_EventScript_Angelo](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L233) — MauvilleCity_Gym_EventScript_Angelo at (7,10); JOLTIK's THUNDER does not miss, and /  TOXTRICITY only gets louder. //  Accuracy is a luxury. I have it! / Our relay lost its charge… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MauvilleCity_Gym:coord_events:001` | 4,12 | [MauvilleCity_Gym_EventScript_Switch2](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L162) — Coordinate trigger at (4,12); VAR_TEMP_0 == 0 invokes MauvilleCity_Gym_EventScript_Switch2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MauvilleCity_Gym:coord_events:002` | 3,9 | [MauvilleCity_Gym_EventScript_Switch3](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L171) — Coordinate trigger at (3,9); VAR_TEMP_0 == 0 invokes MauvilleCity_Gym_EventScript_Switch3. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MauvilleCity_Gym:coord_events:003` | 0,15 | [MauvilleCity_Gym_EventScript_Switch1](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L153) — Coordinate trigger at (0,15); VAR_TEMP_0 == 0 invokes MauvilleCity_Gym_EventScript_Switch1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MauvilleCity_Gym:coord_events:004` | 8,9 | [MauvilleCity_Gym_EventScript_Switch4](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L180) — Coordinate trigger at (8,9); VAR_TEMP_0 == 0 invokes MauvilleCity_Gym_EventScript_Switch4. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MauvilleCity_Gym:bg_events:001` | 3,18 | [MauvilleCity_Gym_EventScript_LeftGymStatue](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L251) — MauvilleCity_Gym_EventScript_LeftGymStatue at (3,18); MAUVILLE CITY POKéMON GYM / MAUVILLE CITY POKéMON GYM //  WATTSON'S CERTIFIED TRAINERS: /  {PLAYER} | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MauvilleCity_Gym:bg_events:002` | 6,18 | [MauvilleCity_Gym_EventScript_RightGymStatue](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L257) — MauvilleCity_Gym_EventScript_RightGymStatue at (6,18); MAUVILLE CITY POKéMON GYM / MAUVILLE CITY POKéMON GYM //  WATTSON'S CERTIFIED TRAINERS: /  {PLAYER} | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MauvilleCity_Gym:warp_events:001` | 4,20 | Warp from (4,20, elevation 0) to MAP_MAUVILLE_CITY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MauvilleCity_Gym:warp_events:002` | 5,20 | Warp from (5,20, elevation 0) to MAP_MAUVILLE_CITY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MauvilleCity_Gym:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [MauvilleCity_Gym_OnLoad](../../baseline/source/data/maps/MauvilleCity_Gym/scripts.inc#L5) — MAP_SCRIPT_ON_LOAD calls MauvilleCity_Gym_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
