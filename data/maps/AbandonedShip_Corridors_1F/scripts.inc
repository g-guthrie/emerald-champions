AbandonedShip_Corridors_1F_MapScripts::
	.byte 0

AbandonedShip_Corridors_1F_EventScript_Youngster::
	msgbox AbandonedShip_Corridors_1F_Text_IsntItFunHere, MSGBOX_NPC
	end

AbandonedShip_Corridors_1F_EventScript_Charlie::
	msgbox AbandonedShip_Corridors_1F_Text_CharliePostBattle, MSGBOX_NPC
	end

AbandonedShip_Corridors_1F_Text_CharliePostBattle:
	.string "It's not easy throwing POKé BALLS\n"
	.string "while hanging on to an inner tube!$"

AbandonedShip_Corridors_1F_Text_IsntItFunHere:
	.string "Isn't it fun here?\n"
	.string "I get excited just being here!$"
