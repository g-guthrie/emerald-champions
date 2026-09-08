# VerdanturfTown_House

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/VerdanturfTown_House/map.json) · [Scripts](../../baseline/source/data/maps/VerdanturfTown_House/scripts.inc)

## Current map contract

`MAP_VERDANTURF_TOWN_HOUSE` · `LAYOUT_HOUSE1` · `WEATHER_NONE` · `MUS_VERDANTURF`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VerdanturfTown_House:object_events:001` | 4,5 | [VerdanturfTown_House_EventScript_Woman1](../../baseline/source/data/maps/VerdanturfTown_House/scripts.inc#L4) — VerdanturfTown_House_EventScript_Woman1 at (4,5); Far away, deep in EVER GRANDE CITY, /  is the POKéMON LEAGUE. //  The TRAINERS who gather there are /  all frighteningly well skilled. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown_House:object_events:002` | 4,4 | [VerdanturfTown_House_EventScript_MegaGift_GALLADITE](../../baseline/source/data/maps/VerdanturfTown_House/scripts.inc#L27) — VerdanturfTown_House_EventScript_MegaGift_GALLADITE at (4,4); In the POKéMON LEAGUE, I think the /  rules say that you have to battle the /  ELITE FOUR all in a row. //  If you lose to any of them, you have /  to begin your challenge again from the /  first one. / Strength can be quiet. A Gallade /  uses its blades to protect others. //  I think this stone belongs with /  someone who understands that. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `VerdanturfTown_House:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_VERDANTURF_TOWN warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown_House:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_VERDANTURF_TOWN warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
