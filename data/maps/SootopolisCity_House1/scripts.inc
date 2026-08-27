SootopolisCity_House1_MapScripts:: @ 822694C
	.byte 0

SootopolisCity_House1_EventScript_BrickBreakBlackBelt:: @ 822694D
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TM75, SootopolisCity_House1_EventScript_ReceivedBrickBreak
	msgbox SootopolisCity_House1_Text_DevelopedThisTM, MSGBOX_DEFAULT
	giveitem ITEM_RAZOR_CLAW
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM75
	msgbox SootopolisCity_House1_Text_ExplainBrickBreak, MSGBOX_DEFAULT
	release
	end

SootopolisCity_House1_EventScript_ReceivedBrickBreak:: @ 8226984
	msgbox SootopolisCity_House1_Text_ExplainBrickBreak, MSGBOX_DEFAULT
	release
	end

SootopolisCity_House1_EventScript_Kecleon:: @ 822698E
	lock
	faceplayer
	waitse
	playmoncry SPECIES_KECLEON, 0
	msgbox SootopolisCity_House1_Text_Kecleon, MSGBOX_DEFAULT
	waitmoncry
	release
	end
SootopolisCity_House1_Text_DevelopedThisTM:
	.string "I found this Razor Claw training.\n"
	.string "You should have it.$"
SootopolisCity_House1_Text_ExplainBrickBreak:
	.string "A Razor Claw evolves Sneasel.\p"
	.string "Level it at night while holding it.$"

SootopolisCity_House1_Text_Kecleon: @ 8226A60
	.string "Kecleon: Puu puhyaah.$"
