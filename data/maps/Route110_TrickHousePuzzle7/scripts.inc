Route110_TrickHousePuzzle7_MapScripts:: @ 826E091
	map_script MAP_SCRIPT_ON_RESUME, Route110_TrickHousePuzzle7_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, Route110_TrickHousePuzzle7_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route110_TrickHousePuzzle7_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route110_TrickHousePuzzle7_OnFrame
	.byte 0

@ Puzzle Room 7 in RSE uses whatever puzzle Mossdeep Gym uses
@ Because Mossdeep Gym was redesigned for Emerald, theres a good deal of leftover script from the old R/S puzzle

Route110_TrickHousePuzzle7_OnResume: @ 826E0A6
	call Route110_TrickHousePuzzle7_EventScript_UpdateSwitchMetatiles
	end

Route110_TrickHousePuzzle7_EventScript_UpdateSwitchMetatiles:: @ 826E0AC
	call_if_set FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_1, Route110_TrickHousePuzzle7_EventScript_SetSwitch1MetatilesOn
	call_if_set FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_2, Route110_TrickHousePuzzle7_EventScript_SetSwitch2MetatilesOn
	call_if_set FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_3, Route110_TrickHousePuzzle7_EventScript_SetSwitch3MetatilesOn
	call_if_set FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_4, Route110_TrickHousePuzzle7_EventScript_SetSwitch4MetatilesOn
	call_if_set FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_5, Route110_TrickHousePuzzle7_EventScript_SetSwitch5MetatilesOn
	return

@ Leftover from R/S, none of the below metatile scripts are ever called
Route110_TrickHousePuzzle7_EventScript_SetSwitch1MetatilesOn:: @ 826E0DA
	setmetatile 13, 17, METATILE_TrickHousePuzzle_Arrow_RedOnBlack_Up, 0
	setmetatile 12, 16, METATILE_TrickHousePuzzle_Lever_On, 1
	return

Route110_TrickHousePuzzle7_EventScript_SetSwitch2MetatilesOn:: @ 826E0ED
	setmetatile 12, 13, METATILE_TrickHousePuzzle_Arrow_RedOnBlack_Up, 0
	setmetatile 12, 11, METATILE_TrickHousePuzzle_Lever_On, 1
	return

Route110_TrickHousePuzzle7_EventScript_SetSwitch3MetatilesOn:: @ 826E100
	setmetatile 7, 12, METATILE_TrickHousePuzzle_Arrow_RedOnBlack_Up, 0
	setmetatile 5, 10, METATILE_TrickHousePuzzle_Lever_On, 1
	return

Route110_TrickHousePuzzle7_EventScript_SetSwitch4MetatilesOn:: @ 826E113
	setmetatile 6, 6, METATILE_TrickHousePuzzle_Arrow_RedOnBlack_Right_Alt, 0
	setmetatile 4, 4, METATILE_TrickHousePuzzle_Lever_On, 1
	return

Route110_TrickHousePuzzle7_EventScript_SetSwitch5MetatilesOn:: @ 826E126
	setmetatile 8, 4, METATILE_TrickHousePuzzle_Arrow_RedOnBlack_Left, 0
	setmetatile 7, 5, METATILE_TrickHousePuzzle_Lever_On, 1
	return

Route110_TrickHousePuzzle7_OnTransition: @ 826E198
	compare VAR_TRICK_HOUSE_PUZZLE_7_STATE_2, 1
	goto_if_eq Route110_TrickHousePuzzle7_EventScript_TeleportedTransition
	clearflag FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_1
	clearflag FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_2
	clearflag FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_3
	clearflag FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_4
	clearflag FLAG_TRICK_HOUSE_PUZZLE_7_SWITCH_5
	end

Route110_TrickHousePuzzle7_EventScript_TeleportedTransition:: @ 826E1B3
	end

Route110_TrickHousePuzzle7_OnLoad: @ 826E1B4
	compare VAR_TRICK_HOUSE_PUZZLE_7_STATE_2, 1
	call_if_eq Route110_TrickHousePuzzle7_EventScript_UpdateSwitchMetatiles
	end

Route110_TrickHousePuzzle7_OnFrame: @ 826E1C0
	map_script_2 VAR_TRICK_HOUSE_PUZZLE_7_STATE_2, 1, Route110_TrickHousePuzzle7_EventScript_ClearState2
	.2byte 0

Route110_TrickHousePuzzle7_EventScript_ClearState2:: @ 826E1CA
	setvar VAR_TRICK_HOUSE_PUZZLE_7_STATE_2, 0
	end

Route110_TrickHousePuzzle7_EventScript_Scroll:: @ 826E1D0
	lockall
	compare VAR_TRICK_HOUSE_PUZZLE_7_STATE, 0
	goto_if_eq Route110_TrickHousePuzzle7_EventScript_FoundScroll
	goto Route110_TrickHousePuzzle_EventScript_ReadScrollAgain
	end

