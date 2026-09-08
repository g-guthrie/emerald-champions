# PetalburgCity_House2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/PetalburgCity_House2/map.json) · [Scripts](../../baseline/source/data/maps/PetalburgCity_House2/scripts.inc)

## Current map contract

`MAP_PETALBURG_CITY_HOUSE2` · `LAYOUT_HOUSE_WITH_BED` · `WEATHER_NONE` · `MUS_PETALBURG`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PetalburgCity_House2:object_events:001` | 2,5 | [PetalburgCity_House2_EventScript_Woman](../../baseline/source/data/maps/PetalburgCity_House2/scripts.inc#L4) — PetalburgCity_House2_EventScript_Woman at (2,5); NORMAN became our town's new /  GYM LEADER. //  I think he called his family over from /  somewhere far away. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgCity_House2:object_events:002` | 7,6 | [PetalburgCity_House2_EventScript_SchoolKid](../../baseline/source/data/maps/PetalburgCity_House2/scripts.inc#L8) — PetalburgCity_House2_EventScript_SchoolKid at (7,6); I battled NORMAN once, but, whew, /  he was way too strong. //  How would I put it? //  I just got the feeling that he /  lives for POKéMON. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgCity_House2:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_PETALBURG_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgCity_House2:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_PETALBURG_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
