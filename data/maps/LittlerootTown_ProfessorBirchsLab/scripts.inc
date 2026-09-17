LittlerootTown_ProfessorBirchsLab_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, LittlerootTown_ProfessorBirchsLab_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, LittlerootTown_ProfessorBirchsLab_OnWarp
	map_script MAP_SCRIPT_ON_FRAME_TABLE, LittlerootTown_ProfessorBirchsLab_OnFrame
	.byte 0

	@ State descriptions for VAR_BIRCH_POSTGAME_RESEARCH_STATE
	@ 1: Beat Elite Four, final field-note sync ready
	@ 2: Synchronized the postgame research notes
	@ 3: Left lab after the research sync
	@ 4: Entered lab after catching every Hoenn species
	@ 5: Legendary Form Research Kit offered or partially delivered
	@ 6: Legendary Form Research Kit fully delivered

	@ State descriptions for VAR_BIRCH_LAB_STATE
	@ 1: Never occurs
	@ 2: Chose starter
	@ 3: Received starter in lab
	@ 4: Defeated rival on Route 103
	@ 5: Completed the post-rival send-off
	@ 6: Legacy post-rival gift retry; new failures use an independent receipt.

LittlerootTown_ProfessorBirchsLab_OnTransition:
	call Common_EventScript_SetupRivalGfxId
	call ProfBirch_EventScript_UpdateLocation
	goto_if_ge VAR_BIRCH_POSTGAME_RESEARCH_STATE, 6, LittlerootTown_ProfessorBirchsLab_EventScript_SetAfterResearchRewardLayout
	goto_if_ge VAR_BIRCH_POSTGAME_RESEARCH_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_SetResearchRewardLayout
	goto_if_eq VAR_BIRCH_POSTGAME_RESEARCH_STATE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_CheckReadyForResearchReward
	end

LittlerootTown_ProfessorBirchsLab_EventScript_CheckReadyForResearchReward::
	specialvar VAR_RESULT, HasAllHoennMons
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_ProfessorBirchsLab_EventScript_SetReadyForResearchReward
	setobjectmovementtype LOCALID_BIRCHS_LAB_RIVAL, MOVEMENT_TYPE_WANDER_UP_AND_DOWN
	setobjectxyperm LOCALID_BIRCHS_LAB_RIVAL, 5, 10
	end

LittlerootTown_ProfessorBirchsLab_EventScript_SetReadyForResearchReward::
	setvar VAR_BIRCH_POSTGAME_RESEARCH_STATE, 4
	goto LittlerootTown_ProfessorBirchsLab_EventScript_SetResearchRewardLayout

LittlerootTown_ProfessorBirchsLab_EventScript_SetResearchRewardLayout::
	setmaplayoutindex LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB_WITH_TABLE
	end

LittlerootTown_ProfessorBirchsLab_EventScript_SetAfterResearchRewardLayout::
	setmaplayoutindex LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB_WITH_TABLE
	setobjectmovementtype LOCALID_BIRCHS_LAB_RIVAL, MOVEMENT_TYPE_WANDER_UP_AND_DOWN
	setobjectxyperm LOCALID_BIRCHS_LAB_RIVAL, 5, 10
	end

LittlerootTown_ProfessorBirchsLab_OnWarp:
	map_script_2 VAR_BIRCH_LAB_STATE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_SetPlayerPosForReceiveStarter
	map_script_2 VAR_BIRCH_POSTGAME_RESEARCH_STATE, 1, LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexUpgrade
	map_script_2 VAR_BIRCH_POSTGAME_RESEARCH_STATE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexUpgrade
	map_script_2 VAR_BIRCH_POSTGAME_RESEARCH_STATE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_AddRivalObject
	map_script_2 VAR_BIRCH_POSTGAME_RESEARCH_STATE, 6, LittlerootTown_ProfessorBirchsLab_EventScript_AddRivalObject
	map_script_2 VAR_BIRCH_POSTGAME_RESEARCH_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForResearchReward
	map_script_2 VAR_BIRCH_POSTGAME_RESEARCH_STATE, 5, LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForResearchReward
	.2byte 0

LittlerootTown_ProfessorBirchsLab_EventScript_SetPlayerPosForReceiveStarter::
	turnobject LOCALID_PLAYER, DIR_NORTH
	end

LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexUpgrade::
	turnobject LOCALID_PLAYER, DIR_NORTH
	addobject LOCALID_BIRCHS_LAB_BIRCH
	addobject LOCALID_BIRCHS_LAB_RIVAL
	turnobject LOCALID_BIRCHS_LAB_RIVAL, DIR_NORTH
	setobjectxy LOCALID_BIRCHS_LAB_RIVAL, 5, 5
	turnobject LOCALID_BIRCHS_LAB_BIRCH, DIR_SOUTH
	setobjectxy LOCALID_BIRCHS_LAB_BIRCH, 6, 4
	turnobject LOCALID_BIRCHS_LAB_AIDE, DIR_EAST
	setobjectxy LOCALID_BIRCHS_LAB_AIDE, 10, 10
	end

