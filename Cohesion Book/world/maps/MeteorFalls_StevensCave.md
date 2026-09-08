# MeteorFalls_StevensCave

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/MeteorFalls_StevensCave/map.json) · [Scripts](../../baseline/source/data/maps/MeteorFalls_StevensCave/scripts.inc)

## Current map contract

`MAP_METEOR_FALLS_STEVENS_CAVE` · `LAYOUT_METEOR_FALLS_STEVENS_CAVE` · `WEATHER_NONE` · `MUS_CAVE_OF_ORIGIN`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MeteorFalls_StevensCave:object_events:001` | 19,3 | [MeteorFalls_StevensCave_EventScript_Steven](../../baseline/source/data/maps/MeteorFalls_StevensCave/scripts.inc#L4) — MeteorFalls_StevensCave_EventScript_Steven at (19,3); STEVEN: Oh, wow, {PLAYER}{KUN}. /  I'm amazed you knew where to find me. //  Do you, uh…maybe think of me as /  just a rock maniac? //  No, that can't be right. //  We battled alongside each other at /  the MOSSDEEP SPACE CENTER. //  You should have a very good idea /  about how good I am. //  Okay, {PLAYER}{KUN}, if you're going to mount /  a serious challenge, expect the worst! / You… /  I had no idea you'd become so strong… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `MeteorFalls_StevensCave:warp_events:001` | 10,29 | Warp from (10,29, elevation 3) to MAP_METEOR_FALLS_1F_1R warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
