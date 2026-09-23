.set LOCALID_HEATRAN, 1

ScorchedSlab_HeatransRoom_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Common_EventScript_RemoveOverworldAfterBattle
	.byte 0

ScorchedSlab_HeatransRoom_EventScript_Heatran::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_HEATRAN, 2
	delay 40
	waitmoncry
	specialvar VAR_RESULT, GetHighestLevelInPlayerParty
	copyvar VAR_0x800A, VAR_RESULT
	setwildbattle SPECIES_HEATRAN, VAR_0x800A, ITEM_OCCA_BERRY
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	@ Only a capture resolves it; a knockout, escape or forced end retreats.
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, ScorchedSlab_HeatransRoom_EventScript_RanFromHeatran
	setflag FLAG_DEFEATED_HEATRAN
	release
	end

ScorchedSlab_HeatransRoom_EventScript_RanFromHeatran::
	setvar VAR_0x8004, SPECIES_HEATRAN
	goto Common_EventScript_LegendaryFlewAway
	end

ScorchedSlab_HeatransRoom_EventScript_MagmaStone::
	lock
	checkitem ITEM_MAGMA_STONE, 1
	compare VAR_RESULT, TRUE
	goto_if_eq ScorchedSlab_HeatransRoom_EventScript_PlaceMagmaStone
	release
	end

ScorchedSlab_HeatransRoom_EventScript_PlaceMagmaStone::
	msgbox ScorchedSlab_HeatransRoom_Text_PlaceMagmaStone, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq ScorchedSlab_HeatransRoom_EventScript_MagmaStoneEnd
	fadescreen FADE_TO_BLACK
	closemessage
	waitse
	playmoncry SPECIES_HEATRAN, 2
	delay 40
	waitmoncry
	addobject LOCALID_HEATRAN
	clearflag FLAG_DEFEATED_HEATRAN
	fadescreen FADE_FROM_BLACK
	removeitem ITEM_MAGMA_STONE
ScorchedSlab_HeatransRoom_EventScript_MagmaStoneEnd::
	releaseall
	end

ScorchedSlab_HeatransRoom_Text_PlaceMagmaStone:
	.string "The Magma Stone is reacting\n"
	.string "to something…\p"
	.string "Will you set it down here?$"
