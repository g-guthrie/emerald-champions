LilycoveCity_House2_MapScripts::
	.byte 0

LilycoveCity_House2_EventScript_FatMan::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_LILYCOVE_MOON_STONE, LilycoveCity_House2_EventScript_ReceivedMoonStone
	msgbox LilycoveCity_House2_Text_NotAwakeYetHaveThis, MSGBOX_DEFAULT
	giveitem ITEM_MOON_STONE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_LILYCOVE_MOON_STONE
	msgbox LilycoveCity_House2_Text_SleepIsEssential, MSGBOX_DEFAULT
	release
	end

LilycoveCity_House2_EventScript_ReceivedMoonStone::
	msgbox LilycoveCity_House2_Text_SleepIsEssential, MSGBOX_DEFAULT
	release
	end

LilycoveCity_House2_Text_NotAwakeYetHaveThis:
	.string "Huh? What? Oh, a visitor…\p"
	.string "I was admiring this MOON STONE and\n"
	.string "drifted off. You can have it…\l"
	.string "Yawn…$"

LilycoveCity_House2_Text_SleepIsEssential:
	.string "Yawn… A MOON STONE can help\n"
	.string "certain POKéMON evolve…\p"
	.string "Me? A little more sleep will do\n"
	.string "nicely…$"
