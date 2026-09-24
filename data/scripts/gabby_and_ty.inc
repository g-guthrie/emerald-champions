@ Gabby and Ty are finite. They visit four stops, each once, with their
@ authored parties 1, 2, 5 and 6 (parties 3 and 4 are retired):
@   Route 111 (party 1) -> Route 118 (party 2) -> Route 118 (party 5)
@   -> Route 120 (party 6).
@ Once party 6 is beaten they stay on Route 120 as interviewers with no battle.
@ GabbyAndTyGetBattleNum (src/tv.c) returns 0, 1, 4, 5 for the four stops and
@ 6 for the retired pair. Every stop is hidden first, then the current one is
@ shown, so no pair from an earlier stop is ever left behind.
@ Note: The local IDs of Gabby and Ty are hard-coded in GetGabbyAndTyLocalIds
GabbyAndTy_EventScript_UpdateLocation::
	call GabbyAndTy_EventScript_HideAllStops
	specialvar VAR_RESULT, GabbyAndTyGetBattleNum
	switch VAR_RESULT
	case 0, GabbyAndTy_EventScript_ShowAtRoute111_1
	case 1, GabbyAndTy_EventScript_ShowAtRoute118_1
	case 4, GabbyAndTy_EventScript_ShowAtRoute118_2
	case 5, GabbyAndTy_EventScript_ShowAtRoute120_2
	case 6, GabbyAndTy_EventScript_StayAtRoute120
	return

GabbyAndTy_EventScript_HideAllStops::
	setflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_1
	setflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_2
	setflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_3
	setflag FLAG_HIDE_ROUTE_118_GABBY_AND_TY_1
	setflag FLAG_HIDE_ROUTE_118_GABBY_AND_TY_2
	setflag FLAG_HIDE_ROUTE_118_GABBY_AND_TY_3
	setflag FLAG_HIDE_ROUTE_120_GABBY_AND_TY_1
	setflag FLAG_HIDE_ROUTE_120_GABBY_AND_TY_2
	return

@ Party 6 is beaten (the counter reaches 6 only in its post-battle interview).
@ Keep its trainer flag set, including for saves from when it was cleared to
@ allow repeat battles.
GabbyAndTy_EventScript_StayAtRoute120::
	settrainerflag TRAINER_GABBY_AND_TY_6
	clearflag FLAG_HIDE_ROUTE_120_GABBY_AND_TY_2
	return

GabbyAndTy_EventScript_ShowAtRoute111_1::
	clearflag FLAG_HIDE_ROUTE_111_GABBY_AND_TY_1
	return

GabbyAndTy_EventScript_ShowAtRoute118_1::
	clearflag FLAG_HIDE_ROUTE_118_GABBY_AND_TY_1
	return

GabbyAndTy_EventScript_ShowAtRoute118_2::
	clearflag FLAG_HIDE_ROUTE_118_GABBY_AND_TY_2
	return

GabbyAndTy_EventScript_ShowAtRoute120_2::
	clearflag FLAG_HIDE_ROUTE_120_GABBY_AND_TY_2
	return

