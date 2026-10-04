MauvilleCity_Mart_MapScripts:: @ 82110E5
	.byte 0

MauvilleCity_Mart_EventScript_Clerk:: @ 82110E6
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	goto_if_set FLAG_BADGE04_GET, MauvilleCity_Mart_EventScript_FourBadges
	goto_if_set FLAG_BADGE03_GET, MauvilleCity_Mart_EventScript_ThreeBadges
	pokemart MauvilleCity_Mart_PokemartEarly
	goto MauvilleCity_Mart_EventScript_Farewell

MauvilleCity_Mart_EventScript_ThreeBadges::
	pokemart MauvilleCity_Mart_PokemartTrade
	goto MauvilleCity_Mart_EventScript_Farewell

MauvilleCity_Mart_EventScript_FourBadges::
	pokemart MauvilleCity_Mart_Pokemart

MauvilleCity_Mart_EventScript_Farewell::
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
MauvilleCity_Mart_Pokemart::
	.2byte ITEM_UPGRADE
	.2byte ITEM_METAL_POWDER
	.2byte ITEM_LEEK
	.2byte ITEM_LIGHT_BALL
	.2byte ITEM_THICK_CLUB
	.2byte ITEM_DEEP_SEA_TOOTH
	.2byte ITEM_DEEP_SEA_SCALE
	.2byte ITEM_METAL_COAT
	.2byte ITEM_KINGS_ROCK
	.2byte ITEM_PRISM_SCALE
	.2byte ITEM_SACHET
	.2byte ITEM_WHIPPED_DREAM
	.2byte ITEM_NONE
	release
	end

MauvilleCity_Mart_EventScript_ExpertM:: @ 821111C
	msgbox MauvilleCity_Mart_Text_ItemsToTemporarilyElevateStats, MSGBOX_NPC
	end

MauvilleCity_Mart_EventScript_Man:: @ 8211125
	msgbox MauvilleCity_Mart_Text_DecisionsDetermineBattle, MSGBOX_NPC
	end

MauvilleCity_Mart_Text_ItemsToTemporarilyElevateStats: @ 821112E
	.string "Did you know every Pokémon joins you\n"
	.string "already fully trained?\p"
	.string "Battles won't change its training,\n"
	.string "but the move tutor at a Pokémon\l"
	.string "Center can plan a new spread.$"

MauvilleCity_Mart_Text_DecisionsDetermineBattle: @ 82111D8
	.string "Use a certain move, or switch to\n"
	.string "another Pokémon instead…\p"
	.string "The Trainer's decisions determine how\n"
	.string "battles turn out, I think.$"


	.align 2
MauvilleCity_Mart_PokemartEarly:
	.2byte ITEM_LIGHT_BALL
	.2byte ITEM_LEEK
	.2byte ITEM_METAL_POWDER
	.2byte ITEM_NONE

	.align 2
MauvilleCity_Mart_PokemartTrade:
	.2byte ITEM_LIGHT_BALL
	.2byte ITEM_LEEK
	.2byte ITEM_METAL_POWDER
	.2byte ITEM_UPGRADE
	.2byte ITEM_DEEP_SEA_TOOTH
	.2byte ITEM_DEEP_SEA_SCALE
	.2byte ITEM_METAL_COAT
	.2byte ITEM_KINGS_ROCK
	.2byte ITEM_PRISM_SCALE
	.2byte ITEM_SACHET
	.2byte ITEM_WHIPPED_DREAM
	.2byte ITEM_NONE
