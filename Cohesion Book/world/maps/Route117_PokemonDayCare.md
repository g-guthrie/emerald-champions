# Route117_PokemonDayCare

**KEEP.** Keep precise Togepi gift exclusion, compatible-parent hatch detection, Oval Charm and Kangaskhanite pending reward. Do not restore egg-only baby availability or genetic-stat grinding.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/Route117_PokemonDayCare/map.json) · [Scripts](../../baseline/source/data/maps/Route117_PokemonDayCare/scripts.inc)

## Current map contract

`MAP_ROUTE117_POKEMON_DAY_CARE` · `LAYOUT_ROUTE117_POKEMON_DAY_CARE` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route117_PokemonDayCare:object_events:001` | 2,2 | [Route117_PokemonDayCare_EventScript_DaycareWoman](../../baseline/source/data/scripts/day_care.inc#L71) — Route117_PokemonDayCare_EventScript_DaycareWoman at (2,2); shared behavior DAYCARE | **KEEP** · [W-C-DAYCARE](../common-contracts.md#w-c-daycare) |
| `Route117_PokemonDayCare:object_events:002` | 9,6 | [Route117_PokemonDayCare_EventScript_TogepiEgg](../../baseline/source/data/maps/Route117_PokemonDayCare/scripts.inc#L9) — Route117_PokemonDayCare_EventScript_TogepiEgg at (9,6); MEGA KANGASKHAN fights with its child. /  Their Ability is PARENTAL BOND. //  Leave two compatible adult POKéMON /  here, then hatch the EGG they produce. //  I'll give you a KANGASKHANITE! /  DITTO in the grass can help you breed. //  Want this TOGEPI EGG as a gift? /  This gift won't count for the stone. / Take TOGEPI along on your adventure. //  For the stone, hatch an EGG produced /  by two POKéMON you leave here. | **KEEP** · [W-C-DAYCARE](../common-contracts.md#w-c-daycare) |
| `Route117_PokemonDayCare:warp_events:001` | 2,8 | Warp from (2,8, elevation 0) to MAP_ROUTE117 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route117_PokemonDayCare:warp_events:002` | 3,8 | Warp from (3,8, elevation 0) to MAP_ROUTE117 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route117_PokemonDayCare:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route117_PokemonDayCare_OnTransition](../../baseline/source/data/maps/Route117_PokemonDayCare/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route117_PokemonDayCare_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
