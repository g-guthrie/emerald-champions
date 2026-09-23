Route113_GlassWorkshop_MapScripts:: @ 826ED03
	map_script MAP_SCRIPT_ON_TRANSITION, Route113_GlassWorkshop_OnTransition
	.byte 0

	.set BLUE_FLUTE_PRICE,   250
	.set YELLOW_FLUTE_PRICE, 500
	.set RED_FLUTE_PRICE,    500
	.set WHITE_FLUTE_PRICE,  1000
	.set BLACK_FLUTE_PRICE,  1000
	.set PRETTY_CHAIR_PRICE, 6000
	.set PRETTY_DESK_PRICE,  8000
	.set LOWEST_ASH_PRICE,   BLUE_FLUTE_PRICE

Route113_GlassWorkshop_OnTransition: @ 826ED09
	setflag FLAG_LANDMARK_GLASS_WORKSHOP
	compare VAR_GLASS_WORKSHOP_STATE, 1
	call_if_eq Route113_GlassWorkshop_EventScript_ReenterWorkshopAfterSootSack
	end

Route113_GlassWorkshop_EventScript_ReenterWorkshopAfterSootSack:: @ 826ED18
	setvar VAR_GLASS_WORKSHOP_STATE, 2
	return

Route113_GlassWorkshop_EventScript_GlassWorker:: @ 826ED1E
	lock
	faceplayer
	setvar VAR_0x8004, LEGENDARY_SIGN_MARSHADOW
	special TryUnlockLocalLegendaryDiscovery
	goto_if_eq VAR_RESULT, TRUE, Route113_GlassWorkshop_EventScript_FoundMarshadow
	compare VAR_GLASS_WORKSHOP_STATE, 10
	goto_if_ge Route113_GlassWorkshop_EventScript_GiveItemAfterNoRoom
	compare VAR_GLASS_WORKSHOP_STATE, 2
	goto_if_eq Route113_GlassWorkshop_EventScript_CheckCollectedAsh
	compare VAR_GLASS_WORKSHOP_STATE, 1
	goto_if_eq Route113_GlassWorkshop_EventScript_ExplainSootSack
	msgbox Route113_GlassWorkshop_Text_GoCollectAshWithThis, MSGBOX_DEFAULT
	giveitem ITEM_SOOT_SACK
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setvar VAR_GLASS_WORKSHOP_STATE, 1
	msgbox Route113_GlassWorkshop_Text_ExplainSootSack, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_ExplainSootSack:: @ 826ED64
	msgbox Route113_GlassWorkshop_Text_ExplainSootSack, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_CheckCollectedAsh:: @ 826ED6E
	checkitem ITEM_SOOT_SACK, 1
	compare VAR_RESULT, FALSE
	goto_if_eq Route113_GlassWorkshop_EventScript_SootSackNotInBag
	msgbox Route113_GlassWorkshop_Text_LetsSeeCollectedAshes, MSGBOX_DEFAULT
	compare VAR_ASH_GATHER_COUNT, LOWEST_ASH_PRICE
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAsh
	message Route113_GlassWorkshop_Text_WhichGlassItemWoudYouLike
	waitmessage
	goto Route113_GlassWorkshop_EventScript_ChooseGlassItem
	end

Route113_GlassWorkshop_EventScript_SootSackNotInBag:: @ 826ED9D
	msgbox Route113_GlassWorkshop_Text_HaventGotYourSootSack, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_ChooseGlassItem:: @ 826EDA7
	setvar VAR_0x8009, 0
	setvar VAR_0x8004, SCROLL_MULTI_GLASS_WORKSHOP_VENDOR
	special ShowScrollableMultichoice
	switch VAR_RESULT
	case 0, Route113_GlassWorkshop_EventScript_BlueFlute
	case 1, Route113_GlassWorkshop_EventScript_YellowFlute
	case 2, Route113_GlassWorkshop_EventScript_RedFlute
	case 3, Route113_GlassWorkshop_EventScript_WhiteFlute
	case 4, Route113_GlassWorkshop_EventScript_BlackFlute
	case 5, Route113_GlassWorkshop_EventScript_PrettyChair
	case 6, Route113_GlassWorkshop_EventScript_PrettyDesk
	case 7, Route113_GlassWorkshop_EventScript_CancelGlassItemSelect
	case MULTI_B_PRESSED, Route113_GlassWorkshop_EventScript_CancelGlassItemSelect
	end

