ShoalCave_LowTideEntranceRoom_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ShoalCave_LowTideEntranceRoom_OnTransition
	.byte 0

ShoalCave_LowTideEntranceRoom_OnTransition:
	goto_if_set FLAG_SYS_SHOAL_TIDE, ShoalCave_LowTideEntranceRoom_EventScript_SetHighTide
	goto ShoalCave_LowTideEntranceRoom_EventScript_SetLowTide

ShoalCave_LowTideEntranceRoom_EventScript_SetHighTide::
	setmaplayoutindex LAYOUT_SHOAL_CAVE_HIGH_TIDE_ENTRANCE_ROOM
	end

ShoalCave_LowTideEntranceRoom_EventScript_SetLowTide::
	setmaplayoutindex LAYOUT_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM
	end

@ Shoalmaster Fen. The old ShellBellExpert label is kept until the map object
@ points at the new one.
ShoalCave_LowTideEntranceRoom_EventScript_ShellBellExpert::
ShoalCave_LowTideEntranceRoom_EventScript_Shoalmaster::
	lock
	faceplayer
	message ShoalCave_Text_FenWelcome
	waitmessage
	dynmultipush ShoalCave_Text_TopicTide, 0
	dynmultipush ShoalCave_Text_TopicTrade, 1
	dynmultipush ShoalCave_Text_TopicEvolutions, 2
	dynmultipush ShoalCave_Text_TopicLeave, 3
	dynmultistack 8, 1, FALSE, 4, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, 0, ShoalCave_EventScript_TurnTide
	goto_if_eq VAR_RESULT, 1, ShoalCave_EventScript_Trade
	goto_if_eq VAR_RESULT, 2, ShoalCave_EventScript_Evolutions
	release
	end

ShoalCave_EventScript_Evolutions:
	msgbox ShoalCave_Text_RegionalEvolutions, MSGBOX_DEFAULT
	release
	end

@ The tide is save state. Fen flips it, clears the eight receipts so the
@ deposits restock on the turn, and the warp reloads both layouts.
ShoalCave_EventScript_TurnTide:
	goto_if_set FLAG_SYS_SHOAL_TIDE, ShoalCave_EventScript_OfferEbb
	msgbox ShoalCave_Text_OfferFlood, MSGBOX_YESNO
	goto_if_ne VAR_RESULT, YES, ShoalCave_EventScript_TideWaits
	msgbox ShoalCave_Text_HighTideOffers, MSGBOX_DEFAULT
	setflag FLAG_SYS_SHOAL_TIDE
	goto ShoalCave_EventScript_TurnTideNow

ShoalCave_EventScript_OfferEbb:
	msgbox ShoalCave_Text_OfferEbb, MSGBOX_YESNO
	goto_if_ne VAR_RESULT, YES, ShoalCave_EventScript_TideWaits
	msgbox ShoalCave_Text_LowTideOffers, MSGBOX_DEFAULT
	clearflag FLAG_SYS_SHOAL_TIDE
	goto ShoalCave_EventScript_TurnTideNow

ShoalCave_EventScript_TurnTideNow:
	call ShoalCave_LowTideEntranceRoom_EventScript_ResetShoalItems
	closemessage
	warp MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM, 20, 30
	waitstate
	release
	end

ShoalCave_EventScript_TideWaits:
	msgbox ShoalCave_Text_TideWaits, MSGBOX_DEFAULT
	release
	end

ShoalCave_EventScript_Trade:
	checkitem ITEM_SHOAL_SALT, 4
	goto_if_eq VAR_RESULT, FALSE, ShoalCave_EventScript_NeedMaterials
	checkitem ITEM_SHOAL_SHELL, 4
	goto_if_eq VAR_RESULT, FALSE, ShoalCave_EventScript_NeedMaterials
	special BufferEmeraldChampionsShoalReward
	goto_if_eq VAR_RESULT, 0, ShoalCave_EventScript_TradeStone
	goto_if_eq VAR_RESULT, 1, ShoalCave_EventScript_TradeCharting
	goto ShoalCave_EventScript_TradeDecoration

ShoalCave_EventScript_TradeStone:
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, ShoalCave_EventScript_NeedMaterials
	msgbox ShoalCave_Text_ConfirmExchange, MSGBOX_YESNO
	goto_if_ne VAR_RESULT, YES, ShoalCave_EventScript_DeclineExchange
	special TradeEmeraldChampionsShoalMaterials
	goto_if_eq VAR_RESULT, 0, ShoalCave_EventScript_NeedMaterials
	goto_if_eq VAR_RESULT, 3, ShoalCave_EventScript_NoRoom
	goto_if_eq VAR_RESULT, 2, ShoalCave_EventScript_RewardPC
	playfanfare MUS_OBTAIN_ITEM
	msgbox ShoalCave_Text_RewardBag, MSGBOX_DEFAULT
	waitfanfare
	release
	end

