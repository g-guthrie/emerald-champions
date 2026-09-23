Route124_MapScripts:: @ 81F656C
	map_script MAP_SCRIPT_ON_TRANSITION, Route124_OnTransition
	.byte 0

Route124_OnTransition: @ 81F6572
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	end

Route124_EventScript_HuntersHouseSign:: @ 81F657C
	msgbox Route124_Text_HuntersHouse, MSGBOX_SIGN
	end

Route124_EventScript_Spencer:: @ 81F6585
	@ This retired encounter has no campaign battle.
	msgbox Route124_Text_SpencerPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Roland:: @ 81F659C
	@ This retired encounter has no campaign battle.
	msgbox Route124_Text_RolandPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Chad:: @ 81F662C
	@ This retired encounter has no campaign battle.
	msgbox Route124_Text_ChadPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Lila:: @ 81F6643
	trainerbattle_double TRAINER_LILA_AND_ROY_1, Route124_Text_LilaIntro, Route124_Text_LilaDefeat, Route124_Text_LilaNotEnoughMons
	msgbox Route124_Text_LilaPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Roy:: @ 81F66A7
	trainerbattle_double TRAINER_LILA_AND_ROY_1, Route124_Text_RoyIntro, Route124_Text_RoyDefeat, Route124_Text_RoyNotEnoughMons
	msgbox Route124_Text_RoyPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_Text_HuntersHouse: @ 81F6739
	.string "Hunter's House$"
