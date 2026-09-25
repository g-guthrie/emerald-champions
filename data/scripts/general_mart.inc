General_Mart_Script::
	lock
	faceplayer
	call EmeraldChampions_EventScript_TryStarterKit
General_Mart_ChooseService:
	message gText_HowMayIServeYou
	waitmessage
	dynmultipush General_Mart_Text_Supplies, 0
	dynmultipush EmeraldChampions_Text_HeldItems, 1
	dynmultipush EmeraldChampions_Text_Exit, 2
	dynmultistack 30, 1, FALSE, 3, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, 0, General_Mart_Supplies
	goto_if_eq VAR_RESULT, 1, Common_EventScript_EmeraldChampionsBattleVendor
	release
	end
General_Mart_Text_Supplies:
	.string "Supplies$"

General_Mart_Text_BuyingOrSelling:
	.string "Certainly! Will you be buying\n"
	.string "or selling today?$"

@ Buy/Sell follows the service menu, so the clerk answers instead of greeting again.
General_Mart_Supplies::
	message General_Mart_Text_BuyingOrSelling
	waitmessage
	goto_if_set FLAG_BADGE06_GET, Mart_Poke_Center_Feather_Badge
	goto_if_set FLAG_BADGE05_GET, Mart_Poke_Center_Balance_Badge
	goto_if_set FLAG_BADGE04_GET, Mart_Poke_Center_Heat_Badge
	goto_if_set FLAG_BADGE03_GET, Mart_Poke_Center_Dynamo_Badge
	goto_if_set FLAG_BADGE02_GET, Mart_Poke_Center_Knuckle_Badge
	goto_if_set FLAG_BADGE01_GET, Mart_Poke_Center_Stone_Badge
	goto_if_set FLAG_VISITED_RUSTBORO_CITY, Mart_Poke_Center_Rustboro
	goto_if_set FLAG_SYS_POKEMON_GET, Mart_Poke_Center_Pokedex
	pokemart PokeMart_Poke_Center_No_Badges
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_Poke_Center_No_Badges::
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_NONE
	release
	end

Mart_Poke_Center_Pokedex::
	pokemart PokeMart_Poke_Center_Pokedex
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_Poke_Center_Pokedex::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_NONE
	release
	end
	
Mart_Poke_Center_Rustboro::
	pokemart PokeMart_Poke_Center_Rustboro
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_Poke_Center_Rustboro::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_NONE
	release
	end
	
Mart_Poke_Center_Stone_Badge::
	pokemart PokeMart_Poke_Center_Stone_Badge
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_Poke_Center_Stone_Badge::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_HONEY
	.2byte ITEM_REPEL
	.2byte ITEM_NONE
	release
	
Mart_Poke_Center_Knuckle_Badge::
	pokemart PokeMart_Poke_Center_Knuckle_Badge
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_Poke_Center_Knuckle_Badge::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_DREAM_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_HONEY
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_NONE
	release
	end	
	
Mart_Poke_Center_Dynamo_Badge::
	pokemart PokeMart_Poke_Center_Dynamo_Badge
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_Poke_Center_Dynamo_Badge::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_DREAM_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_HONEY
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_NONE
	release
	end	

Mart_Poke_Center_Heat_Badge::
	pokemart PokeMart_Poke_Center_Heat_Badge
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_Poke_Center_Heat_Badge::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_DREAM_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_MAX_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_ETHER
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_HONEY
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_NONE
	release
	end	
	
Mart_Poke_Center_Balance_Badge::
	pokemart PokeMart_Poke_Center_Balance_Badge
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_Poke_Center_Balance_Badge::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_DREAM_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_MAX_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_ETHER
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_HONEY
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_NONE
	release
	end	
	
Mart_Poke_Center_Feather_Badge::
	pokemart PokeMart_Poke_Center_Feather_Badge
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_Poke_Center_Feather_Badge::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_DREAM_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_MAX_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_ETHER
	.2byte ITEM_MAX_ETHER
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	.2byte ITEM_HONEY
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_NONE
	release
	end

