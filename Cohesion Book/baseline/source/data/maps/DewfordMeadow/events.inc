@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/DewfordMeadow/map.json
@

	.align 2

DewfordMeadow_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_ITEM_BALL, 27, 1, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, ITEM_SHINY_STONE, Common_EventScript_FindItem, FLAG_EC_ITEM_MEADOW_SHINY_STONE
	object_event 2, OBJ_EVENT_GFX_MEGA_STONE, 4, 13, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, ITEM_HAWLUCHANITE, Common_EventScript_FindItem, FLAG_EC_ITEM_MEADOW_HAWLUCHANITE
	object_event 3, OBJ_EVENT_GFX_MEGA_STONE, 21, 14, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_MAWILITE, Common_EventScript_FindItem, FLAG_EC_ITEM_MEADOW_MAWILITE
	object_event 4, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 10, 12, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

DewfordMeadow_MapWarps:
	warp_def 10, 7, 0, 0, MAP_DEWFORD_MANOR_1F

DewfordMeadow_MapBGEvents:
	bg_hidden_item_event 11, 18, 3, ITEM_YELLOW_NECTAR, FLAG_EC_HIDDEN_ITEM_DEWFORD_MEADOW_YELLOW_NECTAR, 1, FALSE
	bg_hidden_item_event 21, 5, 3, ITEM_RED_NECTAR, FLAG_EC_HIDDEN_ITEM_DEWFORD_MEADOW_RED_NECTAR, 1, FALSE
	bg_sign_event 8, 8, 0, BG_EVENT_PLAYER_FACING_ANY, DewfordMeadow_EventScript_ManorSign

DewfordMeadow_MapEvents::
	map_events DewfordMeadow_ObjectEvents, DewfordMeadow_MapWarps, NULL, DewfordMeadow_MapBGEvents

