Route106_MapScripts::
	.byte 0

Route106_EventScript_TrainerTipsSign::
	msgbox Route106_Text_TrainerTips, MSGBOX_SIGN
	end

Route106_EventScript_Douglas::
	msgbox Route106_Text_DOUGLAS_LocalConversation, MSGBOX_NPC
	end

Route106_EventScript_Kyla::
	msgbox Route106_Text_KylaPostBattle, MSGBOX_NPC
	end

Route106_EventScript_Elliot::
	trainerbattle_double TRAINER_ELLIOT_1, Route106_Text_ElliotIntro, Route106_Text_ElliotDefeated, EmeraldChampions_Text_NeedTwoPokemon, Route106_EventScript_ElliotRegisterMatchCallAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route106_EventScript_ElliotRematch
	msgbox Route106_Text_ElliotPostBattle, MSGBOX_DEFAULT
	release
	end

Route106_EventScript_ElliotRegisterMatchCallAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route106_Text_ElliotRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_ELLIOT_1
	release
	end

Route106_EventScript_ElliotRematch::
	trainerbattle_rematch_double TRAINER_ELLIOT_1, Route106_Text_ElliotRematchIntro, Route106_Text_ElliotRematchDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route106_Text_ElliotRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route106_EventScript_Ned::
	trainerbattle_double TRAINER_NED, Route106_Text_NedIntro, Route106_Text_NedDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route106_Text_NedPostBattle, MSGBOX_AUTOCLOSE
	end

Route106_Text_TrainerTips:
	.string "TRAINER TIPS\p"
	.string "Advice on catching POKéMON with a ROD:\n"
	.string "Press the A Button if you get a bite.$"


Route106_Text_DOUGLAS_LocalConversation:
	.string "A LAPRAS ferry sounds relaxing.\n"
	.string "I should have booked one.$"
