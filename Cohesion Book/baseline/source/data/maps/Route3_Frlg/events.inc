@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/Route3_Frlg/map.json
@

	.align 2

Route3_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_YOUNGSTER_FRLG, 70, 13, 3, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, Route3_EventScript_Youngster, 0
	object_event 2, OBJ_EVENT_GFX_LASS_FRLG, 40, 11, 3, MOVEMENT_TYPE_WANDER_UP_AND_DOWN, 1, 1, TRAINER_TYPE_NORMAL, 3, Route3_EventScript_Robin, 0
	object_event 3, OBJ_EVENT_GFX_BUG_CATCHER_FRLG, 32, 6, 3, MOVEMENT_TYPE_FACE_RIGHT, 1, 1, TRAINER_TYPE_NORMAL, 4, Route3_EventScript_James, 0
	object_event 4, OBJ_EVENT_GFX_LASS_FRLG, 30, 3, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NORMAL, 5, Route3_EventScript_Sally, 0
	object_event 5, OBJ_EVENT_GFX_BUG_CATCHER_FRLG, 25, 4, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NORMAL, 2, Route3_EventScript_Greg, 0
	object_event 6, OBJ_EVENT_GFX_YOUNGSTER_FRLG, 29, 10, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NORMAL, 5, Route3_EventScript_Calvin, 0
	object_event 7, OBJ_EVENT_GFX_LASS_FRLG, 19, 9, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NORMAL, 5, Route3_EventScript_Janice, 0
	object_event 8, OBJ_EVENT_GFX_BUG_CATCHER_FRLG, 12, 6, 3, MOVEMENT_TYPE_FACE_RIGHT, 1, 1, TRAINER_TYPE_NORMAL, 3, Route3_EventScript_Colton, 0
	object_event 9, OBJ_EVENT_GFX_YOUNGSTER_FRLG, 17, 4, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NORMAL, 2, Route3_EventScript_Ben, 0

Route3_Frlg_MapBGEvents:
	bg_sign_event 72, 11, 0, BG_EVENT_PLAYER_FACING_ANY, Route3_EventScript_RouteSign
	bg_hidden_item_event 26, 9, 3, ITEM_ORAN_BERRY, FLAG_HIDDEN_ITEM_ROUTE3_ORAN_BERRY, 1, FALSE

Route3_Frlg_MapEvents::
	map_events Route3_Frlg_ObjectEvents, NULL, NULL, Route3_Frlg_MapBGEvents

