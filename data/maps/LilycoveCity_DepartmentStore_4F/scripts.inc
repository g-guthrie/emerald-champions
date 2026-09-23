LilycoveCity_DepartmentStore_4F_MapScripts:: @ 821FDEA
	.byte 0

LilycoveCity_DepartmentStore_4F_EventScript_Gentleman:: @ 821FDEB
	msgbox LilycoveCity_DepartmentStore_4F_Text_AttackOrDefenseTM, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Woman:: @ 821FDF4
	msgbox LilycoveCity_DepartmentStore_4F_Text_FiftyDifferentTMs, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Youngster:: @ 821FDFD
	msgbox LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_ClerkLeft:: @ 821FE06
	lock
	faceplayer
	msgbox LilycoveCity_DepartmentStore_4F_Text_Stones, MSGBOX_DEFAULT
	setvar VAR_0x8004, 0
	special OpenEmeraldChampionsEvolutionSpecialist
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LilycoveCity_DepartmentStore_4F_EventScript_ClerkRight:: @ 821FE2C
	lock
	faceplayer
	msgbox LilycoveCity_DepartmentStore_4F_Text_Tools, MSGBOX_DEFAULT
	setvar VAR_0x8004, 1
	special OpenEmeraldChampionsEvolutionSpecialist
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LilycoveCity_DepartmentStore_4F_Text_Stones:
	.string "We stock every evolution stone!\n"
	.string "Which one does your partner need?$"

LilycoveCity_DepartmentStore_4F_Text_Tools:
	.string "Special evolution tools and\n"
	.string "reusable devices, all in stock!\p"
	.string "A reusable device only needs to be\n"
	.string "bought once. Check its description!$"

LilycoveCity_DepartmentStore_4F_Text_AttackOrDefenseTM: @ 821FE50
	.string "Should I evolve my partner now,\n"
	.string "or wait a little longer?\p"
	.string "There are so many choices here.$"

LilycoveCity_DepartmentStore_4F_Text_FiftyDifferentTMs: @ 821FEC7
	.string "Evolution items help all sorts\n"
	.string "of Pokémon grow.\p"
	.string "Check what your partner needs\n"
	.string "before buying one.$"

LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves: @ 821FF2D
	.string "The Center tutor offers plenty\n"
	.string "of moves, but a Pokémon can use\l"
	.string "only four at once.\p"
	.string "I choose them to suit the whole\n"
	.string "team.$"

