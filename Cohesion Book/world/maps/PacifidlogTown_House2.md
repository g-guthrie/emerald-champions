# PacifidlogTown_House2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/PacifidlogTown_House2/map.json) · [Scripts](../../baseline/source/data/maps/PacifidlogTown_House2/scripts.inc)

## Current map contract

`MAP_PACIFIDLOG_TOWN_HOUSE2` · `LAYOUT_PACIFIDLOG_TOWN_HOUSE2` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PacifidlogTown_House2:object_events:001` | 3,5 | [PacifidlogTown_House2_EventScript_FanClubYoungerBrother](../../baseline/source/data/maps/PacifidlogTown_House2/scripts.inc#L4) — PacifidlogTown_House2_EventScript_FanClubYoungerBrother at (3,5); Er-hem! //  I am the POKéMON FAN CLUB's most /  important person, the CHAIRMAN's /  younger brother. //  I'm here enjoying my vacation with /  POKéMON, yes, indeed. / Ah! /  Your POKéMON… | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PacifidlogTown_House2:object_events:002` | 8,6 | [PacifidlogTown_House2_EventScript_HappyAzurill](../../baseline/source/data/maps/PacifidlogTown_House2/scripts.inc#L71) — PacifidlogTown_House2_EventScript_HappyAzurill at (8,6); AZURILL: Rurii. / It appears to be very friendly with the /  TRAINER. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PacifidlogTown_House2:object_events:003` | 1,7 | [PacifidlogTown_House2_EventScript_UnhappyAzurill](../../baseline/source/data/maps/PacifidlogTown_House2/scripts.inc#L82) — PacifidlogTown_House2_EventScript_UnhappyAzurill at (1,7); AZURILL: Rururi! / It doesn't appear to like the TRAINER /  very much. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PacifidlogTown_House2:warp_events:001` | 4,8 | Warp from (4,8, elevation 0) to MAP_PACIFIDLOG_TOWN warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown_House2:warp_events:002` | 5,8 | Warp from (5,8, elevation 0) to MAP_PACIFIDLOG_TOWN warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
