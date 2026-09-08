# FortreeCity_Mart

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/FortreeCity_Mart/map.json) · [Scripts](../../baseline/source/data/maps/FortreeCity_Mart/scripts.inc)

## Current map contract

`MAP_FORTREE_CITY_MART` · `LAYOUT_MART` · `WEATHER_NONE` · `MUS_POKE_MART`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FortreeCity_Mart:object_events:001` | 1,3 | [FortreeCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/FortreeCity_Mart/scripts.inc#L4) — FortreeCity_Mart_EventScript_Clerk at (1,3); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_Mart:object_events:002` | 1,4 | [FortreeCity_Mart_EventScript_Clerk](../../baseline/source/data/maps/FortreeCity_Mart/scripts.inc#L4) — FortreeCity_Mart_EventScript_Clerk at (1,4); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_Mart:object_events:003` | 9,3 | [FortreeCity_Mart_EventScript_Woman](../../baseline/source/data/maps/FortreeCity_Mart/scripts.inc#L28) — FortreeCity_Mart_EventScript_Woman at (9,3); SUPER REPEL lasts a long time, /  and it gets the job done. //  It's much better than an ordinary /  REPEL. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_Mart:object_events:004` | 8,5 | [FortreeCity_Mart_EventScript_Girl](../../baseline/source/data/maps/FortreeCity_Mart/scripts.inc#L32) — FortreeCity_Mart_EventScript_Girl at (8,5); I always stock up on more items than /  I'm sure I'll need. //  You never know what might happen. /  Better to be safe than sorry! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_Mart:object_events:005` | 5,6 | [FortreeCity_Mart_EventScript_Boy](../../baseline/source/data/maps/FortreeCity_Mart/scripts.inc#L36) — FortreeCity_Mart_EventScript_Boy at (5,6); The LEVELER raises your whole party to /  the current level limit at once. //  It makes training completely optional. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `FortreeCity_Mart:object_events:006` | 4,2 | [FortreeCity_Mart_EventScript_Spenser](../../baseline/source/data/maps/FortreeCity_Mart/scripts.inc#L40) — FortreeCity_Mart_EventScript_Spenser at (4,2); I am SPENSER of the BATTLE PALACE. //  A Pokémon's instincts reveal its /  Trainer's trust. Let me see yours! / Gwahaha! Your trust held firm! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `FortreeCity_Mart:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_FORTREE_CITY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FortreeCity_Mart:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_FORTREE_CITY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
