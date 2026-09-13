RustboroCity_PokemonCenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RustboroCity_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

RustboroCity_PokemonCenter_1F_OnTransition:
	setrespawn HEAL_LOCATION_RUSTBORO_CITY
	call Common_EventScript_UpdateBrineyLocation
	end

RustboroCity_PokemonCenter_1F_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_RUSTBORO_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

RustboroCity_PokemonCenter_1F_EventScript_Man::
	msgbox RustboroCity_PokemonCenter_1F_Text_PokemonHavePersonalities, MSGBOX_NPC
	end

RustboroCity_PokemonCenter_1F_EventScript_Boy::
	msgbox RustboroCity_PokemonCenter_1F_Text_MaleAndFemalePokemon, MSGBOX_NPC
	end

RustboroCity_PokemonCenter_1F_EventScript_Girl::
	msgbox RustboroCity_PokemonCenter_1F_Text_HMCutNextDoor, MSGBOX_NPC
	end

RustboroCity_PokemonCenter_1F_Text_PokemonHavePersonalities:
	.string "My partner's JOLLY Nature raises\n"
	.string "Speed but lowers Sp. Atk. That\l"
	.string "suits its physical moves!\p"
	.string "The TUTOR can change the Nature\n"
	.string "that affects your stats for free.\l"
	.string "Each choice shows its tradeoff.\p"
	.string "IVs are fixed at the best value,\n"
	.string "31, for everyone. You don't need\l"
	.string "to breed for better stats.\p"
	.string "To change an available Ability,\n"
	.string "select your POKéMON in the party\l"
	.string "menu and choose Ability.$"

RustboroCity_PokemonCenter_1F_Text_MaleAndFemalePokemon:
	.string "Some POKéMON are male or female.\n"
	.string "Others have no gender.\p"
	.string "Sometimes it even changes how\n"
	.string "they evolve! Female BURMY become\l"
	.string "WORMADAM; males become MOTHIM.$"

RustboroCity_PokemonCenter_1F_Text_HMCutNextDoor:
	.string "The man next door gave me CUT!\p"
	.string "With the STONE BADGE and a\n"
	.string "compatible partner, I can clear\l"
	.string "skinny trees. It doesn't take up\l"
	.string "a battle move slot.$"

RustboroCity_PokemonCenter_1F_EventScript_Clerk2::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart PokeMart_PokeCenterRustboro
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PokeMart_PokeCenterRustboro::
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_NONE
