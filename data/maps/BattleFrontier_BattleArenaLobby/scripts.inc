.set LOCALID_ATTENDANT, 1

BattleFrontier_BattleArenaLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattleArenaLobby_OnWarp
	.byte 0

BattleFrontier_BattleArenaLobby_OnWarp: @ 8255C41
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattleArenaLobby_EventScript_TurnPlayerNorth
	.2byte 0

BattleFrontier_BattleArenaLobby_EventScript_TurnPlayerNorth:: @ 8255C4B
	setvar VAR_TEMP_1, 1
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	end

@ Emerald Champions: the Battle Arena is retired. src/battle_arena.c and the Arena's own
@ maps are gone from this tree, so the challenge flow that used to live here
@ (challenge-status dispatch, party select, save, Arena) has been removed
@ rather than left calling missing code. The attendants still greet the
@ player and explain that the Arena is closed.

BattleFrontier_BattleArenaLobby_EventScript_Attendant::
	lock
	faceplayer
	msgbox BattleFrontier_BattleArenaLobby_Text_WelcomeToBattleArena, MSGBOX_DEFAULT
	msgbox BattleFrontier_BattleArenaLobby_Text_ArenaClosed, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleArenaLobby_EventScript_ShowResults::
	lockall
	frontier_results FRONTIER_FACILITY_ARENA
	waitbuttonpress
	special RemoveRecordsWindow
	releaseall
	end

BattleFrontier_BattleArenaLobby_EventScript_Youngster:: @ 82560A6
	msgbox BattleFrontier_BattleArenaLobby_Text_BadIdeaToNotAttack, MSGBOX_NPC
	end

BattleFrontier_BattleArenaLobby_EventScript_Man:: @ 82560AF
	msgbox BattleFrontier_BattleArenaLobby_Text_LandingHitsWorked, MSGBOX_NPC
	end

BattleFrontier_BattleArenaLobby_EventScript_Camper:: @ 82560B8
	msgbox BattleFrontier_BattleArenaLobby_Text_MatchWasDeclaredDraw, MSGBOX_NPC
	end

BattleFrontier_BattleArenaLobby_EventScript_Woman:: @ 82560C1
	msgbox BattleFrontier_BattleArenaLobby_Text_OrderOfMonsImportant, MSGBOX_NPC
	end

BattleFrontier_BattleArenaLobby_EventScript_RulesBoard:: @ 82560CA
	lockall
	msgbox BattleFrontier_BattleArenaLobby_Text_RulesAreListed, MSGBOX_DEFAULT
	goto BattleFrontier_BattleArenaLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleArenaLobby_EventScript_ReadRulesBoard:: @ 82560D9
	message BattleFrontier_BattleArenaLobby_Text_ReadWhichHeading
	waitmessage
	multichoice 17, 2, MULTI_BATTLE_ARENA_RULES, FALSE
	switch VAR_RESULT
	case 0, BattleFrontier_BattleArenaLobby_EventScript_BattleRules
	case 1, BattleFrontier_BattleArenaLobby_EventScript_MindRules
	case 2, BattleFrontier_BattleArenaLobby_EventScript_SkillRules
	case 3, BattleFrontier_BattleArenaLobby_EventScript_BodyRules
	case 4, BattleFrontier_BattleArenaLobby_EventScript_ExitRules
	case MULTI_B_PRESSED, BattleFrontier_BattleArenaLobby_EventScript_ExitRules
	end

BattleFrontier_BattleArenaLobby_EventScript_BattleRules:: @ 825612C
	msgbox BattleFrontier_BattleArenaLobby_Text_ExplainBattleRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleArenaLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleArenaLobby_EventScript_MindRules:: @ 825613A
	msgbox BattleFrontier_BattleArenaLobby_Text_ExplainMindRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleArenaLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleArenaLobby_EventScript_SkillRules:: @ 8256148
	msgbox BattleFrontier_BattleArenaLobby_Text_ExplainSkillRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleArenaLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleArenaLobby_EventScript_BodyRules:: @ 8256156
	msgbox BattleFrontier_BattleArenaLobby_Text_ExplainBodyRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattleArenaLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattleArenaLobby_EventScript_ExitRules:: @ 8256164
	releaseall
	end

BattleFrontier_BattleArenaLobby_Text_ArenaClosed:
	.string "But I regret to say that the set\n"
	.string "battles are not being held today.\p"
	.string "The arena beyond this room is closed\n"
	.string "until further notice.\p"
	.string "Please enjoy the rest of the\n"
	.string "BATTLE FRONTIER!$"

BattleFrontier_BattleArenaLobby_Text_WelcomeToBattleArena: @ 8256166
	.string "Where the battling spirit of Trainers\n"
	.string "is put to the test!\p"
	.string "I welcome you to the BATTLE ARENA!\p"
	.string "I am your guide to the Set KO Tourney!$"

BattleFrontier_BattleArenaLobby_Text_WishToTakeChallenge: @ 82561EA
	.string "Now, do you wish to take\n"
	.string "the BATTLE ARENA challenge?$"

