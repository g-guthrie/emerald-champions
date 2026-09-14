Common_EventScript_PkmnCenterNurse::
	lock
	faceplayer
	call EC_DeliverTravelDocuments
	call_if_set FLAG_EC_BIRCH_GREAT_BALLS_PENDING, EventScript_PkmnCenterNurse_BirchGift
	call_if_set FLAG_EC_WOODS_GREAT_BALL_PENDING, EventScript_PkmnCenterNurse_WoodsGift
	call_if_set FLAG_EC_RUSTBORO_GREAT_BALL_PENDING, EventScript_PkmnCenterNurse_RustboroGift
	checkitem ITEM_POKE_VIAL, 1
	goto_if_eq VAR_RESULT, TRUE, EventScript_PkmnCenterNurse_HasVial
	msgbox gText_EmeraldChampionsFirstCenterVisit, MSGBOX_DEFAULT
	giveitem ITEM_POKE_VIAL, 1
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setvar VAR_POKE_VIAL_MAX_CHARGES, 1
	setvar VAR_POKE_VIAL_CHARGES, 1
	msgbox gText_EmeraldChampionsGiveLeveler, MSGBOX_DEFAULT
	giveitem ITEM_LEVELER, 1
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	msgbox gText_EmeraldChampionsGiveRepelSpray, MSGBOX_DEFAULT
	giveitem ITEM_REPEL_SPRAY, 1
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	msgbox gText_EmeraldChampionsGiveFlightBeacon, MSGBOX_DEFAULT
	giveitem ITEM_FLIGHT_BEACON, 1
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	message gText_EmeraldChampionsExplainTools
	waitmessage
	waitbuttonpress
	goto EventScript_PkmnCenterNurse_OfferHealAfterTools

EventScript_PkmnCenterNurse_HasVial::
	checkitem ITEM_LEVELER, 1
	goto_if_eq VAR_RESULT, TRUE, EventScript_PkmnCenterNurse_CheckRepelSpray
	msgbox gText_EmeraldChampionsGiveMissingLeveler, MSGBOX_DEFAULT
	giveitem ITEM_LEVELER, 1
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	msgbox gText_EmeraldChampionsExplainLeveler, MSGBOX_DEFAULT

@ Resume tool delivery if the Bag filled during an earlier visit.
EventScript_PkmnCenterNurse_CheckRepelSpray::
	checkitem ITEM_REPEL_SPRAY, 1
	goto_if_eq VAR_RESULT, TRUE, EventScript_PkmnCenterNurse_CheckFlightBeacon
	msgbox gText_EmeraldChampionsGiveMissingRepelSpray, MSGBOX_DEFAULT
	giveitem ITEM_REPEL_SPRAY, 1
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	msgbox gText_EmeraldChampionsExplainRepelSpray, MSGBOX_DEFAULT

EventScript_PkmnCenterNurse_CheckFlightBeacon::
	checkitem ITEM_FLIGHT_BEACON, 1
	goto_if_eq VAR_RESULT, TRUE, EventScript_PkmnCenterNurse_Main
	msgbox gText_EmeraldChampionsGiveMissingFlightBeacon, MSGBOX_DEFAULT
	giveitem ITEM_FLIGHT_BEACON, 1
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	msgbox gText_EmeraldChampionsExplainFlightBeacon, MSGBOX_DEFAULT
	goto EventScript_PkmnCenterNurse_OfferHealAfterTools

EventScript_PkmnCenterNurse_OfferHealAfterTools::
	setvar VAR_0x8004, 0
	msgbox gText_EmeraldChampionsHealAfterTools, MSGBOX_YESNO
	goto EventScript_PkmnCenterNurse_AnswerHeal

EventScript_PkmnCenterNurse_Main::
	setvar VAR_0x8004, 0
.if !IS_FRLG
	specialvar VAR_RESULT, CountPlayerTrainerStars
	goto_if_eq VAR_RESULT, 4, EventScript_PkmnCenterNurse_GoldCard
.endif
	msgbox gText_WouldYouLikeToRestYourPkmn, MSGBOX_YESNO
