@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/ScorchedSlab_B2F/map.json
@

	.align 2

ScorchedSlab_B2F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_PUSHABLE_BOULDER, 28, 22, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_StrengthBoulder, FLAG_TEMP_11
	object_event 2, OBJ_EVENT_GFX_PUSHABLE_BOULDER, 29, 22, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_StrengthBoulder, FLAG_TEMP_12
	object_event 3, OBJ_EVENT_GFX_PUSHABLE_BOULDER, 29, 25, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_StrengthBoulder, FLAG_TEMP_13
	object_event 4, OBJ_EVENT_GFX_PUSHABLE_BOULDER, 29, 24, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_StrengthBoulder, FLAG_TEMP_14
	object_event 5, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 23, 22, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_BEAST_BALL, Common_EventScript_FindItem, FLAG_EC_ITEM_SCORCHED_BEAST_BALL
	object_event 6, OBJ_EVENT_GFX_PUSHABLE_BOULDER, 18, 28, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_StrengthBoulder, FLAG_TEMP_16
	object_event 7, OBJ_EVENT_GFX_PUSHABLE_BOULDER, 19, 28, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_StrengthBoulder, FLAG_TEMP_15
	object_event 8, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 12, 15, 4, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

ScorchedSlab_B2F_MapWarps:
	warp_def 17, 15, 0, 1, MAP_SCORCHED_SLAB_B1F
	warp_def 18, 4, 0, 0, MAP_SCORCHED_SLAB_HEATRANS_ROOM

ScorchedSlab_B2F_MapEvents::
	map_events ScorchedSlab_B2F_ObjectEvents, ScorchedSlab_B2F_MapWarps, NULL, NULL

