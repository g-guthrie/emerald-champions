# SSTidalLowerDeck

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SSTidalLowerDeck/map.json) · [Scripts](../../baseline/source/data/maps/SSTidalLowerDeck/scripts.inc)

## Current map contract

`MAP_SS_TIDAL_LOWER_DECK` · `LAYOUT_SS_TIDAL_LOWER_DECK` · `WEATHER_NONE` · `MUS_SAILING`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SSTidalLowerDeck:object_events:001` | 10,4 | [SSTidalLowerDeck_EventScript_Phillip](../../baseline/source/data/maps/SSTidalLowerDeck/scripts.inc#L4) — SSTidalLowerDeck_EventScript_Phillip at (10,4); Arrrgh! I'm fed up and dog-tired of /  cleaning this huge place! //  Let's have a quick battle! / Little bro, I lost! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SSTidalLowerDeck:object_events:002` | 7,4 | [SSTidalLowerDeck_EventScript_Leonard](../../baseline/source/data/maps/SSTidalLowerDeck/scripts.inc#L9) — SSTidalLowerDeck_EventScript_Leonard at (7,4); This is the bottom of the ship's hull. /  There's plenty of room. /  It'll be alright for a POKéMON battle. / Big bro, I lost! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SSTidalLowerDeck:bg_events:001` | 0,2 | Hidden ITEM_ULTRA_BALL at (0,2); persistent flag FLAG_HIDDEN_ITEM_SS_TIDAL_LOWER_DECK_ULTRA_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `SSTidalLowerDeck:warp_events:001` | 15,2 | Warp from (15,2, elevation 3) to MAP_SS_TIDAL_CORRIDOR warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
