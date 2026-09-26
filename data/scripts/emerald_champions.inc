Common_EventScript_EmeraldChampionsMoveTutor::
	lockall
	faceplayer
	setvar VAR_TUTOR_CURSOR, 0
	call_if_unset FLAG_EC_TUTOR_MACHINES_LINE_SEEN, EmeraldChampions_EventScript_MoveTutorNoMachines

@ One menu; VAR_TUTOR_CURSOR keeps the cursor on the service last used. The
@ relearner and Bonding never touch it (include/constants/service_vars.h).
EmeraldChampions_EventScript_MoveTutorMain:
	message EmeraldChampions_Text_MoveTutorHowCanHelp
	waitmessage
	dynmultipush EmeraldChampions_Text_LearnMove, 0
	dynmultipush EmeraldChampions_Text_Bonding, 1
	dynmultipush EmeraldChampions_Text_EVTraining, 2
	dynmultipush EmeraldChampions_Text_Exit, 3
	dynmultistack 30, 1, FALSE, 4, 0, VAR_TUTOR_CURSOR, DYN_MULTICHOICE_CB_NONE
	closemessage
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_OpenMoveRelearner
	case 1, EmeraldChampions_EventScript_SpecialistBonding
	case 2, EmeraldChampions_EventScript_EVTraining
	goto EmeraldChampions_EventScript_MoveTutorExit

EmeraldChampions_EventScript_MoveTutorNoMachines:
	setflag FLAG_EC_TUTOR_MACHINES_LINE_SEEN
	msgbox EmeraldChampions_Text_MoveTutorNoMachines, MSGBOX_DEFAULT
	return

@ The party screen and the move list carry their own prompts and open each
@ other directly (ChooseMonForMoveRelearnerDirect): a Pokémon with moves to
@ relearn goes to the list, and B in the list returns to the party screen.
@ The script resumes only when the party screen closes: B returns to the
@ tutor's menu; an Egg or a Pokémon with nothing to relearn gets its message.
EmeraldChampions_EventScript_OpenMoveRelearner:
	setvar VAR_TUTOR_CURSOR, 0
	setmoverelearnerstate MOVE_RELEARNER_ALL_MOVES
	bufferstring STR_VAR_3, MoveRelearner_Text_MoveLWR

EmeraldChampions_EventScript_MoveRelearnerChooseMon:
	getpartysize
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_MoveRelearnerNoPokemon
	chooseboxmon SELECT_PC_MON_MOVE_RELEARNER_DIRECT
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_MoveTutorMain
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_MoveRelearnerEgg
	special Special_HasMoveToRelearn
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_MoveRelearnerNoMoves
	@ Only a pick from the PC storage screen (OW_CHOOSE_FROM_PC_AND_PARTY)
	@ reaches the list from here: the Pokémon in VAR_0x8004, or
	@ PC_MON_CHOSEN with the box position. Waits until the list closes.
	special TeachMoveRelearnerMove
	goto EmeraldChampions_EventScript_MoveRelearnerChooseMon

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

@ EV training: pick a Pokémon, plan its whole spread in the editor, then
@ confirm to apply it at once for EV_PLAN_FEE (src/inclement_stat_services.c
@ keeps the plan). B steps back one level; leaving the editor with changes
@ asks first. Nothing is applied or charged until the player confirms.
EmeraldChampions_EventScript_EVTraining:
	setvar VAR_TUTOR_CURSOR, 2
	msgbox EmeraldChampions_Text_EVTrainingIntro, MSGBOX_DEFAULT
EmeraldChampions_EventScript_EVTrainingChooseMon:
	special ChoosePartyMon
	goto_if_ge VAR_0x8004, PARTY_SIZE, EmeraldChampions_EventScript_MoveTutorMain
	copyvar VAR_EV_PLAN_MON, VAR_0x8004
	special StartPlannedEVSpread
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_EVTrainingEgg
	setvar VAR_EV_PLAN_CURSOR, 0
	setvar VAR_EV_PLAN_STEP, 0

@ The editor: every stat as "now > planned", then Undo Changes, Clear All
@ and Confirm.
EmeraldChampions_EventScript_EVPlan:
	bufferpartymonnick STR_VAR_1, VAR_EV_PLAN_MON
	special BufferPlannedEVTotal
	message EmeraldChampions_Text_EVPlanPrompt
	waitmessage
	@ Let the previous list window finish clearing before this one reuses
	@ its BG tiles (see General_Mart_ServiceMenu).
	delay 1
	setvar VAR_0x8005, 0
	special BufferPlannedEVRow
	dynmultipush EmeraldChampions_Text_EVRow, 0
	setvar VAR_0x8005, 1
	special BufferPlannedEVRow
	dynmultipush EmeraldChampions_Text_EVRow, 1
	setvar VAR_0x8005, 2
	special BufferPlannedEVRow
	dynmultipush EmeraldChampions_Text_EVRow, 2
	setvar VAR_0x8005, 3
	special BufferPlannedEVRow
	dynmultipush EmeraldChampions_Text_EVRow, 3
	setvar VAR_0x8005, 4
	special BufferPlannedEVRow
	dynmultipush EmeraldChampions_Text_EVRow, 4
	setvar VAR_0x8005, 5
	special BufferPlannedEVRow
	dynmultipush EmeraldChampions_Text_EVRow, 5
	dynmultipush EmeraldChampions_Text_EVPlanUndo, 6
	dynmultipush EmeraldChampions_Text_EVPlanClearAll, 7
	dynmultipush EmeraldChampions_Text_EVPlanConfirm, 8
	dynmultistack 0, 0, FALSE, 6, 0, VAR_EV_PLAN_CURSOR, DYN_MULTICHOICE_CB_NONE
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_EVPlanBack
	copyvar VAR_EV_PLAN_CURSOR, VAR_RESULT
	goto_if_eq VAR_RESULT, 8, EmeraldChampions_EventScript_EVPlanConfirm
	goto_if_eq VAR_RESULT, 7, EmeraldChampions_EventScript_EVPlanClearAll
	goto_if_eq VAR_RESULT, 6, EmeraldChampions_EventScript_EVPlanUndo

