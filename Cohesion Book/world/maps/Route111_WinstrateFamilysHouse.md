# Route111_WinstrateFamilysHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route111_WinstrateFamilysHouse/map.json) · [Scripts](../../baseline/source/data/maps/Route111_WinstrateFamilysHouse/scripts.inc)

## Current map contract

`MAP_ROUTE111_WINSTRATE_FAMILYS_HOUSE` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route111_WinstrateFamilysHouse:object_events:001` | 7,5 | [Route111_WinstrateFamilysHouse_EventScript_Vivi](../../baseline/source/data/maps/Route111_WinstrateFamilysHouse/scripts.inc#L29) — Route111_WinstrateFamilysHouse_EventScript_Vivi at (7,5); Mommy is stronger than Daddy. //  I'm stronger than Mommy. //  And Grandma's stronger than me! //  But my big brother is even stronger /  than Grandma. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route111_WinstrateFamilysHouse:object_events:002` | 4,5 | [Route111_WinstrateFamilysHouse_EventScript_Victor](../../baseline/source/data/maps/Route111_WinstrateFamilysHouse/scripts.inc#L4) — Route111_WinstrateFamilysHouse_EventScript_Victor at (4,5); You're the first TRAINER I've seen who /  deploys POKéMON so masterfully. //  But, I should tell you--my son is /  stronger than you. //  He even took the POKéMON LEAGUE /  challenge, I'll have you know. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route111_WinstrateFamilysHouse:object_events:003` | 7,4 | [Route111_WinstrateFamilysHouse_EventScript_Victoria](../../baseline/source/data/maps/Route111_WinstrateFamilysHouse/scripts.inc#L12) — Route111_WinstrateFamilysHouse_EventScript_Victoria at (7,4); You defeated all four of us without /  a chance to rest. That's remarkable! //  Take these berries from our garden. /  You have earned a little harvest. / Plant those berries or save them for /  the BERRY MASTER on ROUTE 123. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route111_WinstrateFamilysHouse:object_events:004` | 4,4 | [Route111_WinstrateFamilysHouse_EventScript_Vicky](../../baseline/source/data/maps/Route111_WinstrateFamilysHouse/scripts.inc#L37) — Route111_WinstrateFamilysHouse_EventScript_Vicky at (4,4); There's no question that you're strong. //  But if you were to battle my grandson, /  you'd end up crying in frustration. //  He's much stronger than any TRAINER /  our family knows. //  He must be challenging the POKéMON /  LEAGUE CHAMPION by now. //  Knowing my grandson, he could be the /  CHAMPION already! / My grandson must be challenging the /  POKéMON LEAGUE CHAMPION by now. //  Knowing my grandson, he could be the /  CHAMPION already! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route111_WinstrateFamilysHouse:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_ROUTE111 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route111_WinstrateFamilysHouse:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_ROUTE111 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