Route110_TrickHousePuzzle7_EventScript_FoundScroll:: @ 826E1E2
	setvar VAR_TRICK_HOUSE_PUZZLE_7_STATE, 1
	goto Route110_TrickHousePuzzle_EventScript_FoundScroll
	end

@ Unused, leftover from R/S
Route110_TrickHousePuzzle7_EventScript_YellowButton:: @ 826E31B
	playse SE_SWITCH
	waitse
	playse SE_REPEL
	initrotatingtilepuzzle TRUE
	moverotatingtileobjects 0
	waitmovement 0
	turnrotatingtileobjects
	waitmovement 0
	freerotatingtilepuzzle
	end

Route110_TrickHousePuzzle7_EventScript_BlueButton:: @ 826E331
	playse SE_SWITCH
	waitse
	playse SE_REPEL
	initrotatingtilepuzzle TRUE
	moverotatingtileobjects 1
	waitmovement 0
	turnrotatingtileobjects
	waitmovement 0
	freerotatingtilepuzzle
	end

Route110_TrickHousePuzzle7_EventScript_GreenButton:: @ 826E347
	playse SE_SWITCH
	waitse
	playse SE_REPEL
	initrotatingtilepuzzle TRUE
	moverotatingtileobjects 2
	waitmovement 0
	turnrotatingtileobjects
	waitmovement 0
	freerotatingtilepuzzle
	end

Route110_TrickHousePuzzle7_EventScript_PurpleButton:: @ 826E35D
	playse SE_SWITCH
	waitse
	playse SE_REPEL
	initrotatingtilepuzzle TRUE
	moverotatingtileobjects 3
	waitmovement 0
	turnrotatingtileobjects
	waitmovement 0
	freerotatingtilepuzzle
	end

@ Unused
Route110_TrickHousePuzzle7_EventScript_Alexis:: @ 826E3B7
	trainerbattle_double TRAINER_ALEXIS, Route110_TrickHousePuzzle7_Text_AlexisIntro, Route110_TrickHousePuzzle7_Text_AlexisDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route110_TrickHousePuzzle7_Text_AlexisPostBattle, MSGBOX_AUTOCLOSE
	end

Route110_TrickHousePuzzle7_EventScript_Mariela:: @ 826E3CE
	trainerbattle_double TRAINER_MARIELA, Route110_TrickHousePuzzle7_Text_MarielaIntro, Route110_TrickHousePuzzle7_Text_MarielaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route110_TrickHousePuzzle7_Text_MarielaPostBattle, MSGBOX_AUTOCLOSE
	end

Route110_TrickHousePuzzle7_EventScript_Everett:: @ 826E3FC
	trainerbattle_double TRAINER_EVERETT, Route110_TrickHousePuzzle7_Text_EverettIntro, Route110_TrickHousePuzzle7_Text_EverettDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route110_TrickHousePuzzle7_Text_EverettPostBattle, MSGBOX_AUTOCLOSE
	end

Route110_TrickHousePuzzle7_EventScript_WroteSecretCodeLockOpened:: @ 826E413
	.string "{PLAYER} wrote down the secret code\n"
	.string "on the door.\p"
	.string "“Trick Master is huggable.”\n"
	.string "… … … … … … … …\p"
	.string "The lock clicked open!$"

Route110_TrickHousePuzzle7_Text_AlexisIntro: @ 826E5C0
	.string "Whoever wins will get through here\n"
	.string "first. That's the feeling I get.$"

Route110_TrickHousePuzzle7_Text_AlexisDefeat: @ 826E604
	.string "Oh!\n"
	.string "Well, go ahead, then!$"

Route110_TrickHousePuzzle7_Text_AlexisPostBattle: @ 826E61E
	.string "You're solving all the puzzles in the\n"
	.string "Trick House. That's the feeling I get.$"

Route110_TrickHousePuzzle7_Text_MarielaIntro: @ 826E66B
	.string "Nufufufu, here at last!\n"
	.string "Let's get right with it!$"

Route110_TrickHousePuzzle7_Text_MarielaDefeat: @ 826E69C
	.string "You're so casual about winning!$"

Route110_TrickHousePuzzle7_Text_MarielaPostBattle: @ 826E6BC
	.string "Humph! I'm not upset!\n"
	.string "Not me!$"

Route110_TrickHousePuzzle7_Text_EverettIntro: @ 826E78D
	.string "It's awfully cramped in here…$"

Route110_TrickHousePuzzle7_Text_EverettDefeat: @ 826E7AB
	.string "Oh, yes, strong you are.$"

Route110_TrickHousePuzzle7_Text_EverettPostBattle: @ 826E7C4
	.string "I was hoping to switch places with you\n"
	.string "when I beat you, but…$"

