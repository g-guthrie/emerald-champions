# MtPyre_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/MtPyre_1F/map.json) · [Scripts](../../baseline/source/data/maps/MtPyre_1F/scripts.inc)

## Current map contract

`MAP_MT_PYRE_1F` · `LAYOUT_MT_PYRE_1F` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MtPyre_1F:object_events:001` | 21,2 | [MtPyre_1F_EventScript_CleanseTagWoman](../../baseline/source/data/maps/MtPyre_1F/scripts.inc#L4) — MtPyre_1F_EventScript_CleanseTagWoman at (21,2); All sorts of beings wander the slopes /  of MT. PYRE… //  There is no telling what may happen. /  Take this. It's for your own good. / Have a POKéMON hold that /  CLEANSE TAG. //  It will help ward off wild POKéMON. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MtPyre_1F:object_events:002` | 17,8 | [MtPyre_1F_EventScript_PokefanF](../../baseline/source/data/maps/MtPyre_1F/scripts.inc#L20) — MtPyre_1F_EventScript_PokefanF at (17,8); Did you come to pay your respect /  to the spirits of departed POKéMON? //  You must care for your POKéMON a lot. //  This mountain changes some evolutions. /  Level QUILAVA here at Lv. 36 or higher /  for Hisuian TYPHLOSION. //  Level RUFFLET here at Lv. 54 or higher /  for Hisuian BRAVIARY. Elsewhere, both /  grow into their usual forms. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MtPyre_1F:object_events:003` | 13,10 | [MtPyre_1F_EventScript_Man](../../baseline/source/data/maps/MtPyre_1F/scripts.inc#L24) — MtPyre_1F_EventScript_Man at (13,10); This is the final resting place of my /  ZIGZAGOON. I cherished it… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MtPyre_1F:warp_events:001` | 17,18 | Warp from (17,18, elevation 3) to MAP_ROUTE122 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_1F:warp_events:002` | 3,6 | Warp from (3,6, elevation 3) to MAP_MT_PYRE_EXTERIOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_1F:warp_events:003` | 18,18 | Warp from (18,18, elevation 3) to MAP_ROUTE122 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_1F:warp_events:004` | 4,6 | Warp from (4,6, elevation 3) to MAP_MT_PYRE_EXTERIOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_1F:warp_events:005` | 11,1 | Warp from (11,1, elevation 3) to MAP_MT_PYRE_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_1F:warp_events:006` | 20,9 | Warp from (20,9, elevation 3) to MAP_MT_PYRE_2F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