BattleFrontier_BattleArenaLobby_Text_AwaitAnotherChallenge: @ 825621F
	.string "We await your challenge on\n"
	.string "another occasion!$"

BattleFrontier_BattleArenaLobby_Text_ExplainChallenge: @ 825624C
	.string "In the BATTLE ARENA, we undertake\n"
	.string "the Set KO Tourney.\p"
	.string "All participants enter with a team of\n"
	.string "three Pokémon.\p"
	.string "The three Pokémon must be lined up in\n"
	.string "the order in which they are to appear.\p"
	.string "During battle, the Pokémon will appear\n"
	.string "one at a time in the preset order.\p"
	.string "Once a Pokémon enters battle, it must\n"
	.string "remain out until the match is decided.\p"
	.string "If a match remains undecided in\n"
	.string "three turns, it goes to judging.\p"
	.string "If you wish to interrupt your challenge,\n"
	.string "please save the game.\p"
	.string "If you don't save, you will not be able\n"
	.string "to continue with your challenge.\p"
	.string "If you pull off the feat of beating\n"
	.string "seven Trainers in succession,\l"
	.string "we will present you with Battle Points.$"

BattleFrontier_BattleArenaLobby_Text_OkayToSave: @ 82564CE
	.string "Before showing you to the BATTLE\n"
	.string "ARENA, you must save. Is that okay?$"

BattleFrontier_BattleArenaLobby_Text_WhichLevelMode: @ 8256513
	.string "The BATTLE ARENA offers two levels\n"
	.string "of challenge, Level 50 and Open Level.\l"
	.string "Which is your choice?$"

BattleFrontier_BattleArenaLobby_Text_SelectThreeMons: @ 8256573
	.string "Very well, now select your\n"
	.string "three Pokémon, please.$"

BattleFrontier_BattleArenaLobby_Text_NotEnoughValidMonsLvOpen: @ 82565A5
	.string "My dear challenger!\p"
	.string "You do not have the three Pokémon\n"
	.string "required for entry.\p"
	.string "To qualify, you must bring three\n"
	.string "different kinds of Pokémon.\p"
	.string "They also must not hold the same\n"
	.string "kinds of items.\p"
	.string "EGGS{STR_VAR_1} ineligible.\p"
	.string "When you have made your preparations,\n"
	.string "please do return.$"

BattleFrontier_BattleArenaLobby_Text_NotEnoughValidMonsLv50: @ 82566A8
	.string "My dear challenger!\p"
	.string "You do not have the three Pokémon\n"
	.string "required for entry.\p"
	.string "To qualify, you must bring three\n"
	.string "different kinds of Pokémon.\p"
	.string "The three different Pokémon\n"
	.string "must all be Level 50 or lower.\p"
	.string "They also must not hold the same\n"
	.string "kinds of items.\p"
	.string "EGGS{STR_VAR_1} ineligible.\p"
	.string "When you have made your preparations,\n"
	.string "please do return.$"

BattleFrontier_BattleArenaLobby_Text_GuideYouToArena: @ 82567E6
	.string "I shall now guide you to\n"
	.string "the BATTLE ARENA.$"

BattleFrontier_BattleArenaLobby_Text_DidntSaveBeforeShuttingDown: @ 8256811
	.string "My dear challenger!\p"
	.string "You did not save the game before\n"
	.string "shutting down, did you?\p"
	.string "It is unfortunate, but that has\n"
	.string "resulted in your disqualification\l"
	.string "from your challenge.\p"
	.string "You may, of course, start with a fresh\n"
	.string "challenge.$"

BattleFrontier_BattleArenaLobby_Text_CongratsOnSevenWins: @ 82568E7
	.string "We congratulate you for your splendid\n"
	.string "string of wins over seven Trainers!$"

BattleFrontier_BattleArenaLobby_Text_RecordAchievement: @ 8256931
	.string "Your achievement will be recorded.\n"
	.string "Please wait while I save the game.$"

@ Unused
BattleFrontier_BattleArenaLobby_Text_PresentYouWithPrize: @ 8256977
	.string "In commemoration of your 7-win streak,\n"
	.string "we present you with this prize.$"

@ Unused
BattleFrontier_BattleArenaLobby_Text_ReceivedPrize: @ 82569BE
	.string "{PLAYER} received the prize\n"
	.string "{STR_VAR_1}.$"

@ Unused
BattleFrontier_BattleArenaLobby_Text_BagFullReturnForPrize: @ 82569D8
	.string "Oh?\n"
	.string "Your Bag seems to be full.\p"
	.string "I urge you to clear space and\n"
	.string "return for your prize.$"

BattleFrontier_BattleArenaLobby_Text_ThankYouWaitWhileSave: @ 8256A2C
	.string "Thank you so much for participating!\p"
	.string "Please wait while I save the game.$"

