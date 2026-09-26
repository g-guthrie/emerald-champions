
FallarborTown_MoveRelearnersHouse_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, FallarborTown_MoveRelearnersHouse_OnFrame
	.byte 0

FallarborTown_MoveRelearnersHouse_OnFrame:
	map_script_2 VAR_IVY_EVIE_HOUSE_STATE, 0, FallarborTown_EventScript_IvyEvieIntro
	.2byte 0

FallarborTown_EventScript_IvyEvieIntro::
	lockall
	applymovement LOCALID_EVIE, Common_Movement_WalkInPlaceFastestDown
	applymovement LOCALID_IVY, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	playse SE_PIN
	applymovement LOCALID_EVIE, Common_Movement_ExclamationMark
	applymovement LOCALID_IVY, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_EVIE, Common_Movement_Delay48
	applymovement LOCALID_IVY, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_EVIE, FallarborTown_Movement_MoveEvieDown
	applymovement LOCALID_IVY, FallarborTown_Movement_MoveIvyDown
	waitmovement 0	
	msgbox FallarborTown_Text_AnnoyingIntroPart1, MSGBOX_DEFAULT
	applymovement LOCALID_IVY, Common_Movement_WalkInPlaceFastestDown
	msgbox FallarborTown_Text_AnnoyingIntroPart2, MSGBOX_DEFAULT
	applymovement LOCALID_EVIE, Common_Movement_WalkInPlaceFastestDown
	msgbox FallarborTown_Text_AnnoyingIntroPart3, MSGBOX_DEFAULT
	applymovement LOCALID_IVY, Common_Movement_WalkInPlaceFastestDown
	msgbox FallarborTown_Text_AnnoyingIntroPart4, MSGBOX_DEFAULT
	applymovement LOCALID_EVIE, Common_Movement_WalkInPlaceFastestDown
	msgbox FallarborTown_Text_AnnoyingIntroPart5, MSGBOX_YESNO
	compare VAR_RESULT, NO
	call_if_eq FallarborTown_Explain_EVs
	applymovement LOCALID_IVY, Common_Movement_WalkInPlaceFastestDown
	msgbox FallarborTown_Text_AnnoyingIntroPart6, MSGBOX_YESNO
	compare VAR_RESULT, YES
	call_if_eq HyperTraining_EventScript_Explain_IVs
	applymovement LOCALID_EVIE, Common_Movement_WalkInPlaceFastestDown
	msgbox FallarborTown_Text_AnnoyingIntroPart7, MSGBOX_DEFAULT
	applymovement LOCALID_IVY, Common_Movement_WalkInPlaceFastestDown
	msgbox FallarborTown_Text_AnnoyingIntroPart8, MSGBOX_DEFAULT
	setvar VAR_IVY_EVIE_HOUSE_STATE, 1
	releaseall
	end

FallarborTown_Movement_MoveEvieDown:
	walk_faster_down
	walk_faster_down
	walk_faster_left
	walk_in_place_fast_down
	step_end

FallarborTown_Movement_MoveIvyDown:
	walk_faster_down
	walk_faster_down
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_in_place_fast_down
	step_end

FallarborTown_Text_AnnoyingIntroPart1:
	.string "Hey there, Trainer!\p"
	.string "If you want your Pokémon to be the very\n"
	.string "best in all of Hoenn, you've come to\l"
	.string "the right place!\p"
	.string "I'm Evie, and this is my sister, Ivy.$"

FallarborTown_Text_AnnoyingIntroPart2:
	.string "Ivy: We offer special training\n"
	.string "programs that will seriously beef up\l"
	.string "your team!$"

FallarborTown_Text_AnnoyingIntroPart3:
	.string "Evie: But our services don't come cheap.\n"
	.string "Well, mine don't, at least.\p"
	.string "You can pay Ivy with junk-$"

FallarborTown_Text_AnnoyingIntroPart4:
	.string "Ivy: Hey!\n"
	.string "Those Bottle Caps aren't junk!\p"
	.string "They're highly collectable, thank\n"
	.string "you very much!$"

