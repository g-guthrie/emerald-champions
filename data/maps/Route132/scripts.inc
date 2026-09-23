Route132_MapScripts:: @ 81F74E9
	.byte 0

Route132_EventScript_Gilbert:: @ 81F74EA
	@ This retired encounter has no campaign battle.
	msgbox Route132_Text_GilbertPostBattle, MSGBOX_AUTOCLOSE
	end

Route132_EventScript_Ronald:: @ 81F7518
	trainerbattle_double TRAINER_RONALD, Route132_Text_RonaldIntro, Route132_Text_RonaldDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route132_Text_RonaldPostBattle, MSGBOX_AUTOCLOSE
	end

Route132_EventScript_Paxton:: @ 81F7546
	trainerbattle_double TRAINER_PAXTON, Route132_Text_PaxtonIntro, Route132_Text_PaxtonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route132_Text_PaxtonPostBattle, MSGBOX_AUTOCLOSE
	end

Route132_EventScript_Darcy:: @ 81F755D
	trainerbattle_double TRAINER_DARCY, Route132_Text_DarcyIntro, Route132_Text_DarcyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route132_Text_DarcyPostBattle, MSGBOX_AUTOCLOSE
	end

Route132_EventScript_Jonathan:: @ 81F7574
	trainerbattle_double TRAINER_JONATHAN, Route132_Text_JonathanIntro, Route132_Text_JonathanDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route132_Text_JonathanPostBattle, MSGBOX_AUTOCLOSE
	end

Route132_EventScript_Makayla:: @ 81F758B
	trainerbattle_double TRAINER_MAKAYLA, Route132_Text_MakaylaIntro, Route132_Text_MakaylaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route132_Text_MakaylaPostBattle, MSGBOX_AUTOCLOSE
	end

