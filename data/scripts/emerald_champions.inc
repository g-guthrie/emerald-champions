Common_EventScript_EmeraldChampionsMoveTutor::
	lockall
	faceplayer
	setvar VAR_0x8007, EC_BATTLE_FORMAT_DOUBLES
	setvar VAR_0x8009, 0
	setvar VAR_0x800B, 0

EmeraldChampions_EventScript_MoveTutorMain:
	message EmeraldChampions_Text_MoveTutorHowCanHelp
	waitmessage
	dynmultipush EmeraldChampions_Text_BuildBattleSet, 0
	dynmultipush EmeraldChampions_Text_ChangeNature, 1
	dynmultipush EmeraldChampions_Text_EditStatPoints, 2
	dynmultipush EmeraldChampions_Text_LearnMove, 3
	dynmultipush EmeraldChampions_Text_OtherServices, 4
	dynmultipush EmeraldChampions_Text_Exit, 5
	@ A far-right anchor gives this the same upper-right presentation as
	@ Inclement Emerald's native Pokémon Center specialist menu.  The dynamic
	@ menu code clamps the final left edge from its measured text width.
	dynmultistack 30, 1, FALSE, 5, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_BattleSetChooseMon
	case 1, EmeraldChampions_EventScript_NatureChooseMon
	case 2, EmeraldChampions_EventScript_StatPointChooseMon
	case 3, EmeraldChampions_EventScript_OpenMoveRelearner
	case 4, EmeraldChampions_EventScript_OtherServices
	case 5, EmeraldChampions_EventScript_MoveTutorExit
	case MULTI_B_PRESSED, EmeraldChampions_EventScript_MoveTutorExit

EmeraldChampions_EventScript_OtherServices:
	message EmeraldChampions_Text_OtherServicesPrompt
	waitmessage
	dynmultipush EmeraldChampions_Text_ForgetMove, 0
	dynmultipush EmeraldChampions_Text_RenamePokemon, 1
	dynmultipush EmeraldChampions_Text_Back, 2
	dynmultistack 30, 1, FALSE, 3, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_OpenMoveDeleter
	case 1, EmeraldChampions_EventScript_OpenNameRater
	case 2, EmeraldChampions_EventScript_MoveTutorMain
	case MULTI_B_PRESSED, EmeraldChampions_EventScript_MoveTutorMain

EmeraldChampions_EventScript_OpenMoveRelearner:
	setmoverelearnerstate MOVE_RELEARNER_ALL_MOVES
	bufferstring STR_VAR_3, MoveRelearner_Text_MoveLWR

EmeraldChampions_EventScript_MoveRelearnerChooseMon:
	getpartysize
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_MoveRelearnerNoPokemon
	msgbox MoveRelearner_Text_ChoosePkmn, MSGBOX_DEFAULT
	chooseboxmon SELECT_PC_MON_MOVE_RELEARNER
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_MoveTutorMain
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_MoveRelearnerEgg
	special Special_HasMoveToRelearn
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_MoveRelearnerNoMoves
	msgbox MoveRelearner_Text_WhichXmoveShouldTeach, MSGBOX_DEFAULT
	special TeachMoveRelearnerMove
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_MoveTutorMain
	msgbox MoveRelearner_Text_AnythingElse, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EmeraldChampions_EventScript_MoveRelearnerChooseMon
	goto EmeraldChampions_EventScript_MoveTutorMain

EmeraldChampions_EventScript_MoveRelearnerNoPokemon:
	msgbox MoveRelearner_Text_HaveNoPkmn, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_MoveTutorMain

EmeraldChampions_EventScript_MoveRelearnerEgg:
	msgbox MoveRelearner_Text_CantTeachMoveToEgg, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_MoveRelearnerChooseMon

EmeraldChampions_EventScript_MoveRelearnerNoMoves:
	msgbox MoveRelearner_Text_CantTeachMoveToPkmn, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_MoveRelearnerChooseMon

EmeraldChampions_EventScript_OpenMoveDeleter:
	msgbox EmeraldChampions_Text_SpecialistForgetMove, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_OtherServices

