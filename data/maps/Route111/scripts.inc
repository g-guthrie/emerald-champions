Route111_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Route111_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, Route111_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Route111_OnWarp
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route111_OnFrame
	.byte 0

Route111_OnLoad:
	call_if_unset FLAG_REGI_DOORS_OPENED, Route111_EventScript_CloseDesertRuins
	call_if_eq VAR_MIRAGE_TOWER_STATE, 1, Route111_EventScript_ShowTemporaryMirageTower
	end

Route111_EventScript_CloseDesertRuins::
	setmetatile 29, 86, METATILE_General_RockWall_RockBase, TRUE
	setmetatile 29, 87, METATILE_General_RockWall_SandBase, TRUE
	return

@ Show Mirage Tower just prior to disintegration. Mirage Tower is otherwise handled by the map layout
Route111_EventScript_ShowTemporaryMirageTower::
	setmetatile 18, 53, METATILE_Mauville_MirageTower_Tile0, FALSE
	setmetatile 19, 53, METATILE_Mauville_MirageTower_Tile1, FALSE
	setmetatile 20, 53, METATILE_Mauville_MirageTower_Tile2, FALSE
	setmetatile 18, 54, METATILE_Mauville_MirageTower_Tile3, FALSE
	setmetatile 19, 54, METATILE_Mauville_MirageTower_Tile4, FALSE
	setmetatile 20, 54, METATILE_Mauville_MirageTower_Tile5, FALSE
	setmetatile 18, 55, METATILE_Mauville_MirageTower_Tile6, FALSE
	setmetatile 19, 55, METATILE_Mauville_MirageTower_Tile7, FALSE
	setmetatile 20, 55, METATILE_Mauville_MirageTower_Tile8, FALSE
	setmetatile 18, 56, METATILE_Mauville_MirageTower_Tile9, FALSE
	setmetatile 19, 56, METATILE_Mauville_MirageTower_TileA, FALSE
	setmetatile 20, 56, METATILE_Mauville_MirageTower_TileB, FALSE
	setmetatile 18, 57, METATILE_Mauville_MirageTower_TileC, FALSE
	setmetatile 19, 57, METATILE_Mauville_MirageTower_TileD, FALSE
	setmetatile 20, 57, METATILE_Mauville_MirageTower_TileE, FALSE
	setmetatile 18, 58, METATILE_Mauville_MirageTower_TileF, FALSE
	setmetatile 19, 58, METATILE_Mauville_MirageTower_Tile10, FALSE
	setmetatile 20, 58, METATILE_Mauville_MirageTower_Tile11, FALSE
	return

Route111_OnTransition:
	setvar VAR_TRAINER_HILL_IS_ACTIVE, 0
	special SetMirageTowerVisibility
	call_if_unset FLAG_MIRAGE_TOWER_VISIBLE, Route111_EventScript_SetLayoutNoMirageTower
	call_if_eq VAR_MIRAGE_TOWER_STATE, 1, Route111_EventScript_SetFallingPlayerGfx
	call_if_eq VAR_MIRAGE_TOWER_STATE, 2, Route111_EventScript_SetMirageTowerGone
	call Route111_EventScript_CheckSetSandstorm
	call Route111_EventScript_CheckDynamoGuard
	call GabbyAndTy_EventScript_UpdateLocation
	call Route111_EventScript_UpdateVialQuestObjects
	goto_if_not_defeated TRAINER_VICKY, Route111_EventScript_SetWinstratesNotDefeated
	end

Route111_EventScript_UpdateVialQuestObjects::
	setflag FLAG_HIDE_ROUTE111_VIAL_NURSE
	setflag FLAG_HIDE_ROUTE111_VIAL_CHANSEY
	switch VAR_CHANSEY_NURSE_STATE
	case 0, Route111_EventScript_ShowVialNurseAndChansey
	case 1, Route111_EventScript_ShowVialNurse
	case 2, Route111_EventScript_ShowVialNurse
	case 3, Route111_EventScript_ShowVialNurse
	case 4, Route111_EventScript_ShowVialNurse
	case 5, Route111_EventScript_ShowVialNurse
	case 6, Route111_EventScript_ShowVialNurse
	return

