MossdeepCity_SpaceCenter_2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MossdeepCity_SpaceCenter_2F_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, MossdeepCity_SpaceCenter_2F_OnFrame
	.byte 0

MossdeepCity_SpaceCenter_2F_OnTransition:
	call_if_eq VAR_MOSSDEEP_CITY_STATE, 2, MossdeepCity_SpaceCenter_2F_EventScript_MoveCivilians
	call_if_defeated TRAINER_GRUNT_SPACE_CENTER_5, MossdeepCity_SpaceCenter_2F_EventScript_MoveFirstDefeatedGrunt
	call_if_eq VAR_MOSSDEEP_SPACE_CENTER_STATE, 2, MossdeepCity_SpaceCenter_2F_EventScript_MoveDefeatedGrunts
	end

MossdeepCity_SpaceCenter_2F_EventScript_MoveFirstDefeatedGrunt:
	setobjectxyperm LOCALID_SPACE_CENTER_GRUNT_5, 13, 4
	return

MossdeepCity_SpaceCenter_2F_EventScript_MoveCivilians::
	clearflag FLAG_INTERACTED_WITH_STEVEN_SPACE_CENTER
	setobjectxyperm LOCALID_SPACE_CENTER_2F_SCIENTIST, 5, 3
	setobjectmovementtype LOCALID_SPACE_CENTER_2F_SCIENTIST, MOVEMENT_TYPE_FACE_RIGHT
	setobjectxyperm LOCALID_SPACE_CENTER_2F_RICH_BOY, 3, 2
	setobjectmovementtype LOCALID_SPACE_CENTER_2F_RICH_BOY, MOVEMENT_TYPE_FACE_RIGHT
	setobjectxyperm LOCALID_SPACE_CENTER_2F_GENTLEMAN, 1, 3
	setobjectmovementtype LOCALID_SPACE_CENTER_2F_GENTLEMAN, MOVEMENT_TYPE_FACE_RIGHT
	return

MossdeepCity_SpaceCenter_2F_EventScript_MoveDefeatedGrunts::
	setobjectxyperm LOCALID_SPACE_CENTER_GRUNT_6, 11, 2
	setobjectxyperm LOCALID_SPACE_CENTER_GRUNT_7, 15, 2
	setobjectxyperm LOCALID_SPACE_CENTER_GRUNT_5, 13, 4
	return

MossdeepCity_SpaceCenter_2F_OnFrame:
	map_script_2 VAR_MOSSDEEP_SPACE_CENTER_STATE, 1, MossdeepCity_SpaceCenter_2F_EventScript_ThreeMagmaGrunts
	.2byte 0

MossdeepCity_SpaceCenter_2F_EventScript_ThreeMagmaGrunts::
	call EC_RequireHideout
	call EC_RequireMind
	goto_if_defeated TRAINER_GRUNT_SPACE_CENTER_5, MossdeepCity_SpaceCenter_2F_EventScript_FirstLegComplete
	playse SE_PIN
	applymovement LOCALID_PLAYER, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_Delay48
	waitmovement 0
	msgbox MossdeepCity_SpaceCenter_2F_Text_YoureOutnumberedTakeUsOn, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, MossdeepCity_SpaceCenter_2F_EventScript_BattleThreeMagmaGrunts
	msgbox MossdeepCity_SpaceCenter_2F_Text_GoodAnswer, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, MossdeepCity_SpaceCenter_2F_Movement_PlayerExit
	waitmovement 0
	warp MAP_MOSSDEEP_CITY_SPACE_CENTER_1F, 13, 1
	waitstate
	releaseall
	end

MossdeepCity_SpaceCenter_2F_Movement_PlayerExit:
	walk_up
	step_end

MossdeepCity_SpaceCenter_2F_EventScript_BattleThreeMagmaGrunts::
	goto_if_defeated TRAINER_GRUNT_SPACE_CENTER_5, MossdeepCity_SpaceCenter_2F_EventScript_FirstLegComplete
	msgbox MossdeepCity_SpaceCenter_2F_Text_Grunt5Intro, MSGBOX_DEFAULT
	trainerbattle_no_intro_double TRAINER_GRUNT_SPACE_CENTER_5, MossdeepCity_SpaceCenter_2F_Text_Grunt5Defeat, EmeraldChampions_Text_NeedTwoPokemon
	applymovement LOCALID_SPACE_CENTER_GRUNT_5, MossdeepCity_SpaceCenter_2F_Movement_Grunt5Defeated
	waitmovement 0
