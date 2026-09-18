.set LOCALID_ATTENDANT_SINGLES, 1
.set LOCALID_ATTENDANT_DOUBLES, 6

BattleFrontier_BattleFactoryLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattleFactoryLobby_OnWarp
	.byte 0

BattleFrontier_BattleFactoryLobby_OnWarp: @ 82583F3
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattleFactoryLobby_EventScript_TurnPlayerNorth
	.2byte 0

BattleFrontier_BattleFactoryLobby_EventScript_TurnPlayerNorth:: @ 82583FD
	setvar VAR_TEMP_1, 1
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	end

@ Emerald Champions: the Battle Factory is retired. src/battle_factory.c and the Factory's own
@ maps are gone from this tree, so the challenge flow that used to live here
@ (challenge-status dispatch, party select, save, Factory) has been removed
@ rather than left calling missing code. The attendants still greet the
@ player and explain that the Factory is closed.

BattleFrontier_BattleFactoryLobby_EventScript_SinglesAttendant::
	lock
	faceplayer
	msgbox BattleFrontier_BattleFactoryLobby_Text_WelcomeForSingleBattle, MSGBOX_DEFAULT
	msgbox BattleFrontier_BattleFactoryLobby_Text_FactoryClosed, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleFactoryLobby_EventScript_DoublesAttendant::
	lock
	faceplayer
	msgbox BattleFrontier_BattleFactoryLobby_Text_WelcomeForDoubleBattle, MSGBOX_DEFAULT
	msgbox BattleFrontier_BattleFactoryLobby_Text_FactoryClosed, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleFactoryLobby_EventScript_ShowSinglesResults::
	lockall
	frontier_results FRONTIER_FACILITY_FACTORY, FRONTIER_MODE_SINGLES
	waitbuttonpress
	special RemoveRecordsWindow
	releaseall
	end

BattleFrontier_BattleFactoryLobby_EventScript_ShowDoublesResults:: @ 82587FA
	lockall
	frontier_results FRONTIER_FACILITY_FACTORY, FRONTIER_MODE_DOUBLES
	waitbuttonpress
	special RemoveRecordsWindow
	releaseall
	end

BattleFrontier_BattleFactoryLobby_EventScript_Woman:: @ 8258813
	msgbox BattleFrontier_BattleFactoryLobby_Text_NeedKnowledgeOfMonsMoves, MSGBOX_NPC
	end

BattleFrontier_BattleFactoryLobby_EventScript_Camper:: @ 825881C
	msgbox BattleFrontier_BattleFactoryLobby_Text_SwappedForWeakMon, MSGBOX_NPC
	end

BattleFrontier_BattleFactoryLobby_EventScript_Picnicker:: @ 8258825
	lock
	msgbox BattleFrontier_BattleFactoryLobby_Text_NeedToCheckOpponentsMons, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleFactoryLobby_EventScript_FatMan:: @ 8258830
	msgbox BattleFrontier_BattleFactoryLobby_Text_CantFigureOutStaffHints, MSGBOX_NPC
	end

BattleFrontier_BattleFactoryLobby_EventScript_RulesBoard:: @ 8258839
	lockall
	msgbox BattleFrontier_BattleFactoryLobby_Text_RulesAreListed, MSGBOX_DEFAULT
	goto BattleFrontier_BattleFactoryLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleFactoryLobby_EventScript_ReadRulesBoard:: @ 8258848
	message BattleFrontier_BattleFactoryLobby_Text_ReadWhichHeading
	waitmessage
	multichoice 17, 0, MULTI_BATTLE_FACTORY_RULES, FALSE
	switch VAR_RESULT
	case 0, BattleFrontier_BattleFactoryLobby_EventScript_RulesBasics
	case 1, BattleFrontier_BattleFactoryLobby_EventScript_RulesSwapPartner
	case 2, BattleFrontier_BattleFactoryLobby_EventScript_RulesSwapNumber
	case 3, BattleFrontier_BattleFactoryLobby_EventScript_RulesSwapNotes
	case 4, BattleFrontier_BattleFactoryLobby_EventScript_RulesOpenLv
	case 5, BattleFrontier_BattleFactoryLobby_EventScript_ExitRules
	case MULTI_B_PRESSED, BattleFrontier_BattleFactoryLobby_EventScript_ExitRules
	end