Route111_EventScript_ShowVialNurseAndChansey::
	clearflag FLAG_HIDE_ROUTE111_VIAL_CHANSEY
Route111_EventScript_ShowVialNurse::
	clearflag FLAG_HIDE_ROUTE111_VIAL_NURSE
	return

Route111_EventScript_SetFallingPlayerGfx::
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, Route111_EventScript_SetFallingPlayerGfxMale
	goto_if_eq VAR_RESULT, FEMALE, Route111_EventScript_SetFallingPlayerGfxFemale
	return

Route111_EventScript_SetFallingPlayerGfxMale::
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	return

Route111_EventScript_SetFallingPlayerGfxFemale::
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
	return

Route111_EventScript_CheckSetSandstorm::
	getplayerxy VAR_TEMP_0, VAR_TEMP_1
	goto_if_lt VAR_TEMP_1, 34, Route111_EventScript_EndCheckSetSandstorm
	goto_if_gt VAR_TEMP_1, 107, Route111_EventScript_EndCheckSetSandstorm
	goto_if_gt VAR_TEMP_1, 72, Route111_EventScript_SetSandstorm
	goto_if_gt VAR_TEMP_0, 2000, Route111_EventScript_EndCheckSetSandstorm
	goto_if_lt VAR_TEMP_0, 8, Route111_EventScript_EndCheckSetSandstorm
Route111_EventScript_SetSandstorm::
	setweather WEATHER_SANDSTORM
Route111_EventScript_EndCheckSetSandstorm::
	return

Route111_EventScript_SetWinstratesNotDefeated::
	clearflag FLAG_HIDE_ROUTE_111_VICTOR_WINSTRATE
	setflag FLAG_HIDE_ROUTE_111_VICTORIA_WINSTRATE
	setflag FLAG_HIDE_ROUTE_111_VIVI_WINSTRATE
	setflag FLAG_HIDE_ROUTE_111_VICKY_WINSTRATE
	cleartrainerflag TRAINER_VICTOR
	cleartrainerflag TRAINER_VICTORIA
	cleartrainerflag TRAINER_VIVI
	end

Route111_EventScript_SetLayoutNoMirageTower::
	setmaplayoutindex LAYOUT_ROUTE111_NO_MIRAGE_TOWER
	return

Route111_EventScript_SetMirageTowerGone::
	setvar VAR_MIRAGE_TOWER_STATE, 3
	return

Route111_OnWarp:
	map_script_2 VAR_MIRAGE_TOWER_STATE, 1, Route111_EventScript_HidePlayerForMirageTower
	.2byte 0

Route111_EventScript_HidePlayerForMirageTower::
	hideobjectat LOCALID_PLAYER, MAP_LITTLEROOT_TOWN
	end

Route111_OnFrame:
	map_script_2 VAR_MIRAGE_TOWER_STATE, 1, Route111_EventScript_MirageTowerDisappear
	.2byte 0

Route111_EventScript_MirageTowerDisappear::
	lockall
	special StartMirageTowerShake
	delay 24
	playse SE_FALL
	addobject LOCALID_ROUTE111_PLAYER_FALLING
	special StartPlayerDescendMirageTower
	showobjectat LOCALID_PLAYER, MAP_LITTLEROOT_TOWN
	removeobject LOCALID_ROUTE111_PLAYER_FALLING
	delay 16
	turnobject LOCALID_PLAYER, DIR_NORTH
	delay 16
	special StartMirageTowerDisintegration
	special StartMirageTowerFossilFallAndSink
	setvar VAR_MIRAGE_TOWER_STATE, 2
	clearflag FLAG_HIDE_DESERT_UNDERPASS_FOSSIL
	goto_if_set FLAG_CHOSE_CLAW_FOSSIL, Route111_EventScript_RootFossilDisappeared
	msgbox Route111_Text_ClawFossilDisappeared, MSGBOX_DEFAULT
	releaseall
	end

Route111_EventScript_RootFossilDisappeared::
	msgbox Route111_Text_RootFossilDisappeared, MSGBOX_DEFAULT
	releaseall
	end

@ Unused
Route111_Movement_PlayerFall::
	lock_anim
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

