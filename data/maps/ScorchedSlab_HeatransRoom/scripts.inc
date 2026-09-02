ScorchedSlab_HeatransRoom_MapScripts::
	.byte 0

ScorchedSlab_HeatransRoom_EventScript_MagmaStone::
	lock
	checkitem ITEM_MAGMA_STONE, 1
	goto_if_eq VAR_RESULT, TRUE, ScorchedSlab_HeatransRoom_EventScript_PlaceMagmaStone
	release
	end

ScorchedSlab_HeatransRoom_EventScript_PlaceMagmaStone::
	msgbox ScorchedSlab_HeatransRoom_Text_PlaceMagmaStone, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ScorchedSlab_HeatransRoom_EventScript_MagmaStoneEnd
	fadescreen FADE_TO_BLACK
	closemessage
	waitse
	playmoncry SPECIES_HEATRAN, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	clearflag FLAG_EC_CAUGHT_HEATRAN
	addobject LOCALID_EC_HEATRAN
	fadescreen FADE_FROM_BLACK
	removeitem ITEM_MAGMA_STONE
ScorchedSlab_HeatransRoom_EventScript_MagmaStoneEnd::
	releaseall
	end

ScorchedSlab_HeatransRoom_Text_PlaceMagmaStone:
	.string "The MAGMA STONE is reacting\n"
	.string "to something…\p"
	.string "Will you set it down here?$"

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
