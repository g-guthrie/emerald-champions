Route108_MapScripts:: @ 81EE689
	.byte 0

Route108_EventScript_Jerome:: @ 81EE68A
	@ This retired encounter has no campaign battle.
	msgbox Route108_Text_JeromePostBattle, MSGBOX_AUTOCLOSE
	end

Route108_EventScript_Matthew:: @ 81EE6A1
	@ This retired encounter has no campaign battle.
	msgbox Route108_Text_MatthewPostBattle, MSGBOX_AUTOCLOSE
	end

Route108_EventScript_Tara:: @ 81EE6B8
	trainerbattle_double TRAINER_TARA, Route108_Text_TaraIntro, Route108_Text_TaraDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route108_Text_TaraPostBattle, MSGBOX_AUTOCLOSE
	end

Route108_EventScript_Carolina:: @ 81EE6E6
	trainerbattle_double TRAINER_CAROLINA, Route108_Text_CarolinaIntro, Route108_Text_CarolinaDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route108_Text_CarolinaPostBattle, MSGBOX_AUTOCLOSE
	end
