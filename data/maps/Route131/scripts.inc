Route131_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route131_OnTransition
	.byte 0

Route131_OnTransition:
	call_if_ge VAR_SOOTOPOLIS_CITY_STATE, 4, Route131_EventScript_CheckSetAbnormalWeather
	call Route131_EventScript_SetLayout
	end

Route131_EventScript_SetLayout::
	setmaplayoutindex LAYOUT_ROUTE131_SKY_PILLAR
	return

Route131_EventScript_CheckSetAbnormalWeather::
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	return

Route131_EventScript_Richard::
	msgbox Route131_Text_RichardPostBattle, MSGBOX_NPC
	end

Route131_EventScript_Herman::
	trainerbattle_double TRAINER_HERMAN, Route131_Text_HermanIntro, Route131_Text_HermanDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route131_Text_HermanPostBattle, MSGBOX_AUTOCLOSE
	end

Route131_EventScript_Susie::
	trainerbattle_double TRAINER_SUSIE, Route131_Text_SusieIntro, Route131_Text_SusieDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route131_Text_SusiePostBattle, MSGBOX_AUTOCLOSE
	end

Route131_EventScript_Kara::
	trainerbattle_double TRAINER_KARA, Route131_Text_KaraIntro, Route131_Text_KaraDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route131_Text_KaraPostBattle, MSGBOX_AUTOCLOSE
	end

Route131_EventScript_Reli::
	msgbox Route131_Text_ReliPostBattle, MSGBOX_NPC
	end

Route131_EventScript_Ian::
	msgbox Route131_Text_IanPostBattle, MSGBOX_NPC
	end

Route131_EventScript_Talia::
	msgbox Route131_Text_TaliaPostBattle, MSGBOX_NPC
	end

Route131_EventScript_Kevin::
	msgbox Route131_Text_KevinPostBattle, MSGBOX_NPC
	end

