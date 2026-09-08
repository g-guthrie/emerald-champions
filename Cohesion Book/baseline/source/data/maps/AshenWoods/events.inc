@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/AshenWoods/map.json
@

	.align 2

AshenWoods_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_ITEM_BALL, 10, 5, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_HONEY, Common_EventScript_FindItem, FLAG_EC_ITEM_ASHEN_HONEY
	object_event 2, OBJ_EVENT_GFX_ITEM_BALL, 26, 5, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, ITEM_DUSK_STONE, Common_EventScript_FindItem, FLAG_EC_ITEM_ASHEN_DUSK_STONE
	object_event 3, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 26, 43, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, ITEM_QUICK_BALL, Common_EventScript_FindItem, FLAG_EC_ITEM_ASHEN_QUICK_BALL
	object_event 4, OBJ_EVENT_GFX_HIKER, 10, 13, 3, MOVEMENT_TYPE_WALK_UP_AND_DOWN, 0, 3, TRAINER_TYPE_NORMAL, 4, AshenWoods_EventScript_Roman, 0
	object_event 5, OBJ_EVENT_GFX_PICNICKER, 12, 25, 3, MOVEMENT_TYPE_FACE_UP_LEFT_AND_RIGHT, 0, 3, TRAINER_TYPE_NORMAL, 4, AshenWoods_EventScript_Alannah, 0
	object_event 6, OBJ_EVENT_GFX_CAMPER, 12, 22, 3, MOVEMENT_TYPE_FACE_DOWN_AND_LEFT, 0, 3, TRAINER_TYPE_NORMAL, 3, AshenWoods_EventScript_Martin, 0
	object_event 7, OBJ_EVENT_GFX_MANIAC, 6, 33, 3, MOVEMENT_TYPE_WALK_RIGHT_AND_LEFT, 7, 0, TRAINER_TYPE_NORMAL, 7, AshenWoods_EventScript_Elmer, 0
	object_event 8, OBJ_EVENT_GFX_COOK, 19, 39, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 2, TRAINER_TYPE_NONE, 0, AshenWoods_EventScript_Caretaker, 0
	object_event 9, OBJ_EVENT_GFX_SPECIES(CHANSEY), 14, 29, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, 0, AshenWoods_EventScript_VialChansey, FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY
	object_event 10, OBJ_EVENT_GFX_ITEM_BALL, 27, 43, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, 0, FLAG_HIDE_ASHEN_WOODS_VIAL_BALL
	object_event 11, OBJ_EVENT_GFX_MEGA_STONE, 6, 40, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_PINSIRITE, Common_EventScript_FindItem, FLAG_EC_MEGA_REWARD_PINSIRITE
	object_event 12, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 26, 7, 4, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

AshenWoods_MapWarps:
	warp_def 23, 5, 0, 1, MAP_EMBER_PATH

AshenWoods_MapCoordEvents:
	coord_weather_event 16, 28, 0, COORD_EVENT_WEATHER_SHADE
	coord_weather_event 17, 28, 0, COORD_EVENT_WEATHER_SHADE
	coord_weather_event 20, 10, 0, COORD_EVENT_WEATHER_VOLCANIC_ASH
	coord_weather_event 24, 18, 0, COORD_EVENT_WEATHER_VOLCANIC_ASH
	coord_weather_event 10, 20, 0, COORD_EVENT_WEATHER_SUNNY
	coord_weather_event 11, 20, 0, COORD_EVENT_WEATHER_SUNNY
	coord_weather_event 16, 26, 0, COORD_EVENT_WEATHER_SUNNY
	coord_weather_event 17, 26, 0, COORD_EVENT_WEATHER_SUNNY
	coord_event 16, 28, 0, VAR_CHANSEY_NURSE_STATE, 3, AshenWoods_EventScript_VialChanseyEscape1
	coord_event 16, 29, 0, VAR_CHANSEY_NURSE_STATE, 3, AshenWoods_EventScript_VialChanseyEscape1
	coord_event 16, 30, 0, VAR_CHANSEY_NURSE_STATE, 3, AshenWoods_EventScript_VialChanseyEscape1
	coord_event 6, 36, 0, VAR_CHANSEY_NURSE_STATE, 4, AshenWoods_EventScript_VialChanseyEscape2

AshenWoods_MapEvents::
	map_events AshenWoods_ObjectEvents, AshenWoods_MapWarps, AshenWoods_MapCoordEvents, NULL

