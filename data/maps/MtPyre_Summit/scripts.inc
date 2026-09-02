MtPyre_Summit_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MtPyre_Summit_OnTransition
	.byte 0

.set LEGENDARY_SIGN_CRESSELIA_ID, 8
.set LEGENDARY_SIGN_DARKRAI_ID, 9
.set LEGENDARY_SIGN_DIALGA_ID, 10

MtPyre_Summit_OnTransition:
	call_if_eq VAR_MT_PYRE_STATE, 2, MtPyre_Summit_EventScript_SetArchieMaxiePositions
	end

MtPyre_Summit_EventScript_SetArchieMaxiePositions::
	setobjectxyperm LOCALID_MT_PYRE_SUMMIT_MAXIE, 23, 6
	setobjectxyperm LOCALID_MT_PYRE_SUMMIT_ARCHIE, 22, 6
	end

MtPyre_Summit_EventScript_TeamAquaTrigger0::
	lockall
	setvar VAR_0x8008, 0
	goto MtPyre_Summit_EventScript_TeamAquaExits
	end

MtPyre_Summit_EventScript_TeamAquaTrigger1::
	lockall
	setvar VAR_0x8008, 1
	goto MtPyre_Summit_EventScript_TeamAquaExits
	end

MtPyre_Summit_EventScript_TeamAquaTrigger2::
	lockall
	setvar VAR_0x8008, 2
	goto MtPyre_Summit_EventScript_TeamAquaExits
	end

MtPyre_Summit_EventScript_TeamAquaExits::
	playbgm MUS_ENCOUNTER_AQUA, FALSE
	applymovement LOCALID_PLAYER, Common_Movement_FaceUp
	waitmovement 0
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 50
	call_if_eq VAR_0x8008, 0, MtPyre_Summit_EventScript_ArchieFacePlayer0
	call_if_eq VAR_0x8008, 1, MtPyre_Summit_EventScript_ArchieFacePlayer1
	call_if_eq VAR_0x8008, 2, MtPyre_Summit_EventScript_ArchieFacePlayer2
	msgbox MtPyre_Summit_Text_ArchieWeGotTheOrbLetsGo, MSGBOX_DEFAULT
	closemessage
	fadescreenswapbuffers FADE_TO_BLACK
	removeobject LOCALID_MT_PYRE_SUMMIT_ARCHIE
	removeobject LOCALID_MT_PYRE_SUMMIT_GRUNT_1
	removeobject LOCALID_MT_PYRE_SUMMIT_GRUNT_2
	removeobject LOCALID_MT_PYRE_SUMMIT_GRUNT_3
	removeobject LOCALID_MT_PYRE_SUMMIT_GRUNT_4
	removeobject LOCALID_EC_MATT_MT_PYRE
	setflag FLAG_HIDE_MT_PYRE_SUMMIT_ARCHIE
	setflag FLAG_HIDE_MT_PYRE_SUMMIT_TEAM_AQUA
	fadedefaultbgm
	fadescreenswapbuffers FADE_FROM_BLACK
	delay 20
	setvar VAR_MT_PYRE_STATE, 1
	call_if_eq VAR_0x8008, 0, MtPyre_Summit_EventScript_OldLadyApproachPlayer0
	call_if_eq VAR_0x8008, 1, MtPyre_Summit_EventScript_OldLadyApproachPlayer1
	call_if_eq VAR_0x8008, 2, MtPyre_Summit_EventScript_OldLadyApproachPlayer2
	msgbox MtPyre_Summit_Text_BothOrbsTakenMagmaLeftThis, MSGBOX_DEFAULT
	giveitem ITEM_MAGMA_EMBLEM
	goto_if_eq VAR_RESULT, FALSE, MtPyre_Summit_EventScript_ArchiveMagmaEmblem
	goto MtPyre_Summit_EventScript_ReceivedMagmaEmblem
	end

MtPyre_Summit_EventScript_ArchiveMagmaEmblem::
	addpcitem ITEM_MAGMA_EMBLEM
	goto_if_eq VAR_RESULT, FALSE, MtPyre_Summit_EventScript_MagmaEmblemNoRoom
	msgbox MtPyre_Summit_Text_MagmaEmblemSentToPC, MSGBOX_DEFAULT

