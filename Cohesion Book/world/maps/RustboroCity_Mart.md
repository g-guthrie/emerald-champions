# RustboroCity_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_Mart/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_Mart/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_Mart:object_events:001` | 1,3 | [RustboroCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/RustboroCity_Mart/scripts.inc#L13) — RustboroCity_Mart_EventScript_Clerk at (1,3); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RustboroCity_Mart:object_events:002` | 2,6 | [RustboroCity_Mart_EventScript_Boy](../../baseline/source/data/maps/RustboroCity_Mart/scripts.inc#L68) — RustboroCity_Mart_EventScript_Boy at (2,6); My POKéMON evolved. /  It has a lot of HP now. //  I should buy SUPER POTIONS for it /  instead of ordinary POTIONS. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Mart:object_events:003` | 8,4 | [RustboroCity_Mart_EventScript_PokefanF](../../baseline/source/data/maps/RustboroCity_Mart/scripts.inc#L64) — RustboroCity_Mart_EventScript_PokefanF at (8,4); I'm buying some PARLYZ HEALS and /  ANTIDOTES. //  Just in case I run into SHROOMISH /  in PETALBURG WOODS. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Mart:object_events:004` | 8,2 | [RustboroCity_Mart_EventScript_BugCatcher](../../baseline/source/data/maps/RustboroCity_Mart/scripts.inc#L72) — RustboroCity_Mart_EventScript_BugCatcher at (8,2); I'm getting an ESCAPE ROPE just in /  case I get lost in a cave. //  I just need to use it to get back to /  the entrance. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Mart:object_events:005` | 5,2 | [RustboroCity_Mart_EventScript_MoveSpecialist](../../baseline/source/data/maps/RustboroCity_Mart/scripts.inc#L4) — RustboroCity_Mart_EventScript_MoveSpecialist at (5,2); FALSE SWIPE used to be my special TM. //  The POKéMON CENTER specialist now /  teaches it whenever it is legal. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `RustboroCity_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_RUSTBORO_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_RUSTBORO_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
