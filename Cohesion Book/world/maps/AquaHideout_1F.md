# AquaHideout_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AquaHideout_1F/map.json) · [Scripts](../../baseline/source/data/maps/AquaHideout_1F/scripts.inc)

## Current map contract

`MAP_AQUA_HIDEOUT_1F` · `LAYOUT_AQUA_HIDEOUT_1F` · `WEATHER_NONE` · `MUS_AQUA_MAGMA_HIDEOUT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AquaHideout_1F:object_events:001` | 13,11 | [AquaHideout_1F_EventScript_HideoutEntranceGrunt1](../../baseline/source/data/maps/AquaHideout_1F/scripts.inc#L5) — AquaHideout_1F_EventScript_HideoutEntranceGrunt1 at (13,11); What? What? What do you want with  /  TEAM AQUA? //  Our BOSS isn't here! He's gone off to /  snatch something important! //  … … /  Where did he go? //  Wahaha! Do you really think I'd tell /  you something that crucial? / What? What? /  Are you a TEAM MAGMA grunt? //  I hear that TEAM MAGMA is trying to /  awaken an awesome POKéMON at their /  HIDEOUT. //  But where might their HIDEOUT be? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `AquaHideout_1F:object_events:002` | 14,11 | [AquaHideout_1F_EventScript_HideoutEntranceGrunt2](../../baseline/source/data/maps/AquaHideout_1F/scripts.inc#L24) — AquaHideout_1F_EventScript_HideoutEntranceGrunt2 at (14,11); What? What? What do you want with  /  TEAM AQUA? //  Our BOSS isn't here! He's on his way to /  MT. PYRE on ROUTE 122! //  … … /  Why did he go? //  Wahaha! Do you really think I'd tell /  you something that crucial? / What? What? /  Are you a TEAM MAGMA grunt? //  I hear that TEAM MAGMA is after /  an awesome POKéMON at MT. CHIMNEY. //  But what is that POKéMON like? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `AquaHideout_1F:object_events:003` | 20,4 | [AquaHideout_1F_EventScript_Grunt1](../../baseline/source/data/maps/AquaHideout_1F/scripts.inc#L43) — AquaHideout_1F_EventScript_Grunt1 at (20,4); Ayiyiyi! Suspicious character! //  Right, right, procedure. TENTACRUEL, /  DHELMISE, hold the gate! / Grrrr… I lost it! I lost the gate! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AquaHideout_1F:warp_events:001` | 13,27 | Warp from (13,27, elevation 1) to MAP_LILYCOVE_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_1F:warp_events:002` | 14,27 | Warp from (14,27, elevation 1) to MAP_LILYCOVE_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AquaHideout_1F:warp_events:003` | 22,1 | Warp from (22,1, elevation 3) to MAP_AQUA_HIDEOUT_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