EventScript_PkmnCenterNurse_AnswerHeal::
	goto_if_eq VAR_RESULT, YES, EventScript_PkmnCenterNurse_HealPkmn
	goto_if_eq VAR_RESULT, NO, EventScript_PkmnCenterNurse_Goodbye
	end

EventScript_PkmnCenterNurse_Goodbye::
	message gText_WeHopeToSeeYouAgain
	return

@ VAR_0x8004 is 1 when player has Gold Card; jumps are identical
EventScript_PkmnCenterNurse_HealPkmn::
	incrementgamestat GAME_STAT_USED_POKECENTER
	call_if_eq VAR_0x8004, 0, EventScript_PkmnCenterNurse_IllTakeYourPkmn
	call_if_eq VAR_0x8004, 1, EventScript_PkmnCenterNurse_IllTakeYourPkmn2
	waitmessage
	call EventScript_PkmnCenterNurse_TakeAndHealPkmn
	goto_if_unset FLAG_POKERUS_EXPLAINED, EventScript_PkmnCenterNurse_CheckPokerus
	goto EventScript_PkmnCenterNurse_CheckTrainerHillAndUnionRoom
	end

EventScript_PkmnCenterNurse_IllTakeYourPkmn::
	message gText_IllTakeYourPkmn
	return

EventScript_PkmnCenterNurse_IllTakeYourPkmn2::
	message gText_IllTakeYourPkmn2
	return

EventScript_PkmnCenterNurse_TakeAndHealPkmn::
	hidefollower 0
	applymovement VAR_0x800B, Common_Movement_FaceLeft
	waitmovement 0
	dofieldeffect FLDEFF_POKECENTER_HEAL
.if OW_UNION_DISABLE_CHECK == FALSE && OW_FLAG_MOVE_UNION_ROOM_CHECK != 0
	setflag OW_FLAG_MOVE_UNION_ROOM_CHECK
	call CableClub_OnResume
	clearflag OW_FLAG_MOVE_UNION_ROOM_CHECK
.endif
	waitfieldeffect FLDEFF_POKECENTER_HEAL
	applymovement VAR_0x800B, Common_Movement_FaceDown
	waitmovement 0
	special HealPlayerParty
	special RestockEmeraldChampionsBasicBalls
	callnative RetryPendingLegendaryRelics
	copyvar VAR_POKE_VIAL_CHARGES, VAR_POKE_VIAL_MAX_CHARGES
	callnative UpdateFollowingPokemon
	return

EventScript_PkmnCenterNurse_CheckTrainerHillAndUnionRoom::
	specialvar VAR_RESULT, PlayerNotAtTrainerHillEntrance
	goto_if_eq VAR_RESULT, 0, EventScript_PkmnCenterNurse_ReturnPkmn
	specialvar VAR_RESULT, BufferUnionRoomPlayerName
	copyvar VAR_0x8008, VAR_RESULT
	goto_if_eq VAR_0x8008, 0, EventScript_PkmnCenterNurse_ReturnPkmn
	goto_if_eq VAR_0x8008, 1, EventScript_PkmnCenterNurse_PlayerWaitingInUnionRoom
	end

@ VAR_0x8004 is 1 when player has Gold Card
EventScript_PkmnCenterNurse_ReturnPkmn::
	goto_if_eq VAR_0x8004, 1, EventScript_PkmnCenterNurse_ReturnPkmn2
	message gText_RestoredPkmnToFullHealth
	waitmessage
	applymovement VAR_0x800B, Movement_PkmnCenterNurse_Bow
	waitmovement 0
	message gText_WeHopeToSeeYouAgain
	return

EventScript_PkmnCenterNurse_ReturnPkmn2::
	message gText_ThankYouForWaiting
	waitmessage
	applymovement VAR_0x800B, Movement_PkmnCenterNurse_Bow
	waitmovement 0
	message gText_WeHopeToSeeYouAgain2
	return

EventScript_PkmnCenterNurse_PlayerWaitingInUnionRoom::
	goto_if_set FLAG_NURSE_UNION_ROOM_REMINDER, EventScript_PkmnCenterNurse_ReturnPkmn
	msgbox gText_RestoredPkmnToFullHealth, MSGBOX_DEFAULT
	setflag FLAG_NURSE_UNION_ROOM_REMINDER
	message CableClub_Text_PlayerIsWaiting
	waitmessage
	applymovement VAR_0x800B, Movement_PkmnCenterNurse_Bow
	waitmovement 0
	message gText_WeHopeToSeeYouAgain
	return

