# VerdanturfTown_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/VerdanturfTown_Mart/map.json) · [Scripts](../../baseline/source/data/maps/VerdanturfTown_Mart/scripts.inc)

## Current map contract

`MAP_VERDANTURF_TOWN_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VerdanturfTown_Mart:object_events:001` | 1,3 | [VerdanturfTown_Mart_EventScript_Clerk](../../baseline/source/data/maps/VerdanturfTown_Mart/scripts.inc#L14) — VerdanturfTown_Mart_EventScript_Clerk at (1,3); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown_Mart:object_events:002` | 1,4 | [VerdanturfTown_Mart_EventScript_Clerk](../../baseline/source/data/maps/VerdanturfTown_Mart/scripts.inc#L14) — VerdanturfTown_Mart_EventScript_Clerk at (1,4); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown_Mart:object_events:003` | 5,4 | [VerdanturfTown_Mart_EventScript_Boy](../../baseline/source/data/maps/VerdanturfTown_Mart/scripts.inc#L39) — VerdanturfTown_Mart_EventScript_Boy at (5,4); A LUXURY BALL helps a caught POKéMON /  grow friendly more quickly. //  The right Ball can make team building /  feel like part of the adventure. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown_Mart:object_events:004` | 8,5 | [VerdanturfTown_Mart_EventScript_ExpertF](../../baseline/source/data/maps/VerdanturfTown_Mart/scripts.inc#L43) — VerdanturfTown_Mart_EventScript_ExpertF at (8,5); They don't seem to sell any winning /  strategy guides for the BATTLE TENT… //  It seems one must rely on one's /  own wits after all… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown_Mart:object_events:005` | 3,2 | [VerdanturfTown_Mart_EventScript_Lass](../../baseline/source/data/maps/VerdanturfTown_Mart/scripts.inc#L47) — VerdanturfTown_Mart_EventScript_Lass at (3,2); The NEST BALL works better on /  lower-level wild POKéMON. //  It is one of several specialty BALLS /  sold around HOENN. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown_Mart:object_events:006` | 8,2 | [VerdanturfTown_Mart_EventScript_MoveSpecialist](../../baseline/source/data/maps/VerdanturfTown_Mart/scripts.inc#L4) — VerdanturfTown_Mart_EventScript_MoveSpecialist at (8,2); I used to reward patient POKéMON /  with the move PAYBACK. //  The CENTER specialist teaches it now /  whenever the species can learn it. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VerdanturfTown_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_VERDANTURF_TOWN warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VerdanturfTown_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_VERDANTURF_TOWN warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
