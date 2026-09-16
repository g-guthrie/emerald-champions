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
	.string "Ships bring the rest.\n"
	.string "Ten stones and the imported tools.\p"
	.string "LILYCOVE's department store is the\n"
	.string "only place with everything.$"

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
