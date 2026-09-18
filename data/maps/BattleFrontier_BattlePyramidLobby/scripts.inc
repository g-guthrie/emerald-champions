.set LOCALID_ATTENDANT, 1
.set LOCALID_HINT_GIVER, 2

@ Emerald Champions: the Battle Pyramid is retired. src/battle_pyramid.c, the
@ Pyramid Bag and the Pyramid's own maps are gone from this tree, so the challenge
@ flow that used to live here (challenge-status dispatch, party select, save,
@ battle-bag hand-over, floor warp) and the hint giver's fortune-telling have been
@ removed rather than left calling missing code. The attendant and the hint giver
@ still respond and explain that the Pyramid is closed.

BattleFrontier_BattlePyramidLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattleDomeLobby_OnWarp
	.byte 0

@ Unused. Pyramid uses Dome's OnWarp (presumably by mistake). Their effects are identical
BattleFrontier_BattlePyramidLobby_OnWarp: @ 8250721
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattlePyramidLobby_EventScript_TurnPlayerNorth
	.2byte 0

BattleFrontier_BattlePyramidLobby_EventScript_TurnPlayerNorth: @ 825072B
	setvar VAR_TEMP_1, 1
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	end

BattleFrontier_BattlePyramidLobby_EventScript_Attendant::
	lock
	faceplayer
	msgbox BattleFrontier_BattlePyramidLobby_Text_WelcomeToBattlePyramid, MSGBOX_DEFAULT
	msgbox BattleFrontier_BattlePyramidLobby_Text_PyramidClosed, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattlePyramidLobby_EventScript_HintGiver::
	lockall
	applymovement LOCALID_HINT_GIVER, Common_Movement_FacePlayer
	waitmovement 0
	msgbox BattleFrontier_BattlePyramidLobby_Text_NoFortuneToTell, MSGBOX_DEFAULT
	releaseall
	end

BattleFrontier_BattlePyramidLobby_EventScript_ShowResults::
	lockall
	frontier_results FRONTIER_FACILITY_PYRAMID
	waitbuttonpress
	special RemoveRecordsWindow
	releaseall
	end

BattleFrontier_BattlePyramidLobby_EventScript_Woman::
	msgbox BattleFrontier_BattlePyramidLobby_Text_TrainersNoticeRunning, MSGBOX_NPC
	end

BattleFrontier_BattlePyramidLobby_EventScript_FatMan:: @ 8250E8C
	msgbox BattleFrontier_BattlePyramidLobby_Text_LostLotOfItems, MSGBOX_NPC
	end

BattleFrontier_BattlePyramidLobby_EventScript_RulesBoard:: @ 8250E95
	lockall
	msgbox BattleFrontier_BattlePyramidLobby_Text_RulesAreListed, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePyramidLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePyramidLobby_EventScript_ReadRulesBoard:: @ 8250EA4
	message BattleFrontier_BattlePyramidLobby_Text_ReadWhichHeading
	waitmessage
	multichoice 15, 2, MULTI_BATTLE_PYRAMID_RULES, FALSE
	switch VAR_RESULT
	case 0, BattleFrontier_BattlePyramidLobby_EventScript_RulesPokemon
	case 1, BattleFrontier_BattlePyramidLobby_EventScript_RulesTrainers
	case 2, BattleFrontier_BattlePyramidLobby_EventScript_RulesMaze
	case 3, BattleFrontier_BattlePyramidLobby_EventScript_RulesBag
	case 4, BattleFrontier_BattlePyramidLobby_EventScript_ExitRules
	case MULTI_B_PRESSED, BattleFrontier_BattlePyramidLobby_EventScript_ExitRules
	end

BattleFrontier_BattlePyramidLobby_EventScript_RulesPokemon:: @ 8250EF7
	msgbox BattleFrontier_BattlePyramidLobby_Text_ExplainMonRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePyramidLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePyramidLobby_EventScript_RulesTrainers:: @ 8250F05
	msgbox BattleFrontier_BattlePyramidLobby_Text_ExplainTrainerRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePyramidLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePyramidLobby_EventScript_RulesMaze:: @ 8250F13
	msgbox BattleFrontier_BattlePyramidLobby_Text_ExplainMazeRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePyramidLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePyramidLobby_EventScript_RulesBag:: @ 8250F21
	msgbox BattleFrontier_BattlePyramidLobby_Text_ExplainBagRules, MSGBOX_DEFAULT
	goto BattleFrontier_BattlePyramidLobby_EventScript_ReadRulesBoard
	end

