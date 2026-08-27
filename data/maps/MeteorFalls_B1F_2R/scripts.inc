MeteorFalls_B1F_2R_MapScripts:: @ 822CE26
	map_script MAP_SCRIPT_ON_RESUME, Common_EventScript_RemoveOverworldAfterBattle
	.byte 0

.set LEGENDARY_SIGN_CRESSELIA_ID, 8

MeteorFalls_B1F_2R_EventScript_Cresselia::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_CRESSELIA, 2
	delay 40
	waitmoncry
	setvar VAR_0x8004, LEGENDARY_SIGN_CRESSELIA_ID
	specialvar VAR_RESULT, GetSelectedLegendarySignLevel
	copyvar VAR_0x800A, VAR_RESULT
	setwildbattle SPECIES_CRESSELIA, VAR_0x800A, ITEM_NONE
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_CAUGHT
	goto_if_eq MeteorFalls_B1F_2R_EventScript_CresseliaCaught
	setvar VAR_0x8004, SPECIES_CRESSELIA
	goto Common_EventScript_LegendaryFlewAway

MeteorFalls_B1F_2R_EventScript_CresseliaCaught:
	release
	end
