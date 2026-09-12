SlateportCity_Harbor_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SlateportCity_Harbor_OnTransition
	.byte 0

SlateportCity_Harbor_OnTransition:
	setescapewarp MAP_SLATEPORT_CITY, 28, 13
	setvar VAR_TEMP_1, 0
	goto_if_eq VAR_SLATEPORT_HARBOR_STATE, 1, SlateportCity_Harbor_EventScript_ReadyAquaEscapeScene
	call_if_set FLAG_BADGE06_GET, SlateportCity_Harbor_EventScript_ShowSSTidal
	end

SlateportCity_Harbor_EventScript_ShowSSTidal::
	clearflag FLAG_HIDE_SLATEPORT_CITY_HARBOR_SS_TIDAL
	clearflag FLAG_HIDE_SLATEPORT_CITY_HARBOR_PATRONS
	return

SlateportCity_Harbor_EventScript_ReadyAquaEscapeScene::
	savebgm MUS_ENCOUNTER_AQUA
	setobjectxyperm LOCALID_SLATEPORT_HARBOR_CAPT_STERN, 12, 13
	setobjectmovementtype LOCALID_SLATEPORT_HARBOR_CAPT_STERN, MOVEMENT_TYPE_FACE_LEFT
	setflag FLAG_HIDE_SLATEPORT_CITY_HARBOR_PATRONS
	setflag FLAG_HIDE_SLATEPORT_CITY_HARBOR_SS_TIDAL
	end

SlateportCity_Harbor_EventScript_AquaEscapeTrigger0::
	lockall
	setvar VAR_0x8008, 0
	goto SlateportCity_Harbor_EventScript_AquaEscapeScene
	end

SlateportCity_Harbor_EventScript_AquaEscapeTrigger1::
	lockall
	setvar VAR_0x8008, 1
	goto SlateportCity_Harbor_EventScript_AquaEscapeScene
	end

SlateportCity_Harbor_EventScript_AquaEscapeTrigger2::
	lockall
	setvar VAR_0x8008, 2
	goto SlateportCity_Harbor_EventScript_AquaEscapeScene
	end

SlateportCity_Harbor_EventScript_AquaEscapeTrigger3::
	lockall
	setvar VAR_0x8008, 3
	applymovement LOCALID_PLAYER, SlateportCity_Harbor_Movement_PlayerWalkUp
	waitmovement 0
	goto SlateportCity_Harbor_EventScript_AquaEscapeScene
	end

SlateportCity_Harbor_EventScript_AquaEscapeScene::
	applymovement LOCALID_SLATEPORT_HARBOR_ARCHIE, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	applymovement LOCALID_SLATEPORT_HARBOR_GRUNT, Common_Movement_WalkInPlaceFasterDown
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox SlateportCity_Harbor_Text_ArchieYouAgainHideoutInLilycove, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SLATEPORT_HARBOR_GRUNT, SlateportCity_Harbor_Movement_AquaBoardSub
	applymovement LOCALID_SLATEPORT_HARBOR_ARCHIE, SlateportCity_Harbor_Movement_AquaBoardSub
	applymovement LOCALID_SLATEPORT_HARBOR_SUBMARINE, SlateportCity_Harbor_Movement_SubmarineExit
	waitmovement 0
	removeobject LOCALID_SLATEPORT_HARBOR_GRUNT
	removeobject LOCALID_SLATEPORT_HARBOR_ARCHIE
	removeobject LOCALID_SLATEPORT_HARBOR_SUBMARINE
	setvar VAR_SLATEPORT_HARBOR_STATE, 2
	setflag FLAG_MET_TEAM_AQUA_HARBOR
	setflag FLAG_HIDE_LILYCOVE_MOTEL_SCOTT
	call_if_eq VAR_0x8008, 0, SlateportCity_Harbor_EventScript_SternApproachPlayer0
	call_if_eq VAR_0x8008, 1, SlateportCity_Harbor_EventScript_SternApproachPlayer1
	call_if_eq VAR_0x8008, 2, SlateportCity_Harbor_EventScript_SternApproachPlayer
	call_if_eq VAR_0x8008, 3, SlateportCity_Harbor_EventScript_SternApproachPlayer
	msgbox SlateportCity_Harbor_Text_CaptSternWhyStealMySubmarine, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_HIDE_AQUA_HIDEOUT_1F_GRUNT_1_BLOCKING_ENTRANCE
	setflag FLAG_HIDE_AQUA_HIDEOUT_1F_GRUNT_2_BLOCKING_ENTRANCE
	copyobjectxytoperm LOCALID_SLATEPORT_HARBOR_CAPT_STERN
	setobjectmovementtype LOCALID_SLATEPORT_HARBOR_CAPT_STERN, MOVEMENT_TYPE_FACE_RIGHT
	releaseall
	end

