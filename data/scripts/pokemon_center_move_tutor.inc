PKMN_Center_Move_Tutor::
	lockall
	message PKMN_Center_Move_Tutor_Text_HowCanHelp
	waitmessage
	setvar VAR_0x8004, SCROLL_MULTI_POKE_CENTER_TUTOR
	special ShowScrollableMultichoice
	waitstate
	switch VAR_RESULT
	case 0, PKMN_Center_Move_Tutor_MoveTutorIntro
	case 1, PKMN_Center_MoveDeleter_EventScript
	case 2, PKMN_Center_NameRater_EventScript
	case 3, PKMN_Center_EventScript_Mystery_Gift
	case 4, PKMN_Center_EventScript_Event_Reset
	case 5, PKMN_Center_BattleSet_ChooseMon
	case 6, PKMN_Center_Move_Tutor_General_Exit
	case MULTI_B_PRESSED, PKMN_Center_Move_Tutor_General_Exit

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Battle Set @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

PKMN_Center_BattleSet_ChooseMon::
	msgbox PKMN_Center_BattleSet_Text_ChooseMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, 255
	goto_if_eq PKMN_Center_Move_Tutor_General_Exit
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq PKMN_Center_BattleSet_CantBuildForEgg
	copyvar VAR_0x800A, VAR_0x8004
	special GetSelectedMonBattleSetCount
	compare VAR_RESULT, 1
	goto_if_eq PKMN_Center_BattleSet_UseDefault

PKMN_Center_BattleSet_ChooseStyle::
	msgbox PKMN_Center_BattleSet_Text_ChooseStyle, MSGBOX_DEFAULT
	setvar VAR_0x8004, SCROLL_MULTI_BATTLE_SET_STYLE
	special ShowScrollableMultichoice
	waitstate
	copyvar VAR_0x8006, VAR_RESULT
	special GetSelectedMonBattleSetCount
	compare VAR_0x8006, VAR_RESULT
	goto_if_ge PKMN_Center_BattleSet_ChooseMon
	copyvar VAR_0x8005, VAR_0x8006
	goto PKMN_Center_BattleSet_Confirm

PKMN_Center_BattleSet_UseDefault::
	setvar VAR_0x8005, 0

PKMN_Center_BattleSet_Confirm::
	copyvar VAR_0x8004, VAR_0x800A
	bufferpartymonnick 0, VAR_0x8004
	special BufferSelectedMonBattleSetName
	compare VAR_RESULT, TRUE
	goto_if_eq PKMN_Center_BattleSet_ConfirmMega
	msgbox PKMN_Center_BattleSet_Text_Confirm, MSGBOX_YESNO
	goto PKMN_Center_BattleSet_ConfirmResult

PKMN_Center_BattleSet_ConfirmMega::
	msgbox PKMN_Center_BattleSet_Text_ConfirmMega, MSGBOX_YESNO

PKMN_Center_BattleSet_ConfirmResult::
	compare VAR_RESULT, NO
	goto_if_eq PKMN_Center_BattleSet_ChooseStyle
	special ApplySelectedMonBattleSet
	compare VAR_RESULT, BATTLE_SET_APPLY_SPECIAL_ITEM
	goto_if_eq PKMN_Center_BattleSet_SpecialItemEquipped
	compare VAR_RESULT, BATTLE_SET_APPLY_MEGA_SET
	goto_if_eq PKMN_Center_BattleSet_MegaSuccess
	compare VAR_RESULT, TRUE
	goto_if_ne PKMN_Center_BattleSet_NoPreset
	playfanfare MUS_LEVEL_UP
	msgbox PKMN_Center_BattleSet_Text_Success, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_BattleSet_MegaSuccess::
	playfanfare MUS_LEVEL_UP
	msgbox PKMN_Center_BattleSet_Text_MegaSuccess, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_BattleSet_CantBuildForEgg::
	msgbox PKMN_Center_BattleSet_Text_CantBuildForEgg, MSGBOX_DEFAULT
	goto PKMN_Center_BattleSet_ChooseMon

