@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/SafariZone_South/map.json
@

	.align 2

SafariZone_South_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_CAMPER, 32, 34, 4, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, SafariZone_South_EventScript_ExitAttendant, 0
	object_event 2, OBJ_EVENT_GFX_BOY_1, 26, 28, 3, MOVEMENT_TYPE_WANDER_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, SafariZone_South_EventScript_Boy, 0
	object_event 3, OBJ_EVENT_GFX_MAN_4, 16, 6, 5, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, SafariZone_South_EventScript_Man, 0
	object_event 4, OBJ_EVENT_GFX_YOUNGSTER, 15, 31, 3, MOVEMENT_TYPE_WANDER_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, SafariZone_South_EventScript_Youngster, 0
	object_event 5, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 30, 5, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, ITEM_HONEY, Common_EventScript_FindItem, FLAG_EC_ITEM_SAFARI_ZONE_SOUTH_HONEY

SafariZone_South_MapWarps:
	warp_def 32, 33, 0, 0, MAP_ROUTE121_SAFARI_ZONE_ENTRANCE

SafariZone_South_MapEvents::
	map_events SafariZone_South_ObjectEvents, SafariZone_South_MapWarps, NULL, NULL

