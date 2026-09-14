CaveOfOrigin_DianciesRoom_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CaveOfOrigin_DianciesRoom_OnTransition
	.byte 0

CaveOfOrigin_DianciesRoom_OnTransition:
	setvar VAR_TEMP_1, 0
	end

CaveOfOrigin_DianciesRoom_EventScript_WallaceExhibition::
	lock
	faceplayer
	goto_if_unset FLAG_SYS_GAME_CLEAR, CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady
	goto_if_defeated TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CaveOfOrigin_DianciesRoom_EventScript_WallaceDecline
	call EventScript_CheckStoryDoubleBattleParty
	trainerbattle_double TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_Text_WallaceIntro, CaveOfOrigin_DianciesRoom_Text_WallaceDefeat, EmeraldChampions_Text_NeedTwoPokemon
	goto CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter

CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceAfter, MSGBOX_DEFAULT
	release
	end

CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceNotReady, MSGBOX_DEFAULT
	release
	end

CaveOfOrigin_DianciesRoom_Text_WallaceIntro:
	.string "WALLACE: Listen to the water in\n"
	.string "this chamber. Even a quiet place\l"
	.string "has its own rhythm.\p"
	.string "We've met at the LEAGUE. Here, I'd\n"
	.string "like to show you another side of\l"
	.string "my team!$"

CaveOfOrigin_DianciesRoom_Text_WallaceDefeat:
	.string "A magnificent performance!$"

CaveOfOrigin_DianciesRoom_Text_WallaceAfter:
	.string "WALLACE: What a pleasure to battle\n"
	.string "you here! The crystals seemed to\l"
	.string "catch every flash of light.\p"
	.string "You needn't win an exhibition to\n"
	.string "meet DIANCIE or study this place.\l"
	.string "Take your time exploring!$"
CaveOfOrigin_DianciesRoom_Text_WallaceNotReady:
	.string "WALLACE: Challenge me at the\n"
	.string "POKéMON LEAGUE first. Then we can\l"
	.string "hold a special exhibition here.\p"
	.string "You're welcome to explore the\n"
	.string "chamber in the meantime.$"

CaveOfOrigin_DianciesRoom_EventScript_Diancie::
	call EC_RequireFinalBadge
	lock
	faceplayer
	call_if_unset FLAG_EC_SURVEYED_ORIGIN_CHAMBER, CaveOfOrigin_DianciesRoom_EventScript_Observe
	goto_if_set FLAG_EC_RESOLVED_DIANCIE, CaveOfOrigin_DianciesRoom_EventScript_OptionalBattle
	msgbox CaveOfOrigin_DianciesRoom_Text_BattleOffer, MSGBOX_YESNO
	goto CaveOfOrigin_DianciesRoom_EventScript_BattleChoice
CaveOfOrigin_DianciesRoom_EventScript_OptionalBattle::
	msgbox EmeraldChampions_Text_ResearchChallengeComplete, MSGBOX_YESNO
CaveOfOrigin_DianciesRoom_EventScript_BattleChoice::
	goto_if_eq VAR_RESULT, NO, CaveOfOrigin_DianciesRoom_EventScript_Handoff
	closemessage
	waitse
	playmoncry SPECIES_DIANCIE, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, SPECIES_DIANCIE
	setvar VAR_0x8005, 2
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	call_if_eq VAR_RESULT, B_OUTCOME_WON, CaveOfOrigin_DianciesRoom_EventScript_Resolved
	call_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, CaveOfOrigin_DianciesRoom_EventScript_Resolved
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, CaveOfOrigin_DianciesRoom_EventScript_DiancieResting
	goto CaveOfOrigin_DianciesRoom_EventScript_DiancieCaught
	end

CaveOfOrigin_DianciesRoom_EventScript_DiancieCaught::
	setflag FLAG_EC_CAUGHT_DIANCIE
	removeobject LOCALID_EC_DIANCIE
	goto CaveOfOrigin_DianciesRoom_EventScript_Handoff
	end

