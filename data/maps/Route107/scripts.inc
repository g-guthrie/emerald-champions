Route107_MapScripts:: @ 81EE594
	.byte 0

Route107_EventScript_Darrin:: @ 81EE595
	trainerbattle_double TRAINER_DARRIN, Route107_Text_DarrinIntro, Route107_Text_DarrinDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route107_Text_DarrinPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Beth:: @ 81EE625
	@ This retired encounter has no campaign battle.
	msgbox Route107_Text_BethPostBattle, MSGBOX_AUTOCLOSE
	end
