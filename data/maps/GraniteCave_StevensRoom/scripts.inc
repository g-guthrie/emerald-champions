GraniteCave_StevensRoom_MapScripts::
	.byte 0

GraniteCave_StevensRoom_EventScript_Steven::
	lock
	faceplayer
	goto_if_set FLAG_DELIVERED_STEVEN_LETTER, GraniteCave_StevensRoom_EventScript_CheckKnuckleBadge
	setvar VAR_0x8004, ITEM_LETTER
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_MissingHandoffItem
	msgbox GraniteCave_StevensRoom_Text_ImStevenLetterForMe, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_LETTER
	call Common_EventScript_PlayerHandedOverTheItem
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_MissingHandoffItem
	setflag FLAG_DELIVERED_STEVEN_LETTER

GraniteCave_StevensRoom_EventScript_CheckKnuckleBadge::
	goto_if_unset FLAG_BADGE02_GET, GraniteCave_StevensRoom_EventScript_WaitForKnuckleBadge
	setvar VAR_0x8004, ITEM_MEGA_RING
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, TRUE, GraniteCave_StevensRoom_EventScript_StarterGifts
	msgbox GraniteCave_StevensRoom_Text_ThankYouTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_MEGA_RING
	goto_if_eq VAR_RESULT, FALSE, GraniteCave_StevensRoom_EventScript_BagFull
GraniteCave_StevensRoom_EventScript_StarterGifts::
	setvar VAR_0x8008, 0
	setvar VAR_0x8007, TRUE
	setvar VAR_0x8006, FALSE

GraniteCave_StevensRoom_EventScript_StarterStoneLoop::
	special GiveEmeraldChampionsStarterMegaStoneAtIndex
	goto_if_eq VAR_RESULT, FALSE, GraniteCave_StevensRoom_EventScript_StarterStonePending
	goto GraniteCave_StevensRoom_EventScript_StarterStoneReceipt

GraniteCave_StevensRoom_EventScript_StarterStonePending::
	setvar VAR_0x8007, FALSE

GraniteCave_StevensRoom_EventScript_StarterStoneReceipt::
	goto_if_eq VAR_0x8004, ITEM_NONE, GraniteCave_StevensRoom_EventScript_NextStarterStone
	setvar VAR_0x8006, TRUE
	goto_if_eq VAR_0x8005, 0, GraniteCave_StevensRoom_EventScript_NextStarterStone
	copyvar VAR_0x8000, VAR_0x8004
	copyvar VAR_0x800B, VAR_0x8005
	call GraniteCave_StevensRoom_EventScript_StoneReceipt

GraniteCave_StevensRoom_EventScript_NextStarterStone::
	addvar VAR_0x8008, 1
	goto_if_lt VAR_0x8008, 4, GraniteCave_StevensRoom_EventScript_StarterStoneLoop
	goto_if_eq VAR_0x8007, FALSE, GraniteCave_StevensRoom_EventScript_StoneStorageFull
	goto_if_eq VAR_0x8006, FALSE, GraniteCave_StevensRoom_EventScript_NoStarterStone
	goto GraniteCave_StevensRoom_EventScript_ArchiveOpen

GraniteCave_StevensRoom_EventScript_NoStarterStone::
	msgbox GraniteCave_StevensRoom_Text_NoStarterStone, MSGBOX_DEFAULT

GraniteCave_StevensRoom_EventScript_ArchiveOpen::
	msgbox GraniteCave_StevensRoom_Text_ArchiveOpen, MSGBOX_DEFAULT

GraniteCave_StevensRoom_EventScript_RegisterSteven::
	goto_if_set FLAG_REGISTERED_STEVEN_POKENAV, GraniteCave_StevensRoom_EventScript_Finish
	msgbox GraniteCave_StevensRoom_Text_CouldBecomeChampionLetsRegister, MSGBOX_DEFAULT
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox GraniteCave_StevensRoom_Text_RegisteredSteven, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_REGISTERED_STEVEN_POKENAV

GraniteCave_StevensRoom_EventScript_Finish::
	call_if_unset FLAG_DELIVERED_DEVON_GOODS, GraniteCave_StevensRoom_EventScript_ManorHandoff
	call GraniteCave_StevensRoom_EventScript_CobalionFarewell
	closemessage
	setflag FLAG_HIDE_GRANITE_CAVE_STEVEN
	call_if_eq VAR_FACING, DIR_NORTH, GraniteCave_StevensRoom_EventScript_StevenExitNorth
	call_if_eq VAR_FACING, DIR_SOUTH, GraniteCave_StevensRoom_EventScript_StevenExitSouth
	call_if_eq VAR_FACING, DIR_WEST, GraniteCave_StevensRoom_EventScript_StevenExitWestEast
	call_if_eq VAR_FACING, DIR_EAST, GraniteCave_StevensRoom_EventScript_StevenExitWestEast
	playse SE_EXIT
	removeobject LOCALID_GRANITE_CAVE_STEVEN
	release
	end

