# DewfordTown_House2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/DewfordTown_House2/map.json) · [Scripts](../../baseline/source/data/maps/DewfordTown_House2/scripts.inc)

## Current map contract

`MAP_DEWFORD_TOWN_HOUSE2` · `LAYOUT_HOUSE4` · `WEATHER_NONE` · `MUS_DEWFORD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `DewfordTown_House2:object_events:001` | 6,5 | [DewfordTown_House2_EventScript_MegaGift_GYARADOSITE](../../baseline/source/data/maps/DewfordTown_House2/scripts.inc#L67) — DewfordTown_House2_EventScript_MegaGift_GYARADOSITE at (6,5); Gorge your eyes on this rare /  REAPER CLOTH! Spooky, yet stylish! //  You appreciate my dazzling taste. /  Here, I want you to have it! / Oh, you don't have room? //  This REAPER CLOTH is rare! Make room /  and come back for it. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `DewfordTown_House2:object_events:002` | 2,3 | [DewfordTown_House2_EventScript_MegaGift_KINGLERITE](../../baseline/source/data/maps/DewfordTown_House2/scripts.inc#L50) — DewfordTown_House2_EventScript_MegaGift_KINGLERITE at (2,3); Wow, you bothered to cross the sea /  to visit DEWFORD? //  Did you maybe come here because you /  heard about BRAWLY? //  He's so cool… /  Everyone idolizes him. / I followed a Krabby along the shore /  and found this where it was digging. //  When its claw is big and strong, /  this stone could make it stronger! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `DewfordTown_House2:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_DEWFORD_TOWN warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordTown_House2:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_DEWFORD_TOWN warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
