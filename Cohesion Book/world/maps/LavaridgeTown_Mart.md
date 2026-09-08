# LavaridgeTown_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/LavaridgeTown_Mart/map.json) · [Scripts](../../baseline/source/data/maps/LavaridgeTown_Mart/scripts.inc)

## Current map contract

`MAP_LAVARIDGE_TOWN_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LavaridgeTown_Mart:object_events:001` | 1,3 | [LavaridgeTown_Mart_EventScript_Clerk](../../baseline/source/data/maps/LavaridgeTown_Mart/scripts.inc#L4) — LavaridgeTown_Mart_EventScript_Clerk at (1,3); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_Mart:object_events:002` | 1,4 | [LavaridgeTown_Mart_EventScript_Clerk](../../baseline/source/data/maps/LavaridgeTown_Mart/scripts.inc#L4) — LavaridgeTown_Mart_EventScript_Clerk at (1,4); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_Mart:object_events:003` | 4,2 | [LavaridgeTown_Mart_EventScript_ExpertM](../../baseline/source/data/maps/LavaridgeTown_Mart/scripts.inc#L26) — LavaridgeTown_Mart_EventScript_ExpertM at (4,2); TRAINER battles don't allow items /  from the BAG. //  Prepare your moves and held items /  before the battle begins! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_Mart:object_events:004` | 9,5 | [LavaridgeTown_Mart_EventScript_OldWoman](../../baseline/source/data/maps/LavaridgeTown_Mart/scripts.inc#L30) — LavaridgeTown_Mart_EventScript_OldWoman at (9,5); On MT. CHIMNEY's peak, there's a local /  specialty sold beside the crater. //  Give it to a POKéMON--it will be elated. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_LAVARIDGE_TOWN warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_LAVARIDGE_TOWN warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
