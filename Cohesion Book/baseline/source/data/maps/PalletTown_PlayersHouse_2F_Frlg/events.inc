@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/PalletTown_PlayersHouse_2F_Frlg/map.json
@

	.align 2

PalletTown_PlayersHouse_2F_Frlg_MapWarps:
	warp_def 10, 2, 3, 2, MAP_PALLET_TOWN_PLAYERS_HOUSE_1F

PalletTown_PlayersHouse_2F_Frlg_MapBGEvents:
	bg_sign_event 6, 5, 3, BG_EVENT_PLAYER_FACING_ANY, PalletTown_PlayersHouse_2F_EventScript_NES
	bg_sign_event 1, 1, 0, BG_EVENT_PLAYER_FACING_ANY, EventScript_PalletTown_PlayersHouse_2F_TurnOnPC
	bg_sign_event 11, 1, 0, BG_EVENT_PLAYER_FACING_ANY, PalletTown_PlayersHouse_2F_EventScript_Sign

PalletTown_PlayersHouse_2F_Frlg_MapEvents::
	map_events NULL, PalletTown_PlayersHouse_2F_Frlg_MapWarps, NULL, PalletTown_PlayersHouse_2F_Frlg_MapBGEvents

