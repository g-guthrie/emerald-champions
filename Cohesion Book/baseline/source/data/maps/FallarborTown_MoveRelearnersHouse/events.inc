@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/FallarborTown_MoveRelearnersHouse/map.json
@

	.align 2

FallarborTown_MoveRelearnersHouse_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GIRL_3, 4, 4, 3, MOVEMENT_TYPE_FACE_RIGHT, 0, 0, TRAINER_TYPE_NONE, 0, FallarborTown_MoveRelearnersHouse_EventScript_MoveRelearner, 0
	object_event 2, OBJ_EVENT_GFX_GIRL_3, 7, 4, 3, MOVEMENT_TYPE_FACE_LEFT, 0, 0, TRAINER_TYPE_NONE, 0, FallarborTown_MoveRelearnersHouse_EventScript_StatPointTrainer, 0

FallarborTown_MoveRelearnersHouse_MapWarps:
	warp_def 3, 7, 0, 4, MAP_FALLARBOR_TOWN
	warp_def 4, 7, 0, 4, MAP_FALLARBOR_TOWN

FallarborTown_MoveRelearnersHouse_MapEvents::
	map_events FallarborTown_MoveRelearnersHouse_ObjectEvents, FallarborTown_MoveRelearnersHouse_MapWarps, NULL, NULL

