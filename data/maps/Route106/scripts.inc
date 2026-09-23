Route106_MapScripts:: @ 81EE489
	.byte 0

Route106_EventScript_TrainerTipsSign:: @ 81EE48A
	msgbox Route106_Text_TrainerTips, MSGBOX_SIGN
	end

Route106_EventScript_Douglas:: @ 81EE493
	@ This retired encounter has no campaign battle.
	msgbox Route106_Text_DouglasPostBattle, MSGBOX_AUTOCLOSE
	end

Route106_EventScript_Elliot:: @ 81EE4C1
	trainerbattle_double TRAINER_ELLIOT_1, Route106_Text_ElliotIntro, Route106_Text_ElliotDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route106_Text_ElliotPostBattle, MSGBOX_AUTOCLOSE
	end

Route106_EventScript_Ned:: @ 81EE523
	trainerbattle_double TRAINER_NED, Route106_Text_NedIntro, Route106_Text_NedDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route106_Text_NedPostBattle, MSGBOX_AUTOCLOSE
	end

Route106_Text_TrainerTips: @ 81EE53A
	.string "Trainer Tips\p"
	.string "Advice on catching Pokémon with a Rod:\n"
	.string "Press the A Button if you get a bite.$"
