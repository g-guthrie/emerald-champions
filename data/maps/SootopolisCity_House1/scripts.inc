SootopolisCity_House1_MapScripts:: @ 822694C
	.byte 0

SootopolisCity_House1_EventScript_BrickBreakBlackBelt:: @ 822694D
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TM75, SootopolisCity_House1_EventScript_ReceivedBrickBreak
	msgbox SootopolisCity_House1_Text_DevelopedThisTM, MSGBOX_DEFAULT
	giveitem ITEM_SCOPE_LENS
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

SootopolisCity_House1_Text_DevelopedThisTM: @ 82269A1
	.string "For thirty years I've remained in\n"
	.string "Sootopolis honing my skills.\p"
	.string "I perfected this Scope Lens.\n"
	.string "I bequeath it to you!$"

SootopolisCity_House1_Text_ExplainBrickBreak: @ 8226A13
	.string "A Scope Lens raises the holder's\n"
	.string "critical-hit ratio. Use it well!$"

SootopolisCity_House1_Text_Kecleon: @ 8226A60
	.string "Kecleon: Puu puhyaah.$"