MtPyre_Summit_EventScript_ReceivedMagmaEmblem::
	setflag FLAG_RECEIVED_RED_OR_BLUE_ORB
	setflag FLAG_HIDE_JAGGED_PASS_MAGMA_GUARD
	releaseall
	end

MtPyre_Summit_EventScript_MagmaEmblemNoRoom::
	msgbox MtPyre_Summit_Text_MagmaEmblemNoRoom, MSGBOX_DEFAULT
	releaseall
	end

MtPyre_Summit_EventScript_ArchieFacePlayer0::
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, MtPyre_Summit_Movement_ArchieFacePlayer0
	waitmovement 0
	return

@ Archie is already facing player
MtPyre_Summit_EventScript_ArchieFacePlayer1::
	return

MtPyre_Summit_EventScript_ArchieFacePlayer2::
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, MtPyre_Summit_Movement_ArchieFacePlayer2
	waitmovement 0
	return

MtPyre_Summit_EventScript_OldLadyApproachPlayer0::
	applymovement LOCALID_MT_PYRE_SUMMIT_OLD_LADY, MtPyre_Summit_Movement_OldLadyApproachPlayer0
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	return

MtPyre_Summit_EventScript_OldLadyApproachPlayer1::
	applymovement LOCALID_MT_PYRE_SUMMIT_OLD_LADY, MtPyre_Summit_Movement_OldLadyApproachPlayer1
	waitmovement 0
	return

MtPyre_Summit_EventScript_OldLadyApproachPlayer2::
	applymovement LOCALID_MT_PYRE_SUMMIT_OLD_LADY, MtPyre_Summit_Movement_OldLadyApproachPlayer2
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	return

MtPyre_Summit_Movement_OldLadyApproachPlayer0:
	walk_down
	walk_down
	walk_in_place_faster_left
	step_end

MtPyre_Summit_Movement_OldLadyApproachPlayer1:
	walk_down
	step_end

MtPyre_Summit_Movement_OldLadyApproachPlayer2:
	walk_down
	walk_down
	walk_in_place_faster_right
	step_end

MtPyre_Summit_Movement_ArchieFacePlayer0:
	walk_left
	walk_in_place_faster_down
	step_end

MtPyre_Summit_Movement_ArchieFacePlayer2:
	walk_right
	walk_in_place_faster_down
	step_end

MtPyre_Summit_EventScript_OldMan::
	lock
	faceplayer
	goto_if_set FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, MtPyre_Summit_EventScript_OldManAfterRayquaza
	msgbox MtPyre_Summit_Text_WillYouHearOutMyTale, MSGBOX_YESNO
	call_if_eq VAR_RESULT, YES, MtPyre_Summit_EventScript_OldManTale
	call_if_eq VAR_RESULT, NO, MtPyre_Summit_EventScript_DeclineOldManTale
	call MtPyre_Summit_EventScript_LegendarySign
	release
	end

MtPyre_Summit_EventScript_OldManAfterRayquaza::
	msgbox MtPyre_Summit_Text_HearTheNewLegendOfHoenn, MSGBOX_YESNO
	call_if_eq VAR_RESULT, YES, MtPyre_Summit_EventScript_OldManNewTale
	call_if_eq VAR_RESULT, NO, MtPyre_Summit_EventScript_DeclineOldManTale
	call MtPyre_Summit_EventScript_LegendarySign
	release
	end

