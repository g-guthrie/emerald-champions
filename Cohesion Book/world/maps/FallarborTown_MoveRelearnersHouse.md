# FallarborTown_MoveRelearnersHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/FallarborTown_MoveRelearnersHouse/map.json) · [Scripts](../../baseline/source/data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc)

## Current map contract

`MAP_FALLARBOR_TOWN_MOVE_RELEARNERS_HOUSE` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_FALLARBOR`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FallarborTown_MoveRelearnersHouse:object_events:001` | 4,4 | [FallarborTown_MoveRelearnersHouse_EventScript_MoveRelearner](../../baseline/source/data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc#L51) — FallarborTown_MoveRelearnersHouse_EventScript_MoveRelearner at (4,4); Hey there, TRAINER! //  If you want your POKéMON to be the very /  best in all of HOENN, you've come to /  the right place! //  I'm EVIE, and this is my sister, IVY. / IVY: We run the training programs that /  seriously beef up your team! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FallarborTown_MoveRelearnersHouse:object_events:002` | 7,4 | [FallarborTown_MoveRelearnersHouse_EventScript_StatPointTrainer](../../baseline/source/data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc#L44) — FallarborTown_MoveRelearnersHouse_EventScript_StatPointTrainer at (7,4); Hey there, TRAINER! //  If you want your POKéMON to be the very /  best in all of HOENN, you've come to /  the right place! //  I'm EVIE, and this is my sister, IVY. / IVY: We run the training programs that /  seriously beef up your team! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown_MoveRelearnersHouse:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_FALLARBOR_TOWN warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown_MoveRelearnersHouse:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_FALLARBOR_TOWN warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
