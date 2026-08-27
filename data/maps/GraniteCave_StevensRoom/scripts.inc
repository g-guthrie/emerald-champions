.set LOCALID_STEVEN, 1

GraniteCave_StevensRoom_MapScripts:: @ 822DC7A
	.byte 0

GraniteCave_StevensRoom_EventScript_Steven:: @ 822DC7B
	lock
	faceplayer
	goto_if_set FLAG_DELIVERED_STEVEN_LETTER, GraniteCave_StevensRoom_EventScript_TryGiveRewards
	msgbox GraniteCave_StevensRoom_Text_ImStevenLetterForMe, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_LETTER
	call Common_EventScript_PlayerHandedOverTheItem
	setflag FLAG_DELIVERED_STEVEN_LETTER

GraniteCave_StevensRoom_EventScript_TryGiveRewards:
	goto_if_set FLAG_SYS_RECEIVED_KEYSTONE, GraniteCave_StevensRoom_EventScript_RegisterSteven
	msgbox GraniteCave_StevensRoom_Text_ThankYouTakeThis, MSGBOX_DEFAULT
	special TryGiveVerdantStevenRewardBundle
	compare VAR_RESULT, FALSE
	goto_if_eq GraniteCave_StevensRoom_EventScript_BagFull
	setflag FLAG_SYS_RECEIVED_KEYSTONE
	playfanfare MUS_OBTAIN_ITEM
	message GraniteCave_StevensRoom_Text_ReceivedRewards
	waitmessage
	waitfanfare
	msgbox GraniteCave_StevensRoom_Text_MegaBracelet, MSGBOX_DEFAULT
	msgbox GraniteCave_StevensRoom_Text_StarterMegaStones, MSGBOX_DEFAULT
GraniteCave_StevensRoom_EventScript_RegisterSteven:
	msgbox GraniteCave_StevensRoom_Text_CouldBecomeChampionLetsRegister, MSGBOX_DEFAULT
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox GraniteCave_StevensRoom_Text_RegisteredSteven, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_REGISTERED_STEVEN_POKENAV
	msgbox GraniteCave_StevensRoom_Text_IveGotToHurryAlong, MSGBOX_DEFAULT
	closemessage
	compare VAR_FACING, DIR_NORTH
	call_if_eq GraniteCave_StevensRoom_EventScript_StevenExitNorth
	compare VAR_FACING, DIR_SOUTH
	call_if_eq GraniteCave_StevensRoom_EventScript_StevenExitSouth
	compare VAR_FACING, DIR_WEST
	call_if_eq GraniteCave_StevensRoom_EventScript_StevenExitWestEast
	compare VAR_FACING, DIR_EAST
	call_if_eq GraniteCave_StevensRoom_EventScript_StevenExitWestEast
	playse SE_EXIT
	removeobject LOCALID_STEVEN
	release
	end

GraniteCave_StevensRoom_EventScript_StevenExitNorth:: @ 822DD0D
	applymovement LOCALID_STEVEN, GraniteCave_StevensRoom_Movement_StevenExit
	waitmovement 0
	return

GraniteCave_StevensRoom_EventScript_StevenExitWestEast:: @ 822DD18
	applymovement OBJ_EVENT_ID_PLAYER, GraniteCave_StevensRoom_Movement_PlayerTurnTowardExit
	applymovement LOCALID_STEVEN, GraniteCave_StevensRoom_Movement_StevenExit
	waitmovement 0
	return

GraniteCave_StevensRoom_EventScript_StevenExitSouth:: @ 822DD2A
	applymovement OBJ_EVENT_ID_PLAYER, GraniteCave_StevensRoom_Movement_PlayerTurnTowardExit
	applymovement LOCALID_STEVEN, GraniteCave_StevensRoom_Movement_StevenExitSouth
	waitmovement 0
	return

GraniteCave_StevensRoom_EventScript_BagFull:: @ 822DD3C
	msgbox GraniteCave_StevensRoom_Text_OhBagIsFull, MSGBOX_DEFAULT
	release
	end

GraniteCave_StevensRoom_Movement_StevenExit: @ 822DD45
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	delay_8
	step_end

GraniteCave_StevensRoom_Movement_PlayerTurnTowardExit: @ 822DD4C
	delay_16
	delay_16
	delay_16
	walk_in_place_fastest_up
	step_end

GraniteCave_StevensRoom_Movement_StevenExitSouth: @ 822DD51
	walk_left
	walk_up
	walk_up
	walk_up
	walk_right
	walk_up
	walk_up
	delay_8
	step_end

GraniteCave_StevensRoom_Text_ImStevenLetterForMe: @ 822DD5A
	.string "My name is Steven.\p"
	.string "I'm interested in rare stones,\n"
	.string "so I travel here and there.\p"
	.string "Oh?\n"
	.string "A Letter for me?$"

GraniteCave_StevensRoom_Text_ThankYouTakeThis: @ 822DDBD
	.string "Steven: Okay, thank you.\p"
	.string "You went through all this trouble to\n"
	.string "deliver that. I need to thank you.\p"
	.string "Let me see…\n"
	.string "I'll give you this Metal Coat.\p"
	.string "Some Pokémon evolve by leveling up\n"
	.string "while holding it.$"

GraniteCave_StevensRoom_Text_ReceivedRewards:
	.string "{PLAYER} received a Metal Coat,\n"
	.string "a Mega Bracelet, and three\l"
	.string "Mega Stones from Steven!$"

GraniteCave_StevensRoom_Text_MegaBracelet:
	.string "Steven: There's something else.\p"
	.string "Rare stones react to the bond between\n"
	.string "a Trainer and their Pokémon.\p"
	.string "This Mega Bracelet lets you draw out\n"
	.string "that power during battle.$"

GraniteCave_StevensRoom_Text_StarterMegaStones:
	.string "Let a compatible Pokémon hold its Mega\n"
	.string "Stone, then press START before choosing\l"
	.string "a move.\p"
	.string "These three stones belong with Hoenn's\n"
	.string "partner Pokémon. Use them well.$"

GraniteCave_StevensRoom_Text_CouldBecomeChampionLetsRegister: @ 822DE6B
	.string "Steven: Your Pokémon appear quite\n"
	.string "capable.\p"
	.string "If you keep training, you could even\n"
	.string "become the Champion of the Pokémon\l"
	.string "League one day. That's what I think.\p"
	.string "I know, since we've gotten to know each\n"
	.string "other, let's register one another in\l"
	.string "our PokéNavs.\p"
	.string "… … … … … …$"

GraniteCave_StevensRoom_Text_RegisteredSteven: @ 822DF6A
	.string "Registered Steven\n"
	.string "in the PokéNav.$"

GraniteCave_StevensRoom_Text_IveGotToHurryAlong: @ 822DF8C
	.string "Now, I've got to hurry along.$"

GraniteCave_StevensRoom_Text_OhBagIsFull: @ 822DFAA
	.string "Steven: Your Bag doesn't have room\n"
	.string "for the full set. Make some space,\l"
	.string "then speak with me again.$"
