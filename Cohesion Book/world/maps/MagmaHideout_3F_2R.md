# MagmaHideout_3F_2R

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MagmaHideout_3F_2R/map.json) · [Scripts](../../baseline/source/data/maps/MagmaHideout_3F_2R/scripts.inc)

## Current map contract

`MAP_MAGMA_HIDEOUT_3F_2R` · `LAYOUT_MAGMA_HIDEOUT_3F_2R` · `WEATHER_NONE` · `MUS_AQUA_MAGMA_HIDEOUT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MagmaHideout_3F_2R:object_events:001` | 16,3 | [MagmaHideout_3F_2R_EventScript_Grunt10](../../baseline/source/data/maps/MagmaHideout_3F_2R/scripts.inc#L4) — MagmaHideout_3F_2R_EventScript_Grunt10 at (16,3); I understand everything our leader /  says. But you know what? //  Doing stuff like digging up a super- /  ancient POKéMON and ripping off /  someone's METEORITE… //  I think we're going a little too far. /  What do you think? / Yeah, I think we are doing something /  wrong somehow. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_3F_2R:object_events:002` | 5,9 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_PP_MAX; root Common_EventScript_FindItem; flag FLAG_ITEM_MAGMA_HIDEOUT_3F_2R_PP_MAX. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MagmaHideout_3F_2R:warp_events:001` | 12,15 | Warp from (12,15, elevation 0) to MAP_MAGMA_HIDEOUT_3F_1R warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
