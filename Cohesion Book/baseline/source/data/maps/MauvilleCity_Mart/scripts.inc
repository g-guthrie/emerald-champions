MauvilleCity_Mart_MapScripts::
	.byte 0

MauvilleCity_Mart_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart MauvilleCity_Mart_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
MauvilleCity_Mart_Pokemart:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_HEAL_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_DUSK_BALL
	.2byte ITEM_QUICK_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_POKE_TOY
	pokemartlistend

MauvilleCity_Mart_EventScript_ExpertM::
	msgbox MauvilleCity_Mart_Text_ItemsToTemporarilyElevateStats, MSGBOX_NPC
	end

MauvilleCity_Mart_EventScript_Man::
	msgbox MauvilleCity_Mart_Text_DecisionsDetermineBattle, MSGBOX_NPC
	end

MauvilleCity_Mart_Text_ItemsToTemporarilyElevateStats:
	.string "Trainer battles do not allow BAG items.\n"
	.string "This shop stocks specialized POKé BALLS\l"
	.string "instead, so you can build more teams.$"

MauvilleCity_Mart_Text_DecisionsDetermineBattle:
	.string "Choose a move, switch a partner, or\n"
	.string "protect a position…\p"
	.string "The TRAINER's decisions determine how\n"
	.string "battles turn out.$"
