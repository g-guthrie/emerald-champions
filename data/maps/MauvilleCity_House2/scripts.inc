MauvilleCity_House2_MapScripts::
	.byte 0

MauvilleCity_House2_EventScript_MoveSpecialist::
	msgbox MauvilleCity_House2_Text_MoveSpecialist, MSGBOX_NPC
	end

MauvilleCity_House2_Text_MoveSpecialist:
	.string "I studied the PLEDGE moves and the\n"
	.string "three ultimate starter techniques.\p"
	.string "The POKéMON CENTER specialist can\n"
	.string "teach all six now, free of charge.$"

MauvilleCity_House2_EventScript_Woman::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_COIN_CASE, MauvilleCity_House2_EventScript_ReceivedCoinCase
	msgbox MauvilleCity_House2_Text_TakeMyCoinCase, MSGBOX_DEFAULT
	giveitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_COIN_CASE
	goto MauvilleCity_House2_EventScript_ReceivedCoinCase
	end

MauvilleCity_House2_EventScript_ReceivedCoinCase::
	msgbox MauvilleCity_House2_Text_MachinesPayInMoney, MSGBOX_DEFAULT
	release
	end

MauvilleCity_House2_Text_TakeMyCoinCase:
	.string "I don't sit at the GAME CORNER\n"
	.string "machines any more.\p"
	.string "Take my old COIN CASE. They won't\n"
	.string "let you play without one.$"

MauvilleCity_House2_Text_MachinesPayInMoney:
	.string "The machines pay straight into your\n"
	.string "wallet. Every coin on the counter\l"
	.string "is worth twenty.$"

MauvilleCity_House2_EventScript_MegaGift_AMPHAROSITE::
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_House2_EventScript_MoveSpecialist
	goto_if_set FLAG_EC_MEGA_GIFT_AMPHAROSITE, MauvilleCity_House2_EventScript_MoveSpecialist
	lock
	faceplayer
	msgbox MauvilleCity_House2_EventScript_MegaGift_AMPHAROSITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_AMPHAROSITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_AMPHAROSITE
	release
	goto MauvilleCity_House2_EventScript_MoveSpecialist

MauvilleCity_House2_EventScript_MegaGift_AMPHAROSITE_Text:
	.string "An Ampharos can light a road for\n"
	.string "everyone following behind it.\p"
	.string "Take this stone. A strong partner\n"
	.string "can help others find their way.$"
