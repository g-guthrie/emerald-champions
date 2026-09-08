# Route107

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/Route107/map.json) · [Scripts](../../baseline/source/data/maps/Route107/scripts.inc)

## Current map contract

`MAP_ROUTE107` · `LAYOUT_ROUTE107` · `WEATHER_SUNNY` · `MUS_ROUTE104`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route107:object_events:001` | 41,10 | [Route107_EventScript_Darrin](../../baseline/source/data/maps/Route107/scripts.inc#L4) — Route107_EventScript_Darrin at (41,10); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route107:object_events:002` | 23,11 | [Route107_EventScript_Tony](../../baseline/source/data/maps/Route107/scripts.inc#L9) — Route107_EventScript_Tony at (23,11); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route107:object_events:003` | 16,7 | [Route107_EventScript_Denise](../../baseline/source/data/maps/Route107/scripts.inc#L30) — Route107_EventScript_Denise at (16,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route107:object_events:004` | 50,11 | [Route107_EventScript_Beth](../../baseline/source/data/maps/Route107/scripts.inc#L35) — Route107_EventScript_Beth at (50,11); I swim for the scenery. /  The scenery keeps splashing me. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route107:object_events:005` | 33,4 | [Route107_EventScript_Lisa](../../baseline/source/data/maps/Route107/scripts.inc#L39) — Route107_EventScript_Lisa at (33,4); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route107:object_events:006` | 32,4 | [Route107_EventScript_Ray](../../baseline/source/data/maps/Route107/scripts.inc#L44) — Route107_EventScript_Ray at (32,4); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route107:object_events:007` | 50,5 | [Route107_EventScript_Camron](../../baseline/source/data/maps/Route107/scripts.inc#L49) — Route107_EventScript_Camron at (50,5); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route107:connections:001` | left | left connection to MAP_DEWFORD_TOWN, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route107:connections:002` | right | right connection to MAP_ROUTE108, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
