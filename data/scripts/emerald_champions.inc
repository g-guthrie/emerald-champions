Common_EventScript_EmeraldChampionsMoveTutor::
	lockall
	faceplayer
	setvar VAR_0x8009, 0
	call_if_unset FLAG_EC_TUTOR_MACHINES_LINE_SEEN, EmeraldChampions_EventScript_MoveTutorNoMachines

EmeraldChampions_EventScript_MoveTutorMain:
	message EmeraldChampions_Text_MoveTutorHowCanHelp
	waitmessage
	dynmultipush EmeraldChampions_Text_LearnMove, 0
	dynmultipush EmeraldChampions_Text_OtherServices, 1
	dynmultipush EmeraldChampions_Text_Exit, 2
	dynmultistack 30, 1, FALSE, 3, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_OpenMoveRelearner
	case 1, EmeraldChampions_EventScript_OtherServices
	goto EmeraldChampions_EventScript_MoveTutorExit

EmeraldChampions_EventScript_MoveTutorNoMachines:
	setflag FLAG_EC_TUTOR_MACHINES_LINE_SEEN
	msgbox EmeraldChampions_Text_MoveTutorNoMachines, MSGBOX_DEFAULT
	return

EmeraldChampions_EventScript_OtherServices:
	message EmeraldChampions_Text_OtherServicesPrompt
	waitmessage
	dynmultipush EmeraldChampions_Text_Bonding, 0
	dynmultipush EmeraldChampions_Text_Back, 1
	dynmultistack 30, 1, FALSE, 2, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_SpecialistBonding
	case 1, EmeraldChampions_EventScript_MoveTutorMain
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


EmeraldChampions_EventScript_MoveTutorExit:
	msgbox EmeraldChampions_Text_MoveTutorGoodbye, MSGBOX_DEFAULT
	releaseall
	end

Common_EventScript_EmeraldChampionsBattleVendor::
	lock
	faceplayer
	setvar VAR_0x8008, 0
	setvar VAR_0x8009, 0

@ The five held items a competitive set cannot function without are handed over
@ once, free. Everything else on his shelves is bought.
EmeraldChampions_EventScript_BattleVendorStarterKit:
	goto_if_set FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS, EmeraldChampions_EventScript_BattleVendorMain
	special GiveEmeraldChampionsStarterBattleItems
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_StarterItemsBagFull
	msgbox EmeraldChampions_Text_BattleVendorStarterKit, MSGBOX_DEFAULT

	goto EmeraldChampions_EventScript_BattleVendorMain

EmeraldChampions_EventScript_StarterItemsBagFull:
	msgbox EmeraldChampions_Text_StarterItemsBagFull, MSGBOX_DEFAULT
	release
	end

EmeraldChampions_Text_StarterItemsBagFull:
	.string "Make room in your Bag and come back.\n"
	.string "I'll keep the rest of your kit here.$"

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
	dynmultipush EmeraldChampions_Text_EvolutionArchive, 1
	dynmultipush EmeraldChampions_Text_Exit, 2
	dynmultistack 30, 1, FALSE, 4, 0, VAR_0x8009, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_BattleVendorExit
	copyvar VAR_0x8009, VAR_RESULT
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_BattleItemCategories
	case 1, EmeraldChampions_EventScript_OpenEvolutionArchive
	goto EmeraldChampions_EventScript_BattleVendorExit

EmeraldChampions_EventScript_BattleItemCategories:
EmeraldChampions_EventScript_BattleItemCategoriesReady:
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
	dynmultipush EmeraldChampions_Text_Berries, EC_BATTLE_ITEM_CATEGORY_BERRIES
	dynmultipush EmeraldChampions_Text_Back, 8
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

EmeraldChampions_EventScript_OpenEvolutionArchive:
	special OpenEmeraldChampionsEvolutionItemArchive
	goto EmeraldChampions_EventScript_BattleVendorMain

EmeraldChampions_EventScript_BattleVendorExit:
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

EmeraldChampions_Text_BattleVendorStarterKit:
	.string "Before you shop, take these.\p"
	.string "A CHOICE BAND, CHOICE SPECS and\n"
	.string "CHOICE SCARF, a FOCUS SASH and\l"
	.string "an EVIOLITE.\p"
	.string "These will help you get started.\n"
	.string "The rest you buy like anyone else.$"

EmeraldChampions_Text_BattleVendorWelcome:
	.string "I stock the held items you discover.\p"
	.string "Buy extra copies for your team here.\p"
	.string "Which kind of held item do you need?$"

EmeraldChampions_Text_BattleVendorCompleteWelcome:
	.string "Your RING opens the Form Tools archive.\n"
	.string "Tools are free; held equipment is sold\lafter you catch its Pokémon.\p"
	.string "Evolution specialists sell stones and\n"
	.string "devices. LILYCOVE has the full range.\p"
	.string "For Mega Stones, explore HOENN and\n"
	.string "get to know the people who live here.$"

EmeraldChampions_Text_EvolutionArchive:
	.string "Form Tools$"

EmeraldChampions_Text_HeldItems:
	.string "Held Items$"

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

EmeraldChampions_Text_MoveTutorHowCanHelp:
	.string "How can I help prepare your Pokémon?$"

EmeraldChampions_Text_MoveTutorNoMachines:
	.string "There's no need for machines any\n"
	.string "more.\p"
	.string "Bring me any Pokémon and I'll teach\n"
	.string "it anything it can legally learn.$"

