@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/EmberPath/map.json
@

	.align 2

EmberPath_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 12, 10, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, ITEM_MAGMARIZER, Common_EventScript_FindItem, FLAG_EC_ITEM_EMBER_MAGMARIZER
	object_event 2, OBJ_EVENT_GFX_PUSHABLE_BOULDER, 34, 4, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_StrengthBoulder, FLAG_TEMP_11
	object_event 3, OBJ_EVENT_GFX_ITEM_BALL, 36, 2, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_QUICK_BALL, Common_EventScript_FindItem, FLAG_EC_ITEM_EMBER_QUICK_BALL
	object_event 4, OBJ_EVENT_GFX_PUSHABLE_BOULDER, 13, 36, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_StrengthBoulder, FLAG_TEMP_13
	object_event 5, OBJ_EVENT_GFX_PUSHABLE_BOULDER, 14, 32, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_StrengthBoulder, FLAG_TEMP_12
	object_event 6, OBJ_EVENT_GFX_INCLEMENT_MOLTRES, 21, 14, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, EmberPath_EventScript_Moltres, FLAG_EC_CAUGHT_MOLTRES

EmberPath_MapWarps:
	warp_def 33, 38, 0, 5, MAP_JAGGED_PASS
	warp_def 3, 17, 0, 0, MAP_ASHEN_WOODS

EmberPath_MapBGEvents:
	bg_hidden_item_event 9, 38, 3, ITEM_MAGMARIZER, FLAG_EC_HIDDEN_ITEM_EMBER_PATH_MAGMARIZER, 1, FALSE

EmberPath_MapEvents::
	map_events EmberPath_ObjectEvents, EmberPath_MapWarps, NULL, EmberPath_MapBGEvents

