# Route134

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route134/map.json) · [Scripts](../../baseline/source/data/maps/Route134/scripts.inc)

## Current map contract

`MAP_ROUTE134` · `LAYOUT_ROUTE134` · `WEATHER_SUNNY` · `MUS_ROUTE119`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route134:object_events:001` | 49,9 | [Route134_EventScript_Jack](../../baseline/source/data/maps/Route134/scripts.inc#L9) — Route134_EventScript_Jack at (49,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route134:object_events:002` | 58,7 | [Route134_EventScript_Laurel](../../baseline/source/data/maps/Route134/scripts.inc#L14) — Route134_EventScript_Laurel at (58,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route134:object_events:003` | 41,23 | [Route134_EventScript_Aaron](../../baseline/source/data/maps/Route134/scripts.inc#L24) — Route134_EventScript_Aaron at (41,23); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route134:object_events:004` | 24,23 | [Route134_EventScript_Alex](../../baseline/source/data/maps/Route134/scripts.inc#L19) — Route134_EventScript_Alex at (24,23); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route134:object_events:005` | 49,16 | [Route134_EventScript_Hitoshi](../../baseline/source/data/maps/Route134/scripts.inc#L29) — Route134_EventScript_Hitoshi at (49,16); Even MACHAMP takes a rest day. /  It rests four arms. I rest two. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route134:object_events:006` | 43,23 | [Route134_EventScript_Marley](../../baseline/source/data/maps/Route134/scripts.inc#L43) — Route134_EventScript_Marley at (43,23); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route134:object_events:007` | 24,30 | [Route134_EventScript_Kelvin](../../baseline/source/data/maps/Route134/scripts.inc#L48) — Route134_EventScript_Kelvin at (24,30); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route134:object_events:008` | 50,16 | [Route134_EventScript_Reyna](../../baseline/source/data/maps/Route134/scripts.inc#L38) — Route134_EventScript_Reyna at (50,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route134:object_events:009` | 63,14 | [Route134_EventScript_Hudson](../../baseline/source/data/maps/Route134/scripts.inc#L33) — Route134_EventScript_Hudson at (63,14); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route134:object_events:010` | 50,17 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DIVE_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_134_DIVE_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route134:object_events:011` | 22,27 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_STAR_PIECE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_134_STAR_PIECE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route134:connections:001` | left | left connection to MAP_SLATEPORT_CITY, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route134:connections:002` | right | right connection to MAP_ROUTE133, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route134:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [Route134_OnResume](../../baseline/source/data/maps/Route134/scripts.inc#L5) — MAP_SCRIPT_ON_RESUME calls Route134_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
