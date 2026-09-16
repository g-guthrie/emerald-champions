SlateportCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SlateportCity_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SlateportCity_OnFrame
	.byte 0

SlateportCity_EventScript_BattleItemScholar::
	msgbox SlateportCity_Text_BattleItemScholar, MSGBOX_NPC
	end

SlateportCity_Text_BattleItemScholar:
	.string "I once sold POWER items here.\p"
	.string "The left-side vendor in every CENTER\n"
	.string "now supplies battle items for free.$"

SlateportCity_EventScript_IncenseScholar::
	msgbox SlateportCity_Text_IncenseScholar, MSGBOX_NPC
	end

SlateportCity_Text_IncenseScholar:
	.string "You don't need Incense to get\n"
	.string "baby POKéMON from compatible\l"
	.string "parents.\p"
	.string "Some Incenses have effects in\n"
	.string "battle, though. The CENTER\l"
	.string "vendors can help you find them.$"

@ Inclement: Brawly is waiting in the Slateport crowd. Finding him sends him home,
@ and removeobject sets FLAG_HIDE_SLATEPORT_CITY_BRAWLY, which is shared with the
@ Dewford Gym guide, so the Gym entrance clears at the same moment.
SlateportCity_EventScript_Brawly::
	lockall
	msgbox SlateportCity_Text_WhoahLineSoLong, MSGBOX_DEFAULT
	waitse
	playse SE_PIN
	applymovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement VAR_LAST_TALKED, Common_Movement_Delay48
	waitmovement 0
	faceplayer
	msgbox SlateportCity_Text_LookingForBrawly, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	closemessage
	removeobject VAR_LAST_TALKED
	fadescreen FADE_FROM_BLACK
	releaseall
	end

SlateportCity_Text_WhoahLineSoLong:
	.string "Whoa! This line barely moves!\n"
	.string "I could have paddled home by now.$"

SlateportCity_Text_LookingForBrawly:
	.string "Oh! You came looking for\n"
	.string "DEWFORD's GYM LEADER?\p"
	.string "That's me! BRAWLY.\n"
	.string "Guess I've kept you waiting.\p"
	.string "I'll head back to DEWFORD right now.\n"
	.string "Bring two POKéMON. I battle doubles!$"

SlateportCity_OnTransition:
	setvar VAR_SLATEPORT_MUSEUM_1F_STATE, 0
	call SlateportCity_EventScript_CheckKnuckleGuard
	call SlateportCity_EventScript_EnterSlateport
	call_if_eq VAR_SLATEPORT_CITY_STATE, 1, SlateportCity_EventScript_MovePeopleForSternInterview
	call_if_eq VAR_SLATEPORT_OUTSIDE_MUSEUM_STATE, 1, SlateportCity_EventScript_SetReadyForScottScene
	end

SlateportCity_EventScript_EnterSlateport::
	setflag FLAG_VISITED_SLATEPORT_CITY
	setvar VAR_CONTEST_HALL_STATE, 0
	setflag FLAG_HIDE_SLATEPORT_CITY_CONTEST_REPORTER
	return

SlateportCity_EventScript_MovePeopleForSternInterview::
	setobjectxyperm LOCALID_SLATEPORT_CAPT_STERN, 28, 13
	setobjectxyperm LOCALID_SLATEPORT_OLD_WOMAN, 25, 13
	setobjectxyperm LOCALID_SLATEPORT_RICH_BOY, 25, 14
	setobjectxyperm LOCALID_SLATEPORT_COOK, 27, 16
	setobjectxyperm LOCALID_SLATEPORT_GIRL, 28, 16
	setobjectxyperm LOCALID_SLATEPORT_FAT_MAN, 29, 16
	setobjectxyperm LOCALID_SLATEPORT_MAN_1, 31, 14
	setobjectmovementtype LOCALID_SLATEPORT_CAPT_STERN, MOVEMENT_TYPE_FACE_DOWN
	setobjectmovementtype LOCALID_SLATEPORT_OLD_WOMAN, MOVEMENT_TYPE_FACE_DOWN_AND_RIGHT
	setobjectmovementtype LOCALID_SLATEPORT_RICH_BOY, MOVEMENT_TYPE_FACE_UP_AND_RIGHT
	setobjectmovementtype LOCALID_SLATEPORT_COOK, MOVEMENT_TYPE_FACE_UP
	setobjectmovementtype LOCALID_SLATEPORT_GIRL, MOVEMENT_TYPE_FACE_UP
	setobjectmovementtype LOCALID_SLATEPORT_FAT_MAN, MOVEMENT_TYPE_FACE_UP
	setobjectmovementtype LOCALID_SLATEPORT_MAN_1, MOVEMENT_TYPE_FACE_LEFT
	return

SlateportCity_EventScript_SetReadyForScottScene::
	setflag FLAG_HIDE_MAP_NAME_POPUP
	getplayerxy VAR_0x8004, VAR_0x8005
	goto_if_eq VAR_0x8004, 30, SlateportCity_EventScript_MoveScottLeft
	setobjectxyperm LOCALID_SLATEPORT_SCOTT, 23, 27
	setobjectmovementtype LOCALID_SLATEPORT_SCOTT, MOVEMENT_TYPE_FACE_RIGHT
	return

SlateportCity_EventScript_MoveScottLeft::
	setobjectxyperm LOCALID_SLATEPORT_SCOTT, 22, 27
	setobjectmovementtype LOCALID_SLATEPORT_SCOTT, MOVEMENT_TYPE_FACE_RIGHT
	return

SlateportCity_OnFrame:
	map_script_2 VAR_SLATEPORT_OUTSIDE_MUSEUM_STATE, 1, SlateportCity_EventScript_ScottScene
	.2byte 0

