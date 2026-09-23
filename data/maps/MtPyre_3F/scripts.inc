MtPyre_3F_MapScripts:: @ 8231752
	.byte 0

MtPyre_3F_EventScript_William:: @ 8231753
	@ This retired encounter has no campaign battle.
	msgbox MtPyre_3F_Text_WilliamPostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_3F_EventScript_Gabrielle:: @ 8231781
	trainerbattle_double TRAINER_GABRIELLE_1, MtPyre_3F_Text_GabrielleIntro, MtPyre_3F_Text_GabrielleDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MtPyre_3F_Text_GabriellePostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_3F_Text_WilliamPostBattle: @ 8231869
	.string "My psychic powers have surely\n"
	.string "grown several times, but…$"

MtPyre_3F_Text_GabrielleIntro: @ 8231952
	.string "Why have you come here?$"

MtPyre_3F_Text_GabrielleDefeat: @ 823196A
	.string "That was amazing!\n"
	.string "You're a very special Trainer.$"

MtPyre_3F_Text_GabriellePostBattle: @ 823199B
	.string "Pokémon no longer of this world.\n"
	.string "Pokémon that are with you now.\p"
	.string "And the Pokémon that you will meet\n"
	.string "in the future…\p"
	.string "They are all to be equally cherished.\n"
	.string "Please remember that.$"
