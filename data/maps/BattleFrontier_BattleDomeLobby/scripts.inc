.set LOCALID_ATTENDANT_SINGLES, 1
.set LOCALID_ATTENDANT_DOUBLES, 6

@ Emerald Champions: the Battle Dome is retired. src/battle_dome.c and the Dome's
@ own maps are gone from this tree, so the tournament flow that used to live here
@ (results tree, challenge-status dispatch, party select, save, walk to the Dome
@ corridor) has been removed rather than left calling missing code. The attendants
@ still greet the player and explain that the Dome is closed.
@ Note: BattleFrontier_BattlePyramidLobby's map scripts reference the OnWarp below,
@ so it must keep its name.

BattleFrontier_BattleDomeLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattleDomeLobby_OnWarp
	.byte 0

BattleFrontier_BattleDomeLobby_OnWarp:
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattleDomeLobby_EventScript_TurnPlayerNorth
	.2byte 0

BattleFrontier_BattleDomeLobby_EventScript_TurnPlayerNorth:: @ 8249805
	setvar VAR_TEMP_1, 1
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	end

BattleFrontier_BattleDomeLobby_EventScript_SinglesAttendant::
	lock
	faceplayer
	msgbox BattleFrontier_BattleDomeLobby_Text_WelcomeSingleBattle, MSGBOX_DEFAULT
	msgbox BattleFrontier_BattleDomeLobby_Text_DomeClosed, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleDomeLobby_EventScript_DoublesAttendant::
	lock
	faceplayer
	msgbox BattleFrontier_BattleDomeLobby_Text_WelcomeDoubleBattle, MSGBOX_DEFAULT
	msgbox BattleFrontier_BattleDomeLobby_Text_DomeClosed, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleDomeLobby_EventScript_ShowSinglesResults::
	lockall
	frontier_results FRONTIER_FACILITY_DOME, FRONTIER_MODE_SINGLES
	waitbuttonpress
	special RemoveRecordsWindow
	releaseall
	end

BattleFrontier_BattleDomeLobby_EventScript_ShowDoublesResults:: @ 8249D6B
	lockall
	frontier_results FRONTIER_FACILITY_DOME, FRONTIER_MODE_DOUBLES
	waitbuttonpress
	special RemoveRecordsWindow
	releaseall
	end

BattleFrontier_BattleDomeLobby_EventScript_ShowPrevTourneyTree::
	lockall
	msgbox BattleFrontier_BattleDomeLobby_Text_NoTourneyRecords, MSGBOX_DEFAULT
	releaseall
	end

BattleFrontier_BattleDomeLobby_EventScript_Maniac::
	msgbox BattleFrontier_BattleDomeLobby_Text_ManiacNoTourney, MSGBOX_NPC
	end

BattleFrontier_BattleDomeLobby_EventScript_Lass::
	msgbox BattleFrontier_BattleDomeLobby_Text_WinnersGainReputation, MSGBOX_NPC
	end

BattleFrontier_BattleDomeLobby_EventScript_FatMan:: @ 8249E07
	msgbox BattleFrontier_BattleDomeLobby_Text_TrashedInFirstRound, MSGBOX_NPC
	end

BattleFrontier_BattleDomeLobby_EventScript_Man:: @ 8249E10
	msgbox BattleFrontier_BattleDomeLobby_Text_NeedToCheckOpponentCarefully, MSGBOX_NPC
	end

@ A few OutsideWest event scripts are inserted here instead, two of which are unused
BattleFrontier_OutsideWest_EventScript_Man3:: @ 8249E19
	msgbox BattleFrontier_OutsideWest_Text_LongDreamedAboutBattleFrontier, MSGBOX_NPC
	end

@ Unused
BattleFrontier_OutsideWest_EventScript_BattleDomeSign2:: @ 8249E22
	msgbox BattleFrontier_OutsideWest_Text_BattleDomeSign2, MSGBOX_NPC
	end

@ Unused
BattleFrontier_OutsideWest_EventScript_UnderConstructionSign:: @ 8249E2B
	msgbox BattleFrontier_OutsideWest_Text_QuestionMarkUnderConstruction, MSGBOX_NPC
	end