SlateportCity_EventScript_ScottScene::
	lockall
	addobject LOCALID_SLATEPORT_SCOTT
	applymovement LOCALID_PLAYER, SlateportCity_Movement_PlayerFaceScott
	applymovement LOCALID_SLATEPORT_SCOTT, SlateportCity_Movement_ScottApproachPlayer
	waitmovement 0
	msgbox SlateportCity_Text_YouDroveTeamAquaAway, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_SCOTT, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	delay 60
	msgbox SlateportCity_Text_MaybeThisTrainer, MSGBOX_DEFAULT
	applymovement LOCALID_SLATEPORT_SCOTT, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox SlateportCity_Text_LetsRegisterEachOther, MSGBOX_DEFAULT
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox SlateportCity_Text_RegisteredScott, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_SCOTT_MATCH_CALL
	msgbox SlateportCity_Text_KeepEyeOnTrainersBeSeeingYou, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_SCOTT, SlateportCity_Movement_ScottExit
	waitmovement 0
	removeobject LOCALID_SLATEPORT_SCOTT
	setobjectxyperm LOCALID_SLATEPORT_SCOTT, 10, 12
	setobjectmovementtype LOCALID_SLATEPORT_SCOTT, MOVEMENT_TYPE_FACE_DOWN
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	setvar VAR_SLATEPORT_OUTSIDE_MUSEUM_STATE, 2
	addvar VAR_SCOTT_STATE, 1
	releaseall
	end

SlateportCity_Movement_PlayerFaceScott:
	delay_16
	delay_8
	walk_in_place_faster_left
	step_end

SlateportCity_Movement_ScottApproachPlayer:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

SlateportCity_Movement_ScottExit:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

SlateportCity_EventScript_CatchingGuru::
	lock
	faceplayer
	message SlateportCity_Text_CatchingGuruSellWhatYouNeed
	waitmessage
	pokemart SlateportCity_Pokemart_CatchingGuru
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
SlateportCity_Pokemart_CatchingGuru:
	.2byte ITEM_HEAL_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_REPEAT_BALL
	pokemartlistend

SlateportCity_EventScript_EvGuide::
	bufferleadmonspeciesname STR_VAR_1
	msgbox SlateportCity_Text_EvsAlreadyReady, MSGBOX_NPC
	end

SlateportCity_EventScript_Cook::
	lock
	faceplayer
	call_if_eq VAR_SLATEPORT_CITY_STATE, 1, SlateportCity_EventScript_CookSternInterview
	msgbox SlateportCity_Text_SeaweedFullOfLife, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_CookSternInterview::
	msgbox SlateportCity_Text_CaptainComeBackWithBigFish, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_OldWoman::
	lock
	faceplayer
	call_if_eq VAR_SLATEPORT_CITY_STATE, 1, SlateportCity_EventScript_OldWomanSternInterview
	msgbox SlateportCity_Text_HowTownIsBornAndGrows, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_OldWomanSternInterview::
	msgbox SlateportCity_Text_CaptSternBeingInterviewed, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_Girl::
	lock
	faceplayer
	call_if_eq VAR_SLATEPORT_CITY_STATE, 1, SlateportCity_EventScript_GirlSternInterview
	goto_if_set FLAG_RECEIVED_SECRET_POWER, SlateportCity_EventScript_GirlSecretBase
	msgbox SlateportCity_Text_SlateportWonderfulPlace, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_GirlSternInterview::
	msgbox SlateportCity_Text_InterviewerSoCool, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_GirlSecretBase::
	msgbox SlateportCity_Text_BuyBricksSoDecorWontGetDirty, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_RichBoy::
	lock
	faceplayer
	call_if_eq VAR_SLATEPORT_CITY_STATE, 1, SlateportCity_EventScript_RichBoySternInterview
	msgbox SlateportCity_Text_GoingToCompeteInBattleTent, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_RichBoySternInterview::
	msgbox SlateportCity_Text_SternSaysDiscoveredSomething, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_FatMan::
	goto_if_eq VAR_SLATEPORT_CITY_STATE, 1, SlateportCity_EventScript_FatManSternInterview
	msgbox SlateportCity_Text_BushedHikingFromMauville, MSGBOX_NPC
	end

SlateportCity_EventScript_FatManSternInterview::
	msgbox SlateportCity_Text_AmIOnTV, MSGBOX_SIGN
	end

SlateportCity_EventScript_Man1::
	lock
	faceplayer
	call_if_eq VAR_SLATEPORT_CITY_STATE, 1, SlateportCity_EventScript_Man1SternInterview
	msgbox SlateportCity_Text_EveryoneCallsHimCaptStern, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_Man1SternInterview::
	msgbox SlateportCity_Text_CaptainsACelebrity, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_Woman1::
	lock
	faceplayer
	goto_if_set FLAG_DOCK_REJECTED_DEVON_GOODS, SlateportCity_EventScript_Woman1AquaGone
	msgbox SlateportCity_Text_WhatsLongLineOverThere, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_Woman1AquaGone::
	msgbox SlateportCity_Text_VisitedMuseumOften, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_BattleTentSign::
	msgbox SlateportCity_Text_BattleTentSign, MSGBOX_SIGN
	end

SlateportCity_EventScript_SternsShipyardSign::
	lockall
	goto_if_set FLAG_BADGE06_GET, SlateportCity_EventScript_SternsShipyardFerryComplete
	goto_if_set FLAG_BADGE07_GET, SlateportCity_EventScript_SternsShipyardNearsCompletion
	msgbox SlateportCity_Text_SternsShipyardWantedSign, MSGBOX_DEFAULT
	releaseall
	end

