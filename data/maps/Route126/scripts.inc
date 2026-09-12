Route126_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route126_OnTransition
	.byte 0

Route126_OnTransition:
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	end

Route126_EventScript_Barry::
	msgbox Route126_Text_BarryPostBattle, MSGBOX_NPC
	end

Route126_EventScript_Dean::
	msgbox Route126_Text_DeanPostBattle, MSGBOX_NPC
	end

Route126_EventScript_Nikki::
	trainerbattle_double TRAINER_NIKKI, Route126_Text_NikkiIntro, Route126_Text_NikkiDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route126_Text_NikkiPostBattle, MSGBOX_AUTOCLOSE
	end

Route126_EventScript_Brenda::
	trainerbattle_double TRAINER_BRENDA, Route126_Text_BrendaIntro, Route126_Text_BrendaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route126_Text_BrendaPostBattle, MSGBOX_AUTOCLOSE
	end

Route126_EventScript_Leonardo::
	msgbox Route126_Text_LEONARDO_LocalConversation, MSGBOX_NPC
	end

Route126_EventScript_Isobel::
	msgbox Route126_Text_ISOBEL_LocalConversation, MSGBOX_NPC
	end

Route126_EventScript_Sienna::
	msgbox Route126_Text_SiennaPostBattle, MSGBOX_NPC
	end

Route126_EventScript_Pablo::
	msgbox Route126_Text_PabloPostBattle, MSGBOX_NPC
	end

Route126_Text_LEONARDO_LocalConversation:
	.string "I was racing a FLOATZEL.\n"
	.string "Now I am enjoying a quiet swim.$"

Route126_Text_ISOBEL_LocalConversation:
	.string "My warm-up was a little long.\n"
	.string "I think I crossed a time zone.$"
