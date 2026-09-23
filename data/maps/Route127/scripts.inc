Route127_MapScripts:: @ 81F69D9
	map_script MAP_SCRIPT_ON_TRANSITION, Route127_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route127_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route127_OnFrame
	.byte 0

Route127_OnTransition: @ 81F69E9
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	compare VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	call_if_eq AbnormalWeather_EventScript_HideMapNamePopup
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_127_NORTH
	call_if_eq AbnormalWeather_StartKyogreWeather
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_127_SOUTH
	call_if_eq AbnormalWeather_StartKyogreWeather
	end

Route127_OnLoad: @ 81F6A14
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_127_NORTH
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute127North
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_127_SOUTH
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute127South
	end

Route127_OnFrame: @ 81F6A2B
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route127_EventScript_Camden:: @ 81F6A35
	@ This retired encounter has no campaign battle.
	msgbox Route127_Text_CamdenPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Jonah:: @ 81F6A63
	@ This retired encounter has no campaign battle.
	msgbox Route127_Text_JonahPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Roger:: @ 81F6A91
	trainerbattle_double TRAINER_ROGER, Route127_Text_RogerIntro, Route127_Text_RogerDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route127_Text_RogerPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Aidan:: @ 81F6AA8
	@ This retired encounter has no campaign battle.
	msgbox Route127_Text_AidanPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Athena:: @ 81F6ABF
	trainerbattle_double TRAINER_ATHENA, Route127_Text_AthenaIntro, Route127_Text_AthenaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route127_Text_AthenaPostBattle, MSGBOX_AUTOCLOSE
	end