SlateportCity_EventScript_SternsShipyardNearsCompletion::
	msgbox SlateportCity_Text_SternsShipyardNearsCompletion, MSGBOX_DEFAULT
	releaseall
	end

SlateportCity_EventScript_SternsShipyardFerryComplete::
	msgbox SlateportCity_Text_SternsShipyardFerryComplete, MSGBOX_DEFAULT
	releaseall
	end

SlateportCity_EventScript_PokemonFanClubSign::
	msgbox SlateportCity_Text_PokemonFanClubSign, MSGBOX_SIGN
	end

SlateportCity_EventScript_OceanicMuseumSign::
	msgbox SlateportCity_Text_OceanicMuseumSign, MSGBOX_SIGN
	end

SlateportCity_EventScript_CitySign::
	msgbox SlateportCity_Text_CitySign, MSGBOX_SIGN
	end

SlateportCity_EventScript_MarketSign::
	msgbox SlateportCity_Text_MarketSign, MSGBOX_SIGN
	end

SlateportCity_EventScript_HarborSign::
	lockall
	goto_if_set FLAG_BADGE06_GET, SlateportCity_EventScript_HarborSignFerryComplete
	msgbox SlateportCity_Text_HarborFerryUnderConstruction, MSGBOX_DEFAULT
	releaseall
	end

SlateportCity_EventScript_HarborSignFerryComplete::
	msgbox SlateportCity_Text_HarborSign, MSGBOX_DEFAULT
	releaseall
	end

SlateportCity_EventScript_NameRatersHouseSign::
	msgbox SlateportCity_Text_NameRatersHouseSign, MSGBOX_SIGN
	end

SlateportCity_EventScript_Maniac::
	lock
	faceplayer
	call_if_eq VAR_SLATEPORT_CITY_STATE, 1, SlateportCity_EventScript_ManiacSternInterview
	msgbox SlateportCity_Text_GetNameRaterToHelpYou, MSGBOX_DEFAULT
	release
	end

@ Unclear if the text here was meant to be different, but its not
SlateportCity_EventScript_ManiacSternInterview::
	msgbox SlateportCity_Text_GetNameRaterToHelpYou, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_Woman2::
	msgbox SlateportCity_Text_CantChangeTradeMonName, MSGBOX_NPC
	end

SlateportCity_EventScript_Sailor1::
	msgbox SlateportCity_Text_SeaIsSoWet, MSGBOX_NPC
	end

SlateportCity_EventScript_Sailor2::
	msgbox SlateportCity_Text_SinkOldBoats, MSGBOX_NPC
	end

SlateportCity_EventScript_PokefanF::
	msgbox SlateportCity_Text_BuyTooMuch, MSGBOX_NPC
	end

SlateportCity_EventScript_Man2::
	msgbox SlateportCity_Text_BattleTentBuiltRecently, MSGBOX_NPC
	end

SlateportCity_EventScript_AquaGrunt1::
	lock
	faceplayer
	msgbox SlateportCity_Text_QuitPushing, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_1, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_EventScript_AquaGrunt2::
	lock
	faceplayer
	msgbox SlateportCity_Text_AquaHasPolicy, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_2, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_EventScript_AquaGrunt3::
	lock
	faceplayer
	msgbox SlateportCity_Text_BossIsBrilliant, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_3, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_EventScript_AquaGrunt4::
	lock
	faceplayer
	msgbox SlateportCity_Text_WhatsNewSchemeIWonder, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_4, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_EventScript_AquaGrunt5::
	lock
	faceplayer
	msgbox SlateportCity_Text_ShouldTakeItAll, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_5, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_EventScript_AquaGrunt6::
	lock
	faceplayer
	msgbox SlateportCity_Text_DontButtIn, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_6, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_EventScript_AquaGrunt7::
	lock
	faceplayer
	msgbox SlateportCity_Text_RemindsMeOfLongLineForGames, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_7, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_EventScript_AquaGrunt8::
	lock
	faceplayer
	msgbox SlateportCity_Text_WhyAreWeLiningUp, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_8, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_EventScript_AquaGrunt9::
	lock
	faceplayer
	playse SE_PIN
	applymovement LOCALID_SLATEPORT_GRUNT_9, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_SLATEPORT_GRUNT_9, Common_Movement_Delay48
	waitmovement 0
	msgbox SlateportCity_Text_WhatDoYouWant, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_9, Common_Movement_FaceOriginalDirection
	waitmovement 0
	applymovement LOCALID_SLATEPORT_GRUNT_9, SlateportCity_Movement_DelayAquaGrunt
	waitmovement 0
	applymovement LOCALID_SLATEPORT_GRUNT_9, Common_Movement_FacePlayer
	waitmovement 0
	msgbox SlateportCity_Text_IllReadSignForYou, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_9, Common_Movement_FaceOriginalDirection
	waitmovement 0
	applymovement LOCALID_SLATEPORT_GRUNT_9, SlateportCity_Movement_DelayAquaGrunt
	waitmovement 0
	msgbox SlateportCity_Text_SaysSomethingLikeSeaIsEndless, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_9, Common_Movement_FacePlayer
	waitmovement 0
	applymovement LOCALID_SLATEPORT_GRUNT_9, SlateportCity_Movement_DelayAquaGrunt
	waitmovement 0
	applymovement LOCALID_SLATEPORT_GRUNT_9, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_Movement_DelayAquaGrunt:
	delay_16
	delay_16
	step_end

SlateportCity_EventScript_AquaGrunt10::
	msgbox SlateportCity_Text_ShouldveBroughtMyGameBoy, MSGBOX_SIGN
	end