EmeraldChampions_EventScript_MoveDeleterChooseMon:
	msgbox LilycoveCity_MoveDeletersHouse_Text_WhichMonShouldForget, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_OtherServices
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_MoveDeleterEgg
	special GetNumMovesSelectedMonHas
	goto_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_MoveDeleterOneMove
	msgbox LilycoveCity_MoveDeletersHouse_Text_WhichMoveShouldBeForgotten, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	special MoveDeleterChooseMoveToForget
	fadescreen FADE_FROM_BLACK
	goto_if_eq VAR_0x8005, MAX_MON_MOVES, EmeraldChampions_EventScript_MoveDeleterChooseMon
	special BufferMoveDeleterNicknameAndMove
	msgbox LilycoveCity_MoveDeletersHouse_Text_MonsMoveShouldBeForgotten, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_MoveDeleterChooseMon
	special IsLastMonThatKnowsSurf
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_MoveDeleterLastSurf
	special MoveDeleterForgetMove
	playfanfare MUS_MOVE_DELETED
	waitfanfare
	msgbox LilycoveCity_MoveDeletersHouse_Text_MonHasForgottenMove, MSGBOX_DEFAULT
	msgbox EmeraldChampions_Text_ForgetAnotherMove, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EmeraldChampions_EventScript_MoveDeleterChooseMon
	goto EmeraldChampions_EventScript_OtherServices

EmeraldChampions_EventScript_MoveDeleterEgg:
	msgbox LilycoveCity_MoveDeletersHouse_Text_EggCantForgetMoves, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_MoveDeleterChooseMon

EmeraldChampions_EventScript_MoveDeleterOneMove:
	special BufferMoveDeleterNicknameAndMove
	msgbox LilycoveCity_MoveDeletersHouse_Text_MonOnlyKnowsOneMove, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_MoveDeleterChooseMon

EmeraldChampions_EventScript_MoveDeleterLastSurf:
	special BufferMoveDeleterNicknameAndMove
	msgbox LilycoveCity_MoveDeletersHouse_Text_CantForgetSurf, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_MoveDeleterChooseMon

EmeraldChampions_EventScript_OpenNameRater:
EmeraldChampions_EventScript_NameRaterChooseMon:
	msgbox SlateportCity_NameRatersHouse_Text_CritiqueWhichMonNickname, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_OtherServices
	specialvar VAR_RESULT, ScriptGetPartyMonSpecies
	goto_if_eq VAR_RESULT, SPECIES_EGG, EmeraldChampions_EventScript_NameRaterEgg
	special BufferMonNickname
	special IsMonOTIDNotPlayers
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_NameRaterNotPlayerOT
	specialvar VAR_RESULT, MonOTNameNotPlayer
	special BufferMonNickname
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_NameRaterNotPlayerOT
	msgbox SlateportCity_NameRatersHouse_Text_FineNameSuggestBetterOne, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_OtherServices
	msgbox SlateportCity_NameRatersHouse_Text_WhatShallNewNameBe, MSGBOX_DEFAULT
	call Common_EventScript_NameReceivedPartyMon
	specialvar VAR_RESULT, TryPutNameRaterShowOnTheAir
	special BufferMonNickname
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_NameRaterChanged
	msgbox SlateportCity_NameRatersHouse_Text_NameNoDifferentYetSuperior, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_OtherServices

EmeraldChampions_EventScript_NameRaterEgg:
	msgbox SlateportCity_NameRatersHouse_Text_ThatIsMerelyAnEgg, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_NameRaterChooseMon

EmeraldChampions_EventScript_NameRaterNotPlayerOT:
	msgbox SlateportCity_NameRatersHouse_Text_MagnificentName, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_NameRaterChooseMon

EmeraldChampions_EventScript_NameRaterChanged:
	msgbox SlateportCity_NameRatersHouse_Text_MonShallBeKnownAsName, MSGBOX_DEFAULT
	msgbox EmeraldChampions_Text_RenameAnotherPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EmeraldChampions_EventScript_NameRaterChooseMon
	goto EmeraldChampions_EventScript_OtherServices

Common_EventScript_EmeraldChampionsStatPointEditor::
	lockall
	faceplayer
	setvar VAR_0x8009, 1
	goto EmeraldChampions_EventScript_StatPointChooseMon

EmeraldChampions_EventScript_StatPointChooseMon:
	msgbox EmeraldChampions_Text_ChooseStatPointMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_StatPointCancel
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_StatPointEgg
	copyvar VAR_0x800A, VAR_0x8004
	setvar VAR_0x8008, 0
	goto EmeraldChampions_EventScript_StatPointMain

