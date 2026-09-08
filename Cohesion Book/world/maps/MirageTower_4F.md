# MirageTower_4F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/MirageTower_4F/map.json) · [Scripts](../../baseline/source/data/maps/MirageTower_4F/scripts.inc)

## Current map contract

`MAP_MIRAGE_TOWER_4F` · `LAYOUT_MIRAGE_TOWER_4F` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MirageTower_4F:object_events:001` | 5,4 | [MirageTower_4F_EventScript_RootFossil](../../baseline/source/data/maps/MirageTower_4F/scripts.inc#L4) — MirageTower_4F_EventScript_RootFossil at (5,4); You found the ROOT FOSSIL. //  If this FOSSIL is taken, the ground /  around it will likely crumble away… //  Take the ROOT FOSSIL anyway? / {PLAYER} left the ROOT FOSSIL alone. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MirageTower_4F:object_events:002` | 7,4 | [MirageTower_4F_EventScript_ClawFossil](../../baseline/source/data/maps/MirageTower_4F/scripts.inc#L25) — MirageTower_4F_EventScript_ClawFossil at (7,4); You found the CLAW FOSSIL. //  If this FOSSIL is taken, the ground /  around it will likely crumble away… //  Take the CLAW FOSSIL anyway? / {PLAYER} left the CLAW FOSSIL alone. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `MirageTower_4F:object_events:003` | 6,7 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (6,7); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `MirageTower_4F:warp_events:001` | 1,4 | Warp from (1,4, elevation 3) to MAP_MIRAGE_TOWER_3F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