BattleFrontier_BattleFactoryLobby_EventScript_RulesBasics:: @ 82588A6
	msgbox BattleFrontier_BattleFactoryLobby_Text_ExplainBasicRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleFactoryLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleFactoryLobby_EventScript_RulesSwapPartner:: @ 82588B4
	msgbox BattleFrontier_BattleFactoryLobby_Text_ExplainSwapPartnerRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleFactoryLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleFactoryLobby_EventScript_RulesSwapNumber:: @ 82588C2
	msgbox BattleFrontier_BattleFactoryLobby_Text_ExplainSwapNumberRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleFactoryLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleFactoryLobby_EventScript_RulesSwapNotes:: @ 82588D0
	msgbox BattleFrontier_BattleFactoryLobby_Text_ExplainSwapNotesRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleFactoryLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleFactoryLobby_EventScript_RulesOpenLv:: @ 82588DE
	msgbox BattleFrontier_BattleFactoryLobby_Text_ExplainOpenLvRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleFactoryLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleFactoryLobby_EventScript_ExitRules:: @ 82588EC
	releaseall
	end

BattleFrontier_BattleFactoryLobby_Text_FactoryClosed:
	.string "I am sorry to report that our rental\n"
	.string "Pokémon are not available today.\p"
	.string "The pre-battle room is closed, so no\n"
	.string "challenges can be accepted.\p"
	.string "Please enjoy the rest of the\n"
	.string "BATTLE FRONTIER!$"

BattleFrontier_BattleFactoryLobby_Text_WelcomeForSingleBattle: @ 82588EE
	.string "Where the intelligence of Trainers\n"
	.string "is put to the test!\p"
	.string "Welcome to the BATTLE FACTORY!\p"
	.string "I am your guide to the Battle Swap\n"
	.string "Single Tournament.$"

BattleFrontier_BattleFactoryLobby_Text_TakeSinglesChallenge: @ 825897A
	.string "Would you like to take the Battle\n"
	.string "Swap Single challenge?$"

BattleFrontier_BattleFactoryLobby_Text_ExplainSinglesChallenge: @ 82589B3
	.string "The Battle Swap Single Tournament\n"
	.string "is a SINGLE BATTLE competition using\l"
	.string "only rental Pokémon.\p"
	.string "First, you will be loaned three\n"
	.string "Pokémon specifically for this event.\p"
	.string "Using the supplied rental Pokémon,\n"
	.string "you must conduct a SINGLE BATTLE.\p"
	.string "If you win, you are permitted to trade\n"
	.string "one of your rental Pokémon.\p"
	.string "Repeat this cycle of battling and\n"
	.string "trading--if you win seven times in\l"
	.string "a row, you will earn Battle Points.\p"
	.string "If you want to interrupt your\n"
	.string "challenge, please save the game.\p"
	.string "If you don't save, you will be\n"
	.string "disqualified from your challenge.$"

BattleFrontier_BattleFactoryLobby_Text_LookForwardToNextVisit: @ 8258BC5
	.string "We look forward to your next visit.$"

BattleFrontier_BattleFactoryLobby_Text_WhichLevelMode: @ 8258BE9
	.string "Which level do you wish to challenge?\n"
	.string "Level 50 or Open Level?$"

BattleFrontier_BattleFactoryLobby_Text_OkayToSaveBeforeChallenge: @ 8258C27
	.string "Before you begin your challenge,\n"
	.string "I need to save the game. Is that okay?$"

@ Unused
BattleFrontier_BattleFactoryLobby_Text_WillHoldMonsForSafekeeping: @ 8258C6F
	.string "Okay, I will hold your Pokémon for\n"
	.string "safekeeping while you compete.$"

BattleFrontier_BattleFactoryLobby_Text_StepThisWay: @ 8258CB1
	.string "Please step this way.$"