MossdeepCity_SpaceCenter_2F_EventScript_FirstLegComplete:
	@ State 0 stops the entrance auto-trigger; the trainer flag retains this leg.
	setvar VAR_MOSSDEEP_SPACE_CENTER_STATE, 0
	copyobjectxytoperm LOCALID_SPACE_CENTER_GRUNT_5
	msgbox MossdeepCity_SpaceCenter_2F_Text_PauseBeforeLastGrunt, MSGBOX_DEFAULT
	releaseall
	end

MossdeepCity_SpaceCenter_2F_EventScript_BattleLastGrunt:
	call EventScript_CheckStoryDoubleBattleParty
	msgbox MossdeepCity_SpaceCenter_2F_Text_Grunt7Intro, MSGBOX_DEFAULT
	trainerbattle_no_intro_double_allow_single TRAINER_GRUNT_SPACE_CENTER_7, MossdeepCity_SpaceCenter_2F_Text_Grunt7Defeat
	applymovement LOCALID_SPACE_CENTER_GRUNT_6, MossdeepCity_SpaceCenter_2F_Movement_Grunt6Defeated
	waitmovement 0
	applymovement LOCALID_SPACE_CENTER_GRUNT_7, MossdeepCity_SpaceCenter_2F_Movement_Grunt7Defeated
	waitmovement 0
	setvar VAR_MOSSDEEP_SPACE_CENTER_STATE, 2
	copyobjectxytoperm LOCALID_SPACE_CENTER_GRUNT_6
	copyobjectxytoperm LOCALID_SPACE_CENTER_GRUNT_7
	copyobjectxytoperm LOCALID_SPACE_CENTER_GRUNT_5
	releaseall
	end

MossdeepCity_SpaceCenter_2F_Movement_Grunt6Defeated:
	lock_facing_direction
	walk_left
	unlock_facing_direction
	step_end

MossdeepCity_SpaceCenter_2F_Movement_Grunt5Defeated:
	lock_facing_direction
	walk_down
	unlock_facing_direction
	step_end

MossdeepCity_SpaceCenter_2F_Movement_Grunt7Defeated:
	lock_facing_direction
	walk_right
	unlock_facing_direction
	step_end

MossdeepCity_SpaceCenter_2F_EventScript_Scientist::
	lock
	faceplayer
	goto_if_ge VAR_MOSSDEEP_CITY_STATE, 3, MossdeepCity_SpaceCenter_2F_EventScript_ScientistNormal
	goto_if_eq VAR_MOSSDEEP_CITY_STATE, 2, MossdeepCity_SpaceCenter_2F_EventScript_ScientistMagma
	goto_if_lt VAR_MOSSDEEP_CITY_STATE, 2, MossdeepCity_SpaceCenter_2F_EventScript_ScientistNormal
	goto MossdeepCity_SpaceCenter_2F_EventScript_ScientistMagma
	end

MossdeepCity_SpaceCenter_2F_EventScript_ScientistNormal::
	goto_if_set FLAG_EC_REPORT_C42_COMPLETE, MossdeepCity_SpaceCenter_2F_EventScript_BirthIslandBriefing
	msgbox MossdeepCity_SpaceCenter_2F_Text_MossdeepIdealForRockets, MSGBOX_DEFAULT
	release
	end

MossdeepCity_SpaceCenter_2F_EventScript_ScientistMagma::
	msgbox MossdeepCity_SpaceCenter_2F_Text_WhyWouldMagmaStealRocketFuel, MSGBOX_DEFAULT
	release
	end

MossdeepCity_SpaceCenter_2F_EventScript_Gentleman::
	lock
	faceplayer
	goto_if_ge VAR_MOSSDEEP_CITY_STATE, 3, MossdeepCity_SpaceCenter_2F_EventScript_GentlemanNormal
	goto_if_eq VAR_MOSSDEEP_CITY_STATE, 2, MossdeepCity_SpaceCenter_2F_EventScript_GentlemanMagma
	goto_if_lt VAR_MOSSDEEP_CITY_STATE, 2, MossdeepCity_SpaceCenter_2F_EventScript_GentlemanNormal
	goto MossdeepCity_SpaceCenter_2F_EventScript_GentlemanMagma
	end

