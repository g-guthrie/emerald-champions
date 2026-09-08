# MagmaHideout_4F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MagmaHideout_4F/map.json) · [Scripts](../../baseline/source/data/maps/MagmaHideout_4F/scripts.inc)

## Current map contract

`MAP_MAGMA_HIDEOUT_4F` · `LAYOUT_MAGMA_HIDEOUT_4F` · `WEATHER_NONE` · `MUS_AQUA_MAGMA_HIDEOUT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MagmaHideout_4F:object_events:001` | 16,17 | Passive/staged OBJ_EVENT_GFX_GROUDON_FRONT at (16,17); visibility flag FLAG_HIDE_MAGMA_HIDEOUT_4F_GROUDON; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `MagmaHideout_4F:object_events:002` | 31,22 | [MagmaHideout_4F_EventScript_Grunt11](../../baseline/source/data/maps/MagmaHideout_4F/scripts.inc#L118) — MagmaHideout_4F_EventScript_Grunt11 at (31,22); MAXIE promised solid ground under every /  home in HOENN. I believe him. //  MANDIBUZZ taunts, MAMOSWINE crushes. /  You will not reach the chamber. / You moved before my TAUNT mattered… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_4F:object_events:003` | 30,13 | [MagmaHideout_4F_EventScript_Grunt12](../../baseline/source/data/maps/MagmaHideout_4F/scripts.inc#L123) — MagmaHideout_4F_EventScript_Grunt12 at (30,13); Sun, sand, and GARCHOMP's stone. MAXIE /  built this formation for one outcome. //  Soon! Very soon our ground will hold! / The formation had one answer, and you /  found the other… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_4F:object_events:004` | 26,13 | [MagmaHideout_4F_EventScript_Grunt13](../../baseline/source/data/maps/MagmaHideout_4F/scripts.inc#L128) — MagmaHideout_4F_EventScript_Grunt13 at (26,13); AMOONGUSS pulls your attacks. RHYPERIOR /  turns the ones that land into power. //  You're not getting by me easily! / You hit the slot I left open… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_4F:object_events:005` | 21,4 | [MagmaHideout_4F_EventScript_Tabitha](../../baseline/source/data/maps/MagmaHideout_4F/scripts.inc#L133) — MagmaHideout_4F_EventScript_Tabitha at (21,4); TABITHA: Every chamber behind you was /  built to make one approach predictable. //  MAXIE is already with GROUDON. I only /  need to make your last answer too slow. / Your last answer was faster than my /  model allowed… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_4F:object_events:006` | 16,21 | [MagmaHideout_4F_EventScript_Maxie](../../baseline/source/data/maps/MagmaHideout_4F/scripts.inc#L4) — MagmaHideout_4F_EventScript_Maxie at (16,21); COURTNEY and TABITHA still hold this /  chamber. Face both before MAXIE. / MAXIE: GROUDON, ancient architect of /  continents... accept the BLUE ORB. //  The SIGN network is awake. Bind its /  energy to land that will never yield. //  Show HOENN the certainty of your power! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_4F:object_events:007` | 16,17 | Passive/staged OBJ_EVENT_GFX_GROUDON_ASLEEP at (16,17); visibility flag FLAG_HIDE_MAGMA_HIDEOUT_4F_GROUDON_ASLEEP; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `MagmaHideout_4F:object_events:008` | 3,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAX_REVIVE; root Common_EventScript_FindItem; flag FLAG_ITEM_MAGMA_HIDEOUT_4F_MAX_REVIVE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MagmaHideout_4F:object_events:009` | 21,7 | [MagmaHideout_4F_EventScript_Courtney](../../baseline/source/data/maps/MagmaHideout_4F/scripts.inc#L138) — MagmaHideout_4F_EventScript_Courtney at (21,7); COURTNEY: Prediction: you will target the /  sun before noticing the split pressure. //  Let us measure how quickly you correct. / Correction arrived before failure. Noted. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_4F:object_events:010` | 46,12 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (46,12); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `MagmaHideout_4F:warp_events:001` | 46,7 | Warp from (46,7, elevation 0) to MAP_MAGMA_HIDEOUT_3F_1R warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_4F:warp_events:002` | 20,21 | Warp from (20,21, elevation 0) to MAP_MAGMA_HIDEOUT_3F_3R warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
