Interview_EventScript_EndInterview::
	special InterviewAfter
	incrementgamestat GAME_STAT_GOT_INTERVIEWED
	release
	end

@ Shares reporter object with TVSHOW_PKMN_FAN_CLUB_OPINIONS
SlateportCity_PokemonFanClub_EventScript_ReporterNoNickname::
	setvar VAR_0x8005, TVSHOW_FAN_CLUB_LETTER
	special InterviewBefore
	goto_if_eq VAR_RESULT, TRUE, SlateportCity_PokemonFanClub_EventScript_AlreadyInterviewed2
	copyvar VAR_0x8009, VAR_0x8006
	msgbox SlateportCity_PokemonFanClub_Text_InterviewRequest, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, SlateportCity_PokemonFanClub_EventScript_AcceptInterview2
	goto_if_eq VAR_RESULT, NO, SlateportCity_PokemonFanClub_EventScript_DeclineInterview2
	end

SlateportCity_PokemonFanClub_EventScript_AcceptInterview2::
	msgbox SlateportCity_PokemonFanClub_Text_TellMeAnythingAboutYourMon, MSGBOX_DEFAULT
	setvar VAR_0x8004, EASY_CHAT_TYPE_INTERVIEW
	copyvar VAR_0x8005, VAR_0x8009
	setvar VAR_0x8006, EASY_CHAT_PERSON_REPORTER_FEMALE
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	goto_if_eq VAR_RESULT, 1, SlateportCity_PokemonFanClub_EventScript_SubmitResponse2
	goto_if_eq VAR_RESULT, 0, SlateportCity_PokemonFanClub_EventScript_DeclineInterview2
	end

SlateportCity_PokemonFanClub_EventScript_DeclineInterview2::
	msgbox SlateportCity_PokemonFanClub_Text_HereIfYouGetUrgeToTellMe, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_SubmitResponse2::
	msgbox SlateportCity_PokemonFanClub_Text_ThatsAllForInterview2, MSGBOX_DEFAULT
	setvar VAR_0x8005, TVSHOW_FAN_CLUB_LETTER
	goto Interview_EventScript_EndInterview
	end

SlateportCity_PokemonFanClub_EventScript_AlreadyInterviewed2::
	msgbox SlateportCity_PokemonFanClub_Text_EnjoyDoingInterviews, MSGBOX_DEFAULT
	release
	end

SlateportCity_OceanicMuseum_1F_EventScript_Reporter::
	lock
	faceplayer
	setvar VAR_0x8005, TVSHOW_RECENT_HAPPENINGS
	special InterviewBefore
	goto_if_eq VAR_RESULT, TRUE, SlateportCity_OceanicMuseum_1F_EventScript_AlreadyInterviewed
	copyvar VAR_0x8009, VAR_0x8006
	goto_if_set FLAG_OCEANIC_MUSEUM_MET_REPORTER, SlateportCity_OceanicMuseum_1F_EventScript_RequestInterviewShort
	setflag FLAG_OCEANIC_MUSEUM_MET_REPORTER
	msgbox SlateportCity_OceanicMuseum_1F_Text_InterviewRequest, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, SlateportCity_OceanicMuseum_1F_EventScript_AcceptInterview
	goto_if_eq VAR_RESULT, NO, SlateportCity_OceanicMuseum_1F_EventScript_DeclineInterview
	end

SlateportCity_OceanicMuseum_1F_EventScript_RequestInterviewShort::
	msgbox SlateportCity_OceanicMuseum_1F_Text_InterviewRequestShort, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, SlateportCity_OceanicMuseum_1F_EventScript_AcceptInterview
	goto_if_eq VAR_RESULT, NO, SlateportCity_OceanicMuseum_1F_EventScript_DeclineInterview
	end

SlateportCity_OceanicMuseum_1F_EventScript_AcceptInterview::
	msgbox SlateportCity_OceanicMuseum_1F_Text_TellMeExperienceInvolvingPokemon, MSGBOX_DEFAULT
	setvar VAR_0x8004, EASY_CHAT_TYPE_INTERVIEW
	copyvar VAR_0x8005, VAR_0x8009
	setvar VAR_0x8006, EASY_CHAT_PERSON_REPORTER_MALE
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	goto_if_eq VAR_RESULT, 1, SlateportCity_OceanicMuseum_1F_EventScript_SubmitResponse
	goto_if_eq VAR_RESULT, 0, SlateportCity_OceanicMuseum_1F_EventScript_DeclineInterview
	end

