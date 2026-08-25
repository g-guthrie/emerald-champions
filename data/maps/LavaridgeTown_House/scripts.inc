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
	goto_if_set FLAG_VERDANT_GIFT_SUCKER_PUNCH, LavaridgeTown_House_EventScript_ReceivedTM94
	msgbox MoveTutor_MimicTeach, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MoveTutor_EventScript_MimicDeclined
	msgbox LavaridgeTown_House_Text_GiveTM94, MSGBOX_DEFAULT
	giveitem ITEM_KINGS_ROCK
	setflag FLAG_VERDANT_GIFT_SUCKER_PUNCH
	msgbox LavaridgeTown_House_Text_HaveTM94, MSGBOX_DEFAULT
	release
	end

MoveTutor_EventScript_MimicDeclined::
	msgbox MoveTutor_MimicDeclined, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_House_EventScript_ReceivedTM94::
	msgbox LavaridgeTown_House_Text_HaveTM94, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_House_Text_HaveTM94::
	.string "A King's Rock may make the target\n"
	.string "flinch when its holder attacks.$"

LavaridgeTown_House_Text_GiveTM94::
	.string "Too bad!\p"
	.string "You can have this King's Rock\n"
	.string "instead.$"

LavaridgeTown_House_Text_WifeWarmingEggInHotSprings:
	.string "My wife's warming an Egg in the hot\n"
	.string "springs. This is what she told me.\p"
	.string "She left two Pokémon with the Day Care.\n"
	.string "And they discovered that Egg!$"

LavaridgeTown_House_Text_Zigzagoon: @ 81FF9BB
	.string "Zigzagoon: Pshoo!$"