MtPyre_Summit_EventScript_OldManTale::
	msgbox MtPyre_Summit_Text_GroudonKyogreTale, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_DeclineOldManTale::
	msgbox MtPyre_Summit_Text_WellThatTooIsFine, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_OldManNewTale::
	msgbox MtPyre_Summit_Text_HoennTrioTale, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_LegendarySign::
	goto_if_unset FLAG_RECEIVED_RED_OR_BLUE_ORB, MtPyre_Summit_EventScript_LegendarySignReturn
	special TryUnlockDarkraiLegendarySign
	goto_if_eq VAR_RESULT, 0, MtPyre_Summit_EventScript_DarkraiNeedsTrainers
	goto_if_eq VAR_RESULT, 1, MtPyre_Summit_EventScript_DarkraiNeedsMusharna
	goto_if_eq VAR_RESULT, 2, MtPyre_Summit_EventScript_DarkraiAwakened
	goto_if_eq VAR_RESULT, 3, MtPyre_Summit_EventScript_DarkraiAwaits
	setvar VAR_0x8004, LEGENDARY_SIGN_CRESSELIA_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 2, MtPyre_Summit_EventScript_CresseliaAwakened
	goto_if_eq VAR_RESULT, 4, MtPyre_Summit_EventScript_TryDialgaSign
	msgbox MtPyre_Summit_Text_CresseliaAwaits, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_TryDialgaSign::
	goto_if_unset FLAG_SYS_GAME_CLEAR, MtPyre_Summit_EventScript_TimeSignDormant
	setvar VAR_0x8004, LEGENDARY_SIGN_DIALGA_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 2, MtPyre_Summit_EventScript_DialgaAwakened
	goto_if_eq VAR_RESULT, 4, MtPyre_Summit_EventScript_DreamChainComplete
	msgbox MtPyre_Summit_Text_DialgaAwaits, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_DarkraiNeedsTrainers::
	msgbox MtPyre_Summit_Text_DarkraiNeedsTrainers, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_DarkraiNeedsMusharna::
	msgbox MtPyre_Summit_Text_DarkraiNeedsMusharna, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_DarkraiAwakened::
	msgbox MtPyre_Summit_Text_DarkraiAwakened, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_DarkraiAwaits::
	msgbox MtPyre_Summit_Text_DarkraiAwaits, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_CresseliaAwakened::
	msgbox MtPyre_Summit_Text_CresseliaAwakened, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_TimeSignDormant::
	msgbox MtPyre_Summit_Text_TimeSignDormant, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_DialgaAwakened::
	msgbox MtPyre_Summit_Text_DialgaAwakened, MSGBOX_DEFAULT
	return

MtPyre_Summit_EventScript_DreamChainComplete::
	msgbox MtPyre_Summit_Text_DreamChainComplete, MSGBOX_DEFAULT
MtPyre_Summit_EventScript_LegendarySignReturn::
	return

MtPyre_Summit_EventScript_Darkrai::
	lock
	setvar VAR_0x8004, LEGENDARY_SIGN_DARKRAI_ID
	special GetSelectedLegendarySignState
	goto_if_eq VAR_RESULT, 0, MtPyre_Summit_EventScript_DarkraiDormant
	goto_if_eq VAR_RESULT, 2, MtPyre_Summit_EventScript_DarkraiQuiet
	waitse
	playmoncry SPECIES_DARKRAI, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, LEGENDARY_SIGN_DARKRAI_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MtPyre_Summit_EventScript_DarkraiCaught
	msgbox MtPyre_Summit_Text_DarkraiRemains, MSGBOX_DEFAULT
	release
	end

MtPyre_Summit_EventScript_DarkraiCaught::
	release
	end

MtPyre_Summit_EventScript_DarkraiDormant::
	msgbox MtPyre_Summit_Text_DarkraiDormant, MSGBOX_DEFAULT
	release
	end

MtPyre_Summit_EventScript_DarkraiQuiet::
	msgbox MtPyre_Summit_Text_DarkraiQuiet, MSGBOX_DEFAULT
	release
	end

MtPyre_Summit_Text_DarkraiDormant:
	.string "The summit air is still. A black SIGN\n"
	.string "has not yet accepted your challenge.$"

MtPyre_Summit_Text_DarkraiQuiet:
	.string "The nightmare has lifted. The black\n"
	.string "SIGN records the trial you completed.$"

MtPyre_Summit_Text_DarkraiNeedsTrainers:
	.string "A Legendary Sign stains this peak.\p"
	.string "Face every Trainer within Mt. Pyre,\n"
	.string "then return with MUSHARNA.$"

