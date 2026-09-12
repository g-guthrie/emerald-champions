LavaridgeTown_House_MapScripts::
	.byte 0

LavaridgeTown_House_EventScript_OldMan::
	msgbox LavaridgeTown_House_Text_WifeWarmingEggInHotSprings, MSGBOX_NPC
	end

LavaridgeTown_House_EventScript_Zigzagoon::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZIGZAGOON, CRY_MODE_NORMAL
	msgbox LavaridgeTown_House_Text_Zigzagoon, MSGBOX_DEFAULT
	waitmoncry
	release
	end

LavaridgeTown_House_Text_WifeWarmingEggInHotSprings:
	.string "My wife's warming an EGG in the hot\n"
	.string "springs. This is what she told me.\p"
	.string "She left two POKéMON with the DAY CARE.\n"
	.string "And they discovered that EGG!$"

LavaridgeTown_House_Text_Zigzagoon:
	.string "ZIGZAGOON: Pshoo!$"


LavaridgeTown_House_EventScript_MegaGift_BLAZIKENITE::
	goto_if_set FLAG_EC_MEGA_GIFT_BLAZIKENITE, LavaridgeTown_House_EventScript_MimicTutor
	lock
	faceplayer
	msgbox LavaridgeTown_House_EventScript_MegaGift_BLAZIKENITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_BLAZIKENITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_BLAZIKENITE
	release
	goto LavaridgeTown_House_EventScript_MimicTutor

LavaridgeTown_House_EventScript_MegaGift_BLAZIKENITE_Text:
	.string "I learned footwork by watching\n"
	.string "Blaziken train on the hot rocks.\p"
	.string "A flame needs room to grow.\n"
	.string "Take this stone on your journey.$"

LavaridgeTown_House_EventScript_MegaGift_MACHAMPITE::
	goto_if_set FLAG_EC_MEGA_GIFT_MACHAMPITE, LavaridgeTown_House_EventScript_OldMan
	lock
	faceplayer
	msgbox LavaridgeTown_House_EventScript_MegaGift_MACHAMPITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_MACHAMPITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_MACHAMPITE
	release
	goto LavaridgeTown_House_EventScript_OldMan

LavaridgeTown_House_EventScript_MegaGift_MACHAMPITE_Text:
	.string "Machamp carried the stones that\n"
	.string "built some of these old baths.\p"
	.string "This one is small enough to carry\n"
	.string "yourself. Let a Machamp hold it.$"
