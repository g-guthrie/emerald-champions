FarawayIsland_Entrance_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, FarawayIsland_Entrance_OnArrivalFrame
	map_script MAP_SCRIPT_ON_TRANSITION, FarawayIsland_Entrance_OnTransition
	.byte 0

FarawayIsland_Entrance_OnTransition:
	setflag FLAG_ARRIVED_ON_FARAWAY_ISLAND
	end

FarawayIsland_Entrance_EventScript_SetCloudsWeather::
	setweather WEATHER_SUNNY_CLOUDS
	doweather
	end

FarawayIsland_Entrance_EventScript_ClearWeather::
	setweather WEATHER_NONE
	doweather
	end

FarawayIsland_Entrance_EventScript_Sailor::
	lock
	faceplayer
	call_if_set FLAG_EC_RESOLVED_MEW, FarawayIsland_Entrance_EventScript_JourneyComplete
	call_if_unset FLAG_EC_RESOLVED_MEW, FarawayIsland_Entrance_EventScript_RepeatMewGuide
	msgbox FarawayIsland_Entrance_Text_SailorReturn, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, FarawayIsland_Entrance_EventScript_AsYouLike
	msgbox EventTicket_Text_SailHome, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 30
	hideobjectat LOCALID_FARAWAY_ISLAND_SAILOR, MAP_FARAWAY_ISLAND_ENTRANCE
	setvar VAR_0x8004, LOCALID_FARAWAY_ISLAND_SS_TIDAL
	call Common_EventScript_FerryDepartIsland
	setflag FLAG_EC_FERRY_ARRIVAL_PENDING
	warp MAP_LILYCOVE_CITY_HARBOR, 8, 11
	waitstate
	release
	end

FarawayIsland_Entrance_EventScript_AsYouLike::
	msgbox EventTicket_Text_AsYouLike, MSGBOX_DEFAULT
	release
	end

FarawayIsland_Entrance_EventScript_Sign::
	msgbox FarawayIsland_Entrance_Text_Sign, MSGBOX_SIGN
	end


FarawayIsland_Entrance_OnArrivalFrame:
	map_script_2 VAR_TEMP_F, 0, FarawayIsland_Entrance_EventScript_Arrival
	.2byte 0

FarawayIsland_Entrance_EventScript_Arrival::
	setvar VAR_TEMP_F, 1
	goto_if_unset FLAG_EC_FERRY_ARRIVAL_PENDING, FarawayIsland_Entrance_EventScript_ArrivalEnd
	clearflag FLAG_EC_FERRY_ARRIVAL_PENDING
	lockall
	msgbox FarawayIsland_Entrance_Text_ArrivalGuide, MSGBOX_DEFAULT
	releaseall
FarawayIsland_Entrance_EventScript_ArrivalEnd::
	end

FarawayIsland_Entrance_Text_ArrivalGuide:
	.string "BRINEY: A place beyond our charts.\n"
	.string "Follow the path inland to the\l"
	.string "long-grass clearing.\p"
	.string "MEW may vanish from sight. Watch\n"
	.string "for moving grass and close off\l"
	.string "its path as you approach.\p"
	.string "When you're beside it, face it\n"
	.string "and press A. I'll wait at the\l"
	.string "ferry whenever you need to return.$"

FarawayIsland_Entrance_EventScript_JourneyComplete::
	msgbox FarawayIsland_Entrance_Text_BrineyConclusion, MSGBOX_DEFAULT
	return
FarawayIsland_Entrance_EventScript_RepeatMewGuide::
	msgbox FarawayIsland_Entrance_Text_ArrivalGuide, MSGBOX_DEFAULT
	return

FarawayIsland_Entrance_Text_BrineyConclusion:
	.string "BRINEY: A living piece of the old\n"
	.string "story. Imagine that.\p"
	.string "The account points to the Orb\n"
	.string "keepers next. From LILYCOVE, head\l"
	.string "west onto ROUTE 121.\p"
	.string "SURF south on ROUTE 122 to\n"
	.string "MT. PYRE. I'll put you ashore\l"
	.string "near LILYCOVE.$"
