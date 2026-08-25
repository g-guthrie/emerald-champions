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
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_4F_Pokemart_AttackTMs
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_4F_Pokemart_AttackTMs: @ 821FE20
	.2byte ITEM_NEVER_MELT_ICE
	.2byte ITEM_SPELL_TAG
	.2byte ITEM_TWISTED_SPOON
	.2byte ITEM_DRAGON_FANG
	.2byte ITEM_POISON_BARB
	.2byte ITEM_BLACK_GLASSES
	.2byte ITEM_NONE
	release
	end

LilycoveCity_DepartmentStore_4F_EventScript_ClerkRight:: @ 821FE2C
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_4F_Pokemart_DefenseTMs
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_4F_Pokemart_DefenseTMs: @ 821FE44
	.2byte ITEM_DAMP_ROCK
	.2byte ITEM_HEAT_ROCK
	.2byte ITEM_SMOOTH_ROCK
	.2byte ITEM_ICY_ROCK
	.2byte ITEM_NONE
	release
	end

LilycoveCity_DepartmentStore_4F_Text_AttackOrDefenseTM: @ 821FE50
	.string "Hmm…\p"
	.string "A type-boosting item…\n"
	.string "Or a weather-extending rock…\p"
	.string "Choosing the right held item can\n"
	.string "change an entire battle.$"

LilycoveCity_DepartmentStore_4F_Text_FiftyDifferentTMs: @ 821FEC7
	.string "There are so many different held\n"
	.string "items for team strategies.\p"
	.string "Weather teams alone can use four\n"
	.string "different kinds of rocks!$"

LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves: @ 821FF2D
	.string "A Pokémon can hold only one item.\n"
	.string "Picking the right one is hard!$"
