@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/SeafloorCavern_Room6/map.json
@

	.align 2

SeafloorCavern_Room6_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_ITEM_BALL, 2, 2, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_DUSK_BALL, Common_EventScript_FindItem, FLAG_EC_ITEM_SEAFLOOR_CAVERN_ROOM6_DUSK_BALL

SeafloorCavern_Room6_MapWarps:
	warp_def 11, 21, 3, 2, MAP_SEAFLOOR_CAVERN_ROOM2
	warp_def 4, 1, 3, 2, MAP_SEAFLOOR_CAVERN_ROOM3
	warp_def 14, 8, 1, 1, MAP_SEAFLOOR_CAVERN_ENTRANCE

SeafloorCavern_Room6_MapEvents::
	map_events SeafloorCavern_Room6_ObjectEvents, SeafloorCavern_Room6_MapWarps, NULL, NULL

