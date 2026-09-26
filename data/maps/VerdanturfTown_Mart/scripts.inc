VerdanturfTown_Mart_MapScripts:: @ 8202586
	.byte 0

VerdanturfTown_Mart_EventScript_Boy:: @ 82025BA
	msgbox VerdanturfTown_Mart_Text_XSpecialIsCrucial, MSGBOX_NPC
	end

VerdanturfTown_Mart_EventScript_ExpertF:: @ 82025C3
	msgbox VerdanturfTown_Mart_Text_NoStrategyGuidesForBattleTent, MSGBOX_NPC
	end

VerdanturfTown_Mart_EventScript_Lass:: @ 82025CC
	msgbox VerdanturfTown_Mart_Text_NestBallOnWeakenedPokemon, MSGBOX_NPC
	end

VerdanturfTown_Mart_EventScript_PaybackTM::
	lock
	faceplayer
	goto_if_set FLAG_EC_GIFT_VERDANTURF_TOWN_MART, VerdanturfTown_Mart_EventScript_ReceivedTM66
	msgbox VerdanturfTown_Mart_Text_SlowPokemon, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_GIFT_VERDANTURF_TOWN_MART
	msgbox VerdanturfTown_Mart_Text_HaveTM66, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_Mart_EventScript_ReceivedTM66::
	msgbox VerdanturfTown_Mart_Text_HaveTM66, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_Mart_Text_SlowPokemon:
	.string "You've come such a long way!\n"
	.string "Please take this Rare Candy!$"

VerdanturfTown_Mart_Text_HaveTM66:
	.string "A Rare Candy helps a Pokémon grow\n"
	.string "a level, as far as your Badges allow.$"

VerdanturfTown_Mart_Text_XSpecialIsCrucial: @ 82025D5
	.string "For any Pokémon match, the right\n"
	.string "held item is crucial.\p"
	.string "It jacks up the power of some moves\n"
	.string "for every battle it's held in.$"

VerdanturfTown_Mart_Text_NoStrategyGuidesForBattleTent: @ 820264C
	.string "They don't seem to sell any winning\n"
	.string "strategy guides for the Battle Tent…\p"
	.string "It seems one must rely on one's\n"
	.string "own wits after all…$"

VerdanturfTown_Mart_Text_NestBallOnWeakenedPokemon:
	.string "The Nest Ball works better on\n"
	.string "weaker Pokémon.$"

