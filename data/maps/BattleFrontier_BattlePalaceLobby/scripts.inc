.set LOCALID_ATTENDANT_SINGLES, 1
.set LOCALID_ATTENDANT_DOUBLES, 6

BattleFrontier_BattlePalaceLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattlePalaceLobby_OnWarp
	.byte 0

BattleFrontier_BattlePalaceLobby_OnWarp: @ 824D789
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattlePalaceLobby_EventScript_TurnPlayerNorth
	.2byte 0

BattleFrontier_BattlePalaceLobby_EventScript_TurnPlayerNorth:: @ 824D793
	setvar VAR_TEMP_1, 1
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	end

@ Emerald Champions: the Battle Palace is retired. src/battle_palace.c and the Palace's own
@ maps are gone from this tree, so the challenge flow that used to live here
@ (challenge-status dispatch, party select, save, Palace) has been removed
@ rather than left calling missing code. The attendants still greet the
@ player and explain that the Palace is closed.

BattleFrontier_BattlePalaceLobby_EventScript_SinglesAttendant::
	lock
	faceplayer
	msgbox BattleFrontier_BattlePalaceLobby_Text_WelcomeForSingleBattle, MSGBOX_DEFAULT
	msgbox BattleFrontier_BattlePalaceLobby_Text_PalaceClosed, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattlePalaceLobby_EventScript_DoublesAttendant::
	lock
	faceplayer
	msgbox BattleFrontier_BattlePalaceLobby_Text_WelcomeForDoubleBattle, MSGBOX_DEFAULT
	msgbox BattleFrontier_BattlePalaceLobby_Text_PalaceClosed, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattlePalaceLobby_EventScript_ShowSinglesResults::
	lockall
	frontier_results FRONTIER_FACILITY_PALACE, FRONTIER_MODE_SINGLES
	waitbuttonpress
	special RemoveRecordsWindow
	releaseall
	end

BattleFrontier_BattlePalaceLobby_EventScript_ShowDoublesResults:: @ 824DC69
	lockall
	frontier_results FRONTIER_FACILITY_PALACE, FRONTIER_MODE_DOUBLES
	waitbuttonpress
	special RemoveRecordsWindow
	releaseall
	end

BattleFrontier_BattlePalaceLobby_EventScript_BlackBelt:: @ 824DC82
	msgbox BattleFrontier_BattlePalaceLobby_Text_LadyCanTellWhatMonsThink, MSGBOX_NPC
	end

BattleFrontier_BattlePalaceLobby_EventScript_Man:: @ 824DC8B
	msgbox BattleFrontier_BattlePalaceLobby_Text_NatureAndMovesKeyHere, MSGBOX_NPC
	end

BattleFrontier_BattlePalaceLobby_EventScript_Beauty:: @ 824DC94
	msgbox BattleFrontier_BattlePalaceLobby_Text_MonDocileButTransforms, MSGBOX_NPC
	end

BattleFrontier_BattlePalaceLobby_EventScript_Maniac:: @ 824DC9D
	msgbox BattleFrontier_BattlePalaceLobby_Text_WhatNatureFavorsChippingAway, MSGBOX_NPC
	end

BattleFrontier_BattlePalaceLobby_EventScript_RulesBoard:: @ 824DCA6
	lockall
	msgbox BattleFrontier_BattlePalaceLobby_Text_RulesAreListed, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePalaceLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePalaceLobby_EventScript_ReadRulesBoard:: @ 824DCB5
	message BattleFrontier_BattlePalaceLobby_Text_ReadWhichHeading
	waitmessage
	multichoice 16, 0, MULTI_BATTLE_PALACE_RULES, FALSE
	switch VAR_RESULT
	case 0, BattleFrontier_BattlePalaceLobby_EventScript_RulesBasics
	case 1, BattleFrontier_BattlePalaceLobby_EventScript_RulesNature
	case 2, BattleFrontier_BattlePalaceLobby_EventScript_RulesMoves
	case 3, BattleFrontier_BattlePalaceLobby_EventScript_RulesUnderpowered
	case 4, BattleFrontier_BattlePalaceLobby_EventScript_RulesWhenInDanger
	case 5, BattleFrontier_BattlePalaceLobby_EventScript_ExitRules
	case MULTI_B_PRESSED, BattleFrontier_BattlePalaceLobby_EventScript_ExitRules
	end

