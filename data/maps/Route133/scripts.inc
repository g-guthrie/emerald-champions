.set LOCALID_NURSE, 8
.set LOCALID_CHANSEY, 9

Route133_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route133_OnTransition
	.byte 0

Route133_OnTransition:
	compare VAR_POKE_VIAL_MAX_CHARGES, 1
	goto_if_eq Route133_HideNurse
	compare VAR_POKE_VIAL_MAX_CHARGES, 2
	goto_if_eq Route133_ShowNurse
	end

Route133_HideNurse:
	setflag FLAG_LENT_NURSE_SURF
	end

Route133_ShowNurse:
	clearflag FLAG_LENT_NURSE_SURF
	end

Route133_EventScript_Franklin:: @ 81F75A3
	@ This retired encounter has no campaign battle.
	msgbox Route133_Text_FranklinPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Debra:: @ 81F75BA
	@ This retired encounter has no campaign battle.
	msgbox Route133_Text_DebraPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Linda:: @ 81F75D1
	trainerbattle_double TRAINER_LINDA, Route133_Text_LindaIntro, Route133_Text_LindaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_LindaPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Warren:: @ 81F75E8
	trainerbattle_double TRAINER_WARREN, Route133_Text_WarrenIntro, Route133_Text_WarrenDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_WarrenPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Beck:: @ 81F75FF
	@ This retired encounter has no campaign battle.
	msgbox Route133_Text_BeckPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Mollie:: @ 81F7616
	trainerbattle_double TRAINER_MOLLIE, Route133_Text_MollieIntro, Route133_Text_MollieDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_MolliePostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Conor:: @ 81F762D
	trainerbattle_double TRAINER_CONOR, Route133_Text_ConorIntro, Route133_Text_ConorDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route133_Text_ConorPostBattle, MSGBOX_AUTOCLOSE
	end

Route133_EventScript_Nurse::
	lockall
	faceplayer
	playse SE_PIN
	compare VAR_TEMP_1, 1
	goto_if_eq Route133_EventScript_NurseShortMsg
	applymovement LOCALID_NURSE, Common_Movement_ExclamationMark
	waitmovement 0
	delay 40
	msgbox Route133_Text_NurseHello, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route133_EventScript_NurseRefuse
Route133_EventScript_NurseTeachAudino::
	msgbox Route133_Text_NurseTeachAudino, MSGBOX_DEFAULT
	playse SE_PC_LOGIN
	waitse
	playfanfare MUS_LEVEL_UP
	msgbox Route133_Text_NurseDoneHaveVialUpgrade, MSGBOX_DEFAULT
	setvar VAR_POKE_VIAL_MAX_CHARGES, 3
	setvar VAR_POKE_VIAL_CHARGES, 3
	fadescreen FADE_TO_BLACK
	waitse
	playmoncry SPECIES_CHANSEY, 0
	delay 40
	waitmoncry
	removeobject LOCALID_NURSE
	removeobject LOCALID_CHANSEY
	closemessage
	fadescreen FADE_FROM_BLACK
	setflag FLAG_LENT_NURSE_SURF
	releaseall
	end

Route133_EventScript_NurseShortMsg::
	msgbox Route133_Text_NurseSoYoullHelp, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route133_EventScript_NurseRefuse
	goto Route133_EventScript_NurseTeachAudino

Route133_EventScript_NurseRefuse::
	setvar VAR_TEMP_1, 1
	msgbox Route133_Text_NurseReally, MSGBOX_AUTOCLOSE
	releaseall
	end

Route133_EventScript_Blob::
	lockall
	faceplayer
	applymovement LOCALID_CHANSEY, Route133_Movement_ChanseyJump
	waitmovement 0
	message Route133_Text_Chansey
	waitse
	playmoncry SPECIES_CHANSEY, 0
	delay 40
	waitmoncry
	closemessage
	releaseall
	end

Route133_Movement_ChanseyJump:
	jump_in_place_left
	delay_4
	jump_in_place_left
	step_end

Route133_Text_Chansey:
	.string "Siii!$"

Route133_Text_NurseHello:
	.string "Oh, hey, {PLAYER}! Can you help\n"
	.string "us?\p"
	.string "My Audino is frightened of the\n"
	.string "strong currents. Could your team\l"
	.string "show it a safe way across?$"

Route133_Text_NurseReally:
	.string "Really? Please, I'm desperate here!$"

Route133_Text_NurseSoYoullHelp:
	.string "Will your team help my Audino\n"
	.string "across the currents?$"

Route133_Text_NurseTeachAudino:
	.string "Thank you! Look, Audino. Follow\n"
	.string "their lead and take your time.$"

Route133_Text_NurseDoneHaveVialUpgrade:
	.string "And… done!\p"
	.string "Thanks again, {PLAYER}. As a reward,\n"
	.string "I'll make sure you get an extra dose\l"
	.string "whenever you refill your Poké Vial\l"
	.string "from now on.\p"
	.string "See you around!$"
