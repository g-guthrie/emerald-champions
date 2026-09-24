MeteorFalls_JirachisRoom_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Common_EventScript_RemoveOverworldAfterBattle
	.byte 0

MeteorFalls_JirachisRoom_EventScript_Jirachi::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_JIRACHI, 2
	delay 40
	waitmoncry
	special GetLevelCapForScriptedGift
	setwildbattle SPECIES_JIRACHI, VAR_0x800A, ITEM_OCCA_BERRY
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	@ A capture resolves it and a knockout loses it for good; an escape,
	@ Red Card, Emergency Exit or any other ending retreats until the
	@ player comes back.
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, MeteorFalls_JirachisRoom_EventScript_DefeatedJirachi
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, MeteorFalls_JirachisRoom_EventScript_RanFromJirachi
	setflag FLAG_DEFEATED_JIRACHI
	release
	end

MeteorFalls_JirachisRoom_EventScript_DefeatedJirachi::
	setflag FLAG_DEFEATED_JIRACHI
	setvar VAR_0x8004, SPECIES_JIRACHI
	goto Common_EventScript_LegendaryGone

MeteorFalls_JirachisRoom_EventScript_RanFromJirachi::
	setvar VAR_0x8004, SPECIES_JIRACHI
	goto Common_EventScript_LegendaryFlewAway
	end
