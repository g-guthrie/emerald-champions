Route110_TrickHousePuzzle1_MapScripts:: @ 826B90F
	map_script MAP_SCRIPT_ON_LOAD, Route110_TrickHousePuzzle1_OnLoad
	.byte 0

Route110_TrickHousePuzzle1_OnLoad: @ 826B915
	compare VAR_TRICK_HOUSE_PUZZLE_1_STATE, 2
	goto_if_eq Route110_TrickHousePuzzle1_EventScript_OpenDoor
	end

Route110_TrickHousePuzzle1_EventScript_OpenDoor:: @ 826B921
	setmetatile 13, 1, METATILE_TrickHousePuzzle_Stairs_Down, 0
	end

Route110_TrickHousePuzzle1_EventScript_Scroll:: @ 826B92B
	lockall
	compare VAR_TRICK_HOUSE_PUZZLE_1_STATE, 0
	goto_if_eq Route110_TrickHousePuzzle1_EventScript_FoundScroll
	goto Route110_TrickHousePuzzle_EventScript_ReadScrollAgain
	end

Route110_TrickHousePuzzle1_EventScript_FoundScroll:: @ 826B93D
	setvar VAR_TRICK_HOUSE_PUZZLE_1_STATE, 1
	goto Route110_TrickHousePuzzle_EventScript_FoundScroll
	end

Route110_TrickHousePuzzle1_EventScript_Sally:: @ 826B948
	trainerbattle_single TRAINER_SALLY, Route110_TrickHousePuzzle1_Text_SallyIntro, Route110_TrickHousePuzzle1_Text_SallyDefeat
	msgbox Route110_TrickHousePuzzle1_Text_SallyPostBattle, MSGBOX_AUTOCLOSE
	end

Route110_TrickHousePuzzle1_EventScript_Eddie:: @ 826B95F
	trainerbattle_single TRAINER_EDDIE, Route110_TrickHousePuzzle1_Text_EddieIntro, Route110_TrickHousePuzzle1_Text_EddieDefeat
	msgbox Route110_TrickHousePuzzle1_Text_EddiePostBattle, MSGBOX_AUTOCLOSE
	end

Route110_TrickHousePuzzle1_EventScript_Robin:: @ 826B976
	trainerbattle_single TRAINER_ROBIN, Route110_TrickHousePuzzle1_Text_RobinIntro, Route110_TrickHousePuzzle1_Text_RobinDefeat
	msgbox Route110_TrickHousePuzzle1_Text_RobinPostBattle, MSGBOX_AUTOCLOSE
	end

Route110_TrickHousePuzzle1_Text_WroteSecretCodeLockOpened:: @ 826B98D
	.string "{PLAYER} wrote down the secret code\n"
	.string "on the door.\p"
	.string "“Trick Master is fabulous.”\n"
	.string "… … … … … … … …\p"
	.string "The lock clicked open!$"

Route110_TrickHousePuzzle1_Text_SallyIntro: @ 826B9FB
	.string "Cut taught me every blade has a\n"
	.string "different edge. Try all four!$"

Route110_TrickHousePuzzle1_Text_SallyDefeat: @ 826BA3F
	.string "You blunted every blade!$"

Route110_TrickHousePuzzle1_Text_SallyPostBattle: @ 826BA57
	.string "Even the sharpest edge has a\n"
	.string "weak side. That's the real trick!$"

Route110_TrickHousePuzzle1_Text_EddieIntro: @ 826BA82
	.string "I got lost… Then my team made\n"
	.string "every way out disappear!$"

Route110_TrickHousePuzzle1_Text_EddieDefeat: @ 826BAB0
	.string "That was the wrong way, too!$"

Route110_TrickHousePuzzle1_Text_EddiePostBattle: @ 826BAC3
	.string "The floating symbol points both\n"
	.string "ways… That's not helping!$"

Route110_TrickHousePuzzle1_Text_RobinIntro: @ 826BB10
	.string "I wondered who the Trick Master is…\n"
	.string "Then lost track of who I watched!$"

Route110_TrickHousePuzzle1_Text_RobinDefeat: @ 826BB2E
	.string "I followed the wrong thought…$"

Route110_TrickHousePuzzle1_Text_RobinPostBattle: @ 826BB52
	.string "The obvious target feeds the gear.\n"
	.string "Maybe I should watch the quiet one!$"
