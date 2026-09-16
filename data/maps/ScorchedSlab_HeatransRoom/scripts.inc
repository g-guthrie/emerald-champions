ScorchedSlab_HeatransRoom_MapScripts::
	.byte 0

ScorchedSlab_HeatransRoom_EventScript_MagmaStone::
	call EC_RequireGroudon
	lock
	specialvar VAR_RESULT, GetHeatranDiscoveryState
	goto_if_ne VAR_RESULT, 0, ScorchedSlab_HeatransRoom_EventScript_MagmaStoneRetryGift
	setvar VAR_0x8004, ITEM_MAGMA_STONE
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, TRUE, ScorchedSlab_HeatransRoom_EventScript_PlaceMagmaStone
	msgbox ScorchedSlab_HeatransRoom_Text_MissingStone, MSGBOX_DEFAULT
	release
	end

ScorchedSlab_HeatransRoom_EventScript_PlaceMagmaStone::
	msgbox ScorchedSlab_HeatransRoom_Text_PlaceMagmaStone, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, ScorchedSlab_HeatransRoom_EventScript_MagmaStoneEnd
	msgbox ScorchedSlab_HeatransRoom_Text_StoneAnswers, MSGBOX_DEFAULT
	closemessage
	special DoSealedChamberShakingEffect_Short
	waitstate
	fadescreen FADE_TO_BLACK
	waitse
	playmoncry SPECIES_HEATRAN, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	clearflag FLAG_EC_CAUGHT_HEATRAN
	addobject LOCALID_EC_HEATRAN
	fadescreen FADE_FROM_BLACK
	setvar VAR_0x8004, ITEM_MAGMA_STONE
	special TakeEmeraldChampionsHandoffItem
ScorchedSlab_HeatransRoom_EventScript_MagmaStoneEnd::
	releaseall
	end

@ Repeatable: stepping back onto the altar after HEATRAN is resolved retries
@ the gift if a full Bag stopped it in ScorchedSlab_HeatransRoom_EventScript_Report.
ScorchedSlab_HeatransRoom_EventScript_MagmaStoneRetryGift::
	call ScorchedSlab_HeatransRoom_EventScript_MegaGift_HEATRANITE
	goto ScorchedSlab_HeatransRoom_EventScript_MagmaStoneEnd
	end

ScorchedSlab_HeatransRoom_EventScript_MegaGift_HEATRANITE::
	goto_if_set FLAG_EC_MEGA_GIFT_HEATRANITE, ScorchedSlab_HeatransRoom_EventScript_MegaGift_HEATRANITE_Done
	giveuniqueitem ITEM_HEATRANITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_HEATRANITE
ScorchedSlab_HeatransRoom_EventScript_MegaGift_HEATRANITE_Done::
	return

ScorchedSlab_HeatransRoom_Text_PlaceMagmaStone:
	.string "The MAGMA STONE is reacting\n"
	.string "to something…\p"
	.string "Will you set it down here?$"

ScorchedSlab_HeatransRoom_EventScript_Heatran::
	call EC_RequireGroudon
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
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, ScorchedSlab_HeatransRoom_EventScript_HeatranCaught
	goto_if_ne VAR_RESULT, B_OUTCOME_WON, Common_EventScript_LegendaryResting
	call ScorchedSlab_HeatransRoom_EventScript_Report
	goto Common_EventScript_LegendaryResting
	end

ScorchedSlab_HeatransRoom_EventScript_HeatranCaught::
	call ScorchedSlab_HeatransRoom_EventScript_Report
	setflag FLAG_EC_CAUGHT_HEATRAN
	removeobject LOCALID_EC_HEATRAN
	release
	end


ScorchedSlab_HeatransRoom_EventScript_Report::
	goto_if_set FLAG_EC_REPORT_C39_COMPLETE, Common_EventScript_NopReturn
	setflag FLAG_EC_RESOLVED_HEATRAN
	completemilestone FLAG_EC_REPORT_C39_COMPLETE
	msgbox ScorchedSlab_HeatransRoom_Text_StoneCools, MSGBOX_DEFAULT
	msgbox ScorchedSlab_HeatransRoom_Text_MagmaStoneSplit, MSGBOX_DEFAULT
	call ScorchedSlab_HeatransRoom_EventScript_MegaGift_HEATRANITE
	buffermilestonetext FLAG_EC_REPORT_C39_COMPLETE
	call_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_PrintMilestoneCapLine
	call_if_eq VAR_RESULT, 3, EmeraldChampions_EventScript_PrintMilestoneCapLine
	return

ScorchedSlab_HeatransRoom_Text_StoneAnswers:
	.string "The MAGMA STONE grows warm.\n"
	.string "Something answers from beneath\l"
	.string "the chamber.$"

ScorchedSlab_HeatransRoom_Text_StoneCools:
	.string "The stone has gone cold.\n"
	.string "The tremor is fading.\p"
	.string "This chamber is quiet again.\n"
	.string "GROUDON is still loose outside.\p"
	.string "Return to CAPT. STERN at the\n"
	.string "harbor in northeast SLATEPORT.\l"
	.string "His submarine is AQUA's next target.$"

ScorchedSlab_HeatransRoom_Text_MagmaStoneSplit:
	.string "The MAGMA STONE cooled and\n"
	.string "split. Inside was a HEATRANITE.$"

ScorchedSlab_HeatransRoom_Text_MissingStone:
	.string "The chamber answers a MAGMA STONE.\n"
	.string "One remains in MAGMA HIDEOUT.\p"
	.string "From MAXIE's GROUDON room, take\n"
	.string "the nearby east exit. Follow two\l"
	.string "northward passages to the\l"
	.string "main entrance floor.\p"
	.string "Follow that floor north to its\n"
	.string "upper-right side passage. Enter\l"
	.string "the side chamber.\p"
	.string "The item ball along the north\n"
	.string "wall, west of the stairs, holds\l"
	.string "the MAGMA STONE. Return with it.$"