GraniteCave_StevensRoom_EventScript_CobalionFarewell::
	setvar VAR_0x8004, LEGENDARY_SIGN_COBALION
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, GraniteCave_StevensRoom_EventScript_CobalionUncaught

GraniteCave_StevensRoom_EventScript_CobalionCaught::
	msgbox GraniteCave_StevensRoom_Text_CobalionCaught, MSGBOX_DEFAULT
	return

GraniteCave_StevensRoom_EventScript_CobalionUncaught::
	msgbox GraniteCave_StevensRoom_Text_IveGotToHurryAlong, MSGBOX_DEFAULT
	return

GraniteCave_StevensRoom_EventScript_ManorHandoff::
	msgbox GraniteCave_StevensRoom_Text_ManorHandoff, MSGBOX_DEFAULT
	return

GraniteCave_StevensRoom_EventScript_WaitForKnuckleBadge::
	msgbox GraniteCave_StevensRoom_Text_ReturnWithKnuckleBadge, MSGBOX_DEFAULT
	release
	end

GraniteCave_StevensRoom_EventScript_StevenExitNorth::
	applymovement LOCALID_GRANITE_CAVE_STEVEN, GraniteCave_StevensRoom_Movement_StevenExit
	waitmovement 0
	return

GraniteCave_StevensRoom_EventScript_StevenExitWestEast::
	applymovement LOCALID_PLAYER, GraniteCave_StevensRoom_Movement_PlayerTurnTowardExit
	applymovement LOCALID_GRANITE_CAVE_STEVEN, GraniteCave_StevensRoom_Movement_StevenExit
	waitmovement 0
	return

GraniteCave_StevensRoom_EventScript_StevenExitSouth::
	applymovement LOCALID_PLAYER, GraniteCave_StevensRoom_Movement_PlayerTurnTowardExit
	applymovement LOCALID_GRANITE_CAVE_STEVEN, GraniteCave_StevensRoom_Movement_StevenExitSouth
	waitmovement 0
	return

GraniteCave_StevensRoom_EventScript_BagFull::
	msgbox GraniteCave_StevensRoom_Text_OhBagIsFull, MSGBOX_DEFAULT
	release
	end

GraniteCave_StevensRoom_EventScript_StoneStorageFull::
	msgbox GraniteCave_StevensRoom_Text_StoneStorageFull, MSGBOX_DEFAULT
	release
	end

GraniteCave_StevensRoom_EventScript_StoneReceipt::
	goto_if_eq VAR_0x800B, 3, GraniteCave_StevensRoom_EventScript_StoneCashReceipt
	bufferitemnameplural STR_VAR_2, VAR_0x8000, 1
	playfanfare MUS_OBTAIN_ITEM
	msgbox gText_ObtainedTheItem, MSGBOX_DEFAULT
	waitfanfare
	goto_if_eq VAR_0x800B, 2, GraniteCave_StevensRoom_EventScript_StonePCReceipt
	putitemaway VAR_0x8000
	return

GraniteCave_StevensRoom_EventScript_StonePCReceipt::
	msgbox GraniteCave_StevensRoom_Text_StoneInPC, MSGBOX_DEFAULT
	return

GraniteCave_StevensRoom_Text_StoneInPC:
	.string "{PLAYER} put the {STR_VAR_2}\n"
	.string "in the PC.$"

GraniteCave_StevensRoom_Text_StoneStorageFull:
	.string "Your BAG and PC have no room for the\n"
	.string "remaining stone. I'll keep it here.\p"
	.string "Make some room, then speak to me\n"
	.string "again. Your gift will still be here.$"

GraniteCave_StevensRoom_Movement_StevenExit:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	delay_8
	step_end

GraniteCave_StevensRoom_Movement_PlayerTurnTowardExit:
	delay_16
	delay_16
	delay_16
	walk_in_place_faster_up
	step_end

GraniteCave_StevensRoom_Movement_StevenExitSouth:
	walk_left
	walk_up
	walk_up
	walk_up
	walk_right
	walk_up
	walk_up
	delay_8
	step_end

