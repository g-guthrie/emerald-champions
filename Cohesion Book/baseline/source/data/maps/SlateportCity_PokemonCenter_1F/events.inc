@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/SlateportCity_PokemonCenter_1F/map.json
@

	.align 2

SlateportCity_PokemonCenter_1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_NURSE, 8, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, SlateportCity_PokemonCenter_1F_EventScript_Nurse, 0
	object_event 2, OBJ_EVENT_GFX_SAILOR, 4, 5, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, SlateportCity_PokemonCenter_1F_EventScript_Sailor, 0
	object_event 3, OBJ_EVENT_GFX_WOMAN_5, 12, 7, 3, MOVEMENT_TYPE_FACE_RIGHT, 0, 0, TRAINER_TYPE_NONE, 0, SlateportCity_PokemonCenter_1F_EventScript_Woman, 0
	object_event 4, OBJ_EVENT_GFX_MART_EMPLOYEE, 2, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsBattleVendor, 0
	object_event 5, OBJ_EVENT_GFX_OLD_MAN, 13, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_EmeraldChampionsMoveTutor, 0

SlateportCity_PokemonCenter_1F_MapWarps:
	warp_def 8, 8, 3, 0, MAP_SLATEPORT_CITY
	warp_def 7, 8, 3, 0, MAP_SLATEPORT_CITY
	warp_def 1, 6, 4, 0, MAP_SLATEPORT_CITY_POKEMON_CENTER_2F

SlateportCity_PokemonCenter_1F_MapEvents::
	map_events SlateportCity_PokemonCenter_1F_ObjectEvents, SlateportCity_PokemonCenter_1F_MapWarps, NULL, NULL