BattleFrontier_BattlePyramidLobby_EventScript_ExitRules:: @ 8250F2F
	releaseall
	end

BattleFrontier_BattlePyramidLobby_Text_PyramidClosed:
	.string "Regrettably, the PYRAMID is sealed,\n"
	.string "and we cannot take challengers.\p"
	.string "Until the way is opened again, your\n"
	.string "Pokémon may keep their own items.\p"
	.string "Please enjoy the rest of the\n"
	.string "BATTLE FRONTIER!$"

BattleFrontier_BattlePyramidLobby_Text_NoFortuneToTell:
	.string "I would tell you what misfortunes\n"
	.string "await you inside the PYRAMID…\p"
	.string "But no one is going inside, so there\n"
	.string "is no misfortune to foretell.\p"
	.string "Believe my fortunes or not…$"

BattleFrontier_BattlePyramidLobby_Text_WelcomeToBattlePyramid: @ 8250F31
	.string "Where the courage of Trainers\n"
	.string "is put to the test!\p"
	.string "Welcome to the BATTLE PYRAMID!\p"
	.string "I am your guide to\n"
	.string "the Battle Quest.$"

BattleFrontier_BattlePyramidLobby_Text_EmbarkOnChallenge: @ 8250FA7
	.string "Have you the courage to embark on\n"
	.string "the Battle Quest challenge?$"

BattleFrontier_BattlePyramidLobby_Text_AwaitFutureChallenge: @ 8250FE5
	.string "We await your challenge in the future!$"

BattleFrontier_BattlePyramidLobby_Text_ExplainBattlePyramid: @ 825100C
	.string "The Battle Quest is a battling\n"
	.string "event in which you must explore\l"
	.string "the PYRAMID and try to reach the top.\p"
	.string "In the PYRAMID are seven floors of\n"
	.string "mazes, all of them filled with not only\l"
	.string "skilled Trainers but also wild Pokémon.\p"
	.string "You must enter with three Pokémon,\n"
	.string "none of them holding an item.\p"
	.string "On your ascent, you will be provided\n"
	.string "with a BATTLE Bag for items.\p"
	.string "If you fail in your challenge, any item\n"
	.string "placed in the BATTLE Bag disappears.\p"
	.string "If you want to interrupt your\n"
	.string "challenge, you must select “REST”\l"
	.string "and save the game.\p"
	.string "If you don't save before interrupting,\n"
	.string "you will be disqualified.$"

BattleFrontier_BattlePyramidLobby_Text_WhichLevelMode: @ 8251248
	.string "The PYRAMID offers two courses,\n"
	.string "Level 50 and Open Level.\l"
	.string "Which will you enter?$"

BattleFrontier_BattlePyramidLobby_Text_SelectThreeMons: @ 8251297
	.string "Very good. Now, please select the three\n"
	.string "Pokémon you wish to accompany you.$"

BattleFrontier_BattlePyramidLobby_Text_NotEnoughValidMonsLvOpen: @ 82512E2
	.string "A slight problem, adventurer!\p"
	.string "You seem to not have the three\n"
	.string "Pokémon qualified for the challenge.\p"
	.string "Please also remember to take all\n"
	.string "items from your Pokémon.\p"
	.string "EGGS{STR_VAR_1} ineligible.\p"
	.string "When you are ready, please have\n"
	.string "a word with me.$"

BattleFrontier_BattlePyramidLobby_Text_NotEnoughValidMonsLv50: @ 82513C1
	.string "A slight problem, adventurer!\p"
	.string "You seem to not have the three\n"
	.string "Pokémon qualified for the challenge.\p"
	.string "The three Pokémon must be different\n"
	.string "and each no higher than Level 50.\p"
	.string "Please also remember to take all\n"
	.string "items from your Pokémon.\p"
	.string "EGGS{STR_VAR_1} ineligible.\p"
	.string "When you are ready, please have\n"
	.string "a word with me.$"

BattleFrontier_BattlePyramidLobby_Text_OkayToSaveBeforeChallenge: @ 82514E6
	.string "Before you enter the BATTLE PYRAMID,\n"
	.string "the game must be saved. Is that okay?$"

BattleFrontier_BattlePyramidLobby_Text_ShowYouIntoPyramid: @ 8251531
	.string "Very good. I will now show you into\n"
	.string "the BATTLE PYRAMID.$"

