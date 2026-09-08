@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/MtPyre_Exterior/map.json
@

	.align 2

MtPyre_Exterior_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_ITEM_BALL, 27, 15, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_MAX_POTION, Common_EventScript_FindItem, FLAG_ITEM_MT_PYRE_EXTERIOR_MAX_POTION
	object_event 2, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 19, 40, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, ITEM_DUSK_BALL, Common_EventScript_FindItem, FLAG_ITEM_MT_PYRE_EXTERIOR_DUSK_BALL
	object_event 3, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 12, 45, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

MtPyre_Exterior_MapWarps:
	warp_def 10, 42, 3, 1, MAP_MT_PYRE_1F
	warp_def 19, 10, 3, 1, MAP_MT_PYRE_SUMMIT
	warp_def 20, 10, 3, 1, MAP_MT_PYRE_SUMMIT

MtPyre_Exterior_MapCoordEvents:
	coord_event 24, 21, 3, TRIGGER_RUN_IMMEDIATELY, 0, MtPyre_Exterior_EventScript_FogTrigger
	coord_event 25, 21, 3, TRIGGER_RUN_IMMEDIATELY, 0, MtPyre_Exterior_EventScript_FogTrigger
	coord_event 22, 27, 3, TRIGGER_RUN_IMMEDIATELY, 0, MtPyre_Exterior_EventScript_SunTrigger
	coord_event 23, 28, 3, TRIGGER_RUN_IMMEDIATELY, 0, MtPyre_Exterior_EventScript_SunTrigger
	coord_event 26, 21, 3, TRIGGER_RUN_IMMEDIATELY, 0, MtPyre_Exterior_EventScript_FogTrigger

MtPyre_Exterior_MapBGEvents:
	bg_hidden_item_event 9, 8, 0, ITEM_ULTRA_BALL, FLAG_HIDDEN_ITEM_MT_PYRE_EXTERIOR_ULTRA_BALL, 1, FALSE
	bg_hidden_item_event 16, 22, 0, ITEM_MAX_ETHER, FLAG_HIDDEN_ITEM_MT_PYRE_EXTERIOR_MAX_ETHER, 1, FALSE

MtPyre_Exterior_MapEvents::
	map_events MtPyre_Exterior_ObjectEvents, MtPyre_Exterior_MapWarps, MtPyre_Exterior_MapCoordEvents, MtPyre_Exterior_MapBGEvents

