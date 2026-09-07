Route125_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route125_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route125_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route125_OnFrame
	.byte 0

Route125_OnTransition:
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	call_if_eq VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_HideMapNamePopup
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_WEST, AbnormalWeather_StartKyogreWeather
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_EAST, AbnormalWeather_StartKyogreWeather
	end

Route125_OnLoad:
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_WEST, AbnormalWeather_EventScript_PlaceTilesRoute125West
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_EAST, AbnormalWeather_EventScript_PlaceTilesRoute125East
	end

Route125_OnFrame:
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route125_EventScript_Nolen::
	msgbox Route125_Text_NOLEN_LocalConversation, MSGBOX_NPC
	end

Route125_EventScript_Stan::
	trainerbattle_double TRAINER_STAN, Route125_Text_StanIntro, Route125_Text_StanDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route125_Text_StanPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Tanya::
	trainerbattle_double TRAINER_TANYA, Route125_Text_TanyaIntro, Route125_Text_TanyaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route125_Text_TanyaPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Sharon::
	msgbox Route125_Text_SHARON_LocalConversation, MSGBOX_NPC
	end

Route125_EventScript_Ernest::
	trainerbattle_double TRAINER_ERNEST_1, Route125_Text_ErnestIntro, Route125_Text_ErnestDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route125_EventScript_RegisterErnest
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route125_EventScript_RematchErnest
	msgbox Route125_Text_ErnestPostBattle, MSGBOX_DEFAULT
	release
	end

Route125_EventScript_RegisterErnest::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route125_Text_ErnestRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_ERNEST_1
	release
	end

Route125_EventScript_RematchErnest::
	trainerbattle_rematch_double TRAINER_ERNEST_1, Route125_Text_ErnestRematchIntro, Route125_Text_ErnestRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route125_Text_ErnestRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Kim::
	trainerbattle_double TRAINER_KIM_AND_IRIS, Route125_Text_KimIntro, Route125_Text_KimDefeat, Route125_Text_KimNotEnoughMons
	msgbox Route125_Text_KimPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Iris::
	trainerbattle_double TRAINER_KIM_AND_IRIS, Route125_Text_IrisIntro, Route125_Text_IrisDefeat, Route125_Text_IrisNotEnoughMons
	msgbox Route125_Text_IrisPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Presley::
	trainerbattle_double TRAINER_PRESLEY, Route125_Text_PresleyIntro, Route125_Text_PresleyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route125_Text_PresleyPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Auron::
	trainerbattle_double TRAINER_AURON, Route125_Text_AuronIntro, Route125_Text_AuronDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route125_Text_AuronPostBattle, MSGBOX_AUTOCLOSE
	end


Route125_Text_NOLEN_LocalConversation:
	.string "I know a shortcut to the shore.\n"
	.string "First, decide which shore.$"

Route125_Text_SHARON_LocalConversation:
	.string "SLOWBRO never seems rushed.\n"
	.string "I am trying to learn from that.$"
