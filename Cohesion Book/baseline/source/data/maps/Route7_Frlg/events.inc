@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/Route7_Frlg/map.json
@

	.align 2

Route7_Frlg_ObjectEvents:
	clone_event 1, OBJ_EVENT_GFX_CUTTABLE_TREE_FRLG, -8, 12, LOCALID_CELADON_BORDER_TREE, MAP_CELADON_CITY

Route7_Frlg_MapWarps:
	warp_def 7, 14, 0, 1, MAP_UNDERGROUND_PATH_WEST_ENTRANCE
	warp_def 15, 10, 3, 0, MAP_ROUTE7_EAST_ENTRANCE

Route7_Frlg_MapBGEvents:
	bg_sign_event 5, 14, 0, BG_EVENT_PLAYER_FACING_ANY, Route7_EventScript_UndergroundPathSign
	bg_hidden_item_event 16, 15, 3, ITEM_WEPEAR_BERRY, FLAG_HIDDEN_ITEM_ROUTE7_WEPEAR_BERRY, 1, FALSE

Route7_Frlg_MapEvents::
	map_events Route7_Frlg_ObjectEvents, Route7_Frlg_MapWarps, NULL, Route7_Frlg_MapBGEvents