BattleFrontier_BattleFactoryLobby_Text_ReturnMonsSaveResults: @ 8258CC7
	.string "Thank you for participating!\p"
	.string "I will return your Pokémon in exchange\n"
	.string "for our rental Pokémon.\p"
	.string "I must also save the event results.\n"
	.string "Please wait.$"

@ Unused
BattleFrontier_BattleFactoryLobby_Text_ReturnMons: @ 8258D54
	.string "I will return your Pokémon in exchange\n"
	.string "for our rental Pokémon.$"

BattleFrontier_BattleFactoryLobby_Text_CongratsSevenWins: @ 8258D93
	.string "Congratulations! You've won seven\n"
	.string "straight Battle Swap matches!$"

@ Unused
BattleFrontier_BattleFactoryLobby_Text_AwardBattlePointsForStreak: @ 8258DD3
	.string "In recognition of your 7-win streak,\n"
	.string "we award you these Battle Point(s).$"

@ Unused
BattleFrontier_BattleFactoryLobby_Text_MaxBattlePoints: @ 8258E1C
	.string "Oh, oh, oh!\p"
	.string "Your Battle Points are maxed.\p"
	.string "Please come back after using\n"
	.string "some Battle Points.$"

BattleFrontier_BattleFactoryLobby_Text_WaitingForYouToResume: @ 8258E77
	.string "We've been waiting for you!\p"
	.string "Before we resume your challenge,\n"
	.string "I must save the game.$"

BattleFrontier_BattleFactoryLobby_Text_DidntSaveBeforeQuitting: @ 8258ECA
	.string "I'm sorry to say this, but you didn't\n"
	.string "save before you quit playing last time.\p"
	.string "As a result, you have been disqualified\n"
	.string "from your challenge.$"

@ Unused
BattleFrontier_BattleFactoryLobby_Text_WellReturnMons: @ 8258F55
	.string "We'll return your personal Pokémon.$"

@ Unused
BattleFrontier_BattleFactoryLobby_Text_ReceivedPrizeItem: @ 8258F79
	.string "{PLAYER} received the prize\n"
	.string "{STR_VAR_1}.$"

BattleFrontier_BattleFactoryLobby_Text_WelcomeForDoubleBattle: @ 8258F93
	.string "Where the intelligence of Trainers\n"
	.string "is put to the test!\p"
	.string "Welcome to the BATTLE FACTORY!\p"
	.string "I am your guide to the Battle Swap\n"
	.string "Double Tournament.$"

BattleFrontier_BattleFactoryLobby_Text_TakeDoublesChallenge: @ 825901F
	.string "Would you like to take the Battle\n"
	.string "Swap Double challenge?$"

BattleFrontier_BattleFactoryLobby_Text_ExplainDoublesChallenge: @ 8259058
	.string "The Battle Swap Double Tournament\n"
	.string "is a Double Battle competition using\l"
	.string "only rental Pokémon.\p"
	.string "First, you will be loaned three\n"
	.string "Pokémon specifically for this event.\p"
	.string "Using the supplied rental Pokémon,\n"
	.string "you must conduct a Double Battle.\p"
	.string "If you win, you are permitted to trade\n"
	.string "one of your rental Pokémon.\p"
	.string "Repeat this cycle of battling and\n"
	.string "trading--if you win seven times in\l"
	.string "a row, you will earn Battle Points.\p"
	.string "If you want to interrupt your\n"
	.string "challenge, please save the game.\p"
	.string "If you don't save before interrupting,\n"
	.string "you will be disqualified.$"

BattleFrontier_BattleFactoryLobby_Text_CongratsForDefeatingHead: @ 825926A
	.string "Congratulations for defeating\n"
	.string "the FACTORY HEAD and winning\l"
	.string "seven matches in a row!$"

BattleFrontier_BattleFactoryLobby_Text_AwardBattlePoints: @ 82592BD
	.string "In recognition of your wealth of\n"
	.string "knowledge and keen intelligence,\l"
	.string "we award you these Battle Point(s).$"

BattleFrontier_BattleFactoryLobby_Text_ExchangeMonsAndSave: @ 8259323
	.string "Let me exchange your Pokémon\n"
	.string "for our rental Pokémon.\p"
	.string "I need to save the battle data,\n"
	.string "so please wait.$"

