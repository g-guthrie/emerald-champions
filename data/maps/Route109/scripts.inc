@ NOTE: Route 109's sail to Dewford script references local IDs from Dewford's map. 
@  These are labeled in DewfordTown/scripts.inc

Route109_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route109_OnTransition
	.byte 0

Route109_OnTransition:
	setflag FLAG_VISITED_SLATEPORT_CITY
	end

Route109_EventScript_StartDepartForDewford:: @ 81EE760
	call EventScript_BackupMrBrineyLocation
	setobjectpriority LOCALID_BRINEY_R109, MAP_ROUTE109, 0
	setobjectpriority OBJ_EVENT_ID_PLAYER, MAP_ROUTE109, 0
	applymovement LOCALID_BRINEY_R109, Route109_Movement_BrineyEnterBoat
	waitmovement 0
	removeobject LOCALID_BRINEY_R109
	switch VAR_FACING
	case DIR_SOUTH, Route109_EventScript_EnterBoatSouth
	case DIR_WEST, Route109_EventScript_EnterBoatWest
	case DIR_EAST, Route109_EventScript_EnterBoatEast
	end

Route109_EventScript_EnterBoatSouth:: @ 81EE7A5
	applymovement OBJ_EVENT_ID_PLAYER, Route109_Movement_PlayerEnterBoatSouth
	waitmovement 0
	goto Route109_EventScript_DoSailToDewford
	end

Route109_EventScript_EnterBoatEast:: @ 81EE7B5
	applymovement OBJ_EVENT_ID_PLAYER, Route109_Movement_PlayerEnterBoatEast
	waitmovement 0
	goto Route109_EventScript_DoSailToDewford
	end

Route109_EventScript_EnterBoatWest:: @ 81EE7C5
	applymovement OBJ_EVENT_ID_PLAYER, Route109_Movement_PlayerEnterBoatWest
	waitmovement 0
	goto Route109_EventScript_DoSailToDewford
	end

Route109_EventScript_DoSailToDewford:: @ 81EE7D5
	hideobjectat OBJ_EVENT_ID_PLAYER, MAP_ROUTE109
	call Common_EventScript_PlayBrineysBoatMusic
	applymovement LOCALID_BOAT_R109, Route109_Movement_SailToDewford
	applymovement OBJ_EVENT_ID_PLAYER, Route109_Movement_SailToDewford
	waitmovement 0
	delay 50
	call Common_EventScript_StopBrineysBoatMusic
	showobjectat OBJ_EVENT_ID_PLAYER, MAP_DEWFORD_TOWN
	applymovement OBJ_EVENT_ID_PLAYER, Route109_Movement_PlayerExitBoat
	waitmovement 0
	addobject LOCALID_BOAT_DEWFORD
	clearflag FLAG_HIDE_MR_BRINEY_BOAT_DEWFORD_TOWN
	setobjectxyperm LOCALID_BRINEY_DEWFORD, 12, 8
	addobject LOCALID_BRINEY_DEWFORD
	setobjectpriority LOCALID_BRINEY_DEWFORD, MAP_DEWFORD_TOWN, 0
	applymovement LOCALID_BRINEY_DEWFORD, Route109_Movement_BrineyExitBoat
	waitmovement 0
	clearflag FLAG_HIDE_MR_BRINEY_DEWFORD_TOWN
	setflag FLAG_HIDE_ROUTE_109_MR_BRINEY_BOAT
	hideobjectat LOCALID_BOAT_R109, MAP_ROUTE109
	msgbox DewfordTown_Text_BrineyLandedInDewford, MSGBOX_DEFAULT
	closemessage
	copyvar VAR_BRINEY_LOCATION, VAR_0x8008
	resetobjectpriority OBJ_EVENT_ID_PLAYER, MAP_ROUTE109
	resetobjectpriority LOCALID_BRINEY_DEWFORD, MAP_DEWFORD_TOWN
	copyobjectxytoperm LOCALID_BRINEY_DEWFORD
	release
	end

Route109_Movement_SailToDewford: @ 81EE84F
	walk_in_place_fast_down
	walk_down
	walk_down
	walk_fast_down
	walk_fast_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_left
	walk_left
	walk_in_place_fast_down
	step_end

Route109_Movement_PlayerEnterBoatSouth: @ 81EE8FD
	walk_down
	walk_down
	step_end

Route109_Movement_PlayerExitBoat: @ 81EE900
	walk_down
	walk_down
	walk_left
	walk_in_place_fast_right
	step_end

Route109_Movement_PlayerEnterBoatEast: @ 81EE905
	walk_right
	walk_down
	step_end

Route109_Movement_PlayerEnterBoatWest: @ 81EE908
	walk_left
	walk_down
	step_end

Route109_Movement_BrineyEnterBoat: @ 81EE90B
	walk_down
	step_end

