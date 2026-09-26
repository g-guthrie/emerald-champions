OldaleTown_House2_MapScripts:: @ 81FBF5A
	.byte 0

OldaleTown_House2_EventScript_Woman:: @ 81FBF5B
	msgbox OldaleTown_House2_Text_PokemonLevelUp, MSGBOX_NPC
	end

OldaleTown_House2_EventScript_Man:: @ 81FBF64
	msgbox OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon, MSGBOX_NPC
	end

OldaleTown_House2_Text_PokemonLevelUp: @ 81FBF6D
	.string "Battles don't make Pokémon stronger\n"
	.string "here. Your Badges set how high\l"
	.string "they can grow.\p"
	.string "The nurse's Leveler can bring your\n"
	.string "whole party up that high at once!$"

OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon: @ 81FBFB0
	.string "If the Pokémon with you become\n"
	.string "stronger, you'll be able to go farther\l"
	.string "away from here.$"
