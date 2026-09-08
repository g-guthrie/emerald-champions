@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/VerdanturfMeadow/map.json
@

	.align 2

VerdanturfMeadow_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_MEGA_STONE, 7, 10, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, ITEM_FLOETTITE, Common_EventScript_FindItem, FLAG_EC_ITEM_VERDANTURF_FLOETTITE
	object_event 2, OBJ_EVENT_GFX_GIRL_2, 12, 9, 3, MOVEMENT_TYPE_WANDER_DOWN_AND_UP, 0, 3, TRAINER_TYPE_NONE, 0, VerdanturfMeadow_EventScript_Warden, 0
	object_event 3, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 2, 10, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

VerdanturfMeadow_MapBGEvents:
	bg_hidden_item_event 4, 15, 3, ITEM_PINK_NECTAR, FLAG_EC_HIDDEN_ITEM_VERDANTURF_PINK_NECTAR, 1, FALSE
	bg_hidden_item_event 10, 15, 3, ITEM_PURPLE_NECTAR, FLAG_EC_HIDDEN_ITEM_VERDANTURF_PURPLE_NECTAR, 1, FALSE

VerdanturfMeadow_MapEvents::
	map_events VerdanturfMeadow_ObjectEvents, NULL, NULL, VerdanturfMeadow_MapBGEvents

