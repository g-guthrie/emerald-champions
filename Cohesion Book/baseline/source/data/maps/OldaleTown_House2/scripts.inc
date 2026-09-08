OldaleTown_House2_MapScripts::
	.byte 0

OldaleTown_House2_EventScript_Woman::
	msgbox OldaleTown_House2_Text_PokemonLevelUp, MSGBOX_NPC
	end

OldaleTown_House2_EventScript_Man::
	msgbox OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon, MSGBOX_NPC
	end

OldaleTown_House2_Text_PokemonLevelUp:
	.string "POKéMON grow from battles, but you\n"
	.string "never need to grind.\p"
	.string "A POKéMON CENTER LEVELER trains your\n"
	.string "whole party to the current limit.$"

OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon:
	.string "Raw levels alone won't take you far.\p"
	.string "Build different partners for different\n"
	.string "battles, then change plans when needed.$"
