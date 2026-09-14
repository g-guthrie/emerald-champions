SSTidalCorridor_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SSTidalCorridor_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SSTidalCorridor_OnFrame
	.byte 0

SSTidalCorridor_OnFrame:
	map_script_2 VAR_TEMP_F, 0, SSTidalCorridor_EventScript_CheckArrival
	map_script_2 VAR_SS_TIDAL_STATE, SS_TIDAL_BOARD_SLATEPORT, SSTidalCorridor_EventScript_DepartSlateportForLilycove
	map_script_2 VAR_SS_TIDAL_STATE, SS_TIDAL_BOARD_LILYCOVE, SSTidalCorridor_EventScript_DepartLilycoveForSlateport
	map_script_2 VAR_SS_TIDAL_STATE, SS_TIDAL_EXIT_CURRENTS_RIGHT, SSTidalCorridor_EventScript_HalfwayToLilycove
	map_script_2 VAR_SS_TIDAL_STATE, SS_TIDAL_EXIT_CURRENTS_LEFT, SSTidalCorridor_EventScript_ArrivedInSlateport
	.2byte 0

SSTidalCorridor_EventScript_DepartSlateportForLilycove::
	special SetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_DEPART_SLATEPORT
	lockall
	playse SE_DING_DONG
	msgbox SSTidal_Text_FastCurrentsHopeYouEnjoyVoyage, MSGBOX_DEFAULT
	releaseall
	end

SSTidalCorridor_EventScript_DepartLilycoveForSlateport::
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_DEPART_LILYCOVE
	lockall
	playse SE_DING_DONG
	msgbox SSTidal_Text_HopeYouEnjoyVoyage, MSGBOX_DEFAULT
	releaseall
	end

SSTidalRooms_EventScript_HalfwayToSlateport::
	special SetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_HALFWAY_SLATEPORT
	playse SE_DING_DONG
	msgbox SSTidal_Text_FastCurrentsHopeYouEnjoyVoyage, MSGBOX_DEFAULT
	return

SSTidalRooms_EventScript_ArrivedInLilycove::
	special ResetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_LILYCOVE
	playse SE_DING_DONG
	msgbox SSTidal_Text_MadeLandInLilycove, MSGBOX_DEFAULT
	return

SSTidalCorridor_EventScript_ReachedStepCount::
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_DEPART_SLATEPORT, SSTidalCorridor_EventScript_HalfwayToLilycove
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_HALFWAY_SLATEPORT, SSTidalCorridor_EventScript_ArrivedInSlateport
	end

SSTidalCorridor_EventScript_HalfwayToLilycove::
	special ResetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_HALFWAY_LILYCOVE
	lockall
	playse SE_DING_DONG
	msgbox SSTidal_Text_HopeYouEnjoyVoyage, MSGBOX_DEFAULT
	releaseall
	end

SSTidalCorridor_EventScript_ArrivedInSlateport::
	special ResetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_SLATEPORT
	lockall
	playse SE_DING_DONG
	msgbox SSTidal_Text_MadeLandInSlateport, MSGBOX_DEFAULT
	releaseall
	end

SSTidalRooms_EventScript_ArrivedInSlateport::
	special ResetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_SLATEPORT
	playse SE_DING_DONG
	msgbox SSTidal_Text_MadeLandInSlateport, MSGBOX_DEFAULT
	return

SSTidalRooms_EventScript_ProgessCruiseAfterBed::
	switch VAR_SS_TIDAL_STATE
	case SS_TIDAL_DEPART_SLATEPORT, SSTidalRooms_EventScript_ArrivedInLilycove
	case SS_TIDAL_HALFWAY_LILYCOVE, SSTidalRooms_EventScript_ArrivedInLilycove
	case SS_TIDAL_DEPART_LILYCOVE, SSTidalRooms_EventScript_HalfwayToSlateport
	case SS_TIDAL_HALFWAY_SLATEPORT, SSTidalRooms_EventScript_ArrivedInSlateport
	return

