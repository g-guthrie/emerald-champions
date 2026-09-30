.set LOCALID_AIDE, 1
.set LOCALID_BIRCH, 2
.set LOCALID_RIVAL, 3

LittlerootTown_ProfessorBirchsLab_MapScripts:: @ 81F9C91
	map_script MAP_SCRIPT_ON_TRANSITION, LittlerootTown_ProfessorBirchsLab_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, LittlerootTown_ProfessorBirchsLab_OnWarp
	map_script MAP_SCRIPT_ON_FRAME_TABLE, LittlerootTown_ProfessorBirchsLab_OnFrame
	.byte 0

	@ State descriptions for VAR_DEX_UPGRADE_JOHTO_STARTER_STATE
	@ 1: Beat Elite Four, Dex upgrade ready
	@ 2: Received National Dex
	@ 3: Left lab after receiving National Dex
	@ 4: Entered lab after catching all Hoenn mons; completion gift ready
	@ 5: Completion gift pending (also migrates an old pending Johto choice)
	@ 6: Claimed completion gift (or a Johto starter on an older save)

	@ State descriptions for VAR_BIRCH_LAB_STATE
	@ 1: Never occurs
	@ 2: Chose starter
	@ 3: Received starter in lab
	@ 4: Defeated rival on Route 103
	@ 5: Received pokedex

LittlerootTown_ProfessorBirchsLab_OnTransition: @ 81F9CA1
	call Common_EventScript_SetupRivalGfxId
	call ProfBirch_EventScript_UpdateLocation
	@ Retire the old starter display, including pending choices on old saves.
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_CYNDAQUIL
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_TOTODILE
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_POKEBALL_CHIKORITA
	goto_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 7, LittlerootTown_ProfessorBirchsLab_EventScript_KeepCosmogPending
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6
	goto_if_ge LittlerootTown_ProfessorBirchsLab_EventScript_SetAfterDexRewardLayout
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 4
	goto_if_ge LittlerootTown_ProfessorBirchsLab_EventScript_SetDexRewardLayout
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_CheckReadyForDexReward
	end

LittlerootTown_ProfessorBirchsLab_EventScript_CheckReadyForDexReward:: @ 81F9CCD
	specialvar VAR_RESULT, HasAllHoennMons
	compare VAR_RESULT, TRUE
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_SetReadyForDexReward
	setobjectmovementtype LOCALID_RIVAL, MOVEMENT_TYPE_WANDER_UP_AND_DOWN
	setobjectxyperm LOCALID_RIVAL, 5, 10
	end

LittlerootTown_ProfessorBirchsLab_EventScript_SetReadyForDexReward:: @ 81F9CE9
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 4
	goto LittlerootTown_ProfessorBirchsLab_EventScript_SetDexRewardLayout

LittlerootTown_ProfessorBirchsLab_EventScript_SetDexRewardLayout:: @ 81F9CF3
	setmaplayoutindex LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB_WITH_TABLE
	end

LittlerootTown_ProfessorBirchsLab_EventScript_SetAfterDexRewardLayout:: @ 81F9CF7
	setmaplayoutindex LAYOUT_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB_WITH_TABLE
	setobjectmovementtype LOCALID_RIVAL, MOVEMENT_TYPE_WANDER_UP_AND_DOWN
	setobjectxyperm LOCALID_RIVAL, 5, 10
	end

LittlerootTown_ProfessorBirchsLab_OnWarp: @ 81F9D06
	map_script_2 VAR_BIRCH_LAB_STATE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_SetPlayerPosForReceiveStarter
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexUpgrade
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexUpgrade
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 7, LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexUpgrade
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_AddRivalObject
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6, LittlerootTown_ProfessorBirchsLab_EventScript_AddRivalObject
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexReward
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 5, LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexReward
	.2byte 0

LittlerootTown_ProfessorBirchsLab_EventScript_SetPlayerPosForReceiveStarter:: @ 81F9D40
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	end

LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexUpgrade:: @ 81F9D45
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	addobject LOCALID_BIRCH
	addobject LOCALID_RIVAL
	turnobject LOCALID_RIVAL, DIR_NORTH
	setobjectxy LOCALID_RIVAL, 5, 5
	turnobject LOCALID_BIRCH, DIR_SOUTH
	setobjectxy LOCALID_BIRCH, 6, 4
	turnobject LOCALID_AIDE, DIR_EAST
	setobjectxy LOCALID_AIDE, 10, 10
	end

LittlerootTown_ProfessorBirchsLab_EventScript_AddRivalObject:: @ 81F9D71
	addobject LOCALID_RIVAL
	end

LittlerootTown_ProfessorBirchsLab_EventScript_SetObjectPosForDexReward::
	addobject LOCALID_RIVAL
	turnobject LOCALID_BIRCH, DIR_SOUTH
	setobjectxy LOCALID_BIRCH, 6, 4
	turnobject LOCALID_RIVAL, DIR_EAST
	setobjectxy LOCALID_RIVAL, 5, 5
	end

LittlerootTown_ProfessorBirchsLab_OnFrame: @ 81F9DB9
	map_script_2 VAR_BIRCH_LAB_STATE, 2, LittlerootTown_ProfessorBirchsLab_EventScript_GiveStarterEvent
	map_script_2 VAR_BIRCH_LAB_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedexEvent
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, LittlerootTown_ProfessorBirchsLab_EventScript_UpgradeToNationalDex
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_GiveDexRewardEvent
	.2byte 0