CaveOfOrigin_DianciesRoom_EventScript_DiancieResting::
	special HideRestingLegendaryObject
	call Common_EventScript_ExplainLegendaryResting
	goto CaveOfOrigin_DianciesRoom_EventScript_Handoff
	end

@ The entrance only offers a deliberate observation when the actor is gone.
@ Declining consumes this visit's prompt; leaving and returning offers it again.
CaveOfOrigin_DianciesRoom_EventScript_Entrance::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_unset FLAG_EC_CAUGHT_DIANCIE, CaveOfOrigin_DianciesRoom_EventScript_End
	goto_if_set FLAG_EC_SURVEYED_ORIGIN_CHAMBER, CaveOfOrigin_DianciesRoom_EventScript_Handoff
	goto_if_unset FLAG_BADGE08_GET, CaveOfOrigin_DianciesRoom_EventScript_Handoff
	msgbox CaveOfOrigin_DianciesRoom_Text_ObserveEmptyChamber, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CaveOfOrigin_DianciesRoom_EventScript_End
	closemessage
	applymovement LOCALID_PLAYER, Common_Movement_FaceDown
	waitmovement 0
	call CaveOfOrigin_DianciesRoom_EventScript_Observe
	goto CaveOfOrigin_DianciesRoom_EventScript_Handoff
	end

CaveOfOrigin_DianciesRoom_EventScript_Observe::
	goto_if_unset FLAG_BADGE08_GET, Common_EventScript_NopReturn
	msgbox CaveOfOrigin_DianciesRoom_Text_CrystalLight, MSGBOX_DEFAULT
	closemessage
	playmoncry SPECIES_DIANCIE, CRY_MODE_NORMAL
	delay 24
	waitmoncry
	msgbox CaveOfOrigin_DianciesRoom_Text_Observation, MSGBOX_DEFAULT
	setflag FLAG_EC_SURVEYED_ORIGIN_CHAMBER
	return

CaveOfOrigin_DianciesRoom_EventScript_Handoff::
	goto_if_unset FLAG_EC_RESOLVED_DIANCIE, CaveOfOrigin_DianciesRoom_EventScript_BattlePending
	goto_if_unset FLAG_BADGE08_GET, CaveOfOrigin_DianciesRoom_EventScript_BadgePending
	goto_if_set FLAG_EC_RESOLVED_JIRACHI, CaveOfOrigin_DianciesRoom_EventScript_StevenHandoff
	msgbox CaveOfOrigin_DianciesRoom_Text_MeteorHandoff, MSGBOX_DEFAULT
	goto CaveOfOrigin_DianciesRoom_EventScript_End
	end

CaveOfOrigin_DianciesRoom_EventScript_StevenHandoff::
	goto_if_set FLAG_SYS_GAME_CLEAR, CaveOfOrigin_DianciesRoom_EventScript_OptionalHandoff
	goto_if_set FLAG_EC_REPORT_C48_COMPLETE, CaveOfOrigin_DianciesRoom_EventScript_LeagueHandoff
	msgbox CaveOfOrigin_DianciesRoom_Text_StevenHandoff, MSGBOX_DEFAULT
	goto CaveOfOrigin_DianciesRoom_EventScript_End
	end


CaveOfOrigin_DianciesRoom_EventScript_LeagueHandoff::
	msgbox CaveOfOrigin_DianciesRoom_Text_LeagueHandoff, MSGBOX_DEFAULT
	goto CaveOfOrigin_DianciesRoom_EventScript_End
	end

CaveOfOrigin_DianciesRoom_EventScript_OptionalHandoff::
	msgbox CaveOfOrigin_DianciesRoom_Text_OptionalHandoff, MSGBOX_DEFAULT
	goto CaveOfOrigin_DianciesRoom_EventScript_End
	end

CaveOfOrigin_DianciesRoom_EventScript_BadgePending::
	msgbox CaveOfOrigin_DianciesRoom_Text_BadgePending, MSGBOX_DEFAULT
CaveOfOrigin_DianciesRoom_EventScript_End::
	releaseall
	end

