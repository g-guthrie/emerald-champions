@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/DesertUnderpass/map.json
@

	.align 2

DesertUnderpass_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_FOSSIL, 132, 10, 0, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, DesertUnderpass_EventScript_Fossil, FLAG_HIDE_DESERT_UNDERPASS_FOSSIL
	object_event 2, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 107, 3, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_MASTER_BALL, Common_EventScript_FindItem, FLAG_EC_ITEM_EMBER_MASTER_BALL
	object_event 3, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 12, 9, 0, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

DesertUnderpass_MapWarps:
	warp_def 10, 12, 0, 2, MAP_ROUTE114_FOSSIL_MANIACS_TUNNEL
	warp_def 132, 15, 0, 24, MAP_SANDSTREWN_RUINS

DesertUnderpass_MapEvents::
	map_events DesertUnderpass_ObjectEvents, DesertUnderpass_MapWarps, NULL, NULL

