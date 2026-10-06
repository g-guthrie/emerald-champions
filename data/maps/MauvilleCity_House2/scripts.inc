MauvilleCity_House2_MapScripts:: @ 8210C5C
	.byte 0

MauvilleCity_House2_EventScript_Woman:: @ 8210C5D
	lock
	faceplayer
MauvilleCity_House2_EventScript_WomanMenu:
	message MauvilleCity_House2_Text_WomanOffer
	waitmessage
	dynmultichoice 30, 1, FALSE, 3, 0, DYN_MULTICHOICE_CB_NONE, MauvilleCity_House2_Text_Catalogue, MauvilleCity_House2_Text_CoinCase, MauvilleCity_House2_Text_IconicTutorExit
	goto_if_eq VAR_RESULT, 0, MauvilleCity_House2_EventScript_TutorCatalogue
	goto_if_eq VAR_RESULT, 1, MauvilleCity_House2_EventScript_CoinCase
	closemessage
	release
	end

MauvilleCity_House2_EventScript_TutorCatalogue:
	closemessage
	special ShowIconicTutorCatalogue
	goto MauvilleCity_House2_EventScript_WomanMenu

MauvilleCity_House2_EventScript_CoinCase:
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
	buffernumberstring STR_VAR_1, EC_ICONIC_MOVE_CAP_COST
	goto_if_set FLAG_MET_MAUVILLE_ICONIC_MOVE_TUTOR, MauvilleCity_House2_EventScript_IconicTutorMenu
	msgbox MauvilleCity_House2_Text_IconicTutorIntro, MSGBOX_DEFAULT
	setflag FLAG_MET_MAUVILLE_ICONIC_MOVE_TUTOR

MauvilleCity_House2_EventScript_IconicTutorMenu:
	message MauvilleCity_House2_Text_IconicTutorOffer
	waitmessage
	dynmultichoice 30, 1, FALSE, 3, 0, DYN_MULTICHOICE_CB_NONE, MauvilleCity_House2_Text_IconicTutorTeach, MauvilleCity_House2_Text_IconicTutorAbout, MauvilleCity_House2_Text_IconicTutorExit
	goto_if_eq VAR_RESULT, 0, MauvilleCity_House2_EventScript_IconicTutorStart
	goto_if_eq VAR_RESULT, 1, MauvilleCity_House2_EventScript_IconicTutorAbout
	goto MauvilleCity_House2_EventScript_IconicTutorDeclined

MauvilleCity_House2_EventScript_IconicTutorAbout:
	msgbox MauvilleCity_House2_Text_IconicTutorIntro, MSGBOX_DEFAULT
	goto MauvilleCity_House2_EventScript_IconicTutorMenu

MauvilleCity_House2_EventScript_IconicTutorStart:
	closemessage
	setmoverelearnerstate MOVE_RELEARNER_ICONIC_MOVES
	bufferstring STR_VAR_3, MoveRelearner_Text_MoveLWR
@ The party screen and move list carry their own prompts. B in the list
@ returns to the party screen; B there ends the lesson.
MauvilleCity_House2_EventScript_IconicTutorChooseMon:
	chooseboxmon SELECT_PC_MON_MOVE_RELEARNER_DIRECT
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, MauvilleCity_House2_EventScript_IconicTutorDeclined
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_House2_EventScript_IconicTutorNothing
	special Special_HasMoveToRelearn
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_House2_EventScript_IconicTutorNothing
	special TeachMoveRelearnerMove
	goto MauvilleCity_House2_EventScript_IconicTutorChooseMon

MauvilleCity_House2_EventScript_IconicTutorNothing:
	msgbox MauvilleCity_House2_Text_IconicTutorNothing, MSGBOX_DEFAULT
	goto MauvilleCity_House2_EventScript_IconicTutorChooseMon

MauvilleCity_House2_EventScript_IconicTutorDeclined:
	setmoverelearnerstate MOVE_RELEARNER_ALL_MOVES
	closemessage
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
	.string "Pikachu's Zippy Zap, Magikarp's\n"
	.string "Dragon Rage… I know them all.\p"
	.string "More Badges unlock more lessons.\n"
	.string "My neighbor has the catalogue.\p"
	.string "Each move costs {STR_VAR_1} Bottle Caps\n"
	.string "or one Gold Bottle Cap.\p"
	.string "That Pokémon can relearn its purchased\n"
	.string "moves here for free, even if it evolves!$"

MauvilleCity_House2_Text_IconicTutorNothing:
	.string "Hmm… I have nothing for that one.\n"
	.string "Or perhaps you need more Badges.$"

MauvilleCity_House2_Text_IconicTutorOffer:
	.string "What would you like?$"

MauvilleCity_House2_Text_IconicTutorTeach:
	.string "Teach a move$"

MauvilleCity_House2_Text_IconicTutorAbout:
	.string "About lessons$"

MauvilleCity_House2_Text_IconicTutorExit:
	.string "Exit$"

MauvilleCity_House2_Text_WomanOffer:
	.string "Browse the special move catalogue?\n"
	.string "Regional forms share the lessons.$"

MauvilleCity_House2_Text_Catalogue:
	.string "Move catalogue$"

MauvilleCity_House2_Text_CoinCase:
	.string "Coin Case$"