EmeraldChampions_EventScript_StatPointMain:
	special BufferSelectedMonEmeraldChampionsStatPointSummary
	message gStringVar4
	waitmessage
	setvar VAR_0x8004, SCROLL_MULTI_EMERALD_CHAMPIONS_STAT_POINTS
	special ShowScrollableMultichoice
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_StatPointChooseMon
	copyvar VAR_0x8008, VAR_RESULT
	goto_if_eq VAR_0x8008, 7, EmeraldChampions_EventScript_StatPointChooseMon
	goto_if_eq VAR_0x8008, 6, EmeraldChampions_EventScript_StatPointReset
	copyvar VAR_0x8005, VAR_0x8008
	setvar VAR_0x8006, 0
	goto EmeraldChampions_EventScript_StatPointAdjust

EmeraldChampions_EventScript_StatPointAdjust:
	special BufferSelectedMonEmeraldChampionsStatPointDetail
	message gStringVar4
	waitmessage
	setvar VAR_0x8004, SCROLL_MULTI_EMERALD_CHAMPIONS_STAT_ADJUST
	special ShowScrollableMultichoice

@ The adjustment list persists between presses. Each step is applied, the
@ running total is rewritten in place, and the same list resumes input.
EmeraldChampions_EventScript_StatPointAdjustResult:
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_StatPointMain
	copyvar VAR_0x8006, VAR_RESULT
	goto_if_eq VAR_0x8006, 8, EmeraldChampions_EventScript_StatPointMain
	special AdjustSelectedMonEmeraldChampionsStatPoints
	call_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_StatPointAdjustmentBlocked
	special BufferSelectedMonEmeraldChampionsStatPointDetail
	message gStringVar4
	waitmessage
	special ScrollableMultichoice_TryReturnToList
	waitstate
	goto EmeraldChampions_EventScript_StatPointAdjustResult

EmeraldChampions_EventScript_StatPointAdjustmentBlocked:
	playse SE_FAILURE
	return

EmeraldChampions_EventScript_StatPointReset:
	msgbox EmeraldChampions_Text_ResetStatPoints, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_StatPointMain
	special ResetSelectedMonEmeraldChampionsStatPoints
	msgbox EmeraldChampions_Text_StatPointsReset, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_StatPointMain

EmeraldChampions_EventScript_StatPointEgg:
	msgbox EmeraldChampions_Text_StatPointEgg, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_StatPointChooseMon

EmeraldChampions_EventScript_StatPointCancel:
	goto_if_eq VAR_0x8009, 1, EmeraldChampions_EventScript_StatPointExternalExit
	goto EmeraldChampions_EventScript_MoveTutorMain

EmeraldChampions_EventScript_StatPointExternalExit:
	msgbox EmeraldChampions_Text_StatPointGoodbye, MSGBOX_DEFAULT
	releaseall
	end

@ Nature service. Mirrors the party menu's Change Ability flow: pick a party
@ member, see what it is now, then pick the replacement from a scrolling list
@ that names the stat each Nature raises and lowers. B backs out at every step.
EmeraldChampions_EventScript_NatureChooseMon:
	msgbox EmeraldChampions_Text_ChooseNatureMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_MoveTutorMain
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_NatureEgg
	special BufferSelectedMonNature
	copyvar VAR_0x800A, VAR_0x8004
	message EmeraldChampions_Text_CurrentNature
	waitmessage
@ The list is built from gNaturesInfo in C (see BuildEmeraldChampionsNatureMenuText)
@ and opens on the current Nature. VAR_0x8004 carries the menu id for the
@ scrolling list, so the party slot rides in VAR_0x800A meanwhile.
	setvar VAR_0x8004, SCROLL_MULTI_EMERALD_CHAMPIONS_NATURES
	special ShowScrollableMultichoice
	copyvar VAR_0x8004, VAR_0x800A
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_NatureChooseMon
	special SetHiddenNature
	playfanfare MUS_LEVEL_UP
	special BufferSelectedMonNature
	msgbox EmeraldChampions_Text_NatureChanged, MSGBOX_DEFAULT
	msgbox EmeraldChampions_Text_ChangeAnotherNature, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EmeraldChampions_EventScript_NatureChooseMon
	goto EmeraldChampions_EventScript_MoveTutorMain

EmeraldChampions_EventScript_NatureEgg:
	msgbox EmeraldChampions_Text_NatureEgg, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_NatureChooseMon

EmeraldChampions_EventScript_BattleSetChooseMon:
	call_if_eq VAR_0x800B, 0, EmeraldChampions_EventScript_ExplainBattleSets
	msgbox EmeraldChampions_Text_ChooseBattleSetMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_MoveTutorMain
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleSetEgg
	copyvar VAR_0x800A, VAR_0x8004
	goto EmeraldChampions_EventScript_BattleSetChooseFormat

