# LavaridgeTown_PokemonCenter_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/LavaridgeTown_PokemonCenter_1F/map.json) · [Scripts](../../baseline/source/data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc)

## Current map contract

`MAP_LAVARIDGE_TOWN_POKEMON_CENTER_1F` · `LAYOUT_LAVARIDGE_TOWN_POKEMON_CENTER_1F` · `WEATHER_NONE` · `MUS_POKE_CENTER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LavaridgeTown_PokemonCenter_1F:object_events:001` | 8,2 | [LavaridgeTown_PokemonCenter_1F_EventScript_Nurse](../../baseline/source/data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc#L11) — LavaridgeTown_PokemonCenter_1F_EventScript_Nurse at (8,2); shared behavior CENTER | **REPAIR** · [W-C-CENTER](../common-contracts.md#w-c-center) · W-CENTER-FIRST-VISIT |
| `LavaridgeTown_PokemonCenter_1F:object_events:002` | 13,8 | [LavaridgeTown_PokemonCenter_1F_EventScript_Youngster](../../baseline/source/data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc#L19) — LavaridgeTown_PokemonCenter_1F_EventScript_Youngster at (13,8); It's sort of magical how just sitting /  in a hot-spring pool can invigorate. //  I wish I could let my POKéMON /  soak, too. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_PokemonCenter_1F:object_events:003` | 12,6 | [LavaridgeTown_PokemonCenter_1F_EventScript_Woman](../../baseline/source/data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc#L23) — LavaridgeTown_PokemonCenter_1F_EventScript_Woman at (12,6); I think POKéMON get closer to their /  TRAINERS if they spend time together. //  The longer the better. /  That's what I think. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_PokemonCenter_1F:object_events:004` | 3,8 | [LavaridgeTown_PokemonCenter_1F_EventScript_Gentleman](../../baseline/source/data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc#L27) — LavaridgeTown_PokemonCenter_1F_EventScript_Gentleman at (3,8); Hohoho! Hey, kid, you can reach /  the hot springs from here. //  If POKéMON are getting rest, so too /  should their TRAINERS. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_PokemonCenter_1F:object_events:005` | 4,3 | [LavaridgeTown_PokemonCenter_1F_EventScript_MoomooMilk](../../baseline/source/data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc#L85) — LavaridgeTown_PokemonCenter_1F_EventScript_MoomooMilk at (4,3); Moomoo Milk is perfect before or after /  the hot spring. How many would you like? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_PokemonCenter_1F:object_events:006` | 10,4 | [LavaridgeTown_PokemonCenter_1F_EventScript_Lucy](../../baseline/source/data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc#L31) — LavaridgeTown_PokemonCenter_1F_EventScript_Lucy at (10,4); Scott said a serious Trainer would /  come through these springs. //  I am Lucy. I stack the odds until /  they obey. Show me your answer. / So… You made your own luck. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `LavaridgeTown_PokemonCenter_1F:object_events:007` | 1,2 | [Common_EventScript_EmeraldChampionsBattleVendor](../../baseline/source/data/scripts/emerald_champions.inc#L384) — Common_EventScript_EmeraldChampionsBattleVendor at (1,2); shared behavior VENDOR | **KEEP** · [W-C-VENDOR](../common-contracts.md#w-c-vendor) |
| `LavaridgeTown_PokemonCenter_1F:object_events:008` | 13,2 | [Common_EventScript_EmeraldChampionsMoveTutor](../../baseline/source/data/scripts/emerald_champions.inc#L1) — Common_EventScript_EmeraldChampionsMoveTutor at (13,2); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `LavaridgeTown_PokemonCenter_1F:warp_events:001` | 8,8 | Warp from (8,8, elevation 3) to MAP_LAVARIDGE_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_PokemonCenter_1F:warp_events:002` | 7,8 | Warp from (7,8, elevation 3) to MAP_LAVARIDGE_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_PokemonCenter_1F:warp_events:003` | 1,6 | Warp from (1,6, elevation 4) to MAP_LAVARIDGE_TOWN_POKEMON_CENTER_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_PokemonCenter_1F:warp_events:004` | 2,1 | Warp from (2,1, elevation 0) to MAP_LAVARIDGE_TOWN warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_PokemonCenter_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LavaridgeTown_PokemonCenter_1F_OnTransition](../../baseline/source/data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls LavaridgeTown_PokemonCenter_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `LavaridgeTown_PokemonCenter_1F:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [CableClub_OnResume](../../baseline/source/data/scripts/cable_club.inc#L1226) — MAP_SCRIPT_ON_RESUME calls CableClub_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
