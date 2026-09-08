@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/UndergroundPath_NorthEntrance_Frlg/map.json
@

	.align 2

UndergroundPath_NorthEntrance_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_LITTLE_GIRL_FRLG, 5, 6, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, UndergroundPath_NorthEntrance_EventScript_Saige, 0

UndergroundPath_NorthEntrance_Frlg_MapWarps:
	warp_def 5, 8, 3, 0, MAP_ROUTE5
	warp_def 6, 8, 3, 0, MAP_ROUTE5
	warp_def 7, 8, 3, 0, MAP_ROUTE5
	warp_def 7, 4, 3, 0, MAP_UNDERGROUND_PATH_NORTH_SOUTH_TUNNEL

UndergroundPath_NorthEntrance_Frlg_MapEvents::
	map_events UndergroundPath_NorthEntrance_Frlg_ObjectEvents, UndergroundPath_NorthEntrance_Frlg_MapWarps, NULL, NULL

