BattleFrontier_Lounge6_MapScripts:: @ 8264FEC
	.byte 0

@ Inclement moved this lounge's Type: Null trade to FortreeCity_House1
@ (FLAG_BATTLE_FRONTIER_TRADE_DONE); the Beauty here now just chats.
BattleFrontier_Lounge6_EventScript_Beauty::
	msgbox BattleFrontier_Lounge6_Text_Beauty, MSGBOX_NPC
	end

BattleFrontier_Lounge6_Text_Beauty:
	.string "Every Trainer here battles in\n"
	.string "pairs now. Isn't that thrilling?$"
