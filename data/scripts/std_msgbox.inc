Std_MsgboxNPC:
	lock
	faceplayer
	message 0x0
	waitmessage
	waitbuttonpress
	release
	return

Std_MsgboxSign:
	lockall
	message 0x0
	waitmessage
	waitbuttonpress
	releaseall
	return

Std_MsgboxDefault:
	message 0x0
	waitmessage
	waitbuttonpress
	return

Std_MsgboxYesNo:
	message 0x0
	waitmessage
	yesnobox 20, 8
	return

Std_MsgboxGetPoints:
	message 0x0
	playfanfare MUS_OBTAIN_B_POINTS
	waitfanfare
	waitmessage
	return

Common_EventScript_SaveGame::
	special SaveGame
	return

@ Emerald Champions: restored after the Inclement merge overwrote this file;
@ these are referenced from this engine's own C and scripts.

Std_ReceivedItem::
	textcolor NPC_TEXT_COLOR_NEUTRAL
	call_if_eq VAR_0x8002, MUS_LEVEL_UP, EventScript_ReceivedItemFanfare1
	call_if_eq VAR_0x8002, MUS_RG_OBTAIN_KEY_ITEM, EventScript_ReceivedItemFanfare2
	message NULL
	waitmessage
	waitfanfare
	call_if_eq VAR_0x8002, MUS_LEVEL_UP, EventScript_ReceivedItemWaitFanfare
	putitemaway VAR_0x8000, VAR_0x8001
	call EventScript_RestorePrevTextColor
	return

Std_MsgboxPokenav:
	pokenavcall NULL
	waitmessage
	return

@ Emerald Champions: restored from this repo's own pre-merge version; the Inclement
@ merge replaced this file and these definitions were lost while still referenced.

EventScript_ReceivedItemFanfare1::
	playfanfare MUS_LEVEL_UP
	return

EventScript_ReceivedItemFanfare2::
	playfanfare MUS_RG_OBTAIN_KEY_ITEM
	return

EventScript_ReceivedItemWaitFanfare::
	delay 50
	return
