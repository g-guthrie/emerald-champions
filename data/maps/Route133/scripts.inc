Route133_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route133_OnTransition
	.byte 0

Route133_OnTransition:
	goto_if_lt VAR_POKE_VIAL_MAX_CHARGES, 2, Route133_EventScript_HideVialNurse
	goto_if_ge VAR_POKE_VIAL_MAX_CHARGES, 3, Route133_EventScript_HideVialNurse
	clearflag FLAG_HIDE_ROUTE133_VIAL_NURSE
	end

Route133_EventScript_HideVialNurse::
	setflag FLAG_HIDE_ROUTE133_VIAL_NURSE
	end

Route133_EventScript_VialNurse::
	lock
	faceplayer
	msgbox Route133_Text_VialNurseHelp, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route133_EventScript_VialNurseDecline
	msgbox Route133_Text_VialNurseUpgrade, MSGBOX_DEFAULT
	setvar VAR_POKE_VIAL_MAX_CHARGES, 3
	setvar VAR_POKE_VIAL_CHARGES, 3
	setflag FLAG_HIDE_ROUTE133_VIAL_NURSE
	fadescreenswapbuffers FADE_TO_BLACK
	removeobject LOCALID_ROUTE133_VIAL_NURSE
	removeobject LOCALID_ROUTE133_VIAL_CHANSEY
	fadescreenswapbuffers FADE_FROM_BLACK
	release
	end

Route133_EventScript_VialNurseDecline::
	msgbox Route133_Text_VialNurseDecline, MSGBOX_DEFAULT
	release
	end

Route133_EventScript_VialChansey::
	lock
	faceplayer
	playmoncry SPECIES_CHANSEY, CRY_MODE_NORMAL
	msgbox Route133_Text_VialChansey, MSGBOX_DEFAULT
	release
	end

Route133_Text_VialNurseHelp:
	.string "We were swept onto this island by\n"
	.string "Route 133's current.\p"
	.string "May I borrow your help getting\n"
	.string "Chansey and me safely ashore?$"

Route133_Text_VialNurseUpgrade:
	.string "Thank you! Before we go, I tuned your\n"
	.string "Poké Vial one final time.\p"
	.string "Centers will now refill three charges.$"

Route133_Text_VialNurseDecline:
	.string "We'll keep waiting for a safe opening.$"

Route133_Text_VialChansey:
	.string "Chansey!$"

Route133_EventScript_Franklin::
	trainerbattle_double TRAINER_FRANKLIN, Route133_Text_FranklinIntro, Route133_Text_FranklinDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_FranklinPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Debra::
	trainerbattle_double TRAINER_DEBRA, Route133_Text_DebraIntro, Route133_Text_DebraDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_DebraPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Linda::
	trainerbattle_double TRAINER_LINDA, Route133_Text_LindaIntro, Route133_Text_LindaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_LindaPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Warren::
	trainerbattle_double TRAINER_WARREN, Route133_Text_WarrenIntro, Route133_Text_WarrenDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_WarrenPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Beck::
	trainerbattle_double TRAINER_BECK, Route133_Text_BeckIntro, Route133_Text_BeckDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_BeckPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Mollie::
	trainerbattle_double TRAINER_MOLLIE, Route133_Text_MollieIntro, Route133_Text_MollieDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_MolliePostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Conor::
	trainerbattle_double TRAINER_CONOR, Route133_Text_ConorIntro, Route133_Text_ConorDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_ConorPostBattle, MSGBOX_AUTOCLOSE
	end