EmeraldChampions_EventScript_BattleSetChooseFormat:
	message EmeraldChampions_Text_ChooseBattleFormat
	waitmessage
	dynmultipush EmeraldChampions_Text_Doubles, EC_BATTLE_FORMAT_DOUBLES
	dynmultipush EmeraldChampions_Text_Singles, EC_BATTLE_FORMAT_SINGLES
	dynmultipush EmeraldChampions_Text_Back, 2
	dynmultistack 30, 1, FALSE, 3, 0, VAR_0x8007, DYN_MULTICHOICE_CB_NONE
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_BattleSetChooseMon
	goto_if_eq VAR_RESULT, 2, EmeraldChampions_EventScript_BattleSetChooseMon
	copyvar VAR_0x8007, VAR_RESULT
	setvar VAR_0x8005, 0
	special GetSelectedMonEmeraldChampionsBattleSetCount
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_BattleSetMissing

EmeraldChampions_EventScript_BattleSetChooseStyle:
	special BufferSelectedMonCurrentEmeraldChampionsBattleSet
	goto_if_eq VAR_0x8007, EC_BATTLE_FORMAT_SINGLES, EmeraldChampions_EventScript_BattleSetChooseSinglesStyle
	message EmeraldChampions_Text_ChooseDoublesBattleSetStyle
	waitmessage
	goto EmeraldChampions_EventScript_BattleSetOpenStyleList

EmeraldChampions_EventScript_BattleSetChooseSinglesStyle:
	message EmeraldChampions_Text_ChooseSinglesBattleSetStyle
	waitmessage
	goto EmeraldChampions_EventScript_BattleSetOpenStyleList

EmeraldChampions_EventScript_BattleSetOpenStyleList:
	setvar VAR_0x8004, SCROLL_MULTI_EMERALD_CHAMPIONS_BATTLE_SET
	special ShowScrollableMultichoice
	copyvar VAR_0x8006, VAR_RESULT
	special GetSelectedMonEmeraldChampionsBattleSetCount
	goto_if_ge VAR_0x8006, VAR_RESULT, EmeraldChampions_EventScript_BattleSetChooseFormat
	copyvar VAR_0x8005, VAR_0x8006
	goto EmeraldChampions_EventScript_BattleSetPreview

@ Read the set out loud before the yes/no prompt: four moves, then Ability,
@ nature and held item. Declining the prompt returns to the style list.
EmeraldChampions_EventScript_BattleSetPreview:
	copyvar VAR_0x8004, VAR_0x800A
	special BufferSelectedMonEmeraldChampionsBattleSetPreview
	msgbox gStringVar4, MSGBOX_DEFAULT

EmeraldChampions_EventScript_BattleSetConfirm:
	copyvar VAR_0x8004, VAR_0x800A
	bufferpartymonnick 0, VAR_0x8004
	special BufferSelectedMonEmeraldChampionsBattleSetName
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleSetConfirmMega
	msgbox EmeraldChampions_Text_ConfirmBattleSet, MSGBOX_YESNO
	goto EmeraldChampions_EventScript_BattleSetConfirmResult

EmeraldChampions_EventScript_BattleSetConfirmMega:
	msgbox EmeraldChampions_Text_ConfirmMegaBattleSet, MSGBOX_YESNO

EmeraldChampions_EventScript_BattleSetConfirmResult:
	goto_if_eq VAR_RESULT, YES, EmeraldChampions_EventScript_BattleSetApply
	goto EmeraldChampions_EventScript_BattleSetChooseStyle

EmeraldChampions_EventScript_BattleSetApply:
	special ApplySelectedMonEmeraldChampionsBattleSet
	goto_if_eq VAR_RESULT, EC_BATTLE_SET_SPECIAL_ITEM_EQUIPPED, EmeraldChampions_EventScript_BattleSetSpecialItem
	goto_if_eq VAR_RESULT, EC_BATTLE_SET_MEGA_STONE_HELD, EmeraldChampions_EventScript_BattleSetMegaHeldSuccess
	goto_if_eq VAR_RESULT, EC_BATTLE_SET_MEGA, EmeraldChampions_EventScript_BattleSetMegaSuccess
	goto_if_ne VAR_RESULT, EC_BATTLE_SET_SUCCESS, EmeraldChampions_EventScript_BattleSetMissing
	playfanfare MUS_LEVEL_UP
	msgbox EmeraldChampions_Text_BattleSetSuccess, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_PrepareAnotherBattleSet

