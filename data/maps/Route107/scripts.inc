Route107_MapScripts::
	.byte 0

Route107_EventScript_Darrin::
	trainerbattle_double TRAINER_DARRIN, Route107_Text_DarrinIntro, Route107_Text_DarrinDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route107_Text_DarrinPostBattle, MSGBOX_AUTOCLOSE
	end

Route107_EventScript_Tony::
	msgbox Route107_Text_TonyPostBattle, MSGBOX_NPC
	end

Route107_EventScript_Denise::
	msgbox Route107_Text_DenisePostBattle, MSGBOX_NPC
	end

Route107_EventScript_Beth::
	msgbox Route107_Text_BETH_LocalConversation, MSGBOX_NPC
	end

Route107_EventScript_Lisa::
	msgbox Route107_Text_LisaPostBattle, MSGBOX_NPC
	end

Route107_EventScript_Ray::
	msgbox Route107_Text_RayPostBattle, MSGBOX_NPC
	end

Route107_EventScript_Camron::
	msgbox Route107_Text_CamronPostBattle, MSGBOX_NPC
	end

Route107_Text_BETH_LocalConversation:
	.string "I swim for the scenery.\n"
	.string "The scenery keeps splashing me.$"
