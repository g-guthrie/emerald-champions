@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/SafariZone_North_RestHouse_Frlg/map.json
@

	.align 2

SafariZone_North_RestHouse_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GENTLEMAN_FRLG, 3, 4, 3, MOVEMENT_TYPE_WANDER_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, SafariZone_North_RestHouse_EventScript_Gentleman, 0
	object_event 2, OBJ_EVENT_GFX_SCIENTIST, 7, 5, 3, MOVEMENT_TYPE_WANDER_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, SafariZone_North_RestHouse_EventScript_Scientist, 0
	object_event 3, OBJ_EVENT_GFX_FISHER, 6, 2, 3, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, SafariZone_North_RestHouse_EventScript_Fisher, 0
	object_event 4, OBJ_EVENT_GFX_BALDING_MAN, 11, 2, 3, MOVEMENT_TYPE_FACE_UP, 1, 1, TRAINER_TYPE_NONE, 0, SafariZone_North_RestHouse_EventScript_BaldingMan, 0

SafariZone_North_RestHouse_Frlg_MapWarps:
	warp_def 3, 9, 3, 12, MAP_SAFARI_ZONE_NORTH_FRLG
	warp_def 4, 9, 3, 12, MAP_SAFARI_ZONE_NORTH_FRLG
	warp_def 5, 9, 3, 12, MAP_SAFARI_ZONE_NORTH_FRLG

SafariZone_North_RestHouse_Frlg_MapEvents::
	map_events SafariZone_North_RestHouse_Frlg_ObjectEvents, SafariZone_North_RestHouse_Frlg_MapWarps, NULL, NULL