@ Both partners were granted atomically on Route 101. Confirm ownership and
@ offer a nickname for each without granting either Pokemon again.
LittlerootTown_ProfessorBirchsLab_EventScript_GiveStarterEvent::
	lockall
	bufferpartymonnick 0, 0
	bufferpartymonnick 1, 1
	message LittlerootTown_ProfessorBirchsLab_Text_LikeYouToHavePokemon
	waitmessage
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	setvar VAR_0x8004, 0
	call LittlerootTown_ProfessorBirchsLab_EventScript_OfferStarterNickname
	setvar VAR_0x8004, 1
	call LittlerootTown_ProfessorBirchsLab_EventScript_OfferStarterNickname
	goto LittlerootTown_ProfessorBirchsLab_EventScript_GoSeeRival

LittlerootTown_ProfessorBirchsLab_EventScript_OfferStarterNickname::
	bufferpartymonnick 0, VAR_0x8004
	msgbox LittlerootTown_ProfessorBirchsLab_Text_WhyNotGiveNicknameToMon, MSGBOX_YESNO
	call_if_eq VAR_RESULT, YES, Common_EventScript_NameReceivedPartyMon
	return

LittlerootTown_ProfessorBirchsLab_EventScript_GoSeeRival:: @ 81F9E17
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MightBeGoodIdeaToGoSeeRival, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_AgreeToSeeRival
	compare VAR_RESULT, NO
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_DeclineSeeingRival
	end

LittlerootTown_ProfessorBirchsLab_EventScript_AgreeToSeeRival:: @ 81F9E36
	msgbox LittlerootTown_ProfessorBirchsLab_Text_GetRivalToTeachYou, MSGBOX_DEFAULT
	clearflag FLAG_HIDE_ROUTE_101_BOY
	setvar VAR_BIRCH_LAB_STATE, 3
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_DeclineSeeingRival:: @ 81F9E48
	msgbox LittlerootTown_ProfessorBirchsLab_Text_DontBeThatWay, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_AgreeToSeeRival
	compare VAR_RESULT, NO
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_DeclineSeeingRival
	end

LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedexEvent:: @ 81F9E67
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, LittlerootTown_ProfessorBirchsLab_Movement_PlayerEnterLabForPokedex
	waitmovement 0
	goto LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex
	end

LittlerootTown_ProfessorBirchsLab_Movement_PlayerEnterLabForPokedex: @ 81F9E78
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
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OtherDimensionsTakeCosmog, MSGBOX_DEFAULT
	closemessage
	@ A retry (state 7) is a face-to-face talk: the fetch walk would step
	@ into a player standing on Birch's left, so he hands it over in place.
	goto_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 7, LittlerootTown_ProfessorBirchsLab_EventScript_GiveCosmog
	applymovement LOCALID_BIRCH, LittlerootTown_ProfessorBirchsLab_Movement_BirchGiveCosmogs
	waitmovement 0
LittlerootTown_ProfessorBirchsLab_EventScript_GiveCosmog:
	callnative GiveBirchCosmogPair
	goto_if_eq VAR_RESULT, FALSE, LittlerootTown_ProfessorBirchsLab_EventScript_CosmogNoRoom
	call LittlerootTown_ProfessorBirchsLab_EventScript_ReceivedCosmogFanfare
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CosmogPairStorage, MSGBOX_DEFAULT
	goto LittlerootTown_ProfessorBirchsLab_EventScript_RivalCosmog

LittlerootTown_ProfessorBirchsLab_EventScript_ReceivedCosmogFanfare:: @ 82700DB
	playfanfare MUS_OBTAIN_ITEM
	message LittlerootTown_ProfessorBirchsLab_Text_ReceivedCosmog
	waitmessage
	waitfanfare
	bufferspeciesname 0, SPECIES_COSMOG
	return

LittlerootTown_ProfessorBirchsLab_EventScript_RivalCosmog::
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_MayUpgradeComment
	compare VAR_RESULT, FEMALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_BrendanUpgradeComment
	msgbox LittlerootTown_ProfessorBirchsLab_Text_GrassyPatchWaiting2, MSGBOX_DEFAULT
	closemessage
	@ A retry can leave the player anywhere beside the rival, not only on her right.
	facetogether OBJ_EVENT_ID_PLAYER, LOCALID_RIVAL
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_MayGoRoute103
	compare VAR_RESULT, FEMALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_BrendanGoRoute103
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestDown
	applymovement LOCALID_RIVAL, LittlerootTown_ProfessorBirchsLab_Movement_RivalLeaves
	waitmovement 0
	removeobject LOCALID_RIVAL
	clearflag FLAG_HIDE_ROUTE_103_RIVAL
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCH
	setflag FLAG_HIDE_LITTLEROOT_TOWN_RIVAL
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2
	setvar VAR_SCOTT_BF_CALL_STEP_COUNTER, 0
	setflag FLAG_SCOTT_CALL_BATTLE_FRONTIER
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayUpgradeComment:: @ 81F9F32
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayUpgradeSoCool, MSGBOX_DEFAULT
	closemessage
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanUpgradeComment:: @ 81F9F3C
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanYouCanThankMe, MSGBOX_DEFAULT
	closemessage
	return

LittlerootTown_ProfessorBirchsLab_EventScript_GiveDexRewardEvent:: @ 81F9F62
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, LittlerootTown_ProfessorBirchsLab_Movement_PlayerEnterLabForDexReward
	waitmovement 0
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CompletedDexReward, MSGBOX_DEFAULT
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 5
	goto LittlerootTown_ProfessorBirchsLab_EventScript_GiveDexReward

LittlerootTown_ProfessorBirchsLab_Movement_RivalLeaves:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

LittlerootTown_ProfessorBirchsLab_Movement_BirchGiveCosmogs:
	walk_left
	walk_in_place_fast_down
	delay_16
	delay_16
	walk_right
	walk_in_place_fast_down
	delay_16
	delay_16
	step_end

LittlerootTown_ProfessorBirchsLab_Movement_PlayerEnterLabForDexReward: @ 81F9F7C
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

