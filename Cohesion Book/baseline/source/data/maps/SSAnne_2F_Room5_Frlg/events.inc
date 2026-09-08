@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/SSAnne_2F_Room5_Frlg/map.json
@

	.align 2

SSAnne_2F_Room5_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GENTLEMAN_FRLG, 5, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, SSAnne_2F_Room5_EventScript_Gentleman, 0
	object_event 2, OBJ_EVENT_GFX_LITTLE_BOY_FRLG, 3, 5, 3, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, SSAnne_2F_Room5_EventScript_LittleBoy, 0

SSAnne_2F_Room5_Frlg_MapWarps:
	warp_def 4, 7, 3, 7, MAP_SSANNE_2F_CORRIDOR

SSAnne_2F_Room5_Frlg_MapEvents::
	map_events SSAnne_2F_Room5_Frlg_ObjectEvents, SSAnne_2F_Room5_Frlg_MapWarps, NULL, NULL

