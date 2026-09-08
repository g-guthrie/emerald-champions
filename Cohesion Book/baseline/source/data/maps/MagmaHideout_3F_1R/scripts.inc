MagmaHideout_3F_1R_MapScripts::
	.byte 0

MagmaHideout_3F_1R_EventScript_Grunt9::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_9, MagmaHideout_3F_1R_Text_Grunt9Intro, MagmaHideout_3F_1R_Text_Grunt9Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_3F_1R_Text_Grunt9PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_3F_1R_EventScript_Grunt16::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_16, MagmaHideout_3F_1R_Text_Grunt16Intro, MagmaHideout_3F_1R_Text_Grunt16Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_3F_1R_Text_Grunt16PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_3F_1R_Text_Grunt9Intro:
	.string "Guard posting next to the lava. My left\n"
	.string "ear's been cooking for an hour.\p"
	.string "At least fighting you means I can move.$"

MagmaHideout_3F_1R_Text_Grunt9Defeat:
	.string "Heat exhaustion. Definitely the heat.$"

MagmaHideout_3F_1R_Text_Grunt9PostBattle:
	.string "Do you think it's weird that we wear\n"
	.string "hoods inside a volcano? Be honest.$"

MagmaHideout_3F_1R_Text_Grunt16Intro:
	.string "MAXIE offers a world people can predict,\n"
	.string "build on, and pass to their children.\p"
	.string "AQUA rejects it. You question it.\n"
	.string "I will defend it.$"

MagmaHideout_3F_1R_Text_Grunt16Defeat:
	.string "Your uncertainty outplayed my order…$"

MagmaHideout_3F_1R_Text_Grunt16PostBattle:
	.string "I still believe MAGMA's question matters.\n"
	.string "I am less certain our answer is complete.$"