Route109_Movement_BrineyExitBoat: @ 81EE90D
	walk_down
	walk_in_place_fast_left
	step_end

Route109_EventScript_MrBriney:: @ 81EE910
	lock
	faceplayer
	goto_if_unset FLAG_DELIVERED_DEVON_GOODS, Route109_EventScript_HaveNotDeliveredDevonGood
	goto Route109_EventScript_DeliveredDevonGoods
	end

Route109_EventScript_HaveNotDeliveredDevonGood:: @ 81EE921
	msgbox Route109_Text_BrineySailToDewfordQuestion, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route109_EventScript_StayHere
	goto Route109_EventScript_SailToDewford
	end

Route109_EventScript_DeliveredDevonGoods:: @ 81EE93F
	message Route109_Text_BrineyWhereAreWeBound
	waitmessage
	multichoicedefault 21, 8, MULTI_BRINEY_OFF_DEWFORD, 1, FALSE
	switch VAR_RESULT
	case 0, Route109_EventScript_SailToDewford
	case 1, Route109_EventScript_ChoseNotToSail
	case MULTI_B_PRESSED, Route109_EventScript_ChoseNotToSail
	end

Route109_EventScript_SailToDewford:: @ 81EE972
	msgbox Route109_Text_BrineyDewfordItIs, MSGBOX_DEFAULT
	closemessage
	goto Route109_EventScript_StartDepartForDewford
	end

Route109_EventScript_StayHere:: @ 81EE981
	msgbox Route109_Text_BrineyDeliverDevonGoods, MSGBOX_DEFAULT
	release
	end

Route109_EventScript_ChoseNotToSail:: @ 81EE98B
	msgbox Route109_Text_BrineyTellMeWhenYouNeedToSail, MSGBOX_DEFAULT
	release
	end

Route109_EventScript_SeashoreHouseGirl:: @ 81EE995
	msgbox Route109_Text_ChillAtMyPapasSpot, MSGBOX_NPC
	end

Route109_EventScript_SandCastleBoy:: @ 81EE99E
	lock
	faceplayer
	msgbox Route109_Text_SandCastleTakingLongTime, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

Route109_EventScript_SoftSandGirl:: @ 81EE9B5
	lock
	faceplayer
	special GetPlayerBigGuyGirlString
	goto_if_set FLAG_RECEIVED_SOFT_SAND, Route109_EventScript_AlreadyReceivedSoftSand
	msgbox Route109_Text_YouCanHaveThis, MSGBOX_DEFAULT
	giveitem ITEM_SOFT_SAND
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_ShowBagIsFull
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	setflag FLAG_RECEIVED_SOFT_SAND
	release
	end

Route109_EventScript_AlreadyReceivedSoftSand:: @ 81EE9F2
	msgbox Route109_Text_WereGoingToMakeBigCastle, MSGBOX_DEFAULT
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

Route109_EventScript_Woman:: @ 81EEA06
	msgbox Route109_Text_LittleKidsDartAround, MSGBOX_NPC
	end

Route109_EventScript_OldMan:: @ 81EEA0F
	msgbox Route109_Text_ZigzagoonPicksUpLitter, MSGBOX_NPC
	end

Route109_EventScript_Zigzagoon:: @ 81EEA18
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZIGZAGOON, 0
	msgbox Route109_Text_ZigzagoonCry, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Route_109_EventScript_SandMound::
	lockall
	faceplayer
	special RockSmashWildEncounter
	delay 0x60
	removeobject 17
	releaseall
	end

Route_109_EventScript_SandMound1::
	lockall
	faceplayer
	special RockSmashWildEncounter
	delay 0x60
	removeobject 18
	releaseall
	end

Route_109_EventScript_SandMound2::
	lockall
	faceplayer
	special RockSmashWildEncounter
	delay 0x60
	removeobject 19
	releaseall
	end

Route109_EventScript_SeashoreHouseSign:: @ 81EEA2B
	msgbox Route109_Text_SeashoreHouseSign, MSGBOX_SIGN
	end

Route109_EventScript_TrainerTipsSign:: @ 81EEA34
	msgbox Route109_Text_TrainerTipsSign, MSGBOX_SIGN
	end

