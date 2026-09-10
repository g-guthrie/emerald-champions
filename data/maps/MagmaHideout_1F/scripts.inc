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
	.string "This corner is my post. Nobody\n"
	.string "gets past it while I'm on duty!\p"
	.string "MAXIE is counting on us. I won't\n"
	.string "let him down!$"

MagmaHideout_1F_Text_Grunt1Defeat:
	.string "I couldn't hold my own post…$"

MagmaHideout_1F_Text_Grunt1PostBattle:
	.string "I thought standing guard would be\n"
	.string "the easy job.\p"
	.string "The digging crew warned me. I\n"
	.string "should have listened.$"

MagmaHideout_1F_Text_Grunt2Intro:
	.string "Our leader told us to dig into MT.\n"
	.string "CHIMNEY, so we dug and dug.\p"
	.string "What we found could give HOENN\n"
	.string "more land than we ever dreamed of!\p"
	.string "And you're not getting near it!$"

MagmaHideout_1F_Text_Grunt2Defeat:
	.string "All that digging wore me out…$"

MagmaHideout_1F_Text_Grunt2PostBattle:
	.string "MAXIE is in the deepest chamber.\n"
	.string "He says the BLUE ORB will awaken\l"
	.string "what we found.\p"
	.string "I hope he knows what he's doing.$"
