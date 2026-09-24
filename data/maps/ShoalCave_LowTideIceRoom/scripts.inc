ShoalCave_LowTideIceRoom_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Common_EventScript_RemoveOverworldAfterBattle
	.byte 0

ShoalCave_LowTideIceRoom_EventScript_Articuno::
	lock
	faceplayer
	goto_if_unset FLAG_BADGE06_GET, Common_EventScript_LegendaryNotReady
	waitse
	playmoncry SPECIES_ARTICUNO, 2
	delay 40
	waitmoncry
	special GetLevelCapForScriptedGift
	setwildbattle SPECIES_ARTICUNO, VAR_0x800A, ITEM_YACHE_BERRY
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
ShoalCave_LowTideIceRoom_EventScript_CheckArticunoResult::
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_0x8004, SPECIES_ARTICUNO
	specialvar VAR_RESULT, GetBattleOutcome
	@ A knockout loses it for good; capture resolves it; anything else retreats.
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, Common_EventScript_LegendaryGone
	goto Common_EventScript_EndVisibleLegendaryEncounter