GabbyAndTy_EventScript_GabbyBattle1::
	trainerbattle_double TRAINER_GABBY_AND_TY_1, GabbyAndTy_Text_GabbyPreFirstBattle, GabbyAndTy_Text_GabbyDefeatFirstTime, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_FirstInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle1::
	trainerbattle_double TRAINER_GABBY_AND_TY_1, GabbyAndTy_Text_TyPreFirstBattle, GabbyAndTy_Text_TyDefeatFirstTime, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_FirstInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_GabbyBattle2::
	trainerbattle_double TRAINER_GABBY_AND_TY_2, GabbyAndTy_Text_GabbyIntro, GabbyAndTy_Text_GabbyDefeat, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle2::
	trainerbattle_double TRAINER_GABBY_AND_TY_2, GabbyAndTy_Text_TyIntro, GabbyAndTy_Text_TyDefeat, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_GabbyBattle5::
	trainerbattle_double TRAINER_GABBY_AND_TY_5, GabbyAndTy_Text_GabbyIntro, GabbyAndTy_Text_GabbyDefeat, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle5::
	trainerbattle_double TRAINER_GABBY_AND_TY_5, GabbyAndTy_Text_TyIntro, GabbyAndTy_Text_TyDefeat, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_GabbyBattle6::
	goto_if_defeated TRAINER_GABBY_AND_TY_6, GabbyAndTy_EventScript_RetiredInterview
	trainerbattle_double TRAINER_GABBY_AND_TY_6, GabbyAndTy_Text_GabbyIntro, GabbyAndTy_Text_GabbyDefeat, GabbyAndTy_Text_GabbyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_TyBattle6::
	goto_if_defeated TRAINER_GABBY_AND_TY_6, GabbyAndTy_EventScript_RetiredInterview
	trainerbattle_double TRAINER_GABBY_AND_TY_6, GabbyAndTy_Text_TyIntro, GabbyAndTy_Text_TyDefeat, GabbyAndTy_Text_TyNotEnoughMons, GabbyAndTy_EventScript_RequestInterview
	msgbox GabbyAndTy_Text_TyPostBattle, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_EventScript_FirstInterview::
	special GabbyAndTyBeforeInterview
	special GetGabbyAndTyLocalIds
	compare VAR_FACING, DIR_NORTH
	call_if_eq GabbyAndTy_EventScript_FacePlayerNorth
	compare VAR_FACING, DIR_SOUTH
	call_if_eq GabbyAndTy_EventScript_FacePlayerSouth
	compare VAR_FACING, DIR_EAST
	call_if_eq GabbyAndTy_EventScript_FacePlayerEast
	goto_if_set FLAG_TEMP_1, GabbyAndTy_EventScript_KeepingAnEyeOutForYou
	msgbox GabbyAndTy_Text_WhoAreYouInterview, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_FacePlayerNorth::
	applymovement VAR_0x8004, GabbyAndTy_Movement_WalkInPlaceDown
	waitmovement 0
	return

GabbyAndTy_EventScript_FacePlayerSouth::
	applymovement VAR_0x8004, GabbyAndTy_Movement_WalkInPlaceUp
	applymovement VAR_0x8005, Common_Movement_WalkInPlaceFastestUp
	waitmovement 0
	return

GabbyAndTy_EventScript_FacePlayerEast::
	applymovement VAR_0x8004, GabbyAndTy_Movement_WalkInPlaceLeft
	applymovement VAR_0x8005, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	return

GabbyAndTy_EventScript_RequestInterview::
	special GabbyAndTyBeforeInterview
	special GetGabbyAndTyLocalIds
	compare VAR_FACING, DIR_NORTH
	call_if_eq GabbyAndTy_EventScript_FacePlayerNorth
	compare VAR_FACING, DIR_SOUTH
	call_if_eq GabbyAndTy_EventScript_FacePlayerSouth
	compare VAR_FACING, DIR_EAST
	call_if_eq GabbyAndTy_EventScript_FacePlayerEast
	goto_if_set FLAG_TEMP_1, GabbyAndTy_EventScript_KeepingAnEyeOutForYou
	specialvar VAR_RESULT, GabbyAndTyGetLastQuote
	compare VAR_RESULT, 0
	goto_if_eq GabbyAndTy_EventScript_DidntInterviewLastTime
	msgbox GabbyAndTy_Text_QuoteFromLastInterview, MSGBOX_DEFAULT
	specialvar VAR_RESULT, GabbyAndTyGetLastBattleTrivia
	switch VAR_RESULT
	case 0, GabbyAndTy_EventScript_RequestInterviewNoTrivia
	case 1, GabbyAndTy_EventScript_RequestInterviewShortBattle
	case 2, GabbyAndTy_EventScript_RequestInterviewThrewBall
	case 3, GabbyAndTy_EventScript_RequestInterviewUsedItems
	case 4, GabbyAndTy_EventScript_RequestInterviewLostAMon
	end

