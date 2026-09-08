# AshenWoods

**REVISE.** Keep ash/shade weather pockets, caretaker story, deliberate landmark, thoughtful Vial chase and alternative powerful wild options. The chase carries its state through re-entry and replacement Heal Balls are available.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/AshenWoods/map.json) · [Scripts](../../baseline/source/data/maps/AshenWoods/scripts.inc)

## Current map contract

`MAP_ASHEN_WOODS` · `LAYOUT_ASHEN_WOODS` · `WEATHER_VOLCANIC_ASH` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AshenWoods:object_events:001` | 10,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HONEY; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_ASHEN_HONEY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AshenWoods:object_events:002` | 26,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DUSK_STONE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_ASHEN_DUSK_STONE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AshenWoods:object_events:003` | 26,43 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_QUICK_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_ASHEN_QUICK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AshenWoods:object_events:004` | 10,13 | [AshenWoods_EventScript_Roman](../../baseline/source/data/maps/AshenWoods/scripts.inc#L191) — AshenWoods_EventScript_Roman at (10,13); One drop of water can wake a mountain. /  Interrupt the engine if you can! / You stopped the engine before it ran. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AshenWoods:object_events:005` | 12,25 | [AshenWoods_EventScript_Alannah](../../baseline/source/data/maps/AshenWoods/scripts.inc#L183) — AshenWoods_EventScript_Alannah at (12,25); The ash feeds roots tougher than stone. /  Can your team outlast mine? / Your answer grew through the ash. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AshenWoods:object_events:006` | 12,22 | [AshenWoods_EventScript_Martin](../../baseline/source/data/maps/AshenWoods/scripts.inc#L187) — AshenWoods_EventScript_Martin at (12,22); A firebird circles whenever the sun /  breaks through. Face the whole blaze! / You extinguished the wildfire cleanly. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AshenWoods:object_events:007` | 6,33 | [AshenWoods_EventScript_Elmer](../../baseline/source/data/maps/AshenWoods/scripts.inc#L195) — AshenWoods_EventScript_Elmer at (6,33); Every insect here wins differently. /  Show me four different answers! / You changed pace with every swarm. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AshenWoods:object_events:008` | 19,39 | [AshenWoods_EventScript_Caretaker](../../baseline/source/data/maps/AshenWoods/scripts.inc#L199) — AshenWoods_EventScript_Caretaker at (19,39); GROUDON's heat burned this place. /  Rain returned it to life. //  The old wardens say RAYQUAZA is not /  a conqueror, but Hoenn's balance. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `AshenWoods:object_events:009` | 14,29 | [AshenWoods_EventScript_VialChansey](../../baseline/source/data/maps/AshenWoods/scripts.inc#L80) — AshenWoods_EventScript_VialChansey at (14,29); Blob watches the HEAL BALL. /  This is your chance to bring it home. / You need one of the nurse's HEAL BALLS. /  Return to her on ROUTE 111 for another. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `AshenWoods:object_events:010` | 27,43 | Passive/staged OBJ_EVENT_GFX_ITEM_BALL at (27,43); visibility flag FLAG_HIDE_ASHEN_WOODS_VIAL_BALL; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `AshenWoods:object_events:011` | 6,40 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_PINSIRITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_PINSIRITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AshenWoods:object_events:012` | 26,7 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (26,7); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `AshenWoods:coord_events:001` | 16,28 | Coordinate weather at (16,28); weather COORD_EVENT_WEATHER_SHADE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:002` | 17,28 | Coordinate weather at (17,28); weather COORD_EVENT_WEATHER_SHADE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:003` | 20,10 | Coordinate weather at (20,10); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:004` | 24,18 | Coordinate weather at (24,18); weather COORD_EVENT_WEATHER_VOLCANIC_ASH. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:005` | 10,20 | Coordinate weather at (10,20); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:006` | 11,20 | Coordinate weather at (11,20); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:007` | 16,26 | Coordinate weather at (16,26); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:008` | 17,26 | Coordinate weather at (17,26); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:009` | 16,28 | [AshenWoods_EventScript_VialChanseyEscape1](../../baseline/source/data/maps/AshenWoods/scripts.inc#L42) — Coordinate trigger at (16,28); VAR_CHANSEY_NURSE_STATE == 3 invokes AshenWoods_EventScript_VialChanseyEscape1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:010` | 16,29 | [AshenWoods_EventScript_VialChanseyEscape1](../../baseline/source/data/maps/AshenWoods/scripts.inc#L42) — Coordinate trigger at (16,29); VAR_CHANSEY_NURSE_STATE == 3 invokes AshenWoods_EventScript_VialChanseyEscape1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:011` | 16,30 | [AshenWoods_EventScript_VialChanseyEscape1](../../baseline/source/data/maps/AshenWoods/scripts.inc#L42) — Coordinate trigger at (16,30); VAR_CHANSEY_NURSE_STATE == 3 invokes AshenWoods_EventScript_VialChanseyEscape1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:coord_events:012` | 6,36 | [AshenWoods_EventScript_VialChanseyEscape2](../../baseline/source/data/maps/AshenWoods/scripts.inc#L61) — Coordinate trigger at (6,36); VAR_CHANSEY_NURSE_STATE == 4 invokes AshenWoods_EventScript_VialChanseyEscape2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `AshenWoods:warp_events:001` | 23,5 | Warp from (23,5, elevation 0) to MAP_EMBER_PATH warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AshenWoods:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [AshenWoods_OnResume](../../baseline/source/data/maps/AshenWoods/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls AshenWoods_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `AshenWoods:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [AshenWoods_OnTransition](../../baseline/source/data/maps/AshenWoods/scripts.inc#L11) — MAP_SCRIPT_ON_TRANSITION calls AshenWoods_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