BattleFrontier_BattlePyramidLobby_Text_WeWillHoldBagForSafekeeping: @ 8251569
	.string "We will hold your Bag for safekeeping,\n"
	.string "{PLAYER}, while you are exploring.$"

BattleFrontier_BattlePyramidLobby_Text_PleaseTakePreviousBattleBag: @ 82515AD
	.string "In exchange, please take this BATTLE\n"
	.string "BAG, the one you used previously.$"

BattleFrontier_BattlePyramidLobby_Text_PleaseTakeThisBattleBag: @ 82515F4
	.string "In exchange, please take this\n"
	.string "BATTLE Bag.$"

BattleFrontier_BattlePyramidLobby_Text_ExchangedBagForBattleBag: @ 825161E
	.string "{PLAYER} exchanged the Bag for\n"
	.string "the BATTLE Bag.$"

BattleFrontier_BattlePyramidLobby_Text_StepOnFloorPanel: @ 8251647
	.string "When you step on this floor panel,\n"
	.string "you will be transported to a higher\l"
	.string "floor in the PYRAMID.\p"
	.string "I hope for your sake that your\n"
	.string "quest goes safely!$"

BattleFrontier_BattlePyramidLobby_Text_DidntSaveBeforeQuittingTakeBag: @ 82516D6
	.string "A major problem, explorer!\p"
	.string "You did not save before ending\n"
	.string "your challenge the last time.\p"
	.string "Consequently, your challenge has\n"
	.string "been disqualified, I'm sorry to say.\p"
	.string "Here is the Bag we've been holding\n"
	.string "for you.\p"
	.string "{PLAYER} got the Bag back.$"

BattleFrontier_BattlePyramidLobby_Text_YouveConqueredPyramid: @ 82517B5
	.string "Excellent to see you back!\p"
	.string "You've conquered the PYRAMID!\n"
	.string "How splendid!$"

BattleFrontier_BattlePyramidLobby_Text_MonHoldingItemCannotTake: @ 82517FC
	.string "Ah, a slight problem.\p"
	.string "At least one Pokémon is holding\n"
	.string "an item.\p"
	.string "I'm sorry to say, items obtained in\n"
	.string "the PYRAMID cannot be taken away.$"

BattleFrontier_BattlePyramidLobby_Text_HeldItemsMovedToBag: @ 8251881
	.string "All items held by your Pokémon will be\n"
	.string "moved to your BATTLE Bag, {PLAYER}.$"

BattleFrontier_BattlePyramidLobby_Text_BagCannotHoldPickItemsToKeep: @ 82518C6
	.string "The BATTLE Bag cannot hold all your\n"
	.string "items, I'm sorry to say.\p"
	.string "Please pick the items you'll keep in the\n"
	.string "BATTLE Bag, and with your Pokémon.$"

BattleFrontier_BattlePyramidLobby_Text_LeastOneMonHoldingItem: @ 825194F
	.string "At least one Pokémon is still\n"
	.string "holding an item.$"

BattleFrontier_BattlePyramidLobby_Text_PickItemsToKeep: @ 825197E
	.string "Please pick the items you'll keep in the\n"
	.string "BATTLE Bag, and with your Pokémon.$"

@ Unused
BattleFrontier_BattlePyramidLobby_Text_ReturnedEverythingMonsHeld: @ 82519CA
	.string "{PLAYER} returned everything that\n"
	.string "the Pokémon held.$"

BattleFrontier_BattlePyramidLobby_Text_UsedBattleBagWillBeKept: @ 82519F8
	.string "The BATTLE Bag you used will be kept\n"
	.string "in readiness for your next challenge.\p"
	.string "{PLAYER} turned the BATTLE Bag over\n"
	.string "for the Bag's return.$"

BattleFrontier_BattlePyramidLobby_Text_RecordResultsWait: @ 8251A77
	.string "I must record your results.\n"
	.string "Please wait.$"

@ Unused
BattleFrontier_BattlePyramidLobby_Text_ForConqueringPyramidTakeThis: @ 8251AA0
	.string "As a memento for conquering\n"
	.string "the BATTLE PYRAMID, please take this.$"

@ Unused
BattleFrontier_BattlePyramidLobby_Text_ReceivedPrizeItem: @ 8251AE2
	.string "{PLAYER} received the prize\n"
	.string "{STR_VAR_1}.$"

