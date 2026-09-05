MagmaHideout_4F_MapScripts::
	.byte 0

MagmaHideout_4F_EventScript_Maxie::
	lockall
	goto_if_not_defeated TRAINER_COURTNEY_MAGMA_HIDEOUT, MagmaHideout_4F_EventScript_AdminsStillStanding
	goto_if_not_defeated TRAINER_TABITHA_MAGMA_HIDEOUT, MagmaHideout_4F_EventScript_AdminsStillStanding
	call EventScript_CheckStoryDoubleBattleParty
	playbgm MUS_ENCOUNTER_MAGMA, FALSE
	msgbox MagmaHideout_4F_Text_MaxieAwakenGroudon, MSGBOX_DEFAULT
	closemessage
	delay 20
	setvar VAR_RESULT, 1
	playse SE_M_DETECT
	getobjectcurrentxy LOCALID_MAGMA_HIDEOUT_4F_MAXIE, CURRENT_POSITION, VAR_0x8004, VAR_0x8005
	dofieldeffectsparkle VAR_0x8004, VAR_0x8005, 0
	waitfieldeffect FLDEFF_SPARKLE
	setvar VAR_RESULT, 1
	playfanfare MUS_AWAKEN_LEGEND
	playse SE_ORB
	special DoOrbEffect
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	delay 150
	removeobject LOCALID_MAGMA_HIDEOUT_4F_GROUDON_SLEEPING
	addobject LOCALID_MAGMA_HIDEOUT_4F_GROUDON
	waitstate
	delay 60
	applymovement LOCALID_MAGMA_HIDEOUT_4F_GROUDON, MagmaHideout_4F_Movement_GroudonApproach
	waitmovement 0
	special FadeOutOrbEffect
	playmoncry SPECIES_GROUDON, CRY_MODE_ENCOUNTER
	waitmoncry
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 1  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	applymovement LOCALID_MAGMA_HIDEOUT_4F_GROUDON, MagmaHideout_4F_Movement_GroudonExit
	waitmovement 0
	removeobject LOCALID_MAGMA_HIDEOUT_4F_GROUDON
	delay 4
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	delay 30
	applymovement LOCALID_MAGMA_HIDEOUT_4F_MAXIE, MagmaHideout_4F_Movement_MaxieLookAround
	waitmovement 0
	msgbox MagmaHideout_4F_Text_MaxieGroudonWhatsWrong, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	delay 30
	applymovement LOCALID_MAGMA_HIDEOUT_4F_MAXIE, Common_Movement_FacePlayer
	waitmovement 0
	msgbox MagmaHideout_4F_Text_MaxieOhItWasYou, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro_double TRAINER_MAXIE_MAGMA_HIDEOUT, MagmaHideout_4F_Text_MaxieDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_4F_Text_MaxieImGoingAfterGroudon, MSGBOX_DEFAULT
	closemessage
	clearflag FLAG_HIDE_SLATEPORT_CITY_CAPTAIN_STERN
	clearflag FLAG_HIDE_SLATEPORT_CITY_GABBY_AND_TY
	setvar VAR_SLATEPORT_CITY_STATE, 1
	setflag FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT
	setvar VAR_SLATEPORT_HARBOR_STATE, 1
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_MAGMA_HIDEOUT_4F_MAXIE
	removeobject LOCALID_MAGMA_HIDEOUT_4F_GRUNT_1
	removeobject LOCALID_MAGMA_HIDEOUT_4F_GRUNT_2
	removeobject LOCALID_MAGMA_HIDEOUT_4F_GRUNT_3
	removeobject LOCALID_MAGMA_HIDEOUT_4F_TABITHA
	setflag FLAG_HIDE_MAGMA_HIDEOUT_GRUNTS
	fadescreen FADE_FROM_BLACK
	releaseall
	end

MagmaHideout_4F_EventScript_AdminsStillStanding::
	msgbox MagmaHideout_4F_Text_AdminsStillStanding, MSGBOX_DEFAULT
	releaseall
	end

MagmaHideout_4F_Movement_GroudonApproach:
	delay_16
	delay_16
	walk_slow_down
	delay_16
	delay_16
	delay_16
	walk_slow_down
	delay_16
	delay_16
	delay_16
	step_end

MagmaHideout_4F_Movement_GroudonExit:
	slide_up
	slide_up
	step_end

MagmaHideout_4F_Movement_MaxieLookAround:
	face_left
	delay_16
	face_right
	delay_16
	face_left
	delay_16
	face_right
	delay_16
	face_up
	delay_16
	delay_16
	step_end

