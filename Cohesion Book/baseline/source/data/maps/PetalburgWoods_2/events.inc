@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/PetalburgWoods_2/map.json
@

	.align 2

PetalburgWoods_2_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 5, 5, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_QUICK_BALL, PetalburgWoods_2_EventScript_QuickBallPack, FLAG_EC_ITEM_PETALBURG_WOODS_2_QUICK_BALL
	object_event 2, OBJ_EVENT_GFX_ITEM_BALL, 36, 5, 3, MOVEMENT_TYPE_FACE_UP, 0, 0, TRAINER_TYPE_NONE, ITEM_SUN_STONE, Common_EventScript_FindItem, FLAG_EC_ITEM_WOODS2_SUN_STONE
	object_event 3, OBJ_EVENT_GFX_CUTTABLE_TREE, 4, 12, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, 0, 0, EventScript_CutTree, FLAG_TEMP_11
	object_event 4, OBJ_EVENT_GFX_CUTTABLE_TREE, 25, 6, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, 0, 0, EventScript_CutTree, FLAG_TEMP_12
	object_event 5, OBJ_EVENT_GFX_CUTTABLE_TREE, 26, 6, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, 0, 0, EventScript_CutTree, FLAG_TEMP_13
	object_event 6, OBJ_EVENT_GFX_CUTTABLE_TREE, 45, 19, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, 0, 0, EventScript_CutTree, FLAG_TEMP_14
	object_event 7, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 2, 31, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

PetalburgWoods_2_MapWarps:
	warp_def 4, 34, 0, 6, MAP_PETALBURG_WOODS
	warp_def 5, 34, 0, 7, MAP_PETALBURG_WOODS
	warp_def 42, 34, 0, 0, MAP_PETALBURG_WOODS_3
	warp_def 43, 34, 0, 1, MAP_PETALBURG_WOODS_3

PetalburgWoods_2_MapEvents::
	map_events PetalburgWoods_2_ObjectEvents, PetalburgWoods_2_MapWarps, NULL, NULL

