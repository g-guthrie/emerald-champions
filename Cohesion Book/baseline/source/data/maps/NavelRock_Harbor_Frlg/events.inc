@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/NavelRock_Harbor_Frlg/map.json
@

	.align 2

NavelRock_Harbor_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_SEAGALLOP, 8, 9, 0, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, 0x0, 0
	object_event 2, OBJ_EVENT_GFX_SAILOR_FRLG, 8, 6, 3, MOVEMENT_TYPE_FACE_UP, 1, 1, TRAINER_TYPE_NONE, 0, NavelRock_Harbor_Frlg_EventScript_Sailor, 0

NavelRock_Harbor_Frlg_MapWarps:
	warp_def 8, 2, 3, 1, MAP_NAVEL_ROCK_EXTERIOR_FRLG

NavelRock_Harbor_Frlg_MapEvents::
	map_events NavelRock_Harbor_Frlg_ObjectEvents, NavelRock_Harbor_Frlg_MapWarps, NULL, NULL

