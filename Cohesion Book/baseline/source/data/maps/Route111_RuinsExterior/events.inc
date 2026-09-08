@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/Route111_RuinsExterior/map.json
@

	.align 2

Route111_RuinsExterior_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_MEGA_STONE, 10, 22, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_STEELIXITE, Common_EventScript_FindItem, FLAG_EC_ITEM_RUINS_STEELIXITE
	object_event 2, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 8, 11, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

Route111_RuinsExterior_MapWarps:
	warp_def 13, 11, 0, 1, MAP_SANDSTREWN_RUINS_3F

Route111_RuinsExterior_MapEvents::
	map_events Route111_RuinsExterior_ObjectEvents, Route111_RuinsExterior_MapWarps, NULL, NULL

