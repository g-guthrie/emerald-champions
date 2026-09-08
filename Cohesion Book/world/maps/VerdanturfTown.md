# VerdanturfTown

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/VerdanturfTown/map.json) · [Scripts](../../baseline/source/data/maps/VerdanturfTown/scripts.inc)

## Current map contract

`MAP_VERDANTURF_TOWN` · `LAYOUT_VERDANTURF_TOWN` · `WEATHER_SUNNY` · `MUS_VERDANTURF`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VerdanturfTown:object_events:001` | 14,17 | [VerdanturfTown_EventScript_Man](../../baseline/source/data/maps/VerdanturfTown/scripts.inc#L27) — VerdanturfTown_EventScript_Man at (14,17); The way the winds blow, volcanic ash /  is never blown in this direction. //  The air is clean and delicious here. /  Living here should do wonders for even /  frail and sickly people. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown:object_events:002` | 9,2 | [VerdanturfTown_EventScript_Twin](../../baseline/source/data/maps/VerdanturfTown/scripts.inc#L10) — VerdanturfTown_EventScript_Twin at (9,2); My papa told me. //  He says this tunnel is full of /  timid POKéMON. //  They get all scared of loud noise and /  make a big uproar. //  So they had to stop the big tunnel /  project. //  But there's one man. He's trying to dig /  the tunnel by himself! / There was a man who dug a tunnel for /  a lady he loved. //  I don't really get it, but hey! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `VerdanturfTown:object_events:003` | 7,11 | [VerdanturfTown_EventScript_Boy](../../baseline/source/data/maps/VerdanturfTown/scripts.inc#L35) — VerdanturfTown_EventScript_Boy at (7,11); Did you see the cave next to the /  POKéMON MART? //  There's a guy in there who's trying to /  bust up boulders so he can bust out /  through to the other side. //  It'd be great if we could go through… /  It'll make it easy to visit RUSTBORO. / That cave next to the POKéMON MART /  is now a tunnel to the other side. //  It's great--it's easy to go shop for /  new DEVON products in RUSTBORO now. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `VerdanturfTown:object_events:004` | 7,6 | [VerdanturfTown_EventScript_Camper](../../baseline/source/data/maps/VerdanturfTown/scripts.inc#L31) — VerdanturfTown_EventScript_Camper at (7,6); My POKéMON and I, we've been riding /  a hot winning streak. //  So I decided to make my BATTLE TENT /  debut in this town. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown:bg_events:001` | 14,3 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (14,3); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `VerdanturfTown:bg_events:002` | 14,6 | [VerdanturfTown_EventScript_TownSign](../../baseline/source/data/maps/VerdanturfTown/scripts.inc#L48) — VerdanturfTown_EventScript_TownSign at (14,6); VERDANTURF TOWN //  The windswept highlands where grass and /  people recover after every storm. //  A restored meadow lies to the south. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `VerdanturfTown:bg_events:003` | 17,3 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (17,3); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `VerdanturfTown:bg_events:004` | 7,14 | [VerdanturfTown_EventScript_WandasHouseSign](../../baseline/source/data/maps/VerdanturfTown/scripts.inc#L52) — VerdanturfTown_EventScript_WandasHouseSign at (7,14); WANDA'S HOUSE | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `VerdanturfTown:bg_events:005` | 13,3 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (13,3); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `VerdanturfTown:bg_events:006` | 18,3 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (18,3); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `VerdanturfTown:bg_events:007` | 1,8 | [VerdanturfTown_EventScript_BattleTentSign](../../baseline/source/data/maps/VerdanturfTown/scripts.inc#L56) — VerdanturfTown_EventScript_BattleTentSign at (1,8); BATTLE TENT VERDANTURF SITE /  “Feast Your Eyes on Battles!” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `VerdanturfTown:bg_events:008` | 7,3 | [VerdanturfTown_EventScript_RusturfTunnelSign](../../baseline/source/data/maps/VerdanturfTown/scripts.inc#L60) — VerdanturfTown_EventScript_RusturfTunnelSign at (7,3); RUSTURF TUNNEL /  “Linking RUSTBORO and VERDANTURF //  “The tunnel project has been /  canceled.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `VerdanturfTown:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_VERDANTURF_TOWN_BATTLE_TENT_LOBBY warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:warp_events:002` | 12,3 | Warp from (12,3, elevation 0) to MAP_VERDANTURF_TOWN_MART warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:warp_events:003` | 16,3 | Warp from (16,3, elevation 0) to MAP_VERDANTURF_TOWN_POKEMON_CENTER_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:warp_events:004` | 10,14 | Warp from (10,14, elevation 0) to MAP_VERDANTURF_TOWN_WANDAS_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:warp_events:005` | 8,1 | Warp from (8,1, elevation 0) to MAP_RUSTURF_TUNNEL warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:warp_events:006` | 1,14 | Warp from (1,14, elevation 0) to MAP_VERDANTURF_TOWN_FRIENDSHIP_RATERS_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:warp_events:007` | 17,15 | Warp from (17,15, elevation 0) to MAP_VERDANTURF_TOWN_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:connections:001` | up | up connection to MAP_ROUTE116, offset -80. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:connections:002` | right | right connection to MAP_ROUTE117, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:connections:003` | down | down connection to MAP_VERDANTURF_MEADOW, offset -2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [VerdanturfTown_OnTransition](../../baseline/source/data/maps/VerdanturfTown/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls VerdanturfTown_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