@ One stat: stay here, step by step, until B returns to the editor.
EmeraldChampions_EventScript_EVPlanStat:
	copyvar VAR_0x8005, VAR_EV_PLAN_CURSOR
	special BufferPlannedEVStatPrompt
	message gStringVar4
	waitmessage
	delay 1
	dynmultipush EmeraldChampions_Text_EVStepAdd4, EV_PLAN_STEP_ADD_4
	dynmultipush EmeraldChampions_Text_EVStepAdd64, EV_PLAN_STEP_ADD_64
	dynmultipush EmeraldChampions_Text_EVStepAddMax, EV_PLAN_STEP_ADD_MAX
	dynmultipush EmeraldChampions_Text_EVStepSub4, EV_PLAN_STEP_SUB_4
	dynmultipush EmeraldChampions_Text_EVStepSub64, EV_PLAN_STEP_SUB_64
	dynmultipush EmeraldChampions_Text_EVStepClear, EV_PLAN_STEP_CLEAR
	dynmultistack 0, 0, FALSE, 6, 0, VAR_EV_PLAN_STEP, DYN_MULTICHOICE_CB_NONE
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_EVPlan
	copyvar VAR_EV_PLAN_STEP, VAR_RESULT
	copyvar VAR_0x8006, VAR_RESULT
	copyvar VAR_0x8005, VAR_EV_PLAN_CURSOR
	special AdjustPlannedEV
	goto_if_eq VAR_RESULT, EV_PLAN_CHANGED, EmeraldChampions_EventScript_EVPlanStat
	goto_if_eq VAR_RESULT, EV_PLAN_STAT_FULL, EmeraldChampions_EventScript_EVPlanStatFull
	goto_if_eq VAR_RESULT, EV_PLAN_TOTAL_FULL, EmeraldChampions_EventScript_EVPlanTotalFull
	msgbox EmeraldChampions_Text_EVPlanStatEmpty, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_EVPlanStat

EmeraldChampions_EventScript_EVPlanStatFull:
	msgbox EmeraldChampions_Text_EVPlanStatFull, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_EVPlanStat

EmeraldChampions_EventScript_EVPlanTotalFull:
	msgbox EmeraldChampions_Text_EVPlanTotalFull, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_EVPlanStat

@ Undo Changes: back to the Pokémon's own spread; free, nothing applied.
EmeraldChampions_EventScript_EVPlanUndo:
	special UndoPlannedEVs
	msgbox EmeraldChampions_Text_EVPlanUndone, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_EVPlan

@ Clear All: plan 0 everywhere, applied through Confirm like any change.
EmeraldChampions_EventScript_EVPlanClearAll:
	special ClearPlannedEVs
	msgbox EmeraldChampions_Text_EVPlanClearedAll, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_EVPlan

EmeraldChampions_EventScript_EVPlanConfirm:
	special CheckPlannedEVSpreadChanged
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_EVPlanUnchanged
	@ The money box writes STR_VAR_1, so it opens before the name is buffered.
	showmoneybox 0, 0
	bufferpartymonnick STR_VAR_1, VAR_EV_PLAN_MON
	special BufferPlannedEVSummary
	msgbox EmeraldChampions_Text_EVPlanSummary, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_EVPlanDecline
	checkmoney EV_PLAN_FEE
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_EVPlanCantAfford
	special ApplyPlannedEVSpread
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_EVPlanDecline
	removemoney EV_PLAN_FEE
	updatemoneybox
	playse SE_SHOP
	bufferpartymonnick STR_VAR_1, VAR_EV_PLAN_MON
	msgbox EmeraldChampions_Text_EVPlanApplied, MSGBOX_DEFAULT
	hidemoneybox
	goto EmeraldChampions_EventScript_EVTrainingChooseMon

EmeraldChampions_EventScript_EVPlanDecline:
	hidemoneybox
	goto EmeraldChampions_EventScript_EVPlan

EmeraldChampions_EventScript_EVPlanCantAfford:
	msgbox EmeraldChampions_Text_EVPlanCantAfford, MSGBOX_DEFAULT
	hidemoneybox
	goto EmeraldChampions_EventScript_EVPlan

EmeraldChampions_EventScript_EVPlanUnchanged:
	msgbox EmeraldChampions_Text_EVPlanUnchanged, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_EVPlan

@ B in the editor: straight back to the party when nothing is pending.
EmeraldChampions_EventScript_EVPlanBack:
	special CheckPlannedEVSpreadChanged
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_EVTrainingChooseMon
	msgbox EmeraldChampions_Text_EVPlanDiscard, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_EVPlan
	goto EmeraldChampions_EventScript_EVTrainingChooseMon

EmeraldChampions_EventScript_EVTrainingEgg:
	msgbox EmeraldChampions_Text_EVTrainingEgg, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_EVTrainingChooseMon

EmeraldChampions_Text_EVTraining:
	.string "EV Training$"