MtPyre_Summit_Text_DarkraiNeedsMusharna:
	.string "Every spirit here acknowledges you.\p"
	.string "But the Sign answers only a dream.\n"
	.string "Return with MUSHARNA.$"

MtPyre_Summit_Text_DarkraiAwakened:
	.string "The Legendary Sign has awakened!\p"
	.string "Nightmare mist now gathers at the\n"
	.string "summit's black mark.$"

MtPyre_Summit_Text_DarkraiAwaits:
	.string "Nightmare mist still gathers around\n"
	.string "the summit's black mark.$"

MtPyre_Summit_Text_DarkraiRemains:
	.string "The nightmare Sign still waits.\n"
	.string "DARKRAI will await your return.$"

MtPyre_Summit_Text_CresseliaAwakened:
	.string "DARKRAI's capture completes one\n"
	.string "half of the dream.\p"
	.string "A second Sign shines in the deepest\n"
	.string "chamber of Meteor Falls.$"

MtPyre_Summit_Text_CresseliaAwaits:
	.string "The lunar Sign still shines in the\n"
	.string "deepest chamber of Meteor Falls.$"

MtPyre_Summit_Text_TimeSignDormant:
	.string "Dream and nightmare are now bound.\p"
	.string "A final Sign sleeps until you are\n"
	.string "recognized as Champion.$"

MtPyre_Summit_Text_DialgaAwakened:
	.string "The final Legendary Sign rings\n"
	.string "through Meteor Falls.\p"
	.string "Seek the chamber where time stands\n"
	.string "alone.$"

MtPyre_Summit_Text_DialgaAwaits:
	.string "The Sign of time still waits in the\n"
	.string "hidden depths of Meteor Falls.$"

MtPyre_Summit_Text_DreamChainComplete:
	.string "Nightmare, dream, and time have all\n"
	.string "answered you. Their legend is yours.$"

MtPyre_Summit_EventScript_OldLady::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_RED_OR_BLUE_ORB, MtPyre_Summit_EventScript_OldLadyAfterEmblem
	goto_if_eq VAR_MT_PYRE_STATE, 1, MtPyre_Summit_EventScript_RetryMagmaEmblem

MtPyre_Summit_EventScript_OldLadyAfterEmblem::
	goto_if_set FLAG_RETURNED_RED_OR_BLUE_ORB, MtPyre_Summit_EventScript_OldLadyAfterOrbsReturned
	call_if_ge VAR_MT_PYRE_STATE, 3, MtPyre_Summit_EventScript_OldLadyOrbsReturned
	goto_if_set FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, MtPyre_Summit_EventScript_OldLadyLegendariesAwake
	msgbox MtPyre_Summit_Text_OrbsHaveBeenTaken, MSGBOX_DEFAULT
	release
	end

MtPyre_Summit_EventScript_RetryMagmaEmblem::
	msgbox MtPyre_Summit_Text_MagmaEmblemWaiting, MSGBOX_DEFAULT
	giveitem ITEM_MAGMA_EMBLEM
	goto_if_eq VAR_RESULT, FALSE, MtPyre_Summit_EventScript_ArchiveMagmaEmblem
	goto MtPyre_Summit_EventScript_ReceivedMagmaEmblem
	end

MtPyre_Summit_EventScript_OldLadyOrbsReturned::
	msgbox MtPyre_Summit_Text_ThoseTwoMenReturnedOrbs, MSGBOX_DEFAULT
	setflag FLAG_RETURNED_RED_OR_BLUE_ORB
	release
	end

MtPyre_Summit_EventScript_OldLadyLegendariesAwake::
	msgbox MtPyre_Summit_Text_GroudonKyogreAwakened, MSGBOX_DEFAULT
	release
	end

MtPyre_Summit_EventScript_OldLadyAfterOrbsReturned::
	msgbox MtPyre_Summit_Text_SuperAncientPokemonTaughtUs, MSGBOX_DEFAULT
	release
	end

MtPyre_Summit_EventScript_ArchieMaxieTrigger0::
	lockall
	setvar VAR_0x8008, 0
	goto MtPyre_Summit_EventScript_ArchieMaxieReturnOrbs
	end