Route111_EventScript_Girl::
	lock
	faceplayer
	dotimebasedevents
	goto_if_set FLAG_DAILY_ROUTE_111_RECEIVED_BERRY, Route111_EventScript_ReceivedBerry
	msgbox Route111_Text_WateredPlantsEveryDayTakeBerry, MSGBOX_DEFAULT
	giveitem ITEM_RAZZ_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_DAILY_ROUTE_111_RECEIVED_BERRY
	special GetPlayerBigGuyGirlString
	msgbox Route111_Text_GoingToTryToMakeDifferentColorBerries, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_ReceivedBerry::
	msgbox Route111_Text_WhatColorBerriesToLookForToday, MSGBOX_DEFAULT
	release
	end

@ Unused
Route111_EventScript_ViciousSandstormTriggerUp::
	lockall
	setvar VAR_0x8004, 0
	goto Route111_EventScript_ViciousSandstormTrigger
	end

Route111_EventScript_ViciousSandstormTriggerDown::
	lockall
	setvar VAR_0x8004, 1
	goto Route111_EventScript_ViciousSandstormTrigger
	end

Route111_EventScript_ViciousSandstormTriggerLeft::
	lockall
	setvar VAR_0x8004, 2
	goto Route111_EventScript_ViciousSandstormTrigger
	end

@ Unused
Route111_EventScript_ViciousSandstormTriggerRight::
	lockall
	setvar VAR_0x8004, 3
	goto Route111_EventScript_ViciousSandstormTrigger
	end

Route111_EventScript_ViciousSandstormTrigger::
	checkitem ITEM_GO_GOGGLES
	goto_if_eq VAR_RESULT, FALSE, Route111_EventScript_PreventRouteAccess
	setvar VAR_TEMP_3, 1
	releaseall
	end

Route111_EventScript_PreventRouteAccess::
	msgbox gText_SandstormIsVicious, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_0x8004, 0, Route111_EventScript_PushUpFromRoute
	call_if_eq VAR_0x8004, 1, Route111_EventScript_PushDownFromRoute
	call_if_eq VAR_0x8004, 2, Route111_EventScript_PushLeftFromRoute
	call_if_eq VAR_0x8004, 3, Route111_EventScript_PushRightFromRoute
	releaseall
	end

Route111_EventScript_PushUpFromRoute::
	applymovement LOCALID_PLAYER, Route111_Movement_PushUpFromRoute
	waitmovement 0
	return

Route111_EventScript_PushDownFromRoute::
	applymovement LOCALID_PLAYER, Route111_Movement_PushDownFromRoute
	waitmovement 0
	return

Route111_EventScript_PushLeftFromRoute::
	applymovement LOCALID_PLAYER, Route111_Movement_PushLeftFromRoute
	waitmovement 0
	return

Route111_EventScript_PushRightFromRoute::
	applymovement LOCALID_PLAYER, Route111_Movement_PushRightFromRoute
	waitmovement 0
	return

Route111_Movement_PushUpFromRoute:
	walk_up
	step_end

Route111_Movement_PushDownFromRoute:
	walk_down
	step_end

Route111_Movement_PushLeftFromRoute:
	walk_left
	step_end

Route111_Movement_PushRightFromRoute:
	walk_right
	step_end

Route111_EventScript_SunTrigger::
	setweather WEATHER_SUNNY
	fadenewbgm MUS_ROUTE110
	doweather
	setvar VAR_TEMP_3, 0
	end

Route111_EventScript_SandstormTrigger::
	setweather WEATHER_SANDSTORM
	fadenewbgm MUS_DESERT
	doweather
	end

