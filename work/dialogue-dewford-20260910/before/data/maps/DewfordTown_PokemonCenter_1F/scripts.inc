DewfordTown_PokemonCenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordTown_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

DewfordTown_PokemonCenter_1F_OnTransition:
	setrespawn HEAL_LOCATION_DEWFORD_TOWN
	call Common_EventScript_UpdateBrineyLocation
	end

DewfordTown_PokemonCenter_1F_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_DEWFORD_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

DewfordTown_PokemonCenter_1F_EventScript_PokefanF::
	msgbox DewfordTown_PokemonCenter_1F_Text_StoneCavern, MSGBOX_NPC
	end

DewfordTown_PokemonCenter_1F_EventScript_Man::
	msgbox DewfordTown_PokemonCenter_1F_Text_FaintedMonCanUseHM, MSGBOX_NPC
	end

DewfordTown_PokemonCenter_1F_Text_StoneCavern:
	.string "Off to explore the cavern?\n"
	.string "There are rare stones in there.\p"
	.string "If you've earned the STONE BADGE,\n"
	.string "your level cap is now 20. Use\l"
	.string "the LEVELER before heading out!\p"
	.string "A new evolution may suit a new\n"
	.string "Battle Set. The TUTOR can help\l"
	.string "you prepare for BRAWLY, too.$"

DewfordTown_PokemonCenter_1F_Text_FaintedMonCanUseHM:
	.string "Once you have an HM and its\n"
	.string "BADGE, a compatible partner lets\l"
	.string "you use that field skill.\p"
	.string "It doesn't take up a battle move\n"
	.string "slot. You can even clear the way\l"
	.string "if that partner has fainted.$"

