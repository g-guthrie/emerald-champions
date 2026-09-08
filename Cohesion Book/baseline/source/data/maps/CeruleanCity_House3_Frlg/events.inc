@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/CeruleanCity_House3_Frlg/map.json
@

	.align 2

CeruleanCity_House3_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_OLD_MAN_1, 2, 2, 3, MOVEMENT_TYPE_FACE_UP, 1, 1, TRAINER_TYPE_NONE, 0, CeruleanCity_House3_EventScript_Dontae, 0
	object_event 2, OBJ_EVENT_GFX_OLD_WOMAN_FRLG, 7, 5, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NONE, 0, CeruleanCity_House3_EventScript_OldWoman, 0

CeruleanCity_House3_Frlg_MapWarps:
	warp_def 3, 7, 0, 2, MAP_CERULEAN_CITY
	warp_def 4, 7, 0, 2, MAP_CERULEAN_CITY
	warp_def 5, 7, 0, 2, MAP_CERULEAN_CITY

CeruleanCity_House3_Frlg_MapEvents::
	map_events CeruleanCity_House3_Frlg_ObjectEvents, CeruleanCity_House3_Frlg_MapWarps, NULL, NULL

