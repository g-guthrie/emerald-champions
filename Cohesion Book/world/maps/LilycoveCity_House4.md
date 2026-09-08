# LilycoveCity_House4

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/LilycoveCity_House4/map.json) · [Scripts](../../baseline/source/data/maps/LilycoveCity_House4/scripts.inc)

## Current map contract

`MAP_LILYCOVE_CITY_HOUSE4` · `LAYOUT_HOUSE1` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LilycoveCity_House4:object_events:001` | 1,4 | [LilycoveCity_House4_EventScript_MegaGift_STARMINITE](../../baseline/source/data/maps/LilycoveCity_House4/scripts.inc#L40) — LilycoveCity_House4_EventScript_MegaGift_STARMINITE at (1,4); This planet's biggest mysteries are /  at the bottom of the sea. //  Somebody said that, but I don't know… / The sea leaves more than shells /  on our shore after a storm. //  This stone shines like a Starmie. /  Let one carry it beneath the stars. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_House4:object_events:002` | 7,4 | [LilycoveCity_House4_EventScript_MegaGift_LAPRASITE](../../baseline/source/data/maps/LilycoveCity_House4/scripts.inc#L23) — LilycoveCity_House4_EventScript_MegaGift_LAPRASITE at (7,4); There's a deep underwater trench /  between MOSSDEEP and SOOTOPOLIS. //  That's what someone told me, anyway. / A Lapras once carried my family /  home through a terrible storm. //  I kept this stone in gratitude. /  Please find a Lapras to give it to. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LilycoveCity_House4:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_LILYCOVE_CITY warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LilycoveCity_House4:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_LILYCOVE_CITY warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
