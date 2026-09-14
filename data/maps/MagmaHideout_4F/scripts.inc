MagmaHideout_4F_MapScripts::
	.byte 0

MagmaHideout_4F_EventScript_Maxie::
	call EC_RequireVoyage
	lockall
	goto_if_not_defeated TRAINER_COURTNEY_MAGMA_HIDEOUT, MagmaHideout_4F_EventScript_AdminsStillStanding
	goto_if_not_defeated TRAINER_TABITHA_MAGMA_HIDEOUT, MagmaHideout_4F_EventScript_AdminsStillStanding
	call EventScript_CheckStoryDoubleBattleParty
	playbgm MUS_ENCOUNTER_MAGMA, FALSE
	goto_if_set FLAG_HIDE_MAGMA_HIDEOUT_4F_GROUDON_ASLEEP, MagmaHideout_4F_EventScript_MaxieChallenge
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
MagmaHideout_4F_EventScript_MaxieChallenge::
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	delay 30
	applymovement LOCALID_MAGMA_HIDEOUT_4F_MAXIE, Common_Movement_FacePlayer
	waitmovement 0
	msgbox MagmaHideout_4F_Text_MaxieOhItWasYou, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro_double TRAINER_MAXIE_MAGMA_HIDEOUT, MagmaHideout_4F_Text_MaxieDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_4F_Text_MaxieImGoingAfterGroudon, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_MAGMA_STONE
	special CheckEmeraldChampionsHandoffItem
	call_if_eq VAR_RESULT, FALSE, MagmaHideout_4F_EventScript_MissingStone
	msgbox MagmaHideout_4F_Text_ScorchedSlabLead, MSGBOX_DEFAULT
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
	removeobject LOCALID_EC_COURTNEY_MAGMA_HIDEOUT
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
	.string "COURTNEY: You reached the\n"
	.string "chamber.\p"
	.string "MAXIE must not be interrupted. I\n"
	.string "will stop you here.$"

MagmaHideout_4F_Text_CourtneyDefeat:
	.string "COURTNEY: Defeated… Again.$"

MagmaHideout_4F_Text_CourtneyAfter:
	.string "COURTNEY: MAXIE is just ahead.\p"
	.string "GROUDON… What will it do when it\n"
	.string "wakes?$"

MagmaHideout_4F_Text_AdminsStillStanding:
	.string "COURTNEY and TABITHA still hold this\n"
	.string "chamber. Face both before MAXIE.$"

MagmaHideout_4F_Text_Grunt11Intro:
	.string "MAXIE promised solid ground under\n"
	.string "every home in HOENN. I believe\l"
	.string "him.\p"
	.string "You're not getting into that\n"
	.string "chamber!$"

MagmaHideout_4F_Text_Grunt11Defeat:
	.string "You beat my whole team…$"

MagmaHideout_4F_Text_Grunt11PostBattle:
	.string "None of us have seen GROUDON.\n"
	.string "MAXIE keeps the last chamber\l"
	.string "closed.\p"
	.string "The mountain's shaking. I hope he\n"
	.string "knows what he's doing.$"

MagmaHideout_4F_Text_Grunt12Intro:
	.string "Can you feel the mountain\n"
	.string "shaking? That's GROUDON's power!\p"
	.string "MAXIE's waited years for this. I\n"
	.string "won't let you spoil it!$"

MagmaHideout_4F_Text_Grunt12Defeat:
	.string "I couldn't stop you…$"

MagmaHideout_4F_Text_Grunt12PostBattle:
	.string "MAXIE, sir! An intruder is headed for\n"
	.string "the chamber!$"

MagmaHideout_4F_Text_Grunt13Intro:
	.string "This is the last chamber. GROUDON\n"
	.string "is just beyond us.\p"
	.string "You want to stop MAXIE? You'll\n"
	.string "have to get past me!$"

MagmaHideout_4F_Text_Grunt13Defeat:
	.string "My team couldn't hold you back…$"

MagmaHideout_4F_Text_Grunt13PostBattle:
	.string "That tremor… Was it stronger than\n"
	.string "the last one?$"

MagmaHideout_4F_Text_TabithaIntro:
	.string "TABITHA: You've made it through\n"
	.string "the entire hideout. I should have\l"
	.string "expected it.\p"
	.string "MAXIE is with GROUDON. This is\n"
	.string "the moment we've worked for.\p"
	.string "I'm not letting you take it from\n"
	.string "us!$"

MagmaHideout_4F_Text_TabithaDefeat:
	.string "Even here, I couldn't defeat you…$"

MagmaHideout_4F_Text_TabithaPostBattle:
	.string "TABITHA: MAXIE is just ahead.\p"
	.string "The shaking keeps getting worse.\n"
	.string "What is happening in there?$"

MagmaHideout_4F_Text_MaxieAwakenGroudon:
	.string "MAXIE: GROUDON, ancient architect\n"
	.string "of continents... accept the BLUE\l"
	.string "ORB.\p"
	.string "The mountain shakes with your\n"
	.string "power. Rise, and give HOENN land\l"
	.string "that will never yield!$"

MagmaHideout_4F_Text_MaxieGroudonWhatsWrong:
	.string "MAXIE: GROUDON! What's wrong? The\n"
	.string "BLUE ORB was supposed to make\l"
	.string "you obey!\p"
	.string "Wait! Where are you going?$"

MagmaHideout_4F_Text_MaxieOhItWasYou:
	.string "MAXIE: You! Did you see that?\n"
	.string "GROUDON left without a single\l"
	.string "command.\p"
	.string "Don't look at me like that. You\n"
	.string "won't stop TEAM MAGMA!$"

MagmaHideout_4F_Text_MaxieDefeat:
	.string "What makes you so adept at handling\n"
	.string "POKéMON?$"

MagmaHideout_4F_Text_MaxieImGoingAfterGroudon:
	.string "MAXIE: GROUDON is loose. I must\n"
	.string "find out why the ORB failed.\p"
	.string "The MAGMA STONE reacted to our\n"
	.string "readings from SCORCHED SLAB.\l"
	.string "Something else answered down there.\p"
	.string "I'm going after GROUDON. You can\n"
	.string "follow the geological lead.$"

MagmaHideout_4F_EventScript_MissingStone::
	msgbox ScorchedSlab_HeatransRoom_Text_MissingStone, MSGBOX_DEFAULT
	return

MagmaHideout_4F_Text_ScorchedSlabLead:
	.string "From FORTREE's east exit, reach\n"
	.string "the lake on ROUTE 120. SURF to\l"
	.string "the cave entrance at SCORCHED SLAB.\p"
	.string "Descend through the basement\n"
	.string "floors to HEATRAN's chamber.\l"
	.string "Use the MAGMA STONE there.$"
