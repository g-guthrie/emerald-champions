@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/PokemonLeague_ChampionsRoom_Frlg/map.json
@

	.align 2

PokemonLeague_ChampionsRoom_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_BLUE, 6, 8, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, 0x0, 0
	object_event 2, OBJ_EVENT_GFX_PROF_OAK, 6, 15, 0, MOVEMENT_TYPE_FACE_UP, 1, 1, TRAINER_TYPE_NONE, 0, 0x0, FLAG_HIDE_OAK_IN_CHAMP_ROOM

PokemonLeague_ChampionsRoom_Frlg_MapWarps:
	warp_def 6, 19, 3, 1, MAP_POKEMON_LEAGUE_LANCES_ROOM
	warp_def 6, 2, 3, 0, MAP_POKEMON_LEAGUE_HALL_OF_FAME

PokemonLeague_ChampionsRoom_Frlg_MapEvents::
	map_events PokemonLeague_ChampionsRoom_Frlg_ObjectEvents, PokemonLeague_ChampionsRoom_Frlg_MapWarps, NULL, NULL