MtPyre_Summit_EventScript_ArchieMaxieTrigger1::
	lockall
	setvar VAR_0x8008, 1
	goto MtPyre_Summit_EventScript_ArchieMaxieReturnOrbs
	end

MtPyre_Summit_EventScript_ArchieMaxieTrigger2::
	lockall
	setvar VAR_0x8008, 2
	goto MtPyre_Summit_EventScript_ArchieMaxieReturnOrbs
	end

MtPyre_Summit_EventScript_ArchieMaxieReturnOrbs::
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	delay 60
	call_if_eq VAR_0x8008, 0, MtPyre_Summit_EventScript_ArchieMaxieBeginExit0
	call_if_eq VAR_0x8008, 1, MtPyre_Summit_EventScript_ArchieMaxieBeginExit1
	call_if_eq VAR_0x8008, 2, MtPyre_Summit_EventScript_ArchieMaxieBeginExit2
	playse SE_PIN
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, Common_Movement_Delay48
	waitmovement 0
	delay 30
	call_if_eq VAR_0x8008, 0, MtPyre_Summit_EventScript_MaxieApproachPlayer0
	call_if_eq VAR_0x8008, 1, MtPyre_Summit_EventScript_MaxieApproachPlayer1
	call_if_eq VAR_0x8008, 2, MtPyre_Summit_EventScript_MaxieApproachPlayer2
	msgbox MtPyre_Summit_Text_MaxieSilence, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8008, 0, MtPyre_Summit_EventScript_MaxieApproachArchie0
	call_if_eq VAR_0x8008, 1, MtPyre_Summit_EventScript_MaxieApproachArchie1
	call_if_eq VAR_0x8008, 2, MtPyre_Summit_EventScript_MaxieApproachArchie2
	delay 30
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, MtPyre_Summit_Movement_ArchieExit
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieExit
	waitmovement 0
	removeobject LOCALID_MT_PYRE_SUMMIT_ARCHIE
	removeobject LOCALID_MT_PYRE_SUMMIT_MAXIE
	setvar VAR_MT_PYRE_STATE, 3
	releaseall
	end

MtPyre_Summit_EventScript_ArchieMaxieBeginExit0::
	applymovement LOCALID_PLAYER, MtPyre_Summit_Movement_PlayerWatchArchieMaxieExit0
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, MtPyre_Summit_Movement_ArchieExit
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieExit
	waitmovement 0
	return

MtPyre_Summit_EventScript_ArchieMaxieBeginExit1::
	applymovement LOCALID_PLAYER, MtPyre_Summit_Movement_PlayerWatchArchieMaxieExit1
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, MtPyre_Summit_Movement_ArchieExit
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieExit
	waitmovement 0
	return

MtPyre_Summit_EventScript_ArchieMaxieBeginExit2::
	applymovement LOCALID_PLAYER, MtPyre_Summit_Movement_PlayerWatchArchieMaxieExit2
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, MtPyre_Summit_Movement_ArchieExit
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieExit
	waitmovement 0
	return

MtPyre_Summit_EventScript_MaxieApproachPlayer0::
	applymovement LOCALID_PLAYER, MtPyre_Summit_Movement_PlayerFaceMaxie0
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, MtPyre_Summit_Movement_ArchieWatchMaxie
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieApproachPlayer0
	waitmovement 0
	return

MtPyre_Summit_EventScript_MaxieApproachPlayer1::
	applymovement LOCALID_PLAYER, MtPyre_Summit_Movement_PlayerFaceMaxie
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, MtPyre_Summit_Movement_ArchieWatchMaxie
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieApproachPlayer1
	waitmovement 0
	return

MtPyre_Summit_EventScript_MaxieApproachPlayer2::
	applymovement LOCALID_PLAYER, MtPyre_Summit_Movement_PlayerFaceMaxie
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, MtPyre_Summit_Movement_ArchieWatchMaxie
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieApproachPlayer2
	waitmovement 0
	return

MtPyre_Summit_EventScript_MaxieApproachArchie0::
	applymovement LOCALID_PLAYER, MtPyre_Summit_Movement_PlayerWatchMaxie
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieApproachArchie0
	waitmovement 0
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	return

