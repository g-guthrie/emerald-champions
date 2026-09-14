RustboroCity_DevonCorp_3F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RustboroCity_DevonCorp_3F_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, RustboroCity_DevonCorp_3F_OnWarp
	map_script MAP_SCRIPT_ON_FRAME_TABLE, RustboroCity_DevonCorp_3F_OnFrame
	.byte 0

RustboroCity_DevonCorp_3F_OnTransition:
	call_if_eq VAR_DEVON_CORP_3F_STATE, 0, RustboroCity_DevonCorp_3F_EventScript_SetEmployeePos
	end

RustboroCity_DevonCorp_3F_EventScript_SetEmployeePos::
	setobjectxyperm LOCALID_DEVON_CORP_3F_EMPLOYEE, 3, 2
	setobjectmovementtype LOCALID_DEVON_CORP_3F_EMPLOYEE, MOVEMENT_TYPE_FACE_LEFT
	return

RustboroCity_DevonCorp_3F_OnWarp:
	map_script_2 VAR_DEVON_CORP_3F_STATE, 0, RustboroCity_DevonCorp_3F_EventScript_PlayerFaceEast
	.2byte 0

RustboroCity_DevonCorp_3F_EventScript_PlayerFaceEast::
	turnobject LOCALID_PLAYER, DIR_EAST
	end

RustboroCity_DevonCorp_3F_OnFrame:
	map_script_2 VAR_DEVON_CORP_3F_STATE, 0, RustboroCity_DevonCorp_3F_EventScript_MeetPresident
	.2byte 0

RustboroCity_DevonCorp_3F_EventScript_MeetPresident::
	lockall
	msgbox RustboroCity_DevonCorp_3F_Text_ThisIs3rdFloorWaitHere, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_DEVON_CORP_3F_EMPLOYEE, RustboroCity_DevonCorp_3F_Movement_EmployeeWalkOffscreen
	waitmovement 0
	delay 80
	applymovement LOCALID_DEVON_CORP_3F_EMPLOYEE, RustboroCity_DevonCorp_3F_Movement_EmployeeReturnToPlayer
	waitmovement 0
	msgbox RustboroCity_DevonCorp_3F_Text_WordWithPresidentComeWithMe, MSGBOX_DEFAULT
	closemessage
	playbgm MUS_FOLLOW_ME, FALSE
	applymovement LOCALID_DEVON_CORP_3F_EMPLOYEE, RustboroCity_DevonCorp_3F_Movement_LeadPlayerToPresident
	applymovement LOCALID_PLAYER, RustboroCity_DevonCorp_3F_Movement_PlayerFollowToPresident
	waitmovement 0
	msgbox RustboroCity_DevonCorp_3F_Text_PleaseGoAhead, MSGBOX_DEFAULT
	closemessage
	fadedefaultbgm
	applymovement LOCALID_DEVON_CORP_3F_EMPLOYEE, RustboroCity_DevonCorp_3F_Movement_EmployeeFaceDesk
	applymovement LOCALID_PLAYER, RustboroCity_DevonCorp_3F_Movement_PlayerApproachDesk
	waitmovement 0
	msgbox RustboroCity_DevonCorp_3F_Text_MrStoneIHaveFavor, MSGBOX_DEFAULT
	giveitem ITEM_LETTER
	goto_if_eq VAR_RESULT, FALSE, RustboroCity_DevonCorp_3F_EventScript_ArchiveLetter
	goto RustboroCity_DevonCorp_3F_EventScript_ReceivedLetter
	end

RustboroCity_DevonCorp_3F_EventScript_ArchiveLetter::
	addpcitem ITEM_LETTER
	goto_if_eq VAR_RESULT, FALSE, RustboroCity_DevonCorp_3F_EventScript_LetterNoRoom
	msgbox RustboroCity_DevonCorp_3F_Text_LetterSentToPC, MSGBOX_DEFAULT