Route111_EventScript_Victor::
	lock
	faceplayer
	setflag FLAG_LANDMARK_WINSTRATE_FAMILY
	msgbox Route111_Text_BattleOurFamily, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route111_EventScript_BattleWinstrates
	msgbox Route111_Text_IsThatSo, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_BattleWinstrates::
	msgbox Route111_Text_VictorIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_VICTOR, Route111_Text_VictorDefeat
	applymovement LOCALID_ROUTE111_VICTOR, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	msgbox Route111_Text_VictorPostBattle, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE111_VICTOR, Route111_Movement_WinstrateEnterHouse
	waitmovement 0
	removeobject LOCALID_ROUTE111_VICTOR
	call Route111_EventScript_CloseWinstrateDoor
	applymovement LOCALID_PLAYER, Route111_Movement_WaitForNextWinstrate
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	addobject LOCALID_ROUTE111_VICTORIA
	applymovement LOCALID_ROUTE111_VICTORIA, Route111_Movement_WinstrateExitHouse
	waitmovement 0
	call Route111_EventScript_CloseWinstrateDoor
	msgbox Route111_Text_VictoriaIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_VICTORIA, Route111_Text_VictoriaDefeat
	applymovement LOCALID_ROUTE111_VICTORIA, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	msgbox Route111_Text_VictoriaPostBattle, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE111_VICTORIA, Route111_Movement_WinstrateEnterHouse
	waitmovement 0
	removeobject LOCALID_ROUTE111_VICTORIA
	call Route111_EventScript_CloseWinstrateDoor
	applymovement LOCALID_PLAYER, Route111_Movement_WaitForNextWinstrate
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	addobject LOCALID_ROUTE111_VIVI
	applymovement LOCALID_ROUTE111_VIVI, Route111_Movement_WinstrateExitHouse
	waitmovement 0
	call Route111_EventScript_CloseWinstrateDoor
	msgbox Route111_Text_ViviIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_VIVI, Route111_Text_ViviDefeat
	applymovement LOCALID_ROUTE111_VIVI, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	msgbox Route111_Text_ViviPostBattle, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE111_VIVI, Route111_Movement_WinstrateEnterHouse
	waitmovement 0
	removeobject LOCALID_ROUTE111_VIVI
	call Route111_EventScript_CloseWinstrateDoor
	applymovement LOCALID_PLAYER, Route111_Movement_WaitForNextWinstrate
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	addobject LOCALID_ROUTE111_VICKY
	applymovement LOCALID_ROUTE111_VICKY, Route111_Movement_WinstrateExitHouse
	waitmovement 0
	call Route111_EventScript_CloseWinstrateDoor
	msgbox Route111_Text_VickyIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_VICKY, Route111_Text_VickyDefeat
	msgbox Route111_Text_VickyPostBattle, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE111_VICKY, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	call Route111_EventScript_OpenWinstrateDoor
	applymovement LOCALID_ROUTE111_VICKY, Route111_Movement_WinstrateEnterHouse
	waitmovement 0
	removeobject LOCALID_ROUTE111_VICKY
	call Route111_EventScript_CloseWinstrateDoor
	release
	end

Route111_EventScript_OpenWinstrateDoor::
	opendoor 13, 113
	waitdooranim
	return

Route111_EventScript_CloseWinstrateDoor::
	closedoor 13, 113
	waitdooranim
	return

Route111_Movement_WinstrateEnterHouse:
	walk_in_place_faster_up
	walk_up
	step_end

Route111_Movement_WinstrateExitHouse:
	walk_down
	step_end

Route111_Movement_WaitForNextWinstrate:
	delay_16
	delay_16
	delay_16
	step_end

Route111_EventScript_RouteSignMauville::
	msgbox Route111_Text_RouteSignMauville, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route111_EventScript_WinstrateHouseSign::
	msgbox Route111_Text_WinstrateHouseSign, MSGBOX_SIGN
	end

Route111_EventScript_RouteSign112::
	msgbox Route111_Text_RouteSign112, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route111_EventScript_RouteSign113::
	msgbox Route111_Text_RouteSign113, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route111_EventScript_OldLadysRestStopSign::
	msgbox Route111_Text_OldLadysRestStopSign, MSGBOX_SIGN
	end

Route111_EventScript_TrainerTipsSpAtkSpDef::
	msgbox Route111_Text_TrainerTipsSpAtkSpDef, MSGBOX_SIGN
	end

Route111_EventScript_Man1::
	msgbox Route111_Text_ToughToKeepWinningUpTheRanks, MSGBOX_NPC
	end

Route111_EventScript_Man2::
	msgbox Route111_Text_WinstrateFamilyDestroyedMe, MSGBOX_NPC
	end