LittlerootTown_ProfessorBirchsLab_EventScript_AddRivalObject::
	addobject LOCALID_BIRCHS_LAB_RIVAL
	end

LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForResearchReward::
	addobject LOCALID_BIRCHS_LAB_RIVAL
	turnobject LOCALID_BIRCHS_LAB_BIRCH, DIR_SOUTH
	setobjectxy LOCALID_BIRCHS_LAB_BIRCH, 6, 4
	turnobject LOCALID_BIRCHS_LAB_RIVAL, DIR_EAST
	setobjectxy LOCALID_BIRCHS_LAB_RIVAL, 5, 5
	end

LittlerootTown_ProfessorBirchsLab_OnFrame:
	map_script_2 VAR_BIRCH_LAB_STATE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_GiveStarterEvent
	map_script_2 VAR_BIRCH_LAB_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedexEvent
	map_script_2 VAR_BIRCH_POSTGAME_RESEARCH_STATE, 1, LittlerootTown_ProfessorBirchsLab_EventScript_UpgradeToNationalDex
	map_script_2 VAR_BIRCH_POSTGAME_RESEARCH_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_OfferResearchReward
	.2byte 0

@ Both permanent partners are already owned before the rescue.
LittlerootTown_ProfessorBirchsLab_EventScript_GiveStarterEvent::
	lockall
	setvar VAR_BIRCH_LAB_STATE, 3
	special BufferEmeraldChampionsStarterNames
	msgbox LittlerootTown_ProfessorBirchsLab_Text_YourTwoPartners, MSGBOX_DEFAULT
	setvar VAR_0x8004, 0
	special SelectEmeraldChampionsStarterForNaming
	call_if_eq VAR_RESULT, TRUE, LittlerootTown_ProfessorBirchsLab_EventScript_OfferOpeningNickname
	setvar VAR_0x8004, 1
	special SelectEmeraldChampionsStarterForNaming
	call_if_eq VAR_RESULT, TRUE, LittlerootTown_ProfessorBirchsLab_EventScript_OfferOpeningNickname
	goto LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSupplies

LittlerootTown_ProfessorBirchsLab_EventScript_OfferOpeningNickname::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_NameOpeningPartner, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Common_EventScript_NopReturn
	call Common_EventScript_NameReceivedPartyMon
	return

LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSupplies::
	setvar VAR_BIRCH_LAB_STATE, 3

LittlerootTown_ProfessorBirchsLab_EventScript_OpeningBalls::
	special GiveEmeraldChampionsOpeningBalls
	goto_if_eq VAR_RESULT, 0, LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSuppliesFull
	goto_if_eq VAR_RESULT, 2, LittlerootTown_ProfessorBirchsLab_EventScript_OpeningBallsInPC
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OpeningBalls, MSGBOX_DEFAULT
	goto LittlerootTown_ProfessorBirchsLab_EventScript_OpeningReady

LittlerootTown_ProfessorBirchsLab_EventScript_OpeningBallsInPC::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OpeningBallsInPC, MSGBOX_DEFAULT

LittlerootTown_ProfessorBirchsLab_EventScript_OpeningReady::
	clearflag FLAG_HIDE_ROUTE_101_BOY
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OpeningReady, MSGBOX_DEFAULT
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_OpeningReminder::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OpeningReady, MSGBOX_DEFAULT
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSuppliesFull::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OpeningSuppliesFull, MSGBOX_DEFAULT
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedexEvent::
	lockall
	applymovement LOCALID_PLAYER, LittlerootTown_ProfessorBirchsLab_Movement_PlayerEnterLabForPokedex
	waitmovement 0
	goto LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex
	end

LittlerootTown_ProfessorBirchsLab_Movement_PlayerEnterLabForPokedex:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