FallarborTown_Text_AnnoyingIntroPart5:
	.string "Evie: Yeah, yeah, if you say so…\p"
	.string "Anyway, do you know about your Pokémon's\n"
	.string "Effort Values, or EVs for short?$"

FallarborTown_Text_AnnoyingIntroPart6:
	.string "Ivy: Okay! Now, how about Individual\n"
	.string "Values, or IVs?\p"
	.string "Do you want me to explain them?$"

FallarborTown_Text_AnnoyingIntroPart7:
	.string "Evie: Right, here's the deal. For ¥2,\n"
	.string "I'll increase a Pokémon's EVs by one.$"

FallarborTown_Text_AnnoyingIntroPart8:
	.string "Ivy: Your Pokémon already have perfect\n"
	.string "IVs, so I'll only lower them on request.\p"
	.string "I'll set Speed or Attack as low or as\n"
	.string "high as you like, free of charge.\p"
	.string "Also, for three Bottle Caps, I'll give\n"
	.string "a Pokémon a new Hidden Power type.$"

@@@@@@@@@@@@@@@@ Super Training Script @@@@@@@@@@@@@@@@

FallarborTown_SuperTrainer::
	lock
	faceplayer
	msgbox SuperTraining_Text_ChoosePokemon, MSGBOX_DEFAULT
	special ChoosePartyMon
	compare VAR_0x8004, PARTY_SIZE
	goto_if_ge SuperTraining_Refuse
	specialvar VAR_RESULT, ScriptGetPartyMonSpecies
	compare VAR_RESULT, SPECIES_EGG
	goto_if_eq SuperTraining_Egg
	bufferpartymonnick 0, VAR_0x8004
	setvar VAR_EV_TRAINING_CURSOR, 0
	msgbox SuperTraining_Text_TakeALookatEVs, MSGBOX_DEFAULT
	goto FallarborTown_SuperMainMenu

FallarborTown_SuperMainMenu::
	bufferpartymonnick 0, VAR_0x8004
	message SuperTraining_Text_HowCanIHelp
	waitmessage
	multichoice 0, 0, MULTI_SUPER_TRAINING_MENU, 0
	switch VAR_RESULT
	case 0, FallarborTown_ChangeEV
	case 1, FallarborTown_ResetEVs
	case 2, Call_FallarborTown_Explain_EVs
	case 3, HyperTraining_EventScript_DoNothing
	case 127, HyperTraining_EventScript_DoNothing
	releaseall
	end

Call_FallarborTown_Explain_EVs::
	call FallarborTown_Explain_EVs
	goto FallarborTown_SuperMainMenu

FallarborTown_Explain_EVs::
	msgbox FallarborTown_Text_ExplainEvs, MSGBOX_DEFAULT
	return

@ Each row shows that stat's current EVs, so the list is the spread.
@ VAR_EV_TRAINING_CURSOR keeps the cursor on the stat last trained.
FallarborTown_ChangeEV::
	bufferpartymonnick 0, VAR_0x8004
	message SuperTraining_Text_ThisIsMonsCurrentSpread
	waitmessage
	setvar VAR_0x8005, 0
	special BufferChosenMonEV
	dynmultipush SuperTraining_Text_RowHP, 0
	setvar VAR_0x8005, 1
	special BufferChosenMonEV
	dynmultipush SuperTraining_Text_RowAttack, 1
	setvar VAR_0x8005, 2
	special BufferChosenMonEV
	dynmultipush SuperTraining_Text_RowDefense, 2
	setvar VAR_0x8005, 3
	special BufferChosenMonEV
	dynmultipush SuperTraining_Text_RowSpeed, 3
	setvar VAR_0x8005, 4
	special BufferChosenMonEV
	dynmultipush SuperTraining_Text_RowSpAtk, 4
	setvar VAR_0x8005, 5
	special BufferChosenMonEV
	dynmultipush SuperTraining_Text_RowSpDef, 5
	dynmultistack 0, 0, FALSE, 6, 0, VAR_EV_TRAINING_CURSOR, DYN_MULTICHOICE_CB_NONE
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, FallarborTown_SuperMainMenu
	copyvar VAR_EV_TRAINING_CURSOR, VAR_RESULT
	switch VAR_RESULT
	case 0, HyperTraining_EventScript_ChangeHPEV
	case 1, FallarborTown_ChangeATKEV
	case 2, FallarborTown_ChangeDEFEV
	case 3, FallarborTown_ChangeSPEEDEV
	case 4, FallarborTown_ChangeSPATKEV
	case 5, FallarborTown_ChangeSPDEFEV
	case 127, FallarborTown_SuperMainMenu
	releaseall
	end