EmeraldChampions_Text_LearnMove:
	.string "Learn a Move$"

EmeraldChampions_Text_OtherServices:
	.string "Other Services$"

EmeraldChampions_Text_OtherServicesPrompt:
	.string "What else can I help with?$"

EmeraldChampions_Text_Exit:
	.string "Exit$"

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
	.string "Choose two partners. I'll train\l"
	.string "the partner you leave with Dad.$"

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


@ These continuations run under their caller's lock and return to that caller.
Common_EventScript_EmeraldChampionsDeoxysResearch::
	goto_if_unset FLAG_EC_REPORT_C43_COMPLETE, EmeraldChampions_EventScript_ReturnService
	msgbox EmeraldChampions_Text_DeoxysResearch, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_ReturnService
	setvar VAR_0x8006, 2
	goto EmeraldChampions_EventScript_FormChooseMon

EmeraldChampions_EventScript_FormChooseMon:
	msgbox EmeraldChampions_Text_FormChooseMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_ReturnService
	copyvar VAR_0x800A, VAR_0x8004
	special BuildEmeraldChampionsFormChoices
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_FormUnavailable
	message EmeraldChampions_Text_FormChooseTarget
	waitmessage
	dynmultipush EmeraldChampions_Text_Back, 0
	dynmultistack 30, 1, FALSE, 5, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_FormChooseMon
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_FormChooseMon
	copyvar VAR_0x8008, VAR_RESULT
	special PrepareEmeraldChampionsFormSelection
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_FormChooseMon
	goto_if_eq VAR_RESULT, 2, EmeraldChampions_EventScript_CostumeChooseMove
	goto EmeraldChampions_EventScript_FormPreview

EmeraldChampions_EventScript_CostumeChooseMove:
	msgbox EmeraldChampions_Text_WhichMoveShouldBeForgotten, MSGBOX_DEFAULT
	copyvar VAR_0x8004, VAR_0x800A
	fadescreen FADE_TO_BLACK
	special MoveDeleterChooseMoveToForget
	fadescreen FADE_FROM_BLACK
	goto_if_eq VAR_0x8005, MAX_MON_MOVES, EmeraldChampions_EventScript_FormChooseMon

EmeraldChampions_EventScript_FormPreview:
	special BufferEmeraldChampionsFormPreview
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_FormBlocked
	msgbox gStringVar4, MSGBOX_DEFAULT
	msgbox EmeraldChampions_Text_ConfirmFormChange, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_FormChooseMon
	special ApplyEmeraldChampionsFormSelection
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_FormBlocked
	playfanfare MUS_LEVEL_UP
	waitfanfare
	msgbox EmeraldChampions_Text_FormChanged, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_FormChooseMon

EmeraldChampions_EventScript_FormBlocked:
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_FormChooseMon

EmeraldChampions_EventScript_FormUnavailable:
	msgbox EmeraldChampions_Text_FormUnavailable, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_FormChooseMon

EmeraldChampions_EventScript_SpecialistBonding:
	call Common_EventScript_EmeraldChampionsBonding
	goto EmeraldChampions_EventScript_OtherServices

Common_EventScript_EmeraldChampionsBonding::
	msgbox EmeraldChampions_Text_BondingChooseMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_ReturnService
	copyvar VAR_0x800A, VAR_0x8004
	setvar VAR_0x8008, 0
	special BufferEmeraldChampionsBondingPreview
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_BondingUnavailable
	message EmeraldChampions_Text_BondingValues
	waitmessage
	dynmultipush EmeraldChampions_Text_BondingLow, 0
	dynmultipush EmeraldChampions_Text_BondingThreshold, 1
	dynmultipush EmeraldChampions_Text_BondingMaximum, 2
	dynmultipush EmeraldChampions_Text_Back, 3
	dynmultistack 30, 1, FALSE, 4, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_ge VAR_RESULT, 3, Common_EventScript_EmeraldChampionsBonding
	copyvar VAR_0x8008, VAR_RESULT
	special BufferEmeraldChampionsBondingPreview
	msgbox gStringVar4, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Common_EventScript_EmeraldChampionsBonding
	special ApplyEmeraldChampionsBonding
	msgbox EmeraldChampions_Text_BondingDone, MSGBOX_DEFAULT
	goto Common_EventScript_EmeraldChampionsBonding

EmeraldChampions_EventScript_BondingUnavailable:
	msgbox EmeraldChampions_Text_BondingUnavailable, MSGBOX_DEFAULT
	goto Common_EventScript_EmeraldChampionsBonding

EmeraldChampions_EventScript_ReturnService:
	return

EmeraldChampions_EventScript_BattleBondInfo:
	msgbox EmeraldChampions_Text_BattleBondInfo, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_PartnerInfo:
	msgbox EmeraldChampions_Text_PartnerInfo, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_KubfuInfo:
	msgbox EmeraldChampions_Text_KubfuInfo, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_CosmogInfo:
	msgbox EmeraldChampions_Text_CosmogInfo, MSGBOX_DEFAULT
	return

EmeraldChampions_Text_FormGiftOffer:
	.string "Would you like this prepared\n{STR_VAR_2} to join you?$"
EmeraldChampions_Text_FormGiftParty:
	.string "Your new partner joined your party!$"
EmeraldChampions_Text_FormGiftPC:
	.string "Your new partner was sent to the PC!$"
