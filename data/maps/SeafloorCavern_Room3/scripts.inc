SeafloorCavern_Room3_MapScripts::
	.byte 0

SeafloorCavern_Room3_EventScript_Shelly::
	trainerbattle_double TRAINER_SHELLY_SEAFLOOR_CAVERN, SeafloorCavern_Room3_Text_ShellyIntro, SeafloorCavern_Room3_Text_ShellyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SeafloorCavern_Room3_Text_ShellyPostBattle, MSGBOX_AUTOCLOSE
	end

SeafloorCavern_Room3_EventScript_Grunt5::
	trainerbattle_double TRAINER_GRUNT_SEAFLOOR_CAVERN_5, SeafloorCavern_Room3_Text_Grunt5Intro, SeafloorCavern_Room3_Text_Grunt5Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SeafloorCavern_Room3_Text_Grunt5PostBattle, MSGBOX_AUTOCLOSE
	end

SeafloorCavern_Room3_Text_ShellyIntro:
	.string "SHELLY: You reached the seafloor without\n"
	.string "our submarine. I expected no less.\p"
	.string "At the INSTITUTE you read the weather.\n"
	.string "Here the cavern itself changes the field.\p"
	.string "Read the current before it closes.$"

SeafloorCavern_Room3_Text_ShellyDefeat:
	.string "You mapped the current while inside it…$"

SeafloorCavern_Room3_Text_ShellyPostBattle:
	.string "You would have been formidable in AQUA.\p"
	.string "But you keep choosing balance over our\n"
	.string "freedom. ARCHIE waits beyond this room.$"

SeafloorCavern_Room3_Text_Grunt5Intro:
	.string "AQUA needs POKéMON to reach a current\n"
	.string "human tools cannot follow.\p"
	.string "Your team carried you just as far.\n"
	.string "Only one formation advances.$"

SeafloorCavern_Room3_Text_Grunt5Defeat:
	.string "Your formation held deeper…$"

SeafloorCavern_Room3_Text_Grunt5PostBattle:
	.string "We followed ARCHIE because his dream\n"
	.string "made motion sound like freedom.\p"
	.string "You came this deep to ask whether a\n"
	.string "world without limits can survive.$"
