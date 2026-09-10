EmberPath_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, EmberPath_OnTransition
	.byte 0

EmberPath_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_EMBER_PATH
	end

EmberPath_EventScript_Warden::
	goto_if_set FLAG_EC_CAUGHT_MOLTRES, EmberPath_EventScript_WardenMoltresCaught
	msgbox EmberPath_Text_Warden, MSGBOX_NPC
	end
EmberPath_Text_Warden:
	.string "The mountain has been restless.\n"
	.string "TEAM MAGMA tampering with it\l"
	.string "certainly did not help.\p"
	.string "MOLTRES roosts along this path.\n"
	.string "It seems quite at home with the heat.$"
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
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_LegendaryResting
	goto EmberPath_EventScript_MoltresCaught
	end

EmberPath_EventScript_MoltresCaught::
	setflag FLAG_EC_CAUGHT_MOLTRES
	removeobject LOCALID_EC_MOLTRES
	release
	end


EmberPath_EventScript_WardenMoltresCaught::
	msgbox EmberPath_Text_WardenMoltresCaught, MSGBOX_NPC
	end

EmberPath_Text_WardenMoltresCaught:
	.string "MOLTRES has left its roost here\n"
	.string "to travel with you.\p"
	.string "The mountain is still restless.\n"
	.string "Take care along the hot slopes.$"