Route113_GlassWorkshop_EventScript_BlueFlute::
	setvar VAR_0x8008, ITEM_BLUE_FLUTE
	setvar VAR_0x800A, BLUE_FLUTE_PRICE
	setvar VAR_0x800B, 10
	goto Route113_GlassWorkshop_EventScript_PrepareFlute

Route113_GlassWorkshop_EventScript_YellowFlute::
	setvar VAR_0x8008, ITEM_YELLOW_FLUTE
	setvar VAR_0x800A, YELLOW_FLUTE_PRICE
	setvar VAR_0x800B, 11
	goto Route113_GlassWorkshop_EventScript_PrepareFlute

Route113_GlassWorkshop_EventScript_RedFlute::
	setvar VAR_0x8008, ITEM_RED_FLUTE
	setvar VAR_0x800A, RED_FLUTE_PRICE
	setvar VAR_0x800B, 12
	goto Route113_GlassWorkshop_EventScript_PrepareFlute

Route113_GlassWorkshop_EventScript_WhiteFlute::
	setvar VAR_0x8008, ITEM_WHITE_FLUTE
	setvar VAR_0x800A, WHITE_FLUTE_PRICE
	setvar VAR_0x800B, 13
	goto Route113_GlassWorkshop_EventScript_PrepareFlute

Route113_GlassWorkshop_EventScript_BlackFlute::
	setvar VAR_0x8008, ITEM_BLACK_FLUTE
	setvar VAR_0x800A, BLACK_FLUTE_PRICE
	setvar VAR_0x800B, 14
	goto Route113_GlassWorkshop_EventScript_PrepareFlute

Route113_GlassWorkshop_EventScript_PrettyChair::
	setvar VAR_0x8008, DECOR_PRETTY_CHAIR
	setvar VAR_0x800A, PRETTY_CHAIR_PRICE
	setvar VAR_0x800B, 15
	goto Route113_GlassWorkshop_EventScript_PrepareDecoration

Route113_GlassWorkshop_EventScript_PrettyDesk::
	setvar VAR_0x8008, DECOR_PRETTY_DESK
	setvar VAR_0x800A, PRETTY_DESK_PRICE
	setvar VAR_0x800B, 16
	goto Route113_GlassWorkshop_EventScript_PrepareDecoration

Route113_GlassWorkshop_EventScript_PrepareFlute::
	setvar VAR_0x8009, 0
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_CheckPrice

Route113_GlassWorkshop_EventScript_PrepareDecoration::
	setvar VAR_0x8009, 1
	bufferdecorationname 0, VAR_0x8008
Route113_GlassWorkshop_EventScript_CheckPrice::
	compare VAR_ASH_GATHER_COUNT, VAR_0x800A
	goto_if_lt Route113_GlassWorkshop_EventScript_NotEnoughAshForItem
Route113_GlassWorkshop_EventScript_ConfirmGlassItem::
	msgbox Route113_GlassWorkshop_Text_IsThatTheItemForYou, MSGBOX_YESNO
Route113_GlassWorkshop_EventScript_ApplyGlassOrder::
	goto_if_eq VAR_RESULT, NO, Route113_GlassWorkshop_EventScript_ChooseDifferentItem
	copyvar VAR_GLASS_WORKSHOP_STATE, VAR_0x800B
	subvar VAR_ASH_GATHER_COUNT, VAR_0x800A
	goto Route113_GlassWorkshop_EventScript_MakeGlassItem

Route113_GlassWorkshop_EventScript_CancelGlassItemSelect:: @ 826EFCC
	msgbox Route113_GlassWorkshop_Text_AllThatAshButDontWantAnything, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_NotEnoughAsh:: @ 826EFD6
	setvar VAR_0x800A, LOWEST_ASH_PRICE
	subvar VAR_0x800A, VAR_ASH_GATHER_COUNT
	buffernumberstring 0, VAR_0x800A
	msgbox Route113_GlassWorkshop_Text_NotEnoughAshNeedX, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_NotEnoughAshForItem:: @ 826EFEE
	subvar VAR_0x800A, VAR_ASH_GATHER_COUNT
	buffernumberstring 1, VAR_0x800A
	message Route113_GlassWorkshop_Text_NotEnoughAshToMakeItem
	waitmessage
	goto Route113_GlassWorkshop_EventScript_ChooseGlassItem
	end

