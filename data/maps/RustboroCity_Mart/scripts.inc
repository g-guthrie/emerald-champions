RustboroCity_Mart_MapScripts:: @ 8214F05
	.byte 0

RustboroCity_Mart_EventScript_PokefanF:: @ 8214F74
	msgbox RustboroCity_Mart_Text_BuyingHealsInCaseOfShroomish, MSGBOX_NPC
	end

RustboroCity_Mart_EventScript_Boy:: @ 8214F7D
	msgbox RustboroCity_Mart_Text_ShouldBuySuperPotionsInstead, MSGBOX_NPC
	end

RustboroCity_Mart_EventScript_BugCatcher:: @ 8214F86
	msgbox RustboroCity_Mart_Text_HeldItemsForBattle, MSGBOX_NPC
	end

RustboroCity_Mart_EventScript_FalseSwipeTM::
	lock
	faceplayer
	goto_if_set FLAG_EC_GIFT_RUSTBORO_CITY_MART, RustboroCity_Mart_EventScript_ReceivedTM98
	msgbox RustboroCity_Mart_Text_ExcuseMeTrainer, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_GIFT_RUSTBORO_CITY_MART
	msgbox RustboroCity_Mart_Text_HaveTM98, MSGBOX_DEFAULT
	release
	end

RustboroCity_Mart_EventScript_ReceivedTM98::
	msgbox RustboroCity_Mart_Text_HaveTM98, MSGBOX_DEFAULT
	release
	end

RustboroCity_Mart_Text_ExcuseMeTrainer:
	.string "Excuse me, Trainer over there.\n"
	.string "Do you have a minute?\p"
	.string "False Swipe leaves a wild Pokémon\n"
	.string "with at least one HP. It helps\l"
	.string "when you're trying to catch one.\p"
	.string "The Center tutor can teach it\n"
	.string "to compatible Pokémon.\p"
	.string "Here's a Rare Candy to help\n"
	.string "your new partners grow!$"

RustboroCity_Mart_Text_HaveTM98:
	.string "Use that Rare Candy to help a\n"
	.string "partner grow one level. It\l"
	.string "cannot take you past the current\l"
	.string "cap.$"

RustboroCity_Mart_Text_BuyingHealsInCaseOfShroomish: @ 8214F8F
	.string "I buy Paralyze Heals and Antidotes\n"
	.string "at the Pokémon Center.\p"
	.string "Just in case I run into Paras\n"
	.string "in Petalburg Woods.$"

RustboroCity_Mart_Text_ShouldBuySuperPotionsInstead: @ 8214FF1
	.string "My Pokémon evolved.\n"
	.string "It has a lot of HP now.\p"
	.string "The Pokémon Center sells Super\n"
	.string "Potions. I should try those!$"

RustboroCity_Mart_Text_HeldItemsForBattle:
	.string "This Mart sells items that Pokémon\n"
	.string "can hold in battle.\p"
	.string "A held item can help your partner\n"
	.string "when you can't use your Bag!$"
