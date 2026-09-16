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
	msgbox Route125_Text_StanPostBattle, MSGBOX_NPC
	end

Route125_EventScript_Tanya::
	msgbox Route125_Text_TanyaPostBattle, MSGBOX_NPC
	end

Route125_EventScript_Sharon::
	msgbox Route125_Text_SHARON_LocalConversation, MSGBOX_NPC
	end

Route125_EventScript_Ernest::
	msgbox Route125_Text_ErnestPostBattle, MSGBOX_NPC
	end

Route125_EventScript_Kim::
	msgbox Route125_Text_KimPostBattle, MSGBOX_NPC
	end

Route125_EventScript_Iris::
	msgbox Route125_Text_IrisPostBattle, MSGBOX_NPC
	end

Route125_EventScript_Presley::
	msgbox Route125_Text_PresleyPostBattle, MSGBOX_NPC
	end

Route125_EventScript_Auron::
	trainerbattle_double TRAINER_AURON, Route125_Text_AuronIntro, Route125_Text_AuronDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route125_Text_AuronPostBattle, MSGBOX_AUTOCLOSE
	end


Route125_Text_NOLEN_LocalConversation:
	.string "SHOAL CAVE is just off this water.\p"
	.string "Ask the SHOALMASTER inside to turn\n"
	.string "the tide. He does it for anyone.$"

Route125_Text_SHARON_LocalConversation:
	.string "SLOWBRO never seems rushed.\n"
	.string "I am trying to learn from that.$"