CaveOfOrigin_DianciesRoom_Text_CrystalLight:
	.string "A soft light catches the stone.\n"
	.string "A clear note echoes through\l"
	.string "the crystal chamber.$"

CaveOfOrigin_DianciesRoom_Text_Observation:
	.string "The DEWFORD MANOR account\n"
	.string "described this light and melody.\p"
	.string "DIANCIE's chamber preserves the\n"
	.string "living source of that old story.\l"
	.string "You take time to study it.$"

CaveOfOrigin_DianciesRoom_Text_BattleOffer:
	.string "Would you like to battle DIANCIE?\n"
	.string "Win or catch it to finish this\l"
	.string "discovery. Capturing is optional.$"

CaveOfOrigin_DianciesRoom_Text_ObserveEmptyChamber:
	.string "DIANCIE is no longer here, but\n"
	.string "the chamber still catches the light.\p"
	.string "Study the crystal chamber and\n"
	.string "the old account now?$"

CaveOfOrigin_DianciesRoom_Text_MeteorHandoff:
	.string "Next, visit METEOR FALLS from\n"
	.string "ROUTE 114. Bring partners for your\l"
	.string "licensed SURF and WATERFALL.\p"
	.string "Climb the waterfall to the deeper\n"
	.string "JIRACHI chamber. Interact with\l"
	.string "JIRACHI to study its awakening.\p"
	.string "DIANCITE is an optional find in\n"
	.string "SHOAL CAVE's low-tide stair room,\l"
	.string "on ROUTE 125 north of MOSSDEEP.\p"
	.string "This is separate from STEVEN's\n"
	.string "optional exhibition cave. Catching\l"
	.string "either POKéMON is your choice.$"

CaveOfOrigin_DianciesRoom_Text_StevenHandoff:
	.string "Both chambers match the old\n"
	.string "account. Meet STEVEN at his house\l"
	.string "in MOSSDEEP to discuss them.\p"
	.string "Both challenges are complete.\n"
	.string "Catching either is your choice.$"

CaveOfOrigin_DianciesRoom_Text_BadgePending:
	.string "Return to study the old account\n"
	.string "after earning JUAN's RAIN BADGE\l"
	.string "at SOOTOPOLIS GYM.$"


CaveOfOrigin_DianciesRoom_Text_LeagueHandoff:
	.string "The old account is complete.\n"
	.string "Sail east from ROUTE 128 to\l"
	.string "EVER GRANDE and climb its falls.\p"
	.string "Heal at the CENTER, then enter\n"
	.string "VICTORY ROAD. Your LEAGUE\l"
	.string "challenge is waiting.$"

CaveOfOrigin_DianciesRoom_Text_OptionalHandoff:
	.string "You are CHAMPION, and the old\n"
	.string "account is complete. These chambers\l"
	.string "and their battles remain optional.$"

CaveOfOrigin_DianciesRoom_Text_WallaceOffer::
	.string "WALLACE: Would you join me for a\n"
	.string "special exhibition battle here?\p"
	.string "I've brought a different team from\n"
	.string "the one you faced at the LEAGUE.$"

CaveOfOrigin_DianciesRoom_Text_WallaceDecline::
	.string "Whenever your team is ready. I\n"
	.string "will be here.$"

CaveOfOrigin_DianciesRoom_EventScript_WallaceDecline::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceDecline, MSGBOX_DEFAULT
	release
	end

CaveOfOrigin_DianciesRoom_EventScript_Resolved::
	setflag FLAG_EC_RESOLVED_DIANCIE
	return

CaveOfOrigin_DianciesRoom_EventScript_BattlePending::
	msgbox CaveOfOrigin_DianciesRoom_Text_BattlePending, MSGBOX_DEFAULT
	goto CaveOfOrigin_DianciesRoom_EventScript_End
CaveOfOrigin_DianciesRoom_Text_BattlePending:
	.string "DIANCIE awaits your challenge.\n"
	.string "Return ready to win or catch it.\l"
	.string "You do not have to keep it.$"
