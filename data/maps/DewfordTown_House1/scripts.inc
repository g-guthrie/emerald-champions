DewfordTown_House1_MapScripts::
	.byte 0

DewfordTown_House1_EventScript_Man::
	msgbox DewfordTown_House1_Text_LotToBeSaidForLivingOnIsland, MSGBOX_NPC
	end

DewfordTown_House1_EventScript_Woman::
	msgbox DewfordTown_House1_Text_LifeGoesSlowlyOnIsland, MSGBOX_NPC
	end

DewfordTown_House1_EventScript_Zigzagoon::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZIGZAGOON, CRY_MODE_NORMAL
	msgbox DewfordTown_House1_Text_Zigzagoon, MSGBOX_DEFAULT
	waitmoncry
	release
	end

DewfordTown_House1_Text_LotToBeSaidForLivingOnIsland:
	.string "There's a lot to be said for living on\n"
	.string "a small island like this in harmony with\l"
	.string "POKéMON and the family.$"

DewfordTown_House1_Text_LifeGoesSlowlyOnIsland:
	.string "I left the major port of SLATEPORT\n"
	.string "CITY when I married my husband here.\p"
	.string "Life goes by slowly on this little\n"
	.string "island. But being surrounded by the\l"
	.string "beautiful sea--that's happiness, too.$"

DewfordTown_House1_Text_Zigzagoon:
	.string "ZIGZAGOON: Guguuh!$"


DewfordTown_House1_EventScript_MegaGift_BARBARACITE::
	goto_if_set FLAG_EC_MEGA_GIFT_BARBARACITE, DewfordTown_House1_EventScript_Man
	lock
	faceplayer
	msgbox DewfordTown_House1_EventScript_MegaGift_BARBARACITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_BARBARACITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_BARBARACITE
	release
	goto DewfordTown_House1_EventScript_Man

DewfordTown_House1_EventScript_MegaGift_BARBARACITE_Text:
	.string "I found this in a tide pool, held\n"
	.string "tight between two little claws.\p"
	.string "A Binacle has a stubborn grip.\n"
	.string "Its grown form can use this stone.$"