EmeraldChampions_Text_FormGiftFull:
	.string "There's no room in your party or\n"
	.string "PC. I'll keep your partner here\l"
	.string "until a space opens up.$"
EmeraldChampions_Text_ResearchPartners:
	.string "Which research partner interests you?$"
EmeraldChampions_Text_BattleBondInfo:
	.string "Battle Bond boosts Attack, Sp. Atk\nand Speed once after a knockout.\pThis GRENINJA keeps its usual form.$"
EmeraldChampions_Text_PartnerInfo:
	.string "This special partner has enhanced\nstats and cannot evolve.$"
EmeraldChampions_Text_KubfuInfo:
	.string "A Dark Scroll and a Water Scroll\nlead KUBFU to different styles.\pThis second partner lets you explore\nboth paths with your original KUBFU.$"
EmeraldChampions_Text_CosmogInfo:
	.string "COSMOEM evolves into SOLGALEO by\n"
	.string "day or LUNALA at night.\p"
	.string "This second COSMOG lets you\n"
	.string "explore both paths. Use your\l"
	.string "Leveler when ready.$"
EmeraldChampions_Text_StylistServices:
	.string "Would you like help with your\npartner's appearance or costume?$"
EmeraldChampions_Text_Appearance:
	.string "Appearance$"
EmeraldChampions_Text_CosplayPartner:
	.string "Cosplay partner$"
EmeraldChampions_Text_CostumeLessons:
	.string "Costume lessons$"
EmeraldChampions_Text_AppearanceInfo:
	.string "I style matching colors and trims\nwithout changing your partner's build.\pFURFROU trims return to normal with\ntime or storage. Fresh trims are free.\pMINIOR keeps its meteor shell here;\nits matching core appears in battle.$"
EmeraldChampions_Text_DeoxysResearch:
	.string "Would you like to study your DEOXYS\nwith our form-changing equipment?$"
EmeraldChampions_Text_FormChooseMon:
	.string "Which partner would you like to study?$"
EmeraldChampions_Text_FormChooseTarget:
	.string "Which appearance or form?$"
EmeraldChampions_Text_WhichMoveShouldBeForgotten:
	.string "Which move should be forgotten?$"
EmeraldChampions_Text_ConfirmFormChange:
	.string "Make this change?$"
EmeraldChampions_Text_FormChanged:
	.string "Your partner's new look is ready!$"
EmeraldChampions_Text_FormUnavailable:
	.string "That partner isn't part of this\nservice. Choose another Pokémon.$"
EmeraldChampions_Text_Bonding:
	.string "Bonding$"
EmeraldChampions_Text_BondingChooseMon:
	.string "Whose friendship would you like\nto adjust?$"
EmeraldChampions_Text_BondingValues:
	.string "Choose a friendship value.$"
EmeraldChampions_Text_BondingLow:
	.string "Low (0)$"
EmeraldChampions_Text_BondingThreshold:
	.string "Evolution threshold$"
EmeraldChampions_Text_BondingMaximum:
	.string "Maximum (255)$"
EmeraldChampions_Text_BondingDone:
	.string "Friendship is set. Your Leveler can\nhelp when evolution conditions are met.$"
EmeraldChampions_Text_BondingUnavailable:
	.string "Please choose a Pokémon, not an Egg.$"

Common_EventScript_CenterLocalGuide::
	lockall
	faceplayer
	msgbox EmeraldChampions_Text_CenterLocalGuide, MSGBOX_DEFAULT
	setvar VAR_0x8004, 0

EmeraldChampions_EventScript_CenterLocalGuideNext::
	special BufferNextCenterLegendaryLead
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_CenterLocalGuideEnd
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_CenterLocalGuideNext

EmeraldChampions_EventScript_CenterLocalGuideEnd::
	releaseall
	end

EmeraldChampions_Text_CenterLocalGuide:
	.string "I listen for stories about rare\n"
	.string "POKéMON around here. Let me tell\l"
	.string "you where to look!$"

EmeraldChampions_EventScript_UseHarvestPouch::
	lockall
	setvar VAR_0x8009, 0
	call EmeraldChampions_EventScript_HarvestMenu
	releaseall
	end

EmeraldChampions_EventScript_HarvestMenu::
	message EmeraldChampions_Text_HarvestWelcome
	waitmessage
	dynmultipush EmeraldChampions_Text_MyHarvest, 0
	dynmultipush EmeraldChampions_Text_HarvestRequests, 1
	dynmultipush EmeraldChampions_Text_HarvestHelpChoice, 2
	dynmultipush EmeraldChampions_Text_Back, 3
	dynmultistack 30, 1, FALSE, 4, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_HarvestListStart
	goto_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_HarvestRequests
	goto_if_eq VAR_RESULT, 2, EmeraldChampions_EventScript_HarvestHelp
	return

EmeraldChampions_EventScript_HarvestListStart:
	setvar VAR_0x800A, 1
EmeraldChampions_EventScript_HarvestList:
	message EmeraldChampions_Text_HarvestList
	waitmessage
	special BuildEmeraldChampionsHarvestChoices
	dynmultipush EmeraldChampions_Text_Back, 0
	dynmultistack 30, 1, FALSE, 4, 0, VAR_0x800A, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_HarvestMenu
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_HarvestMenu
	copyvar VAR_0x800A, VAR_RESULT
	goto EmeraldChampions_EventScript_HarvestList

