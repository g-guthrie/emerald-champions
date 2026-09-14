PetalburgCity_WallysHouse_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, PetalburgCity_WallysHouse_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, PetalburgCity_WallysHouse_OnWarp
	.byte 0

PetalburgCity_WallysHouse_OnWarp:
	map_script_2 VAR_PETALBURG_CITY_STATE, 4, PetalburgCity_WallysHouse_EventScript_PlayerWallysDadFaceEachOther
	.2byte 0

PetalburgCity_WallysHouse_EventScript_PlayerWallysDadFaceEachOther::
	turnobject LOCALID_PLAYER, DIR_EAST
	turnobject LOCALID_WALLYS_HOUSE_WALLYS_DAD, DIR_WEST
	end

PetalburgCity_WallysHouse_OnFrame:
	map_script_2 VAR_PETALBURG_CITY_STATE, 4, PetalburgCity_WallysHouse_EventScript_GiveHMSurf
	.2byte 0

PetalburgCity_WallysHouse_EventScript_GiveHMSurf::
	lockall
	setvar VAR_PETALBURG_CITY_STATE, 5
	msgbox PetalburgCity_WallysHouse_Text_PleaseExcuseUs, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_HM_SURF
	call PetalburgCity_WallysHouse_EventScript_ExplainSurfRoute
	releaseall
	end

PetalburgCity_WallysHouse_EventScript_WallysDad::
	lock
	faceplayer
	goto_if_lt VAR_PETALBURG_GYM_STATE, 2, PetalburgCity_WallysHouse_EventScript_BeforeWallyCatch
	goto_if_set FLAG_DEFEATED_WALLY_VICTORY_ROAD, PetalburgCity_WallysHouse_EventScript_DefeatedWallyInVictoryRoad
	goto_if_set FLAG_RECEIVED_HM_SURF, PetalburgCity_WallysHouse_EventScript_ReceivedHMSurf
	goto_if_set FLAG_BADGE05_GET, PetalburgCity_WallysHouse_EventScript_GiveHMSurf
	goto_if_set FLAG_THANKED_FOR_PLAYING_WITH_WALLY, PetalburgCity_WallysHouse_EventScript_PlayedWithWally
	msgbox PetalburgCity_WallysHouse_Text_ThanksForPlayingWithWally, MSGBOX_DEFAULT
	setflag FLAG_THANKED_FOR_PLAYING_WITH_WALLY
	release
	end

PetalburgCity_WallysHouse_EventScript_ReceivedHMSurf::
	goto_if_unset FLAG_BADGE06_GET, PetalburgCity_WallysHouse_EventScript_RemindFortreeRoute
	msgbox PetalburgCity_WallysHouse_Text_WallyIsComingHomeSoon, MSGBOX_DEFAULT
	release
	end

PetalburgCity_WallysHouse_EventScript_RemindFortreeRoute::
	call PetalburgCity_WallysHouse_EventScript_ExplainSurfRoute
	release
	end

PetalburgCity_WallysHouse_EventScript_ExplainSurfRoute::
	goto_if_set FLAG_EC_REPORT_C28_COMPLETE, PetalburgCity_WallysHouse_EventScript_ExplainEastRoute
	msgbox PetalburgCity_WallysHouse_Text_SurfGoAllSortsOfPlaces, MSGBOX_DEFAULT
	return

PetalburgCity_WallysHouse_EventScript_ExplainEastRoute::
	msgbox PetalburgCity_WallysHouse_Text_EastRoute, MSGBOX_DEFAULT
	return

PetalburgCity_WallysHouse_EventScript_DefeatedWallyInVictoryRoad::
	msgbox PetalburgCity_WallysHouse_Text_YouMetWallyInEverGrandeCity, MSGBOX_DEFAULT
	release
	end

PetalburgCity_WallysHouse_EventScript_PlayedWithWally::
	msgbox PetalburgCity_WallysHouse_Text_WonderHowWallyIsDoing, MSGBOX_DEFAULT
	release
	end

PetalburgCity_WallysHouse_EventScript_WallysMom::
	lock
	faceplayer
	goto_if_lt VAR_PETALBURG_GYM_STATE, 2, PetalburgCity_WallysHouse_EventScript_BeforeWallyCatch
	goto_if_set FLAG_RECEIVED_HM_SURF, PetalburgCity_WallysHouse_EventScript_MomReceivedHMSurf
	msgbox PetalburgCity_WallysHouse_Text_WallyWasReallyHappy, MSGBOX_DEFAULT
	release
	end

