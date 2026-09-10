VerdanturfTown_Mart_MapScripts::
	.byte 0

VerdanturfTown_Mart_EventScript_MoveSpecialist::
	msgbox VerdanturfTown_Mart_Text_MoveSpecialist, MSGBOX_NPC
	end

VerdanturfTown_Mart_Text_MoveSpecialist:
	.string "I used to reward patient POKéMON\n"
	.string "with the move PAYBACK.\p"
	.string "The CENTER specialist teaches it now\n"
	.string "whenever the species can learn it.$"

VerdanturfTown_Mart_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart VerdanturfTown_Mart_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
VerdanturfTown_Mart_Pokemart:
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_REPEL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_FLUFFY_TAIL
	pokemartlistend

VerdanturfTown_Mart_EventScript_Boy::
	msgbox VerdanturfTown_Mart_Text_LuxuryBallBuildsFriendship, MSGBOX_NPC
	end

VerdanturfTown_Mart_EventScript_ExpertF::
	msgbox VerdanturfTown_Mart_Text_NoStrategyGuidesForBattleTent, MSGBOX_NPC
	end

VerdanturfTown_Mart_EventScript_Lass::
	msgbox VerdanturfTown_Mart_Text_NestBallOnLowerLevelPokemon, MSGBOX_NPC
	end

VerdanturfTown_Mart_Text_LuxuryBallBuildsFriendship:
	.string "A LUXURY BALL helps a caught POKéMON\n"
	.string "grow friendly more quickly.\p"
	.string "The right Ball can make team building\n"
	.string "feel like part of the adventure.$"

VerdanturfTown_Mart_Text_NoStrategyGuidesForBattleTent:
	.string "They don't seem to sell any winning\n"
	.string "strategy guides for the BATTLE TENT…\p"
	.string "It seems one must rely on one's\n"
	.string "own wits after all…$"

VerdanturfTown_Mart_Text_NestBallOnLowerLevelPokemon:
	.string "The NEST BALL works better on\n"
	.string "lower-level wild POKéMON.\p"
	.string "It is one of several specialty BALLS\n"
	.string "sold around HOENN.$"
