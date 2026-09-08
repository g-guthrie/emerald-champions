# MossdeepCity_Mart

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/MossdeepCity_Mart/map.json) · [Scripts](../../baseline/source/data/maps/MossdeepCity_Mart/scripts.inc)

## Current map contract

`MAP_MOSSDEEP_CITY_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MossdeepCity_Mart:object_events:001` | 1,3 | [MossdeepCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/MossdeepCity_Mart/scripts.inc#L4) — MossdeepCity_Mart_EventScript_Clerk at (1,3); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_Mart:object_events:002` | 1,6 | [MossdeepCity_Mart_EventScript_Woman](../../baseline/source/data/maps/MossdeepCity_Mart/scripts.inc#L27) — MossdeepCity_Mart_EventScript_Woman at (1,6); REVIVE is fantastic! //  Give it to a fainted POKéMON, /  and the POKéMON will arise. //  But be careful, REVIVE doesn't restore /  the used-up PP of moves. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_Mart:object_events:003` | 8,3 | [MossdeepCity_Mart_EventScript_Boy](../../baseline/source/data/maps/MossdeepCity_Mart/scripts.inc#L31) — MossdeepCity_Mart_EventScript_Boy at (8,3); MAX REPEL keeps all weak POKéMON away. //  Out of all the REPEL sprays, it lasts /  the longest. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_Mart:object_events:004` | 5,3 | [MossdeepCity_Mart_EventScript_Sailor](../../baseline/source/data/maps/MossdeepCity_Mart/scripts.inc#L35) — MossdeepCity_Mart_EventScript_Sailor at (5,3); NET and DIVE BALLS specialize in /  different kinds of hunts. //  A NET BALL excels against BUG- and /  WATER-type POKéMON. //  A DIVE BALL excels while surfing, /  fishing, or exploring underwater. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `MossdeepCity_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_MOSSDEEP_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MossdeepCity_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_MOSSDEEP_CITY warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