LittlerootTown_ProfessorBirchsLab_EventScript_Aide:: @ 81F9F84
	lock
	faceplayer
	compare VAR_BIRCH_LAB_STATE, 3
	goto_if_ge LittlerootTown_ProfessorBirchsLab_EventScript_AideReceivedStarter
	goto_if_set FLAG_BIRCH_AIDE_MET, LittlerootTown_ProfessorBirchsLab_EventScript_AideAlreadyMet
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BirchAwayOnFieldwork, MSGBOX_DEFAULT
	setflag FLAG_BIRCH_AIDE_MET
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_AideAlreadyMet:: @ 81F9FA7
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BirchIsntOneForDeskWork, MSGBOX_DEFAULT
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_AideReceivedStarter:: @ 81F9FB1
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BirchEnjoysRivalsHelpToo, MSGBOX_DEFAULT
	release
	end

@ Retained map-script labels for the retired display objects.
LittlerootTown_ProfessorBirchsLab_EventScript_Cyndaquil::
LittlerootTown_ProfessorBirchsLab_EventScript_Totodile::
LittlerootTown_ProfessorBirchsLab_EventScript_Chikorita::
	lockall
	goto LittlerootTown_ProfessorBirchsLab_EventScript_GiveDexReward

LittlerootTown_ProfessorBirchsLab_EventScript_GiveDexReward::
	goto_if_ge VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6, LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardDone
	goto_if_lt VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardDone
LittlerootTown_ProfessorBirchsLab_EventScript_DeliverDexReward::
	@ AddBagItem preflights the whole quantity; failure changes no items.
	additem ITEM_GOLD_BOTTLE_CAP, PARTY_SIZE
	goto_if_eq VAR_RESULT, FALSE, LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardBagFull
	@ Record delivery before the presentation so retries cannot duplicate it.
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6
LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardReceived::
	setvar VAR_0x8000, ITEM_GOLD_BOTTLE_CAP
	setvar VAR_0x8001, PARTY_SIZE
	setvar VAR_0x8007, TRUE
	call EventScript_ObtainItemMessage
	msgbox LittlerootTown_ProfessorBirchsLab_Text_DexRewardUses, MSGBOX_DEFAULT
LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardDone::
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardBagFull::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_DexRewardBagFull, MSGBOX_DEFAULT
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_EventScript_Birch:: @ 81FA25A
	lock
	faceplayer
	goto_if_eq VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 7, LittlerootTown_ProfessorBirchsLab_EventScript_UpgradeToNationalDex
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 5
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_PendingDexReward
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_GrassyPatchWaiting
	goto LittlerootTown_ProfessorBirchsLab_EventScript_TryRatePokedexOrRegister
	end

LittlerootTown_ProfessorBirchsLab_EventScript_PendingDexReward::
	goto LittlerootTown_ProfessorBirchsLab_EventScript_GiveDexReward

LittlerootTown_ProfessorBirchsLab_EventScript_GrassyPatchWaiting:: @ 81FA294
	msgbox LittlerootTown_ProfessorBirchsLab_Text_GrassyPatchWaiting2, MSGBOX_DEFAULT
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_TryRatePokedexOrRegister:: @ 81FA29E
	goto_if_unset FLAG_RECEIVED_DEXNAV, LittlerootTown_ProfessorBirchsLab_EventScript_NormalVisit
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OfferDexNavRefresher, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, LittlerootTown_ProfessorBirchsLab_EventScript_NormalVisit
	call LittlerootTown_ProfessorBirchsLab_EventScript_ExplainDexNav
	release
	end
LittlerootTown_ProfessorBirchsLab_EventScript_NormalVisit:
	goto_if_unset FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_UNKNOWN_0x380, ProfBirch_EventScript_RatePokedexOrRegister
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3
	goto_if_eq ProfBirch_EventScript_RatePokedexOrRegister
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6
	goto_if_ge ProfBirch_EventScript_RatePokedexOrRegister
	compare VAR_BIRCH_LAB_STATE, 5
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_PokemonAwait
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BirchRivalGoneHome, MSGBOX_DEFAULT
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex:: @ 81FA2F8
	msgbox LittlerootTown_ProfessorBirchsLab_Text_HeardYouBeatRivalTakePokedex, MSGBOX_DEFAULT
	call LittlerootTown_ProfessorBirchsLab_EventScript_ReceivePokedex
	msgbox LittlerootTown_ProfessorBirchsLab_Text_ExplainPokedex, MSGBOX_DEFAULT
	msgbox LittlerootTown_ProfessorBirchsLab_Text_DexNavBrief, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_RIVAL, LittlerootTown_ProfessorBirchsLab_Movement_RivalApproachPlayer
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_MayGivePokeBalls
	compare VAR_RESULT, FEMALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_BrendanGivePokeBalls
	msgbox LittlerootTown_ProfessorBirchsLab_Text_RivalDexNavInvite, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_RIVAL, LittlerootTown_ProfessorBirchsLab_Movement_RivalLeaveForDemo
	waitmovement 0
	removeobject LOCALID_RIVAL
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL
	setvar VAR_BIRCH_LAB_STATE, 5
	setflag FLAG_ADVENTURE_STARTED
	setvar VAR_OLDALE_TOWN_STATE, 1
	setvar VAR_LITTLEROOT_RIVAL_STATE, 5 @ Pending Route 101 demonstration; preserves Mom's normal exit event.
	setvar VAR_LITTLEROOT_TOWN_STATE, 3
	releaseall
	end

