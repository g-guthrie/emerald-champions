DewfordManor_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordManor_1F_OnTransition
	.byte 0

DewfordManor_1F_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_DEWFORD_MANOR
	end

DewfordManor_1F_EventScript_Historian::
	lockall
	faceplayer
	setvar VAR_0x8004, ITEM_MEGA_RING
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, FALSE, DewfordMeadow_EventScript_NeedRing
	goto_if_set FLAG_EC_MANOR_NOTES_READ, DewfordManor_1F_EventScript_RepeatDirection
	msgbox DewfordManor_1F_Text_Historian, MSGBOX_DEFAULT
	setflag FLAG_EC_MANOR_NOTES_READ
DewfordManor_1F_EventScript_RepeatDirection:
	goto_if_unset FLAG_EC_MEADOW_SONG_PERFORMED, DewfordManor_1F_EventScript_FindWarden
	call DewfordManor_1F_EventScript_RecordResearch
	msgbox DewfordMeadow_Text_ChapterComplete, MSGBOX_DEFAULT
	goto_if_unset FLAG_EC_MEGA_GIFT_CHANDELURITE, DewfordManor_1F_EventScript_MegaGift_CHANDELURITE
	goto DewfordManor_1F_EventScript_MegaGift_BANETTITE
	end
DewfordManor_1F_EventScript_FindWarden:
	msgbox DewfordManor_1F_Text_FindWarden, MSGBOX_DEFAULT
	releaseall
	end
DewfordManor_1F_Text_FindWarden:
	.string "Take a partner that knows SING to\n"
	.string "the meadow warden southeast of here.\p"
	.string "Need a singer? The JIGGLYPUFF in\n"
	.string "these rooms already know SING.\p"
	.string "Any partner that knows the move\n"
	.string "will do. The song is what matters.$"
DewfordManor_1F_Text_Historian:
	.string "This manor housed Hoenn's first study\n"
	.string "of MEGA EVOLUTION.\p"
	.string "The researchers found that a stone is\n"
	.string "only a key. Trust supplies the power.\p"
	.string "Their final entry mentions a crystal\n"
	.string "chamber beneath SOOTOPOLIS.$"

DewfordManor_1F_EventScript_RecordResearch::
	goto_if_set FLAG_EC_REPORT_C14_COMPLETE, DewfordManor_1F_EventScript_ResearchRecorded
	completemilestone FLAG_EC_REPORT_C14_COMPLETE
	buffermilestonetext FLAG_EC_REPORT_C14_COMPLETE
	call_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_PrintMilestoneCapLine
	call_if_eq VAR_RESULT, 3, EmeraldChampions_EventScript_PrintMilestoneCapLine
DewfordManor_1F_EventScript_ResearchRecorded:
	return

DewfordManor_1F_EventScript_MegaGift_CHANDELURITE::
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, DewfordManor_1F_EventScript_EndVisit
	msgbox DewfordManor_1F_Text_MegaGift_CHANDELURITE, MSGBOX_DEFAULT
	giveuniqueitem ITEM_CHANDELURITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_CHANDELURITE
	releaseall
	end

DewfordManor_1F_EventScript_MegaGift_BANETTITE::
	goto_if_set FLAG_EC_MEGA_GIFT_BANETTITE, DewfordManor_1F_EventScript_EndVisit
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, DewfordManor_1F_EventScript_EndVisit
	msgbox DewfordManor_1F_Text_MegaGift_BANETTITE, MSGBOX_DEFAULT
	giveuniqueitem ITEM_BANETTITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_BANETTITE
	releaseall
	end

DewfordManor_1F_EventScript_EndVisit::
	releaseall
	end

DewfordManor_1F_Text_MegaGift_CHANDELURITE:
	.string "Oh, before you go…\n"
	.string "This hung in the chandelier once.\p"
	.string "It caught the candlelight strangely,\n"
	.string "long before we knew what it was.\p"
	.string "Take it. It belongs with someone\n"
	.string "who still lights a room.$"

DewfordManor_1F_Text_MegaGift_BANETTITE:
	.string "One more thing, while you're here.\p"
	.string "We found this stitched inside an\n"
	.string "old doll in the nursery.\p"
	.string "Whoever hid it there wanted it kept,\n"
	.string "not found. Now it's yours.$"
