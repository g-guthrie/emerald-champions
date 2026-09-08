# Route103

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/Route103/map.json) · [Scripts](../../baseline/source/data/maps/Route103/scripts.inc)

## Current map contract

`MAP_ROUTE103` · `LAYOUT_ROUTE103` · `WEATHER_SUNNY` · `MUS_ROUTE101`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route103:object_events:001` | 49,12 | [Route103_EventScript_Man](../../baseline/source/data/maps/Route103/scripts.inc#L194) — Route103_EventScript_Man at (49,12); If you cross the sea from here, /  it'll be a shortcut to OLDALE TOWN. //  Fufufu, that's useful, isn't it? | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · INTRO-01 |
| `Route103:object_events:002` | 10,3 | [Route103_EventScript_Rival](../../baseline/source/data/maps/Route103/scripts.inc#L20) — Route103_EventScript_Rival at (10,3); MAY: Let's see… The POKéMON found /  on ROUTE 103 include… / MAY: There you are, {PLAYER}{KUN}! //  Dad said you helped him out. I wanted to /  meet the POKéMON that was with you! //  Mine has the type advantage, though. /  Did you use OLDALE's LEVELER? //  Let's see how we do together! | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `Route103:object_events:003` | 71,11 | [Route103_EventScript_Daisy](../../baseline/source/data/maps/Route103/scripts.inc#L202) — Route103_EventScript_Daisy at (71,11); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01, INTRO-01 |
| `Route103:object_events:004` | 65,12 | [Route103_EventScript_Liv](../../baseline/source/data/maps/Route103/scripts.inc#L225) — Route103_EventScript_Liv at (65,12); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01, INTRO-01 |
| `Route103:object_events:005` | 64,12 | [Route103_EventScript_Amy](../../baseline/source/data/maps/Route103/scripts.inc#L207) — Route103_EventScript_Amy at (64,12); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01, INTRO-01 |
| `Route103:object_events:006` | 50,8 | [Route103_EventScript_Andrew](../../baseline/source/data/maps/Route103/scripts.inc#L243) — Route103_EventScript_Andrew at (50,8); The fish are ignoring my bait. /  I may have offended the chef. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · INTRO-01 |
| `Route103:object_events:007` | 58,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (58,5); shared behavior BERRY | **REVISE** · [W-C-BERRY](../common-contracts.md#w-c-berry) · INTRO-01 |
| `Route103:object_events:008` | 59,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (59,5); shared behavior BERRY | **REVISE** · [W-C-BERRY](../common-contracts.md#w-c-berry) · INTRO-01 |
| `Route103:object_events:009` | 60,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (60,5); shared behavior BERRY | **REVISE** · [W-C-BERRY](../common-contracts.md#w-c-berry) · INTRO-01 |
| `Route103:object_events:010` | 20,10 | [Route103_EventScript_Boy](../../baseline/source/data/maps/Route103/scripts.inc#L190) — Route103_EventScript_Boy at (20,10); My POKéMON is staggeringly tired… /  I should have brought a POTION… | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · INTRO-01 |
| `Route103:object_events:011` | 7,3 | [ProfBirch_EventScript_RatePokedexOrRegister](../../baseline/source/data/scripts/prof_birch.inc#L35) — ProfBirch_EventScript_RatePokedexOrRegister at (7,3); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `Route103:object_events:012` | 56,13 | [Route103_EventScript_Miguel](../../baseline/source/data/maps/Route103/scripts.inc#L247) — Route103_EventScript_Miguel at (56,13); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01, INTRO-01 |
| `Route103:object_events:013` | 50,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_FRIEND_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_103_FRIEND_BALL. | **REVISE** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) · INTRO-01 |
| `Route103:object_events:014` | 67,7 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (67,7); shared behavior FIELD | **REVISE** · [W-C-FIELD](../common-contracts.md#w-c-field) · INTRO-01 |
| `Route103:object_events:015` | 72,8 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (72,8); shared behavior FIELD | **REVISE** · [W-C-FIELD](../common-contracts.md#w-c-field) · INTRO-01 |
| `Route103:object_events:016` | 67,5 | [Route103_EventScript_Rhett](../../baseline/source/data/maps/Route103/scripts.inc#L272) — Route103_EventScript_Rhett at (67,5); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01, INTRO-01 |
| `Route103:object_events:017` | 67,9 | [Route103_EventScript_Marcos](../../baseline/source/data/maps/Route103/scripts.inc#L268) — Route103_EventScript_Marcos at (67,9); I wrote a song about this river. /  It has a very strong current. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · INTRO-01 |
| `Route103:object_events:018` | 36,6 | [Route103_EventScript_Isabelle](../../baseline/source/data/maps/Route103/scripts.inc#L281) — Route103_EventScript_Isabelle at (36,6); I found a shell for my collection. /  It objected and walked away. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · INTRO-01 |
| `Route103:object_events:019` | 36,13 | [Route103_EventScript_Pete](../../baseline/source/data/maps/Route103/scripts.inc#L277) — Route103_EventScript_Pete at (36,13); Swimming is all about timing. /  So is remembering to breathe. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · INTRO-01 |
| `Route103:object_events:020` | 64,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_PP_UP; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_103_PP_UP. | **REVISE** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) · INTRO-01 |
| `Route103:bg_events:001` | 11,9 | [Route103_EventScript_RouteSign](../../baseline/source/data/maps/Route103/scripts.inc#L198) — Route103_EventScript_RouteSign at (11,9); ROUTE 103 /  {DOWN_ARROW} OLDALE TOWN | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01, INTRO-01 |
| `Route103:warp_events:001` | 45,6 | Warp from (45,6, elevation 0) to MAP_ALTERING_CAVE warp 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `Route103:connections:001` | down | down connection to MAP_OLDALE_TOWN, offset 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `Route103:connections:002` | right | right connection to MAP_ROUTE110, offset -60. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `Route103:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route103_OnTransition](../../baseline/source/data/maps/Route103/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls Route103_OnTransition. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |
| `Route103:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [Route103_OnLoad](../../baseline/source/data/maps/Route103/scripts.inc#L11) — MAP_SCRIPT_ON_LOAD calls Route103_OnLoad. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