LittlerootTown_ProfessorBirchsLab_EventScript_UpgradeToNationalDex::
	lockall
	delay 30
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OtherRegionsUpgradeToNational, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_BIRCHS_LAB_BIRCH, LittlerootTown_ProfessorBirchsLab_Movement_BirchRetrievePokedexes
	waitmovement 0
	delay 20
	playse SE_CLICK
	delay 10
	playse SE_CLICK
	delay 10
	playse SE_CLICK
	delay 10
	playse SE_CLICK
	delay 20
	applymovement LOCALID_BIRCHS_LAB_RIVAL, Common_Movement_WalkInPlaceFasterRight
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	checkplayergender
	call_if_eq VAR_RESULT, MALE, LittlerootTown_ProfessorBirchsLab_EventScript_MayUpgradeComment
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_ProfessorBirchsLab_EventScript_BrendanUpgradeComment
	playse SE_PC_ON
	waitse
	delay 20
	applymovement LOCALID_BIRCHS_LAB_BIRCH, LittlerootTown_ProfessorBirchsLab_Movement_BirchReturnPokedex
	waitmovement 0
	applymovement LOCALID_BIRCHS_LAB_RIVAL, Common_Movement_WalkInPlaceFasterUp
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OkayAllDone, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message LittlerootTown_ProfessorBirchsLab_Text_PokedexUpgradedToNational
	waitmessage
	waitfanfare
	setflag FLAG_SYS_NATIONAL_DEX
	special EnableNationalPokedex
	msgbox LittlerootTown_ProfessorBirchsLab_Text_GrassyPatchWaiting2, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCH
	setflag FLAG_HIDE_LITTLEROOT_TOWN_RIVAL
	setvar VAR_BIRCH_POSTGAME_RESEARCH_STATE, 2
	clearflag FLAG_SCOTT_CALL_BATTLE_FRONTIER
	goto_if_set FLAG_MET_SCOTT_ON_SS_TIDAL, LittlerootTown_ProfessorBirchsLab_EventScript_ResearchSyncFinished
	setvar VAR_SCOTT_BF_CALL_STEP_COUNTER, 0
	setflag FLAG_SCOTT_CALL_BATTLE_FRONTIER
LittlerootTown_ProfessorBirchsLab_EventScript_ResearchSyncFinished::
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayUpgradeComment::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayUpgradeSoCool, MSGBOX_DEFAULT
	closemessage
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanUpgradeComment::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanYouCanThankMe, MSGBOX_DEFAULT
	closemessage
	return

LittlerootTown_ProfessorBirchsLab_Movement_BirchRetrievePokedexes:
	walk_left
	walk_in_place_faster_down
	delay_16
	delay_16
	walk_right
	walk_in_place_faster_down
	delay_16
	delay_16
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_up
	step_end

LittlerootTown_ProfessorBirchsLab_Movement_BirchReturnPokedex:
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	walk_left
	walk_in_place_faster_down
	step_end

LittlerootTown_ProfessorBirchsLab_EventScript_OfferResearchReward::
	lockall
	applymovement LOCALID_PLAYER, LittlerootTown_ProfessorBirchsLab_Movement_PlayerEnterLabForResearchReward
	waitmovement 0
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CompletedDexResearchReward, MSGBOX_DEFAULT
	setvar VAR_BIRCH_POSTGAME_RESEARCH_STATE, 5
	goto LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveResearchReward

LittlerootTown_ProfessorBirchsLab_Movement_PlayerEnterLabForResearchReward:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

LittlerootTown_ProfessorBirchsLab_EventScript_Aide::
	lock
	faceplayer
	goto_if_eq VAR_EC_OPENING_STATE, EC_OPENING_RESCUE_WON, LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSupplies
	goto_if_eq VAR_BIRCH_LAB_STATE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_OpeningReminder
	call Common_EventScript_EmeraldChampionsResearchPartners
	goto_if_ge VAR_BIRCH_LAB_STATE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_AideReceivedStarter
	goto_if_set FLAG_BIRCH_AIDE_MET, LittlerootTown_ProfessorBirchsLab_EventScript_AideAlreadyMet
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BirchAwayOnFieldwork, MSGBOX_DEFAULT
	setflag FLAG_BIRCH_AIDE_MET
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_AideAlreadyMet::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BirchIsntOneForDeskWork, MSGBOX_DEFAULT
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_AideReceivedStarter::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BirchEnjoysRivalsHelpToo, MSGBOX_DEFAULT
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardWaiting::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_ResearchRewardWaiting, MSGBOX_DEFAULT

@ Deliver each tool independently.  If the Key Items pocket fills partway,
@ state 5 and the per-item ownership checks make the reward exactly-once and
@ resumable without duplicating anything already received.
LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveResearchReward::
	checkitem ITEM_DNA_SPLICERS, 1
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveZygardeCube
	giveitem ITEM_DNA_SPLICERS
	goto_if_eq VAR_RESULT, FALSE, LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardBagFull

LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveZygardeCube::
	checkitem ITEM_ZYGARDE_CUBE, 1
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveNSolarizer
	giveitem ITEM_ZYGARDE_CUBE
	goto_if_eq VAR_RESULT, FALSE, LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardBagFull

LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveNSolarizer::
	checkitem ITEM_N_SOLARIZER, 1
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveNLunarizer
	giveitem ITEM_N_SOLARIZER
	goto_if_eq VAR_RESULT, FALSE, LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardBagFull

LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveNLunarizer::
	checkitem ITEM_N_LUNARIZER, 1
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveReinsOfUnity
	giveitem ITEM_N_LUNARIZER
	goto_if_eq VAR_RESULT, FALSE, LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardBagFull

