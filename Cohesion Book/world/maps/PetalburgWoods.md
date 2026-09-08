# PetalburgWoods

**REVISE.** Preserve Shroomish/Devon rescue and broad forest discovery. Current Burmy/Hisuian Decidueye explanation is already useful. The live deep-forest sign must drop old Breloom/Blissey prerequisites.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/PetalburgWoods/map.json) · [Scripts](../../baseline/source/data/maps/PetalburgWoods/scripts.inc)

## Current map contract

`MAP_PETALBURG_WOODS` · `LAYOUT_PETALBURG_WOODS` · `WEATHER_SHADE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PetalburgWoods:object_events:001` | 19,10 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (19,10); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods:object_events:002` | 19,11 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (19,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods:object_events:003` | 26,17 | Passive/staged OBJ_EVENT_GFX_AQUA_MEMBER_M at (26,17); visibility flag FLAG_HIDE_PETALBURG_WOODS_AQUA_GRUNT; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `PetalburgWoods:object_events:004` | 26,20 | Passive/staged OBJ_EVENT_GFX_MAN_2 at (26,20); visibility flag FLAG_HIDE_PETALBURG_WOODS_DEVON_EMPLOYEE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `PetalburgWoods:object_events:005` | 45,15 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SCOLIPITE; root Common_EventScript_FindItem; flag FLAG_ITEM_PETALBURG_WOODS_SCOLIPITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgWoods:object_events:006` | 35,20 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HERACRONITE; root Common_EventScript_FindItem; flag FLAG_ITEM_PETALBURG_WOODS_HERACRONITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgWoods:object_events:007` | 4,8 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BUTTERFRENITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_BUTTERFRENITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgWoods:object_events:008` | 15,19 | [PetalburgWoods_EventScript_Boy1](../../baseline/source/data/maps/PetalburgWoods/scripts.inc#L242) — PetalburgWoods_EventScript_Boy1 at (15,19); Yo, there! /  Your POKéMON doing okay? //  If your POKéMON are weak and you want /  to avoid battles, you should stay out /  of tall grass. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgWoods:object_events:009` | 7,32 | [PetalburgWoods_EventScript_Lyle](../../baseline/source/data/maps/PetalburgWoods/scripts.inc#L274) — PetalburgWoods_EventScript_Lyle at (7,32); I caught a whole bunch of POKéMON! //  Go, go, go! /  My BUG POKéMON team! / I have all these POKéMON, /  but I couldn't win… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `PetalburgWoods:object_events:010` | 4,14 | [PetalburgWoods_EventScript_James](../../baseline/source/data/maps/PetalburgWoods/scripts.inc#L279) — PetalburgWoods_EventScript_James at (4,14); If you take BUG POKéMON to school, /  you get to be instantly popular! / I can't be popular if I lose. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `PetalburgWoods:object_events:011` | 30,34 | [PetalburgWoods_EventScript_Boy2](../../baseline/source/data/maps/PetalburgWoods/scripts.inc#L246) — PetalburgWoods_EventScript_Boy2 at (30,34); BURMY hide among these leaves. Females /  become WORMADAM at Lv. 20; males /  become MOTHIM at the same level. //  After a battle, BURMY makes a cloak /  from its surroundings: leaves in grass, /  sand in caves, or trash in buildings. //  Raise DARTRIX to Lv. 34 or higher here /  to get Hisuian DECIDUEYE. Elsewhere, /  it grows into the usual form. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PetalburgWoods:object_events:012` | 4,26 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_LOPUNNITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_115_LOPUNNITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgWoods:object_events:013` | 33,6 | [PetalburgWoods_EventScript_Girl](../../baseline/source/data/maps/PetalburgWoods/scripts.inc#L250) — PetalburgWoods_EventScript_Girl at (33,6); Oh, neat! /  That's the BADGE from RUSTBORO GYM! //  You must be a TRAINER. /  You should try using this item. / It's a TART APPLE. /  Use it to evolve the right APPLIN. //  Evolution items reward exploring even /  when battle gear is easy to obtain. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `PetalburgWoods:coord_events:001` | 26,23 | [PetalburgWoods_EventScript_DevonResearcherLeft](../../baseline/source/data/maps/PetalburgWoods/scripts.inc#L4) — Coordinate trigger at (26,23); VAR_PETALBURG_WOODS_STATE == 0 invokes PetalburgWoods_EventScript_DevonResearcherLeft. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgWoods:coord_events:002` | 27,23 | [PetalburgWoods_EventScript_DevonResearcherRight](../../baseline/source/data/maps/PetalburgWoods/scripts.inc#L40) — Coordinate trigger at (27,23); VAR_PETALBURG_WOODS_STATE == 0 invokes PetalburgWoods_EventScript_DevonResearcherRight. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `PetalburgWoods:bg_events:001` | 14,32 | [PetalburgWoods_EventScript_Sign1](../../baseline/source/data/maps/PetalburgWoods/scripts.inc#L266) — PetalburgWoods_EventScript_Sign1 at (14,32); TRAINER TIPS //  Use the LEVELER to bring a new partner /  to your current training limit. //  Then prepare its moves, Ability, Nature, /  and held item before the next battle. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PetalburgWoods:bg_events:002` | 39,35 | Hidden ITEM_POTION at (39,35); persistent flag FLAG_HIDDEN_ITEM_PETALBURG_WOODS_POTION. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `PetalburgWoods:bg_events:003` | 26,6 | Hidden ITEM_TINY_MUSHROOM at (26,6); persistent flag FLAG_HIDDEN_ITEM_PETALBURG_WOODS_TINY_MUSHROOM_1. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `PetalburgWoods:bg_events:004` | 40,29 | Hidden ITEM_TINY_MUSHROOM at (40,29); persistent flag FLAG_HIDDEN_ITEM_PETALBURG_WOODS_TINY_MUSHROOM_2. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `PetalburgWoods:bg_events:005` | 4,19 | Hidden ITEM_POKE_BALL at (4,19); persistent flag FLAG_HIDDEN_ITEM_PETALBURG_WOODS_POKE_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `PetalburgWoods:bg_events:006` | 11,8 | [PetalburgWoods_EventScript_Sign2](../../baseline/source/data/maps/PetalburgWoods/scripts.inc#L270) — PetalburgWoods_EventScript_Sign2 at (11,8); TRAINER TIPS //  CUT reveals the eastern paths into the /  old-growth forest. //  Rangers report two CHAMPION'S SIGNS: /  one seeks BRELOOM, one seeks BLISSEY. | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-SIGN-LOCAL |
| `PetalburgWoods:warp_events:001` | 14,5 | Warp from (14,5, elevation 0) to MAP_ROUTE104 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods:warp_events:002` | 15,5 | Warp from (15,5, elevation 0) to MAP_ROUTE104 warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods:warp_events:003` | 16,38 | Warp from (16,38, elevation 0) to MAP_ROUTE104 warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods:warp_events:004` | 17,38 | Warp from (17,38, elevation 0) to MAP_ROUTE104 warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods:warp_events:005` | 36,38 | Warp from (36,38, elevation 0) to MAP_ROUTE104 warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods:warp_events:006` | 37,38 | Warp from (37,38, elevation 0) to MAP_ROUTE104 warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods:warp_events:007` | 42,10 | Warp from (42,10, elevation 0) to MAP_PETALBURG_WOODS_2 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods:warp_events:008` | 43,10 | Warp from (43,10, elevation 0) to MAP_PETALBURG_WOODS_2 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
