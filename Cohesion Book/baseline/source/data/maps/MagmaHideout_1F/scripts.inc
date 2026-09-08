MagmaHideout_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MagmaHideout_1F_OnTransition
	.byte 0

MagmaHideout_1F_OnTransition:
	setvar VAR_JAGGED_PASS_ASH_WEATHER, 0
	end

MagmaHideout_1F_EventScript_Grunt1::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_1, MagmaHideout_1F_Text_Grunt1Intro, MagmaHideout_1F_Text_Grunt1Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_1F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_1F_EventScript_Grunt2::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_2, MagmaHideout_1F_Text_Grunt2Intro, MagmaHideout_1F_Text_Grunt2Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_1F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_1F_Text_Grunt1Intro:
	.string "Every guard has one fixed position.\n"
	.string "Mine is this corner and this approach.\p"
	.string "MAXIE says order fails when one person\n"
	.string "abandons their place. I will hold mine.$"

MagmaHideout_1F_Text_Grunt1Defeat:
	.string "I held the position, but not the battle…$"

MagmaHideout_1F_Text_Grunt1PostBattle:
	.string "A fixed post is useless if the opponent\n"
	.string "can change every approach.$"

MagmaHideout_1F_Text_Grunt2Intro:
	.string "Our leader told us to dig into\n"
	.string "MT. CHIMNEY, so we dug and dug.\p"
	.string "We found a chamber tied to the same\n"
	.string "network as the CHAMPION'S SIGNS.\p"
	.string "Beat my formation and see it yourself.$"

MagmaHideout_1F_Text_Grunt2Defeat:
	.string "The entrance line has fallen…$"

MagmaHideout_1F_Text_Grunt2PostBattle:
	.string "The chamber is deeper in. What MAXIE\n"
	.string "woke there may already be moving.$"
