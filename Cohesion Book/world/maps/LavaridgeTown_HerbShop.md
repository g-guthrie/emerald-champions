# LavaridgeTown_HerbShop

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/LavaridgeTown_HerbShop/map.json) · [Scripts](../../baseline/source/data/maps/LavaridgeTown_HerbShop/scripts.inc)

## Current map contract

`MAP_LAVARIDGE_TOWN_HERB_SHOP` · `LAYOUT_LAVARIDGE_TOWN_HERB_SHOP` · `WEATHER_NONE` · `MUS_OLDALE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LavaridgeTown_HerbShop:object_events:001` | 3,2 | [LavaridgeTown_HerbShop_EventScript_Clerk](../../baseline/source/data/maps/LavaridgeTown_HerbShop/scripts.inc#L4) — LavaridgeTown_HerbShop_EventScript_Clerk at (3,2); Welcome to the HERB SHOP, home of /  effective and inexpensive medicine! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_HerbShop:object_events:002` | 7,5 | [LavaridgeTown_HerbShop_EventScript_OldMan](../../baseline/source/data/maps/LavaridgeTown_HerbShop/scripts.inc#L26) — LavaridgeTown_HerbShop_EventScript_OldMan at (7,5); You've come to look at herbal medicine /  in LAVARIDGE? //  That's rather commendable. //  I like you! Take this! / A Fire Stone evolves certain Pokémon. /  Use it from the Bag when ready. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LavaridgeTown_HerbShop:object_events:003` | 9,3 | [LavaridgeTown_HerbShop_EventScript_ExpertM](../../baseline/source/data/maps/LavaridgeTown_HerbShop/scripts.inc#L22) — LavaridgeTown_HerbShop_EventScript_ExpertM at (9,3); Herbal medicine works impressively well. /  But your POKéMON will dislike you for it. /  It must be horribly bitter! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_HerbShop:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_LAVARIDGE_TOWN warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_HerbShop:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_LAVARIDGE_TOWN warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
