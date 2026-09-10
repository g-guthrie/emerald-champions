AlteringCave_1F_MapScripts::
	.byte 0

AlteringCave_1F_EventScript_Researcher::
	msgbox AlteringCave_1F_Text_Researcher, MSGBOX_NPC
	goto Common_EventScript_LocalLegendaryResearch

AlteringCave_1F_Text_Researcher:
	.string "This cavern feels like a doorway.\n"
	.string "The deeper I go, the stranger it\l"
	.string "becomes.\p"
	.string "With the BALANCE BADGE, HOOPA can\n"
	.string "appear as a 1% wild encounter\l"
	.string "here. SWEET SCENT raises it to\l"
	.string "25%.\p"
	.string "MEWTWO waits separately on B1F.\n"
	.string "It will challenge you with the\l"
	.string "MIND BADGE.$"