HyperTraining_EventScript_ChangeHPEV::
	setvar VAR_0x8005, 0
	special BufferChosenMonEV
	bufferpartymonnick 0, VAR_0x8004
	buffernumberstring 1, VAR_0x8006
	compare VAR_0x8006, 252
	goto_if_eq FallarborTown_MaxEVs
	message SuperTraining_ReportHP
	waitmessage
	goto FallarborTown_ChooseNewEV

FallarborTown_ChangeATKEV::
	setvar VAR_0x8005, 1
	special BufferChosenMonEV
	bufferpartymonnick 0, VAR_0x8004
	buffernumberstring 1, VAR_0x8006
	compare VAR_0x8006, 252
	goto_if_eq FallarborTown_MaxEVs
	message SuperTraining_ReportAtk
	waitmessage
	goto FallarborTown_ChooseNewEV

FallarborTown_ChangeDEFEV::
	setvar VAR_0x8005, 2
	special BufferChosenMonEV
	bufferpartymonnick 0, VAR_0x8004
	buffernumberstring 1, VAR_0x8006
	compare VAR_0x8006, 252
	goto_if_eq FallarborTown_MaxEVs
	message SuperTraining_ReportDef
	waitmessage
	goto FallarborTown_ChooseNewEV

FallarborTown_ChangeSPEEDEV::
	setvar VAR_0x8005, 3
	special BufferChosenMonEV
	bufferpartymonnick 0, VAR_0x8004
	buffernumberstring 1, VAR_0x8006
	compare VAR_0x8006, 252
	goto_if_eq FallarborTown_MaxEVs
	message SuperTraining_ReportSpeed
	waitmessage
	goto FallarborTown_ChooseNewEV

FallarborTown_ChangeSPATKEV::
	setvar VAR_0x8005, 4
	special BufferChosenMonEV
	bufferpartymonnick 0, VAR_0x8004
	buffernumberstring 1, VAR_0x8006
	compare VAR_0x8006, 252
	goto_if_eq FallarborTown_MaxEVs
	message SuperTraining_ReportSpAtk
	waitmessage
	goto FallarborTown_ChooseNewEV

FallarborTown_ChangeSPDEFEV::
	setvar VAR_0x8005, 5
	special BufferChosenMonEV
	bufferpartymonnick 0, VAR_0x8004
	buffernumberstring 1, VAR_0x8006
	compare VAR_0x8006, 252
	goto_if_eq FallarborTown_MaxEVs
	message SuperTraining_ReportSpDef
	waitmessage
	goto FallarborTown_ChooseNewEV

FallarborTown_MaxEVs::
	msgbox SuperTraining_ReportMaxEVs
	goto FallarborTown_ChangeEV

FallarborTown_ChooseNewEV::
	multichoice 0, 0, MULTI_EV_MENU, 0
	switch VAR_RESULT
	case 0, FallarborTown_ChangeEV4
	case 1, FallarborTown_ChangeEV8
	case 2, FallarborTown_ChangeEV12
	case 3, FallarborTown_ChangeEV64
	case 4, FallarborTown_ChangeEV128
	case 5, FallarborTown_ChangeEV252
	case 127, FallarborTown_ChangeEV

FallarborTown_ChangeEV4::
	setvar VAR_0x8006, 4
	setvar VAR_EV_TRAINING_COST, 8
	goto FallarborTown_DoSuperTraining

FallarborTown_ChangeEV8::
	setvar VAR_0x8006, 8
	setvar VAR_EV_TRAINING_COST, 16
	goto FallarborTown_DoSuperTraining

