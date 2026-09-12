Route134_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Route134_OnResume
	.byte 0

Route134_OnResume:
	setdivewarp MAP_UNDERWATER_ROUTE134, 8, 6
	end

Route134_EventScript_Jack::
	msgbox Route134_Text_JackPostBattle, MSGBOX_NPC
	end

Route134_EventScript_Laurel::
	msgbox Route134_Text_LaurelPostBattle, MSGBOX_NPC
	end

Route134_EventScript_Alex::
	msgbox Route134_Text_AlexPostBattle, MSGBOX_NPC
	end

Route134_EventScript_Aaron::
	trainerbattle_double TRAINER_AARON, Route134_Text_AaronIntro, Route134_Text_AaronDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route134_Text_AaronPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Hitoshi::
	msgbox Route134_Text_HITOSHI_LocalConversation, MSGBOX_NPC
	end

Route134_EventScript_Hudson::
	trainerbattle_double TRAINER_HUDSON, Route134_Text_HudsonIntro, Route134_Text_HudsonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route134_Text_HudsonPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Reyna::
	msgbox Route134_Text_ReynaPostBattle, MSGBOX_NPC
	end

Route134_EventScript_Marley::
	trainerbattle_double TRAINER_MARLEY, Route134_Text_MarleyIntro, Route134_Text_MarleyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route134_Text_MarleyPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Kelvin::
	msgbox Route134_Text_KelvinPostBattle, MSGBOX_NPC
	end

Route134_Text_HITOSHI_LocalConversation:
	.string "Even MACHAMP takes a rest day.\n"
	.string "It rests four arms. I rest two.$"
