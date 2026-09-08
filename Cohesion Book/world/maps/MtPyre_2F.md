# MtPyre_2F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/MtPyre_2F/map.json) · [Scripts](../../baseline/source/data/maps/MtPyre_2F/scripts.inc)

## Current map contract

`MAP_MT_PYRE_2F` · `LAYOUT_MT_PYRE_2F` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MtPyre_2F:object_events:001` | 3,6 | [MtPyre_2F_EventScript_Mark](../../baseline/source/data/maps/MtPyre_2F/scripts.inc#L20) — MtPyre_2F_EventScript_Mark at (3,6); Hey! Are you searching for POKéMON? /  You came along after me! You're rude! / Ayieeeeh! /  I'm sorry, forgive me, please! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_2F:object_events:002` | 0,10 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ULTRA_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_MT_PYRE_2F_ULTRA_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MtPyre_2F:object_events:003` | 9,3 | [MtPyre_2F_EventScript_Woman](../../baseline/source/data/maps/MtPyre_2F/scripts.inc#L12) — MtPyre_2F_EventScript_Woman at (9,3); Memories of my darling SKITTY… /  My eyes overflow thinking about it. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MtPyre_2F:object_events:004` | 12,10 | [MtPyre_2F_EventScript_PokefanM](../../baseline/source/data/maps/MtPyre_2F/scripts.inc#L16) — MtPyre_2F_EventScript_PokefanM at (12,10); Ooch, ouch… There are holes in the /  ground here and there. //  I didn't notice and took a tumble from /  the floor above. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MtPyre_2F:object_events:005` | 2,9 | [MtPyre_2F_EventScript_Dez](../../baseline/source/data/maps/MtPyre_2F/scripts.inc#L30) — MtPyre_2F_EventScript_Dez at (2,9); DEZ: I came here on a dare with my /  boyfriend. //  It's really scary, but I'm with my /  boyfriend. It's okay. //  I know! I'll get my boyfriend to look /  cool by beating you! / DEZ: Waaaah! I'm scared! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_2F:object_events:006` | 3,9 | [MtPyre_2F_EventScript_Luke](../../baseline/source/data/maps/MtPyre_2F/scripts.inc#L25) — MtPyre_2F_EventScript_Luke at (3,9); LUKE: We're here on a dare. //  Heheh, if I show her how cool I am, /  she'll fall for me. I know it! //  I know! I'll cream you and show her /  how cool I am! / LUKE: Whoopsie! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_2F:object_events:007` | 6,9 | [MtPyre_2F_EventScript_Zander](../../baseline/source/data/maps/MtPyre_2F/scripts.inc#L40) — MtPyre_2F_EventScript_Zander at (6,9); Kiyaaaaah! /  I'm terrified! / Nooooooo! /  I lost my wits! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_2F:object_events:008` | 6,6 | [MtPyre_2F_EventScript_Leah](../../baseline/source/data/maps/MtPyre_2F/scripts.inc#L35) — MtPyre_2F_EventScript_Leah at (6,6); You are an unfamiliar sight… /  Depart before anything befalls you! / Hmm… /  You're durable. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MtPyre_2F:warp_events:001` | 2,1 | Warp from (2,1, elevation 3) to MAP_MT_PYRE_1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_2F:warp_events:002` | 10,1 | Warp from (10,1, elevation 3) to MAP_MT_PYRE_3F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_2F:warp_events:003` | 10,12 | Warp from (10,12, elevation 3) to MAP_MT_PYRE_3F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_2F:warp_events:004` | 6,12 | Warp from (6,12, elevation 3) to MAP_MT_PYRE_3F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_2F:warp_events:005` | 11,9 | Warp from (11,9, elevation 3) to MAP_MT_PYRE_1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_2F:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [CaveHole_CheckFallDownHole](../../baseline/source/data/scripts/cave_hole.inc#L1) — MAP_SCRIPT_ON_FRAME_TABLE calls CaveHole_CheckFallDownHole. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MtPyre_2F:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [CaveHole_FixCrackedGround](../../baseline/source/data/scripts/cave_hole.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls CaveHole_FixCrackedGround. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MtPyre_2F:map_scripts:003` | MAP_SCRIPT_ON_RESUME | [MtPyre_2F_SetHoleWarp](../../baseline/source/data/maps/MtPyre_2F/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls MtPyre_2F_SetHoleWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
