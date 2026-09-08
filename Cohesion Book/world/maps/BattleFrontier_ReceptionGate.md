# BattleFrontier_ReceptionGate

**REVISE.** Replace obsolete facility-tour and rules claims with current Circuit facts. Preserve first pass grant, Scott welcome, architectural wayfinding and the separation of historical records from active competition.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_ReceptionGate/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_RECEPTION_GATE` · `LAYOUT_BATTLE_FRONTIER_RECEPTION_GATE` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_ReceptionGate:object_events:001` | 0,11 | [BattleFrontier_ReceptionGate_EventScript_Greeter](../../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L122) — BattleFrontier_ReceptionGate_EventScript_Greeter at (0,11); The front lines of POKéMON battling! /  Welcome to the BATTLE FRONTIER! / We hope you enjoy all that the BATTLE /  FRONTIER has to offer! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_ReceptionGate:object_events:002` | 8,11 | [BattleFrontier_ReceptionGate_EventScript_FacilityGuide](../../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L130) — BattleFrontier_ReceptionGate_EventScript_FacilityGuide at (8,11); We hope you enjoy all that the BATTLE /  FRONTIER has to offer! / I'm your guide to the various facilities /  here in the BATTLE FRONTIER. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-GUIDES |
| `BattleFrontier_ReceptionGate:object_events:003` | 8,4 | [BattleFrontier_ReceptionGate_EventScript_RulesGuide](../../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L206) — BattleFrontier_ReceptionGate_EventScript_RulesGuide at (8,4); We hope you enjoy all that the BATTLE /  FRONTIER has to offer! / I'm your guide to the basic rules that /  are common to all the challenges /  offered by the facilities in the BATTLE /  FRONTIER. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-GUIDES |
| `BattleFrontier_ReceptionGate:object_events:004` | 4,5 | Passive/staged OBJ_EVENT_GFX_SCOTT at (4,5); visibility flag FLAG_HIDE_BATTLE_FRONTIER_RECEPTION_GATE_SCOTT; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_ReceptionGate:object_events:005` | 0,4 | [BattleFrontier_ReceptionGate_EventScript_FrontierPassGuide](../../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L257) — BattleFrontier_ReceptionGate_EventScript_FrontierPassGuide at (0,4); We hope you enjoy all that the BATTLE /  FRONTIER has to offer! / I'm your guide to the FRONTIER PASS. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-GUIDES |
| `BattleFrontier_ReceptionGate:warp_events:001` | 4,13 | Warp from (4,13, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_ReceptionGate:warp_events:002` | 4,1 | Warp from (4,1, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_ReceptionGate:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_ReceptionGate_OnFrame](../../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L10) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_ReceptionGate_OnFrame. | **REPAIR** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · W-FRONTIER-GUIDES |
| `BattleFrontier_ReceptionGate:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [BattleFrontier_ReceptionGate_OnTransition](../../baseline/source/data/maps/BattleFrontier_ReceptionGate/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls BattleFrontier_ReceptionGate_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
