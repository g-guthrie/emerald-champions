BattleFrontier_Lounge1_MapScripts::
	.byte 0

@ Emerald Champions treats every Pokemon as having perfect inherited stats.
@ This former IV rater explains the live Stat Point preparation model instead
@ of judging stored IV values that do not affect battle stats.
BattleFrontier_Lounge1_EventScript_Breeder::
	lock
	faceplayer
	goto_if_set FLAG_EXPLAINED_FRONTIER_STAT_POINTS, BattleFrontier_Lounge1_EventScript_StatPointReminder
	msgbox BattleFrontier_Lounge1_Text_StatPointExplanation, MSGBOX_DEFAULT
	setflag FLAG_EXPLAINED_FRONTIER_STAT_POINTS
	release
	end

BattleFrontier_Lounge1_EventScript_StatPointReminder::
	msgbox BattleFrontier_Lounge1_Text_StatPointReminder, MSGBOX_DEFAULT
	release
	end

BattleFrontier_Lounge1_EventScript_Boy1::
	msgbox BattleFrontier_Lounge1_Text_SaidMyMonIsOutstanding, MSGBOX_NPC
	end

BattleFrontier_Lounge1_EventScript_Boy2::
	msgbox BattleFrontier_Lounge1_Text_DidntDoAnythingSpecialRaisingIt, MSGBOX_NPC
	end

BattleFrontier_Lounge1_Text_StatPointExplanation:
	.string "For 70 years I have raised POKéMON!\p"
	.string "In EMERALD CHAMPIONS, every partner is\n"
	.string "treated as having flawless potential.\p"
	.string "Hidden inherited numbers never decide\n"
	.string "whether a caught POKéMON can compete.\p"
	.string "What matters is its Nature and where its\n"
	.string "STAT POINTS are invested.\p"
	.string "A CENTER tutor can apply a competitive\n"
	.string "set and its investment instantly.$"

BattleFrontier_Lounge1_Text_StatPointReminder:
	.string "Every POKéMON has flawless potential.\p"
	.string "Use a CENTER tutor to choose its Nature,\n"
	.string "moves, Ability, item, and STAT POINTS.$"

BattleFrontier_Lounge1_Text_SaidMyMonIsOutstanding:
	.string "My POKéMON was ready the moment I\n"
	.string "caught it!\p"
	.string "Now I can focus on how it fits my team.$"

BattleFrontier_Lounge1_Text_DidntDoAnythingSpecialRaisingIt:
	.string "I changed my POKéMON's set at a\n"
	.string "CENTER.\p"
	.string "No breeding for numbers, no grinding--\n"
	.string "just a different battle plan.$"