SlateportCity_Harbor_EventScript_SternApproachPlayer0::
	applymovement LOCALID_SLATEPORT_HARBOR_CAPT_STERN, SlateportCity_Harbor_Movement_SternApproachPlayer0
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	return

SlateportCity_Harbor_EventScript_SternApproachPlayer1::
	applymovement LOCALID_SLATEPORT_HARBOR_CAPT_STERN, SlateportCity_Harbor_Movement_SternApproachPlayer1
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	return

SlateportCity_Harbor_EventScript_SternApproachPlayer::
	applymovement LOCALID_SLATEPORT_HARBOR_CAPT_STERN, SlateportCity_Harbor_Movement_SternApproachPlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	return

SlateportCity_Harbor_Movement_AquaBoardSub:
	delay_16
	delay_16
	jump_up
	set_invisible
	step_end

@ Unused, Archie instead shares above identical movement script with Grunt
SlateportCity_Harbor_Movement_ArchieBoardSub:
	delay_16
	delay_16
	jump_up
	set_invisible
	step_end

SlateportCity_Harbor_Movement_SubmarineExit:
	delay_16
	delay_16
	delay_16
	delay_16
	walk_right
	walk_right
	walk_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

SlateportCity_Harbor_Movement_SternApproachPlayer0:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	step_end

SlateportCity_Harbor_Movement_SternApproachPlayer1:
	walk_left
	walk_left
	walk_up
	walk_left
	step_end

SlateportCity_Harbor_Movement_SternApproachPlayer:
	walk_left
	walk_left
	walk_left
	step_end

SlateportCity_Harbor_Movement_PlayerWalkUp:
	walk_up
	step_end

SlateportCity_Harbor_EventScript_FerryAttendant::
	lock
	faceplayer
	goto_if_unset FLAG_BADGE06_GET, SlateportCity_Harbor_EventScript_FerryUnavailable
	goto_if_unset FLAG_EC_EARNED_SS_TICKET, SlateportCity_Harbor_EventScript_NoTicket
	goto SlateportCity_Harbor_EventScript_AskForTicket

SlateportCity_Harbor_EventScript_FerryUnavailable::
	msgbox SlateportCity_Harbor_Text_FerryServiceUnavailable, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_AskForTicket::
	msgbox SlateportCity_Harbor_Text_MayISeeYourTicket, MSGBOX_DEFAULT
	message SlateportCity_Harbor_Text_ConfirmedPassWhereTo
	waitmessage
	goto SlateportCity_Harbor_EventScript_ChooseDestination
	end

SlateportCity_Harbor_EventScript_ChooseDestination::
	goto_if_set FLAG_BADGE06_GET, SlateportCity_Harbor_EventScript_ChooseDestinationWithBattleFrontier
	multichoicedefault 18, 8, MULTI_SSTIDAL_SLATEPORT_NO_BF, 2, FALSE
	switch VAR_RESULT
	case 0, SlateportCity_Harbor_EventScript_Lilycove
	case 1, SlateportCity_Harbor_EventScript_CancelDestinationSelect
	case MULTI_B_PRESSED, SlateportCity_Harbor_EventScript_CancelDestinationSelect
	end