EmeraldChampions_EventScript_HarvestRequests:
	message EmeraldChampions_Text_HarvestRequestsPrompt
	waitmessage
	dynmultipush EmeraldChampions_Text_HarvestBaxcalibrite, 0
	dynmultipush EmeraldChampions_Text_HarvestDragoninite, 1
	dynmultipush EmeraldChampions_Text_HarvestTyranitarite, 2
	dynmultipush EmeraldChampions_Text_HarvestCelebi, 3
	dynmultipush EmeraldChampions_Text_Back, 4
	dynmultistack 30, 1, FALSE, 4, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_ge VAR_RESULT, 4, EmeraldChampions_EventScript_HarvestMenu
	copyvar VAR_0x8004, VAR_RESULT
	special BufferEmeraldChampionsHarvestRecipe
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto_if_eq VAR_0x8009, 0, EmeraldChampions_EventScript_HarvestRequests
	goto_if_eq VAR_0x8004, 3, EmeraldChampions_EventScript_HarvestCelebiCheck
	goto_if_eq VAR_0x800B, 1, EmeraldChampions_EventScript_HarvestAlready
EmeraldChampions_EventScript_HarvestConfirm:
	msgbox EmeraldChampions_Text_HarvestConfirm, MSGBOX_YESNO
	goto_if_ne VAR_RESULT, YES, EmeraldChampions_EventScript_HarvestRequests
	special TradeEmeraldChampionsGardenBerries
	goto_if_eq VAR_RESULT, EC_MEGA_BERRY_TRADE_ALREADY_DONE, EmeraldChampions_EventScript_HarvestAlready
	goto_if_eq VAR_RESULT, EC_MEGA_BERRY_TRADE_NOT_ENOUGH, EmeraldChampions_EventScript_HarvestNotEnough
	goto_if_eq VAR_RESULT, EC_MEGA_BERRY_TRADE_BAG_FULL, EmeraldChampions_EventScript_HarvestBagFull
	goto_if_ne VAR_RESULT, EC_MEGA_BERRY_TRADE_SUCCESS, EmeraldChampions_EventScript_HarvestRequests
	playfanfare MUS_OBTAIN_ITEM
	msgbox EmeraldChampions_Text_HarvestReceived, MSGBOX_DEFAULT
	waitfanfare
	goto_if_eq VAR_0x8004, 3, EmeraldChampions_EventScript_HarvestCelebiCheck
	goto EmeraldChampions_EventScript_HarvestRequests

EmeraldChampions_EventScript_HarvestCelebiCheck:
	special CheckEmeraldChampionsGardenCelebi
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_HarvestConfirm
	goto_if_eq VAR_RESULT, 2, EmeraldChampions_EventScript_HarvestAlready
	msgbox EmeraldChampions_Text_HarvestMeetCelebi, MSGBOX_YESNO
	goto_if_ne VAR_RESULT, YES, EmeraldChampions_EventScript_HarvestRequests
	setvar VAR_0x8004, SPECIES_CELEBI
	setvar VAR_0x8005, 0
	special CreateEmeraldChampionsStaticLegendaryEncounter
	dowildbattle
	waitstate
	goto EmeraldChampions_EventScript_HarvestMenu

EmeraldChampions_EventScript_HarvestAlready:
	msgbox EmeraldChampions_Text_HarvestAlready, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_HarvestRequests
EmeraldChampions_EventScript_HarvestNotEnough:
	msgbox EmeraldChampions_Text_HarvestNotEnough, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_HarvestRequests
EmeraldChampions_EventScript_HarvestBagFull:
	msgbox EmeraldChampions_Text_HarvestBagFull, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_HarvestRequests
EmeraldChampions_EventScript_HarvestHelp:
	msgbox EmeraldChampions_Text_HarvestHelp, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_HarvestMenu

EmeraldChampions_Text_HarvestWelcome:
	.string "Your harvest pouch keeps picked\n"
	.string "berries separate from battle supplies.$"
EmeraldChampions_Text_MyHarvest:
	.string "My harvest$"
EmeraldChampions_Text_HarvestRequests:
	.string "Reward requests$"
EmeraldChampions_Text_HarvestHelpChoice:
	.string "How it works$"
EmeraldChampions_Text_HarvestList:
	.string "HARVESTED BERRIES - by type\n"
	.string "Only picking a tree adds to this pouch.$"
EmeraldChampions_Text_HarvestRequestsPrompt:
	.string "The BERRY MASTER on ROUTE 123\n"
	.string "offers these harvest rewards.$"
EmeraldChampions_Text_HarvestBaxcalibrite:
	.string "Baxcalibrite$"
EmeraldChampions_Text_HarvestDragoninite:
	.string "Dragoninite$"
EmeraldChampions_Text_HarvestTyranitarite:
	.string "Tyranitarite$"
EmeraldChampions_Text_HarvestCelebi:
	.string "Celebi's invitation$"
EmeraldChampions_Text_HarvestConfirm:
	.string "Trade those harvested berries for\n"
	.string "{STR_VAR_1}?$"
EmeraldChampions_Text_HarvestReceived:
	.string "You received {STR_VAR_1}!\n"
	.string "Your battle berries are untouched.$"
EmeraldChampions_Text_HarvestAlready:
	.string "That reward is already yours.\n"
	.string "I'll keep your harvest untouched.$"
EmeraldChampions_Text_HarvestNotEnough:
	.string "You still need some of that harvest.\n"
	.string "Pick as you travel, or plant the gaps!$"
