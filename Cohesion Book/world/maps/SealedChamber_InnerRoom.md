# SealedChamber_InnerRoom

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SealedChamber_InnerRoom/map.json) · [Scripts](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc)

## Current map contract

`MAP_SEALED_CHAMBER_INNER_ROOM` · `LAYOUT_SEALED_CHAMBER_INNER_ROOM` · `WEATHER_NONE` · `MUS_SEALED_CHAMBER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SealedChamber_InnerRoom:object_events:001` | 10,12 | [SealedChamber_InnerRoom_EventScript_RegigigasEC](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L73) — SealedChamber_InnerRoom_EventScript_RegigigasEC at (10,12); It's a statue of a Pokémon. /  It exudes tremendous power… //  There's something engraved on it… / The three REGIS answer together! /  The colossal statue begins to move! | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `SealedChamber_InnerRoom:bg_events:001` | 10,4 | [SealedChamber_InnerRoom_EventScript_BrailleBackWall](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L4) — SealedChamber_InnerRoom_EventScript_BrailleBackWall at (10,4); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SealedChamber_InnerRoom:bg_events:002` | 6,8 | [SealedChamber_InnerRoom_EventScript_BrailleStoryPart1](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L34) — SealedChamber_InnerRoom_EventScript_BrailleStoryPart1 at (6,8); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SealedChamber_InnerRoom:bg_events:003` | 14,8 | [SealedChamber_InnerRoom_EventScript_BrailleStoryPart2](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L40) — SealedChamber_InnerRoom_EventScript_BrailleStoryPart2 at (14,8); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SealedChamber_InnerRoom:bg_events:004` | 4,13 | [SealedChamber_InnerRoom_EventScript_BrailleStoryPart3](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L46) — SealedChamber_InnerRoom_EventScript_BrailleStoryPart3 at (4,13); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SealedChamber_InnerRoom:bg_events:005` | 16,13 | [SealedChamber_InnerRoom_EventScript_BrailleStoryPart4](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L52) — SealedChamber_InnerRoom_EventScript_BrailleStoryPart4 at (16,13); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SealedChamber_InnerRoom:bg_events:006` | 6,18 | [SealedChamber_InnerRoom_EventScript_BrailleStoryPart5](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L58) — SealedChamber_InnerRoom_EventScript_BrailleStoryPart5 at (6,18); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SealedChamber_InnerRoom:bg_events:007` | 14,18 | [SealedChamber_InnerRoom_EventScript_BrailleStoryPart6](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L64) — SealedChamber_InnerRoom_EventScript_BrailleStoryPart6 at (14,18); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SealedChamber_InnerRoom:bg_events:008` | 9,4 | [SealedChamber_InnerRoom_EventScript_BrailleBackWall](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L4) — SealedChamber_InnerRoom_EventScript_BrailleBackWall at (9,4); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SealedChamber_InnerRoom:bg_events:009` | 11,4 | [SealedChamber_InnerRoom_EventScript_BrailleBackWall](../../baseline/source/data/maps/SealedChamber_InnerRoom/scripts.inc#L4) — SealedChamber_InnerRoom_EventScript_BrailleBackWall at (11,4); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SealedChamber_InnerRoom:warp_events:001` | 10,19 | Warp from (10,19, elevation 3) to MAP_SEALED_CHAMBER_OUTER_ROOM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
