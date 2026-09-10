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
	.string "SHELLY: You reached the seafloor\n"
	.string "without a submarine… Impressive.\p"
	.string "First the WEATHER INSTITUTE, now\n"
	.string "here. You really intend to stop\l"
	.string "ARCHIE.\p"
	.string "Then you'll have to get past me\n"
	.string "again!$"

SeafloorCavern_Room3_Text_ShellyDefeat:
	.string "You've grown stronger since we\n"
	.string "met…$"

SeafloorCavern_Room3_Text_ShellyPostBattle:
	.string "SHELLY: We wanted more room for\n"
	.string "sea POKéMON to live. That's why I\l"
	.string "joined AQUA.\p"
	.string "ARCHIE is beyond this room. I hope\n"
	.string "he still remembers why we came.$"

SeafloorCavern_Room3_Text_Grunt5Intro:
	.string "Our submarine brought us this far.\n"
	.string "Your POKéMON brought you.\p"
	.string "They're tough, I'll give you that.\n"
	.string "But I'm not letting you through!$"

SeafloorCavern_Room3_Text_Grunt5Defeat:
	.string "You really did come prepared…$"

SeafloorCavern_Room3_Text_Grunt5PostBattle:
	.string "I joined AQUA because I love the\n"
	.string "sea.\p"
	.string "I thought I'd feel at home down\n"
	.string "here. Now I just want to see the\l"
	.string "sky again.$"