SlateportCity_OceanicMuseum_1F_EventScript_DeclineInterview::
	msgbox SlateportCity_OceanicMuseum_1F_Text_LetMeKnowIfYouHaveStory, MSGBOX_DEFAULT
	release
	end

SlateportCity_OceanicMuseum_1F_EventScript_SubmitResponse::
	msgbox SlateportCity_OceanicMuseum_1F_Text_ThatsAllForInterview, MSGBOX_DEFAULT
	setvar VAR_0x8005, TVSHOW_RECENT_HAPPENINGS
	goto Interview_EventScript_EndInterview
	end

SlateportCity_OceanicMuseum_1F_EventScript_AlreadyInterviewed::
	msgbox SlateportCity_OceanicMuseum_1F_Text_BetterWriteUpStory, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_Reporter::
	lock
	faceplayer
	specialvar VAR_RESULT, IsLeadMonNicknamedOrNotEnglish
	goto_if_eq VAR_RESULT, FALSE, SlateportCity_PokemonFanClub_EventScript_ReporterNoNickname
	setvar VAR_0x8005, TVSHOW_PKMN_FAN_CLUB_OPINIONS
	special InterviewBefore
	goto_if_eq VAR_RESULT, TRUE, SlateportCity_PokemonFanClub_EventScript_AlreadyInterviewed
	copyvar VAR_0x8009, VAR_0x8006
	msgbox SlateportCity_PokemonFanClub_Text_InterviewRequestHasName, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, SlateportCity_PokemonFanClub_EventScript_AcceptInterview
	goto_if_eq VAR_RESULT, NO, SlateportCity_PokemonFanClub_EventScript_DeclineInterview
	end

SlateportCity_PokemonFanClub_EventScript_AcceptInterview::
	msgbox SlateportCity_PokemonFanClub_Text_HereGoesQuickAnswers, MSGBOX_DEFAULT
	random 3
	copyvar VAR_0x800A, VAR_RESULT
	switch VAR_RESULT
	case 0, SlateportCity_PokemonFanClub_EventScript_RandomQuestion1
	case 1, SlateportCity_PokemonFanClub_EventScript_RandomQuestion2
	case 2, SlateportCity_PokemonFanClub_EventScript_RandomQuestion3
	end

SlateportCity_PokemonFanClub_EventScript_RandomQuestion1::
	msgbox SlateportCity_PokemonFanClub_Text_DescribeFeelingsFirstMetMon, MSGBOX_DEFAULT
	goto SlateportCity_PokemonFanClub_EventScript_ContinueInterview
	end

SlateportCity_PokemonFanClub_EventScript_RandomQuestion2::
	msgbox SlateportCity_PokemonFanClub_Text_LikenMonToSomethingYouLike, MSGBOX_DEFAULT
	goto SlateportCity_PokemonFanClub_EventScript_ContinueInterview
	end

SlateportCity_PokemonFanClub_EventScript_RandomQuestion3::
	msgbox SlateportCity_PokemonFanClub_Text_WhatAttractedYouAboutMon, MSGBOX_DEFAULT
	goto SlateportCity_PokemonFanClub_EventScript_ContinueInterview
	end

SlateportCity_PokemonFanClub_EventScript_ContinueInterview::
	setvar VAR_0x8004, EASY_CHAT_TYPE_FAN_CLUB
	copyvar VAR_0x8005, VAR_0x8009
	setvar VAR_0x8006, 0
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	goto_if_eq VAR_RESULT, 0, SlateportCity_PokemonFanClub_EventScript_DeclineInterview
	msgbox SlateportCity_PokemonFanClub_Text_WhatDoPokemonMeanToYou, MSGBOX_DEFAULT
	setvar VAR_0x8006, 1
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	goto_if_eq VAR_RESULT, 0, SlateportCity_PokemonFanClub_EventScript_DeclineInterview
	msgbox SlateportCity_PokemonFanClub_Text_ThatsAllForInterview, MSGBOX_DEFAULT
	copyvar VAR_0x8007, VAR_0x800A
	setvar VAR_0x8005, TVSHOW_PKMN_FAN_CLUB_OPINIONS
	goto Interview_EventScript_EndInterview
	end

