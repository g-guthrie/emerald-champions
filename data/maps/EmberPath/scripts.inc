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
	msgbox EmberPath_Text_HotAir, MSGBOX_DEFAULT
	closemessage
	special DoSealedChamberShakingEffect_Short
	waitse
	playmoncry SPECIES_MOLTRES, CRY_MODE_ENCOUNTER
	applymovement LOCALID_EC_MOLTRES, EmberPath_Movement_SpreadWings
	waitmovement 0
	delay 20
	waitmoncry
	setvar VAR_0x8004, SPECIES_MOLTRES
	setvar VAR_0x8005, 1
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, EmberPath_EventScript_MoltresCaught
	goto_if_ne VAR_RESULT, B_OUTCOME_WON, Common_EventScript_LegendaryResting
	setflag FLAG_EC_RESOLVED_MOLTRES
	goto Common_EventScript_LegendaryResting
	end

EmberPath_EventScript_MoltresCaught::
	setflag FLAG_EC_RESOLVED_MOLTRES
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

EmberPath_Text_HotAir:
	.string "A rush of hot air sweeps across\n"
	.string "the path.\p"
	.string "MOLTRES spreads its wings!$"

EmberPath_Movement_SpreadWings:
	lock_facing_direction
	walk_in_place_down
	walk_in_place_down
	unlock_facing_direction
	step_end
