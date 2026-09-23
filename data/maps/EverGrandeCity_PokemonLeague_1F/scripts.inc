.set LOCALID_NURSE, 1
.set LOCALID_GUARD_1, 3
.set LOCALID_GUARD_2, 4

EverGrandeCity_PokemonLeague_1F_MapScripts:: @ 82295D2
	map_script MAP_SCRIPT_ON_TRANSITION, EverGrandeCity_PokemonLeague_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

EverGrandeCity_PokemonLeague_1F_OnTransition:
	setrespawn HEAL_LOCATION_EVER_GRANDE_CITY_POKEMON_LEAGUE
	setflag FLAG_LANDMARK_POKEMON_LEAGUE
	call_if_unset FLAG_ENTERED_ELITE_FOUR, EverGrandeCity_PokemonLeague_1F_EventScript_GuardsBlockDoor
	end

EverGrandeCity_PokemonLeague_1F_EventScript_GuardsBlockDoor::
	setobjectxyperm LOCALID_GUARD_1, 9, 2
	setobjectxyperm LOCALID_GUARD_2, 10, 2
	return

EverGrandeCity_PokemonLeague_1F_EventScript_Nurse:: @ 82295FC
	setvar VAR_0x800B, LOCALID_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard:: @ 8229636
	lockall
	compare VAR_TEMP_2, 1
	goto_if_eq EverGrandeCity_PokemonLeague_1F_EventScriptSwitchTrainerMsg
	getplayerxy VAR_TEMP_0, VAR_TEMP_1
	compare VAR_TEMP_0, 11
	call_if_ge EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromRight
	compare VAR_TEMP_0, 8
	call_if_le EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromLeft
	goto_if_set FLAG_ENTERED_ELITE_FOUR, EverGrandeCity_PokemonLeague_1F_EventScript_GoForth
	message EverGrandeCity_PokemonLeague_1F_Text_MustHaveAllGymBadges
	waitmessage
	delay 120
EverGrandeCity_PokemonLeague_1F_EventScript_CheckBadges::
	goto_if_unset FLAG_BADGE01_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	goto_if_unset FLAG_BADGE02_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	goto_if_unset FLAG_BADGE03_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	goto_if_unset FLAG_BADGE04_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	goto_if_unset FLAG_BADGE05_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	goto_if_unset FLAG_BADGE06_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	goto_if_unset FLAG_BADGE07_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	goto_if_unset FLAG_BADGE08_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges
	specialvar VAR_RESULT, PlayerPartyLeagueEligible
	compare VAR_RESULT, FALSE
	goto_if_eq EverGrandeCity_PokemonLeague_1F_EventScript_RestrictedParty
EverGrandeCity_PokemonLeague_1F_EventScript_AdmitTrainer::
	closemessage
	setvar VAR_TEMP_2, 1
	applymovement LOCALID_GUARD_1, EverGrandeCity_PokemonLeague_1F_Movement_LeftGuardOutOfWay
	applymovement LOCALID_GUARD_2, EverGrandeCity_PokemonLeague_1F_Movement_RightGuardOutOfWay
	waitmovement 0
	delay 10
	playfanfare MUS_OBTAIN_BADGE
	message EverGrandeCity_PokemonLeague_1F_Text_GoForth
	waitmessage
	waitfanfare
	closemessage
	copyobjectxytoperm LOCALID_GUARD_1
	copyobjectxytoperm LOCALID_GUARD_2
	setflag FLAG_ENTERED_ELITE_FOUR
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_EventScriptSwitchTrainerMsg:
	goto_if_set FLAG_SYS_GAME_CLEAR, EverGrandeCity_PokemonLeague_1F_EventScriptGoForthChampion
	goto EverGrandeCity_PokemonLeague_1F_EventScript_GoForthFacePlayer

EverGrandeCity_PokemonLeague_1F_EventScriptGoForthChampion2::
	message EverGrandeCity_PokemonLeague_1F_Text_GoForthChampion
	waitmessage
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	copyobjectxytoperm LOCALID_GUARD_1
	copyobjectxytoperm LOCALID_GUARD_2
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_EventScriptGoForthChampion::
	applymovement VAR_LAST_TALKED, Common_Movement_FacePlayer
	waitmovement 0
	goto EverGrandeCity_PokemonLeague_1F_EventScriptGoForthChampion2

EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromRight:: @ 8229698
	applymovement OBJ_EVENT_ID_PLAYER, EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromRight
	waitmovement 0
	return

