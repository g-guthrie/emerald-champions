MeteorFalls_JirachisRoom_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MeteorFalls_JirachisRoom_OnTransition
	.byte 0

MeteorFalls_JirachisRoom_OnTransition:
	setflag FLAG_EC_SURVEYED_METEOR_CHAMBER
	end

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
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_LegendaryResting
	goto MeteorFalls_JirachisRoom_EventScript_JirachiCaught
	end

MeteorFalls_JirachisRoom_EventScript_JirachiCaught::
	setflag FLAG_EC_CAUGHT_JIRACHI
	removeobject LOCALID_EC_JIRACHI
	release
	end