LittlerootTown_ProfessorBirchsLab_EventScript_TryGiveReinsOfUnity::
	checkitem ITEM_REINS_OF_UNITY, 1
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardComplete
	giveitem ITEM_REINS_OF_UNITY
	goto_if_eq VAR_RESULT, FALSE, LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardBagFull

LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardComplete::
	setvar VAR_BIRCH_POSTGAME_RESEARCH_STATE, 6
	msgbox LittlerootTown_ProfessorBirchsLab_Text_ResearchRewardComplete, MSGBOX_DEFAULT
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardBagFull::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_ResearchRewardBagFull, MSGBOX_DEFAULT
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_Birch::
	lock
	faceplayer
	goto_if_eq VAR_BIRCH_LAB_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex
	goto_if_eq VAR_BIRCH_LAB_STATE, 6, LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex
	goto_if_eq VAR_EC_OPENING_STATE, EC_OPENING_RESCUE_WON, LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSupplies
	goto_if_eq VAR_BIRCH_LAB_STATE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_OpeningReminder
	call Common_EventScript_EmeraldChampionsResearchPartners
	goto_if_eq VAR_BIRCH_POSTGAME_RESEARCH_STATE, 5, LittlerootTown_ProfessorBirchsLab_EventScript_ResearchRewardWaiting
	goto_if_eq VAR_BIRCH_POSTGAME_RESEARCH_STATE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_GrassyPatchWaiting
	goto LittlerootTown_ProfessorBirchsLab_EventScript_TryRatePokedexOrRegister
	end

LittlerootTown_ProfessorBirchsLab_EventScript_GrassyPatchWaiting::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_GrassyPatchWaiting2, MSGBOX_DEFAULT
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_TryRatePokedexOrRegister::
	goto_if_unset FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_UNKNOWN_0x380, ProfBirch_EventScript_RatePokedexOrRegister
	goto_if_eq VAR_BIRCH_POSTGAME_RESEARCH_STATE, 3, ProfBirch_EventScript_RatePokedexOrRegister
	goto_if_ge VAR_BIRCH_POSTGAME_RESEARCH_STATE, 6, ProfBirch_EventScript_RatePokedexOrRegister
	goto_if_eq VAR_BIRCH_LAB_STATE, 5, LittlerootTown_ProfessorBirchsLab_EventScript_PokemonAwait
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BirchRivalGoneHome, MSGBOX_DEFAULT
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OpeningDebrief, MSGBOX_DEFAULT
	call_if_unset FLAG_RECEIVED_POKEDEX_FROM_BIRCH, LittlerootTown_ProfessorBirchsLab_EventScript_ReceivePokedex
	giveitem ITEM_GREAT_BALL, 10
	goto_if_eq VAR_RESULT, TRUE, LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSendoff
	addpcitem ITEM_GREAT_BALL, 10
	goto_if_eq VAR_RESULT, FALSE, LittlerootTown_ProfessorBirchsLab_EventScript_DebriefBagFull
	msgbox LittlerootTown_ProfessorBirchsLab_Text_GreatBallsInPC, MSGBOX_DEFAULT

LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSendoff::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OpeningSendoff, MSGBOX_DEFAULT
	setvar VAR_BIRCH_LAB_STATE, 5
	setvar VAR_EC_OPENING_STATE, EC_OPENING_COMPLETE
	setflag FLAG_ADVENTURE_STARTED
	setvar VAR_OLDALE_TOWN_STATE, 1
	setvar VAR_LITTLEROOT_RIVAL_STATE, 4
	setvar VAR_LITTLEROOT_TOWN_STATE, 3
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_DebriefBagFull::
	setflag FLAG_EC_BIRCH_GREAT_BALLS_PENDING
	msgbox LittlerootTown_ProfessorBirchsLab_Text_GreatBallsWaiting, MSGBOX_DEFAULT
	goto LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSendoff

LittlerootTown_ProfessorBirchsLab_EventScript_ReceivePokedex::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OpeningDex, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	message LittlerootTown_ProfessorBirchsLab_Text_ReceivedPokedex
	waitfanfare
	setflag FLAG_SYS_POKEDEX_GET
	setflag FLAG_SYS_NATIONAL_DEX
	special SetUnlockedPokedexFlags
	special EnableNationalPokedex
	setflag FLAG_RECEIVED_POKEDEX_FROM_BIRCH
	setvar VAR_CABLE_CLUB_TUTORIAL_STATE, 1
	return

LittlerootTown_ProfessorBirchsLab_EventScript_PokemonAwait::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CountlessPokemonAwait, MSGBOX_DEFAULT
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayNoRoomForPokeBalls::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OhYourBagsFull, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanNoRoomForPokeBalls::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_HeyYourBagsFull, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_Movement_RivalApproachPlayer:
	walk_down
	walk_in_place_faster_left
	step_end

LittlerootTown_ProfessorBirchsLab_EventScript_Machine::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_SeriousLookingMachine, MSGBOX_SIGN
	end

