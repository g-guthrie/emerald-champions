OldaleTown_PokemonCenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OldaleTown_PokemonCenter_1F_OnTransition
	.byte 0

OldaleTown_PokemonCenter_1F_OnTransition:
	setrespawn HEAL_LOCATION_OLDALE_TOWN
	call Common_EventScript_UpdateBrineyLocation
	end

OldaleTown_PokemonCenter_1F_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_OLDALE_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

OldaleTown_PokemonCenter_1F_EventScript_Gentleman::
	msgbox OldaleTown_PokemonCenter_1F_Text_TrainersCanUsePC, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_Boy::
	msgbox OldaleTown_PokemonCenter_1F_Text_PokemonCentersAreGreat, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_Girl::
	lock
	faceplayer
	msgbox OldaleTown_PokemonCenter_1F_Text_WirelessClubNotAvailable, MSGBOX_DEFAULT
	release
	end

OldaleTown_PokemonCenter_1F_Text_TrainersCanUsePC:
	.string "Planning your team at the PC?\n"
	.string "Try the TUTOR beside it, too.\p"
	.string "Ask for a Battle Set: a prepared\n"
	.string "combination of moves, Ability,\l"
	.string "Nature, EVs, and a held item.\p"
	.string "Choose Doubles for team battles.\n"
	.string "You can preview a set, then\l"
	.string "decide whether to apply it.\p"
	.string "It's free, and you can change\n"
	.string "the details afterward. Think of\l"
	.string "it as a starting point!$"

OldaleTown_PokemonCenter_1F_Text_PokemonCentersAreGreat:
	.string "I keep my POKé VIAL handy.\n"
	.string "One use restores the whole party\l"
	.string "outside battle!\p"
	.string "The nurse refills its one dose\n"
	.string "whenever I stop for healing.$"

OldaleTown_PokemonCenter_1F_Text_WirelessClubNotAvailable:
	.string "The TUTOR and the VENDOR beside\n"
	.string "the PC cost nothing at all.\p"
	.string "I come in here to rebuild a team\n"
	.string "before every route.$"

