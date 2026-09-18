@ ---------------------------------------------------------------------------
@ Minimal Match Call text stub.
@
@ The Match Call subsystem (src/match_call.c) is deleted from this engine, so
@ the donor's full data/text/match_call.inc was deliberately NOT imported.
@ These two strings are the only Match Call texts still reached by a live
@ script: EventScript_RegisterProfBirch in
@ data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc registers Prof. Birch
@ during the main story. Restored verbatim from Inclement Emerald so that
@ script links.
@
@ If more Match Call text symbols are ever needed, append them here from the
@ donor rather than importing the whole file.
@ ---------------------------------------------------------------------------

MatchCall_Text_BirchRegisterCall: @ 82B5F52
	.string "Prof. Birch: Oh, {PLAYER}{KUN}!\n"
	.string "I've already heard about you!\p"
	.string "It seems your PokéNav's been\n"
	.string "upgraded with Match Call.\p"
	.string "Well, I should register, too!\p"
	.string "That way, I'd be able to examine\n"
	.string "your Pokédex even while you're\l"
	.string "out in the field.\p"
	.string "… … … … … …$"

MatchCall_Text_RegisteredBirch: @ 82B603A
	.string "Registered Prof. Birch\n"
	.string "in the PokéNav.$"
