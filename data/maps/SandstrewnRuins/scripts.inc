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
	buffermilestonetext FLAG_EC_REPORT_C26_COMPLETE
	msgbox SandstrewnRuins_Text_Report, MSGBOX_DEFAULT
	call_if_eq VAR_RESULT, 3, SandstrewnRuins_EventScript_GrantAndCapLine
	call_if_eq VAR_RESULT, 2, SandstrewnRuins_EventScript_GrantLine
	call_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_PrintMilestoneCapLine
SandstrewnRuins_EventScript_ResearchDone::
	msgbox SandstrewnRuins_Text_Archaeologist, MSGBOX_DEFAULT
	release
	end

SandstrewnRuins_EventScript_ResearchPending::
	msgbox SandstrewnRuins_Text_ResearchPending, MSGBOX_DEFAULT
	release
	end

SandstrewnRuins_EventScript_GrantAndCapLine::
	msgbox SandstrewnRuins_Text_GrantAndCapLine, MSGBOX_DEFAULT
	return

SandstrewnRuins_EventScript_GrantLine::
	msgbox SandstrewnRuins_Text_GrantLine, MSGBOX_DEFAULT
	return

SandstrewnRuins_Text_Report:
	.string "The mountain shook, but this old\n"
	.string "stonework held. The builders knew\l"
	.string "how to protect their world.\p"
	.string "I've recorded your findings.$"

SandstrewnRuins_Text_GrantAndCapLine:
	.string "The research grant is ¥{STR_VAR_2}.\l"
	.string "Your level cap is now {STR_VAR_1}.$"

SandstrewnRuins_Text_GrantLine:
	.string "The research grant is ¥{STR_VAR_2}.$"

SandstrewnRuins_Text_ResearchPending:
	.string "After earning FLANNERY's BADGE,\n"
	.string "help the nurse on ROUTE 111\l"
	.string "and investigate EMBER PATH.\p"
	.string "Explore the lowest chamber, then\n"
	.string "return to me at this entrance. No\l"
	.string "rare capture is needed.$"

SandstrewnRuins_Text_Archaeologist:
	.string "Fossils tell us how POKéMON lived\n"
	.string "before the desert. DEVON can help\l"
	.string "bring those discoveries to life.\p"
	.string "NORMAN in PETALBURG GYM is ready.\n"
	.string "His badge will let you use SURF.\p"
	.string "You can return with SURF for the\n"
	.string "remaining finds. Leave the summit\l"
	.string "fossils alone until you are done.$"

SandstrewnRuins_Text_LastChance:
	.string "Taking a summit fossil collapses\n"
	.string "the tower and seals these ruins\l"
	.string "FOREVER, including this entrance.\p"
	.string "The fossils, the ODD KEYSTONE and\n"
	.string "the PROTECTOR down here would be\l"
	.string "lost for good.\p"
	.string "The rest of these ruins' POKéMON\n"
	.string "also roam the open desert and the\l"
	.string "DESERT UNDERPASS, so take your time.$"