PKMN_Center_BattleSet_NoPreset::
	msgbox PKMN_Center_BattleSet_Text_NoPreset, MSGBOX_DEFAULT
	goto PKMN_Center_BattleSet_ChooseMon

PKMN_Center_BattleSet_SpecialItemEquipped::
	msgbox PKMN_Center_BattleSet_Text_SpecialItemEquipped, MSGBOX_DEFAULT
	goto PKMN_Center_BattleSet_ChooseMon

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Move Reminder @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

PKMN_Center_MoveReminder_EventScript::
	msgbox PKMN_Center_MoveReminder_Text_WantMeToTeachMove, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, PKMN_Center_Move_Tutor_General_Exit
	goto PKMN_Center_Move_Tutor_MoveTutorIntro
	end

PKMN_Center_MoveReminder_EventScriptChooseMon::
	msgbox PKMN_Center_MoveReminder_Text_TutorWhichMon, MSGBOX_DEFAULT
	special ChooseMonForMoveRelearner
	waitstate
	compare VAR_0x8004, 255
	goto_if_eq PKMN_Center_Move_Tutor_General_Exit
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq PKMN_Center_MoveReminder_EventScriptCantTeachEgg
	compare VAR_0x8005, 0
	goto_if_eq PKMN_Center_MoveReminder_EventScriptNoMoveToTeachMon
	goto PKMN_Center_MoveReminder_EventScriptChooseMove
	end

PKMN_Center_MoveReminder_EventScriptChooseMove::
	msgbox PKMN_Center_MoveReminder_Text_TeachWhichMove, MSGBOX_DEFAULT
	special TeachMoveRelearnerMove
	waitstate
	compare VAR_0x8004, 0
	goto_if_eq PKMN_Center_MoveReminder_EventScriptChooseMon
	goto PKMN_Center_MoveReminder_EventScriptRememberAnotherMove
	end

PKMN_Center_MoveReminder_EventScriptNoMoveToTeachMon::
	msgbox PKMN_Center_MoveReminder_Text_DontHaveMoveToTeachPokemon, MSGBOX_DEFAULT
	goto PKMN_Center_MoveReminder_EventScriptChooseMon
	end

PKMN_Center_MoveReminder_EventScriptCantTeachEgg::
	msgbox PKMN_Center_MoveReminder_Text_CantTeachEgg, MSGBOX_DEFAULT
	goto PKMN_Center_MoveReminder_EventScriptChooseMon
	end
	
PKMN_Center_MoveReminder_EventScriptRememberAnotherMove::
	msgbox PKMN_Center_MoveReminder_Text_RememberAnotherMove, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq PKMN_Center_Move_Tutor_General_Exit
	goto PKMN_Center_MoveReminder_EventScriptChooseMon

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Move Deleter @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

PKMN_Center_MoveDeleter_EventScript::
	msgbox PKMN_Center_MoveDeleter_Text_ICanMakeMonForgetMove, MSGBOX_YESNO
	switch VAR_RESULT
	case YES, PKMN_Center_MoveDeleter_EventScript_ChooseMonAndMoveToForget
	case NO, PKMN_Center_Move_Tutor_General_Exit
	releaseall
	end

PKMN_Center_MoveDeleter_EventScript_ChooseMonAndMoveToForget::
	msgbox PKMN_Center_MoveDeleter_Text_WhichMonShouldForget, MSGBOX_DEFAULT
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, 255
	goto_if_eq PKMN_Center_Move_Tutor_General_Exit
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq PKMN_Center_MoveDeleter_EventScript_EggCantForgetMoves
	special GetNumMovesSelectedMonHas
	compare VAR_RESULT, 1
	goto_if_eq PKMN_Center_MoveDeleter_EventScript_MonOnlyKnowsOneMove
	msgbox PKMN_Center_MoveDeleter_Text_WhichMoveShouldBeForgotten, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	special MoveDeleterChooseMoveToForget
	fadescreen FADE_FROM_BLACK
	compare VAR_0x8005, MAX_MON_MOVES
	goto_if_eq PKMN_Center_MoveDeleter_EventScript_ChooseMonAndMoveToForget
	special BufferMoveDeleterNicknameAndMove
	msgbox PKMN_Center_MoveDeleter_Text_MonsMoveShouldBeForgotten, MSGBOX_YESNO
	switch VAR_RESULT
	case YES, PKMN_Center_MoveDeleter_EventScript_TryForgetMove
	case NO, PKMN_Center_Move_Tutor_General_Exit
	releaseall
	end

