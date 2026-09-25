MauvilleCity_House2_MapScripts:: @ 8210C5C
	.byte 0

MauvilleCity_House2_EventScript_Woman:: @ 8210C5D
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_COIN_CASE, MauvilleCity_House2_EventScript_ReceivedCoinCase
	msgbox MauvilleCity_House2_Text_BuyIceStoneAtSlateport, MSGBOX_DEFAULT
	checkitem ITEM_ICE_STONE, 1
	compare VAR_RESULT, TRUE
	goto_if_eq MauvilleCity_House2_EventScript_AskToTradeForIceStone
	release
	end

MauvilleCity_House2_EventScript_AskToTradeForIceStone:: @ 8210C82
	playse SE_PIN
	applymovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement VAR_LAST_TALKED, Common_Movement_Delay48
	waitmovement 0
	msgbox MauvilleCity_House2_Text_TradeIceStoneForCoinCase, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq MauvilleCity_House2_EventScript_AcceptTrade
	compare VAR_RESULT, NO
	goto_if_eq MauvilleCity_House2_EventScript_DeclineTrade
	end

MauvilleCity_House2_EventScript_AcceptTrade:: @ 8210CB8
	msgbox MauvilleCity_House2_Text_IllTradeYouCoinCase, MSGBOX_DEFAULT
	giveitem ITEM_COIN_CASE
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	removeitem ITEM_ICE_STONE
	setflag FLAG_RECEIVED_COIN_CASE
	goto MauvilleCity_House2_EventScript_ReceivedCoinCase
	end

MauvilleCity_House2_EventScript_ReceivedCoinCase:: @ 8210CDA
	msgbox MauvilleCity_House2_Text_UseCoinCaseAtGameCorner, MSGBOX_DEFAULT
	release
	end

MauvilleCity_House2_EventScript_DeclineTrade:: @ 8210CE4
	msgbox MauvilleCity_House2_Text_ThatsDisappointing, MSGBOX_DEFAULT
	release
	end

MauvilleCity_House2_EventScript_Tutor::
	lock
	faceplayer
	msgbox MauvilleCity_House2_Text_IconicTutorIntro, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MauvilleCity_House2_EventScript_IconicTutorDeclined
	setmoverelearnerstate MOVE_RELEARNER_ICONIC_MOVES
	bufferstring STR_VAR_3, MoveRelearner_Text_MoveLWR
MauvilleCity_House2_EventScript_IconicTutorChooseMon:
	msgbox MoveRelearner_Text_ChoosePkmn, MSGBOX_DEFAULT
	chooseboxmon SELECT_PC_MON_MOVE_RELEARNER
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, MauvilleCity_House2_EventScript_IconicTutorDeclined
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_House2_EventScript_IconicTutorNothing
	special Special_HasMoveToRelearn
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_House2_EventScript_IconicTutorNothing
	msgbox MoveRelearner_Text_WhichXmoveShouldTeach, MSGBOX_DEFAULT
	special TeachMoveRelearnerMove
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_House2_EventScript_IconicTutorDeclined
	msgbox MauvilleCity_House2_Text_IconicTutorAnother, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, MauvilleCity_House2_EventScript_IconicTutorChooseMon
	goto MauvilleCity_House2_EventScript_IconicTutorDeclined

MauvilleCity_House2_EventScript_IconicTutorNothing:
	msgbox MauvilleCity_House2_Text_IconicTutorNothing, MSGBOX_DEFAULT
	goto MauvilleCity_House2_EventScript_IconicTutorChooseMon

MauvilleCity_House2_EventScript_IconicTutorDeclined:
	setmoverelearnerstate MOVE_RELEARNER_ALL_MOVES
	msgbox MauvilleCity_House2_Text_IconicTutorDeclined, MSGBOX_DEFAULT
	release
	end

MauvilleCity_House2_Text_BuyIceStoneAtSlateport: @ 8210CEE
	.string "If I had a Bike, it'd be easy to cycle to\n"
	.string "Slateport for some shopping.\p"
	.string "I'd be able to buy an Ice Stone at the\n"
	.string "Poké Mart in Slateport.$"

MauvilleCity_House2_Text_TradeIceStoneForCoinCase: @ 8210D76
	.string "Oh! You have an Ice Stone?\n"
	.string "Will you trade it for a Coin Case?$"

MauvilleCity_House2_Text_IllTradeYouCoinCase: @ 8210DB3
	.string "Oh, now I can evolve my Eevee!\n"
	.string "I'm so happy!\p"
	.string "Okay, I'll trade you a Coin Case!$"

MauvilleCity_House2_Text_UseCoinCaseAtGameCorner: @ 8210DE7
	.string "That Coin Case can be used\n"
	.string "at the Game Corner.$"

MauvilleCity_House2_Text_ThatsDisappointing: @ 8210E16
	.string "Oh, that's disappointing.\p"
	.string "A Coin Case is needed for the\n"
	.string "Game Corner.$"

MauvilleCity_House2_Text_IconicTutorIntro:
	.string "Some Pokémon are famous for moves\n"
	.string "nobody else will ever teach them.\p"
	.string "A flying Pikachu, a Magikarp with\n"
	.string "Dragon Rage… I know them all.\p"
	.string "The wilder ones I only share with\n"
	.string "Trainers who have proven themselves.\p"
	.string "Shall I teach one of yours?$"

MauvilleCity_House2_Text_IconicTutorNothing:
	.string "Hmm… I have nothing for that one.\n"
	.string "Or perhaps you need more Badges.$"

MauvilleCity_House2_Text_IconicTutorAnother:
	.string "Splendid! Shall I teach another?$"

MauvilleCity_House2_Text_IconicTutorDeclined:
	.string "Come back whenever a legend\n"
	.string "needs its signature move.$"
