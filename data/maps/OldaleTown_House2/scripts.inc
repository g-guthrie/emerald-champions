OldaleTown_House2_MapScripts::
	.byte 0

OldaleTown_House2_EventScript_Woman::
	msgbox OldaleTown_House2_Text_PokemonLevelUp, MSGBOX_NPC
	end

OldaleTown_House2_EventScript_Man::
	msgbox OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon, MSGBOX_NPC
	end

OldaleTown_House2_Text_PokemonLevelUp:
	.string "Battles test your team here.\n"
	.string "They don't give experience.\p"
	.string "Use the free LEVELER from the\n"
	.string "CENTER to raise your whole party\l"
	.string "to the current level cap.\p"
	.string "BADGES raise that cap. Use it\n"
	.string "again when a new partner joins!\p"
	.string "It also checks for evolutions.\n"
	.string "If you meet a new requirement,\l"
	.string "try it again, even at the cap.$"

OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon:
	.string "A new move can make an old\n"
	.string "partner feel completely different!\p"
	.string "The TUTOR by the CENTER PC can\n"
	.string "teach your partner its available\l"
	.string "moves for free.\p"
	.string "You don't need to wait for\n"
	.string "level-up moves. Visit again\l"
	.string "after your partner evolves!$"
