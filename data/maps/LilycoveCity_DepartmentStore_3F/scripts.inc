LilycoveCity_DepartmentStore_3F_MapScripts:: @ 821FC64
	.byte 0

LilycoveCity_DepartmentStore_3F_EventScript_ClerkLeft:: @ 821FC65
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_3F_Pokemart_PPBoosters
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	@ PP Boosters (the 1F floor sign): no other counter sells them for money.
	.align 2
LilycoveCity_DepartmentStore_3F_Pokemart_PPBoosters:
	.2byte ITEM_PP_UP
	.2byte ITEM_PP_MAX
	.2byte ITEM_NONE
	release
	end

LilycoveCity_DepartmentStore_3F_EventScript_TriathleteM:: @ 821FCB6
	msgbox LilycoveCity_DepartmentStore_3F_Text_PPUpForFewPPMoves, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_3F_EventScript_PokefanM:: @ 821FCBF
	msgbox LilycoveCity_DepartmentStore_3F_Text_WantMoreEndurance, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_3F_EventScript_Woman:: @ 821FCC8
	msgbox LilycoveCity_DepartmentStore_3F_Text_SwitchedAbilityForFree, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_3F_Text_PPUpForFewPPMoves:
	.string "A PP Up lets a move be used a few\n"
	.string "more times in every battle.\p"
	.string "I save mine for strong moves that\n"
	.string "run out of PP too fast!$"

LilycoveCity_DepartmentStore_3F_Text_WantMoreEndurance:
	.string "I want my Pokémon to have more\n"
	.string "endurance.\p"
	.string "Should its EVs go into Defense,\n"
	.string "or into Sp. Def?\p"
	.string "The move tutor can always change\n"
	.string "them later, so I can't go wrong!$"

LilycoveCity_DepartmentStore_3F_Text_SwitchedAbilityForFree:
	.string "I switched my Pokémon's Ability\n"
	.string "right from the party menu.\p"
	.string "It didn't cost a thing!$"
