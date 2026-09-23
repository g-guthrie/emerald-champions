Route131_MapScripts:: @ 81F7404
	map_script MAP_SCRIPT_ON_TRANSITION, Route131_OnTransition
	.byte 0

Route131_OnTransition: @ 81F740A
	compare VAR_SOOTOPOLIS_CITY_STATE, 4
	call_if_ge Route131_EventScript_CheckSetAbnormalWeather
	call Route131_EventScript_SetLayout
	end

Route131_EventScript_SetLayout:: @ 81F741B
	setmaplayoutindex LAYOUT_ROUTE131_SKY_PILLAR
	return

Route131_EventScript_CheckSetAbnormalWeather:: @ 81F741F
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	return

Route131_EventScript_Herman:: @ 81F7440
	trainerbattle_double TRAINER_HERMAN, Route131_Text_HermanIntro, Route131_Text_HermanDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route131_Text_HermanPostBattle, MSGBOX_AUTOCLOSE
	end

Route131_EventScript_Susie:: @ 81F7457
	trainerbattle_double TRAINER_SUSIE, Route131_Text_SusieIntro, Route131_Text_SusieDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route131_Text_SusiePostBattle, MSGBOX_AUTOCLOSE
	end

Route131_EventScript_Kara:: @ 81F746E
	trainerbattle_double TRAINER_KARA, Route131_Text_KaraIntro, Route131_Text_KaraDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route131_Text_KaraPostBattle, MSGBOX_AUTOCLOSE
	end