Route111_EventScript_Hiker::
	lock
	faceplayer
	goto_if_eq VAR_MIRAGE_TOWER_STATE, 3, Route111_EventScript_HikerMirageTowerGone
	goto_if_eq VAR_MIRAGE_TOWER_STATE, 2, Route111_EventScript_HikerMirageTowerDisintegrated
	goto_if_set FLAG_MIRAGE_TOWER_VISIBLE, Route111_EventScript_HikerMirageTowerVisible
	msgbox Route111_Text_ShouldBeMirageTowerAroundHere, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_HikerMirageTowerGone::
	msgbox Route111_Text_MirageTowerHasntBeenSeenSince, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_HikerMirageTowerDisintegrated::
	msgbox Route111_Text_ThatWasShockingSandRainedDown, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_HikerMirageTowerVisible::
	msgbox Route111_Text_MirageTowerClearlyVisible, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_VialChanseyEscape::
	lockall
	specialvar VAR_RESULT, GetPlayerFacingDirection
	call_if_ne VAR_RESULT, DIR_NORTH, Route111_EventScript_VialPlayerFaceUp
	applymovement LOCALID_ROUTE111_VIAL_NURSE, Common_Movement_WalkInPlaceFasterUp
	applymovement LOCALID_ROUTE111_VIAL_CHANSEY, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	msgbox Route111_Text_VialBlobGetBackHere, MSGBOX_DEFAULT
	applymovement LOCALID_ROUTE111_VIAL_CHANSEY, Route111_Movement_VialChanseyJump
	waitmovement 0
	playmoncry SPECIES_CHANSEY, CRY_MODE_NORMAL
	delay 40
	waitmoncry
	applymovement LOCALID_ROUTE111_VIAL_CHANSEY, Route111_Movement_VialChanseyEscape
	waitmovement 0
	msgbox Route111_Text_VialNurseNeedsHelp, MSGBOX_DEFAULT
	giveitem ITEM_HEAL_BALL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setvar VAR_CHANSEY_NURSE_STATE, 1
	setflag FLAG_HIDE_ROUTE111_VIAL_CHANSEY
	removeobject LOCALID_ROUTE111_VIAL_CHANSEY
	msgbox Route111_Text_VialHealBall, MSGBOX_DEFAULT
	releaseall
	end

Route111_EventScript_VialUpgradeNurse::
	lock
	faceplayer
	goto_if_eq VAR_CHANSEY_NURSE_STATE, 6, Route111_EventScript_VialUpgradeComplete
	goto_if_eq VAR_CHANSEY_NURSE_STATE, 0, Route111_EventScript_VialNurseWatching
	checkitem ITEM_HEAL_BALL
	goto_if_eq VAR_RESULT, TRUE, Route111_EventScript_VialNurseReminder
	msgbox Route111_Text_VialReplacementBall, MSGBOX_DEFAULT
	giveitem ITEM_HEAL_BALL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
Route111_EventScript_VialNurseReminder::
	msgbox Route111_Text_VialBringBlobBack, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_VialNurseWatching::
	msgbox Route111_Text_VialNurseWatching, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_VialUpgradeComplete::
	msgbox Route111_Text_VialThanks, MSGBOX_DEFAULT
	goto_if_ge VAR_POKE_VIAL_MAX_CHARGES, 2, Route111_EventScript_VialUpgradeMarkDone
	setvar VAR_POKE_VIAL_MAX_CHARGES, 2
	setvar VAR_POKE_VIAL_CHARGES, 2
Route111_EventScript_VialUpgradeMarkDone::
	setvar VAR_CHANSEY_NURSE_STATE, 7
	setflag FLAG_HIDE_ROUTE111_VIAL_NURSE
	removeobject LOCALID_ROUTE111_VIAL_NURSE
	release
	end

Route111_EventScript_VialPlayerFaceUp::
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	return

Route111_Movement_VialChanseyJump:
	jump_in_place_down
	delay_4
	jump_in_place_down
	step_end

Route111_Movement_VialChanseyEscape:
	walk_up
	walk_left
	walk_up
	walk_up
	walk_up
	step_end

Route111_Text_VialBlobGetBackHere:
	.string "Blob! Stop goofing around and get back\n"
	.string "here! How did you get over there?$"

