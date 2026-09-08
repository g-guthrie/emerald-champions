@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/ScorchedSlab/map.json
@

	.align 2

ScorchedSlab_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 7, 5, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, ITEM_MASTER_BALL, Common_EventScript_FindItem, FLAG_ITEM_SCORCHED_SLAB_MASTER_BALL

ScorchedSlab_MapWarps:
	warp_def 7, 16, 1, 1, MAP_ROUTE120
	warp_def 9, 3, 0, 0, MAP_SCORCHED_SLAB_B1F

ScorchedSlab_MapEvents::
	map_events ScorchedSlab_ObjectEvents, ScorchedSlab_MapWarps, NULL, NULL

