RustboroCity_CuttersHouse_MapScripts:: @ 8215BD3
	.byte 0

RustboroCity_CuttersHouse_EventScript_Cutter:: @ 8215BD4
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_HM01, RustboroCity_CuttersHouse_EventScript_ExplainCut
	msgbox RustboroCity_CuttersHouse_Text_YouCanPutThisHMToGoodUse, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_HM01
	msgbox RustboroCity_CuttersHouse_Text_ExplainCut, MSGBOX_DEFAULT
	release
	end

RustboroCity_CuttersHouse_EventScript_ExplainCut:: @ 8215C00
	msgbox RustboroCity_CuttersHouse_Text_ExplainCut, MSGBOX_DEFAULT
	release
	end

RustboroCity_CuttersHouse_EventScript_Lass:: @ 8215C0A
	msgbox RustboroCity_CuttersHouse_Text_DadHelpedClearLandOfTrees, MSGBOX_NPC
	end

RustboroCity_CuttersHouse_Text_YouCanPutThisHMToGoodUse: @ 8215C13
	.string "That determined expression...\n"
	.string "You look ready to learn how to\l"
	.string "clear a path.\p"
	.string "I authorize you to use Cut!$"

RustboroCity_CuttersHouse_Text_ExplainCut: @ 8215D33
	.string "With the Stone Badge and this\n"
	.string "license, approach a thin tree\l"
	.string "and press A to cut it down.\p"
	.string "Your Pokémon do not need to\n"
	.string "learn Cut.$"

RustboroCity_CuttersHouse_Text_DadHelpedClearLandOfTrees: @ 8215E39
	.string "When they were expanding the city of\n"
	.string "Rustboro, my dad helped out.\p"
	.string "He made his Pokémon use Cut to clear\n"
	.string "the land of trees.$"