PKMN_Center_MoveDeleter_EventScript_TryForgetMove::
	special IsLastMonThatKnowsSurf
	compare VAR_RESULT, TRUE
	goto_if_eq PKMN_Center_MoveDeleter_EventScript_LastMonWithSurf
	special MoveDeleterForgetMove
	playfanfare MUS_MOVE_DELETED
	waitfanfare
	msgbox PKMN_Center_MoveDeleter_Text_MonHasForgottenMove, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_MoveDeleter_EventScript_MonOnlyKnowsOneMove::
	special BufferMoveDeleterNicknameAndMove
	msgbox PKMN_Center_MoveDeleter_Text_MonOnlyKnowsOneMove, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_MoveDeleter_EventScript_EggCantForgetMoves::
	msgbox PKMN_Center_MoveDeleter_Text_EggCantForgetMoves, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_MoveDeleter_EventScript_LastMonWithSurf::
	special BufferMoveDeleterNicknameAndMove
	msgbox PKMN_Center_MoveDeleter_Text_CantForgetSurf, MSGBOX_DEFAULT
	releaseall
	end

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Move Tutor @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

PKMN_Center_Move_Tutor_MoveTutorIntro::
	goto_if_set FLAG_TALKED_TO_MOVE_TUTOR, PKMN_Center_MoveReminder_EventScriptChooseMon
	msgbox PKMN_Center_Move_Tutor_Text_Explanation, MSGBOX_DEFAULT
	setflag FLAG_TALKED_TO_MOVE_TUTOR
	goto PKMN_Center_MoveReminder_EventScriptChooseMon

PKMN_Center_Move_Tutor_NoBadges::
	msgbox PKMN_Center_Move_Tutor_Text_NoBadgeNoMoves, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_Move_Tutor_MoveTutorMenu::
	setvar VAR_0x8008, 0
	goto_if_unset FLAG_BADGE01_GET, PKMN_Center_Move_Tutor_NoBadges
	msgbox PKMN_Center_Move_Tutor_Text_WhichSetofMoves, MSGBOX_DEFAULT
	special ScriptMenu_CreatePKMNCenterMoveTutorMultichoice
	waitstate
	special GetPKMNCenterMoveListMultichoice
	switch VAR_RESULT
	case 0, PKMN_Center_Move_Tutor_MoveList1
	case 1, PKMN_Center_Move_Tutor_MoveList2
	case 2, PKMN_Center_Move_Tutor_MoveList3
	case 3, PKMN_Center_Move_Tutor_MoveList4
	case 4, PKMN_Center_Move_Tutor_MoveList5
	case 5, PKMN_Center_Move_Tutor_MoveList6
	case 6, PKMN_Center_Move_Tutor_MoveList7
	case 7, PKMN_Center_Move_Tutor_ExitReturn
	case MULTI_B_PRESSED, PKMN_Center_Move_Tutor_ExitReturn
	releaseall
	end
	
PKMN_Center_Move_Tutor_MoveList1::
	setvar VAR_TEMP_C, SCROLL_MULTI_PC_TUTOR_SET_1
	setvar VAR_TEMP_E, 0
	setvar VAR_TEMP_F, 20
	setvar VAR_0x8004, SCROLL_MULTI_PC_TUTOR_SET_1
	setvar VAR_0x8006, 0
	goto PKMN_Center_Move_Tutor_ChooseMove
	
PKMN_Center_Move_Tutor_MoveList2::
	setvar VAR_TEMP_C, SCROLL_MULTI_PC_TUTOR_SET_2
	setvar VAR_TEMP_E, 1
	setvar VAR_TEMP_F, 18
	setvar VAR_0x8004, SCROLL_MULTI_PC_TUTOR_SET_2
	setvar VAR_0x8006, 0
	goto PKMN_Center_Move_Tutor_ChooseMove

