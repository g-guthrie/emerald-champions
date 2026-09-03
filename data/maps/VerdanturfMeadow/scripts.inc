VerdanturfMeadow_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, VerdanturfMeadow_OnTransition
	.byte 0

VerdanturfMeadow_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_VERDANTURF_MEADOW
	end

VerdanturfMeadow_EventScript_Warden::
	msgbox VerdanturfMeadow_Text_Warden, MSGBOX_NPC
	end
VerdanturfMeadow_Text_Warden:
	.string "This meadow heals after every storm.\p"
	.string "Bring ROSERADE once you master the heat.\n"
	.string "A tiny guardian may reveal itself.$"
