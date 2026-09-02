FortreeCity_Mart_MapScripts::
	.byte 0

FortreeCity_Mart_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart FortreeCity_Mart_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
FortreeCity_Mart_Pokemart:
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_REVIVE
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_WOOD_MAIL
	pokemartlistend

FortreeCity_Mart_EventScript_Woman::
	msgbox FortreeCity_Mart_Text_SuperRepelBetter, MSGBOX_NPC
	end

FortreeCity_Mart_EventScript_Girl::
	msgbox FortreeCity_Mart_Text_StockUpOnItems, MSGBOX_NPC
	end

FortreeCity_Mart_EventScript_Boy::
	msgbox FortreeCity_Mart_Text_RareCandyMakesMonGrow, MSGBOX_NPC
	end

FortreeCity_Mart_EventScript_Spenser::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_SPENSER_PROTECTOR, FortreeCity_Mart_EventScript_SpenserAfter
	goto_if_defeated TRAINER_SPENSER_FORTREE, FortreeCity_Mart_EventScript_SpenserReward
	msgbox FortreeCity_Mart_Text_SpenserChallenge, MSGBOX_DEFAULT
	trainerbattle_no_intro_double TRAINER_SPENSER_FORTREE, FortreeCity_Mart_Text_SpenserDefeat, EmeraldChampions_Text_NeedTwoPokemon
FortreeCity_Mart_EventScript_SpenserReward::
	checkitemspace ITEM_PROTECTOR, 1
	goto_if_eq VAR_RESULT, FALSE, FortreeCity_Mart_EventScript_SpenserBagFull
	giveitem ITEM_PROTECTOR
	setflag FLAG_RECEIVED_SPENSER_PROTECTOR
FortreeCity_Mart_EventScript_SpenserAfter::
	msgbox FortreeCity_Mart_Text_SpenserAfter, MSGBOX_DEFAULT
	release
	end

FortreeCity_Mart_EventScript_SpenserBagFull::
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	release
	end

FortreeCity_Mart_Text_SpenserChallenge:
	.string "I am SPENSER of the BATTLE PALACE.\p"
	.string "A Pokémon's instincts reveal its\n"
	.string "Trainer's trust. Let me see yours!$"

FortreeCity_Mart_Text_SpenserDefeat:
	.string "Gwahaha! Your trust held firm!$"

FortreeCity_Mart_Text_SpenserAfter:
	.string "Use that PROTECTOR to raise a new\n"
	.string "partner. We will meet at the FRONTIER.$"

FortreeCity_Mart_Text_SuperRepelBetter:
	.string "SUPER REPEL lasts a long time,\n"
	.string "and it gets the job done.\p"
	.string "It's much better than an ordinary\n"
	.string "REPEL.$"

FortreeCity_Mart_Text_StockUpOnItems:
	.string "I always stock up on more items than\n"
	.string "I'm sure I'll need.\p"
	.string "You never know what might happen.\n"
	.string "Better to be safe than sorry!$"

FortreeCity_Mart_Text_RareCandyMakesMonGrow:
	.string "The LEVELER raises your whole party to\n"
	.string "the current level limit at once.\p"
	.string "It makes training completely optional.$"