FallarborTown_ChangeEV12::
	setvar VAR_0x8006, 12
	setvar VAR_EV_TRAINING_COST, 24
	goto FallarborTown_DoSuperTraining

FallarborTown_ChangeEV64::
	setvar VAR_0x8006, 64
	setvar VAR_EV_TRAINING_COST, 128
	goto FallarborTown_DoSuperTraining

FallarborTown_ChangeEV128::
	setvar VAR_0x8006, 128
	setvar VAR_EV_TRAINING_COST, 256
	goto FallarborTown_DoSuperTraining

FallarborTown_ChangeEV252::
	setvar VAR_0x8006, 252
	setvar VAR_EV_TRAINING_COST, 504
	goto FallarborTown_DoSuperTraining

FallarborTown_ResetEVs::
	msgbox SuperTraining_Text_ICanResetEVs, MSGBOX_YESNO
	compare VAR_RESULT, 0
	goto_if_eq FallarborTown_SuperMainMenu
	special ResetChosenMonEVs
	msgbox SuperTraining_Text_EVsReset, MSGBOX_DEFAULT
	goto FallarborTown_SuperMainMenu
	
FallarborTown_DoSuperTraining::
	special CheckChosenMonCanGainEVs
	compare VAR_RESULT, 0
	goto_if_eq FallarborTown_CantExceedEVLimit
	showmoneybox 15, 0
	buffernumberstring 2, VAR_EV_TRAINING_COST
	msgbox SuperTraining_ShowPrice, MSGBOX_YESNO
	compare VAR_RESULT, 0
	goto_if_eq FallarborTown_WontPay
	@ Native money opcodes take literals; reuse the variable-cost specials.
	copyvar VAR_EV_TRAINING_STAT, VAR_0x8005
	copyvar VAR_0x8005, VAR_EV_TRAINING_COST
	specialvar VAR_RESULT, IsEnoughForCostInVar0x8005
	copyvar VAR_0x8005, VAR_EV_TRAINING_STAT
	compare VAR_RESULT, 0
	goto_if_eq FallarborTown_CantAfford
	copyvar VAR_0x8005, VAR_EV_TRAINING_COST
	special SubtractMoneyFromVar0x8005
	copyvar VAR_0x8005, VAR_EV_TRAINING_STAT
	updatemoneybox
	special IncreaseChosenMonEVs
	bufferpartymonnick 0, VAR_0x8004
	buffernumberstring 1, VAR_0x8007
	msgbox SuperTraining_Text_EVsHaveIncreased
	hidemoneybox
	goto FallarborTown_ChangeEV

FallarborTown_CantExceedEVLimit::
	buffernumberstring 1, VAR_0x8007
	msgbox SuperTraining_TooManyEVs, MSGBOX_DEFAULT
	goto FallarborTown_ChangeEV

FallarborTown_WontPay::
	hidemoneybox
	goto FallarborTown_ChangeEV

FallarborTown_CantAfford::
	msgbox FallarborTown_CantAfford_String, MSGBOX_DEFAULT
	hidemoneybox
	goto FallarborTown_ChangeEV

HyperTraining_EventScript_DoNothing::
	releaseall
	end

SuperTraining_Egg::
	msgbox FallarborTown_Text_CantTrainEgg, MSGBOX_DEFAULT
	releaseall
	end

SuperTraining_Refuse::
	msgbox FallarborTown_Text_SomeOtherTime, MSGBOX_DEFAULT
	releaseall
	end

SuperTraining_Text_ChoosePokemon:
	.string "Evie: Which Pokémon would you like me\n"
	.string "to train?$"

FallarborTown_Text_CantTrainEgg:
	.string "Oh come on, I can't train an Egg!\n"
	.string "Bring it back once it's hatched.$"

FallarborTown_Text_SomeOtherTime:
	.string "Oh, okay.\n"
	.string "Some other time, then?$"

