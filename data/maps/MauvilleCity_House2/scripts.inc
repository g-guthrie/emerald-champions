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
	msgbox MauvilleCity_House2_Text_ArchiveAcrossTheWay, MSGBOX_NPC
	end

MauvilleCity_House2_Text_ArchiveAcrossTheWay:
	.string "The hall across the way is the\n"
	.string "STARTER ARCHIVE now.\p"
	.string "No coins, no tables. You pay at the\n"
	.string "counter like anywhere else.$"

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
