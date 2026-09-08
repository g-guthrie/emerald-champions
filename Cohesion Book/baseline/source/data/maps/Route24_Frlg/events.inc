@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/Route24_Frlg/map.json
@

	.align 2

Route24_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_MAN, 12, 15, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NONE, 0, Route24_EventScript_Rocket, FLAG_HIDE_NUGGET_BRIDGE_ROCKET
	object_event 2, OBJ_EVENT_GFX_CAMPER_FRLG, 12, 19, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NORMAL, 2, Route24_EventScript_Ethan, 0
	object_event 3, OBJ_EVENT_GFX_LASS_FRLG, 10, 22, 3, MOVEMENT_TYPE_FACE_RIGHT, 1, 1, TRAINER_TYPE_NORMAL, 2, Route24_EventScript_Reli, 0
	object_event 4, OBJ_EVENT_GFX_YOUNGSTER_FRLG, 12, 25, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NORMAL, 2, Route24_EventScript_Timmy, 0
	object_event 5, OBJ_EVENT_GFX_LASS_FRLG, 10, 28, 3, MOVEMENT_TYPE_FACE_RIGHT, 1, 1, TRAINER_TYPE_NORMAL, 2, Route24_EventScript_Ali, 0
	object_event 6, OBJ_EVENT_GFX_BUG_CATCHER_FRLG, 12, 31, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NORMAL, 2, Route24_EventScript_Cale, 0
	object_event 7, OBJ_EVENT_GFX_CAMPER_FRLG, 5, 21, 3, MOVEMENT_TYPE_FACE_UP, 1, 1, TRAINER_TYPE_NORMAL, 5, Route24_EventScript_Shane, 0
	object_event 8, OBJ_EVENT_GFX_ITEM_BALL, 11, 4, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, Route24_EventScript_ItemTM45, FLAG_HIDE_ROUTE24_TM45

Route24_Frlg_MapCoordEvents:
	coord_event 10, 15, 3, VAR_MAP_SCENE_ROUTE24, 0, Route24_EventScript_RocketTriggerLeft
	coord_event 11, 15, 3, VAR_MAP_SCENE_ROUTE24, 0, Route24_EventScript_RocketTriggerRight

Route24_Frlg_MapBGEvents:
	bg_hidden_item_event 19, 4, 3, ITEM_PECHA_BERRY, FLAG_HIDDEN_ITEM_ROUTE24_PECHA_BERRY, 1, FALSE

Route24_Frlg_MapEvents::
	map_events Route24_Frlg_ObjectEvents, NULL, Route24_Frlg_MapCoordEvents, Route24_Frlg_MapBGEvents

