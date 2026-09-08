# PacifidlogTown_House3

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/PacifidlogTown_House3/map.json) · [Scripts](../../baseline/source/data/maps/PacifidlogTown_House3/scripts.inc)

## Current map contract

`MAP_PACIFIDLOG_TOWN_HOUSE3` · `LAYOUT_PACIFIDLOG_TOWN_HOUSE1` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PacifidlogTown_House3:object_events:001` | 3,5 | [PacifidlogTown_House3_EventScript_Girl](../../baseline/source/data/maps/PacifidlogTown_House3/scripts.inc#L29) — PacifidlogTown_House3_EventScript_Girl at (3,5); Is that a POKéDEX? //  Did you get to meet a lot of different /  POKéMON? //  I wish I was like you. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PacifidlogTown_House3:object_events:002` | 4,2 | [PacifidlogTown_House3_EventScript_Trader](../../baseline/source/data/maps/PacifidlogTown_House3/scripts.inc#L4) — PacifidlogTown_House3_EventScript_Trader at (4,2); Check out this {STR_VAR_2}! //  It's the {STR_VAR_2} that I caught /  yesterday to celebrate my birthday! //  Oh, I can see that you want it! /  After all, it's priceless! //  I'll tell you what. I might be willing /  to trade it for a {STR_VAR_1}. / Oh, so this is a {STR_VAR_1}? //  It's sort of like a {STR_VAR_2}, /  and yet it's subtly different. //  My {STR_VAR_2} is already trained /  for doubles. Ride it well! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PacifidlogTown_House3:warp_events:001` | 4,8 | Warp from (4,8, elevation 0) to MAP_PACIFIDLOG_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown_House3:warp_events:002` | 5,8 | Warp from (5,8, elevation 0) to MAP_PACIFIDLOG_TOWN warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
