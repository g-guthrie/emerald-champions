DesertRuins_MapScripts:: @ 822D95B
	map_script MAP_SCRIPT_ON_RESUME, DesertRuins_OnResume
	map_script MAP_SCRIPT_ON_LOAD, DesertRuins_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, DesertRuins_OnTransition
	.byte 0

DesertRuins_OnResume: @ 822D96B
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, DesertRuins_EventScript_TryRemoveRegirock
	end

DesertRuins_EventScript_TryRemoveRegirock:: @ 822D975
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_CAUGHT
	goto_if_ne Common_EventScript_NopReturn
	removeobject VAR_LAST_TALKED
	return

DesertRuins_OnLoad: @ 822D989
	call_if_unset FLAG_SYS_REGIROCK_PUZZLE_COMPLETED, DesertRuins_EventScript_HideRegiEntrance
	end

DesertRuins_EventScript_HideRegiEntrance:: @ 822D993
	setmetatile 7, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 8, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 9, 19, METATILE_Cave_EntranceCover, 1
	setmetatile 7, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 8, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	setmetatile 9, 20, METATILE_Cave_SealedChamberBraille_Mid, 1
	return

DesertRuins_OnTransition: @ 822D9CA
	setflag FLAG_LANDMARK_DESERT_RUINS
	call_if_unset FLAG_DEFEATED_REGIROCK, DesertRuins_EventScript_ShowRegirock
	end

DesertRuins_EventScript_ShowRegirock:: @ 822D9D7
	clearflag FLAG_HIDE_REGIROCK
	return

DesertRuins_EventScript_CaveEntranceMiddle:: @ 822D9DB
	lockall
	goto_if_set FLAG_SYS_REGIROCK_PUZZLE_COMPLETED, DesertRuins_EventScript_BigHoleInWall
	braillemessage DesertRuins_Braille_UseRockSmash
	waitbuttonpress
	closebraillemessage
	releaseall
	end

DesertRuins_EventScript_BigHoleInWall:: @ 822D9EE
	msgbox gText_BigHoleInTheWall, MSGBOX_DEFAULT
	msgbox DesertRuins_Text_GuardianWarning, MSGBOX_DEFAULT
	releaseall
	end

DesertRuins_EventScript_CaveEntranceSide:: @ 822D9F8
	lockall
	braillemessage DesertRuins_Braille_UseRockSmash
	waitbuttonpress
	closebraillemessage
	releaseall
	end

DesertRuins_EventScript_Regirock:: @ 822DA02
	lock
	faceplayer
	waitse
	playmoncry SPECIES_REGIROCK, 2
	delay 40
	waitmoncry
	special GetLevelCapForScriptedGift
	setwildbattle SPECIES_REGIROCK, VAR_0x800A, ITEM_HARD_STONE
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special StartRegiBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	@ A capture resolves it and a knockout loses it for good; an escape or
	@ any other ending retreats until the player comes back.
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, DesertRuins_EventScript_DefeatedRegirock
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, DesertRuins_EventScript_RanFromRegirock
	setflag FLAG_DEFEATED_REGIROCK
	call EmeraldChampions_EventScript_ThreeRuinsProgress
	release
	end

DesertRuins_EventScript_DefeatedRegirock::
	setflag FLAG_DEFEATED_REGIROCK
	setvar VAR_0x8004, SPECIES_REGIROCK
	goto Common_EventScript_LegendaryGone

DesertRuins_EventScript_RanFromRegirock:: @ 822DA52
	setvar VAR_0x8004, SPECIES_REGIROCK
	goto Common_EventScript_LegendaryFlewAway
	end

@ The point of no return: its guardian is lost for good if it faints.
DesertRuins_Text_GuardianWarning:
	.string "Words are scratched beside it:\p"
	.string "“A guardian that faints here\n"
	.string "never rises again.”$"
