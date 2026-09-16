SlateportCity_NameRatersHouse_MapScripts::
	.byte 0

@ Emerald Champions (C7): the NAME RATER duplicated the free POKéMON CENTER
@ specialist, so his service is retired and he redirects to it in one line.
SlateportCity_NameRatersHouse_EventScript_NameRater::
	msgbox SlateportCity_NameRatersHouse_Text_SeeTheCenterSpecialist, MSGBOX_NPC
	end

SlateportCity_NameRatersHouse_Text_SeeTheCenterSpecialist:
	.string "I rated nicknames once.\p"
	.string "The specialist at any POKéMON CENTER\n"
	.string "renames your partners for free now.$"

SlateportCity_NameRatersHouse_EventScript_MegaGift_MALAMARITE::
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, SlateportCity_NameRatersHouse_EventScript_NameRater
	goto_if_set FLAG_EC_MEGA_GIFT_MALAMARITE, SlateportCity_NameRatersHouse_EventScript_NameRater
	lock
	faceplayer
	msgbox SlateportCity_NameRatersHouse_EventScript_MegaGift_MALAMARITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_MALAMARITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_MALAMARITE
	release
	goto SlateportCity_NameRatersHouse_EventScript_NameRater

SlateportCity_NameRatersHouse_EventScript_MegaGift_MALAMARITE_Text:
	.string "An Inkay once watched me write\n"
	.string "its name while hanging upside down.\p"
	.string "It left this curious stone behind.\n"
	.string "A Malamar should know what to do.$"

@ Still used by the free POKéMON CENTER specialist (data/scripts/emerald_champions.inc).
SlateportCity_NameRatersHouse_Text_CritiqueWhichMonNickname:
	.string "Which POKéMON's nickname should\n"
	.string "I critique?$"

SlateportCity_NameRatersHouse_Text_FineNameSuggestBetterOne:
	.string "Hmmm… {STR_VAR_1}, is it? That is\n"
	.string "quite a fine name you bestowed.\p"
	.string "But! What say you, if I were to\n"
	.string "suggest a slightly better name?$"

SlateportCity_NameRatersHouse_Text_WhatShallNewNameBe:
	.string "Ah, good. Then, what shall the new\n"
	.string "nickname be?$"

SlateportCity_NameRatersHouse_Text_NameNoDifferentYetSuperior:
	.string "Done! From now on, this POKéMON\n"
	.string "shall be known as {STR_VAR_1}!\p"
	.string "It looks no different from before,\n"
	.string "and yet, this is vastly superior!\p"
	.string "How fortunate for you!$"

SlateportCity_NameRatersHouse_Text_ThatIsMerelyAnEgg:
	.string "Now, now.\n"
	.string "That is merely an EGG!$"

SlateportCity_NameRatersHouse_Text_MagnificentName:
	.string "Hmmm… {STR_VAR_1} it is!\p"
	.string "This is a magnificent nickname!\n"
	.string "It is impeccably beyond reproach!\p"
	.string "You'll do well to cherish your\n"
	.string "{STR_VAR_1} now and beyond.$"

SlateportCity_NameRatersHouse_Text_MonShallBeKnownAsName:
	.string "Done! From now on, this POKéMON\n"
	.string "shall be known as {STR_VAR_1}!\p"
	.string "It is a better name than before!\n"
	.string "How fortunate for you!$"
