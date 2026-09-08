# Route133

**REVISE.** The final Vial nurse correctly appears only at capacity2 and hides at3; preserve its staged dependency on the earlier Blob reward. A simple rescue interaction is an appropriate exploration reward.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route133/map.json) · [Scripts](../../baseline/source/data/maps/Route133/scripts.inc)

## Current map contract

`MAP_ROUTE133` · `LAYOUT_ROUTE133` · `WEATHER_SUNNY` · `MUS_ROUTE119`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route133:object_events:001` | 68,27 | [Route133_EventScript_Franklin](../../baseline/source/data/maps/Route133/scripts.inc#L61) — Route133_EventScript_Franklin at (68,27); WALREIN offered me a ride. /  The seat was colder than expected. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route133:object_events:002` | 13,3 | [Route133_EventScript_Linda](../../baseline/source/data/maps/Route133/scripts.inc#L70) — Route133_EventScript_Linda at (13,3); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route133:object_events:003` | 68,28 | [Route133_EventScript_Debra](../../baseline/source/data/maps/Route133/scripts.inc#L65) — Route133_EventScript_Debra at (68,28); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route133:object_events:004` | 53,12 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BIG_PEARL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_133_BIG_PEARL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route133:object_events:005` | 8,10 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_STAR_PIECE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_133_STAR_PIECE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route133:object_events:006` | 7,14 | [Route133_EventScript_Beck](../../baseline/source/data/maps/Route133/scripts.inc#L80) — Route133_EventScript_Beck at (7,14); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route133:object_events:007` | 37,15 | [Route133_EventScript_Warren](../../baseline/source/data/maps/Route133/scripts.inc#L75) — Route133_EventScript_Warren at (37,15); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route133:object_events:008` | 56,11 | [Route133_EventScript_Mollie](../../baseline/source/data/maps/Route133/scripts.inc#L85) — Route133_EventScript_Mollie at (56,11); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route133:object_events:009` | 56,15 | [Route133_EventScript_Conor](../../baseline/source/data/maps/Route133/scripts.inc#L90) — Route133_EventScript_Conor at (56,15); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route133:object_events:010` | 48,28 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAX_REVIVE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_133_MAX_REVIVE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route133:object_events:011` | 69,7 | [Route133_EventScript_VialNurse](../../baseline/source/data/maps/Route133/scripts.inc#L15) — Route133_EventScript_VialNurse at (69,7); We were swept onto this island by /  Route 133's current. //  May I borrow your help getting /  Chansey and me safely ashore? / Thank you! Before we go, I tuned your /  Poké Vial one final time. //  Centers will now refill three charges. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route133:object_events:012` | 70,7 | [Route133_EventScript_VialChansey](../../baseline/source/data/maps/Route133/scripts.inc#L36) — Route133_EventScript_VialChansey at (70,7); Chansey! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route133:connections:001` | left | left connection to MAP_ROUTE134, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route133:connections:002` | right | right connection to MAP_ROUTE132, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route133:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route133_OnTransition](../../baseline/source/data/maps/Route133/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route133_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
