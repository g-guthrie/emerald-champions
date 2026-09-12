Route110_TrickHousePuzzle4_MapScripts::
	.byte 0

Route110_TrickHousePuzzle4_EventScript_Scroll::
	lockall
	goto_if_eq VAR_TRICK_HOUSE_PUZZLE_4_STATE, 0, Route110_TrickHousePuzzle4_EventScript_FoundScroll
	goto Route110_TrickHousePuzzle_EventScript_ReadScrollAgain
	end

Route110_TrickHousePuzzle4_EventScript_FoundScroll::
	setvar VAR_TRICK_HOUSE_PUZZLE_4_STATE, 1
	goto Route110_TrickHousePuzzle_EventScript_FoundScroll
	end

Route110_TrickHousePuzzle4_EventScript_Cora::
	msgbox Route110_TrickHousePuzzle4_Text_CoraPostBattle, MSGBOX_NPC
	end

Route110_TrickHousePuzzle4_EventScript_Yuji::
	trainerbattle_double TRAINER_YUJI, Route110_TrickHousePuzzle4_Text_YujiIntro, Route110_TrickHousePuzzle4_Text_YujiDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route110_TrickHousePuzzle4_Text_YujiPostBattle, MSGBOX_AUTOCLOSE
	end

Route110_TrickHousePuzzle4_EventScript_Paula::
	msgbox Route110_TrickHousePuzzle4_Text_PaulaPostBattle, MSGBOX_NPC
	end

Route110_TrickHousePuzzle4_Text_WroteSecretCodeLockOpened:
	.string "{PLAYER} wrote down the secret code\n"
	.string "on the door.\p"
	.string "“TRICK MASTER is cool.”\n"
	.string "… … … … … … … …\p"
	.string "The lock clicked open!$"

Route110_TrickHousePuzzle4_Text_CoraPostBattle::
	.string "I enjoy meeting other TRAINERS\n"
	.string "while solving these puzzles.$"

Route110_TrickHousePuzzle4_Text_YujiIntro:
	.string "Heh! Boulders like this, I can brush\n"
	.string "aside with one finger!$"

Route110_TrickHousePuzzle4_Text_YujiDefeat:
	.string "I can push boulders, but I can't solve\n"
	.string "the puzzle…$"

Route110_TrickHousePuzzle4_Text_YujiPostBattle:
	.string "It's not good enough to be brawny…\n"
	.string "You have to use your head. Be brainy!$"

Route110_TrickHousePuzzle4_Text_PaulaPostBattle:
	.string "Has anyone made it to the end?$"

