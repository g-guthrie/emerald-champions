# LilycoveCity_House3

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_House3/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_House3/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_HOUSE3` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_House3:object_events:001` | 3,4 | [LilycoveCity_House3_EventScript_GameBoyKid4](../../baseline/source/data/maps/LilycoveCity_House3/scripts.inc#L70) — LilycoveCity_House3_EventScript_GameBoyKid4 at (3,4); We're having MULTI BATTLES, but I know /  I'm going to win. / We like mixing stuff at /  the RECORD CORNER. //  But what gets mixed up? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_House3:object_events:002` | 7,4 | [LilycoveCity_House3_EventScript_PokefanF](../../baseline/source/data/maps/LilycoveCity_House3/scripts.inc#L10) — LilycoveCity_House3_EventScript_PokefanF at (7,4); Oh, my, my! Are you traveling alone? /  But you're so young! Good for you! //  I'm sure my kids could learn a thing /  or two from you! //  Me? I'm a master of {POKEBLOCK}S. //  If I get serious just a little, why, /  I can concoct great {POKEBLOCK}S. //  Would you like to learn from me, /  a master of {POKEBLOCK}S? / Oh? Are you sure? //  You shouldn't always try to do /  everything by yourself, dear! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_House3:object_events:003` | 1,4 | [LilycoveCity_House3_EventScript_GameBoyKid2](../../baseline/source/data/maps/LilycoveCity_House3/scripts.inc#L50) — LilycoveCity_House3_EventScript_GameBoyKid2 at (1,4); We're having MULTI BATTLES, but I know /  I'm going to win. / We like mixing stuff at /  the RECORD CORNER. //  But what gets mixed up? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_House3:object_events:004` | 2,5 | [LilycoveCity_House3_EventScript_GameBoyKid3](../../baseline/source/data/maps/LilycoveCity_House3/scripts.inc#L60) — LilycoveCity_House3_EventScript_GameBoyKid3 at (2,5); We're having MULTI BATTLES, but I know /  I'm going to win. / We like mixing stuff at /  the RECORD CORNER. //  But what gets mixed up? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_House3:object_events:005` | 2,3 | [LilycoveCity_House3_EventScript_GameBoyKid1](../../baseline/source/data/maps/LilycoveCity_House3/scripts.inc#L40) — LilycoveCity_House3_EventScript_GameBoyKid1 at (2,3); We're having MULTI BATTLES, but I know /  I'm going to win. / We like mixing stuff at /  the RECORD CORNER. //  But what gets mixed up? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LilycoveCity_House3:object_events:006` | 7,5 | [LilycoveCity_House3_EventScript_Man](../../baseline/source/data/maps/LilycoveCity_House3/scripts.inc#L30) — LilycoveCity_House3_EventScript_Man at (7,5); When my wife gave birth to quadruplets, /  you bet I was shocked. //  But, now, seeing them play together, /  it makes me happy. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_House3:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_LILYCOVE_CITY warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_House3:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_LILYCOVE_CITY warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_House3:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LilycoveCity_House3_OnTransition](../../baseline/source/data/maps/LilycoveCity_House3/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls LilycoveCity_House3_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
