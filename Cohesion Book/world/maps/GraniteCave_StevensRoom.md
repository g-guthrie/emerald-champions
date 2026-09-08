# GraniteCave_StevensRoom

**REVISE.** Preserve letter delivery before Brawly, explicit museum-queue directions, the Ring after Knuckle Badge, starter-stone pending storage and Pokenav handoff. Replace the still-live mandatory Devon translation claim.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/GraniteCave_StevensRoom/map.json) · [Scripts](../../baseline/source/data/maps/GraniteCave_StevensRoom/scripts.inc)

## Current map contract

`MAP_GRANITE_CAVE_STEVENS_ROOM` · `LAYOUT_GRANITE_CAVE_STEVENS_ROOM` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `GraniteCave_StevensRoom:object_events:001` | 7,8 | [GraniteCave_StevensRoom_EventScript_Steven](../../baseline/source/data/maps/GraniteCave_StevensRoom/scripts.inc#L4) — GraniteCave_StevensRoom_EventScript_Steven at (7,8); {PLAYER} put the {STR_VAR_2} /  in the PC. / Your BAG and PC have no room for the /  remaining stone. I'll keep it here. //  Make some room, then speak to me /  again. Your gift will still be here. | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-SIGN-OPTIONAL |
| `GraniteCave_StevensRoom:warp_events:001` | 7,3 | Warp from (7,3, elevation 3) to MAP_GRANITE_CAVE_1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