GabbyAndTy_Movement_WalkInPlaceDown:
	walk_in_place_down
	step_end

GabbyAndTy_Movement_WalkInPlaceUp:
	walk_in_place_up
	step_end

GabbyAndTy_Movement_WalkInPlaceLeft:
	walk_in_place_left
	step_end

GabbyAndTy_EventScript_DidntInterviewLastTime::
	msgbox GabbyAndTy_Text_GiveUsAnInterviewThisTime, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewNoTrivia::
	msgbox GabbyAndTy_Text_InterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewShortBattle::
	msgbox GabbyAndTy_Text_YouStompedUsInterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewThrewBall::
	msgbox GabbyAndTy_Text_YouThrewABallAtUsInterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewUsedItems::
	msgbox GabbyAndTy_Text_CleverItemSkillsInterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RequestInterviewLostAMon::
	msgbox GabbyAndTy_Text_WeLookedRespectableInterviewAgain, MSGBOX_YESNO
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_Interview::
	compare VAR_RESULT, NO
	goto_if_eq GabbyAndTy_EventScript_DontGiveUpKeepingEyeOut
	msgbox GabbyAndTy_Text_DescribeYourFeelings, MSGBOX_DEFAULT
	setvar VAR_0x8004, EASY_CHAT_TYPE_GABBY_AND_TY
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	compare VAR_RESULT, 0
	goto_if_eq GabbyAndTy_EventScript_DontGiveUpKeepingEyeOut
	msgbox GabbyAndTy_Text_PerfectWellBeSeeingYou, MSGBOX_DEFAULT
	special GabbyAndTyAfterInterview
	setflag FLAG_TEMP_1
	release
	end

GabbyAndTy_EventScript_DontGiveUpKeepingEyeOut::
	msgbox GabbyAndTy_Text_DontGiveUpKeepingEyeOut, MSGBOX_DEFAULT
	setflag FLAG_TEMP_1
	release
	end

GabbyAndTy_EventScript_KeepingAnEyeOutForYou::
	msgbox GabbyAndTy_Text_KeepingAnEyeOutForYou, MSGBOX_DEFAULT
	release
	end

@ After their last party: no battle, one interview per visit. The show airs
@ the final battle's record with the new quote.
GabbyAndTy_EventScript_RetiredInterview::
	lock
	faceplayer
	special GetGabbyAndTyLocalIds
	compare VAR_FACING, DIR_NORTH
	call_if_eq GabbyAndTy_EventScript_FacePlayerNorth
	compare VAR_FACING, DIR_SOUTH
	call_if_eq GabbyAndTy_EventScript_FacePlayerSouth
	compare VAR_FACING, DIR_EAST
	call_if_eq GabbyAndTy_EventScript_FacePlayerEast
	goto_if_set FLAG_TEMP_1, GabbyAndTy_EventScript_RetiredKeepingAnEyeOut
	msgbox GabbyAndTy_Text_RetiredInterview, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, GabbyAndTy_EventScript_RetiredDeclined
	goto GabbyAndTy_EventScript_Interview
	end

GabbyAndTy_EventScript_RetiredDeclined::
	setflag FLAG_TEMP_1
GabbyAndTy_EventScript_RetiredKeepingAnEyeOut::
	msgbox GabbyAndTy_Text_RetiredKeepingAnEyeOut, MSGBOX_DEFAULT
	release
	end

GabbyAndTy_Text_RetiredInterview:
	.string "Gabby: No more battles for us--\n"
	.string "you're far too strong now!\p"
	.string "But our viewers still want to hear\n"
	.string "from you. Another interview?$"

GabbyAndTy_Text_RetiredKeepingAnEyeOut:
	.string "Gabby: We'll keep reporting on you.\n"
	.string "Come see us again!$"