MossdeepCity_SpaceCenter_2F_EventScript_GentlemanNormal::
	msgbox MossdeepCity_SpaceCenter_2F_Text_WouldveLikedToBeAstronaut, MSGBOX_DEFAULT
	release
	end

MossdeepCity_SpaceCenter_2F_EventScript_GentlemanMagma::
	msgbox MossdeepCity_SpaceCenter_2F_Text_MagmaCantGetAwayWithThis, MSGBOX_DEFAULT
	release
	end

MossdeepCity_SpaceCenter_2F_EventScript_RichBoy::
	lock
	faceplayer
	goto_if_ge VAR_MOSSDEEP_CITY_STATE, 3, MossdeepCity_SpaceCenter_2F_EventScript_RichBoyNormal
	goto_if_eq VAR_MOSSDEEP_CITY_STATE, 2, MossdeepCity_SpaceCenter_2F_EventScript_RichBoyMagma
	goto_if_lt VAR_MOSSDEEP_CITY_STATE, 2, MossdeepCity_SpaceCenter_2F_EventScript_RichBoyNormal
	goto MossdeepCity_SpaceCenter_2F_EventScript_RichBoyMagma
	end

MossdeepCity_SpaceCenter_2F_EventScript_RichBoyNormal::
	msgbox MossdeepCity_SpaceCenter_2F_Text_WishOrdinaryPeopleCouldGoIntoSpace, MSGBOX_DEFAULT
	release
	end

MossdeepCity_SpaceCenter_2F_EventScript_RichBoyMagma::
	msgbox MossdeepCity_SpaceCenter_2F_Text_DoesMagmaWantToGoToSpace, MSGBOX_DEFAULT
	release
	end

@ The two retained guards have separate battles; the middle grunt is an NPC.
MossdeepCity_SpaceCenter_2F_EventScript_Grunt6::
	msgbox MossdeepCity_SpaceCenter_2F_Text_Grunt6PostBattle, MSGBOX_NPC
	end

MossdeepCity_SpaceCenter_2F_EventScript_Grunt7::
	lock
	faceplayer
	goto_if_defeated TRAINER_GRUNT_SPACE_CENTER_7, MossdeepCity_SpaceCenter_2F_EventScript_LastGruntDone
	goto_if_not_defeated TRAINER_GRUNT_SPACE_CENTER_5, MossdeepCity_SpaceCenter_2F_EventScript_FirstGuardWaiting
	msgbox MossdeepCity_SpaceCenter_2F_Text_ReadyForLastGrunt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, MossdeepCity_SpaceCenter_2F_EventScript_BattleLastGrunt
	msgbox MossdeepCity_SpaceCenter_2F_Text_PauseBeforeLastGrunt, MSGBOX_DEFAULT
	releaseall
	end
MossdeepCity_SpaceCenter_2F_EventScript_LastGruntDone:
	msgbox MossdeepCity_SpaceCenter_2F_Text_Grunt7PostBattle, MSGBOX_DEFAULT
	releaseall
	end
MossdeepCity_SpaceCenter_2F_EventScript_FirstGuardWaiting:
	msgbox MossdeepCity_SpaceCenter_2F_Text_FirstGuardWaiting, MSGBOX_DEFAULT
	releaseall
	end

MossdeepCity_SpaceCenter_2F_EventScript_Grunt5::
	msgbox MossdeepCity_SpaceCenter_2F_Text_Grunt5PostBattle, MSGBOX_NPC
	end

MossdeepCity_SpaceCenter_2F_EventScript_Tabitha::
	lock
	faceplayer
	goto_if_not_defeated TRAINER_GRUNT_SPACE_CENTER_7, MossdeepCity_SpaceCenter_2F_EventScript_FirstGuardWaiting
	releaseall
	trainerbattle_double TRAINER_TABITHA_MOSSDEEP, MossdeepCity_SpaceCenter_2F_Text_TabithaIntro, MossdeepCity_SpaceCenter_Text_TabithaDefeat, MossdeepCity_SpaceCenter_2F_Text_TabithaNotEnoughMons
	msgbox MossdeepCity_SpaceCenter_2F_Text_TabithaAfter, MSGBOX_AUTOCLOSE
	end

MossdeepCity_SpaceCenter_2F_EventScript_Courtney::
	lock
	faceplayer
	msgbox MossdeepCity_SpaceCenter_2F_Text_CourtneyFormation, MSGBOX_DEFAULT
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