@ Keep each section below the message buffer limit; this lesson runs only
@ inside the one-time handoff. Repeat visits retain their normal dialogue.
LittlerootTown_ProfessorBirchsLab_EventScript_ExplainDexNav::
	msgbox LittlerootTown_ProfessorBirchsLab_Text_ExplainDexNav, MSGBOX_DEFAULT
	msgbox LittlerootTown_ProfessorBirchsLab_Text_DexNavSearching, MSGBOX_DEFAULT
	msgbox LittlerootTown_ProfessorBirchsLab_Text_DexNavMethods, MSGBOX_DEFAULT
	msgbox LittlerootTown_ProfessorBirchsLab_Text_DexNavChains, MSGBOX_DEFAULT
	msgbox LittlerootTown_ProfessorBirchsLab_Text_DexNavPokerus, MSGBOX_DEFAULT
	msgbox LittlerootTown_ProfessorBirchsLab_Text_DexNavSpread, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_MayGivePokeBalls:: @ 81FA352
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayGotPokedexTooTakeThese, MSGBOX_DEFAULT
	giveitem ITEM_CHERISH_BALL, 30
	compare VAR_RESULT, FALSE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_MayNoRoomForPokeBalls
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CatchCutePokemonWithPokeBalls, MSGBOX_DEFAULT
	setvar VAR_RESULT, 0
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanGivePokeBalls:: @ 81FA37F
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanGotPokedexTooTakeThese, MSGBOX_DEFAULT
	giveitem ITEM_CHERISH_BALL, 30
	compare VAR_RESULT, FALSE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_BrendanNoRoomForPokeBalls
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CatchCoolPokemonWithPokeBalls, MSGBOX_DEFAULT
	setvar VAR_RESULT, 1
	return

LittlerootTown_ProfessorBirchsLab_EventScript_ReceivePokedex:: @ 81FA3AC
	playfanfare MUS_OBTAIN_ITEM
	message LittlerootTown_ProfessorBirchsLab_Text_ReceivedPokedex
	waitfanfare
	setflag FLAG_SYS_POKEDEX_GET
	setflag FLAG_RECEIVED_DEXNAV
	setvar VAR_DEXNAV_SPECIES, SPECIES_NONE
	special SetUnlockedPokedexFlags
	setflag FLAG_RECEIVED_POKEDEX_FROM_BIRCH
	setflag FLAG_SYS_NATIONAL_DEX
	special EnableNationalPokedex
	setvar VAR_CABLE_CLUB_TUTORIAL_STATE, 1
	return

LittlerootTown_ProfessorBirchsLab_EventScript_PokemonAwait:: @ 81FA3C4
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CountlessPokemonAwait, MSGBOX_DEFAULT
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayNoRoomForPokeBalls:: @ 81FA3CE
	msgbox LittlerootTown_ProfessorBirchsLab_Text_OhYourBagsFull, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanNoRoomForPokeBalls:: @ 81FA3D7
	msgbox LittlerootTown_ProfessorBirchsLab_Text_HeyYourBagsFull, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_Movement_RivalApproachPlayer: @ 81FA3E0
	walk_down
	walk_in_place_fast_left
	step_end

LittlerootTown_ProfessorBirchsLab_Movement_RivalReturnBesideBirch:
	walk_up
	face_down
	step_end

LittlerootTown_ProfessorBirchsLab_EventScript_Machine:: @ 81FA3E3
	msgbox LittlerootTown_ProfessorBirchsLab_Text_SeriousLookingMachine, MSGBOX_SIGN
	end

LittlerootTown_ProfessorBirchsLab_EventScript_Rival:: @ 81FA3EC
	lock
	faceplayer
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 5
	goto_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_RivalFuturePlans
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6
	goto_if_ge LittlerootTown_ProfessorBirchsLab_EventScript_RivalHaveYouGoneToBattleFrontier
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2
	goto_if_ge LittlerootTown_ProfessorBirchsLab_EventScript_RivalTakeBreakFromFieldwork
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_MayWhereShouldIGoNext
	compare VAR_RESULT, FEMALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_BrendanWhereShouldIGoNext
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayWhereShouldIGoNext:: @ 81FA428
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayWhereShouldIGoNext, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanWhereShouldIGoNext:: @ 81FA431
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanWhereShouldIGoNext, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_RivalFuturePlans:: @ 81FA43A
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_MayWhatNextImStayingHere
	compare VAR_RESULT, FEMALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_BrendanPreferCollectingSlowly
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayWhatNextImStayingHere:: @ 81FA453
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayWhatNextImStayingHere, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanPreferCollectingSlowly: @ 81FA45C
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanPreferCollectingSlowly, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_RivalHaveYouGoneToBattleFrontier:: @ 81FA465
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_MayHaveYouGoneToBattleFrontier
	compare VAR_RESULT, FEMALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_BrendanHaveYouGoneToBattleFrontier
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayHaveYouGoneToBattleFrontier:: @ 81FA47E
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayHaveYouGoneToBattleFrontier, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanHaveYouGoneToBattleFrontier:: @ 81FA487
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanHaveYouGoneToBattleFrontier, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_RivalTakeBreakFromFieldwork::
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_MayGoRoute103
	compare VAR_RESULT, FEMALE
	call_if_eq LittlerootTown_ProfessorBirchsLab_EventScript_BrendanGoRoute103
	release
	end

LittlerootTown_ProfessorBirchsLab_EventScript_MayGoRoute103:: @ 81FA4A9
	msgbox LittlerootTown_ProfessorBirchsLab_Text_MayGoRoute103, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_BrendanGoRoute103:: @ 81FA4B2
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BrendanGoRoute103, MSGBOX_DEFAULT
	return

LittlerootTown_ProfessorBirchsLab_EventScript_PC:: @ 81FA4BB
	msgbox LittlerootTown_ProfessorBirchsLab_Text_PCUsedForResearch, MSGBOX_SIGN
	end

