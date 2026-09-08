@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/ArtisanCave_B1F/map.json
@

	.align 2

ArtisanCave_B1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_ITEM_BALL, 32, 38, 3, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, ITEM_COMET_SHARD, Common_EventScript_FindItem, FLAG_ITEM_ARTISAN_CAVE_B1F_COMET_SHARD

ArtisanCave_B1F_MapWarps:
	warp_def 8, 48, 0, 10, MAP_BATTLE_FRONTIER_OUTSIDE_WEST
	warp_def 38, 5, 0, 1, MAP_ARTISAN_CAVE_1F

ArtisanCave_B1F_MapBGEvents:
	bg_hidden_item_event 32, 29, 0, ITEM_NUGGET, FLAG_HIDDEN_ITEM_ARTISAN_CAVE_B1F_NUGGET, 1, FALSE
	bg_hidden_item_event 27, 8, 0, ITEM_STAR_PIECE, FLAG_HIDDEN_ITEM_ARTISAN_CAVE_B1F_STAR_PIECE, 1, FALSE
	bg_hidden_item_event 7, 5, 0, ITEM_BIG_PEARL, FLAG_HIDDEN_ITEM_ARTISAN_CAVE_B1F_BIG_PEARL, 1, FALSE
	bg_hidden_item_event 19, 43, 0, ITEM_PEARL_STRING, FLAG_HIDDEN_ITEM_ARTISAN_CAVE_B1F_PEARL_STRING, 1, FALSE

ArtisanCave_B1F_MapEvents::
	map_events ArtisanCave_B1F_ObjectEvents, ArtisanCave_B1F_MapWarps, NULL, ArtisanCave_B1F_MapBGEvents