EmeraldChampions_Text_EVTrainingIntro:
	.string "Choose a Pokémon and plan its EVs.\n"
	.string "A new spread costs ¥500 to apply.\p"
	.string "Undo Changes puts the plan back.\n"
	.string "Clear All plans 0 in every stat.$"
EmeraldChampions_Text_EVTrainingEgg:
	.string "An Egg can't be trained yet.$"
EmeraldChampions_Text_EVPlanPrompt:
	.string "{STR_VAR_1}: {STR_VAR_3} of 510 EVs planned.\n"
	.string "Each row shows now and planned.$"
EmeraldChampions_Text_EVRow:
	.string "{STR_VAR_2}$"
EmeraldChampions_Text_EVPlanUndo:
	.string "Undo Changes$"
EmeraldChampions_Text_EVPlanClearAll:
	.string "Clear All$"
EmeraldChampions_Text_EVPlanConfirm:
	.string "Confirm$"
EmeraldChampions_Text_EVStepAdd4:
	.string "+4$"
EmeraldChampions_Text_EVStepAdd64:
	.string "+64$"
EmeraldChampions_Text_EVStepAddMax:
	.string "Max$"
EmeraldChampions_Text_EVStepSub4:
	.string "-4$"
EmeraldChampions_Text_EVStepSub64:
	.string "-64$"
EmeraldChampions_Text_EVStepClear:
	.string "Clear$"
EmeraldChampions_Text_EVPlanStatFull:
	.string "That stat is at its limit of 252.$"
EmeraldChampions_Text_EVPlanTotalFull:
	.string "All 510 EVs are planned. Lower\n"
	.string "another stat to make room.$"
EmeraldChampions_Text_EVPlanStatEmpty:
	.string "That stat has no EVs to remove.$"
EmeraldChampions_Text_EVPlanSummary:
	.string "{STR_VAR_2}\n"
	.string "{STR_VAR_3}\p"
	.string "Apply this spread to {STR_VAR_1}\n"
	.string "for ¥500?$"
EmeraldChampions_Text_EVPlanCantAfford:
	.string "You need ¥500 for this training.\n"
	.string "Nothing has changed.$"
EmeraldChampions_Text_EVPlanApplied:
	.string "{STR_VAR_1}'s new EV spread is set!$"
EmeraldChampions_Text_EVPlanUnchanged:
	.string "That's the spread it already has.\n"
	.string "Change a stat first.$"
EmeraldChampions_Text_EVPlanUndone:
	.string "The plan is back to {STR_VAR_1}'s\n"
	.string "current spread. Nothing was charged.$"
EmeraldChampions_Text_EVPlanClearedAll:
	.string "Every stat is planned at 0.\n"
	.string "Confirm to remove all its EVs.$"
EmeraldChampions_Text_EVPlanDiscard:
	.string "Discard changes?$"

@ The five held items a competitive set cannot function without are handed over
@ once, free. Everything else on the clerk's shelves is bought.
@ A full Bag only postpones the kit; the counter's services stay open.
EmeraldChampions_EventScript_TryStarterKit::
	goto_if_set FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS, Common_EventScript_NopReturn
	special GiveEmeraldChampionsStarterBattleItems
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_StarterItemsBagFull
	msgbox EmeraldChampions_Text_BattleVendorStarterKit, MSGBOX_DEFAULT
	return

EmeraldChampions_EventScript_StarterItemsBagFull:
	msgbox EmeraldChampions_Text_StarterItemsBagFull, MSGBOX_DEFAULT
	return

EmeraldChampions_Text_StarterItemsBagFull:
	.string "Make room in your Bag and come back.\n"
	.string "I'll keep your kit here.$"

@ Held Items: pick a shelf, buy, and land back on the same shelf. B or Back
@ returns to the clerk's service menu.
EmeraldChampions_EventScript_HeldItems::
	goto_if_eq VAR_CLERK_HELD_ITEMS_GREETED, TRUE, EmeraldChampions_EventScript_BattleItemCategories
	setvar VAR_CLERK_HELD_ITEMS_GREETED, TRUE
	message EmeraldChampions_Text_BattleVendorWelcome
	waitmessage
	goto EmeraldChampions_EventScript_BattleItemCategoryChoices

EmeraldChampions_EventScript_BattleItemCategories:
	message EmeraldChampions_Text_BattleItemCategoryPrompt
	waitmessage
EmeraldChampions_EventScript_BattleItemCategoryChoices:
	delay 1
	dynmultipush EmeraldChampions_Text_OffenseItems, EC_BATTLE_ITEM_CATEGORY_OFFENSE
	dynmultipush EmeraldChampions_Text_DefenseItems, EC_BATTLE_ITEM_CATEGORY_DEFENSE
	dynmultipush EmeraldChampions_Text_FieldItems, EC_BATTLE_ITEM_CATEGORY_FIELD
	dynmultipush EmeraldChampions_Text_TypeItems, EC_BATTLE_ITEM_CATEGORY_TYPE
	dynmultipush EmeraldChampions_Text_GemItems, EC_BATTLE_ITEM_CATEGORY_GEM
	dynmultipush EmeraldChampions_Text_SpeciesItems, EC_BATTLE_ITEM_CATEGORY_SPECIES
	dynmultipush EmeraldChampions_Text_Back, EC_BATTLE_ITEM_CATEGORY_COUNT
	@ Five rows keep the lower scroll arrow clear of the message box.
	dynmultistack 30, 1, FALSE, 5, 0, VAR_CLERK_SHELF_CURSOR, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, General_Mart_AnythingElse
	goto_if_ge VAR_RESULT, EC_BATTLE_ITEM_CATEGORY_COUNT, General_Mart_AnythingElse
	copyvar VAR_CLERK_SHELF_CURSOR, VAR_RESULT
	copyvar VAR_0x8004, VAR_RESULT @ the shelf to stock
	special BufferEmeraldChampionsBattleItemStock
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_BattleItemCategoryEmpty
	special OpenEmeraldChampionsBattleItemMart
	waitstate
	goto EmeraldChampions_EventScript_BattleItemCategories

