@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/Route2_ViridianForest_SouthEntrance_Frlg/map.json
@

	.align 2

Route2_ViridianForest_SouthEntrance_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_WOMAN_2_FRLG, 10, 6, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NONE, 0, Route2_ViridianForest_SouthEntrance_EventScript_Woman1, 0
	object_event 2, OBJ_EVENT_GFX_WOMAN_1_FRLG, 4, 7, 3, MOVEMENT_TYPE_WANDER_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, Route2_ViridianForest_SouthEntrance_EventScript_Woman2, 0

Route2_ViridianForest_SouthEntrance_Frlg_MapWarps:
	warp_def 6, 10, 3, 2, MAP_ROUTE2
	warp_def 7, 10, 3, 2, MAP_ROUTE2
	warp_def 8, 10, 3, 2, MAP_ROUTE2
	warp_def 7, 1, 3, 0, MAP_VIRIDIAN_FOREST

Route2_ViridianForest_SouthEntrance_Frlg_MapEvents::
	map_events Route2_ViridianForest_SouthEntrance_Frlg_ObjectEvents, Route2_ViridianForest_SouthEntrance_Frlg_MapWarps, NULL, NULL