PKMN_Center_Move_Tutor_MoveList3::
	setvar VAR_TEMP_C, SCROLL_MULTI_PC_TUTOR_SET_3
	setvar VAR_TEMP_E, 2
	setvar VAR_TEMP_F, 18
	setvar VAR_0x8004, SCROLL_MULTI_PC_TUTOR_SET_3
	setvar VAR_0x8006, 0
	goto PKMN_Center_Move_Tutor_ChooseMove

PKMN_Center_Move_Tutor_MoveList4::
	setvar VAR_TEMP_C, SCROLL_MULTI_PC_TUTOR_SET_4
	setvar VAR_TEMP_E, 3
	setvar VAR_TEMP_F, 18
	setvar VAR_0x8004, SCROLL_MULTI_PC_TUTOR_SET_4
	setvar VAR_0x8006, 0
	goto PKMN_Center_Move_Tutor_ChooseMove

PKMN_Center_Move_Tutor_MoveList5::
	setvar VAR_TEMP_C, SCROLL_MULTI_PC_TUTOR_SET_5
	setvar VAR_TEMP_E, 4
	setvar VAR_TEMP_F, 18
	setvar VAR_0x8004, SCROLL_MULTI_PC_TUTOR_SET_5
	setvar VAR_0x8006, 0
	goto PKMN_Center_Move_Tutor_ChooseMove

PKMN_Center_Move_Tutor_MoveList6::
	setvar VAR_TEMP_C, SCROLL_MULTI_PC_TUTOR_SET_6
	setvar VAR_TEMP_E, 5
	setvar VAR_TEMP_F, 18
	setvar VAR_0x8004, SCROLL_MULTI_PC_TUTOR_SET_6
	setvar VAR_0x8006, 0
	goto PKMN_Center_Move_Tutor_ChooseMove

PKMN_Center_Move_Tutor_MoveList7::
	setvar VAR_TEMP_C, SCROLL_MULTI_PC_TUTOR_SET_7
	setvar VAR_TEMP_E, 6
	setvar VAR_TEMP_F, 15
	setvar VAR_0x8004, SCROLL_MULTI_PC_TUTOR_SET_7
	setvar VAR_0x8006, 0
	goto PKMN_Center_Move_Tutor_ChooseMove

PKMN_Center_Move_Tutor_ChooseMove::
    msgbox PKMN_Center_Move_Tutor_Text_WhichMove, MSGBOX_DEFAULT
	special ShowScrollableMultichoice
	waitstate
	copyvar VAR_TEMP_D, VAR_RESULT
	compare VAR_RESULT, VAR_TEMP_F @ VAR_TEMP_F is set to the ID of the "Exit" option
	goto_if_eq PKMN_Center_Move_Tutor_Exit
	compare VAR_RESULT MULTI_B_PRESSED
	goto_if_eq PKMN_Center_Move_Tutor_CloseDescriptionWindow
	goto PKMN_Center_Move_Tutor_ConfirmMoveSelection
	end
	
PKMN_Center_Move_Tutor_ConfirmMoveSelection::
	copyvar VAR_0x8004, VAR_TEMP_D
	copyvar VAR_0x8005, VAR_TEMP_E
	special BufferBattleFrontierTutorMoveName
	buffernumberstring 1, VAR_0x8008
	copyvar VAR_0x8004, VAR_TEMP_C
	msgbox PKMN_Center_Move_Tutor_Text_IsThisRightMove, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq PKMN_Center_Move_Tutor_CloseDescriptionWindow
	goto PKMN_Center_Move_Tutor_TeachTutorMove
	end