FallarborTown_Text_ExplainEvs:
	.string "Evie: EVs are part of every stat\n"
	.string "a Pokémon has.\p"
	.string "They can have HP EVs, Attack EVs,\n"
	.string "Defense EVs… you get the idea.\p"
	.string "Battling won't raise them. Only\n"
	.string "vitamins and my training do.\p"
	.string "Once you have enough EVs in a stat,\n"
	.string "that stat will go up!\p"
	.string "You can have up to 252 EVs in one stat,\n"
	.string "but you can only have 510 EVs in total.\p"
	.string "It's normally good to put 252 EVs in\n"
	.string "two of your Pokémon's best stats,\l"
	.string "but you can get creative with EVs too!$"

SuperTraining_Text_TakeALookatEVs:
	.string "Okay! Again, I charge ¥2 per EV\n"
	.string "added. Now let's take a look at your\l"
	.string "{STR_VAR_1}'s EVs.$"

SuperTraining_Text_ThisIsMonsCurrentSpread:
	.string "Here is {STR_VAR_1}'s EV spread.\n"
	.string "Which stat would you like it to train?$"

SuperTraining_Text_RowHP:
	.string "HP{CLEAR_TO 0x34}{STR_VAR_2}$"

SuperTraining_Text_RowAttack:
	.string "Attack{CLEAR_TO 0x34}{STR_VAR_2}$"

SuperTraining_Text_RowDefense:
	.string "Defense{CLEAR_TO 0x34}{STR_VAR_2}$"

SuperTraining_Text_RowSpeed:
	.string "Speed{CLEAR_TO 0x34}{STR_VAR_2}$"

SuperTraining_Text_RowSpAtk:
	.string "Sp. Atk{CLEAR_TO 0x34}{STR_VAR_2}$"

SuperTraining_Text_RowSpDef:
	.string "Sp. Def{CLEAR_TO 0x34}{STR_VAR_2}$"

SuperTraining_ReportHP:
	.string "{STR_VAR_1} has {STR_VAR_2} HP EVs.\n"
	.string "How many EVs would you like to add?$"

SuperTraining_ReportAtk:
	.string "{STR_VAR_1} has {STR_VAR_2} Attack EVs.\n"
	.string "How many EVs would you like to add?$"

SuperTraining_ReportDef:
	.string "{STR_VAR_1} has {STR_VAR_2} Defense EVs.\n"
	.string "How many EVs would you like to add?$"

SuperTraining_ReportSpeed:
	.string "{STR_VAR_1} has {STR_VAR_2} Speed EVs.\n"
	.string "How many EVs would you like to add?$"

SuperTraining_ReportSpAtk:
	.string "{STR_VAR_1} has {STR_VAR_2} Sp. Atk EVs.\n"
	.string "How many EVs would you like to add?$"

SuperTraining_ReportSpDef:
	.string "{STR_VAR_1} has {STR_VAR_2} Sp. Def EVs.\n"
	.string "How many EVs would you like to add?$"

SuperTraining_Text_EVsHaveIncreased:
	.string "{STR_VAR_1}'s EVs in that stat have\n"
	.string "increased to {STR_VAR_2}!$"

SuperTraining_Text_ICanResetEVs:
	.string "I can completely reset your\n"
	.string "{STR_VAR_1}'s EVs for free!\p"
	.string "This will remove all of its EVs, so\n"
	.string "you can add more to the stats you want\l"
	.string "{STR_VAR_1} to focus on.\p"
	.string "Would you like me to reset\n"
	.string "{STR_VAR_1}'s EVs?$"

SuperTraining_Text_HowCanIHelp:
	.string "How can I help {STR_VAR_1} today?$"
	
SuperTraining_Text_EVsReset::
	.string "{STR_VAR_1}'s EVs have been reset!$"

SuperTraining_ReportMaxEVs:
	.string "{STR_VAR_1} has 252 EVs in that stat!\n"
	.string "It can't gain any more!$"

SuperTraining_TooManyEVs:
	.string "{STR_VAR_1} already has\n"
	.string "{STR_VAR_2} EVs in total!\p"
	.string "It can't gain that many more!$"