LittlerootTown_ProfessorBirchsLab_EventScript_Bookshelf:: @ 81FA4C4
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CrammedWithBooksOnPokemon, MSGBOX_SIGN
	end

LittlerootTown_ProfessorBirchsLab_EventScript_Book:: @ 81FA4CD
	msgbox LittlerootTown_ProfessorBirchsLab_Text_BookTooHardToRead, MSGBOX_SIGN
	end

LittlerootTown_ProfessorBirchsLab_EventScript_ScottAboardSSTidalCall:: @ 81FA4D6
	lockall
	pokenavcall LittlerootTown_ProfessorBirchsLab_Text_ScottAboardSSTidalCall
	waitmessage
	clearflag FLAG_SCOTT_CALL_BATTLE_FRONTIER
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_Text_BirchAwayOnFieldwork: @ 81FA4E2
	.string "Hunh? Prof. Birch?\p"
	.string "The Prof's away on fieldwork.\n"
	.string "Ergo, he isn't here.\p"
	.string "Oh, let me explain what fieldwork is.\p"
	.string "It is to study things in the natural\n"
	.string "environment, like fields and mountains,\l"
	.string "instead of a laboratory.\p"
	.string "The Prof isn't one for doing desk work.\n"
	.string "He's the type of person who would\p"
	.string "rather go outside and experience\n"
	.string "things than read about them here.$"

LittlerootTown_ProfessorBirchsLab_Text_BirchIsntOneForDeskWork: @ 81FA641
	.string "The Prof isn't one for doing desk work.\n"
	.string "He's the type of person who would\p"
	.string "rather go outside and experience\n"
	.string "things than read about them here.$"

LittlerootTown_ProfessorBirchsLab_Text_BirchEnjoysRivalsHelpToo: @ 81FA6CE
	.string "Prof. Birch is studying the habitats\n"
	.string "and distribution of Pokémon.\p"
	.string "The Prof enjoys {RIVAL}'s help, too.\n"
	.string "There's a lot of love there.$"

LittlerootTown_ProfessorBirchsLab_Text_LikeYouToHavePokemon: @ 81FA74D
	.string "Prof. Birch: So, {PLAYER}{KUN}.\p"
	.string "I've heard so much about you from\n"
	.string "your father.\p"
	.string "I've heard that you don't have your\n"
	.string "own Pokémon yet.\p"
	.string "But the way you battled earlier,\n"
	.string "you pulled it off with aplomb!\p"
	.string "I guess you have your father's blood\n"
	.string "in your veins after all!\p"
	.string "Oh, yes. As thanks for rescuing me,\n"
	.string "I'd like you to keep both Pokémon\l"
	.string "that helped you rescue me.\p"
	.string "{STR_VAR_1} and {STR_VAR_2}\n"
	.string "are now your partners!$"

LittlerootTown_ProfessorBirchsLab_Text_WhyNotGiveNicknameToMon: @ 81FA8B1
	.string "Prof. Birch: While you're at it, why not\n"
	.string "give a nickname to that {STR_VAR_1}?$"

LittlerootTown_ProfessorBirchsLab_Text_MightBeGoodIdeaToGoSeeRival: @ 81FA8F6
	.string "Prof. Birch: Keep working with your\n"
	.string "partners, and I think you'll make\l"
	.string "an extremely good Trainer.\p"
	.string "My kid, {RIVAL}, is also studying\n"
	.string "Pokémon while helping me out.\p"
	.string "{PLAYER}{KUN}, don't you think it might be\n"
	.string "a good idea to go see {RIVAL}?$"

LittlerootTown_ProfessorBirchsLab_Text_GetRivalToTeachYou: @ 81FA9D5
	.string "Prof. Birch: Great! {RIVAL} is on\n"
	.string "Route 103, north of Oldale Town.\p"
	.string "Stop at Oldale's Pokémon Center\n"
	.string "first. The nurse there has tools\l"
	.string "for every new Trainer.\p"
	.string "Then see the Move Tutor and the\n"
	.string "clerk. The clerk sells Balls, so\l"
	.string "catch more partners on the way!$"

LittlerootTown_ProfessorBirchsLab_Text_DontBeThatWay: @ 81FAA35
	.string "Prof. Birch: Oh, don't be that way.\n"
	.string "You should go meet my kid.$"

LittlerootTown_ProfessorBirchsLab_Text_BirchRivalGoneHome: @ 81FAA74
	.string "Prof. Birch: Look for {RIVAL} on\n"
	.string "Route 103, north of Oldale Town.\p"
	.string "See the nurse at Oldale's Pokémon\n"
	.string "Center first. Then the Move Tutor\l"
	.string "and the clerk will help you prepare.$"

LittlerootTown_ProfessorBirchsLab_Text_HeardYouBeatRivalTakePokedex: @ 81FAB22
	.string "Prof. Birch: Oh, hi, {PLAYER}{KUN}!\p"
	.string "I heard you beat {RIVAL} on\n"
	.string "Route 103. That's excellent!\p"
	.string "{RIVAL}'s been helping with my research\n"
	.string "for a long time.\p"
	.string "{RIVAL} has an extensive history as\n"
	.string "a Trainer already.\p"
	.string "Here, {PLAYER}{KUN}, I ordered this for my\n"
	.string "research, but I think you should have\l"
	.string "this Pokédex.$"

LittlerootTown_ProfessorBirchsLab_Text_ReceivedPokedex: @ 81FAC32
	.string "{PLAYER} received the National Pokédex!$"

LittlerootTown_ProfessorBirchsLab_Text_ExplainPokedex: @ 81FAC4B
	.string "Prof. Birch: The Pokédex is a high-tech\n"
	.string "tool that automatically makes a record\l"
	.string "of any Pokémon you meet or catch.\p"
	.string "My kid, {RIVAL}, goes everywhere\n"
	.string "with it.\p"
	.string "Whenever my kid catches a rare Pokémon\n"
	.string "and records its data in the Pokédex,\p"
	.string "why, {RIVAL} looks for me while I'm out\n"
	.string "doing fieldwork, and shows me.$"