EmeraldChampions_EventScript_BattleSetMegaSuccess:
	playfanfare MUS_LEVEL_UP
	msgbox EmeraldChampions_Text_MegaBattleSetSuccess, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_PrepareAnotherBattleSet

EmeraldChampions_EventScript_BattleSetMegaHeldSuccess:
	playfanfare MUS_LEVEL_UP
	msgbox EmeraldChampions_Text_MegaBattleSetHeldSuccess, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_PrepareAnotherBattleSet

EmeraldChampions_EventScript_PrepareAnotherBattleSet:
	msgbox EmeraldChampions_Text_PrepareAnotherBattleSet, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EmeraldChampions_EventScript_BattleSetChooseMon
	goto EmeraldChampions_EventScript_MoveTutorMain

EmeraldChampions_EventScript_BattleSetEgg:
	msgbox EmeraldChampions_Text_BattleSetEgg, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_BattleSetChooseMon

@ Explain what a set does once per visit, then keep asking the plain question.
EmeraldChampions_EventScript_ExplainBattleSets:
	setvar VAR_0x800B, 1
	msgbox EmeraldChampions_Text_ExplainBattleSets, MSGBOX_DEFAULT
	return

EmeraldChampions_EventScript_BattleSetMissing:
	msgbox EmeraldChampions_Text_BattleSetMissing, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_BattleSetChooseFormat

EmeraldChampions_EventScript_BattleSetSpecialItem:
	msgbox EmeraldChampions_Text_BattleSetSpecialItem, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_BattleSetChooseMon

EmeraldChampions_EventScript_MoveTutorExit:
	msgbox EmeraldChampions_Text_MoveTutorGoodbye, MSGBOX_DEFAULT
	releaseall
	end

Common_EventScript_EmeraldChampionsBattleVendor::
	lock
	faceplayer
	setvar VAR_0x8008, 0
	setvar VAR_0x8009, 0

EmeraldChampions_EventScript_BattleVendorMain:
	@ Let the prior native list window finish clearing before another menu can
	@ reuse the same BG tiles and briefly erase its frame.
	delay 1
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleVendorCompleteArchive
	message EmeraldChampions_Text_BattleVendorWelcome
	waitmessage
	goto EmeraldChampions_EventScript_BattleItemCategoryChoices

EmeraldChampions_EventScript_BattleVendorCompleteArchive:
	message EmeraldChampions_Text_BattleVendorCompleteWelcome
	waitmessage
	dynmultipush EmeraldChampions_Text_HeldItems, 0
	dynmultipush EmeraldChampions_Text_MegaArchive, 1
	dynmultipush EmeraldChampions_Text_EvolutionArchive, 2
	dynmultipush EmeraldChampions_Text_Exit, 3
	dynmultistack 30, 1, FALSE, 4, 0, VAR_0x8009, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_BattleVendorExit
	copyvar VAR_0x8009, VAR_RESULT
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_BattleItemCategories
	case 1, EmeraldChampions_EventScript_OpenMegaArchive
	case 2, EmeraldChampions_EventScript_OpenEvolutionArchive
	goto EmeraldChampions_EventScript_BattleVendorExit

EmeraldChampions_EventScript_BattleItemCategories:
	delay 1
	message EmeraldChampions_Text_BattleItemCategoryPrompt
	waitmessage

EmeraldChampions_EventScript_BattleItemCategoryChoices:
	dynmultipush EmeraldChampions_Text_OffenseItems, EC_BATTLE_ITEM_CATEGORY_OFFENSE
	dynmultipush EmeraldChampions_Text_DefenseItems, EC_BATTLE_ITEM_CATEGORY_DEFENSE
	dynmultipush EmeraldChampions_Text_FieldItems, EC_BATTLE_ITEM_CATEGORY_FIELD
	dynmultipush EmeraldChampions_Text_TypeItems, EC_BATTLE_ITEM_CATEGORY_TYPE
	dynmultipush EmeraldChampions_Text_GemItems, EC_BATTLE_ITEM_CATEGORY_GEM
	dynmultipush EmeraldChampions_Text_SpeciesItems, EC_BATTLE_ITEM_CATEGORY_SPECIES
	dynmultipush EmeraldChampions_Text_Back, EC_BATTLE_ITEM_CATEGORY_COUNT
	@ Four visible rows keep the lower scroll arrow comfortably above the
	@ native message box while preserving the category cursor between visits.
	dynmultistack 30, 1, FALSE, 4, 0, VAR_0x8008, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_BattleItemCategoryBack
	copyvar VAR_0x8008, VAR_RESULT
	goto_if_ge VAR_RESULT, EC_BATTLE_ITEM_CATEGORY_COUNT, EmeraldChampions_EventScript_BattleItemCategoryBack
	copyvar VAR_0x8004, VAR_RESULT
	special OpenEmeraldChampionsBattleItemMart
	goto EmeraldChampions_EventScript_BattleItemCategories