SuperTraining_ShowPrice:
	.string "That'll cost ¥{STR_VAR_3}. Is that okay?$"

FallarborTown_CantAfford_String:
	.string "You don't have the money.$"

@@@@@@@@@@@@@@@@ Hyper Training Script @@@@@@@@@@@@@@@@

FallarborTown_HyperTrainer::
	lockall
	faceplayer
	msgbox HyperTraining_Text_ChoosePokemon, MSGBOX_DEFAULT
	special ChoosePartyMon
	compare VAR_0x8004, PARTY_SIZE
	goto_if_ge SuperTraining_Refuse
	specialvar VAR_RESULT, ScriptGetPartyMonSpecies
	compare VAR_RESULT, SPECIES_EGG
	goto_if_eq SuperTraining_Egg
	bufferpartymonnick 0, VAR_0x8004
	msgbox HyperTraining_Text_LetsTakeALook, MSGBOX_DEFAULT
	goto FallarborTown_HyperMainMenu

FallarborTown_HyperMainMenu::
	bufferpartymonnick 0, VAR_0x8004
	message HyperTraining_Text_HowCanIHelp
	waitmessage
	multichoice 0, 0, MULTI_HYPER_TRAINING_MENU, 0
	switch VAR_RESULT
	case 0, HyperTraining_EventScript_ChangeIV
	case 1, HyperTraining_EventScript_ChangeHiddenPower
	case 2, Call_HyperTraining_EventScript_Explain_IVs
	case 3, HyperTraining_EventScript_DoNothing
	case 127, HyperTraining_EventScript_DoNothing
	releaseall
	end

Call_HyperTraining_EventScript_Explain_IVs::
	call HyperTraining_EventScript_Explain_IVs
	goto FallarborTown_HyperMainMenu

HyperTraining_EventScript_Explain_IVs::
	msgbox HyperTraining_Text_ExplainIVs, MSGBOX_DEFAULT
	return

HyperTraining_EventScript_ChangeIV::
	bufferpartymonnick 0, VAR_0x8004
	message HyperTraining_Text_WhichIVShouldChange
	waitmessage
	dynmultipush HyperTraining_Text_Speed, 3
	dynmultipush HyperTraining_Text_Attack, 1
	dynmultipush EmeraldChampions_Text_Back, 127
	dynmultistack 30, 1, FALSE, 3, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, 127, FallarborTown_HyperMainMenu
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, FallarborTown_HyperMainMenu
	copyvar VAR_0x8005, VAR_RESULT
	special BufferChosenMonIV
	bufferpartymonnick 0, VAR_0x8004
	buffernumberstring 1, VAR_0x8006
	goto_if_eq VAR_0x8005, 3, HyperTraining_EventScript_ReportSpeed
	message HyperTraining_ReportAtk
	goto HyperTraining_EventScript_ChooseLevel

HyperTraining_EventScript_ReportSpeed::
	message HyperTraining_ReportSpeed

HyperTraining_EventScript_ChooseLevel::
	waitmessage
	dynmultipush HyperTraining_Text_Lowest, 0
	dynmultipush HyperTraining_Text_Highest, 31
	dynmultistack 30, 1, FALSE, 2, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, HyperTraining_EventScript_ChangeIV
	copyvar VAR_0x8006, VAR_RESULT
	special ChangeChosenMonIVs
	goto HyperTraining_EventScript_DoHyperTraining

HyperTraining_EventScript_ChangeHiddenPower::
	message HyperTraining_Text_ForThreeCapsChangeAllIVs
	waitmessage
	copyvar VAR_IV_TRAINING_MON, VAR_0x8004
	setvar VAR_0x8004, SCROLL_MULTI_HIDDEN_POWER
	special ShowScrollableMultichoice
HyperTraining_EventScript_SelectHiddenPower::
	goto_if_ge VAR_RESULT, 16, HyperTraining_EventScript_RebufferChosenMon
	copyvar VAR_0x8004, VAR_IV_TRAINING_MON
	copyvar VAR_0x8005, VAR_RESULT
	goto HyperTraining_EventScript_ChangeAllIVs

