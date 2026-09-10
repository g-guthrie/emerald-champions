BattleFrontier_Lounge1_MapScripts::
	.byte 0

@ Emerald Champions treats every Pokemon as having perfect inherited stats.
@ This former IV rater explains the live EV preparation model instead
@ of judging stored IV values that do not affect battle stats.
BattleFrontier_Lounge1_EventScript_Breeder::
	lock
	faceplayer
	goto_if_set FLAG_EXPLAINED_FRONTIER_EVS, BattleFrontier_Lounge1_EventScript_EvReminder
	msgbox BattleFrontier_Lounge1_Text_EvExplanation, MSGBOX_DEFAULT
	setflag FLAG_EXPLAINED_FRONTIER_EVS
	release
	end

BattleFrontier_Lounge1_EventScript_EvReminder::
	msgbox BattleFrontier_Lounge1_Text_EvReminder, MSGBOX_DEFAULT
	release
	end

BattleFrontier_Lounge1_EventScript_Boy1::
	msgbox BattleFrontier_Lounge1_Text_SaidMyMonIsOutstanding, MSGBOX_NPC
	end

BattleFrontier_Lounge1_EventScript_Boy2::
	msgbox BattleFrontier_Lounge1_Text_DidntDoAnythingSpecialRaisingIt, MSGBOX_NPC
	end

BattleFrontier_Lounge1_Text_EvExplanation:
	.string "For 70 years I have raised\n"
	.string "POKéMON!\p"
	.string "Every partner has the potential to\n"
	.string "compete. The one you caught today\l"
	.string "deserves that chance, too.\p"
	.string "A CENTER tutor can help you choose\n"
	.string "its Nature and EVs, or prepare a\l"
	.string "full battle set.\p"
	.string "Then the two of you can get on\n"
	.string "with the adventure!$"

BattleFrontier_Lounge1_Text_EvReminder:
	.string "Every POKéMON has flawless\n"
	.string "potential.\p"
	.string "Use a CENTER tutor to choose its\n"
	.string "Nature, moves, Ability, item, and\l"
	.string "EVs.$"

BattleFrontier_Lounge1_Text_SaidMyMonIsOutstanding:
	.string "My POKéMON was ready the moment I\n"
	.string "caught it!\p"
	.string "Now I can focus on how it fits my team.$"

BattleFrontier_Lounge1_Text_DidntDoAnythingSpecialRaisingIt:
	.string "I changed my POKéMON's set at a\n"
	.string "CENTER.\p"
	.string "No breeding for numbers, no grinding--\n"
	.string "just a different battle plan.$"