PKMN_Center_Move_Tutor_TeachTutorMove::
	msgbox PKMN_Center_Move_Tutor_Text_TeachMoveToWhichMon, MSGBOX_DEFAULT
	special GetBattleFrontierTutorMoveIndex
	fadescreen FADE_TO_BLACK
	special CloseBattleFrontierTutorWindow
	special ChooseMonForMoveTutor
	waitstate
	compare VAR_RESULT, FALSE
	goto_if_eq PKMN_Center_Move_Tutor_CloseDescriptionWindow
	msgbox PKMN_Center_Move_Tutor_Text_TeachAnotherMove, MSGBOX_YESNO
    compare VAR_RESULT, NO
	goto_if_eq PKMN_Center_Move_Tutor_Exit
    goto PKMN_Center_Move_Tutor_CloseDescriptionWindow
	end

PKMN_Center_Move_Tutor_CloseDescriptionWindow::
    special CloseBattleFrontierTutorWindow
    goto PKMN_Center_Move_Tutor_MoveTutorMenu

PKMN_Center_Move_Tutor_CloseDescriptionWindowReturn::
    special CloseBattleFrontierTutorWindow
    goto PKMN_Center_Move_Tutor

PKMN_Center_Move_Tutor_ExitReturn::
    goto PKMN_Center_Move_Tutor

PKMN_Center_Move_Tutor_Exit::
	special CloseBattleFrontierTutorWindow
    msgbox PKMN_Center_Move_Tutor_Text_SeeYou, MSGBOX_DEFAULT
	releaseall
	end

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Name Rater @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

PKMN_Center_NameRater_EventScript::
	msgbox PKMN_Center_NameRater_Text_CritiqueWhichMonNickname, MSGBOX_DEFAULT
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, 255
	goto_if_ne PKMN_Center_NameRater_EventScript_RateMonNickname
	compare VAR_0x8004, 255
	goto_if_eq PKMN_Center_Move_Tutor_General_Exit
	end

PKMN_Center_NameRater_EventScript_RateMonNickname::
	specialvar VAR_RESULT, ScriptGetPartyMonSpecies
	compare VAR_RESULT, SPECIES_EGG
	goto_if_eq PKMN_Center_NameRater_EventScript_CantRateEgg
	special BufferMonNickname
	msgbox PKMN_Center_NameRater_Text_WhatShallNewNameBe, MSGBOX_DEFAULT
	call Common_EventScript_NameReceivedPartyMon
	specialvar VAR_RESULT, TryPutNameRaterShowOnTheAir
	special BufferMonNickname
	compare VAR_RESULT, TRUE
	goto_if_eq PKMN_Center_NameRater_EventScript_NewNameDifferent
	msgbox PKMN_Center_NameRater_Text_NameNoDifferentYetSuperior, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_NameRater_EventScript_CantRateEgg::
	msgbox PKMN_Center_NameRater_Text_ThatIsMerelyAnEgg, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_NameRater_EventScript_NewNameDifferent::
	msgbox PKMN_Center_NameRater_Text_MonShallBeKnownAsName, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_Move_Tutor_General_Exit::
    msgbox PKMN_Center_Move_Tutor_Text_SeeYou, MSGBOX_DEFAULT
	releaseall
	end

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ EXP Grinding @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
PKMN_Center_EventScript_Exp_Grind::

	releaseall
	end

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Event Reset @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
PKMN_Center_EventScript_Event_Reset::
	compare VAR_EVENT_RESET_STATE, 2
	goto_if_eq PKMN_Center_EventReset_EventScript_AlreadyReset
	msgbox PKMN_Center_EventResetText_WhatsBrokenThisTime, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, PKMN_Center_Move_Tutor_General_Exit
	case YES, PKMN_Center_EventReset_EventScript_ResetEvents
	case MULTI_B_PRESSED, PKMN_Center_Move_Tutor_General_Exit
	releaseall
	end

