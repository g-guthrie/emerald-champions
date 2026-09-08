# LavaridgeTown

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/LavaridgeTown/map.json) · [Scripts](../../baseline/source/data/maps/LavaridgeTown/scripts.inc)

## Current map contract

`MAP_LAVARIDGE_TOWN` · `LAYOUT_LAVARIDGE_TOWN` · `WEATHER_SUNNY` · `MUS_OLDALE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LavaridgeTown:object_events:001` | 8,7 | [LavaridgeTown_EventScript_ExpertF](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L253) — LavaridgeTown_EventScript_ExpertF at (8,7); Oh, you like hot springs, do you? //  That's surprising for one as young /  as you. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown:object_events:002` | 5,1 | [LavaridgeTown_EventScript_ExpertM](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L233) — LavaridgeTown_EventScript_ExpertM at (5,1); We draw as much hot water as we need, /  and yet the hot springs never run dry. //  Isn't it magical? //  These hot springs appear near active /  volcanoes. Veins of water under the /  ground are heated by magma to well up /  as hot springs. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown:object_events:003` | 5,8 | [LavaridgeTown_EventScript_OldMan](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L237) — LavaridgeTown_EventScript_OldMan at (5,8); Being buried in this hot sand is… /  Sigh… //  So warm and heavenly… //  Eh? Gyaah! Ouch! //  A POKéMON nipped my backside! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown:object_events:004` | 10,13 | [LavaridgeTown_EventScript_Twin](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L241) — LavaridgeTown_EventScript_Twin at (10,13); I bathe in the hot springs every day. //  I want to become a beautiful and strong /  GYM LEADER like FLANNERY. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown:object_events:005` | 4,4 | [LavaridgeTown_EventScript_HotSpringsOldWoman1](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L245) — LavaridgeTown_EventScript_HotSpringsOldWoman1 at (4,4); If people put POKéMON in hot springs, /  it might be seriously strange. //  Why, it might be an electric bath, or /  a bubble bath, or even a lava bath… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown:object_events:006` | 5,4 | [LavaridgeTown_EventScript_HotSpringsOldWoman2](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L249) — LavaridgeTown_EventScript_HotSpringsOldWoman2 at (5,4); They're claiming that these hot springs /  are good for calming nervous tension, /  relieving aching muscles, solving /  romantic problems, and attracting /  money… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `LavaridgeTown:object_events:007` | 6,16 | Passive/staged OBJ_EVENT_GFX_VAR_3 at (6,16); visibility flag FLAG_HIDE_LAVARIDGE_TOWN_RIVAL_ON_BIKE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `LavaridgeTown:object_events:008` | 12,15 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (12,15); visibility flag FLAG_HIDE_LAVARIDGE_TOWN_RIVAL; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `LavaridgeTown:object_events:009` | 4,7 | [LavaridgeTown_EventScript_EggWoman](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L257) — LavaridgeTown_EventScript_EggWoman at (4,7); I have here an EGG. //  I'd hoped to hatch it by covering it in /  hot sand by the hot springs. /  But that doesn't seem to be enough… //  I've heard it would be best if it were /  kept together with POKéMON and /  carried about. //  You are a TRAINER, yes? /  And your POKéMON radiate vitality. //  So, what say you? /  Will you take this EGG to hatch? / Good! I hope you'll walk plenty with /  this here EGG! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `LavaridgeTown:coord_events:001` | 6,3 | [LavaridgeTown_EventScript_HotSpringsTrigger](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L224) — Coordinate trigger at (6,3); TRIGGER_RUN_IMMEDIATELY == 0 invokes LavaridgeTown_EventScript_HotSpringsTrigger. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `LavaridgeTown:bg_events:001` | 14,16 | [LavaridgeTown_EventScript_HerbShopSign](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L297) — LavaridgeTown_EventScript_HerbShopSign at (14,16); POKéMON HERB SHOP /  “Bitter taste--better cure!” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LavaridgeTown:bg_events:002` | 7,15 | [LavaridgeTown_EventScript_GymSign](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L293) — LavaridgeTown_EventScript_GymSign at (7,15); LAVARIDGE TOWN POKéMON GYM /  LEADER: FLANNERY /  “One with a fiery passion that burns!” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LavaridgeTown:bg_events:003` | 17,5 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (17,5); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LavaridgeTown:bg_events:004` | 13,8 | [LavaridgeTown_EventScript_TownSign](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L289) — LavaridgeTown_EventScript_TownSign at (13,8); LAVARIDGE TOWN //  “POKéMON CENTER HOT SPRINGS /  An excellent place for relaxing!” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LavaridgeTown:bg_events:005` | 10,6 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (10,6); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LavaridgeTown:bg_events:006` | 16,5 | [Common_EventScript_ShowPokemartSign](../../baseline/source/data/event_scripts.s#L1173) — Common_EventScript_ShowPokemartSign at (16,5); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LavaridgeTown:bg_events:007` | 11,6 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (11,6); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `LavaridgeTown:bg_events:008` | 4,5 | Hidden ITEM_ICE_HEAL at (4,5); persistent flag FLAG_HIDDEN_ITEM_LAVARIDGE_TOWN_ICE_HEAL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `LavaridgeTown:warp_events:001` | 12,15 | Warp from (12,15, elevation 0) to MAP_LAVARIDGE_TOWN_HERB_SHOP warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown:warp_events:002` | 5,15 | Warp from (5,15, elevation 0) to MAP_LAVARIDGE_TOWN_GYM_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown:warp_events:003` | 15,5 | Warp from (15,5, elevation 0) to MAP_LAVARIDGE_TOWN_MART warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown:warp_events:004` | 9,6 | Warp from (9,6, elevation 0) to MAP_LAVARIDGE_TOWN_POKEMON_CENTER_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown:warp_events:005` | 16,15 | Warp from (16,15, elevation 0) to MAP_LAVARIDGE_TOWN_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown:warp_events:006` | 9,2 | Warp from (9,2, elevation 3) to MAP_LAVARIDGE_TOWN_POKEMON_CENTER_1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown:connections:001` | right | right connection to MAP_ROUTE112, offset -40. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `LavaridgeTown:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [LavaridgeTown_OnTransition](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls LavaridgeTown_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `LavaridgeTown:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [LavaridgeTown_OnFrame](../../baseline/source/data/maps/LavaridgeTown/scripts.inc#L40) — MAP_SCRIPT_ON_FRAME_TABLE calls LavaridgeTown_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