EventScript_PkmnCenterNurse_CheckPokerus::
	specialvar VAR_RESULT, IsPokerusInParty
	goto_if_eq VAR_RESULT, TRUE, EventScript_PkmnCenterNurse_ExplainPokerus
	goto_if_eq VAR_RESULT, FALSE, EventScript_PkmnCenterNurse_CheckTrainerHillAndUnionRoom
	end

EventScript_PkmnCenterNurse_ExplainPokerus::
	message gText_PokerusExplanation
	setflag FLAG_POKERUS_EXPLAINED
	return

EventScript_PkmnCenterNurse_GoldCard::
	goto_if_set FLAG_NURSE_MENTIONS_GOLD_CARD, EventScript_PkmnCenterNurse_AskForUsual
	setflag FLAG_NURSE_MENTIONS_GOLD_CARD
	msgbox gText_WelcomeCutShort, MSGBOX_DEFAULT
	playse SE_PIN
	applymovement VAR_0x800B, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement VAR_0x800B, Common_Movement_Delay48
	waitmovement 0
	msgbox gText_NoticesGoldCard, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EventScript_PkmnCenterNurse_GoldCardHealPkmn
	message gText_WeHopeToSeeYouAgain2
	return

EventScript_PkmnCenterNurse_AskForUsual::
	msgbox gText_YouWantTheUsual, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EventScript_PkmnCenterNurse_GoldCardHealPkmn
	message gText_WeHopeToSeeYouAgain2
	return

EventScript_PkmnCenterNurse_GoldCardHealPkmn::
	setvar VAR_0x8004, 1
	goto EventScript_PkmnCenterNurse_HealPkmn
	end

Movement_PkmnCenterNurse_Bow:
	nurse_joy_bow
	delay_4
	step_end

@ Devon's two finite gifts are independent of story completion and healing.
EventScript_PkmnCenterNurse_WoodsGift::
	msgbox Text_PkmnCenterNurse_WoodsGift, MSGBOX_DEFAULT
	giveitem ITEM_DUSK_BALL, 3
	goto_if_eq VAR_RESULT, FALSE, EventScript_PkmnCenterNurse_KeepDevonGift
	clearflag FLAG_EC_WOODS_GREAT_BALL_PENDING
	return
EventScript_PkmnCenterNurse_RustboroGift::
	call EventScript_PkmnCenterNurse_DevonGift
	goto_if_eq VAR_RESULT, FALSE, EventScript_PkmnCenterNurse_KeepDevonGift
	clearflag FLAG_EC_RUSTBORO_GREAT_BALL_PENDING
	return
EventScript_PkmnCenterNurse_DevonGift::
	msgbox Text_PkmnCenterNurse_DevonGift, MSGBOX_DEFAULT
	giveitem ITEM_GREAT_BALL
	return
EventScript_PkmnCenterNurse_KeepDevonGift::
	msgbox Text_PkmnCenterNurse_KeepDevonGift, MSGBOX_DEFAULT
	return
Text_PkmnCenterNurse_WoodsGift:
	.string "The WOODS researcher left three\n"
	.string "DUSK BALLS for you. Here you are!$"
Text_PkmnCenterNurse_DevonGift:
	.string "A DEVON researcher left a GREAT BALL\n"
	.string "for you. Here you are!$"
Text_PkmnCenterNurse_KeepDevonGift:
	.string "Your BAG is still full. I'll keep it\n"
	.string "safe. Make room and speak to me again.$"

EventScript_PkmnCenterNurse_BirchGift::
	msgbox Text_PkmnCenterNurse_BirchGift, MSGBOX_DEFAULT
	giveitem ITEM_GREAT_BALL, 10
	goto_if_eq VAR_RESULT, FALSE, EventScript_PkmnCenterNurse_KeepDevonGift
	clearflag FLAG_EC_BIRCH_GREAT_BALLS_PENDING
	return
Text_PkmnCenterNurse_BirchGift:
	.string "PROF. BIRCH left 10 GREAT BALLS\n"
	.string "for your journey. Here you are!$"
