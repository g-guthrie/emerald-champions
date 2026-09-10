DewfordMeadow_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordMeadow_OnTransition
	.byte 0

DewfordMeadow_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_DEWFORD_MEADOW
	end

DewfordMeadow_EventScript_Warden::
	lockall
	faceplayer
	msgbox DewfordMeadow_Text_Warden, MSGBOX_DEFAULT
	setvar VAR_0x8004, LEGENDARY_SIGN_MELOETTA
	special TryUnlockLocalLegendaryDiscovery
	goto_if_eq VAR_RESULT, FALSE, DewfordMeadow_EventScript_Warden_Review
	msgbox DewfordMeadow_EventScript_Warden_Helped, MSGBOX_DEFAULT
DewfordMeadow_EventScript_Warden_Review::
	setvar VAR_0x8004, LEGENDARY_SIGN_MELOETTA
	special ResearchSelectedLegendarySign
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto Common_EventScript_LocalLegendaryResearch

DewfordMeadow_EventScript_Warden_Helped:
	.string "Your partner sings with the warden.\n"
	.string "A distant melody answers the song!$"

DewfordMeadow_Text_Warden:
	.string "A little COSMOG drifted into our\n"
	.string "meadow. Search the grass; you can\l"
	.string "meet it on your first visit!\p"
	.string "MELOETTA answers songs. With the\n"
	.string "KNUCKLE BADGE, bring a partner\l"
	.string "with SING and we'll perform\l"
	.string "together.\p"
	.string "The HEAT BADGE adds MUNKIDORI.\n"
	.string "Each discovery has a 1% chance;\l"
	.string "SWEET SCENT gives 25%, shared\l"
	.string "among those available.$"

DewfordMeadow_EventScript_ManorSign::
	msgbox DewfordMeadow_Text_ManorSign, MSGBOX_SIGN
	end

DewfordMeadow_Text_ManorSign:
	.string "DEWFORD MANOR: Once a sea captain's\n"
	.string "escape, now home only to POKéMON.$"