MagmaHideout_4F_EventScript_Grunt11::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_11, MagmaHideout_4F_Text_Grunt11Intro, MagmaHideout_4F_Text_Grunt11Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_4F_Text_Grunt11PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_4F_EventScript_Grunt12::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_12, MagmaHideout_4F_Text_Grunt12Intro, MagmaHideout_4F_Text_Grunt12Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_4F_Text_Grunt12PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_4F_EventScript_Grunt13::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_13, MagmaHideout_4F_Text_Grunt13Intro, MagmaHideout_4F_Text_Grunt13Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_4F_Text_Grunt13PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_4F_EventScript_Tabitha::
	trainerbattle_double TRAINER_TABITHA_MAGMA_HIDEOUT, MagmaHideout_4F_Text_TabithaIntro, MagmaHideout_4F_Text_TabithaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_4F_Text_TabithaPostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_4F_EventScript_Courtney::
	trainerbattle_double TRAINER_COURTNEY_MAGMA_HIDEOUT, MagmaHideout_4F_Text_CourtneyIntro, MagmaHideout_4F_Text_CourtneyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_4F_Text_CourtneyAfter, MSGBOX_AUTOCLOSE
	end

MagmaHideout_4F_Text_CourtneyIntro:
	.string "COURTNEY: Prediction: you will target the\n"
	.string "sun before noticing the split pressure.\p"
	.string "Let us measure how quickly you correct.$"

MagmaHideout_4F_Text_CourtneyDefeat:
	.string "Correction arrived before failure. Noted.$"

MagmaHideout_4F_Text_CourtneyAfter:
	.string "MAXIE values certainty. You keep proving\n"
	.string "that adaptation is also measurable.$"

MagmaHideout_4F_Text_AdminsStillStanding:
	.string "COURTNEY and TABITHA still hold this\n"
	.string "chamber. Face both before MAXIE.$"

MagmaHideout_4F_Text_Grunt11Intro:
	.string "MAXIE promised solid ground under every\n"
	.string "home in HOENN. I believe him.\p"
	.string "MANDIBUZZ taunts, MAMOSWINE crushes.\n"
	.string "You will not reach the chamber.$"

MagmaHideout_4F_Text_Grunt11Defeat:
	.string "You moved before my TAUNT mattered…$"

MagmaHideout_4F_Text_Grunt11PostBattle:
	.string "None of us have seen GROUDON. MAXIE\n"
	.string "keeps it behind the last door.\p"
	.string "I wonder if certainty needs a door.$"

MagmaHideout_4F_Text_Grunt12Intro:
	.string "Sun, sand, and GARCHOMP's stone. MAXIE\n"
	.string "built this formation for one outcome.\p"
	.string "Soon! Very soon our ground will hold!$"

MagmaHideout_4F_Text_Grunt12Defeat:
	.string "The formation had one answer, and you\n"
	.string "found the other…$"

MagmaHideout_4F_Text_Grunt12PostBattle:
	.string "MAXIE, sir! An intruder is headed for\n"
	.string "the chamber!$"

MagmaHideout_4F_Text_Grunt13Intro:
	.string "AMOONGUSS pulls your attacks. RHYPERIOR\n"
	.string "turns the ones that land into power.\p"
	.string "You're not getting by me easily!$"

MagmaHideout_4F_Text_Grunt13Defeat:
	.string "You hit the slot I left open…$"

MagmaHideout_4F_Text_Grunt13PostBattle:
	.string "Go on, then. Tell MAXIE the ground\n"
	.string "moved under us too.$"

MagmaHideout_4F_Text_TabithaIntro:
	.string "TABITHA: Every chamber behind you was\n"
	.string "built to make one approach predictable.\p"
	.string "MAXIE is already with GROUDON. I only\n"
	.string "need to make your last answer too slow.$"

MagmaHideout_4F_Text_TabithaDefeat:
	.string "Your last answer was faster than my\n"
	.string "model allowed…$"

MagmaHideout_4F_Text_TabithaPostBattle:
	.string "Every minute you spent on me was one\n"
	.string "more MAXIE had with GROUDON.\p"
	.string "Go and see what certainty awakened.$"

MagmaHideout_4F_Text_MaxieAwakenGroudon:
	.string "MAXIE: GROUDON, ancient architect of\n"
	.string "continents... accept the BLUE ORB.\p"
	.string "The SIGN network is awake. Bind its\n"
	.string "energy to land that will never yield.\p"
	.string "Show HOENN the certainty of your power!$"

MagmaHideout_4F_Text_MaxieGroudonWhatsWrong:
	.string "MAXIE: GROUDON! The model was exact.\n"
	.string "Why did the ORB make you reject me?\p"
	.string "Where are you going?$"

MagmaHideout_4F_Text_MaxieOhItWasYou:
	.string "MAXIE: You again. GROUDON fled before\n"
	.string "you touched the machine.\p"
	.string "Then prove the error is mine, not yours.$"

MagmaHideout_4F_Text_MaxieDefeat:
	.string "What makes you so adept at handling\n"
	.string "POKéMON?$"

MagmaHideout_4F_Text_MaxieImGoingAfterGroudon:
	.string "MAXIE: Your team adapted; my plan did\n"
	.string "not. I understand the lesson.\p"
	.string "But GROUDON is loose, and ARCHIE still\n"
	.string "holds the matching disaster.\p"
	.string "I am going after it. This is no longer\n"
	.string "an experiment we control.$"