SlateportCity_EventScript_AquaGrunt11::
	lock
	faceplayer
	msgbox SlateportCity_Text_HotSpringsAfterOperation, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GRUNT_11, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_EventScript_DollClerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration SlateportCity_PokemartDecor_Dolls
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
SlateportCity_PokemartDecor_Dolls:
	.2byte DECOR_AZURILL_DOLL
	.2byte DECOR_MARILL_DOLL
	.2byte DECOR_SKITTY_DOLL
	pokemartlistend

SlateportCity_EventScript_ComeBackWithSecretPower::
	msgbox gText_ComeBackWithSecretPower, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_DecorClerk::
	lock
	faceplayer
	goto_if_unset FLAG_RECEIVED_SECRET_POWER, SlateportCity_EventScript_ComeBackWithSecretPower
	message gText_HowMayIServeYou
	waitmessage
	pokemartdecoration SlateportCity_PokemartDecor
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
SlateportCity_PokemartDecor:
	.2byte DECOR_RED_BRICK
	.2byte DECOR_BLUE_BRICK
	.2byte DECOR_YELLOW_BRICK
	.2byte DECOR_RED_BALLOON
	.2byte DECOR_BLUE_BALLOON
	.2byte DECOR_YELLOW_BALLOON
	.2byte DECOR_C_LOW_NOTE_MAT
	.2byte DECOR_D_NOTE_MAT
	.2byte DECOR_E_NOTE_MAT
	.2byte DECOR_F_NOTE_MAT
	.2byte DECOR_G_NOTE_MAT
	.2byte DECOR_A_NOTE_MAT
	.2byte DECOR_B_NOTE_MAT
	.2byte DECOR_C_HIGH_NOTE_MAT
	pokemartlistend

SlateportCity_EventScript_FieldSupplyClerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart SlateportCity_Pokemart_FieldSupplies
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
SlateportCity_Pokemart_FieldSupplies:
	.2byte ITEM_HONEY
	.2byte ITEM_FLUFFY_TAIL
	pokemartlistend

@ Scene with Capt Sterns interview and Team Aqua announcing plans to steal Submarine
SlateportCity_EventScript_CaptStern::
	lockall
	msgbox SlateportCity_Text_SternMoveAheadWithExploration, MSGBOX_DEFAULT
	msgbox SlateportCity_Text_GabbyWonderfulThanksForInterview, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_GABBY, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	delay 10
	applymovement LOCALID_SLATEPORT_TY, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 25
	applymovement LOCALID_SLATEPORT_CAPT_STERN, SlateportCity_Movement_SternWatchGabbyAndTyExit
	applymovement LOCALID_PLAYER, SlateportCity_Movement_PlayerFaceStern
	applymovement LOCALID_SLATEPORT_GABBY, SlateportCity_Movement_GabbyExit
	applymovement LOCALID_SLATEPORT_TY, SlateportCity_Movement_TyExit
	waitmovement 0
	removeobject LOCALID_SLATEPORT_GABBY
	removeobject LOCALID_SLATEPORT_TY
	msgbox SlateportCity_Text_SternWhewFirstInterview, MSGBOX_DEFAULT
	applymovement LOCALID_SLATEPORT_CAPT_STERN, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox SlateportCity_Text_OhPlayerWeMadeDiscovery, MSGBOX_DEFAULT
	playbgm MUS_ENCOUNTER_AQUA, FALSE
	msgbox SlateportCity_Text_AquaWillAssumeControlOfSubmarine, MSGBOX_DEFAULT
	applymovement LOCALID_SLATEPORT_COOK, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_SLATEPORT_FAT_MAN, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_SLATEPORT_OLD_WOMAN, SlateportCity_Movement_OldWomanConcern
	applymovement LOCALID_SLATEPORT_RICH_BOY, Common_Movement_QuestionMark
	applymovement LOCALID_SLATEPORT_MAN_1, SlateportCity_Movement_ManConcern
	waitmovement 0
	applymovement LOCALID_SLATEPORT_CAPT_STERN, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	msgbox SlateportCity_Text_SternWhatWasAllThat, MSGBOX_DEFAULT
	playse SE_PIN
	applymovement LOCALID_SLATEPORT_CAPT_STERN, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_SLATEPORT_CAPT_STERN, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_SLATEPORT_CAPT_STERN, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox SlateportCity_Text_FromHarborTryingToTakeSub, MSGBOX_DEFAULT
	msgbox SlateportCity_Text_PleaseComeWithMe, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_CAPT_STERN, SlateportCity_Movement_SternEnterHarbor
	applymovement LOCALID_PLAYER, SlateportCity_Movement_PlayerEnterHarbor
	waitmovement 0
	removeobject LOCALID_SLATEPORT_CAPT_STERN
	clearflag FLAG_HIDE_SLATEPORT_CITY_HARBOR_CAPTAIN_STERN
	clearflag FLAG_HIDE_SLATEPORT_CITY_HARBOR_SUBMARINE_SHADOW
	clearflag FLAG_HIDE_SLATEPORT_CITY_HARBOR_AQUA_GRUNT
	clearflag FLAG_HIDE_SLATEPORT_CITY_HARBOR_ARCHIE
	setvar VAR_SLATEPORT_CITY_STATE, 2
	warp MAP_SLATEPORT_CITY_HARBOR, 11, 14
	waitstate
	releaseall
	end

SlateportCity_Movement_OldWomanConcern:
	delay_16
	delay_16
	emote_question_mark
	walk_in_place_faster_right
	delay_16
	delay_16
	walk_in_place_faster_left
	step_end

SlateportCity_Movement_ManConcern:
	emote_question_mark
	walk_in_place_faster_up
	delay_16
	walk_in_place_faster_down
	delay_16
	delay_16
	walk_in_place_faster_left
	step_end

