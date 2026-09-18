VerdanturfTown_Mart_MapScripts:: @ 8202586
	.byte 0

VerdanturfTown_Mart_EventScript_Clerk:: @ 8202587
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart VerdanturfTown_Mart_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_Mart_Pokemart:
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFENSE
	.2byte ITEM_X_SP_ATK
	.2byte ITEM_X_SP_DEF
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ACCURACY
	.2byte ITEM_GUARD_SPEC
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_NONE
	release
	end

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
	.string "You have earned a little help\n"
	.string "for your journey. Please take\l"
	.string "this Rare Candy!$"

VerdanturfTown_Mart_Text_HaveTM66:
	.string "A Rare Candy raises a Pokémon by\n"
	.string "one level, up to your current\l"
	.string "cap.\p"
	.string "For new moves, visit the Center\n"
	.string "tutor.$"

VerdanturfTown_Mart_Text_XSpecialIsCrucial: @ 82025D5
	.string "For any Pokémon match, X Special\n"
	.string "is crucial.\p"
	.string "It jacks up the power of some moves\n"
	.string "even though it's only for one battle.$"

VerdanturfTown_Mart_Text_NoStrategyGuidesForBattleTent: @ 820264C
	.string "They don't seem to sell any winning\n"
	.string "strategy guides for the Battle Tent…\p"
	.string "It seems one must rely on one's\n"
	.string "own wits after all…$"

VerdanturfTown_Mart_Text_NestBallOnWeakenedPokemon:
	.string "The Nest Ball works better on\n"
	.string "weaker Pokémon.$"