BattleFrontier_BattleArenaLobby_Text_AwaitAnotherChallenge2: @ 8256A74
	.string "We await your challenge on\n"
	.string "another occasion!$"

BattleFrontier_BattleArenaLobby_Text_LookingForwardToArrivalSaveGame: @ 8256AA1
	.string "We have been looking forward to\n"
	.string "your arrival.\p"
	.string "Before I show you to the BATTLE\n"
	.string "ARENA, I must save the game.\l"
	.string "Please wait.$"

BattleFrontier_BattleArenaLobby_Text_RecordLastMatch: @ 8256B19
	.string "Shall I record your last BATTLE ARENA\n"
	.string "match on your FRONTIER PASS?$"

BattleFrontier_BattleArenaLobby_Text_BadIdeaToNotAttack: @ 8256B5C
	.string "I lost on the REFEREE's decision…\p"
	.string "I don't think it was a good idea to only\n"
	.string "use defensive moves and not attack…$"

BattleFrontier_BattleArenaLobby_Text_LandingHitsWorked: @ 8256BCB
	.string "I won in judging!\p"
	.string "Landing hits consistently on\n"
	.string "the opponent's Pokémon worked!$"

BattleFrontier_BattleArenaLobby_Text_MatchWasDeclaredDraw: @ 8256C19
	.string "Our match was declared a draw.\p"
	.string "When we ran out of time, both my\n"
	.string "Pokémon and the opponent's had about\l"
	.string "the same amount of HP left.$"

BattleFrontier_BattleArenaLobby_Text_OrderOfMonsImportant: @ 8256C9A
	.string "In the BATTLE ARENA, the order of\n"
	.string "Pokémon is totally important.\p"
	.string "For example, if your first Pokémon\n"
	.string "has certain type disadvantages,\l"
	.string "try making your second Pokémon one\l"
	.string "with moves that are super effective\l"
	.string "against the first one.\p"
	.string "I think that will be a good way of\n"
	.string "making an effective team.$"

BattleFrontier_BattleArenaLobby_Text_RulesAreListed: @ 8256DB8
	.string "The Set KO Tourney's rules are listed.$"

BattleFrontier_BattleArenaLobby_Text_ReadWhichHeading: @ 8256DDF
	.string "Which heading do you want to read?$"

BattleFrontier_BattleArenaLobby_Text_ExplainBattleRules: @ 8256E02
	.string "The Set KO Tourney has special rules,\n"
	.string "unlike standard battles.\p"
	.string "First, one battle lasts only\n"
	.string "three turns.\p"
	.string "If the outcome is not decided after\n"
	.string "three turns, it goes to judging.\p"
	.string "The REFEREE will choose the winner\n"
	.string "based on how the Pokémon battled.\p"
	.string "Also, a Pokémon cannot be switched out\n"
	.string "until its battle's outcome is decided.$"

BattleFrontier_BattleArenaLobby_Text_ExplainMindRules: @ 8256F43
	.string "The first judging factor is “Mind.”\n"
	.string "This factor evaluates how aggressive\l"
	.string "the battlers were.\p"
	.string "The judging is based on how often\n"
	.string "the Trainers ordered the use of\l"
	.string "offensive moves.$"

BattleFrontier_BattleArenaLobby_Text_ExplainSkillRules: @ 8256FF2
	.string "The second judging factor is “Skill.”\n"
	.string "This factor evaluates how effectively\l"
	.string "Pokémon moves were used.\p"
	.string "If a move worked successfully,\n"
	.string "the Skill rating goes up.\p"
	.string "If a move failed, the Skill rating\n"
	.string "is lowered.\p"
	.string "If it was an offensive move, the Skill\n"
	.string "rating goes up if the move was “super\l"
	.string "effective” and goes down if it was\l"
	.string "“not very effective.”\p"
	.string "The Skill rating will not go up for\n"
	.string "moves such as PROTECT and DETECT.\p"
	.string "If the opponent used PROTECT or\n"
	.string "DETECT, and your Pokémon failed to\l"
	.string "hit with a move, its Skill rating will not\l"
	.string "go down.$"

BattleFrontier_BattleArenaLobby_Text_ExplainBodyRules: @ 8257202
	.string "The third judging factor is “Body.”\n"
	.string "This factor is based on how much HP\l"
	.string "remained at the end of a battle.\p"
	.string "The rating takes into account how much\n"
	.string "HP a Pokémon had at the start of\l"
	.string "battle, and what remained at the end.$"

BattleFrontier_BattleArenaLobby_Text_CongratsOnDefeatingTycoon: @ 82572D9
	.string "A victory snatched from the ARENA\n"
	.string "TYCOON, and a seven-Trainer sweep!\p"
	.string "We congratulate you on your most\n"
	.string "splendid challenge!$"

BattleFrontier_BattleArenaLobby_Text_PleaseAcceptBattlePoints: @ 8257353
	.string "My dear challenger, in recognition of\n"
	.string "your indefatigable spirit, please\l"
	.string "accept these Battle Point(s).$"
