# SSTidalRooms

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SSTidalRooms/map.json) · [Scripts](../../baseline/source/data/maps/SSTidalRooms/scripts.inc)

## Current map contract

`MAP_SS_TIDAL_ROOMS` · `LAYOUT_SS_TIDAL_ROOMS` · `WEATHER_NONE` · `MUS_SAILING`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SSTidalRooms:object_events:001` | 4,7 | [SSTidalRooms_EventScript_Colton](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L30) — SSTidalRooms_EventScript_Colton at (4,7); I often sail to LILYCOVE CITY. //  I enjoy attending CONTESTS, /  you see. / That was an enjoyable match! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SSTidalRooms:object_events:002` | 34,11 | [SSTidalRooms_EventScript_Micah](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L35) — SSTidalRooms_EventScript_Micah at (34,11); Are your friends strong? / Your friends are, indeed, strong. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SSTidalRooms:object_events:003` | 21,5 | [SSTidalRooms_EventScript_Thomas](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L40) — SSTidalRooms_EventScript_Thomas at (21,5); Child… /  Did you knock on the door? / A loss is to be accepted without haste /  or panic. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SSTidalRooms:object_events:004` | 5,14 | [SSTidalRooms_EventScript_Jed](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L45) — SSTidalRooms_EventScript_Jed at (5,14); JED: I feel a little shy about this, but… /  We'll show you our lovey-dovey power! / JED: Sigh… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SSTidalRooms:object_events:005` | 4,14 | [SSTidalRooms_EventScript_Lea](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L50) — SSTidalRooms_EventScript_Lea at (4,14); LEA: I feel a little silly, but… /  We'll show you our lovey-dovey power! / LEA: Oh, boo! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SSTidalRooms:object_events:006` | 22,11 | [SSTidalRooms_EventScript_Garret](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L55) — SSTidalRooms_EventScript_Garret at (22,11); Ah, you've come just in time. //  I'm bored, you see. /  You may entertain me. / …That will do. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SSTidalRooms:object_events:007` | 15,6 | [SSTidalRooms_EventScript_Naomi](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L60) — SSTidalRooms_EventScript_Naomi at (15,6); Oh, you're such an adorable TRAINER. /  Would you like to have tea? /  Or would you rather battle? / I see. /  You're the active sort. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `SSTidalRooms:object_events:008` | 28,5 | [SSTidalRooms_EventScript_ReaperClothGiver](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L4) — SSTidalRooms_EventScript_ReaperClothGiver at (28,5); Uh… Hi! I'm not acting suspicious! //  I didn't SNATCH this REAPER CLOTH /  from anyone. It's clean! Take it! / A REAPER CLOTH evolves DUSCLOPS. /  Use it from the Bag when ready. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SSTidalRooms:bg_events:001` | 15,11 | [SSTidalRooms_EventScript_Bed](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L21) — SSTidalRooms_EventScript_Bed at (15,11); There's a bed… /  Let's take a rest. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalRooms:bg_events:002` | 15,12 | [SSTidalRooms_EventScript_Bed](../../baseline/source/data/maps/SSTidalRooms/scripts.inc#L21) — SSTidalRooms_EventScript_Bed at (15,12); There's a bed… /  Let's take a rest. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalRooms:warp_events:001` | 4,16 | Warp from (4,16, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:002` | 5,16 | Warp from (5,16, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:003` | 13,16 | Warp from (13,16, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:004` | 14,16 | Warp from (14,16, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:005` | 22,16 | Warp from (22,16, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:006` | 23,16 | Warp from (23,16, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:007` | 31,16 | Warp from (31,16, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:008` | 32,16 | Warp from (32,16, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:009` | 4,1 | Warp from (4,1, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:010` | 13,1 | Warp from (13,1, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:011` | 22,1 | Warp from (22,1, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalRooms:warp_events:012` | 31,1 | Warp from (31,1, elevation 0) to MAP_SS_TIDAL_CORRIDOR warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
