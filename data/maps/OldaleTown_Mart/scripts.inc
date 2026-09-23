OldaleTown_Mart_MapScripts:: @ 81FC23F
	.byte 0

OldaleTown_Mart_EventScript_Woman::
	msgbox OldaleTown_Mart_Text_BuyPokeBallsAtCenter, MSGBOX_NPC
	end

OldaleTown_Mart_EventScript_Boy:: @ 81FC2A9
	msgbox OldaleTown_Mart_Text_RestoreHPWithPotion, MSGBOX_NPC
	end

OldaleTown_Mart_Text_BuyPokeBallsAtCenter:
	.string "This Mart sells held items.\p"
	.string "I'm going to the Pokémon Center\n"
	.string "to buy Poké Balls for catching!$"

OldaleTown_Mart_Text_RestoreHPWithPotion: @ 81FC338
	.string "If a Pokémon gets hurt and loses its HP\n"
	.string "and faints, it won't be able to battle.\p"
	.string "To prevent your Pokémon from fainting,\n"
	.string "restore its HP with a Potion.$"