EmeraldChampions_Text_HarvestBagFull:
	.string "Make room for the stone in your BAG.\n"
	.string "Your harvested berries are safe.$"
EmeraldChampions_Text_HarvestMeetCelebi:
	.string "CELEBI visits to share your harvest!\n"
	.string "Would you like to meet it now?\p"
	.string "Bring your POKé BALLS. You can try\n"
	.string "again without another berry payment.$"
EmeraldChampions_Text_HarvestHelp:
	.string "Every berry is free from the start\n"
	.string "at the Center's held-item counter.\p"
	.string "Equip them for battle or plant seeds.\n"
	.string "They aren't medicine to use by hand.\p"
	.string "Pick three LUM BERRIES: you get three\n"
	.string "battle berries AND three harvested LUM.\p"
	.string "Each type has its own harvest count.\n"
	.string "Only picking trees fills this pouch.\p"
	.string "Growing takes at most four hours.\n"
	.string "Ripe crops stay until you pick them.\p"
	.string "Watering improves your yield. Check\n"
	.string "requests and plant what you're missing!$"

@ Both berries are already delivered; show the normal receipt without adding again.
Common_EventScript_ShowDeliveredBerry::
	copyvar VAR_0x8006, VAR_0x8000
	setvar VAR_0x8001, 1
	setvar VAR_0x8007, TRUE
	call EventScript_ObtainItemMessage
	return

@ The existing three puzzle receipts own the route discovery; capture is optional.
EmeraldChampions_EventScript_ThreeRuinsProgress::
	goto_if_unset FLAG_REGI_DOORS_OPENED, EmeraldChampions_EventScript_RuinsNeedChamber
	goto_if_unset FLAG_SYS_REGIROCK_PUZZLE_COMPLETED, EmeraldChampions_EventScript_NextDesertRuin
	goto_if_unset FLAG_SYS_BRAILLE_REGICE_COMPLETED, EmeraldChampions_EventScript_NextIslandRuin
	goto_if_unset FLAG_SYS_REGISTEEL_PUZZLE_COMPLETED, EmeraldChampions_EventScript_NextAncientRuin
	goto_if_unset FLAG_EC_RESOLVED_REGIROCK, EmeraldChampions_EventScript_GuardianDesert
	goto_if_unset FLAG_EC_RESOLVED_REGICE, EmeraldChampions_EventScript_GuardianIsland
	goto_if_unset FLAG_EC_RESOLVED_REGISTEEL, EmeraldChampions_EventScript_GuardianAncient
	goto_if_set FLAG_EC_REPORT_C45_COMPLETE, EmeraldChampions_EventScript_RuinsRouteKnown
	completemilestone FLAG_EC_REPORT_C45_COMPLETE
	buffermilestonetext FLAG_EC_REPORT_C45_COMPLETE
	goto_if_set FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, EmeraldChampions_EventScript_ThreeRuinsCompleteConfirmed
	msgbox EmeraldChampions_Text_ThreeRuinsComplete, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_ThreeRuinsReward
EmeraldChampions_EventScript_ThreeRuinsCompleteConfirmed::
	msgbox EmeraldChampions_Text_ThreeRuinsCompleteConfirmed, MSGBOX_DEFAULT
EmeraldChampions_EventScript_ThreeRuinsReward::
	call_if_eq VAR_RESULT, 3, EmeraldChampions_EventScript_ThreeRuinsReportAndCapLine
	call_if_eq VAR_RESULT, 2, EmeraldChampions_EventScript_ThreeRuinsReportLine
	call_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_PrintMilestoneCapLine
	goto EmeraldChampions_EventScript_RuinsRouteKnown
EmeraldChampions_EventScript_ThreeRuinsReportAndCapLine::
	msgbox EmeraldChampions_Text_ThreeRuinsReportAndCapLine, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_ThreeRuinsReportLine::
	msgbox EmeraldChampions_Text_ThreeRuinsReportLine, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_RuinsRouteKnown::
	goto_if_set FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, EmeraldChampions_EventScript_ThreeRuinsLoreConfirmed
	msgbox EmeraldChampions_Text_ThreeRuinsLore, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_ThreeRuinsLoreConfirmed::
	msgbox EmeraldChampions_Text_ThreeRuinsLoreConfirmed, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_RuinsNeedChamber::
	msgbox SealedChamber_InnerRoom_Text_ResearchPending, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_NextDesertRuin::
	msgbox EmeraldChampions_Text_NextDesertRuin, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_NextIslandRuin::
	msgbox EmeraldChampions_Text_NextIslandRuin, MSGBOX_DEFAULT
	return
EmeraldChampions_EventScript_NextAncientRuin::
	msgbox EmeraldChampions_Text_NextAncientRuin, MSGBOX_DEFAULT
	return
SealedChamber_InnerRoom_Text_ResearchPending:
	.string "The marks describe three records:\n"
	.string "the DESERT safeguards, CASTFORM's\l"
	.string "weather, and STERN's seabed scan.\p"
	.string "Complete those research reports\n"
	.string "to read the builders' message.$"
EmeraldChampions_Text_NextDesertRuin:
	.string "Next, read the DESERT RUINS\n"
	.string "account in ROUTE 111's desert.\l"
	.string "Its puzzle uses ROCK SMASH.$"
EmeraldChampions_Text_NextIslandRuin:
	.string "Next, SURF to ISLAND CAVE on the\n"
	.string "west side of ROUTE 105, north of\l"
	.string "DEWFORD. Read its rear wall.$"
