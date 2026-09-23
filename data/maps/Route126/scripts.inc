Route126_MapScripts:: @ 81F68C6
	map_script MAP_SCRIPT_ON_TRANSITION, Route126_OnTransition
	.byte 0

Route126_OnTransition: @ 81F68CC
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	end

Route126_EventScript_Nikki:: @ 81F6904
	trainerbattle_double TRAINER_NIKKI, Route126_Text_NikkiIntro, Route126_Text_NikkiDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route126_Text_NikkiPostBattle, MSGBOX_AUTOCLOSE
	end

Route126_EventScript_Brenda:: @ 81F691B
	trainerbattle_double TRAINER_BRENDA, Route126_Text_BrendaIntro, Route126_Text_BrendaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route126_Text_BrendaPostBattle, MSGBOX_AUTOCLOSE
	end

Route126_EventScript_Leonardo:: @ 81F6932
	@ This retired encounter has no campaign battle.
	msgbox Route126_Text_LeonardoPostBattle, MSGBOX_AUTOCLOSE
	end

Route126_EventScript_Isobel:: @ 81F6949
	@ This retired encounter has no campaign battle.
	msgbox Route126_Text_IsobelPostBattle, MSGBOX_AUTOCLOSE
	end
