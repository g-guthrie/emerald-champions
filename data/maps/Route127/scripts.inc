Route127_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route127_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route127_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route127_OnFrame
	.byte 0

Route127_OnTransition:
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	call_if_eq VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_HideMapNamePopup
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_127_NORTH, AbnormalWeather_StartKyogreWeather
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_127_SOUTH, AbnormalWeather_StartKyogreWeather
	end

Route127_OnLoad:
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_127_NORTH, AbnormalWeather_EventScript_PlaceTilesRoute127North
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_127_SOUTH, AbnormalWeather_EventScript_PlaceTilesRoute127South
	end

Route127_OnFrame:
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route127_EventScript_Camden::
	msgbox Route127_Text_CamdenPostBattle, MSGBOX_NPC
	end

Route127_EventScript_Donny::
	msgbox Route127_Text_DonnyPostBattle, MSGBOX_NPC
	end

Route127_EventScript_Jonah::
	msgbox Route127_Text_JONAH_LocalConversation, MSGBOX_NPC
	end

Route127_EventScript_Henry::
	msgbox Route127_Text_HenryPostBattle, MSGBOX_NPC
	end

Route127_EventScript_Roger::
	trainerbattle_double TRAINER_ROGER, Route127_Text_RogerIntro, Route127_Text_RogerDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route127_Text_RogerPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Aidan::
	msgbox Route127_Text_AIDAN_LocalConversation, MSGBOX_NPC
	end

Route127_EventScript_Athena::
	trainerbattle_double TRAINER_ATHENA, Route127_Text_AthenaIntro, Route127_Text_AthenaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route127_Text_AthenaPostBattle, MSGBOX_AUTOCLOSE
	end

Route127_EventScript_Koji::
	msgbox Route127_Text_KojiPostBattle, MSGBOX_NPC
	end

Route127_Text_JONAH_LocalConversation:
	.string "Fishing teaches patience.\n"
	.string "This spot has a full curriculum.$"

Route127_Text_AIDAN_LocalConversation:
	.string "Birds never need a boarding pass.\n"
	.string "I think they are showing off.$"
