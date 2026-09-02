AlteringCave_B1F_MapScripts::
	.byte 0

AlteringCave_B1F_EventScript_Leaf::
	lockall
	playse SE_PIN
	applymovement LOCALID_EC_LEAF, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_EC_LEAF, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 40
	msgbox AlteringCave_B1F_Text_LeafIntro, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_EC_LEAF, AlteringCave_B1F_Movement_LeafApproach
	applymovement LOCALID_PLAYER, AlteringCave_B1F_Movement_PlayerApproach
	waitmovement 0
	trainerbattle_no_intro_double TRAINER_LEAF_ALTERING_CAVE, AlteringCave_B1F_Text_LeafDefeat, EmeraldChampions_Text_NeedTwoPokemon
	checkitem ITEM_CATCHING_CHARM, 1
	goto_if_eq VAR_RESULT, TRUE, AlteringCave_B1F_EventScript_LeafComplete
	giveitem ITEM_CATCHING_CHARM
	goto_if_eq VAR_RESULT, FALSE, AlteringCave_B1F_EventScript_LeafBagFull
AlteringCave_B1F_EventScript_LeafComplete::
	msgbox AlteringCave_B1F_Text_LeafAfter, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, AlteringCave_B1F_Movement_PlayerGiveWay
	applymovement LOCALID_EC_LEAF, AlteringCave_B1F_Movement_LeafLeaves
	waitmovement 0
	setflag FLAG_EC_DEFEATED_LEAF_ALTERING_CAVE
	removeobject LOCALID_EC_LEAF
	setvar VAR_TEMP_0, 1
	releaseall
	end

AlteringCave_B1F_EventScript_LeafBagFull::
	msgbox AlteringCave_B1F_Text_LeafBagFull, MSGBOX_DEFAULT
	releaseall
	end

AlteringCave_B1F_Movement_LeafApproach:
	walk_down
	step_end

AlteringCave_B1F_Movement_PlayerApproach:
	walk_up
	step_end

AlteringCave_B1F_Movement_PlayerGiveWay:
	walk_left
	walk_in_place_fast_right
	step_end

AlteringCave_B1F_Movement_LeafLeaves:
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

AlteringCave_B1F_Text_LeafIntro:
	.string "I followed the distortions here from\n"
	.string "Kanto. They react to strong teams.\p"
	.string "Show me whether Hoenn chose well.$"

AlteringCave_B1F_Text_LeafDefeat:
	.string "It did. That was a Champion's answer.$"

AlteringCave_B1F_Text_LeafAfter:
	.string "Take this CATCHING CHARM. It rewards\n"
	.string "the patience these distortions demand.$"
AlteringCave_B1F_Text_LeafBagFull:
	.string "Make room for the CATCHING CHARM.\n"
	.string "Your reward will wait here.$"
.set EC_SIGN_MEWTWO_ID, 61

AlteringCave_B1F_EventScript_Mewtwo::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_MEWTWO_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, AlteringCave_B1F_EventScript_MewtwoDormant
	goto_if_eq VAR_RESULT, 1, AlteringCave_B1F_EventScript_MewtwoNeedsPartner
	goto_if_eq VAR_RESULT, 4, AlteringCave_B1F_EventScript_MewtwoCleanup
	msgbox AlteringCave_B1F_Text_MewtwoAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_MEWTWO, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_MEWTWO_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, AlteringCave_B1F_EventScript_MewtwoCaught
	msgbox AlteringCave_B1F_Text_MewtwoRemains, MSGBOX_DEFAULT
	release
	end

AlteringCave_B1F_EventScript_MewtwoDormant::
	msgbox AlteringCave_B1F_Text_MewtwoDormant, MSGBOX_DEFAULT
	release
	end

AlteringCave_B1F_EventScript_MewtwoNeedsPartner::
	msgbox AlteringCave_B1F_Text_MewtwoNeedsPartner, MSGBOX_DEFAULT
	release
	end

AlteringCave_B1F_EventScript_MewtwoCaught::
	setflag FLAG_EC_CAUGHT_MEWTWO
AlteringCave_B1F_EventScript_MewtwoCleanup::
	setflag FLAG_EC_CAUGHT_MEWTWO
	removeobject LOCALID_EC_MEWTWO
	release
	end

AlteringCave_B1F_Text_MewtwoDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

AlteringCave_B1F_Text_MewtwoNeedsPartner:
	.string "The SIGN sketches DITTO.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

AlteringCave_B1F_Text_MewtwoAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "MEWTWO answers the challenge!$"

AlteringCave_B1F_Text_MewtwoRemains:
	.string "The SIGN remains bright. MEWTWO\n"
	.string "will accept another challenge.$"
