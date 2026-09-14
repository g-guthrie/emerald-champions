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
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, LilycoveCity_DepartmentStore_3F_EventScript_NoBracelet
	msgbox LilycoveCity_DepartmentStore_3F_Text_MegaStock, MSGBOX_DEFAULT
	pokemart LilycoveCity_DepartmentStore_3F_Pokemart_MegaStones
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LilycoveCity_DepartmentStore_3F_EventScript_NoBracelet::
	msgbox LilycoveCity_DepartmentStore_3F_Text_NoBracelet, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_3F_Pokemart_MegaStones:
	.2byte ITEM_PIDGEOTITE
	.2byte ITEM_AMPHAROSITE
	.2byte ITEM_MANECTITE
	.2byte ITEM_ALTARIANITE
	pokemartlistend

LilycoveCity_DepartmentStore_3F_Text_MegaStock:
	.string "Our MEGA STONES cost ¥20000 each.\n"
	.string "One of each is all you'll need.\p"
	.string "You may also find these on your travels.\n"
	.string "We won't sell a stone you already own.$"

LilycoveCity_DepartmentStore_3F_Text_NoBracelet:
	.string "MEGA STONES need a MEGA BRACELET.\p"
	.string "STEVEN studies them in GRANITE CAVE,\n"
	.string "near DEWFORD. Visit him first!$"

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
