.set LOCALID_NURSE, 1

PacifidlogTown_PokemonCenter_1F_MapScripts:: @ 82034A7
	map_script MAP_SCRIPT_ON_TRANSITION, PacifidlogTown_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

PacifidlogTown_PokemonCenter_1F_OnTransition: @ 82034B2
	setrespawn HEAL_LOCATION_PACIFIDLOG_TOWN
	end

PacifidlogTown_PokemonCenter_1F_EventScript_Nurse:: @ 82034B6
	setvar VAR_0x800B, LOCALID_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

PacifidlogTown_PokemonCenter_1F_EventScript_Girl:: @ 82034C4
	msgbox PacifidlogTown_PokemonCenter_1F_Text_WhatColorTrainerCard, MSGBOX_NPC
	end

PacifidlogTown_PokemonCenter_1F_EventScript_Woman:: @ 82034CD
	msgbox PacifidlogTown_PokemonCenter_1F_Text_OnColonyOfCorsola, MSGBOX_NPC
	end

PacifidlogTown_PokemonCenter_1F_EventScript_OldMan:: @ 82034D6
	msgbox PacifidlogTown_PokemonCenter_1F_Text_AncestorsLivedOnBoats, MSGBOX_NPC
	end

PacifidlogTown_PokemonCenter_1F_EventScript_ExplosionTM::
	lock
	faceplayer
	goto_if_set FLAG_EC_GIFT_PACIFIDLOG_TOWN_POKEMON_CENTER_1_F, PacifidlogTown_PokemonCenter_1F_EventScript_HaveExplosion
	msgbox PacifidlogTown_PokemonCenter_1F_Text_GiveExplosion, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_GIFT_PACIFIDLOG_TOWN_POKEMON_CENTER_1_F
	msgbox PacifidlogTown_PokemonCenter_1F_Text_HaveExplosion, MSGBOX_DEFAULT
	release
	end

PacifidlogTown_PokemonCenter_1F_EventScript_HaveExplosion::
	msgbox PacifidlogTown_PokemonCenter_1F_Text_HaveExplosion, MSGBOX_DEFAULT
	release
	end

PacifidlogTown_PokemonCenter_1F_Text_WhatColorTrainerCard: @ 82034DF
	.string "What color is your Trainer Card?\n"
	.string "Mine's copper!$"

PacifidlogTown_PokemonCenter_1F_Text_OnColonyOfCorsola: @ 820350F
	.string "Pacifidlog Town floats on top of\n"
	.string "a colony of Corsola.\p"
	.string "If I told you that, would you believe\n"
	.string "me?$"

PacifidlogTown_PokemonCenter_1F_Text_AncestorsLivedOnBoats: @ 820356F
	.string "The ancestors of the people in\n"
	.string "Pacifidlog were said to have been\l"
	.string "born on boats and then lived and died\l"
	.string "aboard them.\p"
	.string "I understand that they lived that way\n"
	.string "because they were searching for\l"
	.string "something.$"

PacifidlogTown_PokemonCenter_1F_Text_GiveExplosion: @ 82C7CFA
	.string "You have earned a little help\n"
	.string "for your journey. Please take\l"
	.string "this Rare Candy!$"

PacifidlogTown_PokemonCenter_1F_Text_HaveExplosion: @ 82C7E7A
	.string "I will become a star one day.\n"
	.string "Use that Rare Candy to help your\l"
	.string "partner grow, too!$"