EmeraldChampions_EventScript_BattleItemCategoryEmpty:
	message EmeraldChampions_Text_BattleItemCategoryEmpty
	waitmessage
	waitbuttonpress
	goto EmeraldChampions_EventScript_BattleItemCategories

@ Form Items (Mega Ring holders): the explanation once per visit, then the list.
EmeraldChampions_EventScript_FormItems::
	goto_if_eq VAR_CLERK_FORM_ITEMS_GREETED, TRUE, EmeraldChampions_EventScript_OpenEvolutionArchive
	setvar VAR_CLERK_FORM_ITEMS_GREETED, TRUE
	msgbox EmeraldChampions_Text_BattleVendorCompleteWelcome, MSGBOX_DEFAULT
EmeraldChampions_EventScript_OpenEvolutionArchive:
	special OpenEmeraldChampionsEvolutionItemArchive
	waitstate
	goto General_Mart_AnythingElse

EmeraldChampions_Text_BattleVendorStarterKit:
	.string "Before you shop, take these.\p"
	.string "A Choice Band, Choice Specs,\n"
	.string "a Choice Scarf, a Focus Sash\l"
	.string "and an Eviolite.\p"
	.string "These will help you get started.\n"
	.string "The rest you buy like anyone else.$"

EmeraldChampions_Text_BattleVendorWelcome:
	.string "Find a held item on your journey,\n"
	.string "and I'll sell you more of it here.\p"
	.string "Which kind of held item do you need?$"

EmeraldChampions_Text_BattleVendorCompleteWelcome:
	.string "Oh, a Mega Ring! Then I can offer\n"
	.string "you form items, too.\p"
	.string "The tools are free. Items a Pokémon\n"
	.string "holds go on sale once you catch it.\p"
	.string "Some Poké Marts sell evolution stones.\n"
	.string "Lilycove Department Store has them all.\p"
	.string "As for Mega Stones, explore Hoenn\n"
	.string "and get to know the people here!$"

EmeraldChampions_Text_EvolutionArchive:
	.string "Form Items$"

EmeraldChampions_Text_HeldItems:
	.string "Held Items$"

EmeraldChampions_Text_BattleItemCategoryPrompt:
	.string "Which kind of held item do you need?$"

EmeraldChampions_Text_BattleItemCategoryEmpty:
	.string "None of those are in stock yet.\n"
	.string "Find one on your journey first!$"

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
	.string "No need for machines anymore!\p"
	.string "Bring me any Pokémon, and I'll teach\n"
	.string "it any move it can learn.$"

EmeraldChampions_Text_LearnMove:
	.string "Learn a Move$"

EmeraldChampions_Text_Exit:
	.string "Exit$"

EmeraldChampions_Text_MoveTutorGoodbye:
	.string "Come back whenever you want to\n"
	.string "change your team.$"

EmeraldChampions_Text_NeedTwoPokemon:
	.string "This is a Double Battle. Come back\n"
	.string "with two usable Pokémon.$"

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
	setvar VAR_EC_REPEL_SPRAY_STEPS, EC_REPEL_SPRAY_STEPS
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

EmeraldChampions_EventScript_SpecialistBonding:
	setvar VAR_TUTOR_CURSOR, 1
	call Common_EventScript_EmeraldChampionsBonding
	goto EmeraldChampions_EventScript_MoveTutorMain

@ Bonding keeps the chosen Pokémon in VAR_BONDING_MON and the value row in
@ VAR_BONDING_CURSOR, and hands both to each special in VAR_0x8004/0x8005.
Common_EventScript_EmeraldChampionsBonding::
	msgbox EmeraldChampions_Text_BondingChooseMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_ReturnService
	copyvar VAR_BONDING_MON, VAR_0x8004
	setvar VAR_BONDING_CURSOR, 0
	copyvar VAR_0x8004, VAR_BONDING_MON
	copyvar VAR_0x8005, VAR_BONDING_CURSOR
	special BufferEmeraldChampionsBondingPreview
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_BondingUnavailable

@ Declining a preview returns here with the cursor on the value just viewed.
EmeraldChampions_EventScript_BondingValues:
	message EmeraldChampions_Text_BondingValues
	waitmessage
	dynmultipush EmeraldChampions_Text_BondingLow, 0
	dynmultipush EmeraldChampions_Text_BondingThreshold, 1
	dynmultipush EmeraldChampions_Text_BondingMaximum, 2
	dynmultipush EmeraldChampions_Text_Back, 3
	dynmultistack 30, 1, FALSE, 4, 0, VAR_BONDING_CURSOR, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_ge VAR_RESULT, 3, Common_EventScript_EmeraldChampionsBonding
	copyvar VAR_BONDING_CURSOR, VAR_RESULT
	copyvar VAR_0x8004, VAR_BONDING_MON
	copyvar VAR_0x8005, VAR_BONDING_CURSOR
	special BufferEmeraldChampionsBondingPreview
	msgbox gStringVar4, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_BondingValues
	copyvar VAR_0x8004, VAR_BONDING_MON
	copyvar VAR_0x8005, VAR_BONDING_CURSOR
	special ApplyEmeraldChampionsBonding
	msgbox EmeraldChampions_Text_BondingDone, MSGBOX_DEFAULT
	goto Common_EventScript_EmeraldChampionsBonding

