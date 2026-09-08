@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/SixIsland_PokemonCenter_1F_Frlg/map.json
@

	.align 2

SixIsland_PokemonCenter_1F_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_NURSE_FRLG, 7, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, SixIsland_PokemonCenter_1F_EventScript_Nurse, 0
	object_event 2, OBJ_EVENT_GFX_BLUE, 5, 5, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NONE, 0, 0x0, FLAG_HIDE_SIX_ISLAND_POKECENTER_RIVAL
	object_event 3, OBJ_EVENT_GFX_HIKER_FRLG, 13, 3, 3, MOVEMENT_TYPE_FACE_UP, 1, 1, TRAINER_TYPE_NONE, 0, SixIsland_PokemonCenter_1F_EventScript_Hiker, 0
	object_event 4, OBJ_EVENT_GFX_OLD_MAN_2, 12, 5, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, SixIsland_PokemonCenter_1F_EventScript_OldMan, 0

SixIsland_PokemonCenter_1F_Frlg_MapWarps:
	warp_def 7, 8, 3, 1, MAP_SIX_ISLAND
	warp_def 1, 6, 4, 0, MAP_SIX_ISLAND_POKEMON_CENTER_2F

SixIsland_PokemonCenter_1F_Frlg_MapEvents::
	map_events SixIsland_PokemonCenter_1F_Frlg_ObjectEvents, SixIsland_PokemonCenter_1F_Frlg_MapWarps, NULL, NULL