@ Unused
BattleFrontier_BattlePyramidLobby_Text_BagIsFull: @ 8251AFC
	.string "…Ah…\n"
	.string "Your Bag appears to be filled.\p"
	.string "Please return after you've organized\n"
	.string "your Bag's contents.$"

BattleFrontier_BattlePyramidLobby_Text_DisappointingHereIsBag: @ 8251B5A
	.string "How disappointing for you…\p"
	.string "Here is the Bag we've been holding\n"
	.string "for you.\p"
	.string "{PLAYER} got the Bag back.$"

BattleFrontier_BattlePyramidLobby_Text_LookForwardToNextChallenge: @ 8251BB6
	.string "We look forward to your\n"
	.string "next challenge!$"

@ Unused
BattleFrontier_BattlePyramidLobby_Text_HereIsPrize: @ 8251BDE
	.string "We have been looking forward to\n"
	.string "your arrival!\p"
	.string "Here is your prize for conquering\n"
	.string "the PYRAMID.$"

BattleFrontier_BattlePyramidLobby_Text_TellYouWhatMisfortunesAwait: @ 8251C3B
	.string "Welcome…\p"
	.string "I shall be pleased to tell you what\n"
	.string "misfortunes await in the PYRAMID…$"

BattleFrontier_BattlePyramidLobby_Text_Aah: @ 8251C8A
	.string "… … … … … …\n"
	.string "… … … … … …\p"
	.string "… … … … … …\n"
	.string "Aah!$"

BattleFrontier_BattlePyramidLobby_Text_HintParalysis: @ 8251CB3
	.string "I see a shower of sparks…\p"
	.string "…And in it, I see your Pokémon\n"
	.string "struggling with paralysis…$"

BattleFrontier_BattlePyramidLobby_Text_HintPoison: @ 8251D07
	.string "I see poison…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from the effects of poison…$"

BattleFrontier_BattlePyramidLobby_Text_HintBurn: @ 8251D54
	.string "I see bright red flames…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from burns…$"

BattleFrontier_BattlePyramidLobby_Text_HintPPWaste: @ 8251D9C
	.string "I sense the tremendous pressure of\n"
	.string "unrequited anger…\p"
	.string "It is a curse…\p"
	.string "…And, I see your Pokémon drained of\n"
	.string "Power Points and having no recourse\l"
	.string "but to use STRUGGLE…$"

BattleFrontier_BattlePyramidLobby_Text_HintLevitate: @ 8251E3D
	.string "I see Pokémon loftily airborne…\p"
	.string "…And, I see your Pokémon frustrated\n"
	.string "by powerless GROUND-type moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintTrapAbility: @ 8251EA1
	.string "I sense terrific energy rising from\n"
	.string "the ground below…\p"
	.string "…And, I see your Pokémon unable to\n"
	.string "escape the power's clutches…$"

BattleFrontier_BattlePyramidLobby_Text_HintIce: @ 8251F17
	.string "I see ICE-type Pokémon…\p"
	.string "…And, I see your Pokémon fighting\n"
	.string "the freezing effects of ice…$"

BattleFrontier_BattlePyramidLobby_Text_HintExplosion: @ 8251F6E
	.string "I see a flurry of moves that imperil\n"
	.string "the user…\p"
	.string "…And, I see your Pokémon falling\n"
	.string "to them…$"

BattleFrontier_BattlePyramidLobby_Text_HintPsychic: @ 8251FC7
	.string "I see Psychic-type Pokémon…\p"
	.string "…And, I see your Pokémon in torment\n"
	.string "from PSYCHIC moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintRock: @ 825201B
	.string "I see ROCK-type Pokémon…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from ROCK moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintFighting: @ 8252068
	.string "I see FIGHTING-type Pokémon…\p"
	.string "…And, I see your Pokémon pummeled\n"
	.string "by FIGHTING moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintWeather: @ 82520BA
	.string "RAIN DANCE… SUNNY DAY…\n"
	.string "SANDSTORM… HAIL…\p"
	.string "I see Pokémon that become stronger\n"
	.string "with the weather…\p"
	.string "…And, I see your Pokémon confounded\n"
	.string "by different types of moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintBug: @ 8252158
	.string "I see BUG-type Pokémon…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from different kinds of attacks…$"

