@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/ScorchedSlab_HeatransRoom/map.json
@

	.align 2

ScorchedSlab_HeatransRoom_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_INCLEMENT_HEATRAN, 10, 12, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, ScorchedSlab_HeatransRoom_EventScript_Heatran, FLAG_EC_CAUGHT_HEATRAN
	object_event 2, OBJ_EVENT_GFX_MEGA_STONE, 8, 16, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_HEATRANITE, Common_EventScript_FindItem, FLAG_EC_MEGA_REWARD_HEATRANITE

ScorchedSlab_HeatransRoom_MapWarps:
	warp_def 10, 18, 0, 1, MAP_SCORCHED_SLAB_B2F

ScorchedSlab_HeatransRoom_MapCoordEvents:
	coord_event 10, 14, 0, VAR_TEMP_0, 0, ScorchedSlab_HeatransRoom_EventScript_MagmaStone

ScorchedSlab_HeatransRoom_MapEvents::
	map_events ScorchedSlab_HeatransRoom_ObjectEvents, ScorchedSlab_HeatransRoom_MapWarps, ScorchedSlab_HeatransRoom_MapCoordEvents, NULL

