SouthernIsland_Exterior_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SouthernIsland_Exterior_OnArrivalFrame
	map_script MAP_SCRIPT_ON_TRANSITION, SouthernIsland_Exterior_OnTransition
	.byte 0

SouthernIsland_Exterior_OnTransition:
	setflag FLAG_LANDMARK_SOUTHERN_ISLAND
	end

SouthernIsland_Exterior_EventScript_Sailor::
	lock
	faceplayer
	goto_if_set FLAG_EC_RESOLVED_SOUTHERN_LATI, SouthernIsland_Exterior_EventScript_OfferFaraway
	goto_if_set FLAG_CAUGHT_LATIAS_OR_LATIOS, SouthernIsland_Exterior_EventScript_OfferFaraway
	msgbox SouthernIsland_Exterior_Text_ArrivalGuide, MSGBOX_DEFAULT
SouthernIsland_Exterior_EventScript_AskHome::
	msgbox EventTicket_Text_SouthernIslandSailBack, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SouthernIsland_Exterior_EventScript_AsYouLike
	msgbox EventTicket_Text_SailHome, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 30
	hideobjectat LOCALID_SOUTHERN_ISLAND_SAILOR, MAP_SOUTHERN_ISLAND_EXTERIOR
	setvar VAR_0x8004, LOCALID_SOUTHERN_ISLAND_SS_TIDAL
	call Common_EventScript_FerryDepartIsland
	setflag FLAG_EC_FERRY_ARRIVAL_PENDING
	warp MAP_LILYCOVE_CITY_HARBOR, 8, 11
	waitstate
	release
	end

SouthernIsland_Exterior_EventScript_AsYouLike::
	msgbox EventTicket_Text_AsYouLike, MSGBOX_DEFAULT
	release
	end

Ferry_EventScript_DepartIslandSouth::
	applymovement LOCALID_PLAYER, Ferry_Movement_DepartIslandBoardSouth
	waitmovement 0
	return

Ferry_EventScript_DepartIslandWest::
	applymovement LOCALID_PLAYER, Ferry_Movement_DepartIslandBoardWest
	waitmovement 0
	return

Ferry_Movement_DepartIslandBoardSouth:
	walk_down
	step_end

Ferry_Movement_DepartIslandBoardWest:
	walk_left
	walk_in_place_faster_down
	step_end

SouthernIsland_Exterior_EventScript_Sign::
	msgbox SouthernIsland_Exterior_Text_Sign, MSGBOX_SIGN
	end


SouthernIsland_Exterior_OnArrivalFrame:
	map_script_2 VAR_TEMP_F, 0, SouthernIsland_Exterior_EventScript_Arrival
	.2byte 0

SouthernIsland_Exterior_EventScript_Arrival::
	setvar VAR_TEMP_F, 1
	goto_if_unset FLAG_EC_FERRY_ARRIVAL_PENDING, SouthernIsland_Exterior_EventScript_ArrivalEnd
	clearflag FLAG_EC_FERRY_ARRIVAL_PENDING
	lockall
	msgbox SouthernIsland_Exterior_Text_ArrivalGuide, MSGBOX_DEFAULT
	releaseall
SouthernIsland_Exterior_EventScript_ArrivalEnd::
	end

SouthernIsland_Exterior_EventScript_OfferFaraway::
	msgbox SouthernIsland_Exterior_Text_SailFaraway, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SouthernIsland_Exterior_EventScript_AskHome
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 30
	hideobjectat LOCALID_SOUTHERN_ISLAND_SAILOR, MAP_SOUTHERN_ISLAND_EXTERIOR
	setvar VAR_0x8004, LOCALID_SOUTHERN_ISLAND_SS_TIDAL
	call Common_EventScript_FerryDepartIsland
	setflag FLAG_EC_FERRY_ARRIVAL_PENDING
	warp MAP_FARAWAY_ISLAND_ENTRANCE, 13, 38
	waitstate
	release
	end

SouthernIsland_Exterior_Text_ArrivalGuide:
	.string "BRINEY: SOUTHERN ISLAND. Quiet,\n"
	.string "isn't it? The old account begins\l"
	.string "with the sanctuary beyond these trees.\p"
	.string "Follow the path north and read\n"
	.string "the stone in the inner clearing.\l"
	.string "I'll wait here if you need passage.$"

SouthernIsland_Exterior_Text_ContinueJourney:
	.string "The sanctuary's guardian has\n"
	.string "answered. Capturing it is your choice.\p"
	.string "BRINEY can take you onward to\n"
	.string "FARAWAY ISLAND. Return south\l"
	.string "through the trees to the ferry.$"

SouthernIsland_Exterior_Text_SailFaraway:
	.string "BRINEY: The old chart leads on\n"
	.string "from here. Shall we sail to\l"
	.string "FARAWAY ISLAND?$"
