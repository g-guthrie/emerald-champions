# RustboroCity_PokemonCenter_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_PokemonCenter_1F/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_PokemonCenter_1F/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_POKEMON_CENTER_1F` · `LAYOUT_POKEMON_CENTER_1F` · `WEATHER_NONE` · `MUS_POKE_CENTER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_PokemonCenter_1F:object_events:001` | 8,2 | [RustboroCity_PokemonCenter_1F_EventScript_Nurse](../../baseline/source/data/maps/RustboroCity_PokemonCenter_1F/scripts.inc#L11) — RustboroCity_PokemonCenter_1F_EventScript_Nurse at (8,2); shared behavior CENTER | **REPAIR** · [W-C-CENTER](../common-contracts.md#w-c-center) · W-CENTER-FIRST-VISIT |
| `RustboroCity_PokemonCenter_1F:object_events:002` | 4,8 | [RustboroCity_PokemonCenter_1F_EventScript_Man](../../baseline/source/data/maps/RustboroCity_PokemonCenter_1F/scripts.inc#L19) — RustboroCity_PokemonCenter_1F_EventScript_Man at (4,8); My POKéMON has a NAIVE nature, and my /  friend's has a JOLLY nature. //  It's fascinating how POKéMON have /  personalities! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_PokemonCenter_1F:object_events:003` | 5,5 | [RustboroCity_PokemonCenter_1F_EventScript_Boy](../../baseline/source/data/maps/RustboroCity_PokemonCenter_1F/scripts.inc#L23) — RustboroCity_PokemonCenter_1F_EventScript_Boy at (5,5); Just like people, there are male and /  female POKéMON. //  But no one seems to have any idea how /  they're different. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_PokemonCenter_1F:object_events:004` | 12,7 | [RustboroCity_PokemonCenter_1F_EventScript_Girl](../../baseline/source/data/maps/RustboroCity_PokemonCenter_1F/scripts.inc#L27) — RustboroCity_PokemonCenter_1F_EventScript_Girl at (12,7); The man next door gave me an HM! //  I used it to teach my POKéMON how to /  CUT down skinny trees. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_PokemonCenter_1F:object_events:005` | 1,2 | [RustboroCity_PokemonCenter_1F_EventScript_Clerk2](../../baseline/source/data/maps/RustboroCity_PokemonCenter_1F/scripts.inc#L48) — RustboroCity_PokemonCenter_1F_EventScript_Clerk2 at (1,2); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_PokemonCenter_1F:object_events:006` | 2,2 | [Common_EventScript_EmeraldChampionsBattleVendor](../../baseline/source/data/scripts/emerald_champions.inc#L384) — Common_EventScript_EmeraldChampionsBattleVendor at (2,2); shared behavior VENDOR | **KEEP** · [W-C-VENDOR](../common-contracts.md#w-c-vendor) |
| `RustboroCity_PokemonCenter_1F:object_events:007` | 13,2 | [Common_EventScript_EmeraldChampionsMoveTutor](../../baseline/source/data/scripts/emerald_champions.inc#L1) — Common_EventScript_EmeraldChampionsMoveTutor at (13,2); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `RustboroCity_PokemonCenter_1F:warp_events:001` | 8,8 | Warp from (8,8, elevation 3) to MAP_RUSTBORO_CITY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_PokemonCenter_1F:warp_events:002` | 7,8 | Warp from (7,8, elevation 3) to MAP_RUSTBORO_CITY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_PokemonCenter_1F:warp_events:003` | 1,6 | Warp from (1,6, elevation 4) to MAP_RUSTBORO_CITY_POKEMON_CENTER_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_PokemonCenter_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [RustboroCity_PokemonCenter_1F_OnTransition](../../baseline/source/data/maps/RustboroCity_PokemonCenter_1F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls RustboroCity_PokemonCenter_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `RustboroCity_PokemonCenter_1F:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [CableClub_OnResume](../../baseline/source/data/scripts/cable_club.inc#L1226) — MAP_SCRIPT_ON_RESUME calls CableClub_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
