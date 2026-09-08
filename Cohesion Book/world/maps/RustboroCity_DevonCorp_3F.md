# RustboroCity_DevonCorp_3F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_DEVON_CORP_3F` · `LAYOUT_RUSTBORO_CITY_DEVON_CORP_3F` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_DevonCorp_3F:object_events:001` | 17,5 | [RustboroCity_DevonCorp_3F_EventScript_MrStone](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L164) — RustboroCity_DevonCorp_3F_EventScript_MrStone at (17,5); You'll find STEVEN in GRANITE CAVE, /  near DEWFORD. He often loses track /  of time when he's studying rocks. //  Our researcher on the second floor /  keeps a guide to the LEGENDARY SIGNS. /  Ask him whenever you want a lead. / STEVEN received my LETTER. Good. //  He will entrust the MEGA RING only after /  you earn DEWFORD's KNUCKLE BADGE. | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-MEGA-GUIDE |
| `RustboroCity_DevonCorp_3F:object_events:002` | 3,5 | [RustboroCity_DevonCorp_3F_EventScript_Employee](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L210) — RustboroCity_DevonCorp_3F_EventScript_Employee at (3,5); If you visit the SHIPYARD in SLATEPORT, /  you should go see CAPT. STERN. / DEVON's new products, the REPEAT BALL /  and TIMER BALL, have become hugely /  popular among TRAINERS. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RustboroCity_DevonCorp_3F:object_events:003` | 15,5 | [RustboroCity_DevonCorp_3F_EventScript_MrStone](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L164) — RustboroCity_DevonCorp_3F_EventScript_MrStone at (15,5); You'll find STEVEN in GRANITE CAVE, /  near DEWFORD. He often loses track /  of time when he's studying rocks. //  Our researcher on the second floor /  keeps a guide to the LEGENDARY SIGNS. /  Ask him whenever you want a lead. / STEVEN received my LETTER. Good. //  He will entrust the MEGA RING only after /  you earn DEWFORD's KNUCKLE BADGE. | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-MEGA-GUIDE |
| `RustboroCity_DevonCorp_3F:bg_events:001` | 1,5 | [RustboroCity_DevonCorp_3F_EventScript_RareRocksDisplay](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L223) — RustboroCity_DevonCorp_3F_EventScript_RareRocksDisplay at (1,5); It's a collection of rare rocks and /  stones assembled by the PRESIDENT. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `RustboroCity_DevonCorp_3F:bg_events:002` | 1,7 | [RustboroCity_DevonCorp_3F_EventScript_RareRocksDisplay](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L223) — RustboroCity_DevonCorp_3F_EventScript_RareRocksDisplay at (1,7); It's a collection of rare rocks and /  stones assembled by the PRESIDENT. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `RustboroCity_DevonCorp_3F:warp_events:001` | 2,1 | Warp from (2,1, elevation 0) to MAP_RUSTBORO_CITY_DEVON_CORP_2F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_DevonCorp_3F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [RustboroCity_DevonCorp_3F_OnTransition](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L7) — MAP_SCRIPT_ON_TRANSITION calls RustboroCity_DevonCorp_3F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `RustboroCity_DevonCorp_3F:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [RustboroCity_DevonCorp_3F_OnWarp](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L16) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls RustboroCity_DevonCorp_3F_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `RustboroCity_DevonCorp_3F:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [RustboroCity_DevonCorp_3F_OnFrame](../../baseline/source/data/maps/RustboroCity_DevonCorp_3F/scripts.inc#L24) — MAP_SCRIPT_ON_FRAME_TABLE calls RustboroCity_DevonCorp_3F_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
