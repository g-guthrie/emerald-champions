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
	.string "Every fossil here records a different\n"
	.string "answer to survival. Take them; build.\p"
	.string "Below us, cells of green light wait for\n"
	.string "the guardian of the land.$"