LittlerootTown_ProfessorBirchsLab_EventScript_Rival::
	lock
	faceplayer
	goto_if_eq VAR_BIRCH_POSTGAME_RESEARCH_STATE, 5, LittlerootTown_ProfessorBirchsLab_EventScript_RivalFuturePlans
	goto_if_ge VAR_BIRCH_POSTGAME_RESEARCH_STATE, 6, LittlerootTown_ProfessorBirchsLab_EventScript_RivalHaveYouGoneToBattleFrontier
	goto_if_ge VAR_BIRCH_POSTGAME_RESEARCH_STATE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_RivalTakeBreakFromFieldwork
	checkplayergender
	call_if_eq VAR_RESULT, MALE, LittlerootTown_ProfessorBirchsLab_EventScript_MayWhereShouldIGoNext
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_ProfessorBirchsLab_EventScript_BrendanWhereShouldIGoNext
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayWhereShouldIGoNext::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayWhereShouldIGoNext, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanWhereShouldIGoNext::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanWhereShouldIGoNext, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_RivalFuturePlans::
	checkplayergender
	call_if_eq VAR_RESULT, MALE, LittlerootTown_ProfessorBirchsLab_EventScript_MayWhatNextImStayingHere
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_ProfessorBirchsLab_EventScript_BrendanPreferCollectingSlowly
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayWhatNextImStayingHere::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayWhatNextImStayingHere, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanPreferCollectingSlowly:
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanPreferCollectingSlowly, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_RivalHaveYouGoneToBattleFrontier::
	checkplayergender
	call_if_eq VAR_RESULT, MALE, LittlerootTown_ProfessorBirchsLab_EventScript_MayHaveYouGoneToBattleFrontier
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_ProfessorBirchsLab_EventScript_BrendanHaveYouGoneToBattleFrontier
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayHaveYouGoneToBattleFrontier::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayHaveYouGoneToBattleFrontier, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanHaveYouGoneToBattleFrontier::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanHaveYouGoneToBattleFrontier, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_RivalTakeBreakFromFieldwork::
	checkplayergender
	call_if_eq VAR_RESULT, MALE, LittlerootTown_ProfessorBirchsLab_EventScript_MayTakeBreakFromFieldwork
	call_if_eq VAR_RESULT, FEMALE, LittlerootTown_ProfessorBirchsLab_EventScript_BrendanTakeBreakFromFieldwork
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayTakeBreakFromFieldwork::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayTakeBreakFromFieldwork, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanTakeBreakFromFieldwork::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanTakeBreakFromFieldwork, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_PC::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_PCUsedForResearch, MSGBOX_SIGN
	end

LittlerootTown_ProfessorBirchsLab_EventScript_Bookshelf::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CrammedWithBooksOnPokemon, MSGBOX_SIGN
	end

LittlerootTown_ProfessorBirchsLab_EventScript_Book::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BookTooHardToRead, MSGBOX_SIGN
	end

LittlerootTown_ProfessorBirchsLab_EventScript_ScottAboardSSTidalCall::
	lockall
	pokenavcall LittlerootTown_ProfessorBirchsLab_Text_ScottAboardSSTidalCall
	waitmessage
	clearflag FLAG_SCOTT_CALL_BATTLE_FRONTIER
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_Text_BirchAwayOnFieldwork:
	.string "Looking for PROF. BIRCH?\p"
	.string "He's on ROUTE 101, north of town,\n"
	.string "studying POKéMON in the wild.\p"
	.string "He calls it fieldwork. I call it\n"
	.string "a good excuse to leave the desk!$"

LittlerootTown_ProfessorBirchsLab_Text_BirchIsntOneForDeskWork:
	.string "The PROF. prefers fieldwork to\n"
	.string "paperwork. Try ROUTE 101, just\l"
	.string "north of town.$"

LittlerootTown_ProfessorBirchsLab_Text_BirchEnjoysRivalsHelpToo:
	.string "PROF. BIRCH is studying the habitats\n"
	.string "and distribution of POKéMON.\p"
	.string "The PROF enjoys {RIVAL}'s help, too.\n"
	.string "There's a lot of love there.$"

LittlerootTown_ProfessorBirchsLab_Text_LikeYouToHavePokemon:
	.string "PROF. BIRCH: {PLAYER}, thank you!\n"
	.string "I really was in a spot back there.\p"
	.string "My colleagues sent these POKéMON from\n"
	.string "nine regions. Quite a welcome to HOENN!\p"
	.string "That one trusted you when it mattered.\n"
	.string "I'd like you to keep traveling together.\p"
	.string "{PLAYER} received the {STR_VAR_1}!$"

LittlerootTown_ProfessorBirchsLab_Text_WhyNotGiveNicknameToMon:
	.string "PROF. BIRCH: While you're at it, why not\n"
	.string "give a nickname to that {STR_VAR_1}?$"