SSTidalCorridor_EventScript_Briney::
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_EXPEDITION, SSTidalCorridor_EventScript_CaptainArrival
	msgbox SSTidalCorridor_Text_BrineyWelcomeAboard, MSGBOX_NPC
	end

SSTidalCorridor_EventScript_Peeko::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_WINGULL, CRY_MODE_NORMAL
	msgbox SSTidalCorridor_Text_Peeko, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SSTidalCorridor_EventScript_Cabin1Sign::
	msgbox SSTidalCorridor_Text_Cabin1, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_Cabin2Sign::
	msgbox SSTidalCorridor_Text_Cabin2, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_Cabin3Sign::
	msgbox SSTidalCorridor_Text_Cabin3, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_Cabin4Sign::
	msgbox SSTidalCorridor_Text_Cabin4, MSGBOX_SIGN
	end

SSTidalCorridor_EventScript_ExitSailor::
	lock
	faceplayer
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_EXPEDITION, SSTidalCorridor_EventScript_ExpeditionGuide
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_LILYCOVE, SSTidalCorridor_EventScript_ExitLilycove
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_SLATEPORT, SSTidalCorridor_EventScript_ExitSlateport
	msgbox SSTidalCorridor_Text_CanRestInCabin2, MSGBOX_DEFAULT
	release
	end

SSTidalCorridor_EventScript_ExitLilycove::
	setrespawn HEAL_LOCATION_LILYCOVE_CITY
	msgbox SSTidalCorridor_Text_WeveArrived, MSGBOX_DEFAULT
	call_if_set FLAG_RECEIVED_SS_TIDAL_REAPER_CLOTH, SSTidalCorridor_EventScript_HideReaperClothGiver
	warp MAP_LILYCOVE_CITY_HARBOR, 8, 11
	waitstate
	release
	end

SSTidalCorridor_EventScript_ExitSlateport::
	setrespawn HEAL_LOCATION_SLATEPORT_CITY
	msgbox SSTidalCorridor_Text_WeveArrived, MSGBOX_DEFAULT
	call_if_set FLAG_RECEIVED_SS_TIDAL_REAPER_CLOTH, SSTidalCorridor_EventScript_HideReaperClothGiver
	warp MAP_SLATEPORT_CITY_HARBOR, 8, 11
	waitstate
	release
	end

SSTidalCorridor_EventScript_HideReaperClothGiver::
	setflag FLAG_HIDE_SS_TIDAL_ROOMS_SNATCH_GIVER
	return

SSTidalCorridor_EventScript_Porthole::
	lockall
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_DEPART_SLATEPORT, SSTidalCorridor_EventScript_LookThroughPorthole
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_HALFWAY_SLATEPORT, SSTidalCorridor_EventScript_LookThroughPorthole
	msgbox SSTidalCorridor_Text_HorizonSpreadsBeyondPorthole, MSGBOX_DEFAULT
	releaseall
	end

SSTidalCorridor_EventScript_LookThroughPorthole::
	special LookThroughPorthole
	end

SSTidalCorridor_EventScript_Sailor::
	msgbox SSTidalCorridor_Text_EnjoyYourCruise, MSGBOX_NPC
	end

SSTidalCorridor_EventScript_ScottScene::
	lockall
	applymovement LOCALID_SS_TIDAL_SCOTT, SSTidalCorridor_Movement_ScottApproachPlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox SSTidalCorridor_Text_ScottBattleFrontierInvite, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, SSTidalCorridor_Movement_PlayerWatchScottExit
	applymovement LOCALID_SS_TIDAL_EXIT_SAILOR, SSTidalCorridor_Movement_SailorMoveForScott
	applymovement LOCALID_SS_TIDAL_SCOTT, SSTidalCorridor_Movement_ScottExit
	waitmovement 0
	playse SE_EXIT
	waitse
	removeobject LOCALID_SS_TIDAL_SCOTT
	applymovement LOCALID_SS_TIDAL_EXIT_SAILOR, SSTidalCorridor_Movement_SailorReturn
	waitmovement 0
	delay 30
	setflag FLAG_MET_SCOTT_ON_SS_TIDAL
	clearflag FLAG_SCOTT_CALL_BATTLE_FRONTIER
	setvar VAR_SS_TIDAL_SCOTT_STATE, 1
	releaseall
	end