MtPyre_Summit_EventScript_MaxieApproachArchie1::
	applymovement LOCALID_PLAYER, MtPyre_Summit_Movement_PlayerWatchMaxie
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieApproachArchie1
	waitmovement 0
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	return

MtPyre_Summit_EventScript_MaxieApproachArchie2::
	applymovement LOCALID_PLAYER, MtPyre_Summit_Movement_PlayerWatchMaxie
	applymovement LOCALID_MT_PYRE_SUMMIT_MAXIE, MtPyre_Summit_Movement_MaxieApproachArchie2
	waitmovement 0
	applymovement LOCALID_MT_PYRE_SUMMIT_ARCHIE, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	return

MtPyre_Summit_Movement_PlayerFaceMaxie0:
	delay_16
	delay_16
	walk_in_place_faster_right
	step_end

MtPyre_Summit_Movement_PlayerFaceMaxie:
	delay_16
	delay_16
	walk_in_place_faster_left
	step_end

MtPyre_Summit_Movement_ArchieExit:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtPyre_Summit_Movement_MaxieExit:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MtPyre_Summit_Movement_PlayerWatchArchieMaxieExit0:
	delay_16
	delay_8
	walk_left
	walk_in_place_faster_right
	delay_16
	walk_in_place_faster_down
	step_end

MtPyre_Summit_Movement_PlayerWatchArchieMaxieExit1:
	delay_16
	delay_8
	walk_right
	walk_in_place_faster_left
	delay_16
	walk_in_place_faster_down
	step_end

MtPyre_Summit_Movement_PlayerWatchArchieMaxieExit2:
	delay_16
	delay_8
	walk_in_place_faster_left
	delay_16
	walk_in_place_faster_down
	step_end

MtPyre_Summit_Movement_MaxieApproachPlayer0:
	walk_up
	walk_up
	walk_up
	walk_left
	step_end

MtPyre_Summit_Movement_MaxieApproachPlayer1:
	walk_up
	walk_up
	walk_up
	walk_in_place_faster_right
	step_end

MtPyre_Summit_Movement_MaxieApproachPlayer2:
	walk_up
	walk_up
	walk_up
	walk_in_place_faster_right
	step_end

MtPyre_Summit_Movement_ArchieWatchMaxie:
	delay_16
	walk_in_place_faster_up
	step_end

MtPyre_Summit_Movement_PlayerWatchMaxie:
	delay_16
	walk_in_place_faster_down
	step_end

MtPyre_Summit_Movement_MaxieApproachArchie0:
	walk_down
	walk_right
	walk_down
	walk_down
	walk_in_place_faster_left
	step_end

MtPyre_Summit_Movement_MaxieApproachArchie1:
	walk_down
	walk_down
	walk_down
	walk_in_place_faster_left
	step_end

MtPyre_Summit_Movement_MaxieApproachArchie2:
	walk_down
	walk_down
	walk_down
	walk_in_place_faster_left
	step_end

MtPyre_Summit_EventScript_Grunt1::
	trainerbattle_double TRAINER_GRUNT_MT_PYRE_1, MtPyre_Summit_Text_Grunt1Intro, MtPyre_Summit_Text_Grunt1Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MtPyre_Summit_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_Summit_EventScript_Grunt2::
	trainerbattle_double TRAINER_GRUNT_MT_PYRE_2, MtPyre_Summit_Text_Grunt2Intro, MtPyre_Summit_Text_Grunt2Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MtPyre_Summit_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_Summit_EventScript_Grunt3::
	trainerbattle_double TRAINER_GRUNT_MT_PYRE_3, MtPyre_Summit_Text_Grunt3Intro, MtPyre_Summit_Text_Grunt3Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MtPyre_Summit_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_Summit_EventScript_Grunt4::
	trainerbattle_double TRAINER_GRUNT_MT_PYRE_4, MtPyre_Summit_Text_Grunt4Intro, MtPyre_Summit_Text_Grunt4Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MtPyre_Summit_Text_Grunt4PostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_Summit_EventScript_Matt::
	trainerbattle_double TRAINER_MATT_MT_PYRE, MtPyre_Summit_Text_MattIntro, MtPyre_Summit_Text_MattDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MtPyre_Summit_Text_MattAfter, MSGBOX_AUTOCLOSE
	end

