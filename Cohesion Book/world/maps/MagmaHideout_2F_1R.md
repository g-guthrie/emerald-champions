# MagmaHideout_2F_1R

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MagmaHideout_2F_1R/map.json) · [Scripts](../../baseline/source/data/maps/MagmaHideout_2F_1R/scripts.inc)

## Current map contract

`MAP_MAGMA_HIDEOUT_2F_1R` · `LAYOUT_MAGMA_HIDEOUT_2F_1R` · `WEATHER_NONE` · `MUS_AQUA_MAGMA_HIDEOUT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MagmaHideout_2F_1R:object_events:001` | 18,19 | [MagmaHideout_2F_1R_EventScript_Grunt4](../../baseline/source/data/maps/MagmaHideout_2F_1R/scripts.inc#L14) — MagmaHideout_2F_1R_EventScript_Grunt4 at (18,19); An intruder! SCRAFTY, it's a real fight /  for once, not another drill! / Graaah! Even the drills didn't go /  like that! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_2F_1R:object_events:002` | 12,14 | [MagmaHideout_2F_1R_EventScript_Grunt5](../../baseline/source/data/maps/MagmaHideout_2F_1R/scripts.inc#L19) — MagmaHideout_2F_1R_EventScript_Grunt5 at (12,14); Oh, oh! Nobody told me anyone would /  actually get this far! / Mutter… mutter… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_2F_1R:object_events:003` | 8,8 | [MagmaHideout_2F_1R_EventScript_Grunt14](../../baseline/source/data/maps/MagmaHideout_2F_1R/scripts.inc#L4) — MagmaHideout_2F_1R_EventScript_Grunt14 at (8,8); Hey! No hood, no business here. //  I joined MAGMA because my town floods /  every spring. Solid ground, that's all /  I ever asked for. Now beat it! / Aiyiyi… My hood and my pride, both. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_2F_1R:object_events:004` | 21,11 | [MagmaHideout_2F_1R_EventScript_Grunt3](../../baseline/source/data/maps/MagmaHideout_2F_1R/scripts.inc#L9) — MagmaHideout_2F_1R_EventScript_Grunt3 at (21,11); Hold it! You think I'll just let you /  stroll past a CLAYDOL and a GARBODOR? / Ooh wow! Okay. I concede. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MagmaHideout_2F_1R:warp_events:001` | 11,23 | Warp from (11,23, elevation 0) to MAP_MAGMA_HIDEOUT_2F_2R warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_2F_1R:warp_events:002` | 8,2 | Warp from (8,2, elevation 0) to MAP_MAGMA_HIDEOUT_1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MagmaHideout_2F_1R:warp_events:003` | 17,33 | Warp from (17,33, elevation 3) to MAP_MAGMA_HIDEOUT_3F_1R warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
