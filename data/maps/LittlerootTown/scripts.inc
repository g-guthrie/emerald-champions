LittlerootTown_MapScripts:: @ 81E7DCB
	map_script MAP_SCRIPT_ON_TRANSITION, LittlerootTown_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, LittlerootTown_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, LittlerootTown_OnWarp
	.byte 0

	@ Who stands where, and when, is in data/progression/maps/LittlerootTown.yaml
	@ (story windows); the story order is data/progression/story.yaml.

LittlerootTown_OnTransition: @ 81E7DDB
	setflag FLAG_VISITED_LITTLEROOT_TOWN
	call Common_EventScript_SetupRivalGfxId
	compare VAR_LITTLEROOT_HOUSES_STATE_MAY, 4
	call_if_eq LittlerootTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent
	compare VAR_LITTLEROOT_HOUSES_STATE_BRENDAN, 4
	call_if_eq LittlerootTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent
	@ Beat the rival on Route 103 and came home some other way: he has left Oldale.
	call_if_eq VAR_STORY_STEP, STORY_STEP_BEAT_RIVAL_103, LittlerootTown_EventScript_RivalLeftOldale
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1
	call_if_eq LittlerootTown_EventScript_HideMapNamePopup
	compare VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2
	call_if_eq LittlerootTown_EventScript_LeftLabAfterDexUpgrade
	end

LittlerootTown_EventScript_LeftLabAfterDexUpgrade:: @ 81E7E45
	setvar VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3
	return

LittlerootTown_EventScript_HideMapNamePopup:: @ 81E7E4B
	setflag FLAG_HIDE_MAP_NAME_POPUP
	return

LittlerootTown_EventScript_RivalLeftOldale::
	advance_story RIVAL_LEFT_OLDALE
	return

LittlerootTown_EventScript_SetExitedHouseAfterLatiSSTicketEvent:: @ 81E7E5C
	setvar VAR_LITTLEROOT_HOUSES_STATE_MAY, 5
	setvar VAR_LITTLEROOT_HOUSES_STATE_BRENDAN, 5
	return

LittlerootTown_OnFrame: @ 81E7EC1
	map_script_2 VAR_STORY_STEP, STORY_STEP_TRUCK_STOPPED, LittlerootTown_EventScript_StepOffTruck
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, LittlerootTown_EventScript_BeginDexUpgradeScene
	.2byte 0

@ Arrival: Mom comes out to the truck and takes the player inside.
LittlerootTown_EventScript_StepOffTruck::
	lockall
	npc_pick 18
	delay 15
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, LittlerootTown_Movement_PlayerStepOffTruck
	waitmovement 0
	opendoor VAR_0x8009, VAR_0x800A
	waitdooranim
	addobject VAR_0x8008
	closedoor VAR_0x8009, VAR_0x800A
	waitdooranim
	delay 10
	approach_player VAR_0x8008
	waitmovement 0
	face_npc VAR_0x8008
	msgbox LittlerootTown_Text_OurNewHomeLetsGoInside, MSGBOX_DEFAULT
	closemessage
	walk_home VAR_0x8008
	waitmovement 0
	opendoor VAR_0x8009, VAR_0x800A
	waitdooranim
	walk_to VAR_0x8008, VAR_0x8009, VAR_0x800A
	waitmovement 0
	removeobject VAR_0x8008
	walk_to OBJ_EVENT_ID_PLAYER, VAR_0x8009, VAR_0x800A
	waitmovement 0
	advance_story ENTERED_HOUSE
	hideplayer
	closedoor VAR_0x8009, VAR_0x800A
	waitdooranim
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	checkplayergender
	goto_if_eq VAR_RESULT, FEMALE, LittlerootTown_EventScript_EnterMaysHouse
	warpsilent MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_1F, 255, 8, 8
	waitstate
	releaseall
	end

LittlerootTown_EventScript_EnterMaysHouse:
	warpsilent MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F, 255, 2, 8
	waitstate
	releaseall
	end