PKMN_Center_EventReset_EventScript_AlreadyReset::
	msgbox PKMN_Center_EventReset_Text_AlreadyReset, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_EventReset_EventScript_ResetEvents::
	compare VAR_EVENT_RESET_STATE, 0
	call_if_eq PKMN_Center_EventReset_EventScript_ResetEvents1
	compare VAR_EVENT_RESET_STATE, 1
	call_if_eq PKMN_Center_EventReset_EventScript_ResetEvents2
	msgbox PKMN_Center_EventReset_Text_Reset, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_EventReset_EventScript_ResetEvents1::
	setvar VAR_LAVARIDGE_LUCY_STATE, 1
	clearflag FLAG_HIDE_LAVARIDGE_POKECENTER_LUCY
	clearflag FLAG_ITEM_MOSSDEEP_CITY_MILOTICITE
	clearflag FLAG_RECEIVED_TOGEPI_EGG
	setvar VAR_EVENT_RESET_STATE, 1
	return

PKMN_Center_EventReset_EventScript_ResetEvents2::
	clearflag FLAG_RECEIVED_AUDINO
	setvar VAR_EVENT_RESET_STATE, 2
	return

PKMN_Center_EventScript_Mystery_Gift::
	specialvar VAR_TEMP_1, GetMysteryGiftSpecies
	compare VAR_TEMP_1, SPECIES_NONE
	goto_if_eq PKMN_Center_EventScript_Mystery_Gift_NoGiftAvailable
	bufferspeciesname 0, VAR_TEMP_1
	msgbox PKMN_Center_Text_Mystery_Gift_WouldYouLikeGift, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq PKMN_Center_Move_Tutor_General_Exit
	special GetStaticEncounterLevel
	givemon VAR_TEMP_1, VAR_0x800A, VAR_0x8009
	compare VAR_RESULT, 0
	goto_if_eq PKMN_Center_Mystery_Gift_EventScript_ReceiveGiftParty
	compare VAR_RESULT, 1
	goto_if_eq PKMN_Center_Mystery_Gift_EventScript_ReceiveGiftPC
	goto Common_EventScript_NoMoreRoomForPokemon
PKMN_Center_Mystery_Gift_EventScript_TakeCare::
	switch VAR_TEMP_1
	case SPECIES_GRENINJA_BATTLE_BOND, PKMN_Center_EventScript_SetflagGreninja
	case SPECIES_MAGEARNA, PKMN_Center_EventScript_SetflagMagearna
	case SPECIES_MELOETTA, PKMN_Center_EventScript_SetflagMeloetta
	releaseall
	end

PKMN_Center_EventScript_SetflagGreninja::
	setflag FLAG_RECEIVED_ASH_GRENINJA
	releaseall
	end

PKMN_Center_EventScript_SetflagMagearna::
	setflag FLAG_RECEIVED_MAGEARNA
	releaseall
	end

PKMN_Center_EventScript_SetflagMeloetta::
	setflag FLAG_RECEIVED_MELOETTA
	releaseall
	end

PKMN_Center_EventScript_Mystery_Gift_NoGiftAvailable::
	msgbox PKMN_Center_Text_Mystery_Gift_NoGiftAvailable, MSGBOX_DEFAULT
	releaseall
	end

PKMN_Center_Mystery_Gift_EventScript_ReceiveGiftParty::
	call PKMN_Center_Mystery_Gift_EventScript_ReceivedGiftFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq PKMN_Center_Mystery_Gift_EventScript_TakeCare
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto PKMN_Center_Mystery_Gift_EventScript_TakeCare
	end

PKMN_Center_Mystery_Gift_EventScript_ReceiveGiftPC::
	call PKMN_Center_Mystery_Gift_EventScript_ReceivedGiftFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq PKMN_Center_Mystery_Gift_EventScript_SendGiftToPC
	call Common_EventScript_NameReceivedBoxMon
	goto PKMN_Center_Mystery_Gift_EventScript_SendGiftToPC
	end

PKMN_Center_Mystery_Gift_EventScript_SendGiftToPC::
	call Common_EventScript_TransferredToPC
	goto PKMN_Center_Mystery_Gift_EventScript_TakeCare
	end

PKMN_Center_Mystery_Gift_EventScript_ReceivedGiftFanfare::
	bufferspeciesname 1, VAR_TEMP_1
	playfanfare MUS_OBTAIN_ITEM
	message PKMN_Center_Mystery_Gift_Text_ReceivedGift
	waitmessage
	waitfanfare
	bufferspeciesname 0, VAR_TEMP_1
	return

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Strings @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