EmeraldChampions_Text_NextAncientRuin:
	.string "Next, visit ANCIENT TOMB on\n"
	.string "ROUTE 120. Read the rear wall;\l"
	.string "its puzzle uses FLASH.$"
EmeraldChampions_Text_ThreeRuinsComplete:
	.string "The three guardians have answered!\p"
	.string "Together, their accounts tell of an\n"
	.string "age when the land and sea powers\l"
	.string "were feared beyond reckoning, until\l"
	.string "a guardian of the sky was called to\l"
	.string "hold them apart.$"
EmeraldChampions_Text_ThreeRuinsCompleteConfirmed:
	.string "The three guardians have answered!\p"
	.string "Together, their accounts tell of an\n"
	.string "age when the land and sea powers\l"
	.string "were feared beyond reckoning, until\l"
	.string "a guardian of the sky was called to\l"
	.string "hold them apart - just as you saw\l"
	.string "at SOOTOPOLIS.$"
EmeraldChampions_Text_ThreeRuinsReportAndCapLine:
	.string "Your discovery earns ¥{STR_VAR_2}.\n"
	.string "Your level cap is now {STR_VAR_1}.$"
EmeraldChampions_Text_ThreeRuinsReportLine:
	.string "Your discovery earns ¥{STR_VAR_2}.$"
EmeraldChampions_Text_ThreeRuinsLore:
	.string "The three accounts agree: long ago\n"
	.string "the land and sea powers were feared\l"
	.string "beyond reckoning, until a guardian\l"
	.string "of the sky was invoked to keep\l"
	.string "them apart.\p"
	.string "A fourth mark repeats across all\n"
	.string "three - REGIGIGAS, said to have\l"
	.string "mustered the others together.$"
EmeraldChampions_Text_ThreeRuinsLoreConfirmed:
	.string "The three accounts agree: long ago\n"
	.string "the land and sea powers were feared\l"
	.string "beyond reckoning, until a guardian\l"
	.string "of the sky was invoked to keep\l"
	.string "them apart - the same balance you\l"
	.string "restored at SOOTOPOLIS.\p"
	.string "A fourth mark repeats across all\n"
	.string "three - REGIGIGAS, said to have\l"
	.string "mustered the others together.$"

EmeraldChampions_EventScript_GuardianDesert::
	msgbox EmeraldChampions_Text_GuardianDesert, MSGBOX_DEFAULT
	return
EmeraldChampions_Text_GuardianDesert:
	.string "REGIROCK still waits in\n"
	.string "DESERT RUINS on ROUTE 111.\p"
	.string "Win the battle or catch it to\n"
	.string "complete this discovery. Keeping\l"
	.string "it is your choice.$"

EmeraldChampions_EventScript_GuardianIsland::
	msgbox EmeraldChampions_Text_GuardianIsland, MSGBOX_DEFAULT
	return
EmeraldChampions_Text_GuardianIsland:
	.string "REGICE still waits in\n"
	.string "ISLAND CAVE on ROUTE 105.\p"
	.string "Win the battle or catch it to\n"
	.string "complete this discovery. Keeping\l"
	.string "it is your choice.$"

EmeraldChampions_EventScript_GuardianAncient::
	msgbox EmeraldChampions_Text_GuardianAncient, MSGBOX_DEFAULT
	return
EmeraldChampions_Text_GuardianAncient:
	.string "REGISTEEL still waits in\n"
	.string "ANCIENT TOMB on ROUTE 120.\p"
	.string "Win the battle or catch it to\n"
	.string "complete this discovery. Keeping\l"
	.string "it is your choice.$"

@ Story prerequisites block the next event, never ordinary return travel.

EC_RequireDewford::
	goto_if_set FLAG_EC_REPORT_C14_COMPLETE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireDewford, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireDewford:
	.string "Finish the manor account and\n"
	.string "perform the meadow song on DEWFORD.\p"
	.string "STEVEN in GRANITE CAVE gives the\n"
	.string "MEGA RING after his letter and\l"
	.string "BRAWLY's badge.$"

EC_RequireMuseum::
	goto_if_set FLAG_DELIVERED_DEVON_GOODS, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireMuseum, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireMuseum:
	.string "Take the DEVON GOODS to STERN in\n"
	.string "SLATEPORT's OCEANIC MUSEUM.\l"
	.string "Resolve the AQUA attack there.$"

EC_RequireWally::
	goto_if_set FLAG_DEFEATED_WALLY_MAUVILLE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireWally, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireWally:
	.string "WALLY waits outside MAUVILLE GYM.\n"
	.string "Accept and finish his challenge\l"
	.string "before facing WATTSON.$"

EC_RequireChimney::
	goto_if_set FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireChimney, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireChimney:
	.string "Follow the METEOR FALLS theft to\n"
	.string "MT. CHIMNEY. Take ROUTE 112's\l"
	.string "cable car and stop MAXIE.$"

EC_RequireDesert::
	goto_if_set FLAG_EC_REPORT_C26_COMPLETE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireDesert, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireDesert:
	.string "Finish the SANDSTREWN investigation\n"
	.string "beneath MIRAGE TOWER on ROUTE 111.\p"
	.string "Explore the lowest chamber, then\n"
	.string "return to the archaeologist above.\l"
	.string "Catching its POKéMON is optional.$"

