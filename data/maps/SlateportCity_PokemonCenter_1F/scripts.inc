SlateportCity_PokemonCenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SlateportCity_PokemonCenter_1F_OnTransition
	.byte 0

SlateportCity_PokemonCenter_1F_OnTransition:
	setrespawn HEAL_LOCATION_SLATEPORT_CITY
	call Common_EventScript_UpdateBrineyLocation
	end

SlateportCity_PokemonCenter_1F_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_SLATEPORT_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

SlateportCity_PokemonCenter_1F_EventScript_Sailor::
	lock
	faceplayer
	goto_if_set FLAG_DELIVERED_DEVON_GOODS, SlateportCity_PokemonCenter_1F_EventScript_RoadToMauville
	goto_if_set FLAG_DOCK_REJECTED_DEVON_GOODS, SlateportCity_PokemonCenter_1F_EventScript_FindStern
	msgbox SlateportCity_PokemonCenter_1F_Text_FindDock, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonCenter_1F_EventScript_FindStern::
	msgbox SlateportCity_PokemonCenter_1F_Text_FindStern, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonCenter_1F_EventScript_RoadToMauville::
	msgbox SlateportCity_PokemonCenter_1F_Text_RoadToMauville, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonCenter_1F_EventScript_Woman::
	msgbox SlateportCity_PokemonCenter_1F_Text_TradedMonWithFriend, MSGBOX_NPC
	end

SlateportCity_PokemonCenter_1F_Text_FindDock:
	.string "Looking for CAPT. STERN?\n"
	.string "Ask DOCK at the SHIPYARD by the\l"
	.string "southeast waterfront.\p"
	.string "The MART sells evolution stones\n"
	.string "and LINKING CORDS if you need\l"
	.string "them. No need to shop first!$"

SlateportCity_PokemonCenter_1F_Text_FindStern:
	.string "CAPT. STERN is upstairs in the\n"
	.string "OCEANIC MUSEUM. Take your\l"
	.string "DEVON delivery to him there.$"

SlateportCity_PokemonCenter_1F_Text_RoadToMauville:
	.string "Heading inland? Take ROUTE 110\n"
	.string "north to MAUVILLE CITY.\p"
	.string "There's a GYM and a bike shop\n"
	.string "waiting for you there.$"

SlateportCity_PokemonCenter_1F_Text_TradedMonWithFriend:
	.string "I trade POKéMON with my friends.\p"
	.string "If a traded POKéMON is holding an\n"
	.string "item, it makes me twice as happy!$"