Route111_Text_VialNurseNeedsHelp:
	.string "Oh, no! My CHANSEY ran north!\p"
	.string "Please bring Blob back. I'll improve\n"
	.string "your Poké Vial's capacity as thanks.$"

Route111_Text_VialHealBall:
	.string "Blob trusts this HEAL BALL.\n"
	.string "Follow the trail north and use it.$"

Route111_Text_VialNurseWatching:
	.string "Blob and I help travelers here.\n"
	.string "CHANSEY can be rather playful.$"

Route111_Text_VialReplacementBall:
	.string "You lost the HEAL BALL? Take another.\n"
	.string "Blob will recognize it.$"

Route111_Text_VialBringBlobBack:
	.string "Blob ran north along ROUTE 111.\n"
	.string "Please bring my CHANSEY home!$"

Route111_Text_VialThanks:
	.string "You caught Blob! Thank you!\p"
	.string "Poké Centers will now refill your\n"
	.string "Poké Vial with two full charges.$"

Route111_EventScript_Drew::
	trainerbattle_double TRAINER_DREW, Route111_Text_DrewIntro, Route111_Text_DrewDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_DrewPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Heidi::
	trainerbattle_double TRAINER_HEIDI, Route111_Text_HeidiIntro, Route111_Text_HeidiDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_HeidiPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Beau::
	trainerbattle_double TRAINER_BEAU, Route111_Text_BeauIntro, Route111_Text_BeauDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_BeauPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Becky::
	trainerbattle_double TRAINER_BECKY, Route111_Text_BeckyIntro, Route111_Text_BeckyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_BeckyPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Dusty::
	trainerbattle_single TRAINER_DUSTY_1, Route111_Text_DustyIntro, Route111_Text_DustyDefeat, Route111_EventScript_RegisterDusty
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route111_EventScript_RematchDusty
	msgbox Route111_Text_DustyPostBattle, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_RegisterDusty::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route111_Text_DustyRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_DUSTY_1
	release
	end

Route111_EventScript_RematchDusty::
	trainerbattle_rematch TRAINER_DUSTY_1, Route111_Text_DustyRematchIntro, Route111_Text_DustyRematchDefeat
	msgbox Route111_Text_DustyPostRematch, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Travis::
	trainerbattle_double TRAINER_TRAVIS, Route111_Text_TravisIntro, Route111_Text_TravisDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_TravisPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Irene::
	trainerbattle_single TRAINER_IRENE, Route111_Text_IreneIntro, Route111_Text_IreneDefeat
	msgbox Route111_Text_IrenePostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Daisuke::
	trainerbattle_single TRAINER_DAISUKE, Route111_Text_DaisukeIntro, Route111_Text_DaisukeDefeat
	msgbox Route111_Text_DaisukePostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Wilton::
	trainerbattle_double TRAINER_WILTON_1, Route111_Text_WiltonIntro, Route111_Text_WiltonDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route111_EventScript_RegisterWilton
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route111_EventScript_RematchWilton
	msgbox Route111_Text_WiltonPostBattle, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_RegisterWilton::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route111_Text_WiltonRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_WILTON_1
	release
	end

Route111_EventScript_RematchWilton::
	trainerbattle_rematch_double TRAINER_WILTON_1, Route111_Text_WiltonRematchIntro, Route111_Text_WiltonRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_WiltonPostRematch, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Brooke::
	trainerbattle_double TRAINER_BROOKE_1, Route111_Text_BrookeIntro, Route111_Text_BrookeDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route111_EventScript_RegisterBrooke
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route111_EventScript_RematchBrooke
	msgbox Route111_Text_BrookePostBattle, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_RegisterBrooke::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route111_Text_BrookeRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_BROOKE_1
	release
	end

