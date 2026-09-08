Route124_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route124_OnTransition
	.byte 0

Route124_OnTransition:
	call_if_set FLAG_SYS_WEATHER_CTRL, Common_EventScript_SetAbnormalWeather
	end

Route124_EventScript_HuntersHouseSign::
	msgbox Route124_Text_HuntersHouse, MSGBOX_SIGN
	end

Route124_EventScript_Spencer::
	msgbox Route124_Text_SPENCER_LocalConversation, MSGBOX_NPC
	end

Route124_EventScript_Roland::
	msgbox Route124_Text_ROLAND_LocalConversation, MSGBOX_NPC
	end

Route124_EventScript_Jenny::
	trainerbattle_double TRAINER_JENNY_1, Route124_Text_JennyIntro, Route124_Text_JennyDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route124_EventScript_RegisterJenny
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route124_EventScript_RematchJenny
	msgbox Route124_Text_JennyPostBattle, MSGBOX_DEFAULT
	release
	end

Route124_EventScript_RegisterJenny::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route124_Text_JennyRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_JENNY_1
	release
	end

Route124_EventScript_RematchJenny::
	trainerbattle_rematch_double TRAINER_JENNY_1, Route124_Text_JennyRematchIntro, Route124_Text_JennyRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route124_Text_JennyPostRematch, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Grace::
	trainerbattle_double TRAINER_GRACE, Route124_Text_GraceIntro, Route124_Text_GraceDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route124_Text_GracePostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Chad::
	msgbox Route124_Text_CHAD_LocalConversation, MSGBOX_NPC
	end

Route124_EventScript_Lila::
	trainerbattle_double TRAINER_LILA_AND_ROY_1, Route124_Text_LilaIntro, Route124_Text_LilaDefeat, Route124_Text_LilaNotEnoughMons, Route124_EventScript_RegisterLila
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route124_EventScript_RematchLila
	msgbox Route124_Text_LilaPostBattle, MSGBOX_DEFAULT
	release
	end

Route124_EventScript_RegisterLila::
	msgbox Route124_Text_LilaRoyRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_LILA_AND_ROY_1
	release
	end

Route124_EventScript_RematchLila::
	trainerbattle_rematch_double TRAINER_LILA_AND_ROY_1, Route124_Text_LilaRematchIntro, Route124_Text_LilaRematchDefeat, Route124_Text_LilaRematchNotEnoughMons
	msgbox Route124_Text_LilaPostRematch, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Roy::
	trainerbattle_double TRAINER_LILA_AND_ROY_1, Route124_Text_RoyIntro, Route124_Text_RoyDefeat, Route124_Text_RoyNotEnoughMons, Route124_EventScript_RegisterRoy
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route124_EventScript_RematchRoy
	msgbox Route124_Text_RoyPostBattle, MSGBOX_DEFAULT
	release
	end

Route124_EventScript_RegisterRoy::
	msgbox Route124_Text_LilaRoyRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_LILA_AND_ROY_1
	release
	end

Route124_EventScript_RematchRoy::
	trainerbattle_rematch_double TRAINER_LILA_AND_ROY_1, Route124_Text_RoyRematchIntro, Route124_Text_RoyRematchDefeat, Route124_Text_RoyRematchNotEnoughMons
	msgbox Route124_Text_RoyPostRematch, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Declan::
	trainerbattle_double TRAINER_DECLAN, Route124_Text_DeclanIntro, Route124_Text_DeclanDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route124_Text_DeclanPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_EventScript_Isabella::
	trainerbattle_double TRAINER_ISABELLA, Route124_Text_IsabellaIntro, Route124_Text_IsabellaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route124_Text_IsabellaPostBattle, MSGBOX_AUTOCLOSE
	end

Route124_Text_HuntersHouse:
	.string "HUNTER'S HOUSE$"


Route124_Text_SPENCER_LocalConversation:
	.string "I keep a map in my swim trunks.\n"
	.string "It is mostly ocean now.$"

Route124_Text_ROLAND_LocalConversation:
	.string "CLAWITZER is a powerful swimmer.\n"
	.string "The brakes could use some work.$"

Route124_Text_CHAD_LocalConversation:
	.string "STOUTLAND found my lost goggles.\n"
	.string "I was wearing them. Good dog.$"
