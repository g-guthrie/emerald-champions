AlteringCave_1F_MapScripts::
	.byte 0

AlteringCave_1F_EventScript_Researcher::
	msgbox AlteringCave_1F_Text_Researcher, MSGBOX_NPC
	releaseall
	end

AlteringCave_1F_Text_Researcher:
	.string "This cavern feels like a doorway.\n"
	.string "The deeper I go, the stranger it\l"
	.string "becomes.\p"
	.string "HOOPA lives among the wild\n"
	.string "POKéMON here. Each encounter\l"
	.string "has a 3% chance of being HOOPA.\p"
	.string "MEWTWO waits on B1F. It accepts\n"
	.string "challengers with the MIND BADGE.$"