Route111_EventScript_RematchBrooke::
	trainerbattle_rematch_double TRAINER_BROOKE_1, Route111_Text_BrookeRematchIntro, Route111_Text_BrookeRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_BrookePostRematch, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Hayden::
	trainerbattle_double TRAINER_HAYDEN, Route111_Text_HaydenIntro, Route111_Text_HaydenDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_HaydenPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Bianca::
	trainerbattle_double TRAINER_BIANCA, Route111_Text_BiancaIntro, Route111_Text_BiancaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_BiancaPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Tyron::
	trainerbattle_single TRAINER_TYRON, Route111_Text_TyronIntro, Route111_Text_TyronDefeat
	msgbox Route111_Text_TyronPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Celina::
	trainerbattle_double TRAINER_CELINA, Route111_Text_CelinaIntro, Route111_Text_CelinaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_CelinaPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Celia::
	trainerbattle_single TRAINER_CELIA, Route111_Text_CeliaIntro, Route111_Text_CeliaDefeat
	msgbox Route111_Text_CeliaPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Bryan::
	trainerbattle_double TRAINER_BRYAN, Route111_Text_BryanIntro, Route111_Text_BryanDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_BryanPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_Branden::
	trainerbattle_double TRAINER_BRANDEN, Route111_Text_BrandenIntro, Route111_Text_BrandenDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route111_Text_BrandenPostBattle, MSGBOX_AUTOCLOSE
	end

Route111_EventScript_TrainerHillSign::
	msgbox Route111_Text_TrainerHillSign, MSGBOX_SIGN
	end

Route111_Text_BattleOurFamily:
	.string "Hello! I take it you're a traveler.\p"
	.string "How's this? What do you say to taking\n"
	.string "on our family of four in a series of\l"
	.string "POKéMON battles?$"

Route111_Text_IsThatSo:
	.string "Is that so?\n"
	.string "Drop in, if you change your mind!$"

Route111_Text_VictorIntro:
	.string "That's the spirit! I like you!$"

Route111_Text_VictorDefeat:
	.string "Aiyah!\n"
	.string "You're a lot tougher than I expected!$"

Route111_Text_VictorPostBattle:
	.string "Hey, all!\n"
	.string "I've found a pretty strong TRAINER!$"

Route111_Text_VictoriaIntro:
	.string "Oh, my goodness! Aren't you young?\p"
	.string "You must be quite the TRAINER to beat\n"
	.string "my husband, though.\p"
	.string "It's my turn to battle now!$"

Route111_Text_VictoriaDefeat:
	.string "Oh, gosh!\n"
	.string "I can't get over how strong you are!$"

Route111_Text_VictoriaPostBattle:
	.string "There's a strong TRAINER here!\n"
	.string "This one's really strong!$"

Route111_Text_ViviIntro:
	.string "You're stronger than Mommy? Wow!\p"
	.string "But I'm strong, too!\n"
	.string "Really! Honestly!$"

Route111_Text_ViviDefeat:
	.string "Huh? Did I just lose?$"

Route111_Text_ViviPostBattle:
	.string "This stinks…\p"
	.string "…Snivel… Grandma!$"

Route111_Text_VickyIntro:
	.string "How dare you make my granddaughter\n"
	.string "cry!\p"
	.string "For that, I'm going to smack you!\n"
	.string "Prepare to lose!$"

Route111_Text_VickyDefeat:
	.string "Kwah! You are strong…\n"
	.string "My granddaughter was right…$"

Route111_Text_VickyPostBattle:
	.string "If you're not in any hurry,\n"
	.string "visit with us awhile.$"

Route111_Text_ToughToKeepWinningUpTheRanks:
	.string "If you don't raise your POKéMON some\n"
	.string "more, it could be tough to keep winning\l"
	.string "up through the ranks.\p"
	.string "I've heard the POKéMON LEAGUE's\n"
	.string "ELITE FOUR are far stronger than\l"
	.string "any GYM LEADER.$"

Route111_Text_WinstrateFamilyDestroyedMe:
	.string "I challenged the WINSTRATE family,\n"
	.string "but four matches in a row is tough\l"
	.string "going… They destroyed me.$"

Route111_Text_RouteSignMauville:
	.string "ROUTE 111\n"
	.string "{DOWN_ARROW} MAUVILLE CITY$"

Route111_Text_WinstrateHouseSign:
	.string "“Our family's hearts beat as one!”\n"
	.string "THE WINSTRATE'S HOUSE$"

Route111_Text_RouteSign112:
	.string "ROUTE 111\n"
	.string "{LEFT_ARROW} ROUTE 112$"

Route111_Text_RouteSign113:
	.string "ROUTE 111\n"
	.string "{LEFT_ARROW} ROUTE 113$"