LittlerootTown_ProfessorBirchsLab_Text_MightBeGoodIdeaToGoSeeRival:
	.string "{RIVAL} is studying POKéMON on\n"
	.string "ROUTE 103. Why not introduce your\l"
	.string "new partners?\p"
	.string "OLDALE's CENTER can help you\n"
	.string "prepare. Its free LEVELER brings\l"
	.string "your POKéMON up to the current\l"
	.string "training limit.\p"
	.string "Would you like to go find\n"
	.string "{RIVAL}?$"

LittlerootTown_ProfessorBirchsLab_Text_GetRivalToTeachYou:
	.string "Good! You can learn a lot from a battle,\n"
	.string "even one you lose. Take care out there!$"

LittlerootTown_ProfessorBirchsLab_Text_DontBeThatWay:
	.string "PROF. BIRCH: Oh, don't be that way.\n"
	.string "You should go meet my kid.$"

LittlerootTown_ProfessorBirchsLab_Text_BirchRivalGoneHome:
	.string "PROF. BIRCH: {RIVAL}?\n"
	.string "Gone home, I think.\p"
	.string "Or maybe that kid's scrabbling around\n"
	.string "in tall grass again somewhere…\p"
	.string "If you or your POKéMON get tired,\n"
	.string "you should get some rest at home.$"

LittlerootTown_ProfessorBirchsLab_Text_HeardYouBeatRivalTakePokedex:
	.string "PROF. BIRCH: {RIVAL} told me all about\n"
	.string "your battle! Sounds like quite a match.\p"
	.string "I have something for your journey. This\n"
	.string "POKéDEX records each species you meet,\l"
	.string "even the ones you haven't caught.\p"
	.string "I can't see all of HOENN from here.\n"
	.string "Will you tell me what you discover?$"

LittlerootTown_ProfessorBirchsLab_Text_ReceivedPokedex:
	.string "{PLAYER} received the POKéDEX!$"

LittlerootTown_ProfessorBirchsLab_Text_ExplainPokedex:
	.string "Read the signs along each route. They'll\n"
	.string "help you find the POKéMON living there.\p"
	.string "Catch the ones that interest you. Try\n"
	.string "traveling with different companions.\p"
	.string "The LEVELER helps new teammates catch\n"
	.string "up. But tough battles still take care\l"
	.string "and preparation. Look after each other!$"

LittlerootTown_ProfessorBirchsLab_Text_CountlessPokemonAwait:
	.string "Take ROUTE 102 west from OLDALE\n"
	.string "to PETALBURG CITY. Your dad will\l"
	.string "be happy to see you at his GYM.\p"
	.string "Bring back a few discoveries\n"
	.string "of your own, too!$"

LittlerootTown_ProfessorBirchsLab_Text_MayGotPokedexTooTakeThese:
	.string "MAY: Oh, wow, {PLAYER}{KUN}!\n"
	.string "You got a POKéDEX, too!\p"
	.string "That's great! Just like me!\n"
	.string "I've got something for you, too!$"

LittlerootTown_ProfessorBirchsLab_Text_CatchCutePokemonWithPokeBalls:
	.string "MAY: Take these POKé BALLS! I always\n"
	.string "meet new friends in unexpected places.\p"
	.string "Catch lots of POKéMON. You can change\n"
	.string "your team to suit the battles ahead!$"

LittlerootTown_ProfessorBirchsLab_Text_OhYourBagsFull:
	.string "Oh? Your BAG's full.$"

LittlerootTown_ProfessorBirchsLab_Text_MayWhereShouldIGoNext:
	.string "MAY: I wonder where I should go look\n"
	.string "for POKéMON next?$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanGotPokedexTooTakeThese:
	.string "BRENDAN: Huh…\n"
	.string "So you got a POKéDEX, too.\p"
	.string "Well then, here.\n"
	.string "I'll give you these.$"

LittlerootTown_ProfessorBirchsLab_Text_CatchCoolPokemonWithPokeBalls:
	.string "BRENDAN: Here, take these POKé BALLS.\n"
	.string "There's more out there than you think.\p"
	.string "Catch different POKéMON. Then you can\n"
	.string "choose the right six for each battle.$"

LittlerootTown_ProfessorBirchsLab_Text_HeyYourBagsFull:
	.string "Hey, your BAG's full.$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanWhereShouldIGoNext:
	.string "BRENDAN: Where should I look for\n"
	.string "POKéMON next…$"

LittlerootTown_ProfessorBirchsLab_Text_SeriousLookingMachine:
	.string "It's a serious-looking machine.\n"
	.string "The PROF must use this for research.$"

LittlerootTown_ProfessorBirchsLab_Text_PCUsedForResearch:
	.string "It's a PC used for research.\n"
	.string "Better not mess around with it.$"

LittlerootTown_ProfessorBirchsLab_Text_CrammedWithBooksOnPokemon:
	.string "It's crammed with books on POKéMON.$"

