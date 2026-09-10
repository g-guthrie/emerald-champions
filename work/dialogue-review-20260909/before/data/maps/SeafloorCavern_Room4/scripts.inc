SeafloorCavern_Room4_MapScripts::
	.byte 0

SeafloorCavern_Room4_EventScript_Grunt3::
	trainerbattle_double TRAINER_GRUNT_SEAFLOOR_CAVERN_3, SeafloorCavern_Room4_Text_Grunt3Intro, SeafloorCavern_Room4_Text_Grunt3Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SeafloorCavern_Room4_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

SeafloorCavern_Room4_EventScript_Grunt4::
	trainerbattle_double TRAINER_GRUNT_SEAFLOOR_CAVERN_4, SeafloorCavern_Room4_Text_Grunt4Intro, SeafloorCavern_Room4_Text_Grunt4Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SeafloorCavern_Room4_Text_Grunt4PostBattle, MSGBOX_AUTOCLOSE
	end

SeafloorCavern_Room4_Text_Grunt3Intro:
	.string "Who are you? How did you even get\n"
	.string "down here without the sub?$"

SeafloorCavern_Room4_Text_Grunt3Defeat:
	.string "Lost it… and I'm lost.$"

SeafloorCavern_Room4_Text_Grunt3PostBattle:
	.string "I can't find the way out. I'm not\n"
	.string "scared. Don't get me wrong!$"

SeafloorCavern_Room4_Text_Grunt4Intro:
	.string "Who are you? Where do you think\n"
	.string "you're going? Not past TOXAPEX.$"

SeafloorCavern_Room4_Text_Grunt4Defeat:
	.string "I failed to win. Twice today.$"

SeafloorCavern_Room4_Text_Grunt4PostBattle:
	.string "My partner left the map in the sub.\n"
	.string "How's that for useless?$"

