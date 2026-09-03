EmberPath_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, EmberPath_OnTransition
	.byte 0

EmberPath_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_EMBER_PATH
	end

EmberPath_EventScript_Warden::
	msgbox EmberPath_Text_Warden, MSGBOX_NPC
	end
EmberPath_Text_Warden:
	.string "TEAM MAGMA's machine woke every fault\n"
	.string "beneath this mountain.\p"
	.string "MOLTRES came to test the heat. Farther\n"
	.string "east, something heavier answered.$"
EmberPath_EventScript_Moltres::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_MOLTRES, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, SPECIES_MOLTRES
	setvar VAR_0x8005, 1
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, EmberPath_EventScript_MoltresCaught
	msgbox EmberPath_Text_MoltresRemains, MSGBOX_DEFAULT
	release
	end

EmberPath_EventScript_MoltresCaught::
	setflag FLAG_EC_CAUGHT_MOLTRES
	removeobject LOCALID_EC_MOLTRES
	release
	end

EmberPath_Text_MoltresRemains:
	.string "MOLTRES accepts the challenge, but the\n"
	.string "sanctuary will hold its place for you.$"
