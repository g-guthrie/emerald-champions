LilycoveCity_DepartmentStore_4F_MapScripts::
	.byte 0

LilycoveCity_DepartmentStore_4F_EventScript_Gentleman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_AttackOrDefense, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Woman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_ManyDifferentMoves, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Youngster::
	msgbox LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_ClerkLeft::
	msgbox LilycoveCity_DepartmentStore_4F_Text_ClerkMoves, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_ClerkRight::
	msgbox LilycoveCity_DepartmentStore_4F_Text_ClerkSets, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_Text_AttackOrDefense:
	.string "Hmm…\p"
	.string "Immediate pressure…\n"
	.string "Or a defensive answer…\p"
	.string "A good set needs four moves that solve\n"
	.string "one plan without becoming predictable.$"

LilycoveCity_DepartmentStore_4F_Text_ManyDifferentMoves:
	.string "There are so many different kinds of\n"
	.string "moves.\p"
	.string "The POKéMON CENTER tutor can teach\n"
	.string "every move a species may legally learn.$"

LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves:
	.string "A POKéMON learns only four moves.\p"
	.string "Changing one can change the role of the\n"
	.string "whole team.$"

LilycoveCity_DepartmentStore_4F_Text_ClerkMoves:
	.string "Every POKéMON CENTER has a tutor who\n"
	.string "teaches legal moves without charging.\p"
	.string "Experiment freely. No TM purchase is\n"
	.string "needed.$"

LilycoveCity_DepartmentStore_4F_Text_ClerkSets:
	.string "The CENTER tutor also applies complete\n"
	.string "competitive sets: moves, Nature,\l"
	.string "Ability, Stat Points, and held item.\p"
	.string "Mega sets still require the matching\n"
	.string "stone from your adventure.$"
