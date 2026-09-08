@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/FuchsiaCity_PokemonCenter_2F_Frlg/map.json
@

	.align 2

FuchsiaCity_PokemonCenter_2F_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_CABLE_CLUB_RECEPTIONIST, 6, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_UnionRoomAttendant, 0
	object_event 2, OBJ_EVENT_GFX_CABLE_CLUB_RECEPTIONIST, 2, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_WirelessClubAttendant, 0
	object_event 3, OBJ_EVENT_GFX_CABLE_CLUB_RECEPTIONIST, 10, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, Common_EventScript_DirectCornerAttendant, 0
	object_event 4, OBJ_EVENT_GFX_MG_DELIVERYMAN, 1, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, CableClub_EventScript_MysteryGiftMan_Frlg, FLAG_HIDE_MG_DELIVERYMEN

FuchsiaCity_PokemonCenter_2F_Frlg_MapWarps:
	warp_def 1, 6, 4, 3, MAP_FUCHSIA_CITY_POKEMON_CENTER_1F
	warp_def 5, 1, 0, 0, MAP_UNION_ROOM_FRLG
	warp_def 9, 1, 0, 0, MAP_TRADE_CENTER_FRLG

FuchsiaCity_PokemonCenter_2F_Frlg_MapEvents::
	map_events FuchsiaCity_PokemonCenter_2F_Frlg_ObjectEvents, FuchsiaCity_PokemonCenter_2F_Frlg_MapWarps, NULL, NULL

