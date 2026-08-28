MeteorFalls_B1F_1R_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, MeteorFalls_B1F_1R_OnResume
	.byte 0

.set LEGENDARY_SIGN_DIALGA_ID, 10

MeteorFalls_B1F_1R_OnResume:
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, MeteorFalls_B1F_1R_EventScript_TryRemoveDialga
	end

MeteorFalls_B1F_1R_EventScript_TryRemoveDialga::
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
	removeobject LOCALID_METEOR_FALLS_DIALGA
	return

MeteorFalls_B1F_1R_EventScript_Dialga::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_DIALGA, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, LEGENDARY_SIGN_DIALGA_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MeteorFalls_B1F_1R_EventScript_DialgaCaught
	msgbox MeteorFalls_B1F_1R_Text_DialgaRemains, MSGBOX_DEFAULT
	release
	end

MeteorFalls_B1F_1R_EventScript_DialgaCaught::
	release
	end

MeteorFalls_B1F_1R_Text_DialgaRemains:
	.string "The Legendary Sign still rings.\n"
	.string "DIALGA will await your return.$"
