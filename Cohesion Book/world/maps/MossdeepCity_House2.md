# MossdeepCity_House2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MossdeepCity_House2/map.json) · [Scripts](../../baseline/source/data/maps/MossdeepCity_House2/scripts.inc)

## Current map contract

`MAP_MOSSDEEP_CITY_HOUSE2` · `LAYOUT_HOUSE1` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MossdeepCity_House2:object_events:001` | 6,6 | [MossdeepCity_House2_EventScript_Man](../../baseline/source/data/maps/MossdeepCity_House2/scripts.inc#L4) — MossdeepCity_House2_EventScript_Man at (6,6); My little sister exchanges MAIL with /  her boyfriend in FORTREE. //  I don't envy her one bit at all. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_House2:object_events:002` | 4,4 | [MossdeepCity_House2_EventScript_Twin](../../baseline/source/data/maps/MossdeepCity_House2/scripts.inc#L8) — MossdeepCity_House2_EventScript_Twin at (4,4); Even though I can't see my friend in /  FORTREE, my POKéMON carries MAIL /  back and forth for us. //  I'm not lonesome, even though we're /  apart. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_House2:object_events:003` | 4,5 | [MossdeepCity_House2_EventScript_Wingull](../../baseline/source/data/maps/MossdeepCity_House2/scripts.inc#L12) — MossdeepCity_House2_EventScript_Wingull at (4,5); WINGULL: Pihyoh! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MossdeepCity_House2:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_MOSSDEEP_CITY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MossdeepCity_House2:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_MOSSDEEP_CITY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
