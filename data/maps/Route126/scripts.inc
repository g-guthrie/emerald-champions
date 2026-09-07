Route126_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route126_OnTransition
	.byte 0

Route126_OnTransition:
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	end

Route126_EventScript_Barry::
	trainerbattle_double TRAINER_BARRY, Route126_Text_BarryIntro, Route126_Text_BarryDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route126_Text_BarryPostBattle, MSGBOX_AUTOCLOSE
	end

Route126_EventScript_Dean::
	trainerbattle_double TRAINER_DEAN, Route126_Text_DeanIntro, Route126_Text_DeanDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route126_Text_DeanPostBattle, MSGBOX_AUTOCLOSE
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
	trainerbattle_double TRAINER_SIENNA, Route126_Text_SiennaIntro, Route126_Text_SiennaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route126_Text_SiennaPostBattle, MSGBOX_AUTOCLOSE
	end

Route126_EventScript_Pablo::
	trainerbattle_double TRAINER_PABLO_1, Route126_Text_PabloIntro, Route126_Text_PabloDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route126_EventScript_RegisterPablo
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route126_EventScript_RematchPablo
	msgbox Route126_Text_PabloPostBattle, MSGBOX_DEFAULT
	release
	end

Route126_EventScript_RegisterPablo::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route126_Text_PabloRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_PABLO_1
	release
	end

Route126_EventScript_RematchPablo::
	trainerbattle_rematch_double TRAINER_PABLO_1, Route126_Text_PabloRematchIntro, Route126_Text_PabloRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route126_Text_PabloPostRematch, MSGBOX_AUTOCLOSE
	end


Route126_Text_LEONARDO_LocalConversation:
	.string "I was racing a FLOATZEL.\n"
	.string "Now I am enjoying a quiet swim.$"

Route126_Text_ISOBEL_LocalConversation:
	.string "My warm-up was a little long.\n"
	.string "I think I crossed a time zone.$"
