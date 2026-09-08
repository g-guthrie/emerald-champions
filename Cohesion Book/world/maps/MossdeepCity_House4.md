# MossdeepCity_House4

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MossdeepCity_House4/map.json) · [Scripts](../../baseline/source/data/maps/MossdeepCity_House4/scripts.inc)

## Current map contract

`MAP_MOSSDEEP_CITY_HOUSE4` · `LAYOUT_HOUSE_WITH_BED` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MossdeepCity_House4:object_events:001` | 3,4 | [MossdeepCity_House4_EventScript_Woman](../../baseline/source/data/maps/MossdeepCity_House4/scripts.inc#L4) — MossdeepCity_House4_EventScript_Woman at (3,4); My little brother says he likes to go /  find people's SECRET BASES. / My little brother says he likes to /  visit people's SECRET BASES and have /  POKéMON battles. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MossdeepCity_House4:object_events:002` | 6,6 | [MossdeepCity_House4_EventScript_NinjaBoy](../../baseline/source/data/maps/MossdeepCity_House4/scripts.inc#L17) — MossdeepCity_House4_EventScript_NinjaBoy at (6,6); Was it you who made a SECRET BASE /  near {STR_VAR_1}? / You should make a SECRET BASE /  somewhere. I'll go find it! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MossdeepCity_House4:object_events:003` | 2,4 | [MossdeepCity_House4_EventScript_Skitty](../../baseline/source/data/maps/MossdeepCity_House4/scripts.inc#L32) — MossdeepCity_House4_EventScript_Skitty at (2,4); SKITTY: Miyaan? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_House4:warp_events:001` | 4,7 | Warp from (4,7, elevation 0) to MAP_MOSSDEEP_CITY warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MossdeepCity_House4:warp_events:002` | 3,7 | Warp from (3,7, elevation 0) to MAP_MOSSDEEP_CITY warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
