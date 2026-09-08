# PetalburgCity_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/PetalburgCity_Mart/map.json) · [Scripts](../../baseline/source/data/maps/PetalburgCity_Mart/scripts.inc)

## Current map contract

`MAP_PETALBURG_CITY_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PetalburgCity_Mart:object_events:001` | 1,3 | [PetalburgCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/PetalburgCity_Mart/scripts.inc#L4) — PetalburgCity_Mart_EventScript_Clerk at (1,3); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PetalburgCity_Mart:object_events:002` | 1,4 | [PetalburgCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/PetalburgCity_Mart/scripts.inc#L4) — PetalburgCity_Mart_EventScript_Clerk at (1,4); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PetalburgCity_Mart:object_events:003` | 9,4 | [PetalburgCity_Mart_EventScript_Man](../../baseline/source/data/maps/PetalburgCity_Mart/scripts.inc#L61) — PetalburgCity_Mart_EventScript_Man at (9,4); Do you carry ANTIDOTES? //  Poison no longer drains HP while /  walking, but it still matters in battle. //  An ANTIDOTE clears it before your /  next fight. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgCity_Mart:object_events:004` | 6,3 | [PetalburgCity_Mart_EventScript_Boy](../../baseline/source/data/maps/PetalburgCity_Mart/scripts.inc#L57) — PetalburgCity_Mart_EventScript_Boy at (6,3); Do you use REPEL? /  It keeps POKéMON away, so it's /  useful when you're in a hurry. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgCity_Mart:object_events:005` | 5,5 | [PetalburgCity_Mart_EventScript_Woman](../../baseline/source/data/maps/PetalburgCity_Mart/scripts.inc#L53) — PetalburgCity_Mart_EventScript_Woman at (5,5); Even if a POKéMON is weak now, /  it will grow stronger. //  The most important thing is love! /  Love for your POKéMON! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgCity_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_PETALBURG_CITY warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_PETALBURG_CITY warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