SlateportCity_PokemonFanClub_EventScript_DeclineInterview::
	msgbox SlateportCity_PokemonFanClub_Text_HereIfYouGetUrgeToTellMe, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_AlreadyInterviewed::
	msgbox SlateportCity_PokemonFanClub_Text_EnjoyDoingInterviews, MSGBOX_DEFAULT
	release
	end

@ Emerald Champions: the Bravo Trainer reporter is retired with the vanilla Battle
@ Tower challenge. The object stays in map.json until the owner removes it and the
@ Tower lobby hides it on transition, so this script is unreachable; it keeps a
@ single line so the map.json reference still resolves.
BattleFrontier_BattleTowerLobby_EventScript_Reporter::
	msgbox BattleFrontier_BattleTowerLobby_Text_NoStoryToFile, MSGBOX_NPC
	end

BattleFrontier_BattleTowerLobby_Text_NoStoryToFile:
	.string "There is no broadcast to file.\n"
	.string "The CIRCUIT keeps its own record.$"



@ ---------------------------------------------------------------------------
@ Inclement Emerald Lilycove Contest Lobby helpers, restored verbatim because
@ data/maps/LilycoveCity_ContestLobby/scripts.inc calls
@ LilycoveCity_ContestLobby_EventScript_TryShowContestReporter (on transition)
@ and LilycoveCity_ContestLobby_EventScript_SetPlayerGfx (contest entry).
@
@ TryShowContestReporter and its two tails come from the donor's
@ data/scripts/interview.inc. SetPlayerGfx and its two tails come from the
@ donor's data/maps/ContestHall/scripts.inc, which has no counterpart in this
@ engine (there is no data/maps/ContestHall here), so they are homed in this
@ file rather than in the map script.
@
@ Note: the special InterviewBefore below is passed
@ TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE, a TV show this engine no longer
@ implements. InterviewBefore clears VAR_RESULT to FALSE and ignores unhandled
@ show types, so the call is inert: the duplicate-broadcast check simply never
@ suppresses the reporter. Kept verbatim rather than rewritten.
@ ---------------------------------------------------------------------------

LilycoveCity_ContestLobby_EventScript_TryShowContestReporter::
	compare VAR_CONTEST_HALL_STATE, 2
	goto_if_ne LilycoveCity_ContestLobby_EventScript_DontShowContestReporter
	setvar VAR_0x8005, TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE
	special InterviewBefore
	compare VAR_RESULT, TRUE
	goto_if_eq LilycoveCity_ContestLobby_EventScript_DontShowContestReporter
	switch VAR_CONTEST_TYPE
	case 0, LilycoveCity_ContestLobby_EventScript_DontShowContestReporter
	case 2, LilycoveCity_ContestLobby_EventScript_ShowContestReporter
	case 1, LilycoveCity_ContestLobby_EventScript_ShowContestReporter
	case 3, LilycoveCity_ContestLobby_EventScript_ShowContestReporter
	case 4, LilycoveCity_ContestLobby_EventScript_ShowContestReporter
	case 5, LilycoveCity_ContestLobby_EventScript_DontShowContestReporter
	end

LilycoveCity_ContestLobby_EventScript_ShowContestReporter::
	clearflag FLAG_HIDE_LILYCOVE_CONTEST_HALL_REPORTER
	return

LilycoveCity_ContestLobby_EventScript_DontShowContestReporter::
	return

LilycoveCity_ContestLobby_EventScript_SetPlayerGfx:: @ 823BEB6
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq LilycoveCity_ContestLobby_EventScript_SetPlayerGfxBrendan
	compare VAR_RESULT, FEMALE
	goto_if_eq LilycoveCity_ContestLobby_EventScript_SetPlayerGfxMay
	return

LilycoveCity_ContestLobby_EventScript_SetPlayerGfxBrendan:: @ 823BECE
	setvar VAR_OBJ_GFX_ID_3, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	return

LilycoveCity_ContestLobby_EventScript_SetPlayerGfxMay:: @ 823BED4
	setvar VAR_OBJ_GFX_ID_3, OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
	return
