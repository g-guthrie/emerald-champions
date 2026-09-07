Route108_MapScripts::
	.byte 0

Route108_EventScript_Jerome::
	msgbox Route108_Text_JEROME_LocalConversation, MSGBOX_NPC
	end

Route108_EventScript_Matthew::
	msgbox Route108_Text_MATTHEW_LocalConversation, MSGBOX_NPC
	end

Route108_EventScript_Tara::
	trainerbattle_double TRAINER_TARA, Route108_Text_TaraIntro, Route108_Text_TaraDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route108_Text_TaraPostBattle, MSGBOX_AUTOCLOSE
	end

Route108_EventScript_Missy::
	trainerbattle_double TRAINER_MISSY, Route108_Text_MissyIntro, Route108_Text_MissyDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route108_Text_MissyPostBattle, MSGBOX_AUTOCLOSE
	end

Route108_EventScript_Carolina::
	trainerbattle_double TRAINER_CAROLINA, Route108_Text_CarolinaIntro, Route108_Text_CarolinaDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route108_Text_CarolinaPostBattle, MSGBOX_AUTOCLOSE
	end

Route108_EventScript_Cory::
	trainerbattle_double TRAINER_CORY_1, Route108_Text_CoryIntro, Route108_Text_CoryDefeated, EmeraldChampions_Text_NeedTwoPokemon, Route108_EventScript_CoryRegisterMatchCallAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route108_EventScript_CoryRematch
	msgbox Route108_Text_CoryPostBattle, MSGBOX_DEFAULT
	release
	end

Route108_EventScript_CoryRegisterMatchCallAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route108_Text_CoryRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_CORY_1
	release
	end

Route108_EventScript_CoryRematch::
	trainerbattle_rematch_double TRAINER_CORY_1, Route108_Text_CoryRematchIntro, Route108_Text_CoryRematchDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route108_Text_CoryRematchPostBattle, MSGBOX_AUTOCLOSE
	end


Route108_Text_JEROME_LocalConversation:
	.string "I counted every wave out here.\n"
	.string "Then somebody used SURF.$"

Route108_Text_MATTHEW_LocalConversation:
	.string "MANTINE makes this look easy.\n"
	.string "I would also like some wings.$"
