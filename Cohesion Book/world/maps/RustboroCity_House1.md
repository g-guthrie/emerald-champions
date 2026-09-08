# RustboroCity_House1

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_House1/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_House1/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_HOUSE1` · `LAYOUT_RUSTBORO_CITY_HOUSE1` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_House1:object_events:001` | 6,4 | [RustboroCity_House1_EventScript_Trader](../../baseline/source/data/maps/RustboroCity_House1/scripts.inc#L4) — RustboroCity_House1_EventScript_Trader at (6,4); Huh? My POKéMON is cute? /  Sure, I knew that. //  But if you really want, I'm willing /  to trade it to you. //  I'll trade you my {STR_VAR_2} for /  a {STR_VAR_1} if you want. / Eheheh… /  Please be good to my POKéMON. //  It already has a proper battle set, /  so it can fight from day one. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RustboroCity_House1:object_events:002` | 9,2 | [RustboroCity_House1_EventScript_Hiker](../../baseline/source/data/maps/RustboroCity_House1/scripts.inc#L29) — RustboroCity_House1_EventScript_Hiker at (9,2); In all sorts of places, there are all /  sorts of POKéMON and people. //  I find that fascinating, so I go to all /  sorts of places. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_House1:warp_events:001` | 5,7 | Warp from (5,7, elevation 0) to MAP_RUSTBORO_CITY warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_House1:warp_events:002` | 6,7 | Warp from (6,7, elevation 0) to MAP_RUSTBORO_CITY warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