MossdeepCity_SpaceCenter_2F_EventScript_Maxie::
	lockall
	msgbox MossdeepCity_SpaceCenter_2F_Text_MaxieDontInterfere, MSGBOX_DEFAULT
	releaseall
	end

MossdeepCity_SpaceCenter_2F_EventScript_Steven::
	call EC_RequireHideout
	call EC_RequireMind
	lockall
	goto_if_not_defeated TRAINER_TABITHA_MOSSDEEP, MossdeepCity_SpaceCenter_2F_EventScript_TabithaStillGuarding
	goto_if_set FLAG_INTERACTED_WITH_STEVEN_SPACE_CENTER, MossdeepCity_SpaceCenter_2F_EventScript_ReadyForBattlePrompt
	setflag FLAG_INTERACTED_WITH_STEVEN_SPACE_CENTER
	msgbox MossdeepCity_SpaceCenter_2F_Text_StevenWhyStealRocketFuel, MSGBOX_DEFAULT
	msgbox MossdeepCity_SpaceCenter_2F_Text_MaxieUseFuelToEruptVolcano, MSGBOX_DEFAULT
	closemessage
	delay 30
	playse SE_NOT_EFFECTIVE
	switch VAR_FACING
	case DIR_SOUTH, MossdeepCity_SpaceCenter_2F_EventScript_StevenFightMovementSouth
	applymovement LOCALID_SPACE_CENTER_2F_STEVEN, MossdeepCity_SpaceCenter_2F_Movement_StevenFight
	waitmovement 0
	releaseall
	end

MossdeepCity_SpaceCenter_2F_EventScript_TabithaStillGuarding:
	msgbox MossdeepCity_SpaceCenter_2F_Text_TabithaStillGuarding, MSGBOX_DEFAULT
	releaseall
	end

MossdeepCity_SpaceCenter_2F_Text_TabithaStillGuarding:
	.string "STEVEN: TABITHA still guards the\n"
	.string "passage on the right. Defeat him,\l"
	.string "then come back to me.$"

MossdeepCity_SpaceCenter_2F_EventScript_StevenFightMovementSouth::
	applymovement LOCALID_SPACE_CENTER_2F_STEVEN, MossdeepCity_SpaceCenter_2F_Movement_StevenFightSouth
	waitmovement 0
	releaseall
	end

MossdeepCity_SpaceCenter_2F_Movement_StevenFight:
	lock_facing_direction
	walk_fast_up
	walk_fast_up
	unlock_facing_direction
	delay_16
	delay_16
	walk_slow_down
	walk_slow_down
	step_end

MossdeepCity_SpaceCenter_2F_Movement_StevenFightSouth:
	face_left
	lock_facing_direction
	walk_fast_right
	walk_fast_right
	unlock_facing_direction
	delay_16
	delay_16
	walk_slow_left
	walk_slow_left
	face_down
	step_end

MossdeepCity_SpaceCenter_2F_EventScript_ReadyForBattlePrompt::
	applymovement VAR_LAST_TALKED, Common_Movement_FacePlayer
	waitmovement 0
	msgbox MossdeepCity_SpaceCenter_2F_Text_StevenAreYouReadyToBattle, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, MossdeepCity_SpaceCenter_2F_EventScript_ChoosePartyForMultiBattle
	msgbox MossdeepCity_SpaceCenter_2F_Text_StevenHurryGetReadyQuickly, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	releaseall
	end

MossdeepCity_SpaceCenter_2F_EventScript_ChoosePartyForMultiBattle::
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	closemessage
	fadescreen FADE_TO_BLACK
	playfanfare MUS_HEAL
	special HealPlayerParty
	copyvar VAR_POKE_VIAL_CHARGES, VAR_POKE_VIAL_MAX_CHARGES
	waitfanfare
	special SavePlayerParty
	special ChooseHalfPartyForBattle
	goto_if_ne VAR_RESULT, 0, MossdeepCity_SpaceCenter_2F_EventScript_DoStevenMultiBattle
	special LoadPlayerParty
	goto MossdeepCity_SpaceCenter_2F_EventScript_ReadyForBattlePrompt

