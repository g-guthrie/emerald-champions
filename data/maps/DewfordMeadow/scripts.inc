DewfordMeadow_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordMeadow_OnTransition
	.byte 0

DewfordMeadow_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_DEWFORD_MEADOW
	end

DewfordMeadow_EventScript_Warden::
	msgbox DewfordMeadow_Text_Warden, MSGBOX_NPC
	end
DewfordMeadow_Text_Warden:
	.string "STEVEN reopened the old manor when the\n"
	.string "CHAMPION'S SIGNS began shining.\p"
	.string "He says the stones, the legends, and\n"
	.string "the shaking at MT. CHIMNEY are linked.$"

DewfordMeadow_EventScript_ManorSign::
	msgbox DewfordMeadow_Text_ManorSign, MSGBOX_SIGN
	end

DewfordMeadow_Text_ManorSign:
	.string "DEWFORD MANOR: Once a sea captain's\n"
	.string "escape, now home only to POKéMON.$"