BattleFrontier_BattleDomeLobby_EventScript_RulesBoard:: @ 8249E34
	lockall
	msgbox BattleFrontier_BattleDomeLobby_Text_RulesAreListed, MSGBOX_DEFAULT
	goto BattleFrontier_BattleDomeLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleDomeLobby_EventScript_ReadRulesBoard:: @ 8249E43
	message BattleFrontier_BattleDomeLobby_Text_ReadWhichHeading
	waitmessage
	multichoice 17, 4, MULTI_BATTLE_DOME_RULES, FALSE
	switch VAR_RESULT
	case 0, BattleFrontier_BattleDomeLobby_EventScript_RulesMatchup
	case 1, BattleFrontier_BattleDomeLobby_EventScript_RulesTourneyTree
	case 2, BattleFrontier_BattleDomeLobby_EventScript_RulesDoubleKO
	case 3, BattleFrontier_BattleDomeLobby_EventScript_ExitRules
	case MULTI_B_PRESSED, BattleFrontier_BattleDomeLobby_EventScript_ExitRules
	end

BattleFrontier_BattleDomeLobby_EventScript_RulesMatchup:: @ 8249E8B
	msgbox BattleFrontier_BattleDomeLobby_Text_ExplainMatchupRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleDomeLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleDomeLobby_EventScript_RulesTourneyTree:: @ 8249E99
	msgbox BattleFrontier_BattleDomeLobby_Text_ExplainTourneyTree, MSGBOX_DEFAULT
	goto BattleFrontier_BattleDomeLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleDomeLobby_EventScript_RulesDoubleKO:: @ 8249EA7
	msgbox BattleFrontier_BattleDomeLobby_Text_ExplainDoubleKORules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleDomeLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleDomeLobby_EventScript_ExitRules:: @ 8249EB5
	releaseall
	end

BattleFrontier_BattleDomeLobby_Text_DomeClosed:
	.string "I am sorry, but no tournament is\n"
	.string "being held at the BATTLE DOME.\p"
	.string "The doors to the tournament floor\n"
	.string "are closed until further notice.\p"
	.string "Please enjoy the rest of the\n"
	.string "BATTLE FRONTIER!$"

BattleFrontier_BattleDomeLobby_Text_NoTourneyRecords:
	.string "It's the tournament board.\p"
	.string "There are no tournament results\n"
	.string "posted on it.$"

BattleFrontier_BattleDomeLobby_Text_ManiacNoTourney:
	.string "I come by every day hoping to see\n"
	.string "a tournament bracket go up.\p"
	.string "One of these days, right?$"

BattleFrontier_BattleDomeLobby_Text_WelcomeSingleBattle: @ 8249EB7
	.string "Where the strategies of Trainers\n"
	.string "are put to the test!\p"
	.string "Welcome to the BATTLE DOME!\p"
	.string "I am your guide to the SINGLE BATTLE\n"
	.string "Tournament.$"

BattleFrontier_BattleDomeLobby_Text_TakeSinglesChallenge: @ 8249F3A
	.string "Would you like to challenge\n"
	.string "the SINGLE BATTLE Tournament?$"

BattleFrontier_BattleDomeLobby_Text_HopeToSeeYouAgain: @ 8249F74
	.string "We hope to see you again.$"

BattleFrontier_BattleDomeLobby_Text_ExplainSinglesChallenge: @ 8249F8E
	.string "The SINGLE BATTLE Tournament\n"
	.string "is exactly as the name suggests--\l"
	.string "a tournament of SINGLE BATTLES.\p"
	.string "All participating Trainers must\n"
	.string "enter three Pokémon each.\p"
	.string "In each battle, one of the three\n"
	.string "Pokémon must be held in reserve.\p"
	.string "The other two Pokémon can battle\n"
	.string "one at a time.\p"
	.string "Choose the two battling Pokémon after\n"
	.string "examining the opponent's trio.\p"
	.string "You win the tournament by defeating\n"
	.string "four Trainers.\p"
	.string "Battle Points are awarded to\n"
	.string "the tournament winner.\p"
	.string "If you want to interrupt your\n"
	.string "tournament, please save the game.\p"
	.string "If you don't save before interrupting,\n"
	.string "you will be disqualified.$"

BattleFrontier_BattleDomeLobby_Text_OkayToSaveBeforeChallenge: @ 824A1C6
	.string "Before I show you to the BATTLE DOME,\n"
	.string "I must save the data. Is that okay?$"

BattleFrontier_BattleDomeLobby_Text_WhichLevelMode: @ 824A210
	.string "The tournament offers two levels\n"
	.string "of challenge, Level 50 and Open Level.\l"
	.string "Which is your choice?$"

BattleFrontier_BattleDomeLobby_Text_SelectThreeMons: @ 824A26E
	.string "Now select the three Pokémon that\n"
	.string "you wish to enter, please.$"

BattleFrontier_BattleDomeLobby_Text_NotEnoughValidMonsLvOpen: @ 824A2AB
	.string "Excuse me!\p"
	.string "You don't have three eligible Pokémon.\p"
	.string "Also, the Pokémon must be holding\n"
	.string "different kinds of items.\p"
	.string "EGGS{STR_VAR_1} ineligible.\p"
	.string "Please come see me when you are ready.$"

