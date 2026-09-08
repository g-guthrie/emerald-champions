# SSTidalCorridor

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SSTidalCorridor/map.json) · [Scripts](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc)

## Current map contract

`MAP_SS_TIDAL_CORRIDOR` · `LAYOUT_SS_TIDAL_CORRIDOR` · `WEATHER_NONE` · `MUS_SAILING`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SSTidalCorridor:object_events:001` | 1,11 | [SSTidalCorridor_EventScript_ExitSailor](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L112) — SSTidalCorridor_EventScript_ExitSailor at (1,11); It'll be some time before we make land, /  I reckon. //  You can rest up in your cabin if you'd /  like. Your cabin's No. 2. //  The bed in there is soft and plushy. /  I can attest to how comfy it is! / We've arrived! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SSTidalCorridor:object_events:002` | 16,7 | [SSTidalCorridor_EventScript_Sailor](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L155) — SSTidalCorridor_EventScript_Sailor at (16,7); Go visit other cabins. /  TRAINERS bored of the boat trip will /  be itching to battle. / Enjoy your cruise! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SSTidalCorridor:object_events:003` | 9,2 | [SSTidalCorridor_EventScript_Briney](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L82) — SSTidalCorridor_EventScript_Briney at (9,2); MR. BRINEY: Welcome aboard, {PLAYER}{KUN}! //  They made me honorary captain of /  the S.S. TIDAL! //  You can call me CAPTAIN BRINEY now! //  You know, I retired once before, /  but when I saw this majestic ship… //  Let me just say, it stirred my sleeping /  soul as a sailor! | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `SSTidalCorridor:object_events:004` | 7,2 | [SSTidalCorridor_EventScript_Peeko](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L86) — SSTidalCorridor_EventScript_Peeko at (7,2); PEEKO: Pihyo pihyohyo… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SSTidalCorridor:object_events:005` | 9,10 | Passive/staged OBJ_EVENT_GFX_SCOTT at (9,10); visibility flag FLAG_HIDE_SS_TIDAL_CORRIDOR_SCOTT; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SSTidalCorridor:bg_events:001` | 2,1 | [SSTidalCorridor_EventScript_Porthole](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L143) — SSTidalCorridor_EventScript_Porthole at (2,1); The horizon spreads beyond /  the porthole. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:002` | 4,1 | [SSTidalCorridor_EventScript_Porthole](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L143) — SSTidalCorridor_EventScript_Porthole at (4,1); The horizon spreads beyond /  the porthole. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:003` | 6,1 | [SSTidalCorridor_EventScript_Porthole](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L143) — SSTidalCorridor_EventScript_Porthole at (6,1); The horizon spreads beyond /  the porthole. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:004` | 8,1 | [SSTidalCorridor_EventScript_Porthole](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L143) — SSTidalCorridor_EventScript_Porthole at (8,1); The horizon spreads beyond /  the porthole. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:005` | 10,1 | [SSTidalCorridor_EventScript_Porthole](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L143) — SSTidalCorridor_EventScript_Porthole at (10,1); The horizon spreads beyond /  the porthole. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:006` | 12,1 | [SSTidalCorridor_EventScript_Porthole](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L143) — SSTidalCorridor_EventScript_Porthole at (12,1); The horizon spreads beyond /  the porthole. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:007` | 14,1 | [SSTidalCorridor_EventScript_Porthole](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L143) — SSTidalCorridor_EventScript_Porthole at (14,1); The horizon spreads beyond /  the porthole. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:008` | 16,1 | [SSTidalCorridor_EventScript_Porthole](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L143) — SSTidalCorridor_EventScript_Porthole at (16,1); The horizon spreads beyond /  the porthole. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:009` | 5,9 | [SSTidalCorridor_EventScript_Cabin1Sign](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L96) — SSTidalCorridor_EventScript_Cabin1Sign at (5,9); Cabin 1 | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:010` | 8,9 | [SSTidalCorridor_EventScript_Cabin2Sign](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L100) — SSTidalCorridor_EventScript_Cabin2Sign at (8,9); Cabin 2 | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:011` | 11,9 | [SSTidalCorridor_EventScript_Cabin3Sign](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L104) — SSTidalCorridor_EventScript_Cabin3Sign at (11,9); Cabin 3 | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:bg_events:012` | 14,9 | [SSTidalCorridor_EventScript_Cabin4Sign](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L108) — SSTidalCorridor_EventScript_Cabin4Sign at (14,9); Cabin 4 | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SSTidalCorridor:warp_events:001` | 4,9 | Warp from (4,9, elevation 3) to MAP_SS_TIDAL_ROOMS warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalCorridor:warp_events:002` | 7,9 | Warp from (7,9, elevation 3) to MAP_SS_TIDAL_ROOMS warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalCorridor:warp_events:003` | 10,9 | Warp from (10,9, elevation 3) to MAP_SS_TIDAL_ROOMS warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalCorridor:warp_events:004` | 13,9 | Warp from (13,9, elevation 3) to MAP_SS_TIDAL_ROOMS warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalCorridor:warp_events:005` | 4,3 | Warp from (4,3, elevation 3) to MAP_SS_TIDAL_ROOMS warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalCorridor:warp_events:006` | 7,3 | Warp from (7,3, elevation 3) to MAP_SS_TIDAL_ROOMS warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalCorridor:warp_events:007` | 10,3 | Warp from (10,3, elevation 3) to MAP_SS_TIDAL_ROOMS warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalCorridor:warp_events:008` | 13,3 | Warp from (13,3, elevation 3) to MAP_SS_TIDAL_ROOMS warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalCorridor:warp_events:009` | 16,2 | Warp from (16,2, elevation 3) to MAP_SS_TIDAL_LOWER_DECK warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SSTidalCorridor:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [SSTidalCorridor_OnFrame](../../baseline/source/data/maps/SSTidalCorridor/scripts.inc#L5) — MAP_SCRIPT_ON_FRAME_TABLE calls SSTidalCorridor_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