PetalburgCity_WallysHouse_EventScript_MomReceivedHMSurf::
	msgbox PetalburgCity_WallysHouse_Text_WallyLeftWithoutTelling, MSGBOX_DEFAULT
	release
	end

PetalburgCity_WallysHouse_Text_ThanksForPlayingWithWally:
	.string "You must be {PLAYER}{KUN}!\n"
	.string "Thank you for helping WALLY catch\l"
	.string "his first POKéMON.\p"
	.string "He's often been too ill to go out.\n"
	.string "Seeing him so excited means a lot.\p"
	.string "He's left to stay with relatives\n"
	.string "in VERDANTURF TOWN. We hope the\l"
	.string "clean air will do him good.$"

PetalburgCity_WallysHouse_Text_WonderHowWallyIsDoing:
	.string "I wonder how our WALLY is doing?$"

PetalburgCity_WallysHouse_Text_PleaseExcuseUs:
	.string "{PLAYER}{KUN}! Please excuse us for\n"
	.string "dragging you here this way.\p"
	.string "But our WALLY's become very healthy\n"
	.string "since he went to VERDANTURF TOWN.\p"
	.string "We owe it all to you!\p"
	.string "When WALLY left town, you helped\n"
	.string "him catch a POKéMON, right?\p"
	.string "I think that made WALLY really\n"
	.string "happy.\p"
	.string "Actually, not just WALLY.\n"
	.string "It made me, his father, happy too.\p"
	.string "Happy that he's gained such a great\n"
	.string "friend as you.\p"
	.string "Let me register SURF for your team.\n"
	.string "You've earned that freedom!$"

PetalburgCity_WallysHouse_Text_SurfGoAllSortsOfPlaces:
	.string "A partner that can learn SURF can\n"
	.string "carry you across water.\p"
	.string "It need not learn the move first.\n"
	.string "Keep it in your party and face\l"
	.string "the water.\p"
	.string "LOTAD in the grass on ROUTE 102,\n"
	.string "just east of town, can help.\p"
	.string "Return to MAUVILLE. WATTSON is\n"
	.string "near the city's east exit.\p"
	.string "He needs help in NEW MAUVILLE.\n"
	.string "Its entrance is across the water\l"
	.string "on ROUTE 110, south of MAUVILLE.$"

PetalburgCity_WallysHouse_Text_EastRoute:
	.string "With WATTSON's problem settled,\n"
	.string "leave MAUVILLE east on ROUTE 118.\p"
	.string "SURF across the river, then take\n"
	.string "ROUTE 119 north to the WEATHER\l"
	.string "INSTITUTE. FORTREE lies beyond.$"

PetalburgCity_WallysHouse_Text_WallyIsComingHomeSoon:
	.string "WALLY's coming home soon.\n"
	.string "I'm looking forward to that.$"

PetalburgCity_WallysHouse_Text_YouMetWallyInEverGrandeCity:
	.string "Oh? You met WALLY in\n"
	.string "EVER GRANDE CITY?\p"
	.string "Oh, {PLAYER}{KUN}, don't be silly.\p"
	.string "He may have gotten healthy, but he\n"
	.string "can't go somewhere far away like\l"
	.string "that all by himself.$"

PetalburgCity_WallysHouse_Text_WallyWasReallyHappy:
	.string "WALLY was really happy when he told\n"
	.string "us that he caught a POKéMON.\p"
	.string "It's been ages since I've seen him\n"
	.string "smile like that.$"

PetalburgCity_WallysHouse_Text_WallyLeftWithoutTelling:
	.string "I want you to keep this a secret\n"
	.string "from my husband…\p"
	.string "But our WALLY left VERDANTURF TOWN\n"
	.string "without telling anyone.\p"
	.string "You know, WALLY is frail, but\n"
	.string "he's surprisingly strong-willed.\p"
	.string "I'm sure that he'll come back safe\n"
	.string "and sound one day!$"

PetalburgCity_WallysHouse_EventScript_BeforeWallyCatch::
	msgbox PetalburgCity_WallysHouse_Text_BeforeWallyCatch, MSGBOX_DEFAULT
	release
	end

PetalburgCity_WallysHouse_Text_BeforeWallyCatch:
	.string "Our WALLY is going to stay with\n"
	.string "relatives in VERDANTURF TOWN.\p"
	.string "He hopes to take a POKéMON along.\n"
	.string "He went to ask NORMAN for help.$"
