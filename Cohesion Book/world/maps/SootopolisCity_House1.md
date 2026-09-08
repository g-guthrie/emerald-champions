# SootopolisCity_House1

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SootopolisCity_House1/map.json) · [Scripts](../../baseline/source/data/maps/SootopolisCity_House1/scripts.inc)

## Current map contract

`MAP_SOOTOPOLIS_CITY_HOUSE1` · `LAYOUT_SOOTOPOLIS_CITY_HOUSE1` · `WEATHER_NONE` · `MUS_SOOTOPOLIS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SootopolisCity_House1:object_events:001` | 2,4 | [SootopolisCity_House1_EventScript_RazorClawBlackBelt](../../baseline/source/data/maps/SootopolisCity_House1/scripts.inc#L4) — SootopolisCity_House1_EventScript_RazorClawBlackBelt at (2,4); For thirty years I've remained in /  SOOTOPOLIS honing my skills. //  This RAZOR CLAW is just as sharp. /  I bequeath it to you! / A Razor Claw evolves Sneasel. /  Use it at night when ready. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SootopolisCity_House1:object_events:002` | 2,3 | [SootopolisCity_House1_EventScript_Kecleon](../../baseline/source/data/maps/SootopolisCity_House1/scripts.inc#L21) — SootopolisCity_House1_EventScript_Kecleon at (2,3); KECLEON: Puu puhyaah. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SootopolisCity_House1:warp_events:001` | 3,6 | Warp from (3,6, elevation 0) to MAP_SOOTOPOLIS_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_House1:warp_events:002` | 4,6 | Warp from (4,6, elevation 0) to MAP_SOOTOPOLIS_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