BattleFrontier_BattlePalaceLobby_EventScript_RulesBasics:: @ 824DD13
	msgbox BattleFrontier_BattlePalaceLobby_Text_ExplainRulesBasics, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePalaceLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePalaceLobby_EventScript_RulesNature:: @ 824DD21
	msgbox BattleFrontier_BattlePalaceLobby_Text_ExplainRulesNature, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePalaceLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePalaceLobby_EventScript_RulesMoves:: @ 824DD2F
	msgbox BattleFrontier_BattlePalaceLobby_Text_ExplainRulesMoves, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePalaceLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePalaceLobby_EventScript_RulesUnderpowered:: @ 824DD3D
	msgbox BattleFrontier_BattlePalaceLobby_Text_ExplainRulesUnderpowered, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePalaceLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePalaceLobby_EventScript_RulesWhenInDanger:: @ 824DD4B
	msgbox BattleFrontier_BattlePalaceLobby_Text_ExplainRulesWhenInDanger, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePalaceLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePalaceLobby_EventScript_ExitRules:: @ 824DD59
	releaseall
	end

BattleFrontier_BattlePalaceLobby_Text_PalaceClosed:
	.string "However, the halls beyond are closed,\n"
	.string "and no challenges are being taken.\p"
	.string "The Pokémon here must rely on their\n"
	.string "own nature another day.\p"
	.string "Please enjoy the rest of the\n"
	.string "BATTLE FRONTIER.$"

BattleFrontier_BattlePalaceLobby_Text_WelcomeForSingleBattle: @ 824DD5B
	.string "Where the hearts of Trainers\n"
	.string "are put to the test.\p"
	.string "I welcome you to the BATTLE PALACE.\p"
	.string "I accept challenges to the SINGLE\n"
	.string "BATTLE HALLS.$"

BattleFrontier_BattlePalaceLobby_Text_TakeSingleBattleChallenge: @ 824DDE1
	.string "Do you wish to take\n"
	.string "the SINGLE BATTLE HALL challenge?$"

BattleFrontier_BattlePalaceLobby_Text_ExplainSingleBattleChallenge: @ 824DE17
	.string "In the BATTLE PALACE, there are\n"
	.string "several auditoriums for SINGLE BATTLES\l"
	.string "that are named SINGLE BATTLE HALLS.\p"
	.string "The SINGLE BATTLES are conducted in\n"
	.string "the usual way, except with one crucial\l"
	.string "rule that must be obeyed.\p"
	.string "Trainers are permitted only to switch\n"
	.string "their Pokémon in and out.\p"
	.string "Trainers are otherwise forbidden to\n"
	.string "command their Pokémon.\p"
	.string "The Pokémon are to behave according\n"
	.string "to their nature and battle on their own.\p"
	.string "You must put your trust in your\n"
	.string "Pokémon and watch over them.\p"
	.string "If you achieve the honor of beating\n"
	.string "seven Trainers in succession,\l"
	.string "we will present you with Battle Points.\p"
	.string "If you want to interrupt your\n"
	.string "challenge, you must save the game.\p"
	.string "If you don't save before interrupting,\n"
	.string "you will be disqualified.$"

BattleFrontier_BattlePalaceLobby_Text_ReturnWhenFortified: @ 824E0D8
	.string "When you have fortified your heart\n"
	.string "and Pokémon, you must return.$"

BattleFrontier_BattlePalaceLobby_Text_WhichChallenge: @ 824E119
	.string "There are two BATTLE HALLS,\n"
	.string "Level 50 and Open Level.\l"
	.string "Which is your choice of a challenge?$"

BattleFrontier_BattlePalaceLobby_Text_NotEnoughValidMonsLv50: @ 824E173
	.string "Sigh…\p"
	.string "You do not have the three Pokémon\n"
	.string "required for the challenge.\p"
	.string "To enter, you must provide three\n"
	.string "different kinds of Pokémon.\p"
	.string "These Pokémon must all be of\n"
	.string "Level 50 or lower.\p"
	.string "They also must not be holding\n"
	.string "the same kinds of items.\p"
	.string "EGGS{STR_VAR_1} ineligible.\p"
	.string "Come back when you have made\n"
	.string "your preparations.$"

BattleFrontier_BattlePalaceLobby_Text_NotEnoughValidMonsLvOpen: @ 824E29E
	.string "Sigh…\p"
	.string "You do not have the three Pokémon\n"
	.string "required for the challenge.\p"
	.string "To enter, you must provide three\n"
	.string "different kinds of Pokémon.\p"
	.string "They also must not be holding\n"
	.string "the same kinds of items.\p"
	.string "EGGS{STR_VAR_1} ineligible.\p"
	.string "Come back when you have made\n"
	.string "your preparations.$"

BattleFrontier_BattlePalaceLobby_Text_NowSelectThreeMons: @ 824E399
	.string "Good. Now, you must select your\n"
	.string "three Pokémon.$"

BattleFrontier_BattlePalaceLobby_Text_MustSaveBeforeChallenge2: @ 824E3C8
	.string "I must save before I show you to\n"
	.string "the BATTLE HALL. Is that okay?$"