MossdeepCity_SpaceCenter_2F_EventScript_DoStevenMultiBattle::
	multi_2_vs_2 TRAINER_MAXIE_MOSSDEEP, MossdeepCity_SpaceCenter_2F_Text_JustWantToExpandLand, TRAINER_COURTNEY_MOSSDEEP, MossdeepCity_SpaceCenter_Text_CourtneyDefeat, PARTNER_STEVEN
	switch VAR_RESULT
	case 1, MossdeepCity_SpaceCenter_2F_EventScript_DefeatedMaxieTabitha
	fadescreen FADE_TO_BLACK
	special SetCB2WhiteOut

MossdeepCity_SpaceCenter_2F_EventScript_DefeatedMaxieTabitha::
	msgbox MossdeepCity_SpaceCenter_2F_Text_MaxieWeFailedIsAquaAlsoMisguided, MSGBOX_DEFAULT
	closemessage
	delay 20
	applymovement LOCALID_SPACE_CENTER_MAXIE, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_EC_COURTNEY_MOSSDEEP, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 60
	applymovement LOCALID_SPACE_CENTER_MAXIE, Common_Movement_WalkInPlaceFasterUp
	applymovement LOCALID_EC_COURTNEY_MOSSDEEP, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	delay 20
	msgbox MossdeepCity_SpaceCenter_2F_Text_MaxieWeWillGiveUp, MSGBOX_DEFAULT
	closemessage
	setvar VAR_MOSSDEEP_CITY_STATE, 3
	fadedefaultbgm
	fadescreen FADE_TO_BLACK
	setvar VAR_MOSSDEEP_SPACE_CENTER_STATE, 3
	setflag FLAG_HIDE_MOSSDEEP_CITY_SPACE_CENTER_1F_TEAM_MAGMA
	setflag FLAG_HIDE_MOSSDEEP_CITY_SPACE_CENTER_2F_TEAM_MAGMA
	setflag FLAG_HIDE_MOSSDEEP_CITY_TEAM_MAGMA
	removeobject LOCALID_SPACE_CENTER_GRUNT_6
	removeobject LOCALID_SPACE_CENTER_GRUNT_7
	removeobject LOCALID_SPACE_CENTER_GRUNT_5
	removeobject LOCALID_EC_COURTNEY_MOSSDEEP
	removeobject LOCALID_SPACE_CENTER_TABITHA
	removeobject LOCALID_SPACE_CENTER_MAXIE
	setobjectxy LOCALID_SPACE_CENTER_2F_SCIENTIST, 5, 6
	turnobject LOCALID_SPACE_CENTER_2F_SCIENTIST, DIR_SOUTH
	setobjectxy LOCALID_SPACE_CENTER_2F_GENTLEMAN, 11, 8
	turnobject LOCALID_SPACE_CENTER_2F_GENTLEMAN, DIR_NORTH
	setobjectxy LOCALID_SPACE_CENTER_2F_RICH_BOY, 6, 2
	turnobject LOCALID_SPACE_CENTER_2F_RICH_BOY, DIR_NORTH
	call MossdeepCity_SpaceCenter_2F_EventScript_StevenFacePlayer
	fadescreen FADE_FROM_BLACK
	msgbox MossdeepCity_SpaceCenter_2F_Text_StevenThankYouComeSeeMeAtHome, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	playfanfare MUS_HEAL
	special HealPlayerParty
	copyvar VAR_POKE_VIAL_CHARGES, VAR_POKE_VIAL_MAX_CHARGES
	waitfanfare
	completemilestone FLAG_EC_REPORT_C42_COMPLETE
	buffermilestonetext FLAG_EC_REPORT_C42_COMPLETE
	call_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_PrintMilestoneCapLine
	call_if_eq VAR_RESULT, 3, EmeraldChampions_EventScript_PrintMilestoneCapLine
	setflag FLAG_EC_EARNED_AURORA_TICKET
	setflag FLAG_ENABLE_SHIP_BIRTH_ISLAND
	setflag FLAG_RECEIVED_HM_DIVE
	clearflag FLAG_HIDE_MOSSDEEP_CITY_STEVENS_HOUSE_BELDUM_POKEBALL
	setflag FLAG_DEFEATED_MAGMA_SPACE_CENTER
	setflag FLAG_HIDE_MOSSDEEP_CITY_STEVENS_HOUSE_STEVEN
	removeobject LOCALID_SPACE_CENTER_2F_STEVEN
	setflag FLAG_HIDE_MOSSDEEP_CITY_SPACE_CENTER_MAGMA_NOTE
	clearflag FLAG_HIDE_MOSSDEEP_CITY_STEVENS_HOUSE_STEVEN
	setvar VAR_STEVENS_HOUSE_STATE, 2
	setflag FLAG_HIDE_SEAFLOOR_CAVERN_ENTRANCE_AQUA_GRUNT
	setflag FLAG_HIDE_MOSSDEEP_CITY_SCOTT
	removeobject LOCALID_SPACE_CENTER_2F_SCIENTIST
	setobjectxyperm LOCALID_SPACE_CENTER_2F_SCIENTIST, 5, 6
	setobjectmovementtype LOCALID_SPACE_CENTER_2F_SCIENTIST, MOVEMENT_TYPE_WANDER_AROUND
	addobject LOCALID_SPACE_CENTER_2F_SCIENTIST
	fadescreen FADE_FROM_BLACK
