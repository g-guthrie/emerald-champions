@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/ShoalCave_LowTideIceRoom/map.json
@

	.align 2

ShoalCave_LowTideIceRoom_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_ITEM_BALL, 12, 8, 3, MOVEMENT_TYPE_FACE_DOWN, 5, 0, TRAINER_TYPE_NONE, ITEM_RAZZ_BERRY, Common_EventScript_FindItem, FLAG_EC_GARDEN_BUNDLE_SHOALCAVE_LOWTIDEICEROOM_RAZZ_BERRY
	object_event 2, OBJ_EVENT_GFX_ITEM_BALL, 12, 21, 4, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, ITEM_ICE_STONE, Common_EventScript_FindItem, FLAG_ITEM_SHOAL_CAVE_ICE_ROOM_ICE_STONE
	object_event 3, OBJ_EVENT_GFX_ITEM_BALL, 2, 26, 3, MOVEMENT_TYPE_FACE_DOWN, 5, 0, TRAINER_TYPE_NONE, ITEM_POMEG_BERRY, Common_EventScript_FindItem, FLAG_EC_GARDEN_BUNDLE_SHOALCAVE_LOWTIDEICEROOM_POMEG_BERRY
	object_event 4, OBJ_EVENT_GFX_INCLEMENT_ARTICUNO, 8, 8, 4, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, ShoalCave_LowTideIceRoom_EventScript_Articuno, FLAG_EC_CAUGHT_ARTICUNO
	object_event 5, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 12, 10, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

ShoalCave_LowTideIceRoom_MapWarps:
	warp_def 17, 10, 3, 3, MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM

ShoalCave_LowTideIceRoom_MapEvents::
	map_events ShoalCave_LowTideIceRoom_ObjectEvents, ShoalCave_LowTideIceRoom_MapWarps, NULL, NULL