BattleFrontier_BattleDomeLobby_Text_NotEnoughValidMonsLv50: @ 824A353
	.string "Excuse me!\p"
	.string "You don't have three eligible Pokémon.\p"
	.string "You must have three different Pokémon\n"
	.string "of Level 50 or less to enter.\p"
	.string "They also must be holding different\n"
	.string "kinds of items.\p"
	.string "EGGS{STR_VAR_1} ineligible.\p"
	.string "Please come see me when you are ready.$"

BattleFrontier_BattleDomeLobby_Text_ShowYouToBattleDome: @ 824A437
	.string "I will now show you to\n"
	.string "the BATTLE DOME.$"

BattleFrontier_BattleDomeLobby_Text_DidntSaveBeforeQuitting: @ 824A45F
	.string "Excuse me!\p"
	.string "You didn't save before you quit your\n"
	.string "challenge last time.\p"
	.string "Because of that, your challenge so far\n"
	.string "has been disqualified. Sorry!$"

BattleFrontier_BattleDomeLobby_Text_CongratsForWinningTourney: @ 824A4E9
	.string "Congratulations for winning\n"
	.string "your Battle Tournament!$"

@ Unused
BattleFrontier_BattleDomeLobby_Text_HereIsYourPrize: @ 824A51D
	.string "Here is your prize for your Battle\n"
	.string "Tournament victory.$"

@ Used by Verdanturf Tent
BattleFrontier_BattleDomeLobby_Text_ReceivedPrize: @ 824A554
	.string "{PLAYER} received the prize\n"
	.string "{STR_VAR_1}.$"

@ Unused
BattleFrontier_BattleDomeLobby_Text_BagFullMakeRoom: @ 824A56E
	.string "Oh, your Bag appears to be full.\p"
	.string "Please make room in your Bag, then come\n"
	.string "see me.$"

BattleFrontier_BattleDomeLobby_Text_ThankYouForPlaying: @ 824A5BF
	.string "Thank you for playing!$"

BattleFrontier_BattleDomeLobby_Text_RecordWillBeSaved: @ 824A5D6
	.string "Your record will be saved.\n"
	.string "Please wait.$"

BattleFrontier_BattleDomeLobby_Text_WeveBeenWaitingForYou: @ 824A5FE
	.string "We've been waiting for you!$"

BattleFrontier_BattleDomeLobby_Text_OkayToSaveBeforeChallenge2: @ 824A61A
	.string "Before I show you to the BATTLE DOME,\n"
	.string "I must save the data. Is that okay?$"

BattleFrontier_BattleDomeLobby_Text_WelcomeDoubleBattle: @ 824A664
	.string "Where the strategies of Trainers\n"
	.string "are put to the test!\p"
	.string "Welcome to the BATTLE DOME!\p"
	.string "I am your guide to the Double Battle\n"
	.string "Tournament.$"

BattleFrontier_BattleDomeLobby_Text_TakeDoublesChallenge: @ 824A6E7
	.string "Would you like to challenge\n"
	.string "the Double Battle Tournament?$"

BattleFrontier_BattleDomeLobby_Text_ExplainDoublesChallenge: @ 824A721
	.string "The Double Battle Tournament\n"
	.string "is exactly as the name suggests--\l"
	.string "a tournament of Double Battles.\p"
	.string "All participating Trainers must\n"
	.string "enter three Pokémon each.\p"
	.string "In each battle, one of the three\n"
	.string "Pokémon must be held in reserve.\p"
	.string "The remaining two Pokémon must take\n"
	.string "part in a Double Battle.\p"
	.string "Choose the two battling Pokémon after\n"
	.string "examining the opponent's trio.\p"
	.string "You win the tournament by defeating\n"
	.string "four Trainers.\p"
	.string "Battle Points are awarded to\n"
	.string "the tournament winner.\p"
	.string "If you want to interrupt your\n"
	.string "tournament, please save the game.\p"
	.string "If you don't save before interrupting,\n"
	.string "you will be disqualified.$"

BattleFrontier_BattleDomeLobby_Text_PrevTourneyResultsSinglesLv50: @ 824A966
	.string "They're the results of the last\n"
	.string "Level 50 SINGLE BATTLE Tournament.$"

BattleFrontier_BattleDomeLobby_Text_PrevTourneyResultsDoublesLv50: @ 824A9A9
	.string "They're the results of the last\n"
	.string "Level 50 Double Battle Tournament.$"

