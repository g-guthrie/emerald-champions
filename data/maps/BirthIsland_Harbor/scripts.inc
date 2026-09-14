BirthIsland_Harbor_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, BirthIsland_Harbor_OnArrivalFrame
	.byte 0

BirthIsland_Harbor_EventScript_Sailor::
	lock
	faceplayer
	call_if_set FLAG_EC_REPORT_C43_COMPLETE, BirthIsland_Harbor_EventScript_RepeatScannerLead
	call_if_unset FLAG_EC_REPORT_C43_COMPLETE, BirthIsland_Harbor_EventScript_RepeatIslandGuide
	msgbox BirthIsland_Harbor_Text_SailorReturn, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, BirthIsland_Harbor_EventScript_AsYouLike
	msgbox EventTicket_Text_SailHome, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	delay 30
	hideobjectat LOCALID_BIRTH_ISLAND_SAILOR, MAP_BIRTH_ISLAND_HARBOR
	setvar VAR_0x8004, LOCALID_BIRTH_ISLAND_SS_TIDAL
	call Common_EventScript_FerryDepartIsland
	setflag FLAG_EC_FERRY_ARRIVAL_PENDING
	warp MAP_LILYCOVE_CITY_HARBOR, 8, 11
	waitstate
	release
	end

BirthIsland_Harbor_EventScript_AsYouLike::
	msgbox EventTicket_Text_AsYouLike, MSGBOX_DEFAULT
	release
	end


BirthIsland_Harbor_OnArrivalFrame:
	map_script_2 VAR_TEMP_F, 0, BirthIsland_Harbor_EventScript_Arrival
	.2byte 0

BirthIsland_Harbor_EventScript_Arrival::
	setvar VAR_TEMP_F, 1
	goto_if_unset FLAG_EC_FERRY_ARRIVAL_PENDING, BirthIsland_Harbor_EventScript_ArrivalEnd
	clearflag FLAG_EC_FERRY_ARRIVAL_PENDING
	lockall
	msgbox BirthIsland_Harbor_Text_ArrivalGuide, MSGBOX_DEFAULT
	releaseall
BirthIsland_Harbor_EventScript_ArrivalEnd::
	end

BirthIsland_Harbor_Text_ArrivalGuide:
	.string "The disturbance is at the\n"
	.string "triangle outside this harbor.\p"
	.string "Reach each new triangle position\n"
	.string "in the fewest steps, then face\l"
	.string "it and press A. Extra steps reset it.\p"
	.string "The sailor stays here for your\n"
	.string "return trip, whatever happens.$"

BirthIsland_Harbor_Text_ScannerLead:
	.string "The meteorite lead is resolved.\n"
	.string "Next, visit the ABANDONED SHIP\l"
	.string "on ROUTE 108.\p"
	.string "Bring licensed SURF and DIVE.\n"
	.string "Recover its SCANNER and take it\l"
	.string "to STERN at SLATEPORT HARBOR.$"

BirthIsland_Harbor_EventScript_RepeatScannerLead::
	msgbox BirthIsland_Harbor_Text_ScannerLead, MSGBOX_DEFAULT
	return
BirthIsland_Harbor_EventScript_RepeatIslandGuide::
	msgbox BirthIsland_Harbor_Text_ArrivalGuide, MSGBOX_DEFAULT
	return
