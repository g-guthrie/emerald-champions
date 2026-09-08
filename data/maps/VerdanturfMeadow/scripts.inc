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
	.string "DEERLING graze among these flowers.\n"
	.string "They evolve into SAWSBUCK at Lv. 34.\p"
	.string "A SUN STONE used on PETILIL in this\n"
	.string "meadow yields Hisuian LILLIGANT.\l"
	.string "Use it elsewhere for the usual form.$"
