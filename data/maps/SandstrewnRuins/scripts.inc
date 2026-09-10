SandstrewnRuins_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SandstrewnRuins_OnTransition
	.byte 0

SandstrewnRuins_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_SANDSTREWN_RUINS
	end

SandstrewnRuins_EventScript_Archaeologist::
	msgbox SandstrewnRuins_Text_Archaeologist, MSGBOX_NPC
	end
SandstrewnRuins_Text_Archaeologist:
	.string "Every fossil here tells us how a\n"
	.string "POKéMON lived. Imagine this place\l"
	.string "before the desert!\p"
	.string "Those green lights below us may\n"
	.string "be ZYGARDE. Search B1F. It has\l"
	.string "a 3% wild encounter chance.$"
