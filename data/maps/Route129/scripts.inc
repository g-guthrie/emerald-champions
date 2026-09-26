Route129_MapScripts:: @ 81F7284
	map_script MAP_SCRIPT_ON_TRANSITION, Route129_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route129_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route129_OnFrame
	.byte 0

Route129_OnLoad: @ 81F7294
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_129_WEST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute129West
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_129_EAST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute129East
	end

Route129_OnTransition: @ 81F72AB
	compare VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	call_if_eq AbnormalWeather_EventScript_HideMapNamePopup
	compare VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_SKY_PILLAR_OPEN
	call_if_ge Route129_EventScript_CheckSetAbnormalWeather
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_129_WEST
	call_if_eq AbnormalWeather_StartKyogreWeather
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_129_EAST
	call_if_eq AbnormalWeather_StartKyogreWeather
	end

Route129_EventScript_CheckSetAbnormalWeather:: @ 81F72D8
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	return

Route129_OnFrame: @ 81F72E2
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route129_EventScript_Chase:: @ 81F72EC
	@ This retired encounter has no campaign battle.
	msgbox Route129_Text_ChasePostBattle, MSGBOX_AUTOCLOSE
	end
