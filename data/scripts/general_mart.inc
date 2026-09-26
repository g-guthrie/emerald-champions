@ The Center clerk. One service menu; every counter returns to it, and B
@ always steps back one level. The Held Items shelves and Form Items share
@ this entry with the retired standalone battle vendor label.
General_Mart_Script::
Common_EventScript_EmeraldChampionsBattleVendor::
	lock
	faceplayer
	call EmeraldChampions_EventScript_TryStarterKit
	@ The clerk's own state (include/constants/service_vars.h). Nothing the
	@ counters call touches VAR_0x8008-VAR_0x800B.
	setvar VAR_CLERK_SHELF_CURSOR, 0
	setvar VAR_CLERK_SERVICE_CURSOR, 0
	setvar VAR_CLERK_HELD_ITEMS_GREETED, FALSE
	setvar VAR_CLERK_FORM_ITEMS_GREETED, FALSE
	message gText_HowMayIServeYou
	waitmessage
General_Mart_ServiceMenu:
	@ Let the prior native list window finish clearing before another menu can
	@ reuse the same BG tiles and briefly erase its frame.
	delay 1
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, TRUE, General_Mart_ServiceMenuWithForms
	dynmultipush General_Mart_Text_Supplies, 0
	dynmultipush EmeraldChampions_Text_HeldItems, 1
	dynmultipush General_Mart_Text_Sell, 2
	dynmultipush EmeraldChampions_Text_Exit, 3
	dynmultistack 30, 1, FALSE, 4, 0, VAR_CLERK_SERVICE_CURSOR, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, General_Mart_Exit
	copyvar VAR_CLERK_SERVICE_CURSOR, VAR_RESULT
	switch VAR_RESULT
	case 0, General_Mart_Supplies
	case 1, EmeraldChampions_EventScript_HeldItems
	case 2, General_Mart_Sell
	goto General_Mart_Exit

General_Mart_ServiceMenuWithForms:
	dynmultipush General_Mart_Text_Supplies, 0
	dynmultipush EmeraldChampions_Text_HeldItems, 1
	dynmultipush EmeraldChampions_Text_EvolutionArchive, 2
	dynmultipush General_Mart_Text_Sell, 3
	dynmultipush EmeraldChampions_Text_Exit, 4
	dynmultistack 30, 1, FALSE, 5, 0, VAR_CLERK_SERVICE_CURSOR, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, General_Mart_Exit
	copyvar VAR_CLERK_SERVICE_CURSOR, VAR_RESULT
	switch VAR_RESULT
	case 0, General_Mart_Supplies
	case 1, EmeraldChampions_EventScript_HeldItems
	case 2, EmeraldChampions_EventScript_FormItems
	case 3, General_Mart_Sell
	goto General_Mart_Exit

General_Mart_AnythingElse::
	message General_Mart_Text_AnythingElse
	waitmessage
	goto General_Mart_ServiceMenu

General_Mart_Sell:
	special OpenEmeraldChampionsSellMenu
	waitstate
	goto General_Mart_AnythingElse

General_Mart_Exit::
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

General_Mart_Text_Supplies:
	.string "Supplies$"

General_Mart_Text_Sell:
	.string "Sell$"

General_Mart_Text_AnythingElse:
	.string "Is there anything else I can do\n"
	.string "for you?$"

@ Supplies scale with badges and open straight into the Buy list.
General_Mart_Supplies::
	goto_if_set FLAG_BADGE06_GET, Mart_Poke_Center_Feather_Badge
	goto_if_set FLAG_BADGE05_GET, Mart_Poke_Center_Balance_Badge
	goto_if_set FLAG_BADGE04_GET, Mart_Poke_Center_Heat_Badge
	goto_if_set FLAG_BADGE03_GET, Mart_Poke_Center_Dynamo_Badge
	goto_if_set FLAG_BADGE02_GET, Mart_Poke_Center_Knuckle_Badge
	goto_if_set FLAG_BADGE01_GET, Mart_Poke_Center_Stone_Badge
	goto_if_set FLAG_VISITED_RUSTBORO_CITY, Mart_Poke_Center_Rustboro
	goto_if_set FLAG_SYS_POKEMON_GET, Mart_Poke_Center_Pokedex
	pokemartbuy PokeMart_Poke_Center_No_Badges
	goto General_Mart_AnythingElse

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
	pokemartbuy PokeMart_Poke_Center_Pokedex
	goto General_Mart_AnythingElse

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
	pokemartbuy PokeMart_Poke_Center_Rustboro
	goto General_Mart_AnythingElse

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
	pokemartbuy PokeMart_Poke_Center_Stone_Badge
	goto General_Mart_AnythingElse

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
	pokemartbuy PokeMart_Poke_Center_Knuckle_Badge
	goto General_Mart_AnythingElse

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
	pokemartbuy PokeMart_Poke_Center_Dynamo_Badge
	goto General_Mart_AnythingElse

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
	pokemartbuy PokeMart_Poke_Center_Heat_Badge
	goto General_Mart_AnythingElse

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
	pokemartbuy PokeMart_Poke_Center_Balance_Badge
	goto General_Mart_AnythingElse

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
	pokemartbuy PokeMart_Poke_Center_Feather_Badge
	goto General_Mart_AnythingElse

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