HyperTraining_EventScript_ChangeAllIVs::
	checkitem ITEM_BOTTLE_CAP, 3
	compare VAR_RESULT, FALSE
	goto_if_eq HyperTraining_EventScript_NotEnoughCaps
	special ChangeChosenMonHiddenPower
	msgbox HyperTraining_Text_PayThreeCaps, MSGBOX_DEFAULT
	removeitem ITEM_BOTTLE_CAP, 3
	goto HyperTraining_EventScript_DoHyperTraining

HyperTraining_EventScript_DoHyperTraining::
	setflag FLAG_EC_PLAYER_IVS_MAXED
	msgbox HyperTraining_Text_StartHyperTraining, MSGBOX_AUTOCLOSE
	fadescreen 1
	playfanfare 0x173
	waitfanfare
	fadescreen 0
	bufferpartymonnick 0, VAR_0x8004
	msgbox HyperTraining_Text_HyperTrainingMadeMonStronger, MSGBOX_DEFAULT
	goto FallarborTown_HyperMainMenu

HyperTraining_EventScript_NotEnoughCaps::
	msgbox HyperTraining_Text_NotEnoughCaps, MSGBOX_DEFAULT
	goto HyperTraining_EventScript_RebufferChosenMon

HyperTraining_EventScript_RebufferChosenMon::
	copyvar VAR_0x8004, VAR_IV_TRAINING_MON
	goto FallarborTown_HyperMainMenu

HyperTraining_Text_ChoosePokemon:
	.string "Ivy: Which Pokémon needs some\n"
	.string "Hyper Training?$"

HyperTraining_Text_ExplainIVs:
	.string "Ivy: IVs are a hidden part of your\n"
	.string "Pokémon's stats.\p"
	.string "Every Pokémon you raise has them at\n"
	.string "their best already.\p"
	.string "But sometimes lower is better!\p"
	.string "Under Trick Room, the slowest\n"
	.string "Pokémon move first.\p"
	.string "And a special attacker with low Attack\n"
	.string "hurts itself less in confusion.\p"
	.string "IVs also decide the type of the move\n"
	.string "Hidden Power.$"

HyperTraining_Text_LetsTakeALook:
	.string "Okay! Let's take a look at your\n"
	.string "{STR_VAR_1}'s IVs.$"

HyperTraining_Text_WhichIVShouldChange:
	.string "Which of {STR_VAR_1}'s IVs\n"
	.string "would you like to set?$"

HyperTraining_Text_Speed:
	.string "Speed$"

HyperTraining_Text_Attack:
	.string "Attack$"

HyperTraining_Text_Lowest:
	.string "Lowest (0)$"

HyperTraining_Text_Highest:
	.string "Highest (31)$"

HyperTraining_ReportAtk:
	.string "{STR_VAR_1}'s Attack IV is {STR_VAR_2}.\n"
	.string "What should I set it to?$"

HyperTraining_ReportSpeed:
	.string "{STR_VAR_1}'s Speed IV is {STR_VAR_2}.\n"
	.string "What should I set it to?$"

HyperTraining_Text_HyperTrainingMadeMonStronger:
	.string "The Hyper Training is complete\n"
	.string "for {STR_VAR_1}.$"

HyperTraining_Text_ForThreeCapsChangeAllIVs:
	.string "For three Bottle Caps, I can change all\n"
	.string "of a Pokémon's IVs at once to give them\l"
	.string "a new Hidden Power type.\p"
	.string "Would you like to change {STR_VAR_1}'s\n"
	.string "Hidden Power to one of these types?$"

HyperTraining_Text_HowCanIHelp:
	.string "How can I help {STR_VAR_1} today?$"

HyperTraining_Text_StartHyperTraining:
	.string "Got it! Let's do some Hyper Training!\p"
	.string "… … … … … …\n"
	.string "… … … … … …$"

HyperTraining_Text_NotEnoughCaps:
	.string "You don't have enough Bottle Caps.$"

HyperTraining_Text_PayThreeCaps:
	.string "{PLAYER} handed over three Bottle Caps.$"
