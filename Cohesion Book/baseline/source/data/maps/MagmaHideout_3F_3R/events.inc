@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/MagmaHideout_3F_3R/map.json
@

	.align 2

MagmaHideout_3F_3R_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 9, 19, 0, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, ITEM_ESCAPE_ROPE, Common_EventScript_FindItem, FLAG_ITEM_MAGMA_HIDEOUT_3F_3R_ESCAPE_ROPE

MagmaHideout_3F_3R_MapWarps:
	warp_def 16, 1, 0, 1, MAP_MAGMA_HIDEOUT_2F_3R
	warp_def 16, 21, 0, 1, MAP_MAGMA_HIDEOUT_4F

MagmaHideout_3F_3R_MapEvents::
	map_events MagmaHideout_3F_3R_ObjectEvents, MagmaHideout_3F_3R_MapWarps, NULL, NULL

