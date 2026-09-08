@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/FallarborTown_PokemonCenter_1F/map.json
@

	.align 2

FallarborTown_PokemonCenter_1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_NURSE, 8, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, FallarborTown_PokemonCenter_1F_EventScript_Nurse, 0
	object_event 2, OBJ_EVENT_GFX_GIRL_3, 12, 7, 3, MOVEMENT_TYPE_FACE_LEFT, 0, 0, TRAINER_TYPE_NONE, 0, FallarborTown_PokemonCenter_1F_EventScript_Girl, 0
	object_event 3, OBJ_EVENT_GFX_EXPERT_M, 5, 4, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, FallarborTown_PokemonCenter_1F_EventScript_ExpertM, 0
	object_event 4, OBJ_EVENT_GFX_WOMAN_2, 11, 4, 3, MOVEMENT_TYPE_FACE_UP, 0, 0, TRAINER_TYPE_NONE, 0, FallarborTown_PokemonCenter_1F_EventScript_Lanette, FLAG_HIDE_FALLARBOR_POKEMON_CENTER_LANETTE
	object_event 5, OBJ_EVENT_GFX_ROOFTOP_SALE_WOMAN, 1, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, FallarborTown_PokemonCenter_1F_EventScript_Clerk2, 0
	object_event 6, OBJ_EVENT_GFX_MART_EMPLOYEE, 2, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsBattleVendor, 0
	object_event 7, OBJ_EVENT_GFX_OLD_MAN, 13, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsMoveTutor, 0

FallarborTown_PokemonCenter_1F_MapWarps:
	warp_def 8, 8, 3, 2, MAP_FALLARBOR_TOWN
	warp_def 7, 8, 3, 2, MAP_FALLARBOR_TOWN
	warp_def 1, 6, 4, 0, MAP_FALLARBOR_TOWN_POKEMON_CENTER_2F

FallarborTown_PokemonCenter_1F_MapEvents::
	map_events FallarborTown_PokemonCenter_1F_ObjectEvents, FallarborTown_PokemonCenter_1F_MapWarps, NULL, NULL