EmeraldChampions_EventScript_BattleItemCategoryBack:
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleVendorMain
	goto EmeraldChampions_EventScript_BattleVendorExit

EmeraldChampions_EventScript_OpenMegaArchive:
	special OpenEmeraldChampionsMegaStoneArchive
	goto EmeraldChampions_EventScript_BattleVendorMain

EmeraldChampions_EventScript_OpenEvolutionArchive:
	special OpenEmeraldChampionsEvolutionItemArchive
	goto EmeraldChampions_EventScript_BattleVendorMain

EmeraldChampions_EventScript_BattleVendorExit:
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

EmeraldChampions_Text_BattleVendorWelcome:
	.string "Competitive held items are free here.\p"
	.string "Take as many as your teams need!\p"
	.string "Which kind of held item do you need?$"

EmeraldChampions_Text_BattleVendorCompleteWelcome:
	.string "A MEGA RING! Then the Mega Stone and\n"
	.string "Evolution archives are open to you.\p"
	.string "Every stone, every item, free.\n"
	.string "What does your team need?$"

EmeraldChampions_Text_HeldItems:
	.string "Held Items$"

EmeraldChampions_Text_MegaArchive:
	.string "Mega Stone Archive$"

EmeraldChampions_Text_EvolutionArchive:
	.string "Evolution Items$"

EmeraldChampions_Text_BattleItemCategoryPrompt:
	.string "Which kind of held item do you need?$"

EmeraldChampions_Text_OffenseItems:
	.string "Offense$"

EmeraldChampions_Text_DefenseItems:
	.string "Defense & Recovery$"

EmeraldChampions_Text_FieldItems:
	.string "Field & Tempo$"

EmeraldChampions_Text_TypeItems:
	.string "Type Boosts$"

EmeraldChampions_Text_GemItems:
	.string "Gems$"

EmeraldChampions_Text_SpeciesItems:
	.string "Species Items$"

EmeraldChampions_Text_Back:
	.string "Back$"

EmeraldChampions_Text_MoveTutorHowCanHelp:
	.string "How can I help prepare your Pokémon?$"

EmeraldChampions_Text_BuildBattleSet:
	.string "Build a Battle Set$"

EmeraldChampions_Text_EditStatPoints:
	.string "Edit Stat Points$"

EmeraldChampions_Text_LearnMove:
	.string "Learn a Move$"

EmeraldChampions_Text_OtherServices:
	.string "Other Services$"

EmeraldChampions_Text_OtherServicesPrompt:
	.string "What else can I help with?$"

EmeraldChampions_Text_ForgetMove:
	.string "Forget a Move$"

EmeraldChampions_Text_RenamePokemon:
	.string "Rename Pokémon$"

EmeraldChampions_Text_Exit:
	.string "Exit$"

EmeraldChampions_Text_ExplainBattleSets:
	.string "A Battle Set rebuilds a Pokémon\n"
	.string "from top to bottom: its moves,\l"
	.string "Nature, Ability, Stat Points,\l"
	.string "and held item.$"

EmeraldChampions_Text_ChooseBattleSetMon:
	.string "Which Pokémon should I prepare?$"

EmeraldChampions_Text_ChooseBattleFormat:
	.string "Which battle format are you\n"
	.string "preparing for?$"

EmeraldChampions_Text_Doubles:
	.string "Doubles$"

EmeraldChampions_Text_Singles:
	.string "Singles$"

EmeraldChampions_Text_ChooseStatPointMon:
	.string "Choose a Pokémon whose Stat Points\n"
	.string "you want to customize.$"

EmeraldChampions_Text_ResetStatPoints:
	.string "Reset all six Stat Points to zero?$"