EmeraldChampions_EventScript_BondingUnavailable:
	msgbox EmeraldChampions_Text_BondingUnavailable, MSGBOX_DEFAULT
	goto Common_EventScript_EmeraldChampionsBonding

EmeraldChampions_EventScript_ReturnService:
	return

EmeraldChampions_Text_Bonding:
	.string "Bonding$"
EmeraldChampions_Text_BondingChooseMon:
	.string "Whose bond with you should\nI help with?$"
EmeraldChampions_Text_BondingValues:
	.string "How close should it feel to you?$"
EmeraldChampions_Text_BondingLow:
	.string "Keeps Its Distance$"
EmeraldChampions_Text_BondingThreshold:
	.string "Ready to Evolve$"
EmeraldChampions_Text_BondingMaximum:
	.string "Adores You$"
EmeraldChampions_Text_BondingDone:
	.string "All done! If it's ready to evolve,\nyour Leveler can help it along.$"
EmeraldChampions_Text_BondingUnavailable:
	.string "Please choose a Pokémon, not an Egg.$"

@ The Pokémon Center local guide: the game's built-in walkthrough. The
@ special BufferNextCenterLegendaryLead reads the topic from VAR_0x8005
@ (values match include/center_guide.h) and a cursor from VAR_0x8004, and
@ fills gStringVar4. msgbox expands into gStringVar4, so every page is
@ buffered again right before it is shown.
	.set CENTER_GUIDE_TOPIC_LEGENDS, 0
	.set CENTER_GUIDE_TOPIC_TIPS, 1
	.set CENTER_GUIDE_TOPIC_STORY, 2
	.set CENTER_GUIDE_TOPIC_BYE, 3

@ The list selects by topic, so VAR_GUIDE_CURSOR keeps the cursor on the topic
@ last read (Where to Go first). VAR_GUIDE_WARNED marks the legend warning as
@ given, so it is said once per conversation.
Common_EventScript_CenterLocalGuide::
	lockall
	faceplayer
	setvar VAR_GUIDE_CURSOR, CENTER_GUIDE_TOPIC_STORY
	setvar VAR_GUIDE_WARNED, FALSE
	msgbox EmeraldChampions_Text_CenterGuideHello, MSGBOX_DEFAULT

EmeraldChampions_EventScript_CenterGuideMenu:
	@ Offer a topic only when this city has something to say about it.
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, CENTER_GUIDE_TOPIC_LEGENDS
	special BufferNextCenterLegendaryLead
	copyvar VAR_GUIDE_HAS_LEGENDS, VAR_RESULT
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, CENTER_GUIDE_TOPIC_TIPS
	special BufferNextCenterLegendaryLead
	copyvar VAR_GUIDE_HAS_TIPS, VAR_RESULT
	message EmeraldChampions_Text_CenterGuideAsk
	waitmessage
	dynmultipush EmeraldChampions_Text_CenterGuideWhereToGo, CENTER_GUIDE_TOPIC_STORY
	goto_if_eq VAR_GUIDE_HAS_LEGENDS, FALSE, EmeraldChampions_EventScript_CenterGuideNoLegends
	dynmultipush EmeraldChampions_Text_CenterGuideRarePokemon, CENTER_GUIDE_TOPIC_LEGENDS
EmeraldChampions_EventScript_CenterGuideNoLegends:
	goto_if_eq VAR_GUIDE_HAS_TIPS, FALSE, EmeraldChampions_EventScript_CenterGuideNoTips
	dynmultipush EmeraldChampions_Text_CenterGuideThingsToDo, CENTER_GUIDE_TOPIC_TIPS
EmeraldChampions_EventScript_CenterGuideNoTips:
	dynmultipush EmeraldChampions_Text_CenterGuideBye, CENTER_GUIDE_TOPIC_BYE
	dynmultistack 30, 1, FALSE, 4, 0, VAR_GUIDE_CURSOR, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_CenterGuideGoodbye
	copyvar VAR_GUIDE_CURSOR, VAR_RESULT
	switch VAR_RESULT
	case CENTER_GUIDE_TOPIC_STORY, EmeraldChampions_EventScript_CenterGuideStory
	case CENTER_GUIDE_TOPIC_LEGENDS, EmeraldChampions_EventScript_CenterGuideLegends
	case CENTER_GUIDE_TOPIC_TIPS, EmeraldChampions_EventScript_CenterGuideTips
EmeraldChampions_EventScript_CenterGuideGoodbye:
	msgbox EmeraldChampions_Text_CenterGuideGoodbye, MSGBOX_DEFAULT
	releaseall
	end

EmeraldChampions_EventScript_CenterGuideStory:
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, CENTER_GUIDE_TOPIC_STORY
	special BufferNextCenterLegendaryLead
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_CenterGuideFinale
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_CenterGuideMenu

@ After the Hall of Fame the shared finale directions say where to go.
EmeraldChampions_EventScript_CenterGuideFinale:
	call Common_EventScript_FinaleDirections
	goto EmeraldChampions_EventScript_CenterGuideMenu