Route109_EventScript_Huey:: @ 81EEA6B
	@ This retired encounter has no campaign battle.
	msgbox Route109_Text_HueyPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Edmond:: @ 81EEA82
	trainerbattle_double TRAINER_EDMOND, Route109_Text_EdmondIntro, Route109_Text_EdmondDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route109_Text_EdmondPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Lola:: @ 81EEAFB
	trainerbattle_double TRAINER_LOLA_1, Route109_Text_LolaIntro, Route109_Text_LolaDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route109_Text_LolaPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Gwen:: @ 81EEB74
	@ This retired encounter has no campaign battle.
	msgbox Route109_Text_GwenPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Paul:: @ 81EEBA2
	trainerbattle_double TRAINER_MEL_AND_PAUL, Route109_Text_PaulIntro, Route109_Text_PaulDefeated, Route109_Text_PaulNotEnoughPokemon
	msgbox Route109_Text_PaulPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Mel:: @ 81EEBBD
	trainerbattle_double TRAINER_MEL_AND_PAUL, Route109_Text_MelIntro, Route109_Text_MelDefeated, Route109_Text_MelNotEnoughPokemon
	msgbox Route109_Text_MelPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Chandler:: @ 81EEBD8
	trainerbattle_double TRAINER_CHANDLER, Route109_Text_ChandlerIntro, Route109_Text_ChandlerDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route109_Text_ChandlerPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Hailey:: @ 81EEBEF
	trainerbattle_double TRAINER_HAILEY, Route109_Text_HaileyIntro, Route109_Text_HaileyDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route109_Text_HaileyPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_EventScript_Elijah:: @ 81EEC06
	@ This retired encounter has no campaign battle.
	msgbox Route109_Text_ElijahPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Text_BrineyLandedInSlateportDeliverGoods: @ 81EEC1D
	.string "Mr. Briney: Ahoy!\n"
	.string "We've made land in Slateport!\p"
	.string "I suppose you're going to visit Capt.\n"
	.string "Stern and deliver the Devon Parts?$"

Route109_Text_BrineySailToDewfordQuestion: @ 81EEC96
	.string "Mr. Briney: I thought you're supposed\n"
	.string "to deliver the Devon Parts.\p"
	.string "Would you like to sail back to\n"
	.string "Dewford, then?$"

Route109_Text_BrineyDewfordItIs: @ 81EED06
	.string "Mr. Briney: Dewford it is, then!\p"
	.string "Anchors aweigh!\n"
	.string "Peeko, we're setting sail, my darling!$"

Route109_Text_BrineyDeliverDevonGoods: @ 81EED5E
	.string "Mr. Briney: Then you go on and deliver\n"
	.string "the Devon Parts. I'll be waiting.$"

DewfordTown_Text_BrineyLandedInSlateport: @ 81EEDA7
	.string "Mr. Briney: Ahoy! We've made land in\n"
	.string "Slateport!\p"
	.string "You just go on and tell me whenever\n"
	.string "you want to set sail again!$"

Route109_Text_BrineyWhereAreWeBound: @ 81EEE17
	.string "Mr. Briney: Ahoy!\n"
	.string "For you, I'll go out to sea anytime!\p"
	.string "Now, my friend, where are we bound?$"

Route109_Text_BrineyTellMeWhenYouNeedToSail: @ 81EEE72
	.string "Mr. Briney: You just tell me whenever\n"
	.string "you need to set sail again!$"

Route109_Text_ChillAtMyPapasSpot: @ 81EEEB4
	.string "Yo, Trainers!\n"
	.string "Whether you're hot to trot,\l"
	.string "or cool cat not,\l"
	.string "chill at my papa's spot!$"

Route109_Text_LittleKidsDartAround: @ 81EEF08
	.string "Little kids can dart around so quickly…\p"
	.string "You don't dare take your eyes off them\n"
	.string "for an instant. It's very taxing.\p"
	.string "Mine are with my Pokémon, so they\n"
	.string "should be okay, but…$"

Route109_Text_SandCastleTakingLongTime: @ 81EEFB0
	.string "Our sand castle's taking a long time\n"
	.string "to make.$"

Route109_Text_YouCanHaveThis: @ 81EEFDE
	.string "You can have this!$"

Route109_Text_WereGoingToMakeBigCastle: @ 81EEFF1
	.string "We're going to get all the sand from\n"
	.string "the beach and make a big castle!\p"
	.string "And then we're going to be a king and\n"
	.string "queen.\p"
	.string "We'll let you be a servant.$"

Route109_Text_ZigzagoonPicksUpLitter: @ 81EF080
	.string "The water around these parts is clean.\p"
	.string "But, I get my Zigzagoon to pick up\n"
	.string "litter from the shoreline at times.\p"
	.string "Dirty water becomes rain and joins our\n"
	.string "rivers, which we drink from…\p"
	.string "If we pollute the sea, it all comes\n"
	.string "back to haunt us eventually.$"

Route109_Text_ZigzagoonCry: @ 81EF173
	.string "Zigzagoon: Guguu?$"

Route109_Text_SeashoreHouseSign: @ 81EF185
	.string "Seashore House\p"
	.string "“May hot battles rage on hot sands!\n"
	.string "The place for hot Trainers!”$"

Route109_Text_TrainerTipsSign: @ 81EF1D5
	.string "Trainer Tips\p"
	.string "Pokémon at the same level may not\n"
	.string "always have identical stats.\p"
	.string "Pokémon raised by Trainers are said\n"
	.string "to grow stronger than wild Pokémon.$"
