
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
	.string "Evie: Adjust your partner's EV rows\n"
	.string "and watch the actual stat previews.\p"
	.string "I'll apply the whole spread together.\n"
	.string "Added EVs cost ¥2 each;\l"
	.string "reductions are free.$"

FallarborTown_Text_AnnoyingIntroPart8:
	.string "Ivy: Your Pokémon already have perfect\n"
	.string "IVs, so I'll only lower them on request.\p"
	.string "I'll set Speed or Attack as low or as\n"
	.string "high as you like for one Bottle Cap.\p"
	.string "Also, for three Bottle Caps, I'll give\n"
	.string "a Pokémon a new Hidden Power type.$"

@@@@@@@@@@@@@@@@ Super Training Script @@@@@@@@@@@@@@@@

FallarborTown_SuperTrainer::
	lockall
	faceplayer
	goto_if_unset FLAG_BADGE02_GET, FallarborTown_EventScript_EVsLocked
	msgbox FallarborTown_Text_SharedEVEditor, MSGBOX_DEFAULT
	setvar VAR_0x8004, TRUE
	special SetPlannedEVPricing
	goto EmeraldChampions_EventScript_EVTrainingChooseMon

FallarborTown_EventScript_EVsLocked:
	msgbox EmeraldChampions_Text_EVTrainingLocked, MSGBOX_DEFAULT
	releaseall
	end

FallarborTown_Text_SharedEVEditor:
	.string "Choose a partner, then adjust its\n"
	.string "EV rows and watch the stat previews.\p"
	.string "Confirm to apply the whole spread.\n"
	.string "I charge ¥2 for each added EV.\l"
	.string "Lowering EVs is free.\p"
	.string "LEFT/RIGHT: 4. L/R: 64.\n"
	.string "SELECT: clear. START: fill.\l"
	.string "Cancel leaves everything as it was.$"

FallarborTown_Explain_EVs::
	msgbox FallarborTown_Text_ExplainEvs, MSGBOX_DEFAULT
	return

HyperTraining_EventScript_DoNothing::
	closemessage
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
	.string "Battling won't raise them. Only my\n"
	.string "training and Center move tutors do.\p"
	.string "Once you have enough EVs in a stat,\n"
	.string "that stat will go up!\p"
	.string "You can have up to 252 EVs in one stat,\n"
	.string "but you can only have 510 EVs in total.\p"
	.string "It's normally good to put 252 EVs in\n"
	.string "two of your Pokémon's best stats,\l"
	.string "but you can get creative with EVs too!$"

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
	special PayForChosenMonIVChange
	goto_if_eq VAR_RESULT, EC_IV_SERVICE_UNCHANGED, HyperTraining_EventScript_Unchanged
	goto_if_eq VAR_RESULT, EC_IV_SERVICE_NOT_ENOUGH_CAPS, HyperTraining_EventScript_IVCapsPoor
	goto HyperTraining_EventScript_DoHyperTraining

HyperTraining_EventScript_ChangeHiddenPower::
	message HyperTraining_Text_ForThreeCapsChangeAllIVs
	waitmessage
	copyvar VAR_IV_TRAINING_MON, VAR_0x8004
	setvar VAR_0x8004, SCROLL_MULTI_HIDDEN_POWER
	special ShowScrollableMultichoice
HyperTraining_EventScript_SelectHiddenPower::
	goto_if_ge VAR_RESULT, 16, HyperTraining_EventScript_RebufferChosenMon
	copyvar VAR_IV_TRAINING_TYPE, VAR_RESULT
	copyvar VAR_0x8004, VAR_IV_TRAINING_MON
	copyvar VAR_0x8005, VAR_IV_TRAINING_TYPE
HyperTraining_EventScript_PreviewHiddenPower::
	closemessage
	delay 1
	special ShowChosenMonHiddenPowerPreview
	goto_if_eq VAR_RESULT, FALSE, HyperTraining_EventScript_RebufferChosenMon
	msgbox HyperTraining_Text_ConfirmHiddenPower, MSGBOX_YESNO
	special HideChosenMonHiddenPowerPreview
	closemessage
	delay 1
	goto_if_eq VAR_RESULT, NO, HyperTraining_EventScript_RebufferChosenMon
	copyvar VAR_0x8004, VAR_IV_TRAINING_MON
	copyvar VAR_0x8005, VAR_IV_TRAINING_TYPE
	goto HyperTraining_EventScript_ChangeAllIVs

HyperTraining_EventScript_ChangeAllIVs::
	special PayForChosenMonHiddenPower
	goto_if_eq VAR_RESULT, EC_IV_SERVICE_UNCHANGED, HyperTraining_EventScript_Unchanged
	goto_if_eq VAR_RESULT, EC_IV_SERVICE_NOT_ENOUGH_CAPS, HyperTraining_EventScript_IVCapsPoor
	goto HyperTraining_EventScript_DoHyperTraining

HyperTraining_EventScript_IVCapsPoor::
	msgbox HyperTraining_Text_NotEnoughCaps, MSGBOX_DEFAULT
	goto FallarborTown_HyperMainMenu

HyperTraining_EventScript_Unchanged::
	msgbox HyperTraining_Text_Unchanged, MSGBOX_DEFAULT
	goto FallarborTown_HyperMainMenu

HyperTraining_EventScript_DoHyperTraining::
	setflag FLAG_EC_PLAYER_IVS_MAXED
	playse SE_SHOP
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
	.string "One Bottle Cap per change. Set it to?$"

HyperTraining_ReportSpeed:
	.string "{STR_VAR_1}'s Speed IV is {STR_VAR_2}.\n"
	.string "One Bottle Cap per change. Set it to?$"

HyperTraining_Text_HyperTrainingMadeMonStronger:
	.string "The Hyper Training is complete\n"
	.string "for {STR_VAR_1}.$"

HyperTraining_Text_ForThreeCapsChangeAllIVs:
	.string "Choose a Hidden Power type.\n"
	.string "I'll show the IV changes first.$"

HyperTraining_Text_ConfirmHiddenPower:
	.string "Apply for three Bottle Caps?\n"
	.string "Unchanged IVs are free.$"

HyperTraining_Text_HowCanIHelp:
	.string "How can I help {STR_VAR_1} today?$"

HyperTraining_Text_NotEnoughCaps:
	.string "You don't have enough Bottle Caps.$"

HyperTraining_Text_Unchanged:
	.string "Nothing would change.\n"
	.string "Keep your Bottle Caps!$"