EmeraldChampions_EventScript_CenterGuideLegends:
	goto_if_eq VAR_GUIDE_WARNED, TRUE, EmeraldChampions_EventScript_CenterGuideLegendPages
	setvar VAR_GUIDE_WARNED, TRUE
	msgbox EmeraldChampions_Text_CenterGuideSaveFirst, MSGBOX_DEFAULT
EmeraldChampions_EventScript_CenterGuideLegendPages:
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, CENTER_GUIDE_TOPIC_LEGENDS
	goto EmeraldChampions_EventScript_CenterGuideNextPage

EmeraldChampions_EventScript_CenterGuideTips:
	setvar VAR_0x8004, 0
	setvar VAR_0x8005, CENTER_GUIDE_TOPIC_TIPS
EmeraldChampions_EventScript_CenterGuideNextPage:
	special BufferNextCenterLegendaryLead
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_CenterGuideMenu
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_CenterGuideNextPage

EmeraldChampions_Text_CenterGuideHello:
	.string "Hello! I'm the local guide.\n"
	.string "I know this area inside out.$"

EmeraldChampions_Text_CenterGuideAsk:
	.string "What would you like to know?$"

EmeraldChampions_Text_CenterGuideWhereToGo:
	.string "Where to Go$"

EmeraldChampions_Text_CenterGuideRarePokemon:
	.string "Rare Pokémon$"

EmeraldChampions_Text_CenterGuideThingsToDo:
	.string "Things to Do$"

EmeraldChampions_Text_CenterGuideBye:
	.string "Exit$"

EmeraldChampions_Text_CenterGuideSaveFirst:
	.string "A legend you meet face to face\n"
	.string "is one of a kind. If it faints,\l"
	.string "it's gone for good.\p"
	.string "Save before you face one. You\n"
	.string "can always run and try again.$"

EmeraldChampions_Text_CenterGuideGoodbye:
	.string "Come back whenever you feel\n"
	.string "lost. Good luck out there!$"

EmeraldChampions_EventScript_UseHarvestPouch::
	lockall
	setvar VAR_HARVEST_AT_BERRY_MASTER, FALSE
	call EmeraldChampions_EventScript_HarvestMenu
	releaseall
	end

EmeraldChampions_EventScript_HarvestMenu::
	message EmeraldChampions_Text_HarvestWelcome
	waitmessage
	dynmultipush EmeraldChampions_Text_MyHarvest, 0
	dynmultipush EmeraldChampions_Text_HarvestRequests, 1
	dynmultipush EmeraldChampions_Text_HarvestHelpChoice, 2
	@ Opened from the Bag, this is the top menu; at the Berry Master's it is one of his.
	goto_if_eq VAR_HARVEST_AT_BERRY_MASTER, TRUE, EmeraldChampions_EventScript_HarvestMenuBack
	dynmultipush EmeraldChampions_Text_Exit, 3
	goto EmeraldChampions_EventScript_HarvestMenuShow
EmeraldChampions_EventScript_HarvestMenuBack:
	dynmultipush EmeraldChampions_Text_Back, 3
EmeraldChampions_EventScript_HarvestMenuShow:
	dynmultistack 30, 1, FALSE, 4, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_HarvestListStart
	goto_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_HarvestRequests
	goto_if_eq VAR_RESULT, 2, EmeraldChampions_EventScript_HarvestHelp
	return

@ VAR_HARVEST_LIST_CURSOR holds the Berry ID under the cursor (the list
@ selects by ID; Berry IDs start at 1 and Back is 0).
EmeraldChampions_EventScript_HarvestListStart:
	setvar VAR_HARVEST_LIST_CURSOR, 1
EmeraldChampions_EventScript_HarvestList:
	message EmeraldChampions_Text_HarvestList
	waitmessage
	special BuildEmeraldChampionsHarvestChoices
	dynmultipush EmeraldChampions_Text_Back, 0
	dynmultistack 30, 1, FALSE, 4, 0, VAR_HARVEST_LIST_CURSOR, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, EmeraldChampions_EventScript_HarvestMenu
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_HarvestMenu
	copyvar VAR_HARVEST_LIST_CURSOR, VAR_RESULT
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
	copyvar VAR_HARVEST_REWARD, VAR_RESULT
	copyvar VAR_0x8004, VAR_HARVEST_REWARD
	special BufferEmeraldChampionsHarvestRecipe
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_HarvestRecipeClaimed
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto_if_eq VAR_HARVEST_AT_BERRY_MASTER, FALSE, EmeraldChampions_EventScript_HarvestRequests
	goto_if_eq VAR_HARVEST_REWARD, 3, EmeraldChampions_EventScript_HarvestCelebiCheck
	goto EmeraldChampions_EventScript_HarvestConfirm

EmeraldChampions_EventScript_HarvestRecipeClaimed:
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto_if_eq VAR_HARVEST_AT_BERRY_MASTER, FALSE, EmeraldChampions_EventScript_HarvestRequests
	goto_if_eq VAR_HARVEST_REWARD, 3, EmeraldChampions_EventScript_HarvestCelebiCheck
	goto EmeraldChampions_EventScript_HarvestAlready

