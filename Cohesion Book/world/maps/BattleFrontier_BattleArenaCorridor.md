# BattleFrontier_BattleArenaCorridor

**INERT/EXCLUDED.** Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattleArenaCorridor/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattleArenaCorridor/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_ARENA_CORRIDOR` · `LAYOUT_BATTLE_FRONTIER_BATTLE_ARENA_CORRIDOR` · `WEATHER_NONE` · `MUS_B_ARENA`

Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattleArenaCorridor:object_events:001` | 9,12 | Passive/staged OBJ_EVENT_GFX_BLACK_BELT at (9,12); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleArenaCorridor:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattleArenaCorridor_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattleArenaCorridor/scripts.inc#L5) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattleArenaCorridor_OnFrame. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
