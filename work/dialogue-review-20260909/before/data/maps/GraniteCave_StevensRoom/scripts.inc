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
	call_if_unset FLAG_DELIVERED_DEVON_GOODS, GraniteCave_StevensRoom_EventScript_SternHandoff
	msgbox GraniteCave_StevensRoom_Text_IveGotToHurryAlong, MSGBOX_DEFAULT
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

GraniteCave_StevensRoom_EventScript_SternHandoff::
	msgbox GraniteCave_StevensRoom_Text_SternHandoff, MSGBOX_DEFAULT
	return

GraniteCave_StevensRoom_EventScript_WaitForKnuckleBadge::
	msgbox GraniteCave_StevensRoom_Text_ReturnWithKnuckleBadge, MSGBOX_DEFAULT
	call_if_unset FLAG_HIDE_SLATEPORT_CITY_BRAWLY, GraniteCave_StevensRoom_EventScript_FindBrawlyInSlateport
	release
	end

GraniteCave_StevensRoom_EventScript_FindBrawlyInSlateport::
	msgbox GraniteCave_StevensRoom_Text_FindBrawlyInSlateport, MSGBOX_DEFAULT
	return

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
	.string "Your original partners have no\n"
	.string "applicable Mega Stone here.\p"
	.string "Your other teammates can carry one!$"

GraniteCave_StevensRoom_Text_ArchiveOpen:
	.string "The RING lets your partners use the\n"
	.string "Mega Stones you find across HOENN.\p"
	.string "Look for their glimmer off the path.\n"
	.string "People may have stones to share, too.\p"
	.string "POKéMON CENTER VENDORS still offer\n"
	.string "evolution items free of charge.$"

GraniteCave_StevensRoom_Text_ReturnWithKnuckleBadge:
	.string "The LETTER confirms it: unusual\n"
	.string "weather is driving rare POKéMON\l"
	.string "into new hiding places.\p"
	.string "Challenge BRAWLY at DEWFORD GYM.\n"
	.string "Come back with the KNUCKLE BADGE.$"

GraniteCave_StevensRoom_Text_FindBrawlyInSlateport:
	.string "BRAWLY is visiting SLATEPORT. Look\n"
	.string "outside the OCEANIC MUSEUM for him.\p"
	.string "You've delivered my LETTER, so ask\n"
	.string "MR. BRINEY at the dock to sail there.\p"
	.string "Find BRAWLY, then return to DEWFORD.\n"
	.string "I'll wait here for your KNUCKLE BADGE.$"

GraniteCave_StevensRoom_Text_CouldBecomeChampionLetsRegister:
	.string "A journey teaches you things you\n"
	.string "cannot learn in a laboratory.\p"
	.string "Let's register each other in the\n"
	.string "POKéNAV. I'll keep studying these\l"
	.string "unusual POKéMON sightings.$"

GraniteCave_StevensRoom_Text_RegisteredSteven:
	.string "Registered STEVEN\n"
	.string "in the POKéNAV.$"

GraniteCave_StevensRoom_Text_SternHandoff:
	.string "Now take DEVON's GOODS to CAPT. STERN\n"
	.string "in SLATEPORT CITY.\p"
	.string "Ask MR. BRINEY at DEWFORD's dock\n"
	.string "to sail you there.\p"
	.string "The SHIPYARD staff in SLATEPORT can\n"
	.string "tell you where to find the captain.$"

GraniteCave_StevensRoom_Text_IveGotToHurryAlong:
	.string "I'm heading east. MAGMA and AQUA\n"
	.string "are disturbing more than land and\l"
	.string "sea.\p"
	.string "With the STONE BADGE, COBALION\n"
	.string "can appear among wild POKéMON on\l"
	.string "GRANITE CAVE B2F.\p"
	.string "It has a 1% chance. SWEET SCENT\n"
	.string "increases that to 25%.\p"
	.string "DEVON's researcher lists the\n"
	.string "other legendary locations and\l"
	.string "requirements.$"

GraniteCave_StevensRoom_Text_OhBagIsFull:
	.string "Your BAG is full. Make room and return;\n"
	.string "I will keep the MEGA RING safe.$"
