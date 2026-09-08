# MossdeepCity_GameCorner_B1F

**REVISE.** Preserve the home, harmless residents, minigame records and imported data. Retire the native e-Reader trainer challenge under the all-doubles policy; externally stored data is not an authored native team.

[Regional experience](../10-support.md) · [Map source](../../baseline/source/data/maps/MossdeepCity_GameCorner_B1F/map.json) · [Scripts](../../baseline/source/data/maps/MossdeepCity_GameCorner_B1F/scripts.inc)

## Current map contract

`MAP_MOSSDEEP_CITY_GAME_CORNER_B1F` · `LAYOUT_MOSSDEEP_CITY_GAME_CORNER_B1F` · `WEATHER_NONE` · `MUS_RUSTBORO`

Preserve the home, harmless residents, minigame records and imported data. Retire the native e-Reader trainer challenge under the all-doubles policy; externally stored data is not an authored native team.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MossdeepCity_GameCorner_B1F:object_events:001` | 6,5 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (6,5); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `MossdeepCity_GameCorner_B1F:warp_events:001` | 3,1 | Warp from (3,1, elevation 0) to MAP_MOSSDEEP_CITY_GAME_CORNER_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