PKMN_Center_EventReset_Text_Reset:
	.string "Events restored!$"

PKMN_Center_EventReset_Text_AlreadyReset:
	.string "You've already restored these events.$"

PKMN_Center_EventResetText_WhatsBrokenThisTime:
	.string "I can restore a few special events\n"
	.string "that may have ended too soon.\p"
	.string "Each group can only be restored once.\p"
	.string "Would you like me to restore them?$"

PKMN_Center_EventResetText_AlreadyReset:

PKMN_Center_Move_Tutor_Text_HowCanHelp:
	.string "How can I help you today?$"

PKMN_Center_Move_Tutor_Text_Explanation:
	.string "I can teach each Pokémon any move\n"
	.string "that species can legally learn.\p"
	.string "That includes level-up moves, Egg\n"
	.string "moves, TMs, HMs, and tutor moves.\p"
	.string "There's no charge, and no Gym Badges\n"
	.string "are required. Choose a Pokémon!$"

PKMN_Center_Move_Tutor_Text_AllTMsAvailable:
	.string "Every legal move is available here.\p"
	.string "Bring me any compatible Pokémon and\n"
	.string "I'll teach it whenever you like.$"

PKMN_Center_Move_Tutor_Text_NoBadgeNoMoves:
	.string "I'm sorry, but that Pokémon has no\n"
	.string "other legal moves I can teach.$"

PKMN_Center_Move_Tutor_Text_WhichSetofMoves:
	.string "Which set of moves would you\n"
	.string "like to choose from?$"

PKMN_Center_Move_Tutor_Text_WhichMove:
	.string "Which move should I teach to your\n"
	.string "Pokémon?$"
	
PKMN_Center_Move_Tutor_Text_IsThisRightMove:
	.string "The move {STR_VAR_1}, is it?$"

PKMN_Center_Move_Tutor_Text_TeachMoveToWhichMon:
	.string "Which Pokémon should learn\n"
	.string "{STR_VAR_1}?$"
	
PKMN_Center_Move_Tutor_Text_TeachAnotherMove:
	.string "Would you like me to teach\n"
    .string "another move?$"

PKMN_Center_Move_Tutor_Text_SeeYou:
	.string "See you later, then!$"

PKMN_Center_BattleSet_Text_ChooseMon:
	.string "Choose a Pokémon for a complete\n"
	.string "battle set.$"

PKMN_Center_BattleSet_Text_ChooseStyle:
	.string "Which battle style should I teach?$"

PKMN_Center_BattleSet_Text_Confirm:
	.string "Ready to give {STR_VAR_1} the\n"
	.string "{STR_VAR_2} set.\p"
	.string "This replaces all four moves,\n"
	.string "the nature, ability, and held item.\p"
	.string "It will hold {STR_VAR_3}.\p"
	.string "Continue?$"

PKMN_Center_BattleSet_Text_Success:
	.string "All set! {STR_VAR_1} learned its\n"
	.string "new battle moves.\p"
	.string "I also matched its ability and\n"
	.string "nature, and gave it {STR_VAR_3}.$"

PKMN_Center_BattleSet_Text_ConfirmMega:
	.string "Ready to give {STR_VAR_1} the\n"
	.string "{STR_VAR_2} set.\p"
	.string "This replaces all four moves,\n"
	.string "the nature, ability, and held item.\p"
	.string "It is built for {STR_VAR_3}.\n"
	.string "I will not supply that Mega Stone.\p"
	.string "Continue?$"

PKMN_Center_BattleSet_Text_MegaSuccess:
	.string "All set! {STR_VAR_1} learned its\n"
	.string "new battle moves.\p"
	.string "I matched its ability and nature.\n"
	.string "Use this set with {STR_VAR_3}.$"

PKMN_Center_BattleSet_Text_CantBuildForEgg:
	.string "An Egg can't use a battle set.$"

PKMN_Center_BattleSet_Text_NoPreset:
	.string "I can't build a battle set for\n"
	.string "that Pokémon.$"