MtPyre_Summit_Text_MattIntro:
	.string "MATT: Rain does more than boost Water. It\n"
	.string "heals, redirects Fire, and changes tempo.\p"
	.string "Break AQUA's formation before the wind!$"

MtPyre_Summit_Text_MattDefeat:
	.string "You kept one answer outside the current.$"

MtPyre_Summit_Text_MattAfter:
	.string "ARCHIE is beyond us with the RED ORB. If\n"
	.string "you follow, expect the sea to fight back.$"

MtPyre_Summit_Text_Grunt1Intro:
	.string "MAGMA reached the ORBS first, but their\n"
	.string "fixed formation left a path behind them.\p"
	.string "AQUA will take that path and the ORB.$"

MtPyre_Summit_Text_Grunt1Defeat:
	.string "You changed course inside my attack…$"

MtPyre_Summit_Text_Grunt1PostBattle:
	.string "One current broke. Others still run\n"
	.string "between you and ARCHIE.$"

MtPyre_Summit_Text_Grunt2Intro:
	.string "MAGMA guards one lane and calls it order.\n"
	.string "AQUA attacks from every open current.\p"
	.string "Read this one before it carries you away.$"

MtPyre_Summit_Text_Grunt2Defeat:
	.string "You found still water inside the rush…$"

MtPyre_Summit_Text_Grunt2PostBattle:
	.string "You keep interfering, but have you\n"
	.string "decided what the ORBS should protect?$"

MtPyre_Summit_Text_Grunt3Intro:
	.string "We saw you at MT. CHIMNEY.\p"
	.string "You reject MAGMA and still pursue AQUA.\n"
	.string "Then show me the answer you chose.$"

MtPyre_Summit_Text_Grunt3Defeat:
	.string "You refused both currents…$"

MtPyre_Summit_Text_Grunt3PostBattle:
	.string "The battle still bought ARCHIE time.\n"
	.string "The RED ORB is already within reach.$"

MtPyre_Summit_Text_Grunt4Intro:
	.string "You adapt too freely for MAGMA and\n"
	.string "question too much for AQUA.\p"
	.string "A lone Trainer should not be able to\n"
	.string "break two organizations. Prove me wrong.$"

MtPyre_Summit_Text_Grunt4Defeat:
	.string "You needed neither uniform…$"

MtPyre_Summit_Text_Grunt4PostBattle:
	.string "Our crews make ARCHIE's plans possible.\n"
	.string "Your team did the same for yours.$"

MtPyre_Summit_Text_ArchieWeGotTheOrbLetsGo:
	.string "ARCHIE: MAXIE took the BLUE ORB, but the\n"
	.string "RED ORB belongs to AQUA now.\p"
	.string "The wardens say the ORBS once quieted\n"
	.string "land, sea, and every shining SIGN.\p"
	.string "Then the sea can use that same network\n"
	.string "to wake. AQUA, move out!$"

MtPyre_Summit_Text_BothOrbsTakenMagmaLeftThis:
	.string "Both ORBS are gone. They were never\n"
	.string "weapons; together they completed a seal.\p"
	.string "The CHAMPION'S SIGNS are that seal's\n"
	.string "outer pattern, spread across HOENN.\p"
	.string "MAGMA dropped this EMBLEM in its haste.\n"
	.string "Take it. You may need every path open.$"

MtPyre_Summit_Text_MagmaEmblemSentToPC:
	.string "Your KEY ITEMS pocket is full, so the\n"
	.string "MAGMA EMBLEM was sent to your PC.$"

MtPyre_Summit_Text_MagmaEmblemWaiting:
	.string "The MAGMA EMBLEM is still waiting for\n"
	.string "you. Make room and take it now.$"

MtPyre_Summit_Text_MagmaEmblemNoRoom:
	.string "Both your KEY ITEMS pocket and PC are\n"
	.string "full. Make room, then speak to me again.$"