#ifdef BUGFIX
	releaseall
#endif
	end

MossdeepCity_SpaceCenter_2F_EventScript_StevenFacePlayer::
	switch VAR_FACING
	case DIR_SOUTH, MossdeepCity_SpaceCenter_2F_EventScript_StevenFacePlayerSouth
	case DIR_WEST, MossdeepCity_SpaceCenter_2F_EventScript_StevenFacePlayerWest
	return

MossdeepCity_SpaceCenter_2F_EventScript_StevenFacePlayerSouth::
	turnobject LOCALID_SPACE_CENTER_2F_STEVEN, DIR_NORTH
	return

MossdeepCity_SpaceCenter_2F_EventScript_StevenFacePlayerWest::
	turnobject LOCALID_SPACE_CENTER_2F_STEVEN, DIR_EAST
	return

MossdeepCity_SpaceCenter_2F_EventScript_RivalRayquazaCall::
	lockall
	checkplayergender
	call_if_eq VAR_RESULT, MALE, MossdeepCity_SpaceCenter_2F_EventScript_MayRayquazaCall
	call_if_eq VAR_RESULT, FEMALE, MossdeepCity_SpaceCenter_2F_EventScript_BrendanRayquazaCall
	closemessage
	clearflag FLAG_DEFEATED_MAGMA_SPACE_CENTER
	releaseall
	end

MossdeepCity_SpaceCenter_2F_EventScript_MayRayquazaCall::
	pokenavcall MossdeepCity_SpaceCenter_2F_Text_MayRayquazaCall
	waitmessage
	return

MossdeepCity_SpaceCenter_2F_EventScript_BrendanRayquazaCall::
	pokenavcall MossdeepCity_SpaceCenter_2F_Text_BrendanRayquazaCall
	waitmessage
	return

MossdeepCity_SpaceCenter_2F_Text_MossdeepIdealForRockets:
	.string "MOSSDEEP has mainly sunny weather,\n"
	.string "and its winds are stable.\p"
	.string "It's an ideal location for launching\n"
	.string "rockets.$"

MossdeepCity_SpaceCenter_2F_Text_WhyWouldMagmaStealRocketFuel:
	.string "Why would TEAM MAGMA want to steal\n"
	.string "our rocket fuel in the first place?$"

MossdeepCity_SpaceCenter_2F_Text_WouldveLikedToBeAstronaut:
	.string "If only I was a little younger, I would\n"
	.string "have liked being an astronaut…\p"
	.string "No… It's not too late!\n"
	.string "I can and will try!$"

MossdeepCity_SpaceCenter_2F_Text_MagmaCantGetAwayWithThis:
	.string "If TEAM MAGMA takes the rocket fuel,\n"
	.string "I won't be able to go to space!\p"
	.string "They can't be allowed to get away\n"
	.string "with such an outrage!$"

MossdeepCity_SpaceCenter_2F_Text_WishOrdinaryPeopleCouldGoIntoSpace:
	.string "I wish ordinary people could go into\n"
	.string "space one day…$"

MossdeepCity_SpaceCenter_2F_Text_DoesMagmaWantToGoToSpace:
	.string "TEAM MAGMA…\n"
	.string "Do they want to go to space, too?$"

MossdeepCity_SpaceCenter_2F_Text_YoureOutnumberedTakeUsOn:
	.string "Two of us will take you on.\n"
	.string "First, get past the front guard!\p"
	.string "Still want to come through?$"

MossdeepCity_SpaceCenter_2F_Text_GoodAnswer:
	.string "That's right. Go downstairs while\n"
	.string "you can!$"

