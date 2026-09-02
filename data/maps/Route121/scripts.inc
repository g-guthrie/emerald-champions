Route121_MapScripts::
	.byte 0

Route121_EventScript_Nurse::
	lock
	faceplayer
	msgbox Route121_Text_NurseOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route121_EventScript_NurseDeclined
	closemessage
	call Common_EventScript_OutOfCenterPartyHeal
	msgbox Route121_Text_NurseHealed, MSGBOX_DEFAULT
	release
	end

Route121_EventScript_NurseDeclined::
	msgbox Route121_Text_NurseDeclined, MSGBOX_DEFAULT
	release
	end

Route121_EventScript_Skitty::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_SKITTY, CRY_MODE_NORMAL
	msgbox Route121_Text_RestoredSkitty, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Route121_Text_NurseOffer:
	.string "MT. PYRE is a long climb. Shall I heal\n"
	.string "your party before you continue?$"

Route121_Text_NurseHealed:
	.string "All better. Keep your partners close\n"
	.string "on the mountain.$"

Route121_Text_NurseDeclined:
	.string "All right. SKITTY and I will be here\n"
	.string "if you need us.$"

Route121_Text_RestoredSkitty:
	.string "SKITTY: Meenya?$"

Route121_EventScript_Woman::
	msgbox Route121_Text_AheadLoomsMtPyre, MSGBOX_NPC
	end

Route121_EventScript_MtPyrePierSign::
	msgbox Route121_Text_MtPyrePierSign, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route121_EventScript_SafariZoneSign::
	msgbox Route121_Text_SafariZoneSign, MSGBOX_SIGN
	end

Route121_EventScript_AquaGruntsMoveOut::
	lockall
	playbgm MUS_ENCOUNTER_AQUA, FALSE
	applymovement LOCALID_ROUTE121_GRUNT_2, Common_Movement_WalkInPlaceRight
	waitmovement 0
	msgbox Route121_Text_OkayMoveOutToMtPyre, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE121_GRUNT_1, Route121_Movement_Grunt1Exit
	applymovement LOCALID_ROUTE121_GRUNT_2, Route121_Movement_Grunt2Exit
	applymovement LOCALID_ROUTE121_GRUNT_3, Route121_Movement_Grunt3Exit
	waitmovement 0
	fadedefaultbgm
	removeobject LOCALID_ROUTE121_GRUNT_1
	removeobject LOCALID_ROUTE121_GRUNT_2
	removeobject LOCALID_ROUTE121_GRUNT_3
	setvar VAR_ROUTE121_STATE, 1
	releaseall
	end

Route121_Movement_Grunt1Exit:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route121_Movement_Grunt2Exit:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route121_Movement_Grunt3Exit:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

Route121_EventScript_Vanessa::
	trainerbattle_double TRAINER_VANESSA, Route121_Text_VanessaIntro, Route121_Text_VanessaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route121_Text_VanessaPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Walter::
	trainerbattle_double TRAINER_WALTER_1, Route121_Text_WalterIntro, Route121_Text_WalterDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route121_EventScript_RegisterWalter
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route121_EventScript_RematchWalter
	msgbox Route121_Text_WalterPostBattle, MSGBOX_DEFAULT
	release
	end

Route121_EventScript_RegisterWalter::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route121_Text_WalterRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_WALTER_1
	release
	end

Route121_EventScript_RematchWalter::
	trainerbattle_rematch_double TRAINER_WALTER_1, Route121_Text_WalterRematchIntro, Route121_Text_WalterRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route121_Text_WalterPostRematch, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Tammy::
	trainerbattle_single TRAINER_TAMMY, Route121_Text_TammyIntro, Route121_Text_TammyDefeat
	msgbox Route121_Text_TammyPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Kate::
	trainerbattle_double TRAINER_KATE_AND_JOY, Route121_Text_KateIntro, Route121_Text_KateDefeat, Route121_Text_KateNotEnoughMons
	msgbox Route121_Text_KatePostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Joy::
	trainerbattle_double TRAINER_KATE_AND_JOY, Route121_Text_JoyIntro, Route121_Text_JoyDefeat, Route121_Text_JoyNotEnoughMons
	msgbox Route121_Text_JoyPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Jessica::
	trainerbattle_double TRAINER_JESSICA_1, Route121_Text_JessicaIntro, Route121_Text_JessicaDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route121_EventScript_RegisterJessica
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route121_EventScript_RematchJessica
	msgbox Route121_Text_JessicaPostBattle, MSGBOX_DEFAULT
	release
	end

Route121_EventScript_RegisterJessica::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route121_Text_JessicaRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_JESSICA_1
	release
	end

Route121_EventScript_RematchJessica::
	trainerbattle_rematch_double TRAINER_JESSICA_1, Route121_Text_JessicaRematchIntro, Route121_Text_JessicaRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route121_Text_JessicaPostRematch, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Cale::
	trainerbattle_double TRAINER_CALE, Route121_Text_CaleIntro, Route121_Text_CaleDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route121_Text_CalePostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Myles::
	trainerbattle_double TRAINER_MYLES, Route121_Text_MylesIntro, Route121_Text_MylesDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route121_Text_MylesPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Pat::
	trainerbattle_double TRAINER_PAT, Route121_Text_PatIntro, Route121_Text_PatDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route121_Text_PatPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Marcel::
	trainerbattle_double TRAINER_MARCEL, Route121_Text_MarcelIntro, Route121_Text_MarcelDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route121_Text_MarcelPostBattle, MSGBOX_AUTOCLOSE
	end

Route121_EventScript_Cristin::
	trainerbattle_double TRAINER_CRISTIN_1, Route121_Text_CristinIntro, Route121_Text_CristinDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route121_EventScript_RegisterCristin
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route121_EventScript_RematchCristin
	msgbox Route121_Text_CristinPostBattle, MSGBOX_DEFAULT
	release
	end

Route121_EventScript_RegisterCristin::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route121_Text_CristinRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_CRISTIN_1
	release
	end

Route121_EventScript_RematchCristin::
	trainerbattle_rematch_double TRAINER_CRISTIN_1, Route121_Text_CristinRematchIntro, Route121_Text_CristinRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route121_Text_CristinPostRematch, MSGBOX_AUTOCLOSE
	end

Route121_Text_OkayMoveOutToMtPyre:
	.string "Okay!\n"
	.string "We're to move out to MT. PYRE!$"

Route121_Text_AheadLoomsMtPyre:
	.string "Ahead looms MT. PYRE…\p"
	.string "It is a natural monument to the spirits \n"
	.string "of departed POKéMON…$"

Route121_Text_MtPyrePierSign:
	.string "MT. PYRE PIER\p"
	.string "…The sign is old and worn out.\n"
	.string "The words are barely legible…$"

Route121_Text_SafariZoneSign:
	.string "“Filled with rare POKéMON!”\n"
	.string "SAFARI ZONE$"