LittlerootTown_ProfessorBirchsLab_Text_ExplainDexNav:
	.string "Prof. Birch: I built a DexNav into\n"
	.string "your Pokédex. Let me show you!\p"
	.string "Open DexNav from the Start menu\n"
	.string "to see what lives where you stand.\p"
	.string "Every available Pokémon has an icon.\n"
	.string "You needn't see or catch it first!\p"
	.string "As you travel and unlock encounters,\n"
	.string "the list changes to match the area.\p"
	.string "Rods, Honey and field licenses\n"
	.string "open more encounter methods.\p"
	.string "Scroll down to see each method:\n"
	.string "grass, water, fishing and more.\p"
	.string "Choose an icon to see its rarity,\n"
	.string "level range and how to encounter it.$"

LittlerootTown_ProfessorBirchsLab_Text_DexNavSearching:
	.string "If the panel says A Search, press A\n"
	.string "to track that Pokémon nearby.\p"
	.string "Grass, cave floors and Surf water\n"
	.string "can provide searchable encounters.\p"
	.string "Your last search is remembered.\n"
	.string "R in the field repeats that search.\p"
	.string "An item assigned to R takes priority.\n"
	.string "Use the DexNav menu in that case.\p"
	.string "Follow the arrow to the moving spot.\n"
	.string "Step onto it to start the battle!\p"
	.string "Hold A while using the directions.\n"
	.string "That makes you sneak toward it.\p"
	.string "Don't run or cycle near the spot.\n"
	.string "Even walking scares it up close!\p"
	.string "You have 15 seconds to reach it.\n"
	.string "Watch the countdown: it turns red\l"
	.string "in the last five seconds.\p"
	.string "Take too long and it gets away,\n"
	.string "ending your chain. Stay in range!\p"
	.string "In caves or water, it may move twice.\n"
	.string "Follow the arrow to its new spot.\p"
	.string "If nothing is found nearby, move\n"
	.string "closer to its habitat and try again.$"

LittlerootTown_ProfessorBirchsLab_Text_DexNavMethods:
	.string "Cut trees, Rock Smash, Honey and\n"
	.string "fishing keep their own methods.\p"
	.string "Those entries cannot be searched.\n"
	.string "Read their instructions instead.\p"
	.string "Storm visitors appear in the list,\n"
	.string "but cannot be searched with DexNav.\p"
	.string "Meet those guests through ordinary\n"
	.string "wild encounters during their storm.\p"
	.string "Their unusual weather may bring\n"
	.string "snow, too. Check Institute reports!\p"
	.string "Other available resident legends\n"
	.string "can be searched where permitted.\p"
	.string "Ordinary wild encounters still use\n"
	.string "the area's normal encounter chances.$"

LittlerootTown_ProfessorBirchsLab_Text_DexNavChains:
	.string "Now for the exciting part: chains!\n"
	.string "Catch or defeat a searched Pokémon.\p"
	.string "That adds one to your chain. Search\n"
	.string "again here to keep building it.\p"
	.string "You can change species. The chain\n"
	.string "counts searched battles, not species.\p"
	.string "Running, losing, scaring a target\n"
	.string "or losing its signal breaks the chain.\p"
	.string "Other battles and changing areas\n"
	.string "also end it. Reloading resets it too.\p"
	.string "A chain raises shiny and Pokérus\n"
	.string "chances, not levels or other rewards.\p"
	.string "The shiny percentage equals your\n"
	.string "chain. Pokérus gets half that chance.\p"
	.string "At chain 50, shiny chance is 50%.\n"
	.string "Pokérus chance is half that: 25%.\p"
	.string "The chain stops rising at 100.\n"
	.string "That's 100% shiny and 50% Pokérus!\p"
	.string "Each is rolled separately, so your\n"
	.string "find can be both shiny and infected.\p"
	.string "These odds apply to your searches,\n"
	.string "not to ordinary wild encounters.\p"
	.string "Before a chain, both are very rare.\n"
	.string "DexNav shows your current odds.$"

LittlerootTown_ProfessorBirchsLab_Text_DexNavPokerus:
	.string "Shininess changes a Pokémon's look.\n"
	.string "Pokérus enhances its nature instead.\p"
	.string "Pokérus adds five percentage points:\n"
	.string "+10% becomes +15%.\p"
	.string "Normally, the penalty stays -10%.\n"
	.string "Neutral natures gain no bonus.\p"
	.string "That benefit lasts after recovery\n"
	.string "and follows any future nature change.\p"
	.string "On the Summary's Skills page, the\n"
	.string "enhanced nature stat is colored gold.\p"
	.string "PKRS marks a contagious Pokémon.\n"
	.string "A tiny dot marks a recovered one.\p"
	.string "Lavaridge's hot spring offers a\n"
	.string "treatment for any past infection.\l"
	.string "The bathers there can explain it!$"

LittlerootTown_ProfessorBirchsLab_Text_DexNavSpread:
	.string "An original carrier can infect just\n"
	.string "two other Pokémon in its lifetime.\p"
	.string "Put an uninfected Pokémon beside\n"
	.string "the carrier in your party and battle.\p"
	.string "After battle, there's about a\n"
	.string "one-in-three chance to spread it.\p"
	.string "One neighbor can be infected per\n"
	.string "battle. It uses one transmission.\p"
	.string "Only successful spread uses one.\n"
	.string "Eggs and infected Pokémon can't get it.\p"
	.string "Recipients keep the nature benefit,\n"
	.string "but cannot pass Pokérus on.\p"
	.string "After two spreads, the original\n"
	.string "carrier keeps its benefit as well.\p"
	.string "There's no calendar deadline, and\n"
	.string "PC storage keeps its progress.\p"
	.string "Keep moving, sneak up gently,\n"
	.string "and build a chain. Happy fieldwork!$"