SSTidalCorridor_Movement_ScottApproachPlayer:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

SSTidalCorridor_Movement_ScottExit:
	walk_in_place_faster_down
	delay_16
	delay_16
	delay_16
	delay_16
	walk_down
	walk_left
	step_end

SSTidalCorridor_Movement_PlayerWatchScottExit:
	delay_16
	delay_16
	delay_16
	delay_16
	delay_8
	walk_in_place_faster_down
	step_end

SSTidalCorridor_Movement_SailorMoveForScott:
	delay_16
	walk_right
	walk_right
	walk_in_place_faster_up
	delay_16
	walk_in_place_faster_left
	step_end

SSTidalCorridor_Movement_SailorReturn:
	walk_left
	walk_left
	walk_in_place_faster_up
	step_end

SSTidalCorridor_Text_ScottBattleFrontierInvite:
	.string "SCOTT: {PLAYER}{KUN}! Just the TRAINER\n"
	.string "I hoped to run into!\p"
	.string "I've watched you come a long way.\n"
	.string "Now the sea has opened up a whole\l"
	.string "new set of challenges for you.\p"
	.string "The BATTLE FRONTIER! Bring six\n"
	.string "partners and see how far you can\l"
	.string "go in the CHAMPIONS CIRCUIT.\p"
	.string "The ferry attendants can take you\n"
	.string "there. I'll be waiting for you!$"

SSTidal_Text_FastCurrentsHopeYouEnjoyVoyage:
	.string "This ferry is built to plow through\n"
	.string "fast-running currents.\p"
	.string "We hope you enjoy your voyage with us.\n"
	.string "Feel free to explore the ship.$"

SSTidal_Text_HopeYouEnjoyVoyage:
	.string "We hope you enjoy your voyage on\n"
	.string "our ferry.$"

SSTidal_Text_MadeLandInSlateport:
	.string "We have made land in SLATEPORT CITY.\n"
	.string "Thank you for sailing with us.$"

SSTidal_Text_MadeLandInLilycove:
	.string "We have made land in LILYCOVE CITY.\n"
	.string "Thank you for sailing with us.$"

SSTidalCorridor_Text_CanRestInCabin2:
	.string "It'll be some time before we make land,\n"
	.string "I reckon.\p"
	.string "You can rest up in your cabin if you'd\n"
	.string "like. Your cabin's No. 2.\p"
	.string "The bed in there is soft and plushy.\n"
	.string "I can attest to how comfy it is!$"

SSTidalCorridor_Text_WeveArrived:
	.string "We've arrived!$"

SSTidalCorridor_Text_VisitOtherCabins:
	.string "Go visit other cabins.\n"
	.string "TRAINERS bored of the boat trip will\l"
	.string "be itching to battle.$"

SSTidalCorridor_Text_EnjoyYourCruise:
	.string "Enjoy your cruise!$"

SSTidalCorridor_Text_HorizonSpreadsBeyondPorthole:
	.string "The horizon spreads beyond\n"
	.string "the porthole.$"

SSTidalCorridor_Text_BrineyWelcomeAboard:
	.string "MR. BRINEY: Welcome aboard, {PLAYER}{KUN}!\p"
	.string "They made me honorary captain of\n"
	.string "the S.S. TIDAL!\p"
	.string "You can call me CAPTAIN BRINEY now!\p"
	.string "You know, I retired once before,\n"
	.string "but when I saw this majestic ship…\p"
	.string "Let me just say, it stirred my sleeping\n"
	.string "soul as a sailor!$"

SSTidalCorridor_Text_Peeko:
	.string "PEEKO: Pihyo pihyohyo…$"

SSTidalCorridor_Text_Cabin1:
	.string "Cabin 1$"

SSTidalCorridor_Text_Cabin2:
	.string "Cabin 2$"