SlateportCity_Movement_GabbyExit:
	delay_16
	walk_left
	walk_left
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

SlateportCity_Movement_TyExit:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

SlateportCity_Movement_Unused:
	walk_down
	walk_in_place_faster_up
	step_end

SlateportCity_Movement_SternEnterHarbor:
	walk_right
	walk_up
	walk_up
	set_invisible
	step_end

SlateportCity_Movement_SternWatchGabbyAndTyExit:
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	walk_down
	walk_left
	step_end

SlateportCity_Movement_PlayerEnterHarbor:
	walk_in_place_faster_right
	delay_16
	delay_16
	walk_right
	walk_up
	step_end

SlateportCity_Movement_PlayerFaceStern:
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	walk_in_place_faster_down
	step_end

SlateportCity_EventScript_Ty::
	msgbox SlateportCity_Text_BigSmileForCamera, MSGBOX_SIGN
	end

SlateportCity_EventScript_Gabby::
	msgbox SlateportCity_Text_MostInvaluableExperience, MSGBOX_SIGN
	end

SlateportCity_EventScript_Man3::
	msgbox SlateportCity_Text_WonderIfLighthouseStartlesPokemon, MSGBOX_NPC
	end

SlateportCity_EventScript_HerbalSupplyClerk::
	lock
	faceplayer
	msgbox SlateportCity_Text_HerbalSupplies, MSGBOX_DEFAULT
	pokemart SlateportCity_HerbalSupplyStock
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
SlateportCity_HerbalSupplyStock:
	.2byte ITEM_ENERGY_POWDER
	.2byte ITEM_ENERGY_ROOT
	.2byte ITEM_HEAL_POWDER
	.2byte ITEM_REVIVAL_HERB
	.2byte ITEM_ETHER
	pokemartlistend

SlateportCity_EventScript_ScottBattleTentScene::
	lockall
	applymovement LOCALID_PLAYER, Common_Movement_FaceUp
	waitmovement 0
	opendoor 10, 12
	waitdooranim
	addobject LOCALID_SLATEPORT_SCOTT
	applymovement LOCALID_PLAYER, SlateportCity_Movement_PushPlayerDown
	applymovement LOCALID_SLATEPORT_SCOTT, SlateportCity_Movement_ScottExitBattleTent
	waitmovement 0
	closedoor 10, 12
	waitdooranim
	msgbox SlateportCity_Text_TakingBattleTentChallenge, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, SlateportCity_Movement_PlayerWatchScottExit
	applymovement LOCALID_SLATEPORT_SCOTT, SlateportCity_Movement_ScottExitAfterBattleTent
	waitmovement 0
	removeobject LOCALID_SLATEPORT_SCOTT
	addvar VAR_SCOTT_STATE, 1
	setvar VAR_SLATEPORT_OUTSIDE_MUSEUM_STATE, 3
	releaseall
	end

SlateportCity_Movement_PushPlayerDown:
	lock_facing_direction
	walk_down
	unlock_facing_direction
	step_end

SlateportCity_Movement_PlayerWatchScottExit:
	delay_16
	walk_in_place_faster_right
	step_end

SlateportCity_Movement_ScottExitBattleTent:
	delay_8
	walk_down
	step_end

SlateportCity_Movement_ScottExitAfterBattleTent:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

SlateportCity_EventScript_MarketDirectory::
	msgbox SlateportCity_Text_MarketDirectory, MSGBOX_SIGN
	end

SlateportCity_Text_CatchingGuruSellWhatYouNeed:
	.string "I'm the CATCHING GURU! I stock\n"
	.string "specialized BALLS for every hunt.\p"
	.string "The right BALL can turn a rare sighting\n"
	.string "into your next teammate!$"

SlateportCity_Text_EvsAlreadyReady:
	.string "A CENTER tutor can prepare\n"
	.string "your {STR_VAR_1}'s EV spread.\p"
	.string "There is no EV training here; only\n"
	.string "team building and battle practice!$"

SlateportCity_Text_WonderIfLighthouseStartlesPokemon:
	.string "The light of the lighthouse reaches\n"
	.string "dozens of miles away.\p"
	.string "I wonder if it doesn't startle POKéMON\n"
	.string "in the sea.$"

SlateportCity_Text_SeaweedFullOfLife:
	.string "Ooh, look at this!\p"
	.string "The seaweed you can get around these\n"
	.string "parts is fresh and full of life.\p"
	.string "Why, it looks like it can even rear up\n"
	.string "and attack!$"

SlateportCity_Text_HowTownIsBornAndGrows:
	.string "Where the water is clean, the fruits\n"
	.string "of bountiful harvest gather.\p"
	.string "And where people gather, a market\n"
	.string "soon starts.\p"
	.string "That is how a town is born and grows.$"

SlateportCity_Text_SlateportWonderfulPlace:
	.string "Shopping where you can breathe the\n"
	.string "scent of the ocean…\p"
	.string "SLATEPORT is such a wonderful place!$"

SlateportCity_Text_BuyBricksSoDecorWontGetDirty:
	.string "If you put DOLLS or CUSHIONS on\n"
	.string "the floor, they'll get dirty.\p"
	.string "I'm going to buy some BRICKS so my\n"
	.string "DOLLS and CUSHIONS won't get dirty\l"
	.string "when I leave them out.$"

SlateportCity_Text_GoingToCompeteInBattleTent:
	.string "Wroooar! I'm going to compete in\n"
	.string "the BATTLE TENT, too!\p"
	.string "But before that, I need to catch\n"
	.string "some POKéMON!$"

SlateportCity_Text_BushedHikingFromMauville:
	.string "Whew… I'm just bushed…\p"
	.string "I hiked over from MAUVILLE CITY.\n"
	.string "But, boy, this city's huge.\p"
	.string "If I'd known this, I would've ridden\n"
	.string "my BIKE here.$"