BattleFrontier_BattleDomeLobby_Text_PrevTourneyResultsSinglesLvOpen: @ 824A9EC
	.string "They're the results of the last\n"
	.string "Open Level SINGLE BATTLE Tournament.$"

BattleFrontier_BattleDomeLobby_Text_PrevTourneyResultsDoublesLvOpen: @ 824AA31
	.string "They're the results of the last\n"
	.string "Open Level Double Battle Tournament.$"

BattleFrontier_BattleDomeLobby_Text_LastWinnerWasTough: @ 824AA76
	.string "Did you see it?\n"
	.string "The last Battle Tournament?\p"
	.string "The winner, {STR_VAR_1}, was seriously\n"
	.string "tough.\p"
	.string "You should check out the results\n"
	.string "on the monitor beside the PC.$"

BattleFrontier_OutsideWest_Text_LongDreamedAboutBattleFrontier: @ 824AB06
	.string "The Battle Frontier…\n"
	.string "I've long dreamed about a place like it.$"

@ Functionally unused
BattleFrontier_OutsideWest_Text_BattleDomeSign2: @ 824AB44
	.string "The BATTLE DOME\n"
	.string "Become the Unbeatable Superstar!$"

@ Functionally unused
BattleFrontier_OutsideWest_Text_QuestionMarkUnderConstruction: @ 824AB75
	.string "The ??????\n"
	.string "Under Construction!$"

BattleFrontier_BattleDomeLobby_Text_WinnersGainReputation: @ 824AB94
	.string "When a Trainer chains tournament\n"
	.string "wins at the BATTLE DOME, he or she\l"
	.string "gains a reputation as a star.\p"
	.string "Tough Trainers are drawn by that\n"
	.string "reputation to the BATTLE DOME.\p"
	.string "A true superstar is a Trainer who\n"
	.string "can keep winning tournaments.$"

BattleFrontier_BattleDomeLobby_Text_TrashedInFirstRound: @ 824AC76
	.string "I ran into one of the tournament\n"
	.string "favorites in the very first round.\p"
	.string "Of course I got trashed…$"

BattleFrontier_BattleDomeLobby_Text_NeedToCheckOpponentCarefully: @ 824ACD3
	.string "I would've won if I'd kept this Pokémon\n"
	.string "held in reserve.\p"
	.string "You need to check your opponent's\n"
	.string "Pokémon carefully before choosing\l"
	.string "your battling Pokémon.$"

BattleFrontier_BattleDomeLobby_Text_CongratsDefeatedTucker: @ 824AD67
	.string "Congratulations!\p"
	.string "You defeated the DOME ACE and won\n"
	.string "the Battle Tournament!$"

BattleFrontier_BattleDomeLobby_Text_AwardTheseBattlePoints: @ 824ADB1
	.string "In recognition of your strategy--\n"
	.string "a thing of beauty it was, too--\l"
	.string "we award you these Battle Point(s)!$"

BattleFrontier_BattleDomeLobby_Text_RecordLastMatch: @ 824AE17
	.string "Would you like to record your\n"
	.string "last BATTLE DOME match on your\l"
	.string "FRONTIER PASS?$"

BattleFrontier_BattleDomeLobby_Text_RulesAreListed: @ 824AE63
	.string "The Battle Tournament rules\n"
	.string "are listed.$"

BattleFrontier_BattleDomeLobby_Text_ReadWhichHeading: @ 824AE8B
	.string "Which heading do you want to read?$"

BattleFrontier_BattleDomeLobby_Text_ExplainMatchupRules: @ 824AEAE
	.string "The tournament matchups are drawn up\n"
	.string "based on the toughness of Pokémon\l"
	.string "held by Trainers.\p"
	.string "The matchups avoid having tough\n"
	.string "Trainers face each other right away.$"

BattleFrontier_BattleDomeLobby_Text_ExplainTourneyTree: @ 824AF4C
	.string "The tournament chart, or as we call it,\n"
	.string "the “Tree,” is available for viewing\l"
	.string "in the Waiting Room from any guide.\p"
	.string "The Tournament Tree will provide\n"
	.string "several kinds of data.\p"
	.string "The data on view includes the previous\n"
	.string "tournament's results, the Pokémon held\l"
	.string "by TRAINERS, and the battle styles of\l"
	.string "Trainers.$"

BattleFrontier_BattleDomeLobby_Text_ExplainDoubleKORules: @ 824B073
	.string "If battling Pokémon faint at the same\n"
	.string "time--a double KO--in a tournament\l"
	.string "match, the REFEREES will review\l"
	.string "the match and declare the winner.$"