SSTidalCorridor_Text_Cabin3:
	.string "Cabin 3$"

SSTidalCorridor_Text_Cabin4:
	.string "Cabin 4$"

@ The first island departure owns the ship chapter, independently of paper receipts.
SSTidalCorridor_EventScript_BoardExpedition::
	goto_if_set FLAG_EC_FIRST_ISLAND_VOYAGE_COMPLETE, SSTidalCorridor_EventScript_ArriveSouthernIsland
	setvar VAR_SS_TIDAL_STATE, SS_TIDAL_EXPEDITION
	special ResetSSTidalFlag
	clearflag FLAG_HIDE_SS_TIDAL_CORRIDOR_MR_BRINEY
	setflag FLAG_EC_FERRY_ARRIVAL_PENDING
	warp MAP_SS_TIDAL_CORRIDOR, 1, 10
	waitstate
	release
	end

SSTidalCorridor_OnTransition:
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_EXPEDITION, SSTidalCorridor_EventScript_HideScott
	goto_if_ne VAR_SS_TIDAL_SCOTT_STATE, 0, SSTidalCorridor_EventScript_ArrivalEnd
	clearflag FLAG_HIDE_SS_TIDAL_CORRIDOR_SCOTT
	end

SSTidalCorridor_EventScript_HideScott::
	setflag FLAG_HIDE_SS_TIDAL_CORRIDOR_SCOTT
	end

SSTidalCorridor_EventScript_CheckArrival::
	setvar VAR_TEMP_F, 1
	goto_if_eq VAR_SS_TIDAL_STATE, SS_TIDAL_EXPEDITION, SSTidalCorridor_EventScript_ExpeditionArrival
	goto_if_eq VAR_SS_TIDAL_SCOTT_STATE, 0, SSTidalCorridor_EventScript_ScottScene
	end

SSTidalCorridor_EventScript_ExpeditionArrival::
	goto_if_unset FLAG_EC_FERRY_ARRIVAL_PENDING, SSTidalCorridor_EventScript_ArrivalEnd
	clearflag FLAG_EC_FERRY_ARRIVAL_PENDING
	lockall
	msgbox SSTidalCorridor_Text_ExpeditionGuide, MSGBOX_DEFAULT
	releaseall
SSTidalCorridor_EventScript_ArrivalEnd::
	end

SSTidalCorridor_EventScript_ExpeditionGuide::
	msgbox SSTidalCorridor_Text_ExpeditionGuide, MSGBOX_DEFAULT
	release
	end

SSTidalCorridor_EventScript_CaptainArrival::
	lock
	faceplayer
	msgbox SSTidalCorridor_Text_ReadyForSouthernIsland, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SSTidalCorridor_EventScript_ExpeditionGuide
	setflag FLAG_EC_FIRST_ISLAND_VOYAGE_COMPLETE
SSTidalCorridor_EventScript_ArriveSouthernIsland::
	special ResetSSTidalFlag
	setvar VAR_SS_TIDAL_STATE, 0
	setflag FLAG_EC_FERRY_ARRIVAL_PENDING
	warp MAP_SOUTHERN_ISLAND_EXTERIOR, 13, 22
	waitstate
	release
	end

SSTidalCorridor_Text_ExpeditionGuide:
	.string "Our first stop is SOUTHERN ISLAND.\n"
	.string "Captain BRINEY waits at the north\l"
	.string "end of this corridor.\p"
	.string "Speak to him when you're ready\n"
	.string "to arrive. CABIN 2 has a healing bed;\l"
	.string "a passenger has a REAPER CLOTH gift.\p"
	.string "Explore and battle if you wish.\n"
	.string "No passenger battle is required\l"
	.string "before we land.$"

SSTidalCorridor_Text_ReadyForSouthernIsland:
	.string "BRINEY: PEEKO and I know these seas!\n"
	.string "The currents are changing, but the\l"
	.string "old chart still shows our course.\p"
	.string "Ready to land at SOUTHERN ISLAND?$"