LittlerootTown_Movement_PlayerStepOffTruck: @ 81E7FAC
	jump_right
	delay_16
	delay_16
	delay_16
	step_end

LittlerootTown_EventScript_BeginDexUpgradeScene:: @ 81E7FB1
	lockall
	playse SE_PIN
	applymovement LOCALID_LITTLEROOT_BIRCH, Common_Movement_ExclamationMark
	waitmovement 0
	delay 80
	msgbox LittlerootTown_Text_BirchSomethingToShowYouAtLab, MSGBOX_DEFAULT
	closemessage
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_RIVAL
	clearflag FLAG_HIDE_LITTLEROOT_TOWN_BIRCH
	delay 20
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	warp MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 255, 6, 5
	waitstate
	releaseall
	end

LittlerootTown_OnWarp: @ 81E7FE2
	map_script_2 VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 1, LittlerootTown_EventScript_SetRivalBirchPosForDexUpgrade
	.2byte 0

LittlerootTown_EventScript_SetRivalBirchPosForDexUpgrade:: @ 81E7FEC
	addobject LOCALID_LITTLEROOT_BIRCH
	addobject LOCALID_LITTLEROOT_RIVAL
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq LittlerootTown_EventScript_SetRivalBirchPosForDexUpgradeMale
	goto LittlerootTown_EventScript_SetRivalBirchPosForDexUpgradeFemale
	end

LittlerootTown_EventScript_SetRivalBirchPosForDexUpgradeMale:: @ 81E8004
	setobjectxy LOCALID_LITTLEROOT_RIVAL, 6, 10
	setobjectxy LOCALID_LITTLEROOT_BIRCH, 5, 10
	end

LittlerootTown_EventScript_SetRivalBirchPosForDexUpgradeFemale:: @ 81E8013
	setobjectxy LOCALID_LITTLEROOT_RIVAL, 13, 10
	setobjectxy LOCALID_LITTLEROOT_BIRCH, 14, 10
	end

LittlerootTown_EventScript_FatMan:: @ 81E8022
	msgbox LittlerootTown_Text_CanUsePCToStoreItems, MSGBOX_NPC
	end

LittlerootTown_EventScript_Boy:: @ 81E802B
	msgbox LittlerootTown_Text_BirchSpendsDaysInLab, MSGBOX_NPC
	end

@ The twin is three entities (guard, worried, free); what she says follows the story.
LittlerootTown_EventScript_Twin:: @ 81E8034
	lock
	faceplayer
	goto_if_set STORY_REACHED_GOT_POKEDEX, LittlerootTown_EventScript_GoodLuck
	goto_if_set STORY_REACHED_RESCUED_BIRCH, LittlerootTown_EventScript_YouSavedBirch
	goto_if_set STORY_REACHED_MET_RIVAL, LittlerootTown_EventScript_GoSaveBirch
	msgbox LittlerootTown_Text_IfYouGoInGrassPokemonWillJumpOut, MSGBOX_DEFAULT
	release
	end

LittlerootTown_EventScript_GoSaveBirch:: @ 81E805D
	special GetPlayerBigGuyGirlString
	msgbox LittlerootTown_Text_CanYouGoSeeWhatsHappening, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_LITTLEROOT_TWIN_WORRIED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	call_if_eq VAR_STORY_STEP, STORY_STEP_MET_RIVAL, LittlerootTown_EventScript_SentToBirch
	release
	end

LittlerootTown_EventScript_SentToBirch:
	advance_story SENT_TO_BIRCH
	return

LittlerootTown_EventScript_YouSavedBirch:: @ 81E807A
	special GetPlayerBigGuyGirlString
	msgbox LittlerootTown_Text_YouSavedBirch, MSGBOX_DEFAULT
	release
	end

LittlerootTown_EventScript_GoodLuck:: @ 81E8087
	msgbox LittlerootTown_Text_GoodLuckCatchingPokemon, MSGBOX_DEFAULT
	release
	end

