ScorchedSlab_B2F_MapScripts::
	.byte 0

ScorchedSlab_B2F_EventScript_Warden::
	lock
	faceplayer
	setvar VAR_0x8004, LEGENDARY_SIGN_RESHIRAM
	specialvar VAR_RESULT, GetSelectedLegendarySignState
	goto_if_eq VAR_RESULT, 2, ScorchedSlab_B2F_EventScript_ReshiramCaught
	msgbox ScorchedSlab_B2F_Text_Warden, MSGBOX_DEFAULT
	goto ScorchedSlab_B2F_EventScript_HeatranClue

ScorchedSlab_B2F_EventScript_ReshiramCaught::
	msgbox ScorchedSlab_B2F_Text_ReshiramCaught, MSGBOX_DEFAULT
ScorchedSlab_B2F_EventScript_HeatranClue::
	specialvar VAR_RESULT, GetHeatranDiscoveryState
	goto_if_eq VAR_RESULT, 2, ScorchedSlab_B2F_EventScript_HeatranCaught
	goto_if_eq VAR_RESULT, 1, ScorchedSlab_B2F_EventScript_HeatranAwake
	msgbox ScorchedSlab_B2F_Text_HeatranClue, MSGBOX_DEFAULT
	release
	end

ScorchedSlab_B2F_Text_Warden:
	.string "A white dragon has been glimpsed\n"
	.string "among these hot stones. RESHIRAM!\p"
	.string "The sign lists the wild POKéMON\n"
	.string "here, including that rare dragon.$"

ScorchedSlab_B2F_Text_ReshiramCaught:
	.string "You've befriended RESHIRAM?\n"
	.string "Then the white dragon's journey\l"
	.string "continues beyond these stones.$"

ScorchedSlab_B2F_Text_HeatranClue:
	.string "HEATRAN sleeps farther below.\p"
	.string "They say a MAGMA STONE can\n"
	.string "awaken it. Search the MAGMA\l"
	.string "HIDEOUT beneath MT. CHIMNEY.$"

ScorchedSlab_B2F_EventScript_HeatranCaught:
	msgbox ScorchedSlab_B2F_Text_HeatranCaught, MSGBOX_DEFAULT
	release
	end
ScorchedSlab_B2F_EventScript_HeatranAwake:
	msgbox ScorchedSlab_B2F_Text_HeatranAwake, MSGBOX_DEFAULT
	release
	end
ScorchedSlab_B2F_Text_HeatranCaught:
	.string "HEATRAN travels with you, too?\n"
	.string "These caves have shared quite a secret!$"
ScorchedSlab_B2F_Text_HeatranAwake:
	.string "HEATRAN is awake in the room below.\p"
	.string "If it retreats after a battle, leave\n"
	.string "that room and return to find it.$"
