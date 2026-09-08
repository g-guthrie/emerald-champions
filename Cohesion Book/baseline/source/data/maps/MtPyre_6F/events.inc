@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/MtPyre_6F/map.json
@

	.align 2

MtPyre_6F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_HEX_MANIAC, 6, 3, 3, MOVEMENT_TYPE_ROTATE_COUNTERCLOCKWISE, 1, 1, TRAINER_TYPE_NORMAL, 3, MtPyre_6F_EventScript_Valerie, 0
	object_event 2, OBJ_EVENT_GFX_ITEM_BALL, 6, 9, 3, MOVEMENT_TYPE_FACE_DOWN, 5, 0, TRAINER_TYPE_NONE, ITEM_BLUK_BERRY, Common_EventScript_FindItem, FLAG_EC_GARDEN_BUNDLE_MTPYRE_6F_BLUK_BERRY
	object_event 3, OBJ_EVENT_GFX_PSYCHIC_M, 10, 3, 3, MOVEMENT_TYPE_ROTATE_CLOCKWISE, 1, 1, TRAINER_TYPE_NORMAL, 3, MtPyre_6F_EventScript_Cedric, 0
	object_event 4, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 5, 10, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_LegendaryLandmark, 0

MtPyre_6F_MapWarps:
	warp_def 2, 1, 3, 0, MAP_MT_PYRE_5F
	warp_def 1, 10, 3, 2, MAP_MT_PYRE_5F

MtPyre_6F_MapBGEvents:
	bg_sign_event 8, 8, 3, BG_EVENT_PLAYER_FACING_NORTH, MtPyre_6F_EventScript_Pecharunt

MtPyre_6F_MapEvents::
	map_events MtPyre_6F_ObjectEvents, MtPyre_6F_MapWarps, NULL, MtPyre_6F_MapBGEvents