EmeraldChampions_Text_StatPointsReset:
	.string "All Stat Points were reset.\n"
	.string "You can now build a new spread.$"

EmeraldChampions_Text_StatPointEgg:
	.string "An Egg's Stat Points can't be edited.$"

EmeraldChampions_Text_StatPointGoodbye:
	.string "Come back whenever you want to\n"
	.string "customize another spread.$"

EmeraldChampions_Text_ChooseDoublesBattleSetStyle:
	.string "Which Doubles set should I build?$"

EmeraldChampions_Text_ChooseSinglesBattleSetStyle:
	.string "Which Singles set should I build?$"

EmeraldChampions_Text_ConfirmBattleSet:
	.string "Apply {STR_VAR_2} to\n"
	.string "{STR_VAR_1}?$"

EmeraldChampions_Text_ConfirmMegaBattleSet:
	.string "Required stone:\n"
	.string "{STR_VAR_3}.\p"
	.string "Apply this set without it?$"

EmeraldChampions_Text_BattleSetSuccess:
	.string "{STR_VAR_1}'s set is ready!\n"
	.string "Its full build was updated.$"

EmeraldChampions_Text_MegaBattleSetSuccess:
	.string "{STR_VAR_1}'s Mega set is ready!\p"
	.string "No stone was supplied.\n"
	.string "Required: {STR_VAR_3}.$"

EmeraldChampions_Text_MegaBattleSetHeldSuccess:
	.string "{STR_VAR_1}'s Mega set is ready!\n"
	.string "It kept its {STR_VAR_3}.$"

EmeraldChampions_Text_PrepareAnotherBattleSet:
	.string "Prepare another Pokémon?$"

EmeraldChampions_Text_BattleSetEgg:
	.string "An Egg can't use a battle set.$"

EmeraldChampions_Text_BattleSetMissing:
	.string "I don't have a finished battle set\n"
	.string "for that Pokémon yet.$"

EmeraldChampions_Text_BattleSetSpecialItem:
	.string "I won't discard that special item.\p"
	.string "Remove it before changing sets.$"

EmeraldChampions_Text_ForgetAnotherMove:
	.string "Should another move be forgotten?$"

EmeraldChampions_Text_RenameAnotherPokemon:
	.string "Rename another Pokémon?$"

EmeraldChampions_Text_MoveTutorGoodbye:
	.string "Come back whenever you want to\n"
	.string "change your team.$"

EmeraldChampions_Text_NeedTwoPokemon:
	.string "This is a Double Battle. Come back\n"
	.string "with two usable Pokémon.$"

Common_EventScript_ChooseStarterRegion::
	setvar VAR_0x8005, 0

EmeraldChampions_EventScript_ChooseStarterRegionMenu:
	msgbox EmeraldChampions_Text_ChooseStarterRegion, MSGBOX_DEFAULT
	setvar VAR_0x8004, SCROLL_MULTI_STARTER_REGIONS
	special ShowScrollableMultichoice
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_ChooseStarterRegionMenu
	copyvar VAR_0x8005, VAR_RESULT
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_StarterKanto
	case 1, EmeraldChampions_EventScript_StarterJohto
	case 2, EmeraldChampions_EventScript_StarterHoenn
	case 3, EmeraldChampions_EventScript_StarterSinnoh
	case 4, EmeraldChampions_EventScript_StarterUnova
	case 5, EmeraldChampions_EventScript_StarterKalos
	case 6, EmeraldChampions_EventScript_StarterAlola
	case 7, EmeraldChampions_EventScript_StarterGalar
	case 8, EmeraldChampions_EventScript_StarterPaldea
	goto EmeraldChampions_EventScript_ChooseStarterRegionMenu

EmeraldChampions_EventScript_StarterKanto:
	setvar VAR_STARTER_GEN, 1
	bufferstring 1, EmeraldChampions_Text_StarterRegionKanto
	return

EmeraldChampions_EventScript_StarterJohto:
	setvar VAR_STARTER_GEN, 2
	bufferstring 1, EmeraldChampions_Text_StarterRegionJohto
	return

EmeraldChampions_EventScript_StarterHoenn:
	setvar VAR_STARTER_GEN, 3
	bufferstring 1, EmeraldChampions_Text_StarterRegionHoenn
	return

EmeraldChampions_EventScript_StarterSinnoh:
	setvar VAR_STARTER_GEN, 4
	bufferstring 1, EmeraldChampions_Text_StarterRegionSinnoh
	return

