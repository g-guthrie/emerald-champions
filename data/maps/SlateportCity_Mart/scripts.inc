SlateportCity_Mart_MapScripts::
	.byte 0

SlateportCity_Mart_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart SlateportCity_Mart_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
SlateportCity_Mart_Pokemart:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_HARBOR_MAIL
	pokemartlistend

SlateportCity_Mart_EventScript_BlackBelt::
	msgbox SlateportCity_Mart_Text_MarketAndMartStockDifferentGoods, MSGBOX_NPC
	end

SlateportCity_Mart_EventScript_Man::
	lock
	faceplayer
	msgbox SlateportCity_Mart_Text_GreatBallIsBetter, MSGBOX_DEFAULT
	pokemart SlateportCity_Mart_EvolutionStock
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

SlateportCity_Mart_Text_MarketAndMartStockDifferentGoods:
	.string "The MARKET has unusual merchandise.\p"
	.string "The MART is still a reliable place to\n"
	.string "restock everyday field supplies.$"

SlateportCity_Mart_Text_GreatBallIsBetter:
	.string "Ships bring evolution items from afar.\n"
	.string "I sell stones and a few special tools.\p"
	.string "LILYCOVE's department store carries\n"
	.string "every kind. Have a look at mine!$"

	.align 2
SlateportCity_Mart_EvolutionStock:
	.2byte ITEM_FIRE_STONE
	.2byte ITEM_WATER_STONE
	.2byte ITEM_THUNDER_STONE
	.2byte ITEM_LEAF_STONE
	.2byte ITEM_MOON_STONE
	.2byte ITEM_SUN_STONE
	.2byte ITEM_ICE_STONE
	.2byte ITEM_DAWN_STONE
	.2byte ITEM_DUSK_STONE
	.2byte ITEM_SHINY_STONE
	.2byte ITEM_LINKING_CORD
	.2byte ITEM_METAL_COAT
	.2byte ITEM_KINGS_ROCK
	.2byte ITEM_DRAGON_SCALE
	.2byte ITEM_DEEP_SEA_TOOTH
	.2byte ITEM_DEEP_SEA_SCALE
	pokemartlistend
