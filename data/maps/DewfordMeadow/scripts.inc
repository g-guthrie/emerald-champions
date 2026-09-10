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
	goto_if_set FLAG_EC_CAUGHT_MELOETTA, DewfordMeadow_EventScript_MeloettaCaught
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
	.string "Our meadow shelters some\n"
	.string "remarkable visitors. Check its\l"
	.string "sign for the local POKéMON.\p"
	.string "COSMOG and MUNKIDORI are rare\n"
	.string "wild residents. A caught legend\l"
	.string "stops appearing; the sign\l"
	.string "records it as Caught.\p"
	.string "MELOETTA answers songs. With the\n"
	.string "KNUCKLE BADGE, bring a partner\l"
	.string "with SING and we can perform.\p"
	.string "We only need to sing once.\n"
	.string "After it answers, search here.\l"
	.string "Let me check our field notes.$"

DewfordMeadow_EventScript_ManorSign::
	msgbox DewfordMeadow_Text_ManorSign, MSGBOX_SIGN
	end

DewfordMeadow_Text_ManorSign:
	.string "DEWFORD MANOR\p"
	.string "A sea captain's retreat, later a\n"
	.string "home for MEGA EVOLUTION research.\p"
	.string "Please respect the POKéMON that\n"
	.string "now shelter inside.$"

DewfordMeadow_EventScript_MeloettaCaught::
	msgbox DewfordMeadow_Text_MeloettaCaught, MSGBOX_DEFAULT
	releaseall
	end

DewfordMeadow_Text_MeloettaCaught:
	.string "MELOETTA has joined your journey!\n"
	.string "I'll remember our song for years.\p"
	.string "Check the meadow sign for any\n"
	.string "other discoveries still waiting.\l"
	.string "Your catches are recorded there.$"
