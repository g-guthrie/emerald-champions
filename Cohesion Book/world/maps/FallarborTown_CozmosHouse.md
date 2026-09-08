# FallarborTown_CozmosHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/FallarborTown_CozmosHouse/map.json) · [Scripts](../../baseline/source/data/maps/FallarborTown_CozmosHouse/scripts.inc)

## Current map contract

`MAP_FALLARBOR_TOWN_COZMOS_HOUSE` · `LAYOUT_HOUSE1` · `WEATHER_NONE` · `MUS_FALLARBOR`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FallarborTown_CozmosHouse:object_events:001` | 6,4 | [FallarborTown_CozmosHouse_EventScript_ProfCozmo](../../baseline/source/data/maps/FallarborTown_CozmosHouse/scripts.inc#L4) — FallarborTown_CozmosHouse_EventScript_ProfCozmo at (6,4); PROF. COZMO: Oh… /  I never should have let myself be /  conned into telling TEAM MAGMA where /  you can find METEORITES… //  That METEORITE from METEOR FALLS… /  It's never going to be mine now… / Oh! Is that the METEORITE TEAM MAGMA /  took from METEOR FALLS? //  Please, may I have it? I can offer this /  rare DAWN STONE in exchange. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FallarborTown_CozmosHouse:object_events:002` | 5,6 | [FallarborTown_CozmosHouse_EventScript_MegaGift_CLEFABLITE](../../baseline/source/data/maps/FallarborTown_CozmosHouse/scripts.inc#L112) — FallarborTown_CozmosHouse_EventScript_MegaGift_CLEFABLITE at (5,6); PROF. COZMO went off to METEOR FALLS /  on ROUTE 114 with some people from /  TEAM MAGMA. / Poor PROF. COZMO… /  He's so depressed… I feel sorry for him. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `FallarborTown_CozmosHouse:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_FALLARBOR_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown_CozmosHouse:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_FALLARBOR_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
