Route105_MapScripts:: @ 81EE1DB
	map_script MAP_SCRIPT_ON_LOAD, Route105_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, Route105_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route105_OnFrame
	.byte 0

Route105_OnLoad: @ 81EE1EB
	call_if_unset FLAG_REGI_DOORS_OPENED, Route105_CloseRegiEntrance
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_105_NORTH
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute105North
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_105_SOUTH
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute105South
	end

Route105_CloseRegiEntrance:: @ 81EE20B
	setmetatile 9, 19, METATILE_General_RockWall_RockBase, 1
	setmetatile 9, 20, METATILE_General_RockWall_SandBase, 1
	return

Route105_OnTransition: @ 81EE21E
	compare VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	call_if_eq AbnormalWeather_EventScript_HideMapNamePopup
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_105_NORTH
	call_if_eq AbnormalWeather_StartKyogreWeather
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_105_SOUTH
	call_if_eq AbnormalWeather_StartKyogreWeather
	end

Route105_OnFrame: @ 81EE240
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route105_EventScript_Foster:: @ 81EE24A
	trainerbattle_double TRAINER_FOSTER, Route105_Text_FosterIntro, Route105_Text_FosterDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route105_Text_FosterPostBattle, MSGBOX_AUTOCLOSE
	end

Route105_EventScript_Luis:: @ 81EE261
	trainerbattle_double TRAINER_LUIS, Route105_Text_LuisIntro, Route105_Text_LuisDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route105_Text_LuisPostBattle, MSGBOX_AUTOCLOSE
	end

Route105_EventScript_Dominik:: @ 81EE278
	@ This retired encounter has no campaign battle.
	msgbox Route105_Text_DominikPostBattle, MSGBOX_AUTOCLOSE
	end

Route105_EventScript_Josue:: @ 81EE2BD
	trainerbattle_double TRAINER_JOSUE, Route105_Text_JosueIntro, Route105_Text_JosueDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route105_Text_JosuePostBattle, MSGBOX_AUTOCLOSE
	end

Route104_Text_DadPokenavCall: @ 81EE336
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "Dad: Oh, {PLAYER}?\p"
	.string "… … … … … …\n"
	.string "Where are you now?\l"
	.string "It sounds windy wherever you are.\p"
	.string "I just heard from Devon's Mr. Stone\n"
	.string "about your PokéNav, so I decided\l"
	.string "to give you a call.\p"
	.string "It sounds like you're doing fine,\n"
	.string "so that's fine with me.\p"
	.string "You take care now.\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"
