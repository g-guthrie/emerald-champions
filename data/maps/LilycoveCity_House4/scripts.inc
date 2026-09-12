LilycoveCity_House4_MapScripts::
	.byte 0

LilycoveCity_House4_EventScript_Man1::
	msgbox LilycoveCity_House4_Text_MysteriesAtBottomOfSea, MSGBOX_NPC
	end

LilycoveCity_House4_EventScript_Man2::
	msgbox LilycoveCity_House4_Text_UnderwaterTrenchMossdeepSootopolis, MSGBOX_NPC
	end

LilycoveCity_House4_Text_MysteriesAtBottomOfSea:
	.string "This planet's biggest mysteries are\n"
	.string "at the bottom of the sea.\p"
	.string "Somebody said that, but I don't know…$"

LilycoveCity_House4_Text_UnderwaterTrenchMossdeepSootopolis:
	.string "There's a deep underwater trench\n"
	.string "between MOSSDEEP and SOOTOPOLIS.\p"
	.string "That's what someone told me, anyway.$"


LilycoveCity_House4_EventScript_MegaGift_LAPRASITE::
	goto_if_set FLAG_EC_MEGA_GIFT_LAPRASITE, LilycoveCity_House4_EventScript_Man2
	lock
	faceplayer
	msgbox LilycoveCity_House4_EventScript_MegaGift_LAPRASITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_LAPRASITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_LAPRASITE
	release
	goto LilycoveCity_House4_EventScript_Man2

LilycoveCity_House4_EventScript_MegaGift_LAPRASITE_Text:
	.string "A Lapras once carried my family\n"
	.string "home through a terrible storm.\p"
	.string "I kept this stone in gratitude.\n"
	.string "Please find a Lapras to give it to.$"

LilycoveCity_House4_EventScript_MegaGift_STARMINITE::
	goto_if_set FLAG_EC_MEGA_GIFT_STARMINITE, LilycoveCity_House4_EventScript_Man1
	lock
	faceplayer
	msgbox LilycoveCity_House4_EventScript_MegaGift_STARMINITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_STARMINITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_STARMINITE
	release
	goto LilycoveCity_House4_EventScript_Man1

LilycoveCity_House4_EventScript_MegaGift_STARMINITE_Text:
	.string "The sea leaves more than shells\n"
	.string "on our shore after a storm.\p"
	.string "This stone shines like a Starmie.\n"
	.string "Let one carry it beneath the stars.$"