EC_RequireGenerator::
	goto_if_set FLAG_EC_REPORT_C28_COMPLETE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireGenerator, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireGenerator:
	.string "Resolve ROTOM at NEW MAUVILLE,\n"
	.string "then speak to WATTSON in town.\l"
	.string "The entrance is on ROUTE 110.$"

EC_RequireSurvey::
	goto_if_set FLAG_EC_REPORT_C30_COMPLETE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireSurvey, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireSurvey:
	.string "Bring CASTFORM to the WEATHER\n"
	.string "INSTITUTE on ROUTE 119. Take its\l"
	.string "survey trip and resolve LANDORUS.$"

EC_RequireScope::
	goto_if_set FLAG_RECEIVED_DEVON_SCOPE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireScope, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireScope:
	.string "Meet STEVEN on ROUTE 120, east of\n"
	.string "FORTREE. Resolve the invisible\l"
	.string "obstacle and collect the DEVON SCOPE.$"

EC_RequireFeather::
	goto_if_set FLAG_BADGE06_GET, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireFeather, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireFeather:
	.string "Challenge WINONA in FORTREE GYM\n"
	.string "before the expedition departs.$"

EC_RequireVoyage::
	goto_if_set FLAG_EC_REPORT_C36_COMPLETE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireVoyage, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireVoyage:
	.string "Sail from LILYCOVE HARBOR to\n"
	.string "SOUTHERN ISLAND, then FARAWAY.\p"
	.string "Resolve the sanctuary encounter\n"
	.string "and MEW's challenge. Capturing\l"
	.string "either POKéMON is optional.$"

EC_RequireHeatran::
	goto_if_set FLAG_EC_REPORT_C39_COMPLETE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireHeatran, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireHeatran:
	.string "Investigate HEATRAN in SCORCHED\n"
	.string "SLAB on ROUTE 120. Bring the\l"
	.string "MAGMA STONE from MAGMA HIDEOUT.$"

EC_RequireHideout::
	goto_if_set FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireHideout, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireHideout:
	.string "Follow the stolen submarine to\n"
	.string "AQUA HIDEOUT, northeast of\l"
	.string "LILYCOVE. Defeat its final guard.$"

EC_RequireMind::
	goto_if_set FLAG_BADGE07_GET, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireMind, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireMind:
	.string "Challenge TATE and LIZA in\n"
	.string "MOSSDEEP GYM, then meet STEVEN\l"
	.string "at the SPACE CENTER.$"

EC_RequireSpace::
	goto_if_set FLAG_EC_REPORT_C42_COMPLETE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireSpace, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireSpace:
	.string "Help STEVEN stop MAGMA at the\n"
	.string "SPACE CENTER in eastern MOSSDEEP.\l"
	.string "Then finish his travel briefing.$"

EC_RequireCrisis::
	goto_if_set FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireCrisis, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireCrisis:
	.string "Meet STEVEN and WALLACE at the\n"
	.string "CAVE OF ORIGIN. Follow their lead\l"
	.string "to RAYQUAZA atop SKY PILLAR.$"

EC_RequireFinalBadge::
	goto_if_set FLAG_BADGE08_GET, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireFinalBadge, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireFinalBadge:
	.string "Finish the SOOTOPOLIS crisis,\n"
	.string "then challenge JUAN for the\l"
	.string "RAIN BADGE.$"

EC_RequireFinalConclusion::
	goto_if_set FLAG_EC_REPORT_C48_COMPLETE, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireFinalConclusion, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireFinalConclusion:
	.string "Resolve DIANCIE in the CAVE OF\n"
	.string "ORIGIN and JIRACHI in METEOR FALLS.\p"
	.string "Then meet STEVEN at his house in\n"
	.string "MOSSDEEP to conclude the research.\l"
	.string "Catching either is optional.$"

EC_RequireLilycoveRival::
	goto_if_defeated TRAINER_MAY_LILYCOVE_TREECKO, Common_EventScript_NopReturn
	goto_if_defeated TRAINER_MAY_LILYCOVE_TORCHIC, Common_EventScript_NopReturn
	goto_if_defeated TRAINER_MAY_LILYCOVE_MUDKIP, Common_EventScript_NopReturn
	goto_if_defeated TRAINER_BRENDAN_LILYCOVE_TREECKO, Common_EventScript_NopReturn
	goto_if_defeated TRAINER_BRENDAN_LILYCOVE_TORCHIC, Common_EventScript_NopReturn
	goto_if_defeated TRAINER_BRENDAN_LILYCOVE_MUDKIP, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireLilycoveRival, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireLilycoveRival:
	.string "Your rival waits by LILYCOVE's\n"
	.string "DEPARTMENT STORE. Finish that\l"
	.string "challenge before this expedition.$"

EC_RequireGroudon::
	goto_if_set FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, Common_EventScript_NopReturn
	lockall
	msgbox EC_Text_RequireGroudon, MSGBOX_DEFAULT
	releaseall
	end
EC_Text_RequireGroudon:
	.string "Follow MAXIE into MAGMA HIDEOUT\n"
	.string "on JAGGED PASS. Stop his plan\l"
	.string "before investigating HEATRAN.$"

