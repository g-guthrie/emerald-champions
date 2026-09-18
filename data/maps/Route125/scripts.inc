Route125_MapScripts:: @ 81F6748
	map_script MAP_SCRIPT_ON_TRANSITION, Route125_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route125_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route125_OnFrame
	.byte 0

Route125_OnTransition: @ 81F6758
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	compare VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	call_if_eq AbnormalWeather_EventScript_HideMapNamePopup
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_WEST
	call_if_eq AbnormalWeather_StartKyogreWeather
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_EAST
	call_if_eq AbnormalWeather_StartKyogreWeather
	end

Route125_OnLoad: @ 81F6783
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_WEST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute125West
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_125_EAST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute125East
	end

Route125_OnFrame: @ 81F679A
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route125_EventScript_Nolen:: @ 81F67A4
	@ This retired encounter has no campaign battle.
	msgbox Route125_Text_NolenPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Stan:: @ 81F67BB
	@ This retired encounter has no campaign battle.
	msgbox Route125_Text_StanPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Tanya:: @ 81F67D2
	@ This retired encounter has no campaign battle.
	msgbox Route125_Text_TanyaPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Sharon:: @ 81F67E9
	@ This retired encounter has no campaign battle.
	msgbox Route125_Text_SharonPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Ernest:: @ 81F6800
	@ This retired encounter has no campaign battle.
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route125_EventScript_RematchErnest
	msgbox Route125_Text_ErnestPostBattle, MSGBOX_DEFAULT
	release
	end

Route125_EventScript_RegisterErnest:: @ 81F682C
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route125_Text_ErnestRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_ERNEST_1
	release
	end

Route125_EventScript_RematchErnest:: @ 81F684B
	@ This retired encounter has no campaign battle.
	msgbox Route125_Text_ErnestRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Kim:: @ 81F6862
	@ This retired encounter has no campaign battle.
	msgbox Route125_Text_KimPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Iris:: @ 81F687D
	@ This retired encounter has no campaign battle.
	msgbox Route125_Text_IrisPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Presley:: @ 81F6898
	@ This retired encounter has no campaign battle.
	msgbox Route125_Text_PresleyPostBattle, MSGBOX_AUTOCLOSE
	end

Route125_EventScript_Auron:: @ 81F68AF
	trainerbattle_double TRAINER_AURON, Route125_Text_AuronIntro, Route125_Text_AuronDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route125_Text_AuronPostBattle, MSGBOX_AUTOCLOSE
	end

