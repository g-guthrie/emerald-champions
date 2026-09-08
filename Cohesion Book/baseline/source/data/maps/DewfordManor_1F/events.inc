@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/DewfordManor_1F/map.json
@

	.align 2

DewfordManor_1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 8, 8, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, ITEM_DUSK_BALL, Common_EventScript_FindItem, FLAG_EC_ITEM_WOODS2_DUSK_BALL
	object_event 2, OBJ_EVENT_GFX_MEGA_STONE, 1, 11, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_SABLENITE, Common_EventScript_FindItem, FLAG_EC_ITEM_MANOR_SABLENITE
	object_event 3, OBJ_EVENT_GFX_MEGA_STONE, 3, 7, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_BANETTITE, Common_EventScript_FindItem, FLAG_ITEM_MT_PYRE_6F_BANETTITE
	object_event 4, OBJ_EVENT_GFX_MEGA_STONE, 3, 9, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_CHANDELURITE, Common_EventScript_FindItem, FLAG_EC_ITEM_ROUTE121_CHANDELURITE

DewfordManor_1F_MapWarps:
	warp_def 11, 11, 0, 0, MAP_DEWFORD_MEADOW
	warp_def 12, 11, 0, 0, MAP_DEWFORD_MEADOW

DewfordManor_1F_MapEvents::
	map_events DewfordManor_1F_ObjectEvents, DewfordManor_1F_MapWarps, NULL, NULL

