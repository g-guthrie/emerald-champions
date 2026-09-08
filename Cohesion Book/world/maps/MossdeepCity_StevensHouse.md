# MossdeepCity_StevensHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MossdeepCity_StevensHouse/map.json) · [Scripts](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc)

## Current map contract

`MAP_MOSSDEEP_CITY_STEVENS_HOUSE` · `LAYOUT_MOSSDEEP_CITY_STEVENS_HOUSE` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MossdeepCity_StevensHouse:object_events:001` | 9,6 | [MossdeepCity_StevensHouse_EventScript_Steven](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L137) — MossdeepCity_StevensHouse_EventScript_Steven at (9,6); STEVEN: Search south on ROUTE 128. /  Use DIVE in the dark water. //  Find the SEAFLOOR CAVERN where AQUA /  took the submarine, and stop ARCHIE. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_StevensHouse:object_events:002` | 4,3 | [MossdeepCity_StevensHouse_EventScript_BeldumPokeball](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L73) — MossdeepCity_StevensHouse_EventScript_BeldumPokeball at (4,3); {PLAYER} checked the POKé BALL. //  It contained the POKéMON /  BELDUM. //  Take the POKé BALL? / {PLAYER} obtained a BELDUM. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MossdeepCity_StevensHouse:object_events:003` | 6,4 | [MossdeepCity_StevensHouse_EventScript_Letter](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L141) — MossdeepCity_StevensHouse_EventScript_Letter at (6,4); It's a letter. //  … … … … … … //  To {PLAYER}{KUN}… //  I've decided to do a little soul- /  searching and train on the road. //  I don't plan to return home for some /  time. //  I have a favor to ask of you. //  I want you to take the POKé BALL on /  the desk. //  Inside it is a BELDUM, my favorite /  POKéMON. //  I'm counting on you. //  May our paths cross someday. //  STEVEN STONE | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_StevensHouse:bg_events:001` | 0,1 | [MossdeepCity_StevensHouse_EventScript_RockDisplay](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L133) — MossdeepCity_StevensHouse_EventScript_RockDisplay at (0,1); It's a collection of rare rocks and /  stones assembled by STEVEN. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MossdeepCity_StevensHouse:bg_events:002` | 1,1 | [MossdeepCity_StevensHouse_EventScript_RockDisplay](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L133) — MossdeepCity_StevensHouse_EventScript_RockDisplay at (1,1); It's a collection of rare rocks and /  stones assembled by STEVEN. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MossdeepCity_StevensHouse:bg_events:003` | 10,4 | [MossdeepCity_StevensHouse_EventScript_RockDisplay](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L133) — MossdeepCity_StevensHouse_EventScript_RockDisplay at (10,4); It's a collection of rare rocks and /  stones assembled by STEVEN. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MossdeepCity_StevensHouse:bg_events:004` | 10,6 | [MossdeepCity_StevensHouse_EventScript_RockDisplay](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L133) — MossdeepCity_StevensHouse_EventScript_RockDisplay at (10,6); It's a collection of rare rocks and /  stones assembled by STEVEN. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `MossdeepCity_StevensHouse:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_MOSSDEEP_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MossdeepCity_StevensHouse:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_MOSSDEEP_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MossdeepCity_StevensHouse:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [MossdeepCity_StevensHouse_OnLoad](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L7) — MAP_SCRIPT_ON_LOAD calls MossdeepCity_StevensHouse_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MossdeepCity_StevensHouse:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [MossdeepCity_StevensHouse_OnTransition](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L15) — MAP_SCRIPT_ON_TRANSITION calls MossdeepCity_StevensHouse_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MossdeepCity_StevensHouse:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [MossdeepCity_StevensHouse_OnFrame](../../baseline/source/data/maps/MossdeepCity_StevensHouse/scripts.inc#L24) — MAP_SCRIPT_ON_FRAME_TABLE calls MossdeepCity_StevensHouse_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