LittlerootTown_ProfessorBirchsLab_Text_CountlessPokemonAwait: @ 81FAD6F
	.string "Prof. Birch: Countless Pokémon\n"
	.string "await you!\p"
	.string "Argh, I'm getting the itch to get out\n"
	.string "and do fieldwork again!$"

LittlerootTown_ProfessorBirchsLab_Text_MayGotPokedexTooTakeThese: @ 81FADD7
	.string "May: Oh, wow, {PLAYER}{KUN}!\n"
	.string "You got a Pokédex, too!\p"
	.string "That's great! Just like me!\n"
	.string "I've got something for you, too!$"

LittlerootTown_ProfessorBirchsLab_Text_CatchCutePokemonWithPokeBalls: @ 81FAE40
	.string "May: It's fun if you can get a lot of\n"
	.string "Pokémon!\p"
	.string "I'm going to look all over the place\n"
	.string "because I want different Pokémon.\p"
	.string "If I find any cute Pokémon, I'll catch\n"
	.string "them with Poké Balls!$"

LittlerootTown_ProfessorBirchsLab_Text_OhYourBagsFull: @ 81FAEF3
	.string "Oh? Your Bag's full.$"

LittlerootTown_ProfessorBirchsLab_Text_MayWhereShouldIGoNext: @ 81FAF08
	.string "May: I wonder where I should go look\n"
	.string "for Pokémon next?$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanGotPokedexTooTakeThese: @ 81FAF3F
	.string "Brendan: Huh…\n"
	.string "So you got a Pokédex, too.\p"
	.string "Well then, here.\n"
	.string "I'll give you these.$"

LittlerootTown_ProfessorBirchsLab_Text_CatchCoolPokemonWithPokeBalls: @ 81FAF8E
	.string "Brendan: You know it's more fun to\n"
	.string "have a whole bunch of Pokémon.\p"
	.string "I'm going to explore all over the place\n"
	.string "to find different Pokémon.\p"
	.string "If I find any cool Pokémon, you bet\n"
	.string "I'll try to get them with Poké Balls.$"

LittlerootTown_ProfessorBirchsLab_Text_HeyYourBagsFull: @ 81FB05D
	.string "Hey, your Bag's full.$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanWhereShouldIGoNext: @ 81FB073
	.string "Brendan: Where should I look for\n"
	.string "Pokémon next…$"

LittlerootTown_ProfessorBirchsLab_Text_SeriousLookingMachine: @ 81FB0A2
	.string "It's a serious-looking machine.\n"
	.string "The Prof must use this for research.$"

LittlerootTown_ProfessorBirchsLab_Text_PCUsedForResearch: @ 81FB0E7
	.string "It's a PC used for research.\n"
	.string "Better not mess around with it.$"

LittlerootTown_ProfessorBirchsLab_Text_CrammedWithBooksOnPokemon: @ 81FB124
	.string "It's crammed with books on Pokémon.$"

LittlerootTown_ProfessorBirchsLab_Text_BookTooHardToRead: @ 81FB148
	.string "It's a book that's too hard to read.$"

LittlerootTown_ProfessorBirchsLab_Text_OtherDimensionsTakeCosmog:
	.string "Prof. Birch: Now…\p"
	.string "{PLAYER}{KUN} and {RIVAL}, I've had the two\n"
	.string "of you help me study Pokémon.\p"
	.string "Thanks to your help, new facts\n"
	.string "are coming to light.\p"
	.string "It appears that in the Hoenn region,\n"
	.string "there are also Pokémon from other\l"
	.string "dimensions!\p"
	.string "I've been raising two Cosmog.\n"
	.string "I'd like you to have both!\p"
	.string "They seem weak now, but their\n"
	.string "evolutions follow the sun and moon.\p"
	.string "You can raise one of each!$"

LittlerootTown_ProfessorBirchsLab_Text_MayUpgradeSoCool: @ 81FB30F
	.string "May: Eheheh!\p"
	.string "It's so cute! I wonder if it evolves?\n"
	.string "I guess we'll find out,\l"
	.string "eh {PLAYER}{KUN}?$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanYouCanThankMe: @ 81FB38E
	.string "Brendan: Huh, so it's like a space…\n"
	.string "cloud… Magikarp…?\p"
	.string "Guess we'll see what it can do,\n"
	.string "huh, {PLAYER}?$"

LittlerootTown_ProfessorBirchsLab_Text_GrassyPatchWaiting2: @ 81FB466
	.string "Prof. Birch: But listen.\n"
	.string "You've become the Champion,\l"
	.string "but your Pokémon journey isn't over.\p"
	.string "There is no end to the road\n"
	.string "that is Pokémon.\p"
	.string "Somewhere, there is a grassy patch\n"
	.string "that's waiting for you!$"

LittlerootTown_ProfessorBirchsLab_Text_MayGoRoute103: @ 81FB528
	.string "May: I'm going to Route 103 to do some\n"
	.string "fieldwork for a while.\p"
	.string "I'll see you out there, {PLAYER}{KUN}!$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanGoRoute103: @ 81FB58A
	.string "Brendan: Right, I'm headed out to\n"
	.string "Route 103 for a while.\p"
	.string "Maybe I'll see you there, {PLAYER}?$"

LittlerootTown_ProfessorBirchsLab_Text_CompletedDexReward:
	.string "Prof. Birch: Oh, {PLAYER}{KUN}!\n"
	.string "You've completed the Hoenn Pokédex!\p"
	.string "Your fieldwork deserves something\n"
	.string "special: six Gold Bottle Caps.\p"
	.string "One for each partner on your team!$"

