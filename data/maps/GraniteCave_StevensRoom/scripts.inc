GraniteCave_StevensRoom_MapScripts::
	.byte 0

GraniteCave_StevensRoom_EventScript_Steven::
	lock
	faceplayer
	goto_if_set FLAG_DELIVERED_STEVEN_LETTER, GraniteCave_StevensRoom_EventScript_CheckKnuckleBadge
	msgbox GraniteCave_StevensRoom_Text_ImStevenLetterForMe, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_LETTER
	call Common_EventScript_PlayerHandedOverTheItem
	setflag FLAG_DELIVERED_STEVEN_LETTER

GraniteCave_StevensRoom_EventScript_CheckKnuckleBadge::
	goto_if_unset FLAG_BADGE02_GET, GraniteCave_StevensRoom_EventScript_WaitForKnuckleBadge
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, TRUE, GraniteCave_StevensRoom_EventScript_RegisterSteven
	msgbox GraniteCave_StevensRoom_Text_ThankYouTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_MEGA_RING
	goto_if_eq VAR_RESULT, FALSE, GraniteCave_StevensRoom_EventScript_BagFull
	msgbox GraniteCave_StevensRoom_Text_StarterStones, MSGBOX_DEFAULT
	giveitem ITEM_SCEPTILITE
	giveitem ITEM_BLAZIKENITE
	giveitem ITEM_SWAMPERTITE
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
	.string "I'm STEVEN. These markings appeared\n"
	.string "when the bedrock began to resonate.\p"
	.string "They are older than DEVON and shaped\n"
	.string "like no natural mineral vein.\p"
	.string "Oh? A LETTER from my father?$"
GraniteCave_StevensRoom_Text_ThankYouTakeThis:
	.string "You earned BRAWLY's trust and crossed\n"
	.string "this cave without turning back.\p"
	.string "Take this MEGA RING. A matching stone\n"
	.string "lets one partner Mega Evolve in battle.\p"
	.string "The RING and those CHAMPION'S SIGNS\n"
	.string "answer to the same bond.$"

GraniteCave_StevensRoom_Text_StarterStones:
	.string "Your starter should be first. Take\n"
	.string "all three of the HOENN starter stones.$"

GraniteCave_StevensRoom_Text_ArchiveOpen:
	.string "And now that you hold a RING, every\n"
	.string "POKéMON CENTER'S VENDOR will open\l"
	.string "the full Mega Stone archive to you.\p"
	.string "Every stone, free. Build what you like.$"

GraniteCave_StevensRoom_Text_ReturnWithKnuckleBadge:
	.string "The LETTER confirms what I feared:\n"
	.string "these marks are shining across HOENN.\p"
	.string "First, challenge BRAWLY. Return with the\n"
	.string "KNUCKLE BADGE and a proven partnership.$"

GraniteCave_StevensRoom_Text_CouldBecomeChampionLetsRegister:
	.string "Your team does not need one perfect\n"
	.string "answer. It needs answers for anything.\p"
	.string "Register me in your POKéNAV. If the\n"
	.string "stone network changes, I will call.$"

GraniteCave_StevensRoom_Text_RegisteredSteven:
	.string "Registered STEVEN\n"
	.string "in the POKéNAV.$"

GraniteCave_StevensRoom_Text_IveGotToHurryAlong:
	.string "I am heading east. MAGMA and AQUA are\n"
	.string "disturbing more than land and sea.\p"
	.string "One lower-cave mark sketches LUCARIO.\n"
	.string "Bring that family back to the bedrock.\p"
	.string "A local SIGN can awaken by itself;\n"
	.string "DEVON is an oracle, not a requirement.$"

GraniteCave_StevensRoom_Text_OhBagIsFull:
	.string "Your BAG is full. Make room and return;\n"
	.string "I will keep the MEGA RING safe.$"