Route111_Text_OldLadysRestStopSign:
	.string "OLD LADY'S REST STOP\n"
	.string "“Come in and rest your tired bones.”$"

Route111_Text_TrainerTipsSpAtkSpDef:
	.string "TRAINER TIPS\p"
	.string "One of the indicators of a POKéMON's\n"
	.string "powerfulness is SP. ATK. It stands for\l"
	.string "“SPECIAL ATTACK.”\p"
	.string "Likewise, SP. DEF stands for “SPECIAL\n"
	.string "DEFENSE.”$"

Route111_Text_ShouldBeMirageTowerAroundHere:
	.string "There should be a tower made of sand\n"
	.string "around here somewhere.\p"
	.string "But for some reason, it can be seen\n"
	.string "sometimes, and sometimes not.\p"
	.string "That's why I call it the MIRAGE TOWER.$"

Route111_Text_MirageTowerClearlyVisible:
	.string "The MIRAGE TOWER is visible! Beneath its\n"
	.string "first floor lies an older stairway.\p"
	.string "Archaeologists say it reaches a fossil\n"
	.string "archive called SANDSTREWN RUINS.$"

Route111_Text_ThatWasShockingSandRainedDown:
	.string "The tower collapsed, but its basement\n"
	.string "passage may still meet DESERT UNDERPASS.\p"
	.string "That route leads into SANDSTREWN RUINS.$"

Route111_Text_MirageTowerHasntBeenSeenSince:
	.string "MIRAGE TOWER is gone, but the ruins below\n"
	.string "remain. FOSSIL MANIAC's tunnel on\l"
	.string "ROUTE 114 now reaches DESERT UNDERPASS.$"

Route111_Text_ClawFossilDisappeared:
	.string "The CLAW FOSSIL disappeared into\n"
	.string "the sand…$"

Route111_Text_RootFossilDisappeared:
	.string "The ROOT FOSSIL disappeared into\n"
	.string "the sand…$"

Route111_Text_MauvilleUncleToldMeToTakeRockSmash:
	.string "Oh, no!\p"
	.string "My uncle in MAUVILLE told me to take\n"
	.string "ROCK SMASH with me if I was going to\l"
	.string "ROUTE 111!\p"
	.string "My uncle? He lives across from\n"
	.string "the bike shop in MAUVILLE.$"

Route111_Text_TrainerHillSign:
	.string "{RIGHT_ARROW} TRAINER HILL ENTRANCE\p"
	.string "“Scale the heights, you hot-blooded\n"
	.string "TRAINERS!”$"

@ Emerald Champions: everything north of this point (Route 111 west lane, Route 112,
@ Fiery Path, Routes 113-114, Meteor Falls, Mt. Chimney, Lavaridge) is authored at
@ chapter cap 40, the Dynamo Badge chapter. Vanilla never gated it, so a two-badge
@ player could walk into cap-40 trainers. A road worker holds the trail until Wattson
@ certifies the player; the three trigger tiles beside him push the player back.
Route111_EventScript_CheckDynamoGuard::
	goto_if_unset FLAG_BADGE03_GET, Route111_EventScript_CheckDynamoGuardEnd
	setflag FLAG_HIDE_ROUTE_111_DYNAMO_GUARD
	setvar VAR_TEMP_4, 1
Route111_EventScript_CheckDynamoGuardEnd::
	return

Route111_EventScript_DynamoGuard::
	lock
	faceplayer
	msgbox Route111_Text_DynamoGuardClosed, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_DynamoGuardTrigger::
	lockall
	msgbox Route111_Text_DynamoGuardTrigger, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Common_Movement_WalkDown
	waitmovement 0
	releaseall
	end

Route111_Text_DynamoGuardClosed:
	.string "The trail north is closed for now.\n"
	.string "The power lines up to LAVARIDGE went\l"
	.string "down in the last storm.\p"
	.string "WATTSON's crew is rewiring the route.\n"
	.string "Once his GYM certifies you with the\l"
	.string "DYNAMO BADGE, you're cleared to pass.$"

Route111_Text_DynamoGuardTrigger:
	.string "Hey! Not past the rope. The trail\n"
	.string "north is closed until WATTSON's crew\l"
	.string "finishes the power lines.$"
