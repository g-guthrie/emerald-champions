DewfordTown_House2_MapScripts::
	.byte 0

DewfordTown_House2_EventScript_Man::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_DEWFORD_REAPER_CLOTH, DewfordTown_House2_EventScript_ExplainReaperCloth
	msgbox DewfordTown_House2_Text_WantYouToHaveReaperCloth, MSGBOX_DEFAULT
	giveitem ITEM_REAPER_CLOTH
	goto_if_eq VAR_RESULT, FALSE, DewfordTown_House2_EventScript_NoRoomForScarf
	setflag FLAG_RECEIVED_DEWFORD_REAPER_CLOTH
	release
	end

DewfordTown_House2_EventScript_NoRoomForScarf::
	msgbox DewfordTown_House2_Text_NoRoom, MSGBOX_DEFAULT
	release
	end

DewfordTown_House2_EventScript_ExplainReaperCloth::
	msgbox DewfordTown_House2_Text_ExplainReaperCloth, MSGBOX_DEFAULT
	release
	end

DewfordTown_House2_EventScript_Boy::
	msgbox DewfordTown_House2_Text_BrawlySoCool, MSGBOX_NPC
	end
DewfordTown_House2_Text_WantYouToHaveReaperCloth:
	.string "Gorge your eyes on this rare\n"
	.string "REAPER CLOTH! Spooky, yet stylish!\p"
	.string "You appreciate my dazzling taste.\n"
	.string "Here, I want you to have it!$"

DewfordTown_House2_Text_NoRoom:
	.string "Oh, you don't have room?\p"
	.string "This REAPER CLOTH is rare! Make room\n"
	.string "and come back for it.$"
DewfordTown_House2_Text_ExplainReaperCloth:
	.string "A Reaper Cloth evolves Dusclops.\n"
	.string "Use it from the Bag when ready.$"

DewfordTown_House2_Text_BrawlySoCool:
	.string "Wow, you bothered to cross the sea\n"
	.string "to visit DEWFORD?\p"
	.string "Did you maybe come here because you\n"
	.string "heard about BRAWLY?\p"
	.string "He's so cool…\n"
	.string "Everyone idolizes him.$"

DewfordTown_House2_EventScript_MegaGift_KINGLERITE::
	goto_if_set FLAG_EC_MEGA_GIFT_KINGLERITE, DewfordTown_House2_EventScript_Boy
	lock
	faceplayer
	msgbox DewfordTown_House2_EventScript_MegaGift_KINGLERITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_KINGLERITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_KINGLERITE
	release
	goto DewfordTown_House2_EventScript_Boy

DewfordTown_House2_EventScript_MegaGift_KINGLERITE_Text:
	.string "I followed a Krabby along the shore\n"
	.string "and found this where it was digging.\p"
	.string "When its claw is big and strong,\n"
	.string "this stone could make it stronger!$"

DewfordTown_House2_EventScript_MegaGift_GYARADOSITE::
	goto_if_set FLAG_EC_MEGA_GIFT_GYARADOSITE, DewfordTown_House2_EventScript_Man
	lock
	faceplayer
	msgbox DewfordTown_House2_EventScript_MegaGift_GYARADOSITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_GYARADOSITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_GYARADOSITE
	release
	goto DewfordTown_House2_EventScript_Man

DewfordTown_House2_EventScript_MegaGift_GYARADOSITE_Text:
	.string "The fish you overlook today might\n"
	.string "carry you through a storm tomorrow.\p"
	.string "Raise a Magikarp with care.\n"
	.string "This stone is for its grown form.$"
