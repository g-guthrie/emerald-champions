OldaleTown_House2_MapScripts::
	.byte 0

OldaleTown_House2_EventScript_Woman::
	msgbox OldaleTown_House2_Text_PokemonLevelUp, MSGBOX_NPC
	end

OldaleTown_House2_EventScript_Man::
	msgbox OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon, MSGBOX_NPC
	end

OldaleTown_House2_Text_PokemonLevelUp:
	.string "POKéMON grow stronger as they\n"
	.string "gain levels.\p"
	.string "The CENTER's free LEVELER brings\n"
	.string "your whole party to the current\l"
	.string "limit. It's handy when a new\l"
	.string "partner joins!$"

OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon:
	.string "Get to know what each partner\n"
	.string "does well.\p"
	.string "A new move or a different\n"
	.string "teammate can make all the\l"
	.string "difference in a tough battle.$"