LittlerootTown_ProfessorBirchsLab_Text_BookTooHardToRead:
	.string "It's a book that's too hard to read.$"

LittlerootTown_ProfessorBirchsLab_Text_OtherRegionsUpgradeToNational:
	.string "PROF. BIRCH: {PLAYER}{KUN},\n"
	.string "{RIVAL}, you've both brought back\l"
	.string "so many field notes!\p"
	.string "Your POKéDEX units could record\n"
	.string "every region from the start.\l"
	.string "Let's add our latest observations\l"
	.string "to both.\p"
	.string "May I see them for a moment?$"

LittlerootTown_ProfessorBirchsLab_Text_MayUpgradeSoCool:
	.string "MAY: Eheheh!\p"
	.string "It's so cool that our field notes\n"
	.string "are finally synchronized!\p"
	.string "It's because you went out and caught\n"
	.string "so many POKéMON, {PLAYER}{KUN}!$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanYouCanThankMe:
	.string "BRENDAN: I went out all over HOENN\n"
	.string "and checked out POKéMON.\p"
	.string "You can thank me for the final\n"
	.string "research notes.\p"
	.string "Yep, you're lucky, {PLAYER}!$"

LittlerootTown_ProfessorBirchsLab_Text_OkayAllDone:
	.string "PROF. BIRCH: Okay, all done!$"

LittlerootTown_ProfessorBirchsLab_Text_PokedexUpgradedToNational:
	.string "{PLAYER}'s POKéDEX synchronized\n"
	.string "the CHAMPION research notes!$"

LittlerootTown_ProfessorBirchsLab_Text_GrassyPatchWaiting2:
	.string "PROF. BIRCH: But listen.\n"
	.string "You've become the CHAMPION,\l"
	.string "but your POKéMON journey isn't over.\p"
	.string "There is no end to the road\n"
	.string "that is POKéMON.\p"
	.string "Somewhere, there is a grassy patch\n"
	.string "that's waiting for you!$"

LittlerootTown_ProfessorBirchsLab_Text_MayTakeBreakFromFieldwork:
	.string "MAY: I think I'll take a short break\n"
	.string "from fieldwork.\p"
	.string "I think I'll help the PROF here for\n"
	.string "a while.$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanTakeBreakFromFieldwork:
	.string "BRENDAN: For the time being,\n"
	.string "I'm taking a break from fieldwork.\p"
	.string "I'll be helping out the PROF here\n"
	.string "for a while.$"

LittlerootTown_ProfessorBirchsLab_Text_CompletedDexResearchReward:
	.string "PROF. BIRCH: Every known HOENN\n"
	.string "species! What a remarkable record\l"
	.string "of your journey.\p"
	.string "I've kept a spare set of our\n"
	.string "legendary form tools here. Take\l"
	.string "any you're missing!\p"
	.string "They're also in the CENTER\n"
	.string "vendors' Form Tools archive.\l"
	.string "Your MEGA RING gives you access\l"
	.string "there.$"

LittlerootTown_ProfessorBirchsLab_Text_ResearchRewardWaiting:
	.string "PROF. BIRCH: Your LEGENDARY FORM tools\n"
	.string "are still here. Let's finish the set.$"

LittlerootTown_ProfessorBirchsLab_Text_ResearchRewardComplete:
	.string "The tools let KYUREM, ZYGARDE,\n"
	.string "NECROZMA, and CALYREX change\l"
	.string "forms or combine with certain\l"
	.string "partners.\p"
	.string "There's always something new to\n"
	.string "learn about POKéMON!$"

LittlerootTown_ProfessorBirchsLab_Text_ResearchRewardBagFull:
	.string "Your KEY ITEMS pocket is full.\n"
	.string "Make room and return; every\l"
	.string "remaining tool is safe.$"

LittlerootTown_ProfessorBirchsLab_Text_MayWhatNextImStayingHere:
	.string "MAY: {PLAYER}{KUN}, after this…\n"
	.string "What are you going to do?\p"
	.string "Are you going to keep battling\n"
	.string "and sharpening your skills?\p"
	.string "Or are you going to try filling\n"
	.string "the NATIONAL POKéDEX?\p"
	.string "I'm staying here to help the PROF.$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanPreferCollectingSlowly:
	.string "BRENDAN: Rather than collecting\n"
	.string "POKéMON, I prefer slowly and \l"
	.string "steadily raising the one I chose.$"

LittlerootTown_ProfessorBirchsLab_Text_MayHaveYouGoneToBattleFrontier:
	.string "MAY: Oh, hi, {PLAYER}{KUN}!\n"
	.string "Have you gone to that place,\l"
	.string "the BATTLE FRONTIER?$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanHaveYouGoneToBattleFrontier:
	.string "BRENDAN: Hey, {PLAYER}!\n"
	.string "Have you gone out to that place,\l"
	.string "the BATTLE FRONTIER?$"