@ No Pokemon yet: the twin steps in front of the player, turns them back, goes back.
LittlerootTown_EventScript_NeedPokemonTrigger::
	lockall
	approach_player LOCALID_LITTLEROOT_TWIN_GUARD, fast=1
	waitmovement 0
	msgbox LittlerootTown_Text_IfYouGoInGrassPokemonWillJumpOut, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, LittlerootTown_Movement_PushPlayerBackFromRoute
	waitmovement 0
	msgbox LittlerootTown_Text_DangerousIfYouDontHavePokemon, MSGBOX_DEFAULT
	closemessage
	walk_home LOCALID_LITTLEROOT_TWIN_GUARD
	waitmovement 0
	applymovement LOCALID_LITTLEROOT_TWIN_GUARD, Common_Movement_FaceDown
	waitmovement 0
	releaseall
	end

LittlerootTown_Movement_PushPlayerBackFromRoute: @ 81E80EB
	lock_facing_direction
	walk_down
	unlock_facing_direction
	step_end

LittlerootTown_EventScript_GoSaveBirchTrigger:: @ 81E811F
	lockall
	applymovement LOCALID_LITTLEROOT_TWIN_WORRIED, Common_Movement_WalkInPlaceFastestRight
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	waitmovement 0
	special GetPlayerBigGuyGirlString
	msgbox LittlerootTown_Text_CanYouGoSeeWhatsHappening, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_LITTLEROOT_TWIN_WORRIED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	advance_story SENT_TO_BIRCH
	releaseall
	end

LittlerootTown_EventScript_TownSign:: @ 81E8151
	msgbox LittlerootTown_Text_TownSign, MSGBOX_SIGN
	end

LittlerootTown_EventScript_BirchsLabSign:: @ 81E815A
	msgbox LittlerootTown_Text_ProfBirchsLab, MSGBOX_SIGN
	end

LittlerootTown_EventScript_BrendansHouseSign:: @ 81E8163
	lockall
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq LittlerootTown_EventScript_PlayersHouseSignMale
	compare VAR_RESULT, FEMALE
	call_if_eq LittlerootTown_EventScript_BirchsHouseSignFemale
	releaseall
	end

LittlerootTown_EventScript_PlayersHouseSignMale:: @ 81E817D
	msgbox LittlerootTown_Text_PlayersHouse, MSGBOX_DEFAULT
	return

LittlerootTown_EventScript_BirchsHouseSignFemale:: @ 81E8186
	msgbox LittlerootTown_Text_ProfBirchsHouse, MSGBOX_DEFAULT
	return

LittlerootTown_EventScript_MaysHouseSign:: @ 81E818F
	lockall
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq LittlerootTown_EventScript_BirchsHouseSignMale
	compare VAR_RESULT, FEMALE
	call_if_eq LittlerootTown_EventScript_PlayersHouseSignFemale
	releaseall
	end

LittlerootTown_EventScript_BirchsHouseSignMale:: @ 81E81A9
	msgbox LittlerootTown_Text_ProfBirchsHouse, MSGBOX_DEFAULT
	return

LittlerootTown_EventScript_PlayersHouseSignFemale:: @ 81E81B2
	msgbox LittlerootTown_Text_PlayersHouse, MSGBOX_DEFAULT
	return

@ After the Pokedex, Mom waits in front of the house. Stepping on any exit tile, or
@ talking to her, brings the Running Shoes and Dad's rod (entity 18).
LittlerootTown_EventScript_MomShoesTrigger::
	lockall
	npc_pick 18
	applymovement VAR_0x8008, Common_Movement_ExclamationMark
	waitmovement 0
	npc_say 18, wait
	closemessage
	approach_player VAR_0x8008
	waitmovement 0
	face_npc VAR_0x8008
	goto LittlerootTown_EventScript_MomGivesShoes
	end

LittlerootTown_EventScript_Mom:: @ 81E863D
	lock
	faceplayer
	npc_pick 18
	goto LittlerootTown_EventScript_MomGivesShoes
	end

