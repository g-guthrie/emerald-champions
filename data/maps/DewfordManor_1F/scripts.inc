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
	releaseall
	end
DewfordManor_1F_EventScript_FindWarden:
	msgbox DewfordManor_1F_Text_FindWarden, MSGBOX_DEFAULT
	releaseall
	end
DewfordManor_1F_Text_FindWarden:
	.string "Speak to the meadow warden southeast\n"
	.string "of this manor. Bring a singer.\p"
	.string "JIGGLYPUFF shelter in these rooms.\n"
	.string "The Center can teach them SING free.$"
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
DewfordManor_1F_EventScript_ResearchRecorded:
	return
