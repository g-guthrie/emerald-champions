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
	.string "Those currents outside spun our sub\n"
	.string "around. You should turn back while\l"
	.string "you still know the way!$"

SeafloorCavern_Room1_Text_Grunt1Defeat:
	.string "You kept your footing better than me!$"

SeafloorCavern_Room1_Text_Grunt1PostBattle:
	.string "ARCHIE went deeper into the cavern.\n"
	.string "Those twisting passages ought to\l"
	.string "keep you busy for a while.$"

SeafloorCavern_Room1_Text_Grunt2Intro:
	.string "The channel outside was just wide\n"
	.string "enough for our sub. But this passage\l"
	.string "isn't wide enough for both of us!$"

SeafloorCavern_Room1_Text_Grunt2Defeat:
	.string "You opened the route again…$"

SeafloorCavern_Room1_Text_Grunt2PostBattle:
	.string "We left the submarine outside.\n"
	.string "ARCHIE took the crew farther in.\l"
	.string "I was supposed to guard the way...$"