EmeraldChampions_EventScript_StarterUnova:
	setvar VAR_STARTER_GEN, 5
	bufferstring 1, EmeraldChampions_Text_StarterRegionUnova
	return

EmeraldChampions_EventScript_StarterKalos:
	setvar VAR_STARTER_GEN, 6
	bufferstring 1, EmeraldChampions_Text_StarterRegionKalos
	return

EmeraldChampions_EventScript_StarterAlola:
	setvar VAR_STARTER_GEN, 7
	bufferstring 1, EmeraldChampions_Text_StarterRegionAlola
	return

EmeraldChampions_EventScript_StarterGalar:
	setvar VAR_STARTER_GEN, 8
	bufferstring 1, EmeraldChampions_Text_StarterRegionGalar
	return

EmeraldChampions_EventScript_StarterPaldea:
	setvar VAR_STARTER_GEN, 9
	bufferstring 1, EmeraldChampions_Text_StarterRegionPaldea
	return

EmeraldChampions_Text_ChooseStarterRegion:
	.string "Dad studies how battling traditions\n"
	.string "differ across every region.\p"
	.string "Which starter trio speaks to you?$"

EmeraldChampions_Text_StarterRegionChosen:
	.string "{STR_VAR_1}! Great choice!\p"
	.string "Dad packed that trio in his field bag.\n"
	.string "Whichever partner you choose, I'll train\l"
	.string "the one with the type advantage.$"

EmeraldChampions_Text_StarterRegionKanto:
	.string "Kanto$"
EmeraldChampions_Text_StarterRegionJohto:
	.string "Johto$"
EmeraldChampions_Text_StarterRegionHoenn:
	.string "Hoenn$"
EmeraldChampions_Text_StarterRegionSinnoh:
	.string "Sinnoh$"
EmeraldChampions_Text_StarterRegionUnova:
	.string "Unova$"
EmeraldChampions_Text_StarterRegionKalos:
	.string "Kalos$"
EmeraldChampions_Text_StarterRegionAlola:
	.string "Alola$"
EmeraldChampions_Text_StarterRegionGalar:
	.string "Galar$"
EmeraldChampions_Text_StarterRegionPaldea:
	.string "Paldea$"

EmeraldChampions_Text_ChangeNature:
	.string "Change Nature$"

EmeraldChampions_Text_ChooseNatureMon:
	.string "Whose Nature should I change?$"

EmeraldChampions_Text_CurrentNature:
	.string "Nature: {STR_VAR_2}.\nWhich Nature would you like?$"

EmeraldChampions_Text_NatureChanged:
	.string "{STR_VAR_1}'s Nature is now\n{STR_VAR_2}. Stats recalculated.$"

EmeraldChampions_Text_ChangeAnotherNature:
	.string "Change another Nature?$"

EmeraldChampions_Text_NatureEgg:
	.string "An Egg's Nature can't be changed.$"

EmeraldChampions_Text_SpecialistForgetMove:
	.string "Forgetting a move clears a slot for\n"
	.string "something your team needs more.\p"
	.string "Shall I help a POKéMON forget one?$"

@ Emerald Champions: the Repel Spray ran out of steps (see UpdateRepelCounter).
EmeraldChampions_EventScript_RepelSprayWoreOff::
	lockall
	msgbox EmeraldChampions_Text_RepelSprayWoreOffUseAgain, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EmeraldChampions_EventScript_RepelSprayReuse
	msgbox EmeraldChampions_Text_RepelSprayLeftOff, MSGBOX_DEFAULT
	closemessage
	releaseall
	end

EmeraldChampions_EventScript_RepelSprayReuse::
	setflag FLAG_EC_REPEL_SPRAY_ACTIVE
	setvar VAR_EC_REPEL_SPRAY_STEPS, 500
	playse SE_REPEL
	waitse
	msgbox EmeraldChampions_Text_RepelSprayMistedAgain, MSGBOX_DEFAULT
	closemessage
	releaseall
	end

EmeraldChampions_Text_RepelSprayWoreOffUseAgain:
	.string "The Repel Spray wore off.\n"
	.string "Use it again?$"

EmeraldChampions_Text_RepelSprayLeftOff:
	.string "The grass stirs. Wild Pokémon are\n"
	.string "coming back.$"

EmeraldChampions_Text_RepelSprayMistedAgain:
	.string "{PLAYER} misted the air again.\p"
	.string "Wild Pokémon will keep their distance\n"
	.string "for the next 500 steps.$"