BattleFrontier_BattlePyramidLobby_Text_HintDark: @ 82521B4
	.string "I see DARK-type Pokémon…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from DARK-type moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintWater: @ 8252206
	.string "I see WATER-type Pokémon…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from WATER-type moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintGhost: @ 825225A
	.string "I see Ghost-type Pokémon…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from Ghost-type moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintSteel: @ 82522AE
	.string "I see STEEL-type Pokémon…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from enormously powerful moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintFlyingDragon: @ 825230B
	.string "I see flying Pokémon…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from enormously powerful moves…$"

BattleFrontier_BattlePyramidLobby_Text_HintStoneEvolve: @ 8252364
	.string "I see those that have evolved from\n"
	.string "the power of stones…\p"
	.string "I also sense fire, water,\n"
	.string "and electricity…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from those three powers…$"

BattleFrontier_BattlePyramidLobby_Text_HintNormal: @ 8252403
	.string "I see NORMAL-type Pokémon…\p"
	.string "…And, I see your Pokémon suffering\n"
	.string "from enormously powerful moves…$"

BattleFrontier_BattlePyramidLobby_Text_BelieveMyFortunesOrNot: @ 8252461
	.string "Whether you believe my fortunes\n"
	.string "or not, the choice is yours…\p"
	.string "The future can be changed anytime…\n"
	.string "I wish you safe passage…$"

BattleFrontier_BattlePyramidLobby_Text_TrainersNoticeRunning: @ 82524DA
	.string "Did you know?\p"
	.string "If you run fast, Trainers may notice\n"
	.string "and come after you for a battle.\p"
	.string "So, if you want to avoid TRAINERS,\n"
	.string "don't catch their eyes, but sneak\l"
	.string "cautiously and quietly past them.$"

BattleFrontier_BattlePyramidLobby_Text_LostLotOfItems: @ 8252595
	.string "Awaaaaaaarrrrgh!\p"
	.string "I had a whole lot of items, but I lost\n"
	.string "them all when I lost!\p"
	.string "Awaaaaaaarrrrgh!$"

BattleFrontier_BattlePyramidLobby_Text_YouveDefeatedPyramidKing: @ 82525F4
	.string "Welcome back!\n"
	.string "You've done the unthinkable!\p"
	.string "You've defeated the PYRAMID KING\n"
	.string "and conquered the BATTLE PYRAMID!$"

BattleFrontier_BattlePyramidLobby_Text_GiveYouTheseBattlePoints: @ 8252662
	.string "Young explorer!\n"
	.string "In commendation of your courage,\l"
	.string "we give you these Battle Point(s)!$"

BattleFrontier_BattlePyramidLobby_Text_RulesAreListed: @ 82526B6
	.string "The Battle Quest rules are listed.$"

BattleFrontier_BattlePyramidLobby_Text_ReadWhichHeading: @ 82526D9
	.string "Which heading do you want to read?$"

BattleFrontier_BattlePyramidLobby_Text_ExplainMonRules: @ 82526FC
	.string "When the PYRAMID is conquered,\n"
	.string "the wild Pokémon that appear in it\l"
	.string "are replaced by different kinds.\p"
	.string "Explore, observe, and learn what kinds\n"
	.string "of wild Pokémon you may encounter.$"

BattleFrontier_BattlePyramidLobby_Text_ExplainTrainerRules: @ 82527A9
	.string "Trainers are lying in wait for you\n"
	.string "inside the PYRAMID.\p"
	.string "On each floor, there are up to\n"
	.string "eight Trainers.\p"
	.string "When you defeat a Trainer, you will\n"
	.string "get a helpful hint for your adventure.$"

BattleFrontier_BattlePyramidLobby_Text_ExplainMazeRules: @ 825285A
	.string "The mazes in the PYRAMID rearrange\n"
	.string "themselves every time you enter it.\p"
	.string "The mazes are poorly lit.\n"
	.string "You must advance with caution.\p"
	.string "The light grows brighter whenever you\n"
	.string "defeat a wild Pokémon or a Trainer.$"

BattleFrontier_BattlePyramidLobby_Text_ExplainBagRules: @ 8252924
	.string "The BATTLE Bag serves as your Bag\n"
	.string "while in the PYRAMID.\p"
	.string "There are two separate BATTLE BAGS--\n"
	.string "one each for the Level 50 and\l"
	.string "Open Level adventures.\p"
	.string "They each can hold up to 99 pieces\n"
	.string "of ten kinds of items.\p"
	.string "The contents of the BATTLE Bag are\n"
	.string "lost if you fail in your quest.$"

