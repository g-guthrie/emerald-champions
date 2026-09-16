Route114_FossilManiacsHouse_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route114_FossilManiacsHouse_OnTransition
	.byte 0

Route114_FossilManiacsHouse_OnTransition:
	setflag FLAG_LANDMARK_FOSSIL_MANIACS_HOUSE
	end

Route114_FossilManiacsHouse_EventScript_FossilManiacsBrother::
	lock
	faceplayer
	call Route114_FossilManiacsHouse_EventScript_CheckMegaGiftGolurkite
	goto_if_set FLAG_RECEIVED_FOSSIL_MANIAC_METAL_COAT, Route114_FossilManiacsHouse_EventScript_ReceivedMetalCoat
	msgbox Route114_FossilManiacsHouse_Text_HaveThisToDigLikeMyBrother, MSGBOX_DEFAULT
	giveitem ITEM_METAL_COAT
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_FOSSIL_MANIAC_METAL_COAT
	release
	end

Route114_FossilManiacsHouse_EventScript_ReceivedMetalCoat::
	msgbox Route114_FossilManiacsHouse_Text_DigReturnsYouToEntrance, MSGBOX_DEFAULT
	release
	end

@ Golurkite: once the Underpass has been visited, the brother who dug there
@ hands it over. Runs before the Metal Coat branch and never disturbs it.
Route114_FossilManiacsHouse_EventScript_CheckMegaGiftGolurkite::
	goto_if_unset FLAG_LANDMARK_DESERT_UNDERPASS, Route114_FossilManiacsHouse_EventScript_CheckMegaGiftGolurkite_Done
	goto_if_set FLAG_EC_MEGA_GIFT_GOLURKITE, Route114_FossilManiacsHouse_EventScript_CheckMegaGiftGolurkite_Done
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, Route114_FossilManiacsHouse_EventScript_CheckMegaGiftGolurkite_Done
	msgbox Route114_FossilManiacsHouse_Text_MegaGift_GOLURKITE, MSGBOX_DEFAULT
	giveuniqueitem ITEM_GOLURKITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_GOLURKITE
Route114_FossilManiacsHouse_EventScript_CheckMegaGiftGolurkite_Done::
	return

Route114_FossilManiacsHouse_EventScript_RockDisplay::
	msgbox Route114_FossilManiacsHouse_Text_RocksFillDisplayCase, MSGBOX_SIGN
	end

Route114_FossilManiacsHouse_EventScript_Bookshelf::
	msgbox Route114_FossilManiacsHouse_Text_CrammedWithBooks, MSGBOX_SIGN
	end
Route114_FossilManiacsHouse_Text_MegaGift_GOLURKITE:
	.string "My brother dug this clay-colored\n"
	.string "stone out of the UNDERPASS shaft.\p"
	.string "No good to us. You should have\n"
	.string "it instead.$"

Route114_FossilManiacsHouse_Text_HaveThisToDigLikeMyBrother:
	.string "My brother dug up this Metal Coat.\p"
	.string "It evolves Onix or Scyther when used.$"

Route114_FossilManiacsHouse_Text_DigReturnsYouToEntrance:
	.string "If you make a POKéMON DIG inside a\n"
	.string "cave, you're returned to the entrance…\p"
	.string "Old peat has another use: use a PEAT\n"
	.string "BLOCK on URSARING at night to evolve\l"
	.string "it into URSALUNA, anywhere in HOENN.\p"
	.string "LILYCOVE's evolution department\n"
	.string "sells PEAT BLOCKS.$"

Route114_FossilManiacsHouse_Text_RocksFillDisplayCase:
	.string "Rocks in peculiar shapes fill\n"
	.string "the display case…$"

Route114_FossilManiacsHouse_Text_CrammedWithBooks:
	.string "THE COMPOSITION OF STRATA…\n"
	.string "HOW RAIN SHAPES THE LAND…\l"
	.string "STONES, SOIL, AND ROCK…\p"
	.string "It's crammed with books.$"

