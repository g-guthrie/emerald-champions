ScorchedSlab_HeatransRoom_MapScripts::
	.byte 0

ScorchedSlab_HeatransRoom_EventScript_Heatran::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_HEATRAN, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, SPECIES_HEATRAN
	setvar VAR_0x8005, 2
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, ScorchedSlab_HeatransRoom_EventScript_HeatranCaught
	msgbox ScorchedSlab_HeatransRoom_Text_HeatranRemains, MSGBOX_DEFAULT
	release
	end

ScorchedSlab_HeatransRoom_EventScript_HeatranCaught::
	setflag FLAG_EC_CAUGHT_HEATRAN
	removeobject LOCALID_EC_HEATRAN
	release
	end

ScorchedSlab_HeatransRoom_Text_HeatranRemains:
	.string "HEATRAN accepts the challenge, but the\n"
	.string "sanctuary will hold its place for you.$"