MtPyre_Summit_Text_OrbsHaveBeenTaken:
	.string "With the ORBS apart, land and sea will\n"
	.string "pull against the same ancient network.\p"
	.string "Only the guardian of the sky ever stood\n"
	.string "outside that struggle.$"

MtPyre_Summit_Text_GroudonKyogreAwakened:
	.string "Oh, my goodness…\p"
	.string "You say that both GROUDON and KYOGRE\n"
	.string "have been awakened?\p"
	.string "It is said that the BLUE ORB and\n"
	.string "the RED ORB quelled the rage of\l"
	.string "the two POKéMON…\p"
	.string "It is also said that the true owner\n"
	.string "of the ORBS still exists.\p"
	.string "However, no one knows who, or what\n"
	.string "exactly, the ORBS belong to.\p"
	.string "It could be a human, or perhaps\n"
	.string "a POKéMON, but no one knows.$"

MtPyre_Summit_Text_ThoseTwoMenReturnedOrbs:
	.string "The two men who took the ORBS came\n"
	.string "back to return them on their own.\p"
	.string "Those men…\n"
	.string "Perhaps they are not so evil after all…$"

MtPyre_Summit_Text_SuperAncientPokemonTaughtUs:
	.string "The embodiments of the land, sea,\n"
	.string "and the sky…\p"
	.string "That is said to be the identities of\n"
	.string "the super-ancient POKéMON…\p"
	.string "In the same way that we humans and\n"
	.string "POKéMON have life, our world, too,\l"
	.string "is alive…\p"
	.string "The super-ancient POKéMON have\n"
	.string "taught us that…$"

MtPyre_Summit_Text_WillYouHearOutMyTale:
	.string "MT. PYRE is where we calm the spirits\n"
	.string "of POKéMON that have passed on…\p"
	.string "This is a high place where one can\n"
	.string "see all of HOENN…\p"
	.string "The disembodied spirits of POKéMON\n"
	.string "must find solace here…\p"
	.string "Is it coincidence that you are here…\p"
	.string "Will you hear out my tale? A tale long\n"
	.string "told in the HOENN region?$"

MtPyre_Summit_Text_GroudonKyogreTale:
	.string "Long ago, GROUDON raised the land and\n"
	.string "KYOGRE carved the sea. Their clash grew\l"
	.string "until neither could stop itself.\p"
	.string "Ancient Trainers formed a network of\n"
	.string "SIGNS with trusted POKéMON as its keys.\p"
	.string "The RED and BLUE ORBS joined that net,\n"
	.string "but RAYQUAZA supplied its balance.\p"
	.string "The ORBS calmed the rivals, the SIGNS\n"
	.string "fell dark, and the sky guardian departed.\p"
	.string "The exterior still bears lesser marks:\n"
	.string "one sketches GARDEVOIR, one ABSOL.$"

MtPyre_Summit_Text_WellThatTooIsFine:
	.string "I see…\n"
	.string "Well, that, too, is fine…$"

MtPyre_Summit_Text_MaxieSilence:
	.string "MAXIE: {PLAYER}…\p"
	.string "… … … … … …\n"
	.string "… … … … … …$"

MtPyre_Summit_Text_HearTheNewLegendOfHoenn:
	.string "The old legend named three great powers.\n"
	.string "SOOTOPOLIS added a fourth: the Trainer\l"
	.string "who understood why balance mattered.\p"
	.string "Will you hear HOENN's new legend?$"

MtPyre_Summit_Text_HoennTrioTale:
	.string "Land and sea became absolute, and HOENN\n"
	.string "began to break between them.\p"
	.string "A green POKéMON descended from the sky,\n"
	.string "but it answered a mortal Trainer's call.\p"
	.string "RAYQUAZA restored the board. The Trainer\n"
	.string "restored the reason to keep playing.\p"
	.string "Each awakened SIGN waits to see what\n"
	.string "kind of Champion that Trainer becomes.\p"
	.string "On the sixth floor, a broken-world mark\n"
	.string "now sketches DUSKNOIR.$"
