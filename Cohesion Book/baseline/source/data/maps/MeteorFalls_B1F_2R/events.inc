@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/MeteorFalls_B1F_2R/map.json
@

	.align 2

MeteorFalls_B1F_2R_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 5, 3, 3, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, ITEM_BEAST_BALL, Common_EventScript_FindItem, FLAG_ITEM_METEOR_FALLS_B1F_2R_BEAST_BALL
	object_event 2, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 6, 4, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

MeteorFalls_B1F_2R_MapWarps:
	warp_def 5, 15, 3, 3, MAP_METEOR_FALLS_B1F_1R
	warp_def 5, 2, 0, 0, MAP_METEOR_FALLS_JIRACHIS_ROOM

MeteorFalls_B1F_2R_MapEvents::
	map_events MeteorFalls_B1F_2R_ObjectEvents, MeteorFalls_B1F_2R_MapWarps, NULL, NULL