LittlerootTown_EventScript_MomGivesShoes:
	npc_say 18, gift
	npc_reward 18
	npc_say 18, shoes
	playfanfare MUS_OBTAIN_ITEM
	message Story_E18_Text_switched
	waitmessage
	waitfanfare
	npc_say 18, explain
	npc_say 18, farewell
	closemessage
	delay 30
	walk_home VAR_0x8008
	waitmovement 0
	opendoor VAR_0x8009, VAR_0x800A
	waitdooranim
	walk_to VAR_0x8008, VAR_0x8009, VAR_0x800A
	waitmovement 0
	removeobject VAR_0x8008
	closedoor VAR_0x8009, VAR_0x800A
	waitdooranim
	releaseall
	end

LittlerootTown_Text_OurNewHomeLetsGoInside: @ 81E86BC
	.string "Mom: {PLAYER}, we're here, honey!\p"
	.string "It must be tiring riding with our things\n"
	.string "in the moving truck.\p"
	.string "Well, this is Littleroot Town.\p"
	.string "How do you like it?\n"
	.string "This is our new home!\p"
	.string "It has a quaint feel, but it seems to be\n"
	.string "an easy place to live, don't you think?\p"
	.string "And, you get your own room, {PLAYER}!\n"
	.string "Let's go inside.$"

LittlerootTown_Text_CanUsePCToStoreItems: @ 81E8ACF
	.string "If you use a PC, you can store items\n"
	.string "and Pokémon.\p"
	.string "The power of science is staggering!$"

LittlerootTown_Text_BirchSpendsDaysInLab: @ 81E8B25
	.string "Prof. Birch spends days in his Lab\n"
	.string "studying, then he'll suddenly go out in\l"
	.string "the wild to do more research…\p"
	.string "When does Prof. Birch spend time\n"
	.string "at home?$"

LittlerootTown_Text_IfYouGoInGrassPokemonWillJumpOut: @ 81E8BB8
	.string "Um, um, um!\p"
	.string "If you go outside and go in the grass,\n"
	.string "wild Pokémon will jump out!$"

LittlerootTown_Text_DangerousIfYouDontHavePokemon: @ 81E8C07
	.string "It's dangerous if you don't have\n"
	.string "your own Pokémon.$"

LittlerootTown_Text_CanYouGoSeeWhatsHappening: @ 81E8C3A
	.string "Um, hi!\p"
	.string "There are scary Pokémon outside!\n"
	.string "I can hear their cries!\p"
	.string "I want to go see what's going on,\n"
	.string "but I don't have any Pokémon…\p"
	.string "Can you go see what's happening\n"
	.string "for me?$"

LittlerootTown_Text_YouSavedBirch: @ 81E8CE3
	.string "You saved Prof. Birch!\n"
	.string "I'm so glad!$"

LittlerootTown_Text_GoodLuckCatchingPokemon: @ 81E8D07
	.string "Are you going to catch Pokémon?\n"
	.string "Good luck!$"

LittlerootTown_Text_TownSign: @ 81E8D32
	.string "Littleroot Town\n"
	.string "“A town that can't be shaded any hue.”$"

LittlerootTown_Text_ProfBirchsLab: @ 81E8D69
	.string "Prof. Birch's Pokémon Lab$"

LittlerootTown_Text_PlayersHouse: @ 81E8D83
	.string "{PLAYER}'s House$"

LittlerootTown_Text_ProfBirchsHouse: @ 81E8D8E
	.string "Prof. Birch's House$"

LittlerootTown_Text_BirchSomethingToShowYouAtLab: @ 81E8DA2
	.string "Prof. Birch: Well, well, {PLAYER}{KUN}!\n"
	.string "That was good work out there!\p"
	.string "I knew there was something special\n"
	.string "about you when I first saw you,\l"
	.string "but I never expected this.\p"
	.string "I have something to show you.\n"
	.string "Let's go to my Lab.$"