SlateportCity_Harbor_EventScript_ChooseDestinationWithBattleFrontier::
	multichoicedefault 17, 6, MULTI_SSTIDAL_SLATEPORT_WITH_BF, 2, FALSE
	switch VAR_RESULT
	case 0, SlateportCity_Harbor_EventScript_Lilycove
	case 1, SlateportCity_Harbor_EventScript_BattleFrontier
	case 2, SlateportCity_Harbor_EventScript_CancelDestinationSelect
	case MULTI_B_PRESSED, SlateportCity_Harbor_EventScript_CancelDestinationSelect
	end

SlateportCity_Harbor_EventScript_NoTicket::
	msgbox SlateportCity_Harbor_Text_YouMustHaveTicket, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_Lilycove::
	msgbox SlateportCity_Harbor_Text_LilycoveItIs, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SlateportCity_Harbor_EventScript_ChooseNewDestination
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_BOARD_SLATEPORT
	call SlateportCity_Harbor_EventScript_BoardFerry
	warp MAP_SS_TIDAL_CORRIDOR, 1, 10
	waitstate
	release
	end

SlateportCity_Harbor_EventScript_BattleFrontier::
	msgbox SlateportCity_Harbor_Text_BattleFrontierItIs, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SlateportCity_Harbor_EventScript_ChooseNewDestination
	call SlateportCity_Harbor_EventScript_BoardFerry
	warp MAP_BATTLE_FRONTIER_OUTSIDE_WEST, 19, 67
	waitstate
	release
	end

SlateportCity_Harbor_EventScript_ChooseNewDestination::
	message SlateportCity_Harbor_Text_WhereWouldYouLikeToGo
	waitmessage
	goto SlateportCity_Harbor_EventScript_ChooseDestination
	end

SlateportCity_Harbor_EventScript_BoardFerry::
	msgbox SlateportCity_Harbor_Text_PleaseBoardFerry, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	delay 30
	hideobjectat VAR_LAST_TALKED, MAP_SLATEPORT_CITY_HARBOR
	call_if_eq VAR_FACING, DIR_NORTH, SlateportCity_Harbor_EventScript_BoardFerryNorth
	call_if_eq VAR_FACING, DIR_EAST, SlateportCity_Harbor_EventScript_BoardFerryEast
	delay 30
	hideplayer
	setvar VAR_0x8004, LOCALID_SLATEPORT_HARBOR_SS_TIDAL
	call Common_EventScript_FerryDepart
	return

SlateportCity_Harbor_EventScript_CancelDestinationSelect::
	msgbox SlateportCity_Harbor_Text_SailAnotherTime, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_BoardFerryEast::
	applymovement LOCALID_PLAYER, SlateportCity_Harbor_Movement_BoardFerryEast
	waitmovement 0
	return

SlateportCity_Harbor_EventScript_BoardFerryNorth::
	applymovement LOCALID_PLAYER, SlateportCity_Harbor_Movement_BoardFerryNorth
	waitmovement 0
	return

SlateportCity_Harbor_Movement_BoardFerryEast:
	walk_right
	walk_in_place_faster_up
	step_end

SlateportCity_Harbor_Movement_BoardFerryNorth:
	walk_up
	step_end

SlateportCity_Harbor_EventScript_Sailor::
	lock
	faceplayer
	goto_if_unset FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, SlateportCity_Harbor_EventScript_SailorNoAbnormalWeather
	setvar VAR_0x8004, 0
	call_if_set FLAG_DEFEATED_KYOGRE, SlateportCity_Harbor_EventScript_CountDefeatedLegendary
	call_if_set FLAG_DEFEATED_GROUDON, SlateportCity_Harbor_EventScript_CountDefeatedLegendary
	goto_if_eq VAR_0x8004, 2, SlateportCity_Harbor_EventScript_SailorNoAbnormalWeather  @ Defeated both
	msgbox SlateportCity_Harbor_Text_AbnormalWeather, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_SailorNoAbnormalWeather::
	msgbox SlateportCity_Harbor_Text_LoveToGoDeepUnderwaterSomeday, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_CountDefeatedLegendary::
	addvar VAR_0x8004, 1
	return

SlateportCity_Harbor_EventScript_FatMan::
	msgbox SlateportCity_Harbor_Text_SubTooSmallForMe, MSGBOX_NPC
	end

