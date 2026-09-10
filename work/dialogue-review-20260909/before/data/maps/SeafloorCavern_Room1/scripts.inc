SeafloorCavern_Room1_MapScripts::
	.byte 0

SeafloorCavern_Room1_EventScript_Grunt1::
	trainerbattle_double TRAINER_GRUNT_SEAFLOOR_CAVERN_1, SeafloorCavern_Room1_Text_Grunt1Intro, SeafloorCavern_Room1_Text_Grunt1Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SeafloorCavern_Room1_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

SeafloorCavern_Room1_EventScript_Grunt2::
	trainerbattle_double TRAINER_GRUNT_SEAFLOOR_CAVERN_2, SeafloorCavern_Room1_Text_Grunt2Intro, SeafloorCavern_Room1_Text_Grunt2Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SeafloorCavern_Room1_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

SeafloorCavern_Room1_Text_Grunt1Intro:
	.string "This cavern changes direction with every\n"
	.string "current. Turn back while you still can.$"

SeafloorCavern_Room1_Text_Grunt1Defeat:
	.string "You made the shifting floor look fixed…$"

SeafloorCavern_Room1_Text_Grunt1PostBattle:
	.string "ARCHIE is farther below. Every room from\n"
	.string "here is meant to misdirect pursuit.$"

SeafloorCavern_Room1_Text_Grunt2Intro:
	.string "The submarine found one narrow route in.\n"
	.string "My formation seals it behind us.$"

SeafloorCavern_Room1_Text_Grunt2Defeat:
	.string "You opened the route again…$"

SeafloorCavern_Room1_Text_Grunt2PostBattle:
	.string "The stolen submarine is already deeper.\n"
	.string "You are following its pressure trail.$"
