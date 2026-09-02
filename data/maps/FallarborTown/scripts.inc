FallarborTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, FallarborTown_OnTransition
	.byte 0

FallarborTown_OnTransition:
	setflag FLAG_VISITED_FALLARBOR_TOWN
	setvar VAR_CONTEST_HALL_STATE, 0
	clearflag FLAG_CONTEST_SKETCH_CREATED
	setflag FLAG_HIDE_FALLARBOR_RIVAL_ON_BIKE
	call_if_ne VAR_METEOR_FALLS_STATE, 0, FallarborTown_EventScript_MarkRivalSceneComplete
	call_if_eq VAR_FALLARBOR_TOWN_STATE, 1, FallarborTown_EventScript_HideRivals
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupRivalOnBikeGfxId
	end

FallarborTown_EventScript_MarkRivalSceneComplete:
	setvar VAR_FALLARBOR_TOWN_STATE, 1
	setflag FLAG_HIDE_FALLARBOR_RIVAL
	setflag FLAG_HIDE_FALLARBOR_RIVAL_ON_BIKE
	return

FallarborTown_EventScript_HideRivals:
	setflag FLAG_HIDE_FALLARBOR_RIVAL
	setflag FLAG_HIDE_FALLARBOR_RIVAL_ON_BIKE
	return

FallarborTown_EventScript_ExpertM::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, FallarborTown_EventScript_ExpertMNormal
	msgbox FallarborTown_Text_ShadyCharactersCozmosHome, MSGBOX_DEFAULT
	release
	end

FallarborTown_EventScript_ExpertMNormal::
	msgbox FallarborTown_Text_RegionKnownForMeteors, MSGBOX_DEFAULT
	release
	end

FallarborTown_EventScript_Girl::
	msgbox FallarborTown_Text_MyPreciousAzurill, MSGBOX_NPC
	end

FallarborTown_EventScript_Gentleman::
	msgbox FallarborTown_Text_HaveYouChallengedFlannery, MSGBOX_NPC
	end

FallarborTown_EventScript_Azurill::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_AZURILL, CRY_MODE_NORMAL
	msgbox FallarborTown_Text_Azurill, MSGBOX_DEFAULT
	waitmoncry
	release
	end

FallarborTown_EventScript_BattleTentSign::
	msgbox FallarborTown_Text_BattleTentSign, MSGBOX_SIGN
	end

FallarborTown_EventScript_TownSign::
	msgbox FallarborTown_Text_TownSign, MSGBOX_SIGN
	end

FallarborTown_EventScript_MoveTutorSign::
	msgbox FallarborTown_Text_MoveTutorSign, MSGBOX_SIGN
	end

FallarborTown_EventScript_RivalTrigger1::
	setvar VAR_0x8004, 1
	goto FallarborTown_EventScript_RivalScene
	end

FallarborTown_EventScript_RivalTrigger2::
	setvar VAR_0x8004, 2
	goto FallarborTown_EventScript_RivalScene
	end

FallarborTown_EventScript_RivalTrigger3::
	setvar VAR_0x8004, 3
	goto FallarborTown_EventScript_RivalScene
	end

FallarborTown_EventScript_RivalTrigger4::
	setvar VAR_0x8004, 4
	goto FallarborTown_EventScript_RivalScene
	end

FallarborTown_EventScript_RivalTrigger5::
	setvar VAR_0x8004, 5
	goto FallarborTown_EventScript_RivalScene
	end

FallarborTown_EventScript_RivalScene::
	lockall
	playse SE_PIN
	applymovement LOCALID_PLAYER, Common_Movement_ExclamationMark
	applymovement LOCALID_FALLARBOR_RIVAL, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_Delay48
	waitmovement 0
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, FallarborTown_EventScript_MayScene
	goto FallarborTown_EventScript_BrendanScene
	end

FallarborTown_EventScript_MayScene::
	playbgm MUS_ENCOUNTER_MAY, TRUE
	msgbox FallarborTown_Text_MayDisaster, MSGBOX_DEFAULT
	closemessage
	call FallarborTown_EventScript_MoveToPlayer
	msgbox FallarborTown_Text_MayWeirdos, MSGBOX_DEFAULT
	closemessage
	goto FallarborTown_EventScript_RivalExit
	end

FallarborTown_EventScript_BrendanScene::
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	msgbox FallarborTown_Text_BrendanComeHere, MSGBOX_DEFAULT
	closemessage
	call FallarborTown_EventScript_MoveToPlayer
	msgbox FallarborTown_Text_BrendanNuts, MSGBOX_DEFAULT
	closemessage
	goto FallarborTown_EventScript_RivalExit
	end

FallarborTown_EventScript_RivalExit::
	setobjectxyperm LOCALID_FALLARBOR_RIVAL_ON_BIKE, 12, 10
	setobjectmovementtype LOCALID_FALLARBOR_RIVAL, MOVEMENT_TYPE_FACE_DOWN
	setobjectmovementtype LOCALID_FALLARBOR_RIVAL_ON_BIKE, MOVEMENT_TYPE_FACE_DOWN
	setflag FLAG_HIDE_FALLARBOR_RIVAL
	removeobject LOCALID_FALLARBOR_RIVAL
	addobject LOCALID_FALLARBOR_RIVAL_ON_BIKE
	delay 45
	applymovement LOCALID_FALLARBOR_RIVAL_ON_BIKE, FallarborTown_Movement_RivalExit
	waitmovement 0
	removeobject LOCALID_FALLARBOR_RIVAL_ON_BIKE
	setvar VAR_FALLARBOR_TOWN_STATE, 1
	savebgm MUS_DUMMY
	fadedefaultbgm
	releaseall
	end

