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
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, TRUE, GraniteCave_StevensRoom_EventScript_StarterGifts
	msgbox GraniteCave_StevensRoom_Text_ThankYouTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_MEGA_RING
	goto_if_eq VAR_RESULT, FALSE, GraniteCave_StevensRoom_EventScript_BagFull
GraniteCave_StevensRoom_EventScript_StarterGifts::
	special GetEmeraldChampionsStarterMegaStone
	goto_if_eq VAR_RESULT, FALSE, GraniteCave_StevensRoom_EventScript_NoStarterStone
	special GiveEmeraldChampionsStarterMegaStones
	copyvar VAR_0x8008, VAR_RESULT
	call_if_ne VAR_0x8009, 0, GraniteCave_StevensRoom_EventScript_FirstStoneReceipt
	call_if_ne VAR_0x800A, 0, GraniteCave_StevensRoom_EventScript_SecondStoneReceipt
	goto_if_eq VAR_0x8008, FALSE, GraniteCave_StevensRoom_EventScript_StoneStorageFull
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

GraniteCave_StevensRoom_EventScript_FirstStoneReceipt::
	copyvar VAR_0x8000, VAR_0x8004
	copyvar VAR_0x800B, VAR_0x8009
	goto GraniteCave_StevensRoom_EventScript_StoneReceipt

GraniteCave_StevensRoom_EventScript_SecondStoneReceipt::
	copyvar VAR_0x8000, VAR_0x8005
	copyvar VAR_0x800B, VAR_0x800A

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
	.string "Your MEGA RING still opens the archive\n"
	.string "at every POKéMON CENTER VENDOR.$"

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
	.string "I'm STEVEN. Forgive me, I didn't hear\n"
	.string "you come in. Look at these markings.\p"
	.string "They began to shine when the bedrock\n"
	.string "started resonating. But the marks\l"
	.string "themselves are much older than DEVON.\p"
	.string "No mineral vein forms a pattern like\n"
	.string "this… Oh? A LETTER from my father?$"
GraniteCave_StevensRoom_Text_ThankYouTakeThis:
	.string "You've earned BRAWLY's trust. And you\n"
	.string "came all this way to find me. Thank you.\p"
	.string "Take this MEGA RING. With a matching\n"
	.string "stone, one partner can Mega Evolve\l"
	.string "during a battle.\p"
	.string "Have your partner hold its stone.\n"
	.string "Choose FIGHT, then press START before\l"
	.string "you choose a move.\p"
	.string "The RING and the CHAMPION'S SIGNS\n"
	.string "respond to that same bond.$"

GraniteCave_StevensRoom_Text_NoStarterStone:
	.string "Your starter's line has no Mega Stone,\n"
	.string "so let your other partners carry one.$"

GraniteCave_StevensRoom_Text_ArchiveOpen:
	.string "With that RING, each POKéMON CENTER\n"
	.string "VENDOR opens the Mega Stone archive.\p"
	.string "Every stone is available free of charge.\n"
	.string "Find the ones that suit your partners.$"

GraniteCave_StevensRoom_Text_ReturnWithKnuckleBadge:
	.string "The LETTER confirms what I feared:\n"
	.string "these marks are shining across HOENN.\p"
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
	.string "There are things a stone can tell me,\n"
	.string "and things only a journey can teach.\p"
	.string "Let's register each other in the\n"
	.string "POKéNAV. If these signs change,\l"
	.string "I'll let you know.$"

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
	.string "I'm heading east. MAGMA and AQUA are\n"
	.string "disturbing more than land and sea.\p"
	.string "A mark in the lower cave resembles\n"
	.string "LUCARIO. Take a member of that family\l"
	.string "to DEVON's researcher on the 2nd floor.\p"
	.string "Each SIGN needs its own translation\n"
	.string "before you return to seek its Pokémon.$"

GraniteCave_StevensRoom_Text_OhBagIsFull:
	.string "Your BAG is full. Make room and return;\n"
	.string "I will keep the MEGA RING safe.$"