BattleFrontier_BattlePalaceLobby_Text_FollowMe: @ 824E408
	.string "Good.\n"
	.string "Now, follow me.$"

BattleFrontier_BattlePalaceLobby_Text_ResultsWillBeRecorded: @ 824E41E
	.string "I feel privileged for having seen\n"
	.string "your Pokémon's exploits.\p"
	.string "The results will be recorded.\n"
	.string "I must ask you to briefly wait.$"

BattleFrontier_BattlePalaceLobby_Text_FirmTrueBondsFor7WinStreak: @ 824E497
	.string "To achieve a 7-win streak…\p"
	.string "The bonds that bind your heart with\n"
	.string "your Pokémon seem firm and true.$"

BattleFrontier_BattlePalaceLobby_Text_FeatWillBeRecorded: @ 824E4F7
	.string "Your feat will be recorded.\n"
	.string "I must ask you to briefly wait.$"

@ Unused
BattleFrontier_BattlePalaceLobby_Text_BattlePointsFor7WinStreak: @ 824E5333
    .string "For the feat of your 7-win streak,\n"
    .string "we present you with Battle Point(s).$"

BattleFrontier_BattlePalaceLobby_Text_NoSpaceForPrize: @ 824E57B
	.string "You seem to have no space for\n"
	.string "our prize.\p"
	.string "You should return when you have\n"
	.string "organized your Bag.$"

BattleFrontier_BattlePalaceLobby_Text_WeHaveBeenWaiting: @ 824E5D8
	.string "We have been waiting for you…$"

BattleFrontier_BattlePalaceLobby_Text_MustSaveBeforeChallenge: @ 824E5F6
	.string "I must save before I show you to\n"
	.string "the BATTLE HALL. Is that okay?$"

BattleFrontier_BattlePalaceLobby_Text_FailedToSaveBeforeEndingChallenge: @ 824E636
	.string "Sigh…\p"
	.string "You failed to save before you ended\n"
	.string "your challenge the last time.\p"
	.string "As a result, your challenge has been\n"
	.string "disqualified. It is most unfortunate.$"

@ Unused
BattleFrontier_BattlePalaceLobby_Text_ReceivedPrize: @ 824E6C9
	.string "{PLAYER} received the prize\n"
	.string "{STR_VAR_1}.$"

BattleFrontier_BattlePalaceLobby_Text_LadyCanTellWhatMonsThink: @ 824E6E3
	.string "For a hardy fellow like me,\n"
	.string "hardy Pokémon are the best.\p"
	.string "Offense is the best defense!\n"
	.string "A relentless attack is the ultimate!\p"
	.string "But that's not what I wanted to chat\n"
	.string "about.\p"
	.string "Every so often a really cute lady\n"
	.string "swings by here.\p"
	.string "She says that she can tell what\n"
	.string "Pokémon are thinking.\p"
	.string "I don't know about that part,\n"
	.string "but she is really cute.\p"
	.string "Huh?\n"
	.string "Why are you looking at me like that?$"

BattleFrontier_BattlePalaceLobby_Text_NatureAndMovesKeyHere: @ 824E851
	.string "Hmm…\p"
	.string "It appears that the nature of Pokémon\n"
	.string "and the moves that they have been\l"
	.string "taught are the keys to battle here.\p"
	.string "To be more precise, it's how well\n"
	.string "the moves match the nature of\l"
	.string "the Pokémon.\p"
	.string "If your Pokémon is in trouble and\n"
	.string "unable to live up to its potential,\l"
	.string "you may need to examine how well\l"
	.string "its moves match its nature.$"

BattleFrontier_BattlePalaceLobby_Text_MonDocileButTransforms: @ 824E992
	.string "My Pokémon is usually very docile.\p"
	.string "But when it's in a BATTLE HALL,\n"
	.string "it sometimes seems to become\l"
	.string "transformed into something wicked!\p"
	.string "It becomes totally intimidating.\n"
	.string "It's shocking, even.$"

BattleFrontier_BattlePalaceLobby_Text_WhatNatureFavorsChippingAway: @ 824EA4B
	.string "I wonder what sort of nature a Pokémon\n"
	.string "would have if it favored enfeebling its\l"
	.string "opponents and chipping away slowly.\p"
	.string "I'd be surprised if it was a LAX nature.\p"
	.string "But, nah, that can't be right.$"

BattleFrontier_BattlePalaceLobby_Text_WelcomeForDoubleBattle: @ 824EB06
	.string "Where the hearts of Trainers\n"
	.string "are put to the test.\p"
	.string "I welcome you to the BATTLE PALACE.\p"
	.string "I accept challenges to the DOUBLE\n"
	.string "BATTLE HALLS.$"