MossdeepCity_SpaceCenter_2F_Text_Grunt5Intro:
	.string "I'm first! You're not getting\n"
	.string "past this corridor!$"

MossdeepCity_SpaceCenter_2F_Text_Grunt5Defeat:
	.string "What?! It's your turn. Stop that\n"
	.string "kid!$"

MossdeepCity_SpaceCenter_2F_Text_Grunt7Intro:
	.string "I'm the last one! MAXIE's\n"
	.string "counting on me!$"

MossdeepCity_SpaceCenter_2F_Text_Grunt7Defeat:
	.string "Both of us… defeated!$"

MossdeepCity_SpaceCenter_2F_Text_PauseBeforeLastGrunt:
	.string "The last guard is on the right\n"
	.string "of this corridor. Talk to him\l"
	.string "when your team is ready.\p"
	.string "You can use your VIAL or return\n"
	.string "to MOSSDEEP's POKéMON CENTER.\l"
	.string "The first fight stays completed.$"

MossdeepCity_SpaceCenter_2F_Text_ReadyForLastGrunt:
	.string "Ready for the last guard?\n"
	.string "You can prepare your team first.$"

MossdeepCity_SpaceCenter_2F_Text_FirstGuardWaiting:
	.string "Finish with the guards by the\n"
	.string "stairs first. The final guard\l"
	.string "stands on the right.$"

MossdeepCity_SpaceCenter_2F_Text_Grunt6PostBattle:
	.string "Three of us guarding one\n"
	.string "corridor. I thought this would be\l"
	.string "an easy job.$"

MossdeepCity_SpaceCenter_2F_Text_Grunt7PostBattle:
	.string "I can't believe you beat us all.\n"
	.string "What do I tell MAXIE?$"

MossdeepCity_SpaceCenter_2F_Text_Grunt5PostBattle:
	.string "We were supposed to keep everyone\n"
	.string "downstairs until MAXIE had the\l"
	.string "fuel.$"

MossdeepCity_SpaceCenter_2F_Text_WellTakeCareOfYou:
	.string "The others couldn't stop you?\n"
	.string "Then we will!\p"
	.string "MAXIE isn't leaving without that\n"
	.string "fuel.$"

MossdeepCity_SpaceCenter_2F_Text_MaxieDontInterfere:
	.string "MAXIE: You again? STEVEN is\n"
	.string "already wasting my time. Stand\l"
	.string "aside!$"

MossdeepCity_SpaceCenter_2F_Text_StevenWhyStealRocketFuel:
	.string "STEVEN: Rocket fuel cannot control\n"
	.string "GROUDON. What are you truly targeting?$"

MossdeepCity_SpaceCenter_2F_Text_MaxieUseFuelToEruptVolcano:
	.string "MAXIE: The rocket fuel will blast\n"
	.string "open MT. CHIMNEY. New land will\l"
	.string "rise across HOENN.\p"
	.string "GROUDON would not obey. I will\n"
	.string "finish what it began myself.$"

MossdeepCity_SpaceCenter_2F_Text_StevenAreYouReadyToBattle:
	.string "STEVEN: Ready to stop them?\p"
	.string "I'll heal your whole party and\n"
	.string "refill your VIAL. Then choose\l"
	.string "three to battle alongside me.$"

MossdeepCity_SpaceCenter_2F_Text_StevenHurryGetReadyQuickly:
	.string "STEVEN: Take time to prepare.\n"
	.string "You can visit the CENTER or PC,\l"
	.string "then return to me here.$"

MossdeepCity_SpaceCenter_2F_Text_JustWantToExpandLand:
	.string "All I want…\n"
	.string "I just want to expand the land mass…$"

MossdeepCity_SpaceCenter_Text_TabithaDefeat:
	.string "I couldn't keep you away…$"

MossdeepCity_SpaceCenter_2F_Text_TabithaIntro:
	.string "TABITHA: MAXIE needs that rocket\n"
	.string "fuel. My orders are to keep\l"
	.string "everyone else away!$"

MossdeepCity_SpaceCenter_2F_Text_TabithaAfter:
	.string "TABITHA: You beat me, but MAXIE\n"
	.string "won't leave empty-handed. STEVEN\l"
	.string "is still trying to stop him.$"

MossdeepCity_SpaceCenter_2F_Text_TabithaNotEnoughMons:
	.string "A formation requires two usable\n"
	.string "POKéMON. Return when both can battle.$"

