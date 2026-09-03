DewfordManor_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordManor_1F_OnTransition
	.byte 0

DewfordManor_1F_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_DEWFORD_MANOR
	end

DewfordManor_1F_EventScript_Historian::
	msgbox DewfordManor_1F_Text_Historian, MSGBOX_NPC
	end
DewfordManor_1F_Text_Historian:
	.string "This manor housed Hoenn's first study\n"
	.string "of MEGA EVOLUTION.\p"
	.string "The researchers found that a stone is\n"
	.string "only a key. Trust supplies the power.\p"
	.string "Their final entry mentions a crystal\n"
	.string "chamber beneath SOOTOPOLIS.$"
