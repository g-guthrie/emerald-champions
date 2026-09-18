Route107_MapScripts:: @ 81EE594
	.byte 0

Route107_EventScript_Darrin:: @ 81EE595
	trainerbattle_double TRAINER_DARRIN, Route107_Text_DarrinIntro, Route107_Text_DarrinDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route107_Text_DarrinPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Tony:: @ 81EE5AC
	@ This retired encounter has no campaign battle.
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route107_EventScript_TonyRematch
	msgbox Route107_Text_TonyPostBattle, MSGBOX_DEFAULT
	release
	end

Route107_EventScript_TonyRegisterMatchCallAfterBattle:: @ 81EE5D8
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route107_Text_TonyRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_TONY_1
	release
	end

Route107_EventScript_TonyRematch:: @ 81EE5F7
	@ This retired encounter has no campaign battle.
	msgbox Route107_Text_TonyRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Denise:: @ 81EE60E
	@ This retired encounter has no campaign battle.
	msgbox Route107_Text_DenisePostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Beth:: @ 81EE625
	@ This retired encounter has no campaign battle.
	msgbox Route107_Text_BethPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Lisa:: @ 81EE63C
	@ This retired encounter has no campaign battle.
	msgbox Route107_Text_LisaPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Ray:: @ 81EE657
	@ This retired encounter has no campaign battle.
	msgbox Route107_Text_RayPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Camron:: @ 81EE672
	@ This retired encounter has no campaign battle.
	msgbox Route107_Text_CamronPostBattle, MSGBOX_AUTOCLOSE
	end