MossdeepCity_SpaceCenter_Text_CourtneyDefeat:
	.string "COURTNEY: Defeated… Both of us.$"

MossdeepCity_SpaceCenter_2F_Text_CourtneyFormation:
	.string "COURTNEY: STEVEN… and you.\p"
	.string "MAXIE, I'll cover you. We still\n"
	.string "need that fuel.$"

MossdeepCity_SpaceCenter_2F_Text_MaxieWeFailedIsAquaAlsoMisguided:
	.string "MAXIE: We couldn't control\n"
	.string "GROUDON. So I came here to take\l"
	.string "fuel and force an eruption\l"
	.string "anyway.\p"
	.string "I nearly destroyed the very land\n"
	.string "I claimed to protect.\p"
	.string "If I was this wrong… ARCHIE may\n"
	.string "be making the same mistake at\l"
	.string "sea.$"

MossdeepCity_SpaceCenter_2F_Text_MaxieWeWillGiveUp:
	.string "MAXIE: Keep the fuel. We're\n"
	.string "leaving.\p"
	.string "I must find GROUDON, and stop\n"
	.string "ARCHIE before he awakens KYOGRE.$"

MossdeepCity_SpaceCenter_2F_Text_StevenThankYouComeSeeMeAtHome:
	.string "STEVEN: We kept the staff safe.\n"
	.string "But MAGMA wanted more than fuel.\p"
	.string "AQUA's submarine went to ground on\n"
	.string "the seabed south of MOSSDEEP. SAIL\l"
	.string "south through ROUTE 127 to ROUTE\l"
	.string "128, then DIVE at the deep basin\l"
	.string "to find SEAFLOOR CAVERN.\p"
	.string "Bring licensed SURF and DIVE, and\n"
	.string "partners ready for STRENGTH and\l"
	.string "ROCK SMASH inside the cavern.\p"
	.string "They were also studying our\n"
	.string "meteorite readings. The readings\l"
	.string "point to BIRTH ISLAND, if you ever\l"
	.string "want to look into it. LILYCOVE\l"
	.string "HARBOR can take you there whenever\l"
	.string "you choose.\p"
	.string "Speak to the scientist here\n"
	.string "before sailing.$"

MossdeepCity_SpaceCenter_2F_EventScript_BirthIslandBriefing::
	msgbox MossdeepCity_SpaceCenter_2F_Text_BirthIslandBriefing, MSGBOX_DEFAULT
	release
	end

MossdeepCity_SpaceCenter_2F_Text_BirthIslandBriefing:
	.string "AQUA's submarine went to ground on\n"
	.string "the seabed south of here. SAIL\l"
	.string "south through ROUTE 127 to ROUTE\l"
	.string "128, then DIVE at the deep basin\l"
	.string "to find SEAFLOOR CAVERN.\p"
	.string "MAGMA also studied our meteorite\n"
	.string "readings. The disturbance at\l"
	.string "BIRTH ISLAND is still active, if\l"
	.string "you ever want to look into it.\p"
	.string "Your clearance is registered at\n"
	.string "LILYCOVE HARBOR. Sail from there\l"
	.string "to investigate the triangle.\p"
	.string "Take the shortest route between\n"
	.string "its successive positions. Extra\l"
	.string "steps reset the pattern.\p"
	.string "The harbor sailor can bring you\n"
	.string "back. A reset costs no ticket\l"
	.string "or reward.$"

MossdeepCity_SpaceCenter_2F_Text_MayRayquazaCall:
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "MAY: Hi, {PLAYER}{KUN}!\p"
	.string "I was just in PACIFIDLOG a little\n"
	.string "while ago.\p"
	.string "I saw a giant green POKéMON\n"
	.string "flying high in the sky.\p"
	.string "I…\n"
	.string "I've never seen anything like it.\p"
	.string "I wonder what it was.\n"
	.string "Is this maybe a major discovery?\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

MossdeepCity_SpaceCenter_2F_Text_BrendanRayquazaCall:
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "BRENDAN: Hey, {PLAYER}!\n"
	.string "I was in PACIFIDLOG just now.\p"
	.string "I saw this huge green POKéMON\n"
	.string "flying across the sky.\p"
	.string "I've never seen anything that\n"
	.string "amazing before.\p"
	.string "I wish you could've seen it,\n"
	.string "{PLAYER}.\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"
