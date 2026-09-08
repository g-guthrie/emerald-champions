# OldaleTown_PokemonCenter_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/OldaleTown_PokemonCenter_1F/map.json) · [Scripts](../../baseline/source/data/maps/OldaleTown_PokemonCenter_1F/scripts.inc)

## Current map contract

`MAP_OLDALE_TOWN_POKEMON_CENTER_1F` · `LAYOUT_POKEMON_CENTER_1F` · `WEATHER_NONE` · `MUS_POKE_CENTER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `OldaleTown_PokemonCenter_1F:object_events:001` | 8,2 | [OldaleTown_PokemonCenter_1F_EventScript_Nurse](../../baseline/source/data/maps/OldaleTown_PokemonCenter_1F/scripts.inc#L11) — OldaleTown_PokemonCenter_1F_EventScript_Nurse at (8,2); shared behavior CENTER | **REPAIR** · [W-C-CENTER](../common-contracts.md#w-c-center) · W-CENTER-FIRST-VISIT |
| `OldaleTown_PokemonCenter_1F:object_events:002` | 4,4 | [OldaleTown_PokemonCenter_1F_EventScript_Gentleman](../../baseline/source/data/maps/OldaleTown_PokemonCenter_1F/scripts.inc#L19) — OldaleTown_PokemonCenter_1F_EventScript_Gentleman at (4,4); That PC in the corner there is /  for any POKéMON TRAINER to use. //  Naturally, that means you're welcome /  to use it, too. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `OldaleTown_PokemonCenter_1F:object_events:003` | 12,6 | [OldaleTown_PokemonCenter_1F_EventScript_Boy](../../baseline/source/data/maps/OldaleTown_PokemonCenter_1F/scripts.inc#L23) — OldaleTown_PokemonCenter_1F_EventScript_Boy at (12,6); POKéMON CENTERS are great! //  You can use their services as much /  as you like, and it's all for free. /  You never have to worry! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `OldaleTown_PokemonCenter_1F:object_events:004` | 3,8 | [OldaleTown_PokemonCenter_1F_EventScript_Girl](../../baseline/source/data/maps/OldaleTown_PokemonCenter_1F/scripts.inc#L27) — OldaleTown_PokemonCenter_1F_EventScript_Girl at (3,8); The POKéMON WIRELESS CLUB on /  the second floor was built recently. //  But they say they're still making /  adjustments. / The POKéMON WIRELESS CLUB on /  the second floor was built recently. //  I traded POKéMON right away. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `OldaleTown_PokemonCenter_1F:object_events:005` | 2,2 | [Common_EventScript_EmeraldChampionsBattleVendor](../../baseline/source/data/scripts/emerald_champions.inc#L384) — Common_EventScript_EmeraldChampionsBattleVendor at (2,2); shared behavior VENDOR | **KEEP** · [W-C-VENDOR](../common-contracts.md#w-c-vendor) |
| `OldaleTown_PokemonCenter_1F:object_events:006` | 13,2 | [Common_EventScript_EmeraldChampionsMoveTutor](../../baseline/source/data/scripts/emerald_champions.inc#L1) — Common_EventScript_EmeraldChampionsMoveTutor at (13,2); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `OldaleTown_PokemonCenter_1F:warp_events:001` | 8,8 | Warp from (8,8, elevation 3) to MAP_OLDALE_TOWN warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown_PokemonCenter_1F:warp_events:002` | 7,8 | Warp from (7,8, elevation 3) to MAP_OLDALE_TOWN warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown_PokemonCenter_1F:warp_events:003` | 1,6 | Warp from (1,6, elevation 4) to MAP_OLDALE_TOWN_POKEMON_CENTER_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `OldaleTown_PokemonCenter_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [OldaleTown_PokemonCenter_1F_OnTransition](../../baseline/source/data/maps/OldaleTown_PokemonCenter_1F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls OldaleTown_PokemonCenter_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `OldaleTown_PokemonCenter_1F:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [CableClub_OnResume](../../baseline/source/data/scripts/cable_club.inc#L1226) — MAP_SCRIPT_ON_RESUME calls CableClub_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
