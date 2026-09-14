FortreeCity_House5_MapScripts::
	.byte 0

FortreeCity_House5_EventScript_PokefanF::
	msgbox FortreeCity_House5_Text_TreeHousesAreGreat, MSGBOX_NPC
	end

FortreeCity_House5_EventScript_Man::
	msgbox FortreeCity_House5_Text_AdaptedToNature, MSGBOX_NPC
	end

FortreeCity_House5_EventScript_Zigzagoon::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZIGZAGOON, CRY_MODE_NORMAL
	msgbox FortreeCity_House5_Text_Zigzagoon, MSGBOX_DEFAULT
	waitmoncry
	release
	end

FortreeCity_House5_Text_TreeHousesAreGreat:
	.string "The tree houses of FORTREE are great!\p"
	.string "I think it's the number one town for\n"
	.string "living together with POKéMON.$"

FortreeCity_House5_Text_AdaptedToNature:
	.string "POKéMON and people have adapted to\n"
	.string "nature for survival.\p"
	.string "There's no need to make nature\n"
	.string "conform to the way we want to live.$"

FortreeCity_House5_Text_Zigzagoon:
	.string "ZIGZAGOON: Bufuu!$"


FortreeCity_House5_EventScript_MegaGift_CHIMECHITE::
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, FortreeCity_House5_EventScript_PokefanF
	goto_if_set FLAG_EC_MEGA_GIFT_CHIMECHITE, FortreeCity_House5_EventScript_PokefanF
	lock
	faceplayer
	msgbox FortreeCity_House5_EventScript_MegaGift_CHIMECHITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_CHIMECHITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_CHIMECHITE
	release
	goto FortreeCity_House5_EventScript_PokefanF

FortreeCity_House5_EventScript_MegaGift_CHIMECHITE_Text:
	.string "Listen to the chimes in the trees.\n"
	.string "No two breezes sound quite alike.\p"
	.string "I kept this for a Chimecho.\n"
	.string "Perhaps yours will give it a voice.$"
