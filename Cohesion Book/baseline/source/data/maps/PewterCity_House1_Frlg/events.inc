@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/PewterCity_House1_Frlg/map.json
@

	.align 2

PewterCity_House1_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_BALDING_MAN, 1, 2, 3, MOVEMENT_TYPE_FACE_UP, 1, 1, TRAINER_TYPE_NONE, 0, PewterCity_House1_EventScript_BaldingMan, 0
	object_event 2, OBJ_EVENT_GFX_LITTLE_BOY_FRLG, 5, 3, 3, MOVEMENT_TYPE_FACE_RIGHT, 1, 1, TRAINER_TYPE_NONE, 0, PewterCity_House1_EventScript_LittleBoy, 0
	object_event 3, OBJ_EVENT_GFX_NIDORAN_M, 6, 3, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NONE, 0, PewterCity_House1_EventScript_Nidoran, 0

PewterCity_House1_Frlg_MapWarps:
	warp_def 3, 7, 0, 4, MAP_PEWTER_CITY
	warp_def 4, 7, 0, 4, MAP_PEWTER_CITY
	warp_def 5, 7, 0, 4, MAP_PEWTER_CITY

PewterCity_House1_Frlg_MapEvents::
	map_events PewterCity_House1_Frlg_ObjectEvents, PewterCity_House1_Frlg_MapWarps, NULL, NULL