Route113_GlassWorkshop_EventScript_ChooseDifferentItem:: @ 826F003
	message Route113_GlassWorkshop_Text_WhichWouldYouLike
	waitmessage
	goto Route113_GlassWorkshop_EventScript_ChooseGlassItem
	end

Route113_GlassWorkshop_EventScript_MakeGlassItem:: @ 826F00F
	msgbox Route113_GlassWorkshop_Text_IllMakeItemForYou, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	playse SE_SELECT
	delay 30
	fadescreen FADE_FROM_BLACK
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain

Route113_GlassWorkshop_EventScript_GiveGlassFlute:: @ 826F047
	giveitem VAR_0x8008
	compare VAR_RESULT, FALSE
	goto_if_eq Route113_GlassWorkshop_EventScript_NoRoomForFlute
	return

Route113_GlassWorkshop_EventScript_GiveGlassDecor:: @ 826F05F
	givedecoration VAR_0x8008
	compare VAR_RESULT, FALSE
	goto_if_eq Route113_GlassWorkshop_EventScript_NoRoomForDecor
	return

Route113_GlassWorkshop_EventScript_NoRoomForFlute:: @ 826F072
	call Common_EventScript_BagIsFull
	msgbox Route113_GlassWorkshop_Text_NoRoomInBag, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_NoRoomForDecor:: @ 826F081
	call Common_EventScript_NoRoomForDecor
	msgbox Route113_GlassWorkshop_Text_NoRoomInPC, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_EventScript_GiveItemAfterNoRoom:: @ 826F090
	switch VAR_GLASS_WORKSHOP_STATE
	case 10, Route113_GlassWorkshop_EventScript_GiveBlueFlute
	case 11, Route113_GlassWorkshop_EventScript_GiveYellowFlute
	case 12, Route113_GlassWorkshop_EventScript_GiveRedFlute
	case 13, Route113_GlassWorkshop_EventScript_GiveWhiteFlute
	case 14, Route113_GlassWorkshop_EventScript_GiveBlackFlute
	case 15, Route113_GlassWorkshop_EventScript_GivePrettyChair
	case 16, Route113_GlassWorkshop_EventScript_GivePrettyDesk
	end

Route113_GlassWorkshop_EventScript_GiveBlueFlute:: @ 826F0E3
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_BLUE_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GiveYellowFlute:: @ 826F0F7
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_YELLOW_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GiveRedFlute:: @ 826F10B
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_RED_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GiveWhiteFlute:: @ 826F11F
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_WHITE_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GiveBlackFlute:: @ 826F133
	setvar VAR_0x8009, 0
	setvar VAR_0x8008, ITEM_BLACK_FLUTE
	bufferitemname 0, VAR_0x8008
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GivePrettyChair:: @ 826F147
	setvar VAR_0x8009, 1
	setvar VAR_0x8008, DECOR_PRETTY_CHAIR
	bufferdecorationname 0, DECOR_PRETTY_CHAIR
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_GivePrettyDesk:: @ 826F15B
	setvar VAR_0x8009, 1
	setvar VAR_0x8008, DECOR_PRETTY_DESK
	bufferdecorationname 0, DECOR_PRETTY_DESK
	goto Route113_GlassWorkshop_EventScript_TryGiveItemAgain
	end

Route113_GlassWorkshop_EventScript_TryGiveItemAgain:: @ 826F16F
	msgbox Route113_GlassWorkshop_Text_IveFinishedGlassItem, MSGBOX_DEFAULT
	compare VAR_0x8009, 0
	call_if_eq Route113_GlassWorkshop_EventScript_GiveGlassFlute
	compare VAR_0x8009, 1
	call_if_eq Route113_GlassWorkshop_EventScript_GiveGlassDecor
	setvar VAR_GLASS_WORKSHOP_STATE, 2
	release
	end

Route113_GlassWorkshop_EventScript_NinjaBoy:: @ 826F194
	msgbox Route113_GlassWorkshop_Text_FunToBlowGlassFlute, MSGBOX_NPC
	end

Route113_GlassWorkshop_Text_GoCollectAshWithThis: @ 826F19D
	.string "This area is covered in volcanic ash,\n"
	.string "huff-puff!\p"
	.string "I'm specially gifted, huff-puff.\p"
	.string "I make glass out of volcanic ash\n"
	.string "and make items, huff-puff.\p"
	.string "Go collect ashes with this, huff-puff.$"

