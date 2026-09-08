# FortreeCity_House2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/FortreeCity_House2/map.json) · [Scripts](../../baseline/source/data/maps/FortreeCity_House2/scripts.inc)

## Current map contract

`MAP_FORTREE_CITY_HOUSE2` · `LAYOUT_FORTREE_CITY_HOUSE2` · `WEATHER_NONE` · `MUS_FORTREE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FortreeCity_House2:object_events:001` | 2,3 | [FortreeCity_House2_EventScript_DubiousDiscGiver](../../baseline/source/data/maps/FortreeCity_House2/scripts.inc#L4) — FortreeCity_House2_EventScript_DubiousDiscGiver at (2,3); People… POKéMON… //  Their hidden powers are aroused by /  living in natural environments… / Let this old woman see if your hidden /  power has awoken… //  I hold a coin in my hand. //  Now, tell me, have I palmed it in /  the right hand? Or in the left? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FortreeCity_House2:object_events:002` | 6,3 | [FortreeCity_House2_EventScript_SleepTalkTutor](../../baseline/source/data/scripts/move_tutors.inc#L83) — FortreeCity_House2_EventScript_SleepTalkTutor at (6,3); shared behavior TUTOR | **KEEP** · [W-C-TUTOR](../common-contracts.md#w-c-tutor) |
| `FortreeCity_House2:warp_events:001` | 3,5 | Warp from (3,5, elevation 0) to MAP_FORTREE_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity_House2:warp_events:002` | 4,5 | Warp from (4,5, elevation 0) to MAP_FORTREE_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
