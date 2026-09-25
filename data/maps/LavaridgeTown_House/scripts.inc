LavaridgeTown_House_MapScripts::
	.byte 0

LavaridgeTown_House_EventScript_OldMan:: @ 81FF912
	msgbox LavaridgeTown_House_Text_WifeWarmingEggInHotSprings, MSGBOX_NPC
	end

LavaridgeTown_House_EventScript_Zigzagoon:: @ 81FF91B
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZIGZAGOON, 0
	msgbox LavaridgeTown_House_Text_Zigzagoon, MSGBOX_DEFAULT
	waitmoncry
	release
	end

LavaridgeTown_House_EventScript_SuckerPunchTM::
	lock
	faceplayer
	goto_if_set FLAG_EC_GIFT_LAVARIDGE_TOWN_HOUSE, LavaridgeTown_House_EventScript_ReceivedTM94
	msgbox LavaridgeTown_House_Text_OfferCandy, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq LavaridgeTown_House_EventScript_DeclineCandy
	msgbox LavaridgeTown_House_Text_GiveTM94, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_GIFT_LAVARIDGE_TOWN_HOUSE
	msgbox LavaridgeTown_House_Text_HaveTM94, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_House_EventScript_ReceivedTM94::
	msgbox LavaridgeTown_House_Text_HaveTM94, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_House_Text_HaveTM94::
	.string "One candy, one level. But it won't\n"
	.string "go past what your Badges allow!\p"
	.string "For new techniques, see the Move\n"
	.string "Tutor at the Pokémon Center!$"

LavaridgeTown_House_Text_GiveTM94::
	.string "Heh! Then it's yours.\n"
	.string "A ninja never takes back a gift!$"

LavaridgeTown_House_Text_WifeWarmingEggInHotSprings:
	.string "My wife's warming an Egg in the hot\n"
	.string "springs. This is what she told me.\p"
	.string "She left two Pokémon with the Day Care.\n"
	.string "And they discovered that Egg!$"

LavaridgeTown_House_Text_Zigzagoon: @ 81FF9BB
	.string "Zigzagoon: Pshoo!$"

LavaridgeTown_House_EventScript_DeclineCandy::
	msgbox LavaridgeTown_House_Text_DeclineCandy, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_House_Text_OfferCandy:
	.string "Training near a volcano is hard\n"
	.string "work, isn't it?\p"
	.string "Would you like a Rare Candy\n"
	.string "to help one of your partners grow?$"
LavaridgeTown_House_Text_DeclineCandy:
	.string "I'll keep it for you. Come back\n"
	.string "if you change your mind!$"
