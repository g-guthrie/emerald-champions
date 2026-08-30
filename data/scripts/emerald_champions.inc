Common_EventScript_EmeraldChampionsMoveTutor::
	lockall
	faceplayer
	message EmeraldChampions_Text_MoveTutorHowCanHelp
	waitmessage
	dynmultipush EmeraldChampions_Text_BuildBattleSet, 0
	dynmultipush EmeraldChampions_Text_LearnMove, 1
	dynmultipush EmeraldChampions_Text_ForgetMove, 2
	dynmultipush EmeraldChampions_Text_RenamePokemon, 3
	dynmultipush EmeraldChampions_Text_Exit, 4
	dynmultistack 0, 0, FALSE, 5, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_BattleSetChooseMon
	case 1, EmeraldChampions_EventScript_OpenMoveRelearner
	case 2, EmeraldChampions_EventScript_OpenMoveDeleter
	case 3, EmeraldChampions_EventScript_OpenNameRater
	case 4, EmeraldChampions_EventScript_MoveTutorExit
	case MULTI_B_PRESSED, EmeraldChampions_EventScript_MoveTutorExit

EmeraldChampions_EventScript_OpenMoveRelearner:
	releaseall
	goto Common_EventScript_MoveRelearner

EmeraldChampions_EventScript_OpenMoveDeleter:
	msgbox LilycoveCity_MoveDeletersHouse_Text_ICanMakeMonForgetMove, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, LilycoveCity_MoveDeletersHouse_EventScript_ChooseMonAndMoveToForget
	goto LilycoveCity_MoveDeletersHouse_EventScript_ComeAgain

EmeraldChampions_EventScript_OpenNameRater:
	releaseall
	goto SlateportCity_NameRatersHouse_EventScript_NameRater

EmeraldChampions_EventScript_BattleSetChooseMon:
	msgbox EmeraldChampions_Text_ChooseBattleSetMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_MoveTutorExit
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleSetEgg
	copyvar VAR_0x800A, VAR_0x8004
	special GetSelectedMonEmeraldChampionsBattleSetCount
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_BattleSetMissing
	goto_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_BattleSetUseDefault

EmeraldChampions_EventScript_BattleSetChooseStyle:
	special BufferSelectedMonCurrentEmeraldChampionsBattleSet
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleSetChooseCurrentStyle
	msgbox EmeraldChampions_Text_ChooseBattleSetStyle, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_BattleSetOpenStyleList

EmeraldChampions_EventScript_BattleSetChooseCurrentStyle:
	msgbox EmeraldChampions_Text_ChooseCurrentBattleSetStyle, MSGBOX_DEFAULT

EmeraldChampions_EventScript_BattleSetOpenStyleList:
	setvar VAR_0x8004, SCROLL_MULTI_EMERALD_CHAMPIONS_BATTLE_SET
	special ShowScrollableMultichoice
	copyvar VAR_0x8006, VAR_RESULT
	special GetSelectedMonEmeraldChampionsBattleSetCount
	goto_if_ge VAR_0x8006, VAR_RESULT, EmeraldChampions_EventScript_BattleSetChooseMon
	copyvar VAR_0x8005, VAR_0x8006
	goto EmeraldChampions_EventScript_BattleSetConfirm

EmeraldChampions_EventScript_BattleSetUseDefault:
	setvar VAR_0x8005, 0

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
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_BattleSetChooseStyle
	special ApplySelectedMonEmeraldChampionsBattleSet
	goto_if_eq VAR_RESULT, EC_BATTLE_SET_SPECIAL_ITEM_EQUIPPED, EmeraldChampions_EventScript_BattleSetSpecialItem
	goto_if_eq VAR_RESULT, EC_BATTLE_SET_MEGA, EmeraldChampions_EventScript_BattleSetMegaSuccess
	goto_if_ne VAR_RESULT, EC_BATTLE_SET_SUCCESS, EmeraldChampions_EventScript_BattleSetMissing
	playfanfare MUS_LEVEL_UP
	msgbox EmeraldChampions_Text_BattleSetSuccess, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_PrepareAnotherBattleSet

EmeraldChampions_EventScript_BattleSetMegaSuccess:
	playfanfare MUS_LEVEL_UP
	msgbox EmeraldChampions_Text_MegaBattleSetSuccess, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_PrepareAnotherBattleSet

EmeraldChampions_EventScript_PrepareAnotherBattleSet:
	msgbox EmeraldChampions_Text_PrepareAnotherBattleSet, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, EmeraldChampions_EventScript_BattleSetChooseMon
	releaseall
	end

EmeraldChampions_EventScript_BattleSetEgg:
	msgbox EmeraldChampions_Text_BattleSetEgg, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_BattleSetChooseMon

