# SecretBase_BlueCave1

**REVISE.** Shared player-owned or external-link support. Preserve geography, trades, records and decoration data. Native trainer challenge entry is governed by the all-doubles external-entry contract, never silently accepted as a singles exception.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/SecretBase_BlueCave1/map.json)

## Current map contract

`MAP_SECRET_BASE_BLUE_CAVE1` · `LAYOUT_SECRET_BASE_BLUE_CAVE1` · `WEATHER_NONE` · `MUS_FORTREE`

Shared player-owned or external-link support. Preserve geography, trades, records and decoration data. Native trainer challenge entry is governed by the all-doubles external-entry contract, never silently accepted as a singles exception.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SecretBase_BlueCave1:object_events:001` | 4,2 | [SecretBase_EventScript_RecordMixTrainer](../../baseline/source/data/scripts/secret_base.inc#L284) — SecretBase_EventScript_RecordMixTrainer at (4,2); shared behavior LINK | **REVISE** · [W-C-LINK](../common-contracts.md#w-c-link)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:002` | 0,0 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (0,0); visibility flag FLAG_DECORATION_1; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:003` | 0,1 | Passive/staged OBJ_EVENT_GFX_VAR_1 at (0,1); visibility flag FLAG_DECORATION_2; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:004` | 0,2 | Passive/staged OBJ_EVENT_GFX_VAR_2 at (0,2); visibility flag FLAG_DECORATION_3; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:005` | 0,3 | Passive/staged OBJ_EVENT_GFX_VAR_3 at (0,3); visibility flag FLAG_DECORATION_4; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:006` | 0,4 | Passive/staged OBJ_EVENT_GFX_VAR_4 at (0,4); visibility flag FLAG_DECORATION_5; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:007` | 0,5 | Passive/staged OBJ_EVENT_GFX_VAR_5 at (0,5); visibility flag FLAG_DECORATION_6; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:008` | 0,6 | Passive/staged OBJ_EVENT_GFX_VAR_6 at (0,6); visibility flag FLAG_DECORATION_7; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:009` | 1,0 | Passive/staged OBJ_EVENT_GFX_VAR_7 at (1,0); visibility flag FLAG_DECORATION_8; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:010` | 1,1 | Passive/staged OBJ_EVENT_GFX_VAR_8 at (1,1); visibility flag FLAG_DECORATION_9; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:011` | 1,2 | Passive/staged OBJ_EVENT_GFX_VAR_9 at (1,2); visibility flag FLAG_DECORATION_10; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:012` | 1,3 | Passive/staged OBJ_EVENT_GFX_VAR_A at (1,3); visibility flag FLAG_DECORATION_11; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:013` | 1,4 | Passive/staged OBJ_EVENT_GFX_VAR_B at (1,4); visibility flag FLAG_DECORATION_12; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:014` | 1,5 | Passive/staged OBJ_EVENT_GFX_VAR_C at (1,5); visibility flag FLAG_DECORATION_13; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:object_events:015` | 1,6 | Passive/staged OBJ_EVENT_GFX_VAR_D at (1,6); visibility flag FLAG_DECORATION_14; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive)  · LINK-01 |
| `SecretBase_BlueCave1:warp_events:001` | 5,7 | Warp from (5,7, elevation 0) to MAP_DYNAMIC warp WARP_ID_SECRET_BASE. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp)  · LINK-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.

Shared entry/format reconciliation: **LINK-01**. Preserve imported teams, trading and records; use its exact doubles-only native entry and recovery rules.
