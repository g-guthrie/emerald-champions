ShoalCave_LowTideEntranceRoom_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ShoalCave_LowTideEntranceRoom_OnTransition
	.byte 0

ShoalCave_LowTideEntranceRoom_OnTransition:
	special UpdateShoalTideFlag
	goto_if_set FLAG_SYS_SHOAL_TIDE, ShoalCave_LowTideEntranceRoom_EventScript_SetHighTide
	goto ShoalCave_LowTideEntranceRoom_EventScript_SetLowTide

ShoalCave_LowTideEntranceRoom_EventScript_SetHighTide::
	setmaplayoutindex LAYOUT_SHOAL_CAVE_HIGH_TIDE_ENTRANCE_ROOM
	end

ShoalCave_LowTideEntranceRoom_EventScript_SetLowTide::
	setmaplayoutindex LAYOUT_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM
	end

ShoalCave_LowTideEntranceRoom_EventScript_ShellBellExpert::
	lock
	faceplayer
	msgbox ShoalCave_Text_RegionalEvolutions, MSGBOX_DEFAULT
	dotimebasedevents
	call_if_set FLAG_SYS_SHOAL_ITEM, ShoalCave_LowTideEntranceRoom_EventScript_ResetShoalItems
	checkitem ITEM_SHOAL_SALT, 4
	goto_if_eq VAR_RESULT, FALSE, ShoalCave_EventScript_NeedMaterials
	checkitem ITEM_SHOAL_SHELL, 4
	goto_if_eq VAR_RESULT, FALSE, ShoalCave_EventScript_NeedMaterials
	setvar VAR_0x8008, ITEM_GLALITITE
	call_if_set FLAG_ITEM_ABANDONED_SHIP_ROOMS_B1F_GLALITITE, ShoalCave_EventScript_PearlReward
	bufferitemname STR_VAR_1, VAR_0x8008
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
ShoalCave_EventScript_PearlReward:
	setvar VAR_0x8008, ITEM_BIG_PEARL
	return
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
	clearflag FLAG_SYS_SHOAL_ITEM
	return

ShoalCave_Text_RegionalEvolutions:
	.string "Level DEWOTT anywhere in SHOAL CAVE at\n"
	.string "Lv. 36 or higher for Hisuian SAMUROTT.\p"
	.string "Level BERGMITE in the low-tide ice room\n"
	.string "at Lv. 37 or higher for Hisuian AVALUGG.\p"
	.string "Elsewhere they take their usual forms.\n"
	.string "Galarian MR. MIME also lives in that\l"
	.string "ice room. It becomes MR. RIME at Lv. 42.$"
