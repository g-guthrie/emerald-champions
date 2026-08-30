SootopolisCity_House1_MapScripts::
	.byte 0

SootopolisCity_House1_EventScript_RazorClawBlackBelt::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_RAZOR_CLAW, SootopolisCity_House1_EventScript_ReceivedRazorClaw
	msgbox SootopolisCity_House1_Text_FoundThisRazorClaw, MSGBOX_DEFAULT
	giveitem ITEM_RAZOR_CLAW
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_RAZOR_CLAW
	msgbox SootopolisCity_House1_Text_ExplainRazorClaw, MSGBOX_DEFAULT
	release
	end

SootopolisCity_House1_EventScript_ReceivedRazorClaw::
	msgbox SootopolisCity_House1_Text_ExplainRazorClaw, MSGBOX_DEFAULT
	release
	end

SootopolisCity_House1_EventScript_Kecleon::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_KECLEON, CRY_MODE_NORMAL
	msgbox SootopolisCity_House1_Text_Kecleon, MSGBOX_DEFAULT
	waitmoncry
	release
	end
SootopolisCity_House1_Text_FoundThisRazorClaw:
	.string "For thirty years I've remained in\n"
	.string "SOOTOPOLIS honing my skills.\p"
	.string "This RAZOR CLAW is just as sharp.\n"
	.string "I bequeath it to you!$"
SootopolisCity_House1_Text_ExplainRazorClaw:
	.string "A Razor Claw evolves Sneasel.\n"
	.string "Use it at night when ready.$"

SootopolisCity_House1_Text_Kecleon:
	.string "KECLEON: Puu puhyaah.$"
