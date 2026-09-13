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
	setvar VAR_0x8004, ITEM_MEGA_RING
	special CheckEmeraldChampionsHandoffItem
	goto_if_eq VAR_RESULT, FALSE, DewfordMeadow_EventScript_NeedRing
	goto_if_set FLAG_EC_MEADOW_SONG_PERFORMED, DewfordMeadow_EventScript_AfterSong
	special PrepareEmeraldChampionsMeadowSong
	goto_if_eq VAR_RESULT, FALSE, DewfordMeadow_EventScript_NeedSinger
	msgbox DewfordMeadow_Text_ReadyToSing, MSGBOX_DEFAULT
	closemessage
	dofieldeffect FLDEFF_FIELD_MOVE_SHOW_MON_INIT
	waitfieldeffect FLDEFF_FIELD_MOVE_SHOW_MON
	playse SE_M_SING
	waitse
	delay 24
	playmoncry SPECIES_MELOETTA, CRY_MODE_NORMAL
	waitmoncry
	applymovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	waitmovement 0
	msgbox DewfordMeadow_Text_MelodyAnswers, MSGBOX_DEFAULT
	setvar VAR_0x8004, LEGENDARY_SIGN_MELOETTA
	special TryUnlockLocalLegendaryDiscovery
	setflag FLAG_EC_MEADOW_SONG_PERFORMED
DewfordMeadow_EventScript_AfterSong::
	goto_if_unset FLAG_EC_MANOR_NOTES_READ, DewfordMeadow_EventScript_NeedAccount
	call DewfordManor_1F_EventScript_RecordResearch
	msgbox DewfordMeadow_Text_ChapterComplete, MSGBOX_DEFAULT
	releaseall
	end

DewfordMeadow_EventScript_NeedRing::
	msgbox DewfordMeadow_Text_NeedRing, MSGBOX_DEFAULT
	releaseall
	end
DewfordMeadow_EventScript_NeedSinger::
	msgbox DewfordMeadow_Text_NeedSinger, MSGBOX_DEFAULT
	releaseall
	end
DewfordMeadow_EventScript_NeedAccount::
	msgbox DewfordMeadow_Text_NeedAccount, MSGBOX_DEFAULT
	releaseall
	end

DewfordMeadow_Text_ReadyToSing:
	.string "{STR_VAR_1} can lead our song.\n"
	.string "Let's see if the meadow answers.$"
DewfordMeadow_Text_MelodyAnswers:
	.string "That voice! MELOETTA answered us\n"
	.string "from somewhere beyond the trees.\p"
	.string "The manor's old account described\n"
	.string "the very same melody.$"
DewfordMeadow_Text_NeedRing:
	.string "STEVEN is studying these visitors.\n"
	.string "Speak to him inside GRANITE CAVE.\p"
	.string "After BRAWLY's Gym, he gives you the\n"
	.string "MEGA RING and sends you here.$"
DewfordMeadow_Text_NeedSinger:
	.string "MELOETTA answers songs. Any partner\n"
	.string "that knows SING can join us.\p"
	.string "The JIGGLYPUFF in DEWFORD MANOR\n"
	.string "already know it. The manor is\l"
	.string "northwest of here.\p"
	.string "Catch one and bring it back to me.\n"
	.string "Speak to the historian there, too.$"
DewfordMeadow_Text_NeedAccount:
	.string "Our song has been answered! You can\n"
	.string "search this meadow for MELOETTA.\p"
	.string "Now visit the manor northwest of me\n"
	.string "and speak to its historian.$"
DewfordMeadow_Text_ChapterComplete:
	.string "The account and melody point to a\n"
	.string "crystal chamber beneath SOOTOPOLIS.\p"
	.string "CAPTAIN STERN may know what's changed\n"
	.string "at sea. We should show him the notes.\p"
	.string "Return east to DEWFORD. Ask BRINEY\n"
	.string "on the shore to sail to SLATEPORT.\p"
	.string "Find STERN in the OCEANIC MUSEUM.\n"
	.string "Catching MELOETTA is optional.$"

DewfordMeadow_EventScript_ManorSign::
	msgbox DewfordMeadow_Text_ManorSign, MSGBOX_SIGN
	end

DewfordMeadow_Text_ManorSign:
	.string "DEWFORD MANOR\p"
	.string "A sea captain's retreat, later a\n"
	.string "home for MEGA EVOLUTION research.\p"
	.string "Please respect the POKéMON that\n"
	.string "now shelter inside.$"