LittlerootTown_ProfessorBirchsLab_Text_DexRewardUses:
	.string "Prof. Birch: A Gold Bottle Cap pays\n"
	.string "for an iconic move lesson at a Center.\p"
	.string "Teach your partners something special\n"
	.string "for the challenges still ahead!$"

LittlerootTown_ProfessorBirchsLab_Text_DexRewardBagFull:
	.string "Prof. Birch: Make room in your Bag\n"
	.string "for all six Gold Bottle Caps.\p"
	.string "I'll keep them safe. Come back and\n"
	.string "talk to me when you're ready!$"

LittlerootTown_ProfessorBirchsLab_Text_ReceivedCosmog:
	.string "{PLAYER} received two Cosmog\n"
	.string "from Prof. Birch!$"

@ Unused
LittlerootTown_ProfessorBirchsLab_Text_MayWhatNextImStayingHere: @ 81FBB68
	.string "May: {PLAYER}{KUN}, after this…\n"
	.string "What are you going to do?\p"
	.string "Are you going to keep battling\n"
	.string "and sharpening your skills?\p"
	.string "Or are you going to try filling\n"
	.string "the National Pokédex?\p"
	.string "I'm staying here to help the Prof.$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanPreferCollectingSlowly: @ 81FBC2D
	.string "Brendan: Rather than collecting\n"
	.string "Pokémon, I prefer slowly and\l"
	.string "steadily raising the one I chose.$"

LittlerootTown_ProfessorBirchsLab_Text_MayHaveYouGoneToBattleFrontier: @ 81FBC8D
	.string "May: Oh, hi, {PLAYER}{KUN}!\n"
	.string "Have you gone to that place,\l"
	.string "the Battle Frontier?$"

LittlerootTown_ProfessorBirchsLab_Text_BrendanHaveYouGoneToBattleFrontier: @ 81FBCD2
	.string "Brendan: Hey, {PLAYER}!\n"
	.string "Have you gone out to that place,\l"
	.string "the Battle Frontier?$"

LittlerootTown_ProfessorBirchsLab_Text_ScottAboardSSTidalCall: @ 81FBD1A
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "Scott: Hi, hi, {PLAYER}!\n"
	.string "It's me, Scott.\p"
	.string "I'm aboard the S.S. Tidal now.\n"
	.string "It feels great to be on the sea!\p"
	.string "By the way…\p"
	.string "There's this place that I'd like to\n"
	.string "invite you to for a visit.\p"
	.string "If you're interested, board a ferry\n"
	.string "at either Slateport or Lilycove.\p"
	.string "I'll fill you in on the details when\n"
	.string "we meet. I'll be waiting!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

@ Emerald Champions: the Rotom appliance/form-change service is not assembled yet.
@ data/scripts/rotom.inc exists but is not .include'd, and restoring it needs six C
@ specials ported from the donor (CountRotomInParty, GetRotomState, TeachRotomMove,
@ RotomForgetSpecialMove, DoesRotomKnowSpecialMove, GetRotomNewSpecialMove) plus the
@ MULTI_ROTOM_APPLIANCES menu and its six strings. map.json points an object here, so
@ this inert stand-in keeps the link resolved until that work is done.
Rotom_Appliances_Main::
	lock
	faceplayer
	msgbox Rotom_Appliances_Text_Unavailable, MSGBOX_DEFAULT
	release
	end

Rotom_Appliances_Text_Unavailable:
	.string "The appliances are all unplugged\n"
	.string "and pushed against the wall.$"

@ Pending Cosmog is interactive; do not re-enter the automatic scene every frame.
LittlerootTown_ProfessorBirchsLab_EventScript_KeepCosmogPending::
	end

LittlerootTown_ProfessorBirchsLab_EventScript_CosmogNoRoom::
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 7
	msgbox LittlerootTown_ProfessorBirchsLab_Text_CosmogNoRoom, MSGBOX_DEFAULT
	releaseall
	end

LittlerootTown_ProfessorBirchsLab_Text_CosmogNoRoom:
	.string "Prof. Birch: Both Cosmog need homes.\p"
	.string "Your party can hold one special\n"
	.string "Pokémon; the other needs a PC slot.\p"
	.string "Make room in your Boxes, then\n"
	.string "speak to me. I'll keep them safe!$"

LittlerootTown_ProfessorBirchsLab_Text_CosmogPairStorage:
	.string "Prof. Birch: I've sent any Cosmog\n"
	.string "that couldn't join your party to PC.\p"
	.string "Name them in your party or the PC.\p"
	.string "Cosmoem evolves into Solgaleo by day\n"
	.string "or Lunala at night. Raise both!$"

LittlerootTown_ProfessorBirchsLab_Movement_RivalLeaveForDemo:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

LittlerootTown_ProfessorBirchsLab_Text_DexNavBrief:
	.string "Prof. Birch: DexNav shows the Pokémon\n"
	.string "available where you're standing.\p"
	.string "Open it from the Start menu.\n"
	.string "You needn't see a Pokémon first!\p"
	.string "{RIVAL} can show you a search. Ask me\n"
	.string "any time for the detailed lesson.$"

LittlerootTown_ProfessorBirchsLab_Text_RivalDexNavInvite:
	.string "{RIVAL}: Come on! I'll meet you at the\n"
	.string "grass on Route 101 and show you.\p"
	.string "Say goodbye to your mom first.\n"
	.string "I'll be waiting just up the road!$"

LittlerootTown_ProfessorBirchsLab_Text_OfferDexNavRefresher:
	.string "Would you like my detailed lesson\n"
	.string "on DexNav, chains and Pokérus?$"
