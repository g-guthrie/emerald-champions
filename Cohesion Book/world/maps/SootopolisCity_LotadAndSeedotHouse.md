# SootopolisCity_LotadAndSeedotHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SootopolisCity_LotadAndSeedotHouse/map.json) · [Scripts](../../baseline/source/data/maps/SootopolisCity_LotadAndSeedotHouse/scripts.inc)

## Current map contract

`MAP_SOOTOPOLIS_CITY_LOTAD_AND_SEEDOT_HOUSE` · `LAYOUT_SOOTOPOLIS_CITY_LOTAD_AND_SEEDOT_HOUSE` · `WEATHER_NONE` · `MUS_SOOTOPOLIS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SootopolisCity_LotadAndSeedotHouse:object_events:001` | 2,4 | [SootopolisCity_LotadAndSeedotHouse_EventScript_LotadBrother](../../baseline/source/data/maps/SootopolisCity_LotadAndSeedotHouse/scripts.inc#L48) — SootopolisCity_LotadAndSeedotHouse_EventScript_LotadBrother at (2,4); Do you know the POKéMON LOTAD? /  It's rarely seen in SOOTOPOLIS. //  I love, I mean love, big LOTAD! //  My big brother says that SEEDOT is /  bigger. //  But that's wrong! Everyone knows that /  LOTAD is a lot bigger. //  Hunh? Do you have a LOTAD? /  P-p-please show me! / {STR_VAR_2}! /  Wow, that is big! //  It might be even bigger than the huge /  SEEDOT my big brother saw. //  Thanks for showing me! /  This is my thanks! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SootopolisCity_LotadAndSeedotHouse:object_events:002` | 5,4 | [SootopolisCity_LotadAndSeedotHouse_EventScript_SeedotBrother](../../baseline/source/data/maps/SootopolisCity_LotadAndSeedotHouse/scripts.inc#L4) — SootopolisCity_LotadAndSeedotHouse_EventScript_SeedotBrother at (5,4); Do you know the POKéMON SEEDOT? /  It's hardly ever seen in SOOTOPOLIS. //  Anyway, I love big SEEDOT. /  The bigger the better. //  But my younger brother, he says that /  LOTAD is bigger. //  That's silly. /  SEEDOT has to be bigger than that! //  Huh? Do you have a SEEDOT with you? /  P-p-please, show me! / {STR_VAR_2}! /  Oh, my gosh, this is a big one! //  It might even beat the big LOTAD /  my younger brother saw! //  Thanks for showing me. /  This is my thanks! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SootopolisCity_LotadAndSeedotHouse:bg_events:001` | 5,1 | [SootopolisCity_LotadAndSeedotHouse_EventScript_SeedotSizeRecord](../../baseline/source/data/maps/SootopolisCity_LotadAndSeedotHouse/scripts.inc#L92) — SootopolisCity_LotadAndSeedotHouse_EventScript_SeedotSizeRecord at (5,1); The biggest SEEDOT in history! /  {STR_VAR_2}'s {STR_VAR_3} giant! //  A SEEDOT bigger than a LOTAD /  always wanted! | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SootopolisCity_LotadAndSeedotHouse:bg_events:002` | 2,1 | [SootopolisCity_LotadAndSeedotHouse_EventScript_LotadSizeRecord](../../baseline/source/data/maps/SootopolisCity_LotadAndSeedotHouse/scripts.inc#L99) — SootopolisCity_LotadAndSeedotHouse_EventScript_LotadSizeRecord at (2,1); The biggest LOTAD in history! /  {STR_VAR_2}'s {STR_VAR_3} colossus! //  A LOTAD bigger than a SEEDOT /  always wanted! | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SootopolisCity_LotadAndSeedotHouse:warp_events:001` | 3,6 | Warp from (3,6, elevation 0) to MAP_SOOTOPOLIS_CITY warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SootopolisCity_LotadAndSeedotHouse:warp_events:002` | 4,6 | Warp from (4,6, elevation 0) to MAP_SOOTOPOLIS_CITY warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
