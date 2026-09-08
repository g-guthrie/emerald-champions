# FallarborTown_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/FallarborTown_Mart/map.json) · [Scripts](../../baseline/source/data/maps/FallarborTown_Mart/scripts.inc)

## Current map contract

`MAP_FALLARBOR_TOWN_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FallarborTown_Mart:object_events:001` | 1,3 | [FallarborTown_Mart_EventScript_Clerk](../../baseline/source/data/maps/FallarborTown_Mart/scripts.inc#L13) — FallarborTown_Mart_EventScript_Clerk at (1,3); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown_Mart:object_events:002` | 1,4 | [FallarborTown_Mart_EventScript_Clerk](../../baseline/source/data/maps/FallarborTown_Mart/scripts.inc#L13) — FallarborTown_Mart_EventScript_Clerk at (1,4); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown_Mart:object_events:003` | 5,3 | [FallarborTown_Mart_EventScript_Woman](../../baseline/source/data/maps/FallarborTown_Mart/scripts.inc#L33) — FallarborTown_Mart_EventScript_Woman at (5,3); I'm having a hard time deciding if I /  should make my SKITTY evolve or not. //  I only have to use this MOON STONE, /  but it's so hard to decide… //  If I make it evolve, it will become /  much stronger. //  But it will look so different, too. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown_Mart:object_events:004` | 9,6 | [FallarborTown_Mart_EventScript_PokefanM](../../baseline/source/data/maps/FallarborTown_Mart/scripts.inc#L37) — FallarborTown_Mart_EventScript_PokefanM at (9,6); This NUGGET I found here… /  I suppose I'll have to sell it, seeing /  as how it has no other use. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown_Mart:object_events:005` | 2,6 | [FallarborTown_Mart_EventScript_Skitty](../../baseline/source/data/maps/FallarborTown_Mart/scripts.inc#L41) — FallarborTown_Mart_EventScript_Skitty at (2,6); SKITTY: Miyao? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown_Mart:object_events:006` | 3,2 | [FallarborTown_Mart_EventScript_MoveSpecialist](../../baseline/source/data/maps/FallarborTown_Mart/scripts.inc#L4) — FallarborTown_Mart_EventScript_MoveSpecialist at (3,2); I once handed out DRAIN PUNCH as a TM. //  Now every POKéMON CENTER specialist /  teaches any legal move for free. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FallarborTown_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_FALLARBOR_TOWN warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FallarborTown_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_FALLARBOR_TOWN warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