EmeraldChampions_EventScript_BattleSetMissing:
	msgbox EmeraldChampions_Text_BattleSetMissing, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_BattleSetChooseMon

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
	goto_if_set FLAG_BADGE08_GET, EmeraldChampions_EventScript_BattleVendorCompleteArchive
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
	dynmultistack 0, 0, FALSE, 4, 0, VAR_0x8009, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_BattleVendorExit
	copyvar VAR_0x8009, VAR_RESULT
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_BattleItemCategories
	case 1, EmeraldChampions_EventScript_OpenMegaArchive
	case 2, EmeraldChampions_EventScript_OpenEvolutionArchive
	goto EmeraldChampions_EventScript_BattleVendorExit

EmeraldChampions_EventScript_BattleItemCategories:
	message EmeraldChampions_Text_BattleItemCategoryPrompt
	waitmessage

EmeraldChampions_EventScript_BattleItemCategoryChoices:
	dynmultipush EmeraldChampions_Text_OffenseItems, 0
	dynmultipush EmeraldChampions_Text_DefenseItems, 1
	dynmultipush EmeraldChampions_Text_FieldItems, 2
	dynmultipush EmeraldChampions_Text_TypeItems, 3
	dynmultipush EmeraldChampions_Text_GemItems, 4
	dynmultipush EmeraldChampions_Text_SpeciesItems, 5
	dynmultipush EmeraldChampions_Text_Back, 6
	dynmultistack 0, 0, FALSE, 5, 0, VAR_0x8008, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_BattleItemCategoryBack
	copyvar VAR_0x8008, VAR_RESULT
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_OpenOffenseItems
	case 1, EmeraldChampions_EventScript_OpenDefenseItems
	case 2, EmeraldChampions_EventScript_OpenFieldItems
	case 3, EmeraldChampions_EventScript_OpenTypeItems
	case 4, EmeraldChampions_EventScript_OpenGemItems
	case 5, EmeraldChampions_EventScript_OpenSpeciesItems
	goto EmeraldChampions_EventScript_BattleItemCategoryBack

EmeraldChampions_EventScript_BattleItemCategoryBack:
	goto_if_set FLAG_BADGE08_GET, EmeraldChampions_EventScript_BattleVendorMain
	goto EmeraldChampions_EventScript_BattleVendorExit

EmeraldChampions_EventScript_OpenOffenseItems:
	special OpenEmeraldChampionsOffenseItemMart
	goto EmeraldChampions_EventScript_BattleItemCategories

EmeraldChampions_EventScript_OpenDefenseItems:
	special OpenEmeraldChampionsDefenseItemMart
	goto EmeraldChampions_EventScript_BattleItemCategories

EmeraldChampions_EventScript_OpenFieldItems:
	special OpenEmeraldChampionsFieldItemMart
	goto EmeraldChampions_EventScript_BattleItemCategories

EmeraldChampions_EventScript_OpenTypeItems:
	special OpenEmeraldChampionsTypeItemMart
	goto EmeraldChampions_EventScript_BattleItemCategories

EmeraldChampions_EventScript_OpenGemItems:
	special OpenEmeraldChampionsGemItemMart
	goto EmeraldChampions_EventScript_BattleItemCategories

EmeraldChampions_EventScript_OpenSpeciesItems:
	special OpenEmeraldChampionsSpeciesItemMart
	goto EmeraldChampions_EventScript_BattleItemCategories

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
	.string "All eight BADGES! The complete Mega\n"
	.string "archive is open before the LEAGUE.\p"
	.string "What does your team need?$"

EmeraldChampions_Text_HeldItems:
	.string "Battle Items$"

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

EmeraldChampions_Text_LearnMove:
	.string "Learn a Move$"

EmeraldChampions_Text_ForgetMove:
	.string "Forget a Move$"

EmeraldChampions_Text_RenamePokemon:
	.string "Rename Pokémon$"

EmeraldChampions_Text_Exit:
	.string "Exit$"

EmeraldChampions_Text_ChooseBattleSetMon:
	.string "Choose a Pokémon for a complete\n"
	.string "competitive battle set.$"

EmeraldChampions_Text_ChooseBattleSetStyle:
	.string "Which battle style should I build?$"

EmeraldChampions_Text_ChooseCurrentBattleSetStyle:
	.string "{STR_VAR_1}'s current set:\n"
	.string "{STR_VAR_2}.\p"
	.string "Which battle style should I build?$"

EmeraldChampions_Text_ConfirmBattleSet:
	.string "Apply the {STR_VAR_2} set to\n"
	.string "{STR_VAR_1}?$"

EmeraldChampions_Text_ConfirmMegaBattleSet:
	.string "Required stone:\n"
	.string "{STR_VAR_3}.\p"
	.string "Apply this set without it?$"

EmeraldChampions_Text_BattleSetSuccess:
	.string "{STR_VAR_1} is ready with the\n"
	.string "{STR_VAR_2} set!$"

EmeraldChampions_Text_MegaBattleSetSuccess:
	.string "{STR_VAR_1} is ready for the\n"
	.string "{STR_VAR_2} role with {STR_VAR_3}.$"

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
