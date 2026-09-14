SandstrewnRuins_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SandstrewnRuins_OnTransition
	.byte 0

SandstrewnRuins_OnTransition:
	setflag FLAG_LANDMARK_SANDSTREWN_RUINS
	end

SandstrewnRuins_EventScript_Archaeologist::
	lock
	faceplayer
	msgbox SandstrewnRuins_Text_LastChance, MSGBOX_DEFAULT
	goto_if_set FLAG_EC_REPORT_C26_COMPLETE, SandstrewnRuins_EventScript_ResearchDone
	goto_if_unset FLAG_BADGE04_GET, SandstrewnRuins_EventScript_ResearchPending
	goto_if_lt VAR_CHANSEY_NURSE_STATE, 7, SandstrewnRuins_EventScript_ResearchPending
	goto_if_unset FLAG_EC_RESOLVED_MOLTRES, SandstrewnRuins_EventScript_ResearchPending
	goto_if_unset FLAG_EC_SURVEYED_DESERT_DEPTHS, SandstrewnRuins_EventScript_ResearchPending
	completemilestone FLAG_EC_REPORT_C26_COMPLETE
	msgbox SandstrewnRuins_Text_Report, MSGBOX_DEFAULT
SandstrewnRuins_EventScript_ResearchDone::
	msgbox SandstrewnRuins_Text_Archaeologist, MSGBOX_DEFAULT
	release
	end

SandstrewnRuins_EventScript_ResearchPending::
	msgbox SandstrewnRuins_Text_ResearchPending, MSGBOX_DEFAULT
	release
	end

SandstrewnRuins_Text_Report:
	.string "The mountain shook, but this old\n"
	.string "stonework held. The builders knew\l"
	.string "how to protect their world.\p"
	.string "I've recorded your findings.\n"
	.string "The research grant is ¥8,000.\l"
	.string "Your level cap is now 48.$"

SandstrewnRuins_Text_ResearchPending:
	.string "After earning FLANNERY's BADGE,\n"
	.string "help the nurse in ASHEN WOODS\l"
	.string "and investigate EMBER PATH.\p"
	.string "Then explore the ruins below us.\n"
	.string "Bring back what you learn; no\l"
	.string "rare capture is needed.$"

SandstrewnRuins_Text_Archaeologist:
	.string "Fossils tell us how POKéMON lived\n"
	.string "before the desert. DEVON can help\l"
	.string "bring those discoveries to life.\p"
	.string "The green lights below may be\n"
	.string "ZYGARDE. You can keep exploring\l"
	.string "without holding up our research.$"

SandstrewnRuins_Text_LastChance:
	.string "ZYGARDE and STAKATAKA live in\n"
	.string "the lowest chamber below us.\p"
	.string "Taking a summit fossil collapses\n"
	.string "the tower and seals these ruins\l"
	.string "FOREVER, including this entrance.\p"
	.string "This is your only chance at those\n"
	.string "two. Catch them before a fossil\l"
	.string "if you want them on your team.$"