SlateportCity_Text_EveryoneCallsHimCaptStern:
	.string "STERN, the fellow who built the\n"
	.string "MUSEUM, also happens to be the leader\l"
	.string "of an undersea exploration team.\p"
	.string "So, everyone calls him CAPT. STERN.$"

SlateportCity_Text_WhatsLongLineOverThere:
	.string "That crowd won't let anyone\n"
	.string "into the MUSEUM.\p"
	.string "Looking for CAPT. STERN? Ask\n"
	.string "DOCK at the SHIPYARD, south\l"
	.string "of here. He works with STERN.$"

SlateportCity_Text_VisitedMuseumOften:
	.string "When I was a child, I visited\n"
	.string "the MUSEUM often.\p"
	.string "I used to dream about the mysteries of\n"
	.string "the sea after seeing the exhibits.$"

SlateportCity_Text_QuitPushing:
	.string "Hey, there! Quit pushing!\n"
	.string "This is the line, can't you see?$"

SlateportCity_Text_AquaHasPolicy:
	.string "TEAM AQUA has a policy of\n"
	.string "assembling and dispersing at\l"
	.string "the operation site.$"

SlateportCity_Text_BossIsBrilliant:
	.string "Our BOSS is brilliant.\p"
	.string "What would he want to do with\n"
	.string "a MUSEUM now?$"

SlateportCity_Text_WhatsNewSchemeIWonder:
	.string "What's the new scheme, I wonder?\p"
	.string "Our BOSS is scary when he's mad, so\n"
	.string "I'd better not screw things up…$"

SlateportCity_Text_ShouldTakeItAll:
	.string "If there's something we need in\n"
	.string "the MUSEUM, we should take it all!$"

SlateportCity_Text_DontButtIn:
	.string "Hey, you there!\n"
	.string "Don't butt in!$"

SlateportCity_Text_RemindsMeOfLongLineForGames:
	.string "A long line, huh?\p"
	.string "It reminds me of the times I lined up to\n"
	.string "buy smash-hit games…$"

SlateportCity_Text_WhyAreWeLiningUp:
	.string "Why are we even lining up and paying?\n"
	.string "We should just march in!$"

SlateportCity_Text_WhatDoYouWant:
	.string "What?\n"
	.string "What do you want?$"

SlateportCity_Text_IllReadSignForYou:
	.string "You want to read this sign?\n"
	.string "I'll read it for you!$"

SlateportCity_Text_SaysSomethingLikeSeaIsEndless:
	.string "Let's see…\p"
	.string "Um… I think it says something like\n"
	.string "“the life in the sea is endless.”\p"
	.string "Yup, I'm pretty sure that's what\n"
	.string "it says.$"

SlateportCity_Text_ShouldveBroughtMyGameBoy:
	.string "Grumble…\p"
	.string "I should've brought my Game Boy\n"
	.string "Advance so I wouldn't get bored in line…\p"
	.string "Grumble…$"

SlateportCity_Text_HotSpringsAfterOperation:
	.string "When this operation's over, I'll take\n"
	.string "you to a hot spring spa!\p"
	.string "That's what our leader said.\n"
	.string "I can't wait!$"

SlateportCity_Text_SeaIsSoWet:
	.string "The sea is just so vast…\p"
	.string "Could the sea have been made by\n"
	.string "the tears shed by POKéMON?$"

SlateportCity_Text_SinkOldBoats:
	.string "Do you know what they do with old\n"
	.string "ships that become too creaky to sail?\p"
	.string "They sink them in the sea so they\n"
	.string "become habitats for POKéMON.$"

SlateportCity_Text_BuyTooMuch:
	.string "Whenever I visit here, I get carried\n"
	.string "away and buy too much.$"

SlateportCity_Text_GetNameRaterToHelpYou:
	.string "If you want to change your POKéMON's\n"
	.string "nickname, you'll have to get the NAME\l"
	.string "RATER to help you.$"

SlateportCity_Text_CantChangeTradeMonName:
	.string "Any POKéMON you get in a trade,\n"
	.string "you can't change its nickname.\p"
	.string "The original TRAINER's love for that\n"
	.string "POKéMON is in the nickname.$"

SlateportCity_Text_BattleTentBuiltRecently:
	.string "Recently, a BATTLE TENT was built\n"
	.string "in SLATEPORT.\p"
	.string "GYMS are fun, but the BATTLE TENT's\n"
	.string "awesome in its own way.\p"
	.string "You should go find tough POKéMON\n"
	.string "for the BATTLE TENT!$"

SlateportCity_Text_CaptSternBeingInterviewed:
	.string "I was hoping that it was a famous star\n"
	.string "so I could get an autograph.\p"
	.string "But who's that being interviewed?\n"
	.string "Isn't that CAPT. STERN?$"

SlateportCity_Text_InterviewerSoCool:
	.string "That lady interviewer is so cool\n"
	.string "and pretty.\p"
	.string "When I grow up, I'm going to be\n"
	.string "an international journalist!$"

SlateportCity_Text_SternSaysDiscoveredSomething:
	.string "CAPT. STERN says they discovered\n"
	.string "something at the bottom of the sea.\p"
	.string "I wonder what it is?\n"
	.string "What could it be?$"

SlateportCity_Text_CaptainComeBackWithBigFish:
	.string "What's going on here?\p"
	.string "Did the good CAPTAIN come back with\n"
	.string "a big fish from the ocean floor?$"

SlateportCity_Text_AmIOnTV:
	.string "Hey! Are you watching?\n"
	.string "Am I on TV?$"