Route113_GlassWorkshop_Text_ExplainSootSack: @ 826F252
	.string "Just take that Soot Sack and walk\n"
	.string "through piles of ash, huff-puff.\p"
	.string "And it will fill up with the volcanic ash,\n"
	.string "huff-puff.\p"
	.string "Once you think you've collected a good\n"
	.string "amount, come see me, huff-puff.$"

Route113_GlassWorkshop_Text_LetsSeeCollectedAshes: @ 826F312
	.string "Have you collected ashes, huff-puff?\n"
	.string "Let me see, huff-puff.$"

Route113_GlassWorkshop_Text_NotEnoughAshNeedX: @ 826F34E
	.string "Hmmm… Not enough ash yet,\n"
	.string "huff-puff.\p"
	.string "Walk {STR_VAR_1} more steps through\n"
	.string "ash for a Blue Flute, huff-puff.$"

Route113_GlassWorkshop_Text_WhichGlassItemWoudYouLike: @ 826F40A
	.string "Oh!\n"
	.string "You've got a lot of ashes, huff-puff!\p"
	.string "I'll make you a glass item, huff-puff!\n"
	.string "Which one would you like, huff-puff?$"

Route113_GlassWorkshop_Text_IsThatTheItemForYou: @ 826F480
	.string "A {STR_VAR_1}, huff-puff?\n"
	.string "Is that the one for you, huff-puff?$"

Route113_GlassWorkshop_Text_WhichWouldYouLike: @ 826F4B5
	.string "Which one would you like, huff-puff?$"

Route113_GlassWorkshop_Text_IllMakeItemForYou: @ 826F4DA
	.string "A {STR_VAR_1} it is, then, huff-puff!\p"
	.string "Okay! I'll make it for you, huff-puff.\n"
	.string "Just wait a little while, huff-puff.$"

Route113_GlassWorkshop_Text_NotEnoughAshToMakeItem: @ 826F543
	.string "A {STR_VAR_1}, huff-puff?\p"
	.string "Not enough ash for that,\n"
	.string "huff-puff.\p"
	.string "Walk {STR_VAR_2} more steps through\n"
	.string "ash so I can make it.\p"
	.string "Which item should I make\n"
	.string "instead, huff-puff?$"

Route113_GlassWorkshop_Text_AllThatAshButDontWantAnything: @ 826F641
	.string "You've collected all that ash, but you\n"
	.string "don't want anything, huff-puff?$"

Route113_GlassWorkshop_Text_IveFinishedGlassItem: @ 826F688
	.string "Ah, I've finished your {STR_VAR_1}.\n"
	.string "Take it, huff-puff.$"

Route113_GlassWorkshop_Text_NoRoomInBag: @ 826F6B7
	.string "Oh?\n"
	.string "You've no room in your Bag, huff-puff.\p"
	.string "I'll hold on to it, so come back later,\n"
	.string "huff-puff.$"

Route113_GlassWorkshop_Text_NoRoomInPC: @ 826F715
	.string "Oh?\n"
	.string "You've no room in your PC, huff-puff?\p"
	.string "I'll hold on to it, so come back later,\n"
	.string "huff-puff.$"

Route113_GlassWorkshop_Text_HaventGotYourSootSack: @ 826F772
	.string "Hah? You haven't got your Soot Sack\n"
	.string "with you, huff-puff.\p"
	.string "You have to keep it with you to collect\n"
	.string "volcanic ash, huff-puff.$"

Route113_GlassWorkshop_Text_FunToBlowGlassFlute: @ 826F7EC
	.string "It's fun to blow a glass flute while\n"
	.string "my boss is talking.\p"
	.string "Huff-huff! Puff-puff!$"


Route113_GlassWorkshop_EventScript_FoundMarshadow::
	msgbox Route113_GlassWorkshop_Text_FoundMarshadow, MSGBOX_DEFAULT
	release
	end

Route113_GlassWorkshop_Text_FoundMarshadow:
	.string "You've gathered 250 soot, huff-puff!\p"
	.string "A shy Marshadow has taken notice.\n"
	.string "Look for it in Route 113's ash grass!\p"
	.string "Your soot total is never spent on\n"
	.string "this discovery, huff-puff.$"
