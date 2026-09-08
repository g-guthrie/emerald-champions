@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/CaveOfOrigin_1F/map.json
@

	.align 2

CaveOfOrigin_1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_INCLEMENT_CARBINK, 5, 8, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, CaveOfOrigin_1F_EventScript_Carbink, FLAG_BADGE08_GET

CaveOfOrigin_1F_MapWarps:
	warp_def 11, 17, 3, 1, MAP_CAVE_OF_ORIGIN_ENTRANCE
	warp_def 14, 5, 3, 0, MAP_CAVE_OF_ORIGIN_B1F
	warp_def 5, 8, 0, 0, MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP1

CaveOfOrigin_1F_MapEvents::
	map_events CaveOfOrigin_1F_ObjectEvents, CaveOfOrigin_1F_MapWarps, NULL, NULL

