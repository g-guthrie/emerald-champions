MeteorFalls_JirachisRoom_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MeteorFalls_JirachisRoom_OnTransition
	.byte 0

MeteorFalls_JirachisRoom_OnTransition:
	setvar VAR_TEMP_1, 0
	end

MeteorFalls_JirachisRoom_EventScript_Jirachi::
	lock
	faceplayer
	call_if_unset FLAG_EC_SURVEYED_METEOR_CHAMBER, MeteorFalls_JirachisRoom_EventScript_Observe
	msgbox MeteorFalls_JirachisRoom_Text_BattleOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MeteorFalls_JirachisRoom_EventScript_Handoff
	closemessage
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, SPECIES_JIRACHI
	setvar VAR_0x8005, 2
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, MeteorFalls_JirachisRoom_EventScript_JirachiResting
	goto MeteorFalls_JirachisRoom_EventScript_JirachiCaught
	end

MeteorFalls_JirachisRoom_EventScript_JirachiCaught::
	setflag FLAG_EC_CAUGHT_JIRACHI
	removeobject LOCALID_EC_JIRACHI
	goto MeteorFalls_JirachisRoom_EventScript_Handoff
	end

MeteorFalls_JirachisRoom_EventScript_JirachiResting::
	special HideRestingLegendaryObject
	msgbox gText_LegendaryResting, MSGBOX_DEFAULT
	goto MeteorFalls_JirachisRoom_EventScript_Handoff
	end

MeteorFalls_JirachisRoom_EventScript_Entrance::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_unset FLAG_EC_CAUGHT_JIRACHI, MeteorFalls_JirachisRoom_EventScript_End
	goto_if_set FLAG_EC_SURVEYED_METEOR_CHAMBER, MeteorFalls_JirachisRoom_EventScript_Handoff
	goto_if_unset FLAG_BADGE08_GET, MeteorFalls_JirachisRoom_EventScript_Handoff
	msgbox MeteorFalls_JirachisRoom_Text_ObserveEmptyChamber, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MeteorFalls_JirachisRoom_EventScript_End
	closemessage
	applymovement LOCALID_PLAYER, Common_Movement_FaceUp
	waitmovement 0
	call MeteorFalls_JirachisRoom_EventScript_Observe
	goto MeteorFalls_JirachisRoom_EventScript_Handoff
	end

MeteorFalls_JirachisRoom_EventScript_Observe::
	goto_if_unset FLAG_BADGE08_GET, Common_EventScript_NopReturn
	msgbox MeteorFalls_JirachisRoom_Text_Awakening, MSGBOX_DEFAULT
	closemessage
	call_if_unset FLAG_EC_CAUGHT_JIRACHI, MeteorFalls_JirachisRoom_EventScript_StartAwakening
	playmoncry SPECIES_JIRACHI, CRY_MODE_NORMAL
	delay 48
	waitmoncry
	call_if_unset FLAG_EC_CAUGHT_JIRACHI, MeteorFalls_JirachisRoom_EventScript_EndAwakening
	msgbox MeteorFalls_JirachisRoom_Text_Observation, MSGBOX_DEFAULT
	setflag FLAG_EC_SURVEYED_METEOR_CHAMBER
	return

MeteorFalls_JirachisRoom_EventScript_StartAwakening::
	applymovement LOCALID_EC_JIRACHI, MeteorFalls_JirachisRoom_Movement_Awaken
	waitmovement 0
	return

MeteorFalls_JirachisRoom_EventScript_EndAwakening::
	applymovement LOCALID_EC_JIRACHI, MeteorFalls_JirachisRoom_Movement_Settle
	waitmovement 0
	return

MeteorFalls_JirachisRoom_Movement_Awaken:
	levitate
	step_end

MeteorFalls_JirachisRoom_Movement_Settle:
	destroy_extra_task
	step_end

MeteorFalls_JirachisRoom_EventScript_Handoff::
	goto_if_unset FLAG_BADGE08_GET, MeteorFalls_JirachisRoom_EventScript_BadgePending
	goto_if_set FLAG_EC_SURVEYED_ORIGIN_CHAMBER, MeteorFalls_JirachisRoom_EventScript_StevenHandoff
	msgbox MeteorFalls_JirachisRoom_Text_OriginHandoff, MSGBOX_DEFAULT
	goto MeteorFalls_JirachisRoom_EventScript_End
	end

MeteorFalls_JirachisRoom_EventScript_StevenHandoff::
	goto CaveOfOrigin_DianciesRoom_EventScript_StevenHandoff
	end

MeteorFalls_JirachisRoom_EventScript_BadgePending::
	msgbox CaveOfOrigin_DianciesRoom_Text_BadgePending, MSGBOX_DEFAULT
MeteorFalls_JirachisRoom_EventScript_End::
	releaseall
	end

MeteorFalls_JirachisRoom_Text_Awakening:
	.string "A quiet note stirs the chamber.\n"
	.string "It echoes like a distant song\l"
	.string "beneath the falling water.$"

MeteorFalls_JirachisRoom_Text_Observation:
	.string "JIRACHI's awakening matches the\n"
	.string "travelers' final account. Their\l"
	.string "story led to a living habitat.\p"
	.string "You study the chamber and the\n"
	.string "history the travelers left behind.$"

MeteorFalls_JirachisRoom_Text_BattleOffer:
	.string "Would you like to battle JIRACHI?\n"
	.string "This battle and capture are optional.$"

MeteorFalls_JirachisRoom_Text_ObserveEmptyChamber:
	.string "JIRACHI is no longer here, but\n"
	.string "a quiet echo lingers in the chamber.\p"
	.string "Study this place and the\n"
	.string "travelers' account now?$"

MeteorFalls_JirachisRoom_Text_OriginHandoff:
	.string "Next, enter the CAVE OF ORIGIN\n"
	.string "in northern SOOTOPOLIS. Follow\l"
	.string "its lower passage.\p"
	.string "In the crystal chamber, interact\n"
	.string "with DIANCIE to study the light.\l"
	.string "Its battle is optional.$"
