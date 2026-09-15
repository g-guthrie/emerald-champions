LilycoveCity_DepartmentStore_3F_MapScripts::
	.byte 0

LilycoveCity_DepartmentStore_3F_EventScript_ClerkLeft::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_3F_Pokemart_CaptureBalls
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_3F_Pokemart_CaptureBalls:
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_QUICK_BALL
	.2byte ITEM_DUSK_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_LUXURY_BALL
	pokemartlistend

LilycoveCity_DepartmentStore_3F_EventScript_ClerkRight::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_3F_Pokemart_StatBoosters
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_3F_Pokemart_StatBoosters:
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_MAX_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_MAX_REVIVE
	.2byte ITEM_ETHER
	.2byte ITEM_MAX_ETHER
	.2byte ITEM_ELIXIR
	pokemartlistend

LilycoveCity_DepartmentStore_3F_EventScript_TriathleteM::
	msgbox LilycoveCity_DepartmentStore_3F_Text_RightBallBuildsTeamsFast, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_3F_EventScript_PokefanM::
	msgbox LilycoveCity_DepartmentStore_3F_Text_TimerBallsRewardPatience, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_3F_EventScript_Woman::
	msgbox LilycoveCity_DepartmentStore_3F_Text_QuickBallFirstTurnCatch, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_3F_Text_RightBallBuildsTeamsFast:
	.string "For building a new team quickly,\n"
	.string "the right kind of BALL is best.\p"
	.string "QUICK BALLS reward a fast throw,\n"
	.string "while DUSK BALLS shine in caves.$"

LilycoveCity_DepartmentStore_3F_Text_TimerBallsRewardPatience:
	.string "Some rare POKéMON take patience.\p"
	.string "A TIMER BALL grows stronger as the\n"
	.string "battle lasts, so I always carry a few.$"

LilycoveCity_DepartmentStore_3F_Text_QuickBallFirstTurnCatch:
	.string "I caught a new partner with a\n"
	.string "QUICK BALL on the very first turn!$"
