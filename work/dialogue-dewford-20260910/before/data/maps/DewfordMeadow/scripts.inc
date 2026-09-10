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
	releaseall
	end

DewfordMeadow_EventScript_Warden_Helped:
	.string "Your partner sings with the warden.\n"
	.string "A distant melody answers the song!$"

DewfordMeadow_Text_Warden:
	.string "COSMOG and MUNKIDORI live in\n"
	.string "this meadow. Each has a 3%\l"
	.string "chance of appearing in the grass.\p"
	.string "MELOETTA answers songs. With the\n"
	.string "KNUCKLE BADGE, bring a partner\l"
	.string "with SING and we will perform\l"
	.string "together.\p"
	.string "Once it answers, you can search\n"
	.string "for it here. We only need to\l"
	.string "sing together once.\p"
	.string "SWEET SCENT helps find MELOETTA\n"
	.string "after our song. All available\l"
	.string "local legendaries share its\l"
	.string "25% chance.$"

DewfordMeadow_EventScript_ManorSign::
	msgbox DewfordMeadow_Text_ManorSign, MSGBOX_SIGN
	end

DewfordMeadow_Text_ManorSign:
	.string "DEWFORD MANOR\p"
	.string "A sea captain's retreat, later a\n"
	.string "home for MEGA EVOLUTION research.\p"
	.string "Please respect the POKéMON that\n"
	.string "now shelter inside.$"
