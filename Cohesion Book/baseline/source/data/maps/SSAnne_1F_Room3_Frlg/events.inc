@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/SSAnne_1F_Room3_Frlg/map.json
@

	.align 2

SSAnne_1F_Room3_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_LITTLE_GIRL_FRLG, 3, 4, 3, MOVEMENT_TYPE_WALK_SEQUENCE_RIGHT_DOWN_LEFT_UP, 2, 3, TRAINER_TYPE_NONE, 0, SSAnne_1F_Room3_EventScript_LittleGirl, 0
	object_event 2, OBJ_EVENT_GFX_WIGGLYTUFF, 4, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, SSAnne_1F_Room3_EventScript_Wigglytuff, 0
	object_event 3, OBJ_EVENT_GFX_BALDING_MAN, 0, 4, 3, MOVEMENT_TYPE_FACE_RIGHT, 1, 1, TRAINER_TYPE_NONE, 0, SSAnne_1F_Room3_EventScript_BaldingMan, 0

SSAnne_1F_Room3_Frlg_MapWarps:
	warp_def 2, 1, 0, 8, MAP_SSANNE_1F_CORRIDOR

SSAnne_1F_Room3_Frlg_MapEvents::
	map_events SSAnne_1F_Room3_Frlg_ObjectEvents, SSAnne_1F_Room3_Frlg_MapWarps, NULL, NULL

