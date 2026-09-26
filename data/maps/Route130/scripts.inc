Route130_MapScripts:: @ 81F735F
	map_script MAP_SCRIPT_ON_TRANSITION, Route130_OnTransition
	.byte 0

Route130_OnTransition: @ 81F7365
	compare VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_SKY_PILLAR_OPEN
	call_if_ge Route130_EventScript_CheckSetAbnormalWeather
	specialvar VAR_RESULT, IsMirageIslandPresent
	compare VAR_RESULT, TRUE
	goto_if_eq Route130_EventScript_SetMirageIslandLayout
	setflag FLAG_TEMP_11
	setflag FLAG_TEMP_12
	setflag FLAG_TEMP_13
	setflag FLAG_TEMP_14
	setflag FLAG_TEMP_15
	setflag FLAG_TEMP_16
	setflag FLAG_TEMP_17
	setflag FLAG_TEMP_18
	setflag FLAG_TEMP_19
	setflag FLAG_TEMP_1A
	setflag FLAG_TEMP_1B
	setflag FLAG_TEMP_1C
	setflag FLAG_TEMP_1D
	setflag FLAG_TEMP_1E
	setflag FLAG_TEMP_1F
	setmaplayoutindex LAYOUT_ROUTE130
	end

Route130_EventScript_SetMirageIslandLayout:: @ 81F73B1
	setmaplayoutindex LAYOUT_ROUTE130_MIRAGE_ISLAND
	end

Route130_EventScript_CheckSetAbnormalWeather:: @ 81F73B5
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	return

Route130_EventScript_Santiago:: @ 81F73ED
	trainerbattle_double TRAINER_SANTIAGO, Route130_Text_SantiagoIntro, Route130_Text_SantiagoDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route130_Text_SantiagoPostBattle, MSGBOX_AUTOCLOSE
	end