EverGrandeCity_PokemonLeague_1F_EventScript_PlayerMoveToFrontFromLeft:: @ 82296A3
	applymovement OBJ_EVENT_ID_PLAYER, EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromLeft
	waitmovement 0
	return

EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges:: @ 82296AE
	playse SE_FAILURE
	msgbox EverGrandeCity_PokemonLeague_1F_Text_HaventObtainedAllBadges, MSGBOX_DEFAULT
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_EventScript_RestrictedParty::
	msgbox EverGrandeCity_PokemonLeague_1F_Text_RestrictedParty, MSGBOX_DEFAULT
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_EventScript_CheckLeagueDoor::
	lockall
	specialvar VAR_RESULT, PlayerPartyLeagueEligible
	compare VAR_RESULT, TRUE
	goto_if_eq EverGrandeCity_PokemonLeague_1F_EventScript_LeagueDoorAllowed
	msgbox EverGrandeCity_PokemonLeague_1F_Text_RestrictedParty, MSGBOX_DEFAULT
	applymovement OBJ_EVENT_ID_PLAYER, EverGrandeCity_PokemonLeague_1F_Movement_StepBack
	waitmovement 0
EverGrandeCity_PokemonLeague_1F_EventScript_LeagueDoorAllowed::
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_Movement_StepBack::
	walk_down
	step_end

EverGrandeCity_PokemonLeague_1F_EventScript_GoForth:: @ 82296BB
	compare VAR_TEMP_2, 1
	goto_if_eq EverGrandeCity_PokemonLeague_1F_EventScript_GoForthFacePlayer
	applymovement VAR_LAST_TALKED, Common_Movement_FacePlayer
	waitmovement 0
	applymovement LOCALID_GUARD_1, EverGrandeCity_PokemonLeague_1F_Movement_LeftGuardOutOfWay
	applymovement LOCALID_GUARD_2, EverGrandeCity_PokemonLeague_1F_Movement_RightGuardOutOfWay
	waitmovement 0
	delay 10
	playfanfare MUS_OBTAIN_BADGE
	setvar VAR_TEMP_2, 1
EverGrandeCity_PokemonLeague_1F_EventScript_GoForth2::
	msgbox EverGrandeCity_PokemonLeague_1F_Text_GoForth, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

EverGrandeCity_PokemonLeague_1F_EventScript_GoForthFacePlayer::
	applymovement VAR_LAST_TALKED, Common_Movement_FacePlayer
	waitmovement 0
	goto EverGrandeCity_PokemonLeague_1F_EventScript_GoForth2

EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromRight: @ 82296DA
	walk_down
	walk_left
	walk_in_place_fast_up
	step_end

EverGrandeCity_PokemonLeague_1F_Movement_MoveToFrontFromLeft: @ 82296DE
	walk_down
	walk_right
	walk_in_place_fast_up
	step_end

EverGrandeCity_PokemonLeague_1F_Movement_LeftGuardOutOfWay: @ 82296E2
	walk_left
	walk_in_place_fast_down
	step_end

EverGrandeCity_PokemonLeague_1F_Movement_RightGuardOutOfWay: @ 82296E5
	walk_right
	walk_in_place_fast_down
	step_end

EverGrandeCity_PokemonLeague_1F_Text_MustHaveAllGymBadges: @ 82296E8
	.string "Beyond this point, only those Trainers\n"
	.string "who have collected all the Gym Badges\l"
	.string "are permitted to enter.\p"
	.string "Trainer, let us confirm that you have\n"
	.string "all the Gym Badges.$"

EverGrandeCity_PokemonLeague_1F_Text_HaventObtainedAllBadges: @ 8229787
	.string "You haven't obtained all the Badges.\p"
	.string "If you're bound for the Pokémon\n"
	.string "League, you must return with them.$"

EverGrandeCity_PokemonLeague_1F_Text_GoForth: @ 82297EF
	.string "Trainer! Believe in yourself and your\n"
	.string "Pokémon, and go forth!$"

EverGrandeCity_PokemonLeague_1F_Text_RestrictedParty::
	.string "League rules exclude Legendary\n"
	.string "and Mythical Pokémon.\p"
	.string "Ultra Beasts and Paradox Pokémon\n"
	.string "may still enter.$"

EverGrandeCity_PokemonLeague_1F_Text_GoForthChampion:
	.string "Champion! Believe in yourself and your\n"
	.string "Pokémon, and go forth!$"