EmeraldChampions_EventScript_HarvestConfirm:
	msgbox EmeraldChampions_Text_HarvestConfirm, MSGBOX_YESNO
	goto_if_ne VAR_RESULT, YES, EmeraldChampions_EventScript_HarvestRequests
	copyvar VAR_0x8004, VAR_HARVEST_REWARD
	special TradeEmeraldChampionsGardenBerries
	goto_if_eq VAR_RESULT, EC_MEGA_BERRY_TRADE_ALREADY_DONE, EmeraldChampions_EventScript_HarvestAlready
	goto_if_eq VAR_RESULT, EC_MEGA_BERRY_TRADE_NOT_ENOUGH, EmeraldChampions_EventScript_HarvestNotEnough
	goto_if_eq VAR_RESULT, EC_MEGA_BERRY_TRADE_BAG_FULL, EmeraldChampions_EventScript_HarvestBagFull
	goto_if_ne VAR_RESULT, EC_MEGA_BERRY_TRADE_SUCCESS, EmeraldChampions_EventScript_HarvestRequests
	playfanfare MUS_OBTAIN_ITEM
	msgbox EmeraldChampions_Text_HarvestReceived, MSGBOX_DEFAULT
	waitfanfare
	goto_if_eq VAR_HARVEST_REWARD, 3, EmeraldChampions_EventScript_HarvestCelebiCheck
	goto EmeraldChampions_EventScript_HarvestRequests

EmeraldChampions_EventScript_HarvestCelebiCheck:
	special CheckEmeraldChampionsGardenCelebi
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_HarvestConfirm
	goto_if_eq VAR_RESULT, 2, EmeraldChampions_EventScript_HarvestAlready
	goto_if_eq VAR_RESULT, 3, EmeraldChampions_EventScript_HarvestCelebiLost
	msgbox EmeraldChampions_Text_HarvestMeetCelebi, MSGBOX_YESNO
	goto_if_ne VAR_RESULT, YES, EmeraldChampions_EventScript_HarvestRequests
	setvar VAR_0x8004, SPECIES_CELEBI
	setvar VAR_0x8005, 0
	special CreateEmeraldChampionsStaticLegendaryEncounter
	dowildbattle
	@ Like every static legend: a knockout loses it for good; anything but a
	@ capture or a knockout leaves it in the garden for another visit.
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_WON, EmeraldChampions_EventScript_HarvestMenu
	special LoseEmeraldChampionsGardenCelebi
	msgbox EmeraldChampions_Text_HarvestCelebiVanished, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_HarvestMenu

EmeraldChampions_EventScript_HarvestCelebiLost:
	msgbox EmeraldChampions_Text_HarvestCelebiVanished, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_HarvestRequests

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

EmeraldChampions_Text_HarvestCelebiVanished:
	.string "Celebi collapsed and vanished from\n"
	.string "the garden without a trace…$"
EmeraldChampions_Text_HarvestWelcome:
	.string "The Harvest Pouch keeps count of\n"
	.string "every Berry you pick from a tree.$"
EmeraldChampions_Text_MyHarvest:
	.string "My Harvest$"
EmeraldChampions_Text_HarvestRequests:
	.string "Harvest Rewards$"
EmeraldChampions_Text_HarvestHelpChoice:
	.string "How It Works$"
EmeraldChampions_Text_HarvestList:
	.string "Here's what you've harvested so far,\n"
	.string "Berry by Berry.$"
EmeraldChampions_Text_HarvestRequestsPrompt:
	.string "The Berry Master on Route 123\n"
	.string "offers these harvest rewards.$"
EmeraldChampions_Text_HarvestBaxcalibrite:
	.string "Baxcalibrite$"
EmeraldChampions_Text_HarvestDragoninite:
	.string "Dragoninite$"
EmeraldChampions_Text_HarvestTyranitarite:
	.string "Tyranitarite$"
EmeraldChampions_Text_HarvestCelebi:
	.string "Celebi's Invitation$"
EmeraldChampions_Text_HarvestConfirm:
	.string "Trade your harvested Berries for\n"
	.string "{STR_VAR_1}?$"
EmeraldChampions_Text_HarvestReceived:
	.string "You received {STR_VAR_1}!\n"
	.string "The Berries in your Bag are safe.$"
EmeraldChampions_Text_HarvestAlready:
	.string "That reward is already yours.\n"
	.string "Your harvest wasn't used.$"
EmeraldChampions_Text_HarvestNotEnough:
	.string "You need more of those Berries.\n"
	.string "Pick as you go, or plant your own!$"
EmeraldChampions_Text_HarvestBagFull:
	.string "Make room in your Bag for the stone.\n"
	.string "Your harvested Berries are safe.$"
EmeraldChampions_Text_HarvestMeetCelebi:
	.string "Celebi has come to share\n"
	.string "your harvest!\p"
	.string "Bring Poké Balls. If you run, you\n"
	.string "can come back without more Berries.\p"
	.string "Would you like to meet it now?$"
EmeraldChampions_Text_HarvestHelp:
	.string "Shop clerks sell every Berry. They\n"
	.string "work when a Pokémon holds them.\p"
	.string "Each Berry you pick from a tree goes\n"
	.string "into your Bag and this pouch's count.\p"
	.string "Trees ripen within four hours and\n"
	.string "wait for you. Water them for more!\p"
	.string "Check the rewards, then plant the\n"
	.string "Berries you're missing!$"

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
	setvar VAR_0x8004, SPECIES_REGIROCK
	special CheckPlayerCaughtSpecies
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_GuardianDesert
	setvar VAR_0x8004, SPECIES_REGICE
	special CheckPlayerCaughtSpecies
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_GuardianIsland
	setvar VAR_0x8004, SPECIES_REGISTEEL
	special CheckPlayerCaughtSpecies
	goto_if_eq VAR_RESULT, FALSE, EmeraldChampions_EventScript_GuardianAncient
	goto_if_set FLAG_EC_REPORT_C45_COMPLETE, EmeraldChampions_EventScript_RuinsRouteKnown
	setflag FLAG_EC_REPORT_C45_COMPLETE
	goto_if_set FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, EmeraldChampions_EventScript_ThreeRuinsCompleteConfirmed
	msgbox EmeraldChampions_Text_ThreeRuinsComplete, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_RuinsRouteKnown
