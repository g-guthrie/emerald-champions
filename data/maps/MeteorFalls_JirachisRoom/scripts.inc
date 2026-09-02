MeteorFalls_JirachisRoom_MapScripts::
	.byte 0

MeteorFalls_JirachisRoom_EventScript_Jirachi::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, SPECIES_JIRACHI
	setvar VAR_0x8005, 2
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MeteorFalls_JirachisRoom_EventScript_JirachiCaught
	msgbox MeteorFalls_JirachisRoom_Text_JirachiRemains, MSGBOX_DEFAULT
	release
	end

MeteorFalls_JirachisRoom_EventScript_JirachiCaught::
	setflag FLAG_EC_CAUGHT_JIRACHI
	removeobject LOCALID_EC_JIRACHI
	release
	end

MeteorFalls_JirachisRoom_Text_JirachiRemains:
	.string "JIRACHI accepts the challenge, but the\n"
	.string "sanctuary will hold its place for you.$"