GraniteCave_StevensRoom_Text_ImStevenLetterForMe:
	.string "I'm STEVEN. Forgive me, I didn't\n"
	.string "hear you come in.\p"
	.string "Rare POKéMON have been seeking\n"
	.string "shelter in these caves. Their\l"
	.string "movements seem to follow the\l"
	.string "unusual weather.\p"
	.string "I'm trying to understand why. Oh?\n"
	.string "A LETTER from my father?$"
GraniteCave_StevensRoom_Text_ThankYouTakeThis:
	.string "You've earned BRAWLY's trust. And you\n"
	.string "came all this way to find me. Thank you.\p"
	.string "Take this MEGA RING. With a matching\n"
	.string "stone, one partner can Mega Evolve\l"
	.string "during a battle.\p"
	.string "Have your partner hold its stone.\n"
	.string "Choose FIGHT, then press START before\l"
	.string "you choose a move.\p"
	.string "A strong bond helps your team\n"
	.string "bring out that power.$"

GraniteCave_StevensRoom_Text_NoStarterStone:
	.string "I don't have a matching Mega\n"
	.string "Stone for your original partners\l"
	.string "here.\p"
	.string "Keep exploring. Other members of\n"
	.string "your team may be able to Mega\l"
	.string "Evolve, too!$"

GraniteCave_StevensRoom_Text_ArchiveOpen:
	.string "The RING lets your partners use\n"
	.string "Mega Stones you find across\l"
	.string "HOENN. Look off the path, and\l"
	.string "talk to people you meet!\p"
	.string "It also opens the CENTER vendors'\n"
	.string "free Form Tools archive.\l"
	.string "Legendary form tools are there,\l"
	.string "too.$"

GraniteCave_StevensRoom_Text_ReturnWithKnuckleBadge:
	.string "My father's survey equipment may\n"
	.string "explain these changes in the sea.\p"
	.string "The manor's old expedition records\n"
	.string "describe similar events. I'd like\l"
	.string "you to help investigate them.\p"
	.string "First challenge BRAWLY in\n"
	.string "DEWFORD's GYM. Then return to me\l"
	.string "here with the KNUCKLE BADGE.\p"
	.string "I'll entrust you with a MEGA RING\n"
	.string "before you explore the manor.$"

GraniteCave_StevensRoom_Text_CouldBecomeChampionLetsRegister:
	.string "A journey teaches you things you\n"
	.string "cannot learn in a laboratory.\p"
	.string "Let's register each other in the\n"
	.string "POKéNAV. I'll keep studying these\l"
	.string "unusual POKéMON sightings.$"

GraniteCave_StevensRoom_Text_RegisteredSteven:
	.string "Registered STEVEN\n"
	.string "in the POKéNAV.$"

GraniteCave_StevensRoom_Text_ManorHandoff:
	.string "Go west from DEWFORD into the\n"
	.string "MEADOW and enter the old MANOR.\p"
	.string "Read the historian's account, then\n"
	.string "speak to the meadow warden with\l"
	.string "a partner that knows SING.\p"
	.string "Wild JIGGLYPUFF live in the manor.\n"
	.string "The CENTER's free move preparation\l"
	.string "can help them remember SING.\p"
	.string "Once you've read the account and\n"
	.string "heard the meadow's answer, take\l"
	.string "STERN's GOODS on to SLATEPORT.$"

GraniteCave_StevensRoom_Text_IveGotToHurryAlong:
	.string "I am heading east. MAGMA and AQUA\n"
	.string "are disturbing more than land\l"
	.string "and sea.\p"
	.string "COBALION lives among wild\n"
	.string "POKéMON on GRANITE CAVE B2F,\l"
	.string "with a 3% encounter chance.\p"
	.string "Read the area signs as you travel.\n"
	.string "They list the local POKéMON and\l"
	.string "any special requirements.$"

GraniteCave_StevensRoom_Text_OhBagIsFull:
	.string "Your BAG is full. Make room and return;\n"
	.string "I will keep the MEGA RING safe.$"

GraniteCave_StevensRoom_Text_CobalionCaught:
	.string "You've already found COBALION!\n"
	.string "It chose a remarkable companion.\p"
	.string "I am heading east to study these\n"
	.string "disturbances. Keep checking the\l"
	.string "route signs as you travel.\p"
	.string "They record your discoveries and\n"
	.string "show which POKéMON remain nearby.$"

GraniteCave_StevensRoom_EventScript_StoneCashReceipt::
	bufferitemname STR_VAR_1, VAR_0x8000
	msgbox GraniteCave_StevensRoom_Text_StoneCash, MSGBOX_DEFAULT
	return

GraniteCave_StevensRoom_Text_StoneCash:
	.string "You already own a {STR_VAR_1}.\n"
	.string "DEVON awards you ¥3,000 instead!$"
