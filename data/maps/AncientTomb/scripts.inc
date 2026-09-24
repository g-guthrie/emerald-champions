AncientTomb_MapScripts:: @ 8238FB3
	map_script MAP_SCRIPT_ON_RESUME, AncientTomb_OnResume
	map_script MAP_SCRIPT_ON_LOAD, AncientTomb_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, AncientTomb_OnTransition
	.byte 0

AncientTomb_OnResume: @ 8238FC3
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, AncientTomb_EventScript_TryRemoveRegisteel
	end

AncientTomb_EventScript_TryRemoveRegisteel:: @ 8238FCD
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_CAUGHT
	goto_if_ne Common_EventScript_NopReturn
	removeobject VAR_LAST_TALKED
	return

AncientTomb_OnTransition: @ 8238FE1
	setflag FLAG_LANDMARK_ANCIENT_TOMB
	call_if_unset FLAG_DEFEATED_REGISTEEL, AncientTomb_EventScript_ShowRegisteel
	end

AncientTomb_EventScript_ShowRegisteel:: @ 8238FEE
	clearflag FLAG_HIDE_REGISTEEL
	return

AncientTomb_OnLoad: @ 8238FF2
	call_if_unset FLAG_SYS_REGISTEEL_PUZZLE_COMPLETED, AncientTomb_EventScript_HideRegiEntrance
	end

AncientTomb_EventScript_HideRegiEntrance:: @ 8238FFC
	setmetatile 7, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 8, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 9, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 7, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 8, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 9, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	return

AncientTomb_EventScript_CaveEntranceMiddle:: @ 8239033
	lockall
	goto_if_set FLAG_SYS_REGISTEEL_PUZZLE_COMPLETED, AncientTomb_EventScript_BigHoleInWall
	braillemessage AncientTomb_Braille_ShineInTheMiddle
	waitbuttonpress
	closebraillemessage
	releaseall
	end

AncientTomb_EventScript_BigHoleInWall:: @ 8239046
	msgbox gText_BigHoleInTheWall, MSGBOX_DEFAULT
	msgbox AncientTomb_Text_GuardianWarning, MSGBOX_DEFAULT
	releaseall
	end

AncientTomb_EventScript_CaveEntranceSide:: @ 8239050
	lockall
	braillemessage AncientTomb_Braille_ShineInTheMiddle
	waitbuttonpress
	closebraillemessage
	releaseall
	end

AncientTomb_EventScript_Registeel:: @ 823905A
	lock
	faceplayer
	waitse
	playmoncry SPECIES_REGISTEEL, 2
	delay 40
	waitmoncry
	special GetLevelCapForScriptedGift
	setwildbattle SPECIES_REGISTEEL, VAR_0x800A, ITEM_METAL_COAT
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special StartRegiBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	@ A capture resolves it and a knockout loses it for good; an escape or
	@ any other ending retreats until the player comes back.
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, AncientTomb_EventScript_DefeatedRegisteel
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, AncientTomb_EventScript_RanFromRegisteel
	setflag FLAG_DEFEATED_REGISTEEL
	call EmeraldChampions_EventScript_ThreeRuinsProgress
	release
	end

AncientTomb_EventScript_DefeatedRegisteel::
	setflag FLAG_DEFEATED_REGISTEEL
	setvar VAR_0x8004, SPECIES_REGISTEEL
	goto Common_EventScript_LegendaryGone

AncientTomb_EventScript_RanFromRegisteel:: @ 82390AA
	setvar VAR_0x8004, SPECIES_REGISTEEL
	goto Common_EventScript_LegendaryFlewAway
	end

@ The point of no return: its guardian is lost for good if it faints.
AncientTomb_Text_GuardianWarning:
	.string "Words are scratched beside it:\p"
	.string "“A guardian that faints here\n"
	.string "never rises again.”$"
