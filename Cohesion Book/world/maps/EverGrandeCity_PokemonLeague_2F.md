# EverGrandeCity_PokemonLeague_2F

**REVISE.** Shared player-owned or external-link support. Preserve geography, trades, records and decoration data. Native trainer challenge entry is governed by the all-doubles external-entry contract, never silently accepted as a singles exception.

[Regional experience](../10-support.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity_PokemonLeague_2F/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity_PokemonLeague_2F/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_2F` · `LAYOUT_POKEMON_CENTER_2F` · `WEATHER_NONE` · `MUS_POKE_CENTER`

Shared player-owned or external-link support. Preserve geography, trades, records and decoration data. Native trainer challenge entry is governed by the all-doubles external-entry contract, never silently accepted as a singles exception.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity_PokemonLeague_2F:object_events:001` | 6,2 | [Common_EventScript_UnionRoomAttendant](../../baseline/source/data/event_scripts.s#L1672) — Common_EventScript_UnionRoomAttendant at (6,2); shared behavior FLAVOR | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:object_events:002` | 2,2 | [Common_EventScript_WirelessClubAttendant](../../baseline/source/data/event_scripts.s#L1680) — Common_EventScript_WirelessClubAttendant at (2,2); shared behavior FLAVOR | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:object_events:003` | 10,2 | [Common_EventScript_DirectCornerAttendant](../../baseline/source/data/event_scripts.s#L1688) — Common_EventScript_DirectCornerAttendant at (10,2); shared behavior FLAVOR | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:object_events:004` | 1,2 | [CableClub_EventScript_MysteryGiftMan](../../baseline/source/data/scripts/cable_club.inc#L21) — CableClub_EventScript_MysteryGiftMan at (1,2); shared behavior LINK | **REVISE** · [W-C-LINK](../common-contracts.md#w-c-link)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:warp_events:001` | 1,6 | Warp from (1,6, elevation 4) to MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_1F warp 4. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:warp_events:002` | 5,1 | Warp from (5,1, elevation 3) to MAP_UNION_ROOM warp 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:warp_events:003` | 9,1 | Warp from (9,1, elevation 3) to MAP_TRADE_CENTER warp 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [CableClub_OnFrame](../../baseline/source/data/scripts/cable_club.inc#L103) — MAP_SCRIPT_ON_FRAME_TABLE calls CableClub_OnFrame. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [CableClub_OnWarp](../../baseline/source/data/scripts/cable_club.inc#L51) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls CableClub_OnWarp. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:map_scripts:003` | MAP_SCRIPT_ON_LOAD | [CableClub_OnLoad](../../baseline/source/data/scripts/cable_club.inc#L68) — MAP_SCRIPT_ON_LOAD calls CableClub_OnLoad. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback)  · LINK-01 |
| `EverGrandeCity_PokemonLeague_2F:map_scripts:004` | MAP_SCRIPT_ON_TRANSITION | [CableClub_OnTransition](../../baseline/source/data/scripts/cable_club.inc#L1) — MAP_SCRIPT_ON_TRANSITION calls CableClub_OnTransition. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback)  · LINK-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.

Shared entry/format reconciliation: **LINK-01**. Preserve imported teams, trading and records; use its exact doubles-only native entry and recovery rules.
