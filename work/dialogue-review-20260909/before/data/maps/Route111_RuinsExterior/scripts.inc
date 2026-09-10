Route111_RuinsExterior_MapScripts::
	.byte 0

Route111_RuinsExterior_EventScript_Archaeologist::
	lockall
	faceplayer
	msgbox Route111_RuinsExterior_Text_Archaeologist, MSGBOX_DEFAULT
	setvar VAR_0x8004, LEGENDARY_SIGN_LANDORUS
	special TryUnlockLocalLegendaryDiscovery
	goto_if_eq VAR_RESULT, FALSE, Route111_RuinsExterior_EventScript_Archaeologist_Review
	msgbox Route111_RuinsExterior_EventScript_Archaeologist_Helped, MSGBOX_DEFAULT
Route111_RuinsExterior_EventScript_Archaeologist_Review::
	setvar VAR_0x8004, LEGENDARY_SIGN_LANDORUS
	special ResearchSelectedLegendarySign
	msgbox gStringVar4, MSGBOX_DEFAULT
	releaseall
	end

Route111_RuinsExterior_EventScript_Archaeologist_Helped:
	.string "CASTFORM helps record the weather.\n"
	.string "The wind settles over the ruins.$"

Route111_RuinsExterior_Text_Archaeologist:
	.string "The storms leave strange patterns\n"
	.string "across these ruins. LANDORUS may\l"
	.string "be sheltering here.\p"
	.string "With the BALANCE BADGE, bring\n"
	.string "CASTFORM and help me survey the\l"
	.string "weather.\p"
	.string "Then LANDORUS can appear among\n"
	.string "the wild POKéMON here. Our work\l"
	.string "only needs doing once.$"
