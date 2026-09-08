@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/CaveOfOrigin_DianciesRoom/map.json
@

	.align 2

CaveOfOrigin_DianciesRoom_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_INCLEMENT_DIANCIE, 9, 9, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, CaveOfOrigin_DianciesRoom_EventScript_Diancie, FLAG_EC_CAUGHT_DIANCIE
	object_event 2, OBJ_EVENT_GFX_MEGA_STONE, 7, 9, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_DIANCITE, Common_EventScript_FindItem, FLAG_EC_MEGA_REWARD_DIANCITE

CaveOfOrigin_DianciesRoom_MapWarps:
	warp_def 9, 6, 0, 1, MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP3

CaveOfOrigin_DianciesRoom_MapEvents::
	map_events CaveOfOrigin_DianciesRoom_ObjectEvents, CaveOfOrigin_DianciesRoom_MapWarps, NULL, NULL