LittlerootTown_ProfessorBirchsLab_Text_ScottAboardSSTidalCall:
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "SCOTT: Hi, hi, {PLAYER}!\n"
	.string "It's me, SCOTT.\p"
	.string "I'm aboard the S.S. TIDAL now.\n"
	.string "It feels great to be on the sea!\p"
	.string "By the way…\p"
	.string "There's this place that I'd like to\n"
	.string "invite you to for a visit.\p"
	.string "If you're interested, board a ferry\n"
	.string "at either SLATEPORT or LILYCOVE.\p"
	.string "I'll fill you in on the details when\n"
	.string "we meet. I'll be waiting!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

LittlerootTown_ProfessorBirchsLab_Text_YourTwoPartners:
	.string "PROF. BIRCH: You and both of these\n"
	.string "POKéMON were brave out there.\p"
	.string "I'd like {STR_VAR_1} and\n"
	.string "{STR_VAR_2} to stay with you.\p"
	.string "I think you're going to make\n"
	.string "a wonderful team!$"

LittlerootTown_ProfessorBirchsLab_Text_NameOpeningPartner:
	.string "Would you like to nickname\n"
	.string "{STR_VAR_1}?$"

LittlerootTown_ProfessorBirchsLab_Text_OpeningDex:
	.string "Take this POKéDEX, too. It records\n"
	.string "the POKéMON you see and catch.\p"
	.string "HOENN's weather has been strange.\n"
	.string "POKéMON are appearing in places\l"
	.string "we've never seen them before.\p"
	.string "Even LEGENDARY POKéMON, once known\n"
	.string "only through stories, are being\l"
	.string "sighted along our routes!\p"
	.string "Are they following the weather,\n"
	.string "or is something drawing them here?\l"
	.string "Your discoveries could help us learn.$"

LittlerootTown_ProfessorBirchsLab_Text_OpeningBalls:
	.string "You received 20 POKé BALLS!$"

LittlerootTown_ProfessorBirchsLab_Text_OpeningBallsInPC:
	.string "Your 20 POKé BALLS were sent\n"
	.string "to the item PC.$"

LittlerootTown_ProfessorBirchsLab_Text_OpeningReady:
	.string "{RIVAL} is studying POKéMON on\n"
	.string "ROUTE 103, north of OLDALE TOWN.\p"
	.string "Stop at OLDALE's POKéMON CENTER.\n"
	.string "Collect your free tools and ask\l"
	.string "the TUTOR about Battle Sets.\p"
	.string "They prepare moves and stats.\n"
	.string "Use the LEVELER to raise your\l"
	.string "party; battles give no experience.\p"
	.string "Catch a few more partners, then\n"
	.string "try a battle with {RIVAL}!\p"
	.string "If you need another try, choose\n"
	.string "RUN in a TRAINER battle. You can\l"
	.string "restart that battle for free.$"

LittlerootTown_ProfessorBirchsLab_Text_OpeningSuppliesFull:
	.string "Your field supplies are waiting.\p"
	.string "Make room in your Bag or item PC,\n"
	.string "then speak to me or the aide.$"

LittlerootTown_ProfessorBirchsLab_Text_OpeningDebrief:
	.string "PROF. BIRCH: Welcome back, {PLAYER}!\n"
	.string "You beat {RIVAL}? Well done!\p"
	.string "First you helped me out of a\n"
	.string "scrape, and now your first win\l"
	.string "against another TRAINER!\p"
	.string "There's nothing like getting out\n"
	.string "in the field with your partners.\l"
	.string "Even I learn something every day!$"

LittlerootTown_ProfessorBirchsLab_Text_OpeningSendoff:
	.string "Head west from OLDALE along\n"
	.string "ROUTE 102 to PETALBURG CITY.\p"
	.string "Your dad runs the GYM there.\n"
	.string "Go show him your team!\p"
	.string "Read the route signs as you go.\n"
	.string "They list local POKéMON by grass,\l"
	.string "water, and fishing method.\p"
	.string "Keep reading past the fishing\n"
	.string "lists for local LEGENDARIES. Many\l"
	.string "can be found in the wild!\p"
	.string "For more leads, ask the local\n"
	.string "guide near the PC in each\l"
	.string "POKéMON CENTER.\p"
	.string "He can tell you which LEGENDARIES\n"
	.string "are nearby and who to talk to\l"
	.string "about them. Enjoy the journey!$"

LittlerootTown_ProfessorBirchsLab_Text_GreatBallsInPC:
	.string "Your 10 GREAT BALLS were sent\n"
	.string "to the item PC.$"

LittlerootTown_ProfessorBirchsLab_Text_GreatBallsWaiting:
	.string "Your Bag and item PC are full.\p"
	.string "I'll send your 10 GREAT BALLS to a\n"
	.string "POKéMON CENTER. Make room, then ask\l"
	.string "any nurse. Your journey can begin!$"