RustboroCity_DevonCorp_3F_EventScript_ReceivedLetter::
	msgbox RustboroCity_DevonCorp_3F_Text_MrStoneWantYouToHaveThis, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message RustboroCity_DevonCorp_3F_Text_ReceivedPokenav
	waitfanfare
	completemilestone FLAG_SYS_POKENAV_GET
	setflag FLAG_RECEIVED_POKENAV
	setflag FLAG_HIDE_RUSTBORO_CITY_POKEMON_SCHOOL_SCOTT
	msgbox RustboroCity_DevonCorp_3F_Text_MrStoneExplainPokenavRestUp, MSGBOX_DEFAULT
	closemessage
	call Common_EventScript_OutOfCenterPartyHeal
	msgbox RustboroCity_DevonCorp_3F_Text_MrStoneGoWithCautionAndCare, MSGBOX_DEFAULT
	setflag FLAG_HIDE_ROUTE_116_WANDAS_BOYFRIEND
	clearflag FLAG_HIDE_RUSTURF_TUNNEL_WANDAS_BOYFRIEND
	clearflag FLAG_HIDE_RUSTURF_TUNNEL_WANDA
	clearflag FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY
	clearflag FLAG_HIDE_BRINEYS_HOUSE_PEEKO
	setvar VAR_BRINEY_LOCATION, 1
	clearflag FLAG_HIDE_RUSTBORO_CITY_RIVAL
	setvar VAR_DEVON_CORP_3F_STATE, 1
	setvar VAR_RUSTBORO_CITY_STATE, 6
	releaseall
	end

RustboroCity_DevonCorp_3F_EventScript_LetterNoRoom::
	msgbox RustboroCity_DevonCorp_3F_Text_LetterNoRoom, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_DevonCorp_3F_Movement_Unused:
	walk_up
	walk_up
	walk_up
	walk_in_place_faster_left
	step_end

RustboroCity_DevonCorp_3F_Movement_LeadPlayerToPresident:
	walk_right
	walk_right
	walk_right
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_left
	step_end

RustboroCity_DevonCorp_3F_Movement_EmployeeFaceDesk:
	delay_16
	walk_in_place_faster_down
	step_end

RustboroCity_DevonCorp_3F_Movement_EmployeeWalkOffscreen:
	walk_right
	walk_right
	walk_right
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

RustboroCity_DevonCorp_3F_Movement_EmployeeReturnToPlayer:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	walk_left
	walk_left
	walk_left
	step_end

RustboroCity_DevonCorp_3F_Movement_PlayerFollowToPresident:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

RustboroCity_DevonCorp_3F_Movement_PlayerApproachDesk:
	delay_16
	delay_16
	walk_down
	walk_down
	walk_in_place_faster_right
	step_end

RustboroCity_DevonCorp_3F_EventScript_MrStone::
	lock
	faceplayer
	goto_if_set FLAG_EC_STEVEN_RESEARCH_CONCLUSION, RustboroCity_DevonCorp_3F_EventScript_FinalConversation
	goto_if_set FLAG_RECEIVED_PIDGEOTITE_FROM_DEVON, RustboroCity_DevonCorp_3F_EventScript_MrStoneAfterFavor
	goto_if_unset FLAG_DELIVERED_STEVEN_LETTER, RustboroCity_DevonCorp_3F_EventScript_MrStoneFavorPending
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, RustboroCity_DevonCorp_3F_EventScript_MegaRingPending
	goto RustboroCity_DevonCorp_3F_EventScript_GivePidgeotite
	end

RustboroCity_DevonCorp_3F_EventScript_MrStoneFavorPending::
	msgbox RustboroCity_DevonCorp_3F_Text_CountingOnYou, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

RustboroCity_DevonCorp_3F_EventScript_MegaRingPending::
	msgbox RustboroCity_DevonCorp_3F_Text_StevenAwaitsKnuckleBadge, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

RustboroCity_DevonCorp_3F_EventScript_GivePidgeotite::
	msgbox RustboroCity_DevonCorp_3F_Text_ThankYouForDeliveringLetter, MSGBOX_DEFAULT
	giveuniqueitem ITEM_PIDGEOTITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_PIDGEOTITE_FROM_DEVON
	msgbox RustboroCity_DevonCorp_3F_Text_ExplainPidgeotite, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

RustboroCity_DevonCorp_3F_EventScript_MrStoneAfterFavor::
	msgbox RustboroCity_DevonCorp_3F_Text_NotFamiliarWithTrends, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

RustboroCity_DevonCorp_3F_EventScript_Employee::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_REPEAT_BALL, RustboroCity_DevonCorp_3F_EventScript_EmployeeBalls
	msgbox RustboroCity_DevonCorp_3F_Text_VisitCaptSternShipyard, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_3F_EventScript_EmployeeBalls::
	msgbox RustboroCity_DevonCorp_3F_Text_RepeatAndTimerHugelyPopular, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_3F_EventScript_RareRocksDisplay::
	msgbox RustboroCity_DevonCorp_3F_Text_RareRocksDisplay, MSGBOX_SIGN
	end

