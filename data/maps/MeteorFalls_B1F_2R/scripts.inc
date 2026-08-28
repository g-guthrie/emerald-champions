MeteorFalls_B1F_2R_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, MeteorFalls_B1F_2R_OnResume
	.byte 0

.set LEGENDARY_SIGN_CRESSELIA_ID, 8

MeteorFalls_B1F_2R_OnResume:
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, MeteorFalls_B1F_2R_EventScript_TryRemoveCresselia
	end

MeteorFalls_B1F_2R_EventScript_TryRemoveCresselia::
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
	removeobject LOCALID_METEOR_FALLS_CRESSELIA
	return

MeteorFalls_B1F_2R_EventScript_Cresselia::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_CRESSELIA, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, LEGENDARY_SIGN_CRESSELIA_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MeteorFalls_B1F_2R_EventScript_CresseliaCaught
	msgbox MeteorFalls_B1F_2R_Text_CresseliaRemains, MSGBOX_DEFAULT
	release
	end

MeteorFalls_B1F_2R_EventScript_CresseliaCaught::
	release
	end

MeteorFalls_B1F_2R_Text_CresseliaRemains:
	.string "The Legendary Sign still shines.\n"
	.string "CRESSELIA will await your return.$"
