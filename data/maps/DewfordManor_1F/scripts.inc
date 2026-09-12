DewfordManor_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordManor_1F_OnTransition
	.byte 0

DewfordManor_1F_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_DEWFORD_MANOR
	end

DewfordManor_1F_EventScript_Historian::
	setflag FLAG_EC_MANOR_NOTES_READ
	call_if_set FLAG_EC_MEADOW_SONG_PERFORMED, DewfordManor_1F_EventScript_RecordResearch
	msgbox DewfordManor_1F_Text_Historian, MSGBOX_NPC
	end
DewfordManor_1F_Text_Historian:
	.string "This manor housed Hoenn's first study\n"
	.string "of MEGA EVOLUTION.\p"
	.string "The researchers found that a stone is\n"
	.string "only a key. Trust supplies the power.\p"
	.string "Their final entry mentions a crystal\n"
	.string "chamber beneath SOOTOPOLIS.$"

DewfordManor_1F_EventScript_RecordResearch::
	completemilestone FLAG_EC_REPORT_C14_COMPLETE
	return