SlateportCity_Harbor_EventScript_CaptStern::
	lock
	faceplayer
	goto_if_set FLAG_BADGE07_GET, SlateportCity_Harbor_EventScript_CaptSternFerryOrScannerComment
	goto_if_set FLAG_EVIL_TEAM_ESCAPED_STERN_SPOKE, SlateportCity_Harbor_EventScript_NeedDive
	goto_if_set FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, SlateportCity_Harbor_EventScript_TeamAquaLeftNeedDive
	goto_if_eq VAR_SLATEPORT_HARBOR_STATE, 2, SlateportCity_Harbor_EventScript_WhyStealSubmarine
	msgbox SlateportCity_Harbor_Text_SameThugsTriedToRobAtMuseum, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

SlateportCity_Harbor_EventScript_WhyStealSubmarine::
	msgbox SlateportCity_Harbor_Text_CaptSternWhyStealMySubmarine, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_TeamAquaLeftNeedDive::
	setflag FLAG_EVIL_TEAM_ESCAPED_STERN_SPOKE
	msgbox SlateportCity_Harbor_Text_TeamAquaLeftNeedDive, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_NeedDive::
	msgbox SlateportCity_Harbor_Text_NeedDiveToCatchSub, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_CaptSternFerryOrScannerComment::
	goto_if_set FLAG_EXCHANGED_SCANNER, SlateportCity_Harbor_EventScript_TradedScanner
	goto_if_set FLAG_EC_REPORT_C44_COMPLETE, SlateportCity_Harbor_EventScript_ResumeScannerReward
	checkitem ITEM_SCANNER
	goto_if_eq VAR_RESULT, TRUE, SlateportCity_Harbor_EventScript_AskToTradeScanner
	goto_if_set FLAG_BADGE06_GET, SlateportCity_Harbor_EventScript_FerryFinished
	msgbox SlateportCity_Harbor_Text_WontBeLongBeforeWeFinishFerry, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_FerryFinished::
	msgbox SlateportCity_Harbor_Text_FinishedMakingFerry, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_AskToTradeScanner::
	completemilestone FLAG_EC_REPORT_C44_COMPLETE
	removeitem ITEM_SCANNER
	msgbox SlateportCity_Harbor_Text_HandedScannerToStern, MSGBOX_DEFAULT
	msgbox SlateportCity_Harbor_Text_ScannerReport, MSGBOX_DEFAULT
SlateportCity_Harbor_EventScript_ResumeScannerReward::
	goto_if_unset FLAG_EC_SCANNER_REWARD_SELECTED, SlateportCity_Harbor_EventScript_ChooseDifferentTrade
	goto_if_set FLAG_EC_SCANNER_REWARD_SCALE, SlateportCity_Harbor_EventScript_DeliverScale
	goto SlateportCity_Harbor_EventScript_DeliverTooth
	end

SlateportCity_Harbor_EventScript_ChooseScannerTrade::
	multichoice 0, 0, MULTI_STERN_DEEPSEA, FALSE
	switch VAR_RESULT
	case 0, SlateportCity_Harbor_EventScript_DeepSeaTooth
	case 1, SlateportCity_Harbor_EventScript_DeepSeaScale
	case 2, SlateportCity_Harbor_EventScript_DeclineTrade
	case MULTI_B_PRESSED, SlateportCity_Harbor_EventScript_DeclineTrade
	end

SlateportCity_Harbor_EventScript_DeepSeaTooth::
	msgbox SlateportCity_Harbor_Text_TradeForDeepSeaTooth, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SlateportCity_Harbor_EventScript_ChooseDifferentTrade
	setflag FLAG_EC_SCANNER_REWARD_SELECTED
SlateportCity_Harbor_EventScript_DeliverTooth::
	giveitem ITEM_DEEP_SEA_TOOTH
	goto SlateportCity_Harbor_EventScript_ScannerRewardDelivered
	end