ShoalCave_EventScript_TradeCharting:
	msgbox ShoalCave_Text_ConfirmCharting, MSGBOX_YESNO
	goto_if_ne VAR_RESULT, YES, ShoalCave_EventScript_DeclineExchange
	removeitem ITEM_SHOAL_SALT, 4
	removeitem ITEM_SHOAL_SHELL, 4
	setflag FLAG_EC_SHOAL_ICE_CHARTED
	setflag FLAG_EC_SHOAL_ICE_SIGHTING
	msgbox ShoalCave_Text_Charted, MSGBOX_DEFAULT
	release
	end

ShoalCave_EventScript_TradeDecoration:
	message ShoalCave_Text_WhichDecoration
	waitmessage
	multichoice 0, 0, MULTI_EC_SHOAL_DECORATIONS, FALSE
	closemessage
	switch VAR_RESULT
	case 0, ShoalCave_EventScript_GiveHardChair
	case 1, ShoalCave_EventScript_GivePowderSnowMat
	case 2, ShoalCave_EventScript_GiveSeaPoster
	goto ShoalCave_EventScript_DeclineExchange

ShoalCave_EventScript_GiveHardChair:
	givedecoration DECOR_HARD_CHAIR
	goto ShoalCave_EventScript_FinishDecoration

ShoalCave_EventScript_GivePowderSnowMat:
	givedecoration DECOR_POWDER_SNOW_MAT
	goto ShoalCave_EventScript_FinishDecoration

ShoalCave_EventScript_GiveSeaPoster:
	givedecoration DECOR_SEA_POSTER
	goto ShoalCave_EventScript_FinishDecoration

ShoalCave_EventScript_FinishDecoration:
	goto_if_eq VAR_RESULT, FALSE, ShoalCave_EventScript_NoRoom
	removeitem ITEM_SHOAL_SALT, 4
	removeitem ITEM_SHOAL_SHELL, 4
	msgbox ShoalCave_Text_DecorationTraded, MSGBOX_DEFAULT
	release
	end

ShoalCave_EventScript_NeedMaterials:
	msgbox ShoalCave_Text_Materials, MSGBOX_DEFAULT
	release
	end
ShoalCave_EventScript_DeclineExchange:
	msgbox ShoalCave_Text_Declined, MSGBOX_DEFAULT
	release
	end
ShoalCave_EventScript_NoRoom:
	msgbox ShoalCave_Text_NoRoom, MSGBOX_DEFAULT
	release
	end
ShoalCave_EventScript_RewardPC:
	msgbox ShoalCave_Text_RewardPC, MSGBOX_DEFAULT
	release
	end

ShoalCave_LowTideEntranceRoom_EventScript_ResetShoalItems::
	clearflag FLAG_RECEIVED_SHOAL_SALT_1
	clearflag FLAG_RECEIVED_SHOAL_SALT_2
	clearflag FLAG_RECEIVED_SHOAL_SALT_3
	clearflag FLAG_RECEIVED_SHOAL_SALT_4
	clearflag FLAG_RECEIVED_SHOAL_SHELL_1
	clearflag FLAG_RECEIVED_SHOAL_SHELL_2
	clearflag FLAG_RECEIVED_SHOAL_SHELL_3
	clearflag FLAG_RECEIVED_SHOAL_SHELL_4
	clearflag FLAG_ITEM_SHOAL_CAVE_ICE_ROOM_ICE_STONE
	return

ShoalCave_Text_FenWelcome:
	.string "I'm FEN. I read this water for\n"
	.string "anyone who asks. What do you need?$"
ShoalCave_Text_TopicTide:
	.string "The tide$"
ShoalCave_Text_TopicTrade:
	.string "Salt and shells$"
ShoalCave_Text_TopicEvolutions:
	.string "Cold-water evolutions$"
ShoalCave_Text_TopicLeave:
	.string "See you later$"

ShoalCave_Text_RegionalEvolutions:
	.string "DEWOTT that reaches Lv. 36 anywhere\n"
	.string "in SHOAL CAVE becomes Hisuian\l"
	.string "SAMUROTT.\p"
	.string "In the low-tide ice room, BERGMITE\n"
	.string "at Lv. 37 becomes Hisuian AVALUGG.\p"
	.string "CRABRAWLER and EEVEE change on any\n"
	.string "level-up in that ice room, into\l"
	.string "CRABOMINABLE and GLACEON.\p"
	.string "Galarian MR. MIME lives there too.\n"
	.string "It becomes MR. RIME at Lv. 42.$"