PKMN_Center_BattleSet_Text_SpecialItemEquipped:
	.string "I won't discard the {STR_VAR_2}.\p"
	.string "Remove that special item first, then\n"
	.string "come back when you're ready.$"

PKMN_Center_MoveReminder_Text_WantMeToTeachMove:
	.string "Would you like me to teach one of your\n"
	.string "Pokémon a legal move?$"

PKMN_Center_MoveReminder_Text_TutorWhichMon:
	.string "Which Pokémon should I teach?$"

PKMN_Center_MoveReminder_Text_TeachWhichMove:
	.string "Which move should it learn?$"

PKMN_Center_MoveReminder_Text_DontHaveMoveToTeachPokemon:
	.string "I'm sorry, that Pokémon has no other\n"
	.string "legal moves to learn.$"

PKMN_Center_MoveReminder_Text_CantTeachEgg:
	.string "I'm sorry, an Egg can't be taught\n"
	.string "moves.$"

PKMN_Center_MoveReminder_Text_RememberAnotherMove:
	.string "Would you like me to teach another\n"
	.string "move?$"

PKMN_Center_MoveDeleter_Text_ICanMakeMonForgetMove:
	.string "Would you like one of your Pokémon\n"
	.string "to forget a move?$"

PKMN_Center_MoveDeleter_Text_WhichMonShouldForget:
	.string "Which Pokémon should forget a move?$"

PKMN_Center_MoveDeleter_Text_WhichMoveShouldBeForgotten:
	.string "Which move should be forgotten?$"

PKMN_Center_MoveDeleter_Text_MonOnlyKnowsOneMove:
	.string "{STR_VAR_1} knows only one move,\n"
	.string "so it can't be forgotten.$"

PKMN_Center_MoveDeleter_Text_MonsMoveShouldBeForgotten:
	.string "Hm! {STR_VAR_1}'s {STR_VAR_2}?\n"
	.string "That move should be forgotten?$"

PKMN_Center_MoveDeleter_Text_MonHasForgottenMove:
	.string "It worked to perfection!\p"
	.string "{STR_VAR_1} has forgotten\n"
	.string "{STR_VAR_2} completely.$"

PKMN_Center_MoveDeleter_Text_EggCantForgetMoves:
	.string "What?\n"
	.string "No Egg should know any moves.$"

PKMN_Center_MoveDeleter_Text_CantForgetSurf:
	.string "Hm!\p"
	.string "Your {STR_VAR_1} doesn't seem willing\n"
	.string "to forget Surf.$"

PKMN_Center_NameRater_Text_CritiqueWhichMonNickname:
	.string "Which Pokémon's nickname should\n"
	.string "be changed?$"

PKMN_Center_NameRater_Text_WhatShallNewNameBe:
	.string "What would you like {STR_VAR_1}'s\n"
	.string "new nickname to be?$"

PKMN_Center_NameRater_Text_MonShallBeKnownAsName:
	.string "Done! From now on, this Pokémon\n"
	.string "shall be known as {STR_VAR_1}!$"

PKMN_Center_NameRater_Text_NameNoDifferentYetSuperior:
	.string "Okay! {STR_VAR_1}'s nickname\n"
	.string "will stay as it is.$"

PKMN_Center_NameRater_Text_ThatIsMerelyAnEgg:
	.string "Now, now.\n"
	.string "That is merely an Egg!$"

PKMN_Center_Text_Mystery_Gift_NoGiftAvailable:
	.string "I'm sorry, there are no Mystery Gifts\n"
	.string "available at this time.$"

PKMN_Center_Mystery_Gift_Text_ReceivedGift::
	.string "{PLAYER} received {STR_VAR_1}!\p"
	.string "Take good care of {STR_VAR_1}!$"

PKMN_Center_Text_Mystery_Gift_WouldYouLikeGift::
	.string "Thank you for playing\n"
	.string "Emerald Champions!\p"
	.string "You have a Mystery Gift available:\n"
	.string "a special {STR_VAR_1}!\p"
	.string "Would you like to receive\n"
	.string "{STR_VAR_1}?$"
