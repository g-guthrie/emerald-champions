# RustboroCity_Flat2_2F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_Flat2_2F/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_Flat2_2F/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_FLAT2_2F` · `LAYOUT_RUSTBORO_CITY_FLAT2_2F` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_Flat2_2F:object_events:001` | 11,4 | [RustboroCity_Flat2_2F_EventScript_OldMan](../../baseline/source/data/maps/RustboroCity_Flat2_2F/scripts.inc#L15) — RustboroCity_Flat2_2F_EventScript_OldMan at (11,4); Way back in the old days, DEVON was just /  a teeny, tiny company. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat2_2F:object_events:002` | 7,3 | [RustboroCity_Flat2_2F_EventScript_NinjaBoy](../../baseline/source/data/maps/RustboroCity_Flat2_2F/scripts.inc#L19) — RustboroCity_Flat2_2F_EventScript_NinjaBoy at (7,3); My daddy's working at the CORPORATION. //  My daddy made this! /  But I can't use it, so you can have it. / My daddy's working at the CORPORATION. //  When I grow up, I'm going to work for /  DEVON, too. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RustboroCity_Flat2_2F:object_events:003` | 4,5 | [RustboroCity_Flat2_2F_EventScript_GiveFloatStone](../../baseline/source/data/maps/RustboroCity_Flat2_2F/scripts.inc#L35) — RustboroCity_Flat2_2F_EventScript_GiveFloatStone at (4,5); My dad is a real ace at DEVON. //  Here. Take something an ACE TRAINER /  like me knows how to use. / A METAL COAT lets some POKéMON evolve /  when traded. DEVON refines them. | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FIELD-CLUES |
| `RustboroCity_Flat2_2F:object_events:004` | 4,5 | [RustboroCity_Flat2_2F_EventScript_FatMan](../../baseline/source/data/maps/RustboroCity_Flat2_2F/scripts.inc#L52) — RustboroCity_Flat2_2F_EventScript_FatMan at (4,5); For some reason, I've put on a lot of /  weight recently… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Flat2_2F:warp_events:001` | 3,1 | Warp from (3,1, elevation 0) to MAP_RUSTBORO_CITY_FLAT2_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_Flat2_2F:warp_events:002` | 1,1 | Warp from (1,1, elevation 0) to MAP_RUSTBORO_CITY_FLAT2_3F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_Flat2_2F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [RustboroCity_Flat2_2F_OnTransition](../../baseline/source/data/maps/RustboroCity_Flat2_2F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls RustboroCity_Flat2_2F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
