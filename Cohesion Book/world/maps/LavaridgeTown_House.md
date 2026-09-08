# LavaridgeTown_House

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/LavaridgeTown_House/map.json) · [Scripts](../../baseline/source/data/maps/LavaridgeTown_House/scripts.inc)

## Current map contract

`MAP_LAVARIDGE_TOWN_HOUSE` · `LAYOUT_HOUSE3` · `WEATHER_NONE` · `MUS_OLDALE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LavaridgeTown_House:object_events:001` | 2,3 | [LavaridgeTown_House_EventScript_MegaGift_MACHAMPITE](../../baseline/source/data/maps/LavaridgeTown_House/scripts.inc#L45) — LavaridgeTown_House_EventScript_MegaGift_MACHAMPITE at (2,3); My wife's warming an EGG in the hot /  springs. This is what she told me. //  She left two POKéMON with the DAY CARE. /  And they discovered that EGG! / Machamp carried the stones that /  built some of these old baths. //  This one is small enough to carry /  yourself. Let a Machamp hold it. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LavaridgeTown_House:object_events:002` | 6,6 | [LavaridgeTown_House_EventScript_Zigzagoon](../../baseline/source/data/maps/LavaridgeTown_House/scripts.inc#L8) — LavaridgeTown_House_EventScript_Zigzagoon at (6,6); ZIGZAGOON: Pshoo! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown_House:object_events:003` | 8,4 | [LavaridgeTown_House_EventScript_MegaGift_BLAZIKENITE](../../baseline/source/data/maps/LavaridgeTown_House/scripts.inc#L28) — LavaridgeTown_House_EventScript_MegaGift_BLAZIKENITE at (8,4); I learned footwork by watching /  Blaziken train on the hot rocks. //  A flame needs room to grow. /  Take this stone on your journey. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LavaridgeTown_House:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_LAVARIDGE_TOWN warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown_House:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_LAVARIDGE_TOWN warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
