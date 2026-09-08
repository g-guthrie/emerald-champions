# Route114_LanettesHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route114_LanettesHouse/map.json) · [Scripts](../../baseline/source/data/maps/Route114_LanettesHouse/scripts.inc)

## Current map contract

`MAP_ROUTE114_LANETTES_HOUSE` · `LAYOUT_ROUTE114_LANETTES_HOUSE` · `WEATHER_NONE` · `MUS_FALLARBOR`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route114_LanettesHouse:object_events:001` | 5,4 | [Route114_LanettesHouse_EventScript_Lanette](../../baseline/source/data/maps/Route114_LanettesHouse/scripts.inc#L9) — Route114_LanettesHouse_EventScript_Lanette at (5,4); LANETTE: Oh! {PLAYER}{KUN}! //  I'm sorry everything is so cluttered… /  When I get engrossed in research, /  things end up this way… //  This is embarrassing… Please keep /  this a secret in exchange for this. / May I offer advice about my POKéMON /  Storage System? //  You should organize your BOXES so you /  can tell which POKéMON are in them. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route114_LanettesHouse:bg_events:001` | 5,1 | [Route114_LanettesHouse_EventScript_Notebook](../../baseline/source/data/maps/Route114_LanettesHouse/scripts.inc#L26) — Route114_LanettesHouse_EventScript_Notebook at (5,1); It's LANETTE's research notes. /  There's information about BOXES. //  Design BOXES to hold 30 POKéMON each. //  Each TRAINER should be able to store /  420 POKéMON on the PC system. //  Keep reading? / A marking system should be added to /  make POKéMON easier to organize. //  The name and wallpaper design of each /  BOX will be made changeable to please /  the stored POKéMON. //  Keep reading? | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114_LanettesHouse:bg_events:002` | 8,1 | [Route114_LanettesHouse_EventScript_PC](../../baseline/source/data/maps/Route114_LanettesHouse/scripts.inc#L44) — Route114_LanettesHouse_EventScript_PC at (8,1); There's an e-mail from someone on /  the PC. //  “… … … … … … … //  “Your Storage System offers more /  convenience than mine. //  “It has a lot of user-friendly features /  that make it fun and useful, too. //  “It makes me proud that I played /  a part in its development. //  “Here's hoping that you'll continue /  research in Storage Systems. //  “From BILL /  … … … … … … … …” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114_LanettesHouse:bg_events:003` | 7,1 | [Route114_LanettesHouse_EventScript_PC](../../baseline/source/data/maps/Route114_LanettesHouse/scripts.inc#L44) — Route114_LanettesHouse_EventScript_PC at (7,1); There's an e-mail from someone on /  the PC. //  “… … … … … … … //  “Your Storage System offers more /  convenience than mine. //  “It has a lot of user-friendly features /  that make it fun and useful, too. //  “It makes me proud that I played /  a part in its development. //  “Here's hoping that you'll continue /  research in Storage Systems. //  “From BILL /  … … … … … … … …” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114_LanettesHouse:warp_events:001` | 5,7 | Warp from (5,7, elevation 0) to MAP_ROUTE114 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114_LanettesHouse:warp_events:002` | 6,7 | Warp from (6,7, elevation 0) to MAP_ROUTE114 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114_LanettesHouse:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route114_LanettesHouse_OnTransition](../../baseline/source/data/maps/Route114_LanettesHouse/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route114_LanettesHouse_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