SlateportCity_Text_CaptainsACelebrity:
	.string "A TV interview! Here!\n"
	.string "The CAPTAIN's a celebrity!$"

SlateportCity_Text_BigSmileForCamera:
	.string "TY: Okay, CAPT. STERN, a big smile\n"
	.string "for the camera!$"

SlateportCity_Text_MostInvaluableExperience:
	.string "GABBY: I see, I see. You've had a most\n"
	.string "invaluable experience…$"

SlateportCity_Text_SternMoveAheadWithExploration:
	.string "CAPT. STERN: Yes, indeed. We intend to\n"
	.string "move ahead with our exploration.$"

SlateportCity_Text_GabbyWonderfulThanksForInterview:
	.string "GABBY: That's wonderful, CAPT. STERN!\n"
	.string "Thank you for taking the time from\l"
	.string "your busy schedule to talk to us.\p"
	.string "We hope we can interview you again\n"
	.string "with news of more discoveries!$"

SlateportCity_Text_SternWhewFirstInterview:
	.string "CAPT. STERN: Whew…\p"
	.string "That was my first time to be filmed for\n"
	.string "TV. That was nerve-wracking.$"

SlateportCity_Text_OhPlayerWeMadeDiscovery:
	.string "Oh! {PLAYER}{KUN}!\n"
	.string "You're looking great!\p"
	.string "We made a huge discovery on our last\n"
	.string "seafloor exploration.\p"
	.string "We found an underwater cavern on\n"
	.string "ROUTE 128.\p"
	.string "We think it's the habitat of a POKéMON\n"
	.string "that's said to have been long extinct.$"

SlateportCity_Text_AquaWillAssumeControlOfSubmarine:
	.string "Fufufu…\n"
	.string "CAPT. STERN, I presume.\p"
	.string "We of TEAM AQUA will assume\n"
	.string "control of your submarine!\p"
	.string "Your objections are meaningless!\n"
	.string "We expect your total cooperation!\p"
	.string "Fufufu…\n"
	.string "Just watch and learn what TEAM\l"
	.string "AQUA has planned!$"

SlateportCity_Text_SternWhatWasAllThat:
	.string "CAPT. STERN: What was that all about?\p"
	.string "It sounded like someone using\n"
	.string "a megaphone…\p"
	.string "Where did it come from?$"

SlateportCity_Text_FromHarborTryingToTakeSub:
	.string "It's from the HARBOR!\p"
	.string "The submarine!\n"
	.string "They're trying to take it!$"

SlateportCity_Text_PleaseComeWithMe:
	.string "{PLAYER}{KUN}!\n"
	.string "Please, come with me!$"

SlateportCity_Text_BattleTentSign:
	.string "BATTLE TENT SLATEPORT SITE\n"
	.string "“Find it! The ultimate POKéMON!”$"

SlateportCity_Text_SternsShipyardWantedSign:
	.string "STERN'S SHIPYARD\p"
	.string "“Wanted: A sailor capable of sailing\n"
	.string "in all currents.”$"

SlateportCity_Text_SternsShipyardNearsCompletion:
	.string "STERN'S SHIPYARD\p"
	.string "“The ferry S.S. TIDAL nears\n"
	.string "completion for serving the ports of\l"
	.string "SLATEPORT and LILYCOVE.”$"

SlateportCity_Text_SternsShipyardFerryComplete:
	.string "STERN'S SHIPYARD\p"
	.string "“Boarding of the SLATEPORT-LILYCOVE\n"
	.string "ferry S.S. TIDAL is handled at the\l"
	.string "HARBOR.”$"

SlateportCity_Text_PokemonFanClubSign:
	.string "POKéMON FAN CLUB\n"
	.string "“Calling all fans of POKéMON!”$"

SlateportCity_Text_OceanicMuseumSign:
	.string "“The endless sea sustains\n"
	.string "all life.”\p"
	.string "OCEANIC MUSEUM$"

SlateportCity_Text_CitySign:
	.string "SLATEPORT CITY\p"
	.string "“The port where people and POKéMON\n"
	.string "cross paths.”$"

SlateportCity_Text_MarketSign:
	.string "SLATEPORT MARKET\n"
	.string "“Specialty goods and local services!”$"

SlateportCity_Text_HarborFerryUnderConstruction:
	.string "SLATEPORT HARBOR\p"
	.string "“The ferry S.S. TIDAL is under\n"
	.string "construction in the SHIPYARD.\p"
	.string "“Service is scheduled to begin\n"
	.string "shortly.”$"

SlateportCity_Text_HarborSign:
	.string "SLATEPORT HARBOR\p"
	.string "“Enjoy a delightful cruise on\n"
	.string "the ferry S.S. TIDAL.”$"

SlateportCity_Text_NameRatersHouseSign:
	.string "NAME RATER'S HOUSE\n"
	.string "“POKéMON nicknames rated.”$"

SlateportCity_Text_HerbalSupplies:
	.string "Fresh herbs and expedition supplies!\n"
	.string "Cash purchases, ready for the road.\p"
	.string "ETHERS restore used move PP.\n"
	.string "Use medicine between battles when\l"
	.string "a POKéMON CENTER is far away.$"

SlateportCity_Text_MarketDirectory:
	.string "SLATEPORT MARKET\p"
	.string "Capture supplies, herbal medicine,\n"
	.string "and decorations for your SECRET BASE.\p"
	.string "Visit the MART for evolution stones\n"
	.string "and special evolution tools.$"

SlateportCity_Text_YouDroveTeamAquaAway:
	.string "SCOTT: Good to see you again!\n"
	.string "We met in PETALBURG.\p"
	.string "I just saw TEAM AQUA rush out.\n"
	.string "Was that your team's doing?$"

