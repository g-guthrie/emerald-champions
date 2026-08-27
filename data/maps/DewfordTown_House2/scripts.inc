DewfordTown_House2_MapScripts:: @ 81FE22D
	.byte 0

DewfordTown_House2_EventScript_Man:: @ 81FE22E
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_SILK_SCARF, DewfordTown_House2_EventScript_ExplainSilkScarf
	msgbox DewfordTown_House2_Text_WantYouToHaveSilkScarf, MSGBOX_DEFAULT
	giveitem ITEM_REAPER_CLOTH
	compare VAR_RESULT, FALSE
	goto_if_eq DewfordTown_House2_EventScript_NoRoomForScarf
	setflag FLAG_RECEIVED_SILK_SCARF
	release
	end

DewfordTown_House2_EventScript_NoRoomForScarf:: @ 81FE25D
	msgbox DewfordTown_House2_Text_NoRoom, MSGBOX_DEFAULT
	release
	end

DewfordTown_House2_EventScript_ExplainSilkScarf:: @ 81FE267
	msgbox DewfordTown_House2_Text_ExplainSilkScarf, MSGBOX_DEFAULT
	release
	end

DewfordTown_House2_EventScript_Boy:: @ 81FE271
	msgbox DewfordTown_House2_Text_BrawlySoCool, MSGBOX_NPC
	end
DewfordTown_House2_Text_WantYouToHaveSilkScarf:
	.string "This Reaper Cloth feels strange…\p"
	.string "I think you should take it.$"

DewfordTown_House2_Text_NoRoom: @ 81FE356
	.string "Oh, you don't have room?\p"
	.string "Now, listen tight, this SCARF is a must-\n"
	.string "have! Why, I would sell all my items\l"
	.string "in order to get it!$"
DewfordTown_House2_Text_ExplainSilkScarf:
	.string "A Reaper Cloth evolves Dusclops when\n"
	.string "it levels up while holding it.$"

DewfordTown_House2_Text_BrawlySoCool: @ 81FE444
	.string "Wow, you bothered to cross the sea\n"
	.string "to visit Dewford?\p"
	.string "Did you maybe come here because you\n"
	.string "heard about Brawly?\p"
	.string "He's so cool…\n"
	.string "Everyone idolizes him.$"