EC_DeliverTravelDocuments::
	setvar VAR_0x8009, 0
	call_if_set FLAG_EC_EARNED_SS_TICKET, EC_Deliver_SS_TICKET
	call_if_set FLAG_EC_EARNED_EON_TICKET, EC_Deliver_EON_TICKET
	call_if_set FLAG_EC_EARNED_OLD_SEA_MAP, EC_Deliver_OLD_SEA_MAP
	call_if_set FLAG_EC_EARNED_AURORA_TICKET, EC_Deliver_AURORA_TICKET
	call_if_set FLAG_ENABLE_SHIP_NAVEL_ROCK, EC_Deliver_MYSTIC_TICKET
	call_if_eq VAR_0x8009, 1, EC_TravelDocumentsPending
	return

EC_Deliver_SS_TICKET::
	goto_if_set FLAG_RECEIVED_SS_TICKET, Common_EventScript_NopReturn
	setvar VAR_0x8004, ITEM_SS_TICKET
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, TRUE, EC_Received_SS_TICKET
	checkitemspace ITEM_SS_TICKET, 1
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
	giveitem ITEM_SS_TICKET
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
EC_Received_SS_TICKET::
	setflag FLAG_RECEIVED_SS_TICKET
	return

EC_Deliver_EON_TICKET::
	goto_if_set FLAG_EC_RECEIVED_EON_TICKET, Common_EventScript_NopReturn
	setvar VAR_0x8004, ITEM_EON_TICKET
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, TRUE, EC_Received_EON_TICKET
	checkitemspace ITEM_EON_TICKET, 1
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
	giveitem ITEM_EON_TICKET
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
EC_Received_EON_TICKET::
	setflag FLAG_EC_RECEIVED_EON_TICKET
	return

EC_Deliver_OLD_SEA_MAP::
	goto_if_set FLAG_RECEIVED_OLD_SEA_MAP, Common_EventScript_NopReturn
	setvar VAR_0x8004, ITEM_OLD_SEA_MAP
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, TRUE, EC_Received_OLD_SEA_MAP
	checkitemspace ITEM_OLD_SEA_MAP, 1
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
	giveitem ITEM_OLD_SEA_MAP
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
EC_Received_OLD_SEA_MAP::
	setflag FLAG_RECEIVED_OLD_SEA_MAP
	return

EC_Deliver_AURORA_TICKET::
	goto_if_set FLAG_RECEIVED_AURORA_TICKET, Common_EventScript_NopReturn
	setvar VAR_0x8004, ITEM_AURORA_TICKET
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, TRUE, EC_Received_AURORA_TICKET
	checkitemspace ITEM_AURORA_TICKET, 1
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
	giveitem ITEM_AURORA_TICKET
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
EC_Received_AURORA_TICKET::
	setflag FLAG_RECEIVED_AURORA_TICKET
	return

EC_Deliver_MYSTIC_TICKET::
	goto_if_set FLAG_RECEIVED_MYSTIC_TICKET, Common_EventScript_NopReturn
	setvar VAR_0x8004, ITEM_MYSTIC_TICKET
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, TRUE, EC_Received_MYSTIC_TICKET
	checkitemspace ITEM_MYSTIC_TICKET, 1
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
	giveitem ITEM_MYSTIC_TICKET
	goto_if_eq VAR_RESULT, FALSE, EC_TravelDocumentFull
EC_Received_MYSTIC_TICKET::
	setflag FLAG_RECEIVED_MYSTIC_TICKET
	return

EC_TravelDocumentFull::
	setvar VAR_0x8009, 1
	return
EC_TravelDocumentsPending::
	msgbox EC_Text_TravelDocumentsPending, MSGBOX_DEFAULT
	return
EC_Text_TravelDocumentsPending:
	.string "Your travel papers are waiting.\n"
	.string "Make room in the KEY ITEMS pocket,\l"
	.string "then speak to any CENTER nurse.\p"
	.string "Your passage is already approved.\n"
	.string "You can sail from LILYCOVE HARBOR\l"
	.string "while we hold the papers for you.$"

EmeraldChampions_Text_ResearchChallengeComplete:
	.string "You've already met its challenge.\n"
	.string "Battle again to try catching it?$"

EmeraldChampions_EventScript_GuardianVictory::
	special HideRestingLegendaryObject
	call Common_EventScript_LegendaryVictoryReminder
	call EmeraldChampions_EventScript_ThreeRuinsProgress
	releaseall
	end

@ Shared follow-up line for milestone dialogue. Call `buffermilestonetext`
@ (or `completemilestone`, which buffers VAR_0x8003 too) immediately before
@ these so VAR_RESULT and STR_VAR_1 are populated for the flag just handled.
@ Only the cap sentence is common across NPCs; stipend phrasing stays local
@ to each script since every reporter describes their payout differently.
EmeraldChampions_EventScript_PrintMilestoneCapLine::
	msgbox EmeraldChampions_Text_MilestoneCapLine, MSGBOX_DEFAULT
	return

EmeraldChampions_Text_MilestoneCapLine:
	.string "Your level cap is now {STR_VAR_1}.$"

@ Every berry gift mints one Harvest Pouch credit. `giverandomberry` calls this
@ straight after its giveitem, so VAR_RESULT still holds the delivery result
@ and VAR_0x8000 still holds the berry that was handed over.
EmeraldChampions_EventScript_MintBerryHarvest::
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_MintBerryHarvestDone
	special MintEmeraldChampionsHarvestCredit
EmeraldChampions_EventScript_MintBerryHarvestDone:
	return

EmeraldChampions_Text_Back:
	.string "Back$"

EmeraldChampions_Text_Berries:
	.string "Berries$"
