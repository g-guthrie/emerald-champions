# VerdanturfTown_FriendshipRatersHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/VerdanturfTown_FriendshipRatersHouse/map.json) · [Scripts](../../baseline/source/data/maps/VerdanturfTown_FriendshipRatersHouse/scripts.inc)

## Current map contract

`MAP_VERDANTURF_TOWN_FRIENDSHIP_RATERS_HOUSE` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_VERDANTURF`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VerdanturfTown_FriendshipRatersHouse:object_events:001` | 3,4 | [VerdanturfTown_FriendshipRatersHouse_EventScript_MegaGift_RAICHUNITE_X](../../baseline/source/data/maps/VerdanturfTown_FriendshipRatersHouse/scripts.inc#L150) — VerdanturfTown_FriendshipRatersHouse_EventScript_MegaGift_RAICHUNITE_X at (3,4); Let me see your POKéMON. /  I'll check to see how much it likes you. //  Oh. /  Your POKéMON… / It adores you. /  It can't possibly love you any more. /  I even feel happy seeing it. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `VerdanturfTown_FriendshipRatersHouse:object_events:002` | 4,4 | [VerdanturfTown_FriendshipRatersHouse_EventScript_Pikachu](../../baseline/source/data/maps/VerdanturfTown_FriendshipRatersHouse/scripts.inc#L100) — VerdanturfTown_FriendshipRatersHouse_EventScript_Pikachu at (4,4); PIKACHU: Pika pika! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown_FriendshipRatersHouse:object_events:003` | 7,4 | [VerdanturfTown_FriendshipRatersHouse_EventScript_Trader](../../baseline/source/data/maps/VerdanturfTown_FriendshipRatersHouse/scripts.inc#L4) — VerdanturfTown_FriendshipRatersHouse_EventScript_Trader at (7,4); This {STR_VAR_2} needs a caring /  TRAINER. Trade a {STR_VAR_1} for it? / Thank you. Friendship is the best /  training CHANSEY can receive. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `VerdanturfTown_FriendshipRatersHouse:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_VERDANTURF_TOWN warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown_FriendshipRatersHouse:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_VERDANTURF_TOWN warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
