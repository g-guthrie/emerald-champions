# BattleColosseum_2P

**REVISE.** Shared player-owned or external-link support. Preserve geography, trades, records and decoration data. Native trainer challenge entry is governed by the all-doubles external-entry contract, never silently accepted as a singles exception.

[Regional experience](../10-support.md) · [Map source](../../baseline/source/data/maps/BattleColosseum_2P/map.json) · [Scripts](../../baseline/source/data/maps/BattleColosseum_2P/scripts.inc)

## Current map contract

`MAP_BATTLE_COLOSSEUM_2P` · `LAYOUT_BATTLE_COLOSSEUM_2P` · `WEATHER_NONE` · `MUS_EVER_GRANDE`

Shared player-owned or external-link support. Preserve geography, trades, records and decoration data. Native trainer challenge entry is governed by the all-doubles external-entry contract, never silently accepted as a singles exception.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleColosseum_2P:object_events:001` | 9,3 | [BattleColosseum_2P_EventScript_Attendant](../../baseline/source/data/scripts/cable_club.inc#L772) — BattleColosseum_2P_EventScript_Attendant at (9,3); shared behavior LINK | **REVISE** · [W-C-LINK](../common-contracts.md#w-c-link)  · LINK-01 |
| `BattleColosseum_2P:coord_events:001` | 3,5 | [EventScript_BattleColosseum_2P_PlayerSpot0](../../baseline/source/data/scripts/cable_club.inc#L656) — Coordinate trigger at (3,5); VAR_TEMP_0 == 0 invokes EventScript_BattleColosseum_2P_PlayerSpot0. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord)  · LINK-01 |
| `BattleColosseum_2P:coord_events:002` | 10,5 | [EventScript_BattleColosseum_2P_PlayerSpot1](../../baseline/source/data/scripts/cable_club.inc#L661) — Coordinate trigger at (10,5); VAR_TEMP_0 == 0 invokes EventScript_BattleColosseum_2P_PlayerSpot1. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord)  · LINK-01 |
| `BattleColosseum_2P:warp_events:001` | 6,8 | Warp from (6,8, elevation 3) to MAP_DYNAMIC warp WARP_ID_DYNAMIC. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp)  · LINK-01 |
| `BattleColosseum_2P:warp_events:002` | 7,8 | Warp from (7,8, elevation 3) to MAP_DYNAMIC warp WARP_ID_DYNAMIC. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp)  · LINK-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.

Shared entry/format reconciliation: **LINK-01**. Preserve imported teams, trading and records; use its exact doubles-only native entry and recovery rules.
