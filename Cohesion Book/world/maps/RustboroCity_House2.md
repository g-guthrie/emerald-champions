# RustboroCity_House2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_House2/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_House2/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_HOUSE2` · `LAYOUT_RUSTBORO_CITY_HOUSE` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_House2:object_events:001` | 4,4 | [RustboroCity_House2_EventScript_PokefanF](../../baseline/source/data/maps/RustboroCity_House2/scripts.inc#L4) — RustboroCity_House2_EventScript_PokefanF at (4,4); The TRAINER'S SCHOOL is excellent. //  If you study there, you could even /  become a GYM LEADER. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_House2:object_events:002` | 4,5 | [RustboroCity_House2_EventScript_LittleGirl](../../baseline/source/data/maps/RustboroCity_House2/scripts.inc#L8) — RustboroCity_House2_EventScript_LittleGirl at (4,5); ROXANNE, the GYM LEADER, really knows /  a lot about POKéMON. //  She's really strong, too! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_House2:warp_events:001` | 5,8 | Warp from (5,8, elevation 0) to MAP_RUSTBORO_CITY warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_House2:warp_events:002` | 6,8 | Warp from (6,8, elevation 0) to MAP_RUSTBORO_CITY warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
