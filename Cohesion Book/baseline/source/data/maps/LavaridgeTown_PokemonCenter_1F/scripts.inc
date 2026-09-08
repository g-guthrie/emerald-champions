LavaridgeTown_PokemonCenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, LavaridgeTown_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

LavaridgeTown_PokemonCenter_1F_OnTransition:
	setrespawn HEAL_LOCATION_LAVARIDGE_TOWN
	call Common_EventScript_UpdateBrineyLocation
	end

LavaridgeTown_PokemonCenter_1F_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_LAVARIDGE_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

LavaridgeTown_PokemonCenter_1F_EventScript_Youngster::
	msgbox LavaridgeTown_PokemonCenter_1F_Text_HotSpringCanInvigorate, MSGBOX_NPC
	end

LavaridgeTown_PokemonCenter_1F_EventScript_Woman::
	msgbox LavaridgeTown_PokemonCenter_1F_Text_TrainersPokemonSpendTimeTogether, MSGBOX_NPC
	end

LavaridgeTown_PokemonCenter_1F_EventScript_Gentleman::
	msgbox LavaridgeTown_PokemonCenter_1F_Text_TrainersShouldRestToo, MSGBOX_NPC
	end

LavaridgeTown_PokemonCenter_1F_EventScript_Lucy::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_LUCY_ABSOLITE, LavaridgeTown_PokemonCenter_1F_EventScript_LucyAfter
	goto_if_defeated TRAINER_LUCY_LAVARIDGE, LavaridgeTown_PokemonCenter_1F_EventScript_LucyReward
	msgbox LavaridgeTown_PokemonCenter_1F_Text_LucyChallenge, MSGBOX_DEFAULT
	trainerbattle_no_intro_double TRAINER_LUCY_LAVARIDGE, LavaridgeTown_PokemonCenter_1F_Text_LucyDefeat, EmeraldChampions_Text_NeedTwoPokemon
LavaridgeTown_PokemonCenter_1F_EventScript_LucyReward::
	checkitemspace ITEM_ABSOLITE, 1
	goto_if_eq VAR_RESULT, FALSE, LavaridgeTown_PokemonCenter_1F_EventScript_LucyBagFull
	giveitem ITEM_ABSOLITE
	setflag FLAG_RECEIVED_LUCY_ABSOLITE
LavaridgeTown_PokemonCenter_1F_EventScript_LucyAfter::
	msgbox LavaridgeTown_PokemonCenter_1F_Text_LucyReward, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_PokemonCenter_1F_EventScript_LucyBagFull::
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_PokemonCenter_1F_Text_LucyChallenge:
	.string "Scott said a serious Trainer would\n"
	.string "come through these springs.\p"
	.string "I am Lucy. I stack the odds until\n"
	.string "they obey. Show me your answer.$"

LavaridgeTown_PokemonCenter_1F_Text_LucyDefeat:
	.string "So… You made your own luck.$"

LavaridgeTown_PokemonCenter_1F_Text_LucyReward:
	.string "That ABSOLITE belongs with someone\n"
	.string "who can force the right outcome.\p"
	.string "The BATTLE FRONTIER awaits.$"

LavaridgeTown_PokemonCenter_1F_Text_TrainersPokemonSpendTimeTogether:
	.string "I think POKéMON get closer to their\n"
	.string "TRAINERS if they spend time together.\p"
	.string "The longer the better.\n"
	.string "That's what I think.$"

LavaridgeTown_PokemonCenter_1F_Text_HotSpringCanInvigorate:
	.string "It's sort of magical how just sitting\n"
	.string "in a hot-spring pool can invigorate.\p"
	.string "I wish I could let my POKéMON\n"
	.string "soak, too.$"

LavaridgeTown_PokemonCenter_1F_Text_TrainersShouldRestToo:
	.string "Hohoho! Hey, kid, you can reach\n"
	.string "the hot springs from here.\p"
	.string "If POKéMON are getting rest, so too\n"
	.string "should their TRAINERS.$"

LavaridgeTown_PokemonCenter_1F_EventScript_MoomooMilk::
	lock
	faceplayer
	message LavaridgeTown_PokemonCenter_1F_Text_MoomooMilkWelcome
	waitmessage
	pokemart PokeMart_PokeCenterLavaridgeMilk
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_PokeCenterLavaridgeMilk::
	.2byte ITEM_MOOMOO_MILK
	.2byte ITEM_NONE

LavaridgeTown_PokemonCenter_1F_Text_MoomooMilkWelcome:
	.string "Moomoo Milk is perfect before or after\n"
	.string "the hot spring. How many would you like?$"