SlateportCity_Harbor_EventScript_DeepSeaScale::
	msgbox SlateportCity_Harbor_Text_TradeForDeepSeaScale, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SlateportCity_Harbor_EventScript_ChooseDifferentTrade
	setflag FLAG_EC_SCANNER_REWARD_SELECTED
	setflag FLAG_EC_SCANNER_REWARD_SCALE
SlateportCity_Harbor_EventScript_DeliverScale::
	giveitem ITEM_DEEP_SEA_SCALE
SlateportCity_Harbor_EventScript_ScannerRewardDelivered::
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EXCHANGED_SCANNER
	goto SlateportCity_Harbor_EventScript_TradedScanner
	end

SlateportCity_Harbor_EventScript_DeclineTrade::
	msgbox SlateportCity_Harbor_Text_IfYouWantToTradeLetMeKnow, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_EventScript_ChooseDifferentTrade::
	message SlateportCity_Harbor_Text_WhichOneDoYouWant
	waitmessage
	goto SlateportCity_Harbor_EventScript_ChooseScannerTrade
	end

SlateportCity_Harbor_EventScript_TradedScanner::
	msgbox SlateportCity_Harbor_Text_ThisWillHelpResearch, MSGBOX_DEFAULT
	release
	end

SlateportCity_Harbor_Text_FerryServiceUnavailable:
	.string "The ferry opens to TRAINERS with\n"
	.string "six BADGES. Come back with the\l"
	.string "FEATHER BADGE!$"

SlateportCity_Harbor_Text_MayISeeYourTicket:
	.string "Hello, are you here for the ferry?\n"
	.string "May I confirm your ferry pass?$"

SlateportCity_Harbor_Text_YouMustHaveTicket:
	.string "You'll need a ferry pass. With\n"
	.string "six BADGES, speak to the sailor\l"
	.string "at LILYCOVE HARBOR. He can\l"
	.string "register yours.$"

SlateportCity_Harbor_Text_ConfirmedPassWhereTo:
	.string "The attendant confirmed your ferry\n"
	.string "pass.\p"
	.string "Perfect! That's all you need!\p"
	.string "And where would you like to go?$"

SlateportCity_Harbor_Text_SailAnotherTime:
	.string "Please sail with us another time!$"

SlateportCity_Harbor_Text_LilycoveItIs:
	.string "LILYCOVE CITY it is, then!$"

SlateportCity_Harbor_Text_BattleFrontierItIs:
	.string "BATTLE FRONTIER it is, then!$"

SlateportCity_Harbor_Text_PleaseBoardFerry:
	.string "Please board the ferry and wait for\n"
	.string "departure.$"

SlateportCity_Harbor_Text_WhereWouldYouLikeToGo:
	.string "Then, where would you like to go?$"

SlateportCity_Harbor_Text_LoveToGoDeepUnderwaterSomeday:
	.string "A journey to the bottom of the sea…\n"
	.string "I wonder what it'd be like?\p"
	.string "I'd love to go deep underwater like\n"
	.string "that someday.$"

SlateportCity_Harbor_Text_AbnormalWeather:
	.string "For a ship to sail safely, we need to\n"
	.string "know about the weather!\p"
	.string "Speaking of weather, I heard something\n"
	.string "from a guy at the WEATHER INSTITUTE.\p"
	.string "He was saying abnormal weather has\n"
	.string "been reported all over the place!\p"
	.string "You should visit the WEATHER INSTITUTE\n"
	.string "and ask around!$"

SlateportCity_Harbor_Text_SubTooSmallForMe:
	.string "I wanted to go with CAPT. STERN on\n"
	.string "the ocean floor exploration.\p"
	.string "But the sub's too small for me.\p"
	.string "If I squeezed in, there wouldn't be\n"
	.string "any room for the CAPTAIN…$"

SlateportCity_Harbor_Text_SameThugsTriedToRobAtMuseum:
	.string "CAPT. STERN: Those thugs…\p"
	.string "They're the same lot who tried to rob\n"
	.string "the DEVON GOODS at the MUSEUM.$"

