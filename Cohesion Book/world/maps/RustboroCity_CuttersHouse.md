# RustboroCity_CuttersHouse

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_CuttersHouse/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_CuttersHouse/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_CUTTERS_HOUSE` · `LAYOUT_RUSTBORO_CITY_CUTTERS_HOUSE` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_CuttersHouse:object_events:001` | 7,5 | [RustboroCity_CuttersHouse_EventScript_Cutter](../../baseline/source/data/maps/RustboroCity_CuttersHouse/scripts.inc#L4) — RustboroCity_CuttersHouse_EventScript_Cutter at (7,5); That determined expression… /  That limber way you move… /  And your well-trained POKéMON… //  You're obviously a skilled TRAINER! //  No, wait, don't say a word. /  I can tell just by looking at you. //  I'm sure that you can put this /  HIDDEN MACHINE to good use. //  No need to be modest or shy. /  Go on, take it! / That HM registers CUT as a field /  technique for your team. //  Once you earn the STONE BADGE, any /  POKéMON that knows CUT can chop thin /  trees outside battle. | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FIELD-CLUES |
| `RustboroCity_CuttersHouse:object_events:002` | 9,2 | [RustboroCity_CuttersHouse_EventScript_Lass](../../baseline/source/data/maps/RustboroCity_CuttersHouse/scripts.inc#L21) — RustboroCity_CuttersHouse_EventScript_Lass at (9,2); When they were expanding the city of /  RUSTBORO, my dad helped out. //  He made his POKéMON use CUT to clear /  the land of trees. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_CuttersHouse:warp_events:001` | 5,8 | Warp from (5,8, elevation 0) to MAP_RUSTBORO_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_CuttersHouse:warp_events:002` | 6,8 | Warp from (6,8, elevation 0) to MAP_RUSTBORO_CITY warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
