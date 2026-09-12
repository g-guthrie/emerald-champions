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
	msgbox Route108_Text_MissyPostBattle, MSGBOX_NPC
	end

Route108_EventScript_Carolina::
	trainerbattle_double TRAINER_CAROLINA, Route108_Text_CarolinaIntro, Route108_Text_CarolinaDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route108_Text_CarolinaPostBattle, MSGBOX_AUTOCLOSE
	end

Route108_EventScript_Cory::
	msgbox Route108_Text_CoryPostBattle, MSGBOX_NPC
	end

Route108_Text_JEROME_LocalConversation:
	.string "I counted every wave out here.\n"
	.string "Then somebody used SURF.$"

Route108_Text_MATTHEW_LocalConversation:
	.string "MANTINE makes this look easy.\n"
	.string "I would also like some wings.$"