FallarborTown_EventScript_MoveToPlayer::
	call_if_eq VAR_0x8004, 1, FallarborTown_EventScript_PlayerApproach1
	call_if_eq VAR_0x8004, 2, FallarborTown_EventScript_PlayerApproach2
	call_if_eq VAR_0x8004, 3, FallarborTown_EventScript_PlayerApproach3
	call_if_eq VAR_0x8004, 4, FallarborTown_EventScript_PlayerApproach4
	call_if_eq VAR_0x8004, 5, FallarborTown_EventScript_PlayerApproach5
	return

FallarborTown_EventScript_PlayerApproach1::
	applymovement LOCALID_PLAYER, FallarborTown_Movement_PlayerApproach1
	applymovement LOCALID_FALLARBOR_RIVAL, FallarborTown_Movement_RivalApproach
	waitmovement 0
	return

FallarborTown_EventScript_PlayerApproach2::
	applymovement LOCALID_PLAYER, FallarborTown_Movement_PlayerApproach2
	applymovement LOCALID_FALLARBOR_RIVAL, FallarborTown_Movement_RivalApproach
	waitmovement 0
	return

FallarborTown_EventScript_PlayerApproach3::
	applymovement LOCALID_PLAYER, FallarborTown_Movement_PlayerApproach3
	applymovement LOCALID_FALLARBOR_RIVAL, FallarborTown_Movement_RivalApproach
	waitmovement 0
	return

FallarborTown_EventScript_PlayerApproach4::
	applymovement LOCALID_PLAYER, FallarborTown_Movement_PlayerApproach4
	applymovement LOCALID_FALLARBOR_RIVAL, FallarborTown_Movement_RivalApproach
	waitmovement 0
	return

FallarborTown_EventScript_PlayerApproach5::
	applymovement LOCALID_PLAYER, FallarborTown_Movement_PlayerApproach5
	applymovement LOCALID_FALLARBOR_RIVAL, FallarborTown_Movement_RivalApproach
	waitmovement 0
	return

FallarborTown_Movement_PlayerApproach1:
	walk_down
	walk_down
	face_left
	step_end

FallarborTown_Movement_PlayerApproach2:
	walk_down
	face_left
	step_end

FallarborTown_Movement_PlayerApproach3:
	face_left
	step_end

FallarborTown_Movement_PlayerApproach4:
	walk_up
	face_left
	step_end

FallarborTown_Movement_PlayerApproach5:
	walk_up
	walk_up
	face_left
	step_end

FallarborTown_Movement_RivalApproach:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

FallarborTown_Movement_RivalExit:
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	step_end

FallarborTown_Text_MayDisaster:
	.string "MAY: Oh! {PLAYER}{KUN}! Help!$"

FallarborTown_Text_BrendanComeHere:
	.string "BRENDAN: {PLAYER}! Come here!$"

FallarborTown_Text_MayWeirdos:
	.string "MAY: PROF. COZMO was abducted by\n"
	.string "TEAM MAGMA!\p"
	.string "I'm heading for METEOR FALLS now.\n"
	.string "Please catch up and help us!$"

FallarborTown_Text_BrendanNuts:
	.string "BRENDAN: PROF. COZMO was abducted by\n"
	.string "TEAM MAGMA!\p"
	.string "I'm heading for METEOR FALLS now.\n"
	.string "Catch up as soon as you can!$"

FallarborTown_Text_ShadyCharactersCozmosHome:
	.string "Something's happening,\n"
	.string "and I don't like it!\p"
	.string "I've seen shady characters wandering\n"
	.string "in and out of PROF. COZMO's home…$"

FallarborTown_Text_RegionKnownForMeteors:
	.string "This region's been known for meteors\n"
	.string "since the olden days.\p"
	.string "They say METEOR FALLS was gouged out\n"
	.string "by a falling meteorite long ago.$"

FallarborTown_Text_MyPreciousAzurill:
	.string "See! Take a look!\n"
	.string "This is my precious AZURILL!\p"
	.string "It's slick and smooth and plushy, too!$"

FallarborTown_Text_Azurill:
	.string "AZURILL: Rooreelooo.$"

FallarborTown_Text_HaveYouChallengedFlannery:
	.string "Have you already challenged FLANNERY,\n"
	.string "the LEADER of LAVARIDGE GYM?\p"
	.string "The girl's grandfather was famous.\n"
	.string "He was one of the ELITE FOUR in the\l"
	.string "POKéMON LEAGUE at one point.\p"
	.string "It wouldn't surprise me to see FLANNERY\n"
	.string "become a great TRAINER in her own\l"
	.string "right.$"

FallarborTown_Text_BattleTentSign:
	.string "BATTLE TENT FALLARBOR SITE\n"
	.string "“May the Greatest Teams Gather!”$"

FallarborTown_Text_TownSign:
	.string "FALLARBOR TOWN\n"
	.string "“A farm community with small gardens.”$"

FallarborTown_Text_MoveTutorSign:
	.string "HEART SCALE WORKSHOP\n"
	.string "“Scales polished into PP UPS.”$"