SlateportCity_Text_MaybeThisTrainer:
	.string "SCOTT: Handling a surprise like\n"
	.string "that takes more than strong moves.\l"
	.string "You kept your team together.$"

SlateportCity_Text_LetsRegisterEachOther:
	.string "SCOTT: I'd like to hear how your\n"
	.string "journey goes. Let's register\l"
	.string "each other in our POKéNAVS.$"

SlateportCity_Text_RegisteredScott:
	.string "Registered SCOTT\n"
	.string "in the POKéNAV.$"

SlateportCity_Text_KeepEyeOnTrainersBeSeeingYou:
	.string "SCOTT: There are promising\n"
	.string "TRAINERS all over HOENN. I have\l"
	.string "a few more stops to make.\p"
	.string "Keep trying new plans,\n"
	.string "{PLAYER}{KUN}. I'll be watching\l"
	.string "for your next BADGE!$"

SlateportCity_Text_TakingBattleTentChallenge:
	.string "SCOTT: Oh, hey!\n"
	.string "If it isn't {PLAYER}{KUN}!\p"
	.string "{PLAYER}{KUN}, let me guess--you're going\n"
	.string "to take the BATTLE TENT challenge?\p"
	.string "Yeah! That's a good idea!\n"
	.string "That's a really great idea!\p"
	.string "After all, a tough TRAINER is\n"
	.string "the perfect fit for the BATTLE TENT!\p"
	.string "Give it your best effort!$"

SlateportCity_EventScript_Greta::
	lock
	faceplayer
	goto_if_defeated TRAINER_GRETA_SLATEPORT, SlateportCity_EventScript_GretaAfter
	msgbox SlateportCity_Text_GretaChallenge, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SlateportCity_EventScript_GretaDecline
	trainerbattle_no_intro_double TRAINER_GRETA_SLATEPORT, SlateportCity_Text_GretaDefeat, EmeraldChampions_Text_NeedTwoPokemon
SlateportCity_EventScript_GretaAfter::
	msgbox SlateportCity_Text_GretaAfter, MSGBOX_DEFAULT
	release
	end

SlateportCity_EventScript_GretaDecline::
	msgbox SlateportCity_Text_GretaDecline, MSGBOX_DEFAULT
	release
	end

SlateportCity_Text_GretaChallenge:
	.string "I'm GRETA of the BATTLE ARENA.\p"
	.string "A real match reveals mind, skill,\n"
	.string "and body all at once. Show me yours!$"

SlateportCity_Text_GretaDefeat:
	.string "All three were sharp. Nicely done!$"

SlateportCity_Text_GretaDecline:
	.string "Return when mind, skill, and body are\n"
	.string "ready to be judged together.$"

SlateportCity_Text_GretaAfter:
	.string "That was the mark of a contender.\n"
	.string "Bring that spirit to the FRONTIER!$"

@ v4 arrives after Brawly, Steven's Ring and the manor song, at cap 24.
@ The museum chapter raises the cap to 30. This inherited no-Knuckle fallback
@ is dormant on the earned v4 path; it does not define the current story order.
SlateportCity_EventScript_CheckKnuckleGuard::
	goto_if_unset FLAG_BADGE02_GET, SlateportCity_EventScript_CheckKnuckleGuardEnd
	setflag FLAG_HIDE_SLATEPORT_CITY_KNUCKLE_GUARD
	setvar VAR_TEMP_4, 1
SlateportCity_EventScript_CheckKnuckleGuardEnd::
	return

SlateportCity_EventScript_KnuckleGuard::
	lock
	faceplayer
	msgbox SlateportCity_Text_KnuckleGuardClosed, MSGBOX_DEFAULT
	call SlateportCity_EventScript_LocateBrawly
	release
	end

SlateportCity_EventScript_KnuckleGuardTrigger::
	lockall
	msgbox SlateportCity_Text_KnuckleGuardTrigger, MSGBOX_DEFAULT
	call SlateportCity_EventScript_LocateBrawly
	closemessage
	applymovement LOCALID_PLAYER, Common_Movement_WalkDown
	waitmovement 0
	releaseall
	end

SlateportCity_Text_KnuckleGuardClosed:
	.string "ROUTE 110 is closed while the CYCLING\n"
	.string "ROAD crew resurfaces the trail.\p"
	.string "The LEAGUE lets certified TRAINERS\n"
	.string "through. BRAWLY's KNUCKLE BADGE\l"
	.string "counts.$"

SlateportCity_EventScript_LocateBrawly::
	goto_if_set FLAG_HIDE_SLATEPORT_CITY_BRAWLY, SlateportCity_EventScript_BrawlyBackInDewford
	msgbox SlateportCity_Text_BrawlyAtMuseum, MSGBOX_DEFAULT
	return

SlateportCity_EventScript_BrawlyBackInDewford::
	msgbox SlateportCity_Text_BrawlyBackInDewford, MSGBOX_DEFAULT
	return

SlateportCity_Text_BrawlyAtMuseum:
	.string "BRAWLY is in the queue outside the\n"
	.string "OCEANIC MUSEUM. Ask him to go home.\p"
	.string "Then sail back with MR. BRINEY and\n"
	.string "challenge DEWFORD GYM.$"

SlateportCity_Text_BrawlyBackInDewford:
	.string "BRAWLY has returned to DEWFORD GYM.\n"
	.string "Challenge him for the KNUCKLE BADGE.\p"
	.string "MR. BRINEY's boat is on ROUTE 109,\n"
	.string "south of SLATEPORT.$"

SlateportCity_Text_KnuckleGuardTrigger:
	.string "Whoa, not past the cones! ROUTE 110\n"
	.string "is closed until you hold the\l"
	.string "KNUCKLE BADGE.$"