SlateportCity_Harbor_Text_ArchieYouAgainHideoutInLilycove:
	.string "ARCHIE: Oh?\n"
	.string "Not you again…\p"
	.string "You are tenacious to track us here,\n"
	.string "that much I will give you.\p"
	.string "But now…\n"
	.string "No one can stop us! No one!\p"
	.string "Or, will you follow us back to our\n"
	.string "HIDEOUT in LILYCOVE CITY?\p"
	.string "Fwahahahaha…$"

SlateportCity_Harbor_Text_CaptSternWhyStealMySubmarine:
	.string "CAPT. STERN: They have my submarine.\n"
	.string "The cavern charts are aboard, too.\p"
	.string "ARCHIE named LILYCOVE. Reach the city\n"
	.string "by heading east from ROUTE 121.\p"
	.string "Find LILYCOVE's eastern beach. SURF\n"
	.string "north to TEAM AQUA's cave hideout.\p"
	.string "Stop their launch before they reach\n"
	.string "the sleeping POKéMON on the seafloor.$"

SlateportCity_Harbor_Text_TeamAquaLeftNeedDive:
	.string "CAPT. STERN: AQUA left before you\n"
	.string "could stop them... Don't blame\l"
	.string "yourself.\p"
	.string "To follow that submarine, you'll\n"
	.string "need DIVE registered, the MIND\l"
	.string "BADGE, and a partner able to use\l"
	.string "the technique.\p"
	.string "Travel to MOSSDEEP. Its divers and\n"
	.string "the SPACE CENTER may be able to\l"
	.string "help.$"
SlateportCity_Harbor_Text_NeedDiveToCatchSub:
	.string "CAPT. STERN: To follow the\n"
	.string "submarine, you'll need DIVE\l"
	.string "registered, the MIND BADGE, and a\l"
	.string "compatible partner.\p"
	.string "Visit MOSSDEEP's SPACE CENTER and\n"
	.string "find STEVEN. He may be able to\l"
	.string "help.$"
SlateportCity_Harbor_Text_WontBeLongBeforeWeFinishFerry:
	.string "CAPT. STERN: Oh, yes.\n"
	.string "MR. BRINEY came to our SHIPYARD.\p"
	.string "It won't be long now before we finish\n"
	.string "making the ferry!$"

SlateportCity_Harbor_Text_FinishedMakingFerry:
	.string "CAPT. STERN: {PLAYER}{KUN}!\p"
	.string "We finally finished making the ferry\n"
	.string "S.S. TIDAL!\p"
	.string "We couldn't have done it without\n"
	.string "your friend MR. BRINEY.\p"
	.string "Please go for a short cruise on it!$"

SlateportCity_Harbor_Text_ScannerReport:
	.string "This SCANNER follows a signal\n"
	.string "through the seabed. We can find\l"
	.string "where the submarine stopped.\p"
	.string "There! ROUTE 128. Mark that inlet.\n"
	.string "You'll need DIVE to follow it.\p"
	.string "Your report earns ¥12,000.\n"
	.string "Choose one of my deep-sea finds\l"
	.string "as a further thank-you.$"

SlateportCity_Harbor_Text_IfYouWantToTradeLetMeKnow:
	.string "Your report is already recorded.\n"
	.string "Come back whenever you're ready\l"
	.string "to choose your reward.$"

SlateportCity_Harbor_Text_TradeForDeepSeaTooth:
	.string "Choose my DEEPSEATOOTH as your\n"
	.string "research reward?$"

SlateportCity_Harbor_Text_TradeForDeepSeaScale:
	.string "Choose my DEEPSEASCALE as your\n"
	.string "research reward?$"

SlateportCity_Harbor_Text_WhichOneDoYouWant:
	.string "CAPT. STERN: Which one do you want?$"

SlateportCity_Harbor_Text_HandedScannerToStern:
	.string "{PLAYER} handed the SCANNER to\n"
	.string "CAPT. STERN.$"

SlateportCity_Harbor_Text_ThisWillHelpResearch:
	.string "CAPT. STERN: Thanks, {PLAYER}{KUN}!\n"
	.string "The signal ends on ROUTE 128.\l"
	.string "DIVE at the inlet to follow it.$"