EmeraldChampions_EventScript_ThreeRuinsCompleteConfirmed::
	msgbox EmeraldChampions_Text_ThreeRuinsCompleteConfirmed, MSGBOX_DEFAULT
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
	.string "The marks are worn. The builders'\n"
	.string "message must be read first.\p"
	.string "Seek it in the Sealed Chamber\n"
	.string "before these ruins will answer.$"
EmeraldChampions_Text_NextDesertRuin:
	.string "Next, read the Desert Ruins\n"
	.string "account in Route 111's desert.\l"
	.string "Its puzzle uses Rock Smash.$"
EmeraldChampions_Text_NextIslandRuin:
	.string "Next, Surf to Island Cave on the\n"
	.string "west side of Route 105, north of\l"
	.string "Dewford. Read its rear wall.$"
EmeraldChampions_Text_NextAncientRuin:
	.string "Next, visit Ancient Tomb on\n"
	.string "Route 120. Read the rear wall;\l"
	.string "its puzzle uses Flash.$"
EmeraldChampions_Text_ThreeRuinsComplete:
	.string "The three guardians have answered!\p"
	.string "Together, their accounts tell of an\n"
	.string "age when the land and sea powers\l"
	.string "were feared beyond reckoning,\p"
	.string "until a guardian of the sky was\n"
	.string "called to hold them apart.$"
EmeraldChampions_Text_ThreeRuinsCompleteConfirmed:
	.string "The three guardians have answered!\p"
	.string "Together, their accounts tell of an\n"
	.string "age when the land and sea powers\l"
	.string "were feared beyond reckoning,\p"
	.string "until a guardian of the sky was\n"
	.string "called to hold them apart,\l"
	.string "just as you saw at Sootopolis.$"
EmeraldChampions_Text_ThreeRuinsLore:
	.string "The three accounts agree: long ago\n"
	.string "the land and sea powers were feared\l"
	.string "beyond reckoning,\p"
	.string "until a guardian of the sky was\n"
	.string "invoked to keep them apart.\p"
	.string "A fourth mark appears in all three.\n"
	.string "It names Regigigas, said to have\l"
	.string "mustered the others together.$"
EmeraldChampions_Text_ThreeRuinsLoreConfirmed:
	.string "The three accounts agree: long ago\n"
	.string "the land and sea powers were feared\l"
	.string "beyond reckoning,\p"
	.string "until a guardian of the sky was\n"
	.string "invoked to keep them apart.\p"
	.string "It is the same balance you restored\n"
	.string "at Sootopolis.\p"
	.string "A fourth mark appears in all three.\n"
	.string "It names Regigigas, said to have\l"
	.string "mustered the others together.$"

EmeraldChampions_EventScript_GuardianDesert::
	msgbox EmeraldChampions_Text_GuardianDesert, MSGBOX_DEFAULT
	return
EmeraldChampions_Text_GuardianDesert:
	.string "Regirock still waits inside the\n"
	.string "Desert Ruins on Route 111.\p"
	.string "Will you be the one to catch it?$"

EmeraldChampions_EventScript_GuardianIsland::
	msgbox EmeraldChampions_Text_GuardianIsland, MSGBOX_DEFAULT
	return
EmeraldChampions_Text_GuardianIsland:
	.string "Regice still waits inside the\n"
	.string "Island Cave on Route 105.\p"
	.string "Will you be the one to catch it?$"

EmeraldChampions_EventScript_GuardianAncient::
	msgbox EmeraldChampions_Text_GuardianAncient, MSGBOX_DEFAULT
	return
EmeraldChampions_Text_GuardianAncient:
	.string "Registeel still waits inside the\n"
	.string "Ancient Tomb on Route 120.\p"
	.string "Will you be the one to catch it?$"

EC_DeliverTravelDocuments::
	setvar VAR_TRAVEL_DOCS_PENDING, FALSE
	call_if_set FLAG_EC_EARNED_SS_TICKET, EC_Deliver_SS_TICKET
	call_if_set FLAG_EC_EARNED_EON_TICKET, EC_Deliver_EON_TICKET
	call_if_set FLAG_EC_EARNED_OLD_SEA_MAP, EC_Deliver_OLD_SEA_MAP
	call_if_set FLAG_EC_EARNED_AURORA_TICKET, EC_Deliver_AURORA_TICKET
	call_if_set FLAG_ENABLE_SHIP_NAVEL_ROCK, EC_Deliver_MYSTIC_TICKET
	call_if_eq VAR_TRAVEL_DOCS_PENDING, TRUE, EC_TravelDocumentsPending
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
	setvar VAR_TRAVEL_DOCS_PENDING, TRUE
	return
EC_TravelDocumentsPending::
	msgbox EC_Text_TravelDocumentsPending, MSGBOX_DEFAULT
	return
EC_Text_TravelDocumentsPending:
	.string "Your travel papers are waiting.\n"
	.string "Make room in the Key Items pocket,\l"
	.string "then speak to any Center nurse.\p"
	.string "Your passage is already approved.\n"
	.string "You can sail from Lilycove Harbor\l"
	.string "while we hold the papers for you.$"

EmeraldChampions_Text_Back:
	.string "Back$"
