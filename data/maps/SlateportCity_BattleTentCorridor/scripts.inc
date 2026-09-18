@ Emerald Champions: the Slateport Battle Tent is retired. It was the Battle
@ Factory's rental-Pokemon variant, and src/battle_factory.c is gone from this
@ tree, so the rental/swap flow that used to run in this corridor cannot run.
@ This map has no warp events of its own, so rather than leave the player stranded
@ in a dead corridor, entering it shows a short message and returns them to the
@ Battle Tent lobby with the challenge cleanly cancelled.

SlateportCity_BattleTentCorridor_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SlateportCity_BattleTentCorridor_OnFrame
	.byte 0

SlateportCity_BattleTentCorridor_OnFrame:
	map_script_2 VAR_TEMP_0, 0, SlateportCity_BattleTentCorridor_EventScript_ReturnToLobby
	.2byte 0

SlateportCity_BattleTentCorridor_EventScript_ReturnToLobby::
	lockall
	setvar VAR_TEMP_0, 1
	msgbox SlateportCity_BattleTentCorridor_Text_CorridorClosed, MSGBOX_DEFAULT
	closemessage
	frontier_set FRONTIER_DATA_CHALLENGE_STATUS, 0
	frontier_set FRONTIER_DATA_BATTLE_NUM, 0
	special LoadPlayerParty
	special HealPlayerParty
	warp MAP_SLATEPORT_CITY_BATTLE_TENT_LOBBY, 255, 6, 6
	waitstate
	releaseall
	end

SlateportCity_BattleTentCorridor_Text_CorridorClosed:
	.string "The rental Pokémon shelves have been\n"
	.string "cleared out.\p"
	.string "There's nothing here but a locked\n"
	.string "door. Better head back.$"

@ Leftover text from when this was a Contest Hall in R/S
@ Unused
SlateportCity_ContestHall_Text_AdviceForContests: @ 820906B
	.string "Want a tasty little bit of advice\n"
	.string "for Contests?\p"
	.string "Using a certain move after another\n"
	.string "certain kind of move sometimes gets\l"
	.string "you extra attention.\p"
	.string "If you know what you're doing,\n"
	.string "you can score big in appeal.\p"
	.string "Of course, your opponents might try\n"
	.string "disrupting your Pokémon's showing.$"

@ Unused
SlateportCity_ContestHall_Text_MyPapaIsContestJudge: @ 820917A
	.string "My papa, he's a Contest JUDGE.\p"
	.string "I wonder what I should be when I\n"
	.string "grow up, a JUDGE or a Gym Leader?$"

@ Unused
SlateportCity_ContestHall_Text_ImLikeMajorlyCheesed: @ 82091DC
	.string "Hey, man, I'm like majorly cheesed,\n"
	.string "you know. Like, you know, I just\l"
	.string "wanted to know why my Pokémon\l"
	.string "never won, you know, like, hey?\p"
	.string "So, like, I gave the JUDGE my two\n"
	.string "cents, you know, they're free.\p"
	.string "And he wouldn't hear me out, like, hey!\n"
	.string "So, like, total bummer, man!\p"
	.string "Hey, like, you! Zip it, you know?\n"
	.string "Just, you know, take this!$"

@ Unused
SlateportCity_ContestHall_Text_ExplainTorment: @ 8209322
	.string "A Rare Candy raises a Pokémon by\n"
	.string "one level, up to your current\l"
	.string "cap.\p"
	.string "For new moves, visit the Center\n"
	.string "tutor.$"

@ Unused
SlateportCity_ContestHall_Text_MCStepUpTakePartInContest: @ 82093ED
	.string "MC: Oh, my, my!\n"
	.string "Now isn't that a dandy of a Pokémon?\p"
	.string "Please! Do step right up and take\n"
	.string "part in our splendid Contests!\p"
	.string "You'll do well! I'm sure of it!\n"
	.string "My eyes have never failed me!$"

@ Unused
SlateportCity_ContestHall_Text_JudgeWouldntDoToMissContest: @ 82094A1
	.string "Judge: Well, hello there!\n"
	.string "I see that you're a Trainer!\p"
	.string "Then, it just wouldn't do for you\n"
	.string "to miss a Pokémon Contest!\p"
	.string "Get a Contest PASS in Verdanturf\n"
	.string "CITY and enter anytime!$"

@ Unused
SlateportCity_ContestHall_Text_ItsAppealTime: @ 820954E
	.string "It's appeal time!\n"
	.string "What should I lead with?$"

@ Unused
SlateportCity_ContestHall_Text_DidntPayAttentionToAppeal: @ 8209579
	.string "They didn't pay much attention to\n"
	.string "my Pokémon's appeal…\p"
	.string "Humph, that JUDGE, he doesn't know\n"
	.string "a good thing when he sees it.$"

@ Unused
SlateportCity_ContestHall_Text_RewardWithSageAdvice: @ 82095F1
	.string "Oh, hi! You must be a serious fan to get\n"
	.string "this close to the action.\p"
	.string "I'll reward your enthusiasm with\n"
	.string "this sage advice.\p"
	.string "If a move goes over really well,\n"
	.string "the audience will get excited.\p"
	.string "The Pokémon that makes its appeal\n"
	.string "right when everyone's excited…\p"
	.string "Well, you'd think something good has\n"
	.string "to happen!$"

@ Unused
SlateportCity_ContestHall_Text_MoreFreakedOutThanMon: @ 8209718
	.string "I can't do this! I'm more freaked out\n"
	.string "than my Pokémon.\p"
	.string "I'm shivering and my heart is racing!$"

@ Unused
SlateportCity_ContestHall_Text_BattleAndContestAlike: @ 8209775
	.string "A battle and a Contest aren't the\n"
	.string "same, but they are alike, too.\p"
	.string "You need to work hard and believe\n"
	.string "in the Pokémon you've raised.$"

@ Unused
SlateportCity_ContestHall_Text_MonLooksOnTopOfGame: @ 82097F6
	.string "That Pokémon looks like it's on top\n"
	.string "of its game, huh?\p"
	.string "A Pokémon that does good in the\n"
	.string "secondary judging seems to be more\l"
	.string "relaxed when it's doing appeals.$"

@ Unused
SlateportCity_ContestHall_Text_MyMonBetterThanThatLot: @ 8209890
	.string "Will you look at that sorry sight?\p"
	.string "Heh, my Pokémon's absolutely better\n"
	.string "than that lot!$"

@ Unused
SlateportCity_ContestHall_Text_GetUrgeToMoveWithMon: @ 82098E6
	.string "Don't you get the urge to move with\n"
	.string "Pokémon if they're putting on an\l"
	.string "energetic appeal?$"

@ Unused
SlateportCity_ContestHall_Text_HyperRankStage: @ 820993D
	.string "Pokémon Contests\n"
	.string "HYPER RANK STAGE!$"