RustboroCity_DevonCorp_3F_Text_MrStoneIHaveFavor:
	.string "I'm MR. STONE, president of DEVON.\n"
	.string "You recovered our GOODS and\l"
	.string "protected my staff. Thank you!\p"
	.string "That equipment is for CAPT. STERN's\n"
	.string "marine surveys. My son STEVEN is\l"
	.string "studying the rocks on DEWFORD.\p"
	.string "Someone went to a great deal of\n"
	.string "trouble to keep those instruments\l"
	.string "from reaching STERN.\p"
	.string "MAGMA and AQUA are searching those\n"
	.string "same places. I fear the thefts\l"
	.string "are connected.\p"
	.string "Please take this LETTER to STEVEN\n"
	.string "in GRANITE CAVE, near DEWFORD.\p"
	.string "Then take the GOODS on to CAPT.\n"
	.string "STERN at SLATEPORT's SHIPYARD.\l"
	.string "One journey can help them both.$"

RustboroCity_DevonCorp_3F_Text_LetterSentToPC:
	.string "Your KEY ITEMS pocket is full, so the\n"
	.string "LETTER was sent to your PC.$"

RustboroCity_DevonCorp_3F_Text_LetterNoRoom:
	.string "Your KEY ITEMS pocket and PC are full.\n"
	.string "Make room, then return to my desk.$"

RustboroCity_DevonCorp_3F_Text_MrStoneWantYouToHaveThis:
	.string "DEVON will provide ¥4,000 for\n"
	.string "your journey. Please take this\l"
	.string "POKéNAV to help you find your way.$"

RustboroCity_DevonCorp_3F_Text_ReceivedPokenav:
	.string "{PLAYER} received a POKéNAV.$"

RustboroCity_DevonCorp_3F_Text_MrStoneExplainPokenavRestUp:
	.string "The POKéNAV maps HOENN and keeps\n"
	.string "you in touch with key Trainers.\p"
	.string "For legendary leads, visit the\n"
	.string "guide near the POKéMON CENTER PC.\p"
	.string "Each town's guide knows nearby\n"
	.string "Pokémon and their requirements.\p"
	.string "Please rest before you leave.\n"
	.string "DEWFORD is your next stop.$"

RustboroCity_DevonCorp_3F_Text_MrStoneGoWithCautionAndCare:
RustboroCity_DevonCorp_3F_Text_CountingOnYou:
	.string "Go south from RUSTBORO, through\n"
	.string "PETALBURG WOODS, to MR. BRINEY's\l"
	.string "cottage on ROUTE 104.\p"
	.string "Ask him to sail to DEWFORD.\n"
	.string "STEVEN is in GRANITE CAVE,\l"
	.string "northwest of town.\p"
	.string "After the LETTER, take STERN's\n"
	.string "GOODS onward to SLATEPORT.$"

RustboroCity_DevonCorp_3F_Text_StevenAwaitsKnuckleBadge:
	.string "STEVEN received my LETTER. Good.\p"
	.string "He will entrust the MEGA RING\n"
	.string "only after you earn DEWFORD's\l"
	.string "KNUCKLE BADGE.$"

RustboroCity_DevonCorp_3F_Text_ThankYouForDeliveringLetter:
	.string "You reached STEVEN and earned the\n"
	.string "right to use MEGA EVOLUTION.\l"
	.string "Excellent.\p"
	.string "This stone was found above\n"
	.string "RUSTBORO. Let PIDGEOT show you\l"
	.string "what it can become.$"

RustboroCity_DevonCorp_3F_Text_ExplainPidgeotite:
	.string "PIDGEOTITE lets a PIDGEOT Mega\n"
	.string "Evolve when it holds the stone and\l"
	.string "you possess a MEGA RING.\p"
	.string "Find Mega Stones in HOENN's caves,\n"
	.string "gyms and side paths, or through\l"
	.string "the people you meet.\p"
	.string "CENTER vendors supply held items\n"
	.string "for free. Evolution specialists\l"
	.string "sell stones and devices.$"
