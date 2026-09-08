@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/PacifidlogTown_PokemonCenter_1F/map.json
@

	.align 2

PacifidlogTown_PokemonCenter_1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_NURSE, 8, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, PacifidlogTown_PokemonCenter_1F_EventScript_Nurse, 0
	object_event 2, OBJ_EVENT_GFX_OLD_MAN, 10, 6, 3, MOVEMENT_TYPE_FACE_LEFT, 0, 0, TRAINER_TYPE_NONE, 0, PacifidlogTown_PokemonCenter_1F_EventScript_OldMan, 0
	object_event 3, OBJ_EVENT_GFX_GIRL_2, 3, 4, 3, MOVEMENT_TYPE_FACE_UP, 0, 0, TRAINER_TYPE_NONE, 0, PacifidlogTown_PokemonCenter_1F_EventScript_Girl, 0
	object_event 4, OBJ_EVENT_GFX_WOMAN_1, 5, 6, 3, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, PacifidlogTown_PokemonCenter_1F_EventScript_Woman, 0
	object_event 5, OBJ_EVENT_GFX_MAN_5, 14, 8, 3, MOVEMENT_TYPE_FACE_UP, 0, 0, TRAINER_TYPE_NONE, 0, PacifidlogTown_PokemonCenter_1F_EventScript_ExplosionTutor, 0
	object_event 6, OBJ_EVENT_GFX_MART_EMPLOYEE, 2, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsBattleVendor, 0
	object_event 7, OBJ_EVENT_GFX_OLD_MAN, 13, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsMoveTutor, 0

PacifidlogTown_PokemonCenter_1F_MapWarps:
	warp_def 8, 8, 3, 0, MAP_PACIFIDLOG_TOWN
	warp_def 7, 8, 3, 0, MAP_PACIFIDLOG_TOWN
	warp_def 1, 6, 4, 0, MAP_PACIFIDLOG_TOWN_POKEMON_CENTER_2F

PacifidlogTown_PokemonCenter_1F_MapEvents::
	map_events PacifidlogTown_PokemonCenter_1F_ObjectEvents, PacifidlogTown_PokemonCenter_1F_MapWarps, NULL, NULL