BattleFrontier_BattlePalaceLobby_Text_TakeDoubleBattleChallenge: @ 824EB8C
	.string "Do you wish to take\n"
	.string "the Double Battle HALL challenge?$"

BattleFrontier_BattlePalaceLobby_Text_ExplainDoubleBattleChallenge: @ 824EBC2
	.string "In the BATTLE PALACE, there are\n"
	.string "several auditoriums for Double Battles\l"
	.string "that are named Double Battle HALLS.\p"
	.string "The Double Battles are conducted in\n"
	.string "the usual way, except with one crucial\l"
	.string "rule that must be obeyed.\p"
	.string "Trainers are permitted only to switch\n"
	.string "their Pokémon in and out.\p"
	.string "Trainers are otherwise forbidden to\n"
	.string "command their Pokémon.\p"
	.string "The Pokémon are to behave according\n"
	.string "to their nature and battle on their own.\p"
	.string "You must put your trust in your\n"
	.string "Pokémon and watch over them.\p"
	.string "If you achieve the honor of beating\n"
	.string "seven Trainers in succession,\l"
	.string "we will present you with Battle Points.\p"
	.string "If you want to interrupt your\n"
	.string "challenge, please save the game.\p"
	.string "If you don't save before interrupting,\n"
	.string "you will be disqualified.$"

BattleFrontier_BattlePalaceLobby_Text_ToDefeatMavenAnd7Trainers: @ 824EE81
	.string "To defeat the PALACE MAVEN\n"
	.string "and seven Trainers in a row…$"

BattleFrontier_BattlePalaceLobby_Text_PresentYouWithBattlePoints: @ 824EEB9
	.string "In honor of the bond you share with\n"
	.string "your Pokémon, we present you with\l"
	.string "these Battle Point(s).$"

BattleFrontier_BattlePalaceLobby_Text_LikeToRecordMatch: @ 824EF16
	.string "Would you like to record your latest\n"
	.string "BATTLE PALACE match on your\l"
	.string "FRONTIER PASS?$"

BattleFrontier_BattlePalaceLobby_Text_RulesAreListed: @ 824EF66
	.string "The BATTLE HALL rules are listed.$"

BattleFrontier_BattlePalaceLobby_Text_ReadWhichHeading: @ 824EF88
	.string "Which heading do you want to read?$"

BattleFrontier_BattlePalaceLobby_Text_ExplainRulesBasics: @ 824EFAB
	.string "Here, Pokémon are required to think\n"
	.string "and battle by themselves.\p"
	.string "Unlike in the wild, Pokémon that live\n"
	.string "with people behave differently\l"
	.string "depending on their nature.$"

BattleFrontier_BattlePalaceLobby_Text_ExplainRulesNature: @ 824F049
	.string "Depending on its nature, a Pokémon\n"
	.string "may prefer to attack no matter what.\p"
	.string "Another Pokémon may prefer to protect\n"
	.string "itself from any harm.\p"
	.string "Yet another may enjoy vexing or\n"
	.string "confounding its foes.\p"
	.string "Due to its nature, a Pokémon\n"
	.string "will have favorite moves that it is good\l"
	.string "at using.\p"
	.string "It may also dislike certain moves that\n"
	.string "it has trouble using.$"

BattleFrontier_BattlePalaceLobby_Text_ExplainRulesMoves: @ 824F190
	.string "There are offensive moves that inflict\n"
	.string "direct damage on the foe.\p"
	.string "There are defensive moves that are\n"
	.string "used to prepare for enemy attacks or\l"
	.string "used to heal HP and so on.\p"
	.string "There are also other somewhat-odd\n"
	.string "moves that may enfeeble the foes with\l"
	.string "stat problems including poison and \l"
	.string "paralysis.\p"
	.string "Pokémon will consider using moves in\n"
	.string "these three categories.$"

BattleFrontier_BattlePalaceLobby_Text_ExplainRulesUnderpowered: @ 824F2E8
	.string "When not under command by its Trainer,\n"
	.string "a Pokémon may be unable to effectively\l"
	.string "use certain moves.\p"
	.string "A Pokémon is not good at using any\n"
	.string "move that it dislikes.\p"
	.string "If a Pokémon only knows moves that\n"
	.string "do not match its nature, it will often\l"
	.string "be unable to live up to its potential.$"

BattleFrontier_BattlePalaceLobby_Text_ExplainRulesWhenInDanger: @ 824F3F4
	.string "Depending on its nature, a Pokémon may\n"
	.string "start using moves that don't match its\l"
	.string "nature when it is in trouble.\p"
	.string "If a Pokémon begins behaving oddly\n"
	.string "in a pinch, watch it carefully.$"