BattleFrontier_BattleFactoryLobby_Text_RecordLastMatch: @ 8259388
	.string "Would you like to record your last\n"
	.string "BATTLE FACTORY match on your\l"
	.string "FRONTIER PASS?$"

BattleFrontier_BattleFactoryLobby_Text_NeedKnowledgeOfMonsMoves: @ 82593D7
	.string "Hi!\n"
	.string "You, there!\p"
	.string "Are you thinking that the events here\n"
	.string "are easy since you don't need to have\l"
	.string "a raised team of Pokémon?\p"
	.string "I wouldn't be too sure about winning\n"
	.string "that easily.\p"
	.string "If you don't have thorough knowledge\n"
	.string "about Pokémon and their moves,\l"
	.string "it will be tough to keep winning.$"

BattleFrontier_BattleFactoryLobby_Text_SwappedForWeakMon: @ 82594E5
	.string "I swapped for a weak Pokémon…\n"
	.string "I thought it was a good kind to have…\p"
	.string "They wiped the floor with us…$"

BattleFrontier_BattleFactoryLobby_Text_NeedToCheckOpponentsMons: @ 8259547
	.string "Things haven't been going my way\n"
	.string "at all.\p"
	.string "You need to check your opponent's\n"
	.string "Pokémon during battle to see if\l"
	.string "they're any good.$"

BattleFrontier_BattleFactoryLobby_Text_CantFigureOutStaffHints: @ 82595C4
	.string "You know how the staff here give you\n"
	.string "a few hints about your next opponent?\p"
	.string "Well, I'm a full-grown man, but I have\n"
	.string "trouble figuring out their hints.$"

@ Unused
BattleFrontier_BattleFactoryLobby_Text_RentalMonsAreVaried: @ 8259658
	.string "Like, I'm really tough, but I get bored\n"
	.string "really easily, so I just kept swapping\l"
	.string "and battling over and over.\p"
	.string "So anyway, when I kept entering\n"
	.string "Battle Swap events, I noticed they\l"
	.string "varied the rental Pokémon.$"

BattleFrontier_BattleFactoryLobby_Text_RulesAreListed: @ 8259721
	.string "The Battle Swap rules are listed.$"

BattleFrontier_BattleFactoryLobby_Text_ReadWhichHeading: @ 8259743
	.string "Which heading do you want to read?$"

BattleFrontier_BattleFactoryLobby_Text_ExplainBasicRules: @ 8259766
	.string "In a Battle Swap event, you may use\n"
	.string "only three Pokémon.\p"
	.string "Whether you are renting or swapping,\n"
	.string "your team may not have two or more\l"
	.string "of the same Pokémon.$"

BattleFrontier_BattleFactoryLobby_Text_ExplainSwapPartnerRules: @ 82597FB
	.string "You may swap Pokémon only with\n"
	.string "the Trainer you have just defeated.\p"
	.string "You may swap for only those Pokémon\n"
	.string "used by the beaten Trainer.$"

BattleFrontier_BattleFactoryLobby_Text_ExplainSwapNumberRules: @ 825987E
	.string "After every battle you win, you may\n"
	.string "swap for one of your defeated\l"
	.string "opponent's Pokémon.\p"
	.string "You will not be able to swap Pokémon\n"
	.string "with the seventh Trainer in the event.$"

BattleFrontier_BattleFactoryLobby_Text_ExplainSwapNotesRules: @ 8259920
	.string "There are two key points to be aware\n"
	.string "of when swapping Pokémon.\p"
	.string "First, when swapping, you can't check\n"
	.string "the stats of the Pokémon you are\l"
	.string "about to receive.\p"
	.string "Second, the Pokémon on your team\n"
	.string "are lined up in sequence, depending on\l"
	.string "the order in which you rented them.\p"
	.string "This sequence remains unchanged\n"
	.string "even when swaps are made.$"

BattleFrontier_BattleFactoryLobby_Text_ExplainOpenLvRules: @ 8259A5E
	.string "In the Open Level, the rental Pokémon\n"
	.string "and the opposing TRAINERS' Pokémon\l"
	.string "are all Level 100.$"
