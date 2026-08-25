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
	goto_if_set FLAG_VERDANT_GIFT_PAYBACK, VerdanturfTown_Mart_EventScript_ReceivedTM66
	msgbox VerdanturfTown_Mart_Text_SlowPokemon, MSGBOX_DEFAULT
	giveitem ITEM_WEAKNESS_POLICY
	setflag FLAG_VERDANT_GIFT_PAYBACK
	msgbox VerdanturfTown_Mart_Text_HaveTM66, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_Mart_EventScript_ReceivedTM66::
	msgbox VerdanturfTown_Mart_Text_HaveTM66, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_Mart_Text_SlowPokemon:
	.string "Do you think slow Pokémon get tired\n"
	.string "of always taking the first hit?\p"
	.string "They'd be itching for a little\n"
	.string "payback!\p"
	.string "If you've got a sturdy Pokémon, give\n"
	.string "this Weakness Policy a try.$"

VerdanturfTown_Mart_Text_HaveTM66:
	.string "A Weakness Policy sharply raises\n"
	.string "both offenses after a hard hit.$"

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
