@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/EverGrandeCity_PokemonLeague_1F/map.json
@

	.align 2

EverGrandeCity_PokemonLeague_1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_NURSE, 3, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, EverGrandeCity_PokemonLeague_1F_EventScript_Nurse, 0
	object_event 2, OBJ_EVENT_GFX_MART_EMPLOYEE, 15, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsBattleVendor, 0
	object_event 3, OBJ_EVENT_GFX_MAN_3, 9, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard, 0
	object_event 4, OBJ_EVENT_GFX_MAN_3, 10, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard, 0
	object_event 5, OBJ_EVENT_GFX_OLD_MAN, 17, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsMoveTutor, 0

EverGrandeCity_PokemonLeague_1F_MapWarps:
	warp_def 9, 11, 3, 0, MAP_EVER_GRANDE_CITY
	warp_def 10, 11, 3, 0, MAP_EVER_GRANDE_CITY
	warp_def 9, 1, 3, 0, MAP_EVER_GRANDE_CITY_HALL5
	warp_def 10, 1, 3, 0, MAP_EVER_GRANDE_CITY_HALL5
	warp_def 1, 7, 4, 0, MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_2F

EverGrandeCity_PokemonLeague_1F_MapEvents::
	map_events EverGrandeCity_PokemonLeague_1F_ObjectEvents, EverGrandeCity_PokemonLeague_1F_MapWarps, NULL, NULL

