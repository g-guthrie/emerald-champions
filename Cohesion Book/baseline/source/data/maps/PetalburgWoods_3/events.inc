@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/PetalburgWoods_3/map.json
@

	.align 2

PetalburgWoods_3_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 22, 26, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_SWEET_APPLE, Common_EventScript_FindItem, FLAG_EC_ITEM_WOODS3_SWEET_APPLE
	object_event 2, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 22, 16, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_TART_APPLE, Common_EventScript_FindItem, FLAG_EC_ITEM_WOODS3_TART_APPLE
	object_event 3, OBJ_EVENT_GFX_CUTTABLE_TREE, 32, 22, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, 0, 0, EventScript_CutTree, FLAG_TEMP_11
	object_event 4, OBJ_EVENT_GFX_CUTTABLE_TREE, 32, 23, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, 0, 0, EventScript_CutTree, FLAG_TEMP_12
	object_event 5, OBJ_EVENT_GFX_CUTTABLE_TREE, 26, 17, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, 0, 0, EventScript_CutTree, FLAG_TEMP_13
	object_event 6, OBJ_EVENT_GFX_CUTTABLE_TREE, 26, 16, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, 0, 0, EventScript_CutTree, FLAG_TEMP_14
	object_event 7, OBJ_EVENT_GFX_MEGA_STONE, 38, 9, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_BEEDRILLITE, Common_EventScript_FindItem, FLAG_EC_ITEM_WOODS3_BEEDRILLITE
	object_event 8, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 4, 23, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

PetalburgWoods_3_MapWarps:
	warp_def 8, 7, 0, 2, MAP_PETALBURG_WOODS_2
	warp_def 9, 7, 0, 3, MAP_PETALBURG_WOODS_2
	warp_def 7, 21, 0, 5, MAP_PETALBURG_WOODS_3
	warp_def 22, 7, 0, 6, MAP_PETALBURG_WOODS_3
	warp_def 7, 39, 0, 3, MAP_PETALBURG_WOODS_3
	warp_def 10, 39, 0, 9, MAP_PETALBURG_WOODS_3
	warp_def 36, 35, 0, 2, MAP_PETALBURG_WOODS_3
	warp_def 39, 35, 0, 8, MAP_PETALBURG_WOODS_3
	warp_def 41, 6, 0, 2, MAP_PETALBURG_WOODS_3
	warp_def 22, 24, 0, 3, MAP_PETALBURG_WOODS_3

PetalburgWoods_3_MapEvents::
	map_events PetalburgWoods_3_ObjectEvents, PetalburgWoods_3_MapWarps, NULL, NULL

