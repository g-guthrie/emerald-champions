EmberPath_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Common_EventScript_RemoveOverworldAfterBattle
	map_script MAP_SCRIPT_ON_TRANSITION, EmberPath_OnTransition
	.byte 0

EmberPath_OnTransition:
	setflag FLAG_LANDMARK_EMBER_PATH
	end

EmberPath_EventScript_Moltres::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_MOLTRES, 2
	delay 40
	waitmoncry
	special GetLevelCapForScriptedGift
	setwildbattle SPECIES_MOLTRES, VAR_0x800A, ITEM_OCCA_BERRY
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	@ A capture resolves it and a knockout loses it for good; an escape,
	@ Red Card, Emergency Exit or any other ending retreats until the
	@ player comes back.
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, EmberPath_EventScript_DefeatedMoltres
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, EmberPath_EventScript_RanFromMoltres
	setflag FLAG_DEFEATED_MOLTRES
	setflag FLAG_EC_CAUGHT_MOLTRES
	release
	end

EmberPath_EventScript_DefeatedMoltres::
	setflag FLAG_DEFEATED_MOLTRES
	setvar VAR_0x8004, SPECIES_MOLTRES
	goto Common_EventScript_LegendaryGone

EmberPath_EventScript_RanFromMoltres:: @ 8238FA8
	setvar VAR_0x8004, SPECIES_MOLTRES
	goto Common_EventScript_LegendaryFlewAway
	end