RustboroCity_DevonCorp_3F_Text_NotFamiliarWithTrends:
	.string "Some rare POKéMON become\n"
	.string "available as you earn BADGES or\l"
	.string "help HOENN.\p"
	.string "A few also require you to have\n"
	.string "caught another POKéMON. Our guide\l"
	.string "tells you exactly which one.\p"
	.string "You can explore and catch them\n"
	.string "without visiting our researcher.$"
RustboroCity_DevonCorp_3F_Text_ThisIs3rdFloorWaitHere:
	.string "This is DEVON's third floor.\n"
	.string "Our PRESIDENT's office is here.\p"
	.string "That parcel is survey equipment\n"
	.string "for CAPT. STERN in SLATEPORT.\l"
	.string "I'll tell MR. STONE it's safe.\p"
	.string "Please wait here a moment.$"

RustboroCity_DevonCorp_3F_Text_WordWithPresidentComeWithMe:
	.string "Our PRESIDENT would like to have\n"
	.string "a word with you.\p"
	.string "Please come with me.$"

RustboroCity_DevonCorp_3F_Text_PleaseGoAhead:
	.string "Please, go ahead.$"

RustboroCity_DevonCorp_3F_Text_VisitCaptSternShipyard:
	.string "If you visit the SHIPYARD in SLATEPORT,\n"
	.string "you should go see CAPT. STERN.$"

RustboroCity_DevonCorp_3F_Text_RepeatAndTimerHugelyPopular:
	.string "DEVON's new products, the REPEAT BALL\n"
	.string "and TIMER BALL, have become hugely\l"
	.string "popular among TRAINERS.$"

RustboroCity_DevonCorp_3F_Text_RareRocksDisplay:
	.string "It's a collection of rare rocks and\n"
	.string "stones assembled by the PRESIDENT.$"

RustboroCity_DevonCorp_3F_EventScript_ResearchConclusion::
	goto_if_set FLAG_EC_REPORT_C48_COMPLETE, Common_EventScript_NopReturn
	completemilestone FLAG_EC_REPORT_C48_COMPLETE
	msgbox RustboroCity_DevonCorp_3F_Text_ResearchConclusion, MSGBOX_DEFAULT
	return

RustboroCity_DevonCorp_3F_Text_ResearchConclusion:
	.string "I asked you to make two deliveries.\n"
	.string "You helped people all across HOENN.\p"
	.string "STEVEN's letter began a search.\n"
	.string "You followed it to the living\l"
	.string "places behind those old accounts.\p"
	.string "Thank you. This final ¥20,000\n"
	.string "research grant is yours.\l"
	.string "Your level cap is now 94.\p"
	.string "Now finish the journey you set\n"
	.string "out to make. SURF east from\l"
	.string "ROUTE 128 to EVER GRANDE.\p"
	.string "Climb the WATERFALL and follow\n"
	.string "VICTORY ROAD. The LEAGUE is waiting.$"

RustboroCity_DevonCorp_3F_EventScript_FinalConversation::
	goto_if_set FLAG_EC_REPORT_C48_COMPLETE, RustboroCity_DevonCorp_3F_EventScript_LeagueSendoff
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	waitmovement 0
	delay 24
	call RustboroCity_DevonCorp_3F_EventScript_ResearchConclusion
	release
	end
RustboroCity_DevonCorp_3F_EventScript_LeagueSendoff::
	msgbox RustboroCity_DevonCorp_3F_Text_LeagueSendoff, MSGBOX_DEFAULT
	call_if_unset FLAG_RECEIVED_PIDGEOTITE_FROM_DEVON, RustboroCity_DevonCorp_3F_EventScript_CheckLateGift
	release
	end

RustboroCity_DevonCorp_3F_Text_LeagueSendoff:
	.string "You brought our scattered\n"
	.string "accounts together. Thank you.\p"
	.string "The LEAGUE is waiting. From\n"
	.string "ROUTE 128, SURF east to EVER GRANDE,\l"
	.string "then climb the WATERFALL.\p"
	.string "VICTORY ROAD leads to your\n"
	.string "next challenge.$"

RustboroCity_DevonCorp_3F_EventScript_CheckLateGift::
	goto_if_unset FLAG_DELIVERED_STEVEN_LETTER, Common_EventScript_NopReturn
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_NopReturn
	giveuniqueitem ITEM_PIDGEOTITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_NopReturn
	setflag FLAG_RECEIVED_PIDGEOTITE_FROM_DEVON
	msgbox RustboroCity_DevonCorp_3F_Text_ExplainPidgeotite, MSGBOX_DEFAULT
	return
