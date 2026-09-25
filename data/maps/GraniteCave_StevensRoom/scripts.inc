.set LOCALID_STEVEN, 1

GraniteCave_StevensRoom_MapScripts:: @ 822DC7A
	.byte 0

GraniteCave_StevensRoom_EventScript_Steven:: @ 822DC7B
	lock
	faceplayer
	goto_if_set FLAG_DELIVERED_STEVEN_LETTER, GraniteCave_StevensRoom_EventScript_GiveReward
	msgbox GraniteCave_StevensRoom_Text_ImStevenLetterForMe, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_LETTER
	call Common_EventScript_PlayerHandedOverTheItem
	setflag FLAG_DELIVERED_STEVEN_LETTER
GraniteCave_StevensRoom_EventScript_GiveReward::
	msgbox GraniteCave_StevensRoom_Text_ThankYouTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	msgbox GraniteCave_StevensRoom_Text_CouldBecomeChampionLetsRegister, MSGBOX_DEFAULT
	closemessage
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
	walk_in_place_fast_up
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
	.string "Steven: Thank you for delivering\n"
	.string "the letter.\p"
	.string "Please take this Rare Candy.\p"
	.string "If you'd like to learn my favorite\n"
	.string "move, Steel Wing, ask the Move Tutor.$"

GraniteCave_StevensRoom_Text_CouldBecomeChampionLetsRegister: @ 822DE6B
	.string "Steven: Your Pokémon appear quite\n"
	.string "capable.\p"
	.string "If you keep training, you could even\n"
	.string "become the Champion of the Pokémon\l"
	.string "League one day. That's what I think.$"

GraniteCave_StevensRoom_Text_IveGotToHurryAlong: @ 822DF8C
	.string "Now, I've got to hurry along.$"
