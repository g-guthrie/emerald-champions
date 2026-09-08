@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/MauvilleCity_PokemonCenter_1F/map.json
@

	.align 2

MauvilleCity_PokemonCenter_1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_NURSE, 8, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, MauvilleCity_PokemonCenter_1F_EventScript_Nurse, 0
	object_event 2, OBJ_EVENT_GFX_VAR_0, 5, 4, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 0, TRAINER_TYPE_NONE, 0, MauvilleCity_PokemonCenter_1F_EventScript_MauvilleOldMan, 0
	object_event 3, OBJ_EVENT_GFX_WOMAN_1, 12, 7, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, 0, MauvilleCity_PokemonCenter_1F_EventScript_Woman1, 0
	object_event 4, OBJ_EVENT_GFX_WOMAN_5, 10, 6, 3, MOVEMENT_TYPE_WANDER_UP_AND_DOWN, 0, 1, TRAINER_TYPE_NONE, 0, MauvilleCity_PokemonCenter_1F_EventScript_Woman2, 0
	object_event 5, OBJ_EVENT_GFX_YOUNGSTER, 3, 5, 3, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, MauvilleCity_PokemonCenter_1F_EventScript_Youngster, 0
	object_event 6, OBJ_EVENT_GFX_MART_EMPLOYEE, 2, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsBattleVendor, 0
	object_event 7, OBJ_EVENT_GFX_OLD_MAN, 13, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsMoveTutor, 0

MauvilleCity_PokemonCenter_1F_MapWarps:
	warp_def 8, 8, 3, 1, MAP_MAUVILLE_CITY
	warp_def 7, 8, 3, 1, MAP_MAUVILLE_CITY
	warp_def 1, 6, 4, 0, MAP_MAUVILLE_CITY_POKEMON_CENTER_2F

MauvilleCity_PokemonCenter_1F_MapEvents::
	map_events MauvilleCity_PokemonCenter_1F_ObjectEvents, MauvilleCity_PokemonCenter_1F_MapWarps, NULL, NULL

