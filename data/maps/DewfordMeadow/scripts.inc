DewfordMeadow_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordMeadow_OnTransition
	.byte 0

DewfordMeadow_OnTransition:
	setflag FLAG_LANDMARK_DEWFORD_MEADOW
	end

DewfordMeadow_EventScript_ManorSign::
	msgbox DewfordMeadow_Text_ManorSign, MSGBOX_SIGN
	end

DewfordMeadow_Text_ManorSign:
	.string "Dewford Manor: Once a Sea Captain's\n"
	.string "escape, now home only to Pokémon.$"

@ The Center lead sends players here with a partner that knows Sing.
DewfordMeadow_EventScript_Warden::
	lock
	faceplayer
	goto_if_set FLAG_EC_MEADOW_SONG_PERFORMED, DewfordMeadow_EventScript_AfterSong
	goto_if_unset FLAG_BADGE02_GET, DewfordMeadow_EventScript_NotYet
	setvar VAR_0x8004, LEGENDARY_SIGN_MELOETTA
	special TryUnlockLocalLegendaryDiscovery
	goto_if_eq VAR_RESULT, FALSE, DewfordMeadow_EventScript_NeedSinger
	msgbox DewfordMeadow_Text_LetsSing, MSGBOX_DEFAULT
	closemessage
	playse SE_M_SING
	waitse
	delay 24
	playmoncry SPECIES_MELOETTA, CRY_MODE_NORMAL
	waitmoncry
	applymovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	waitmovement 0
	setflag FLAG_EC_MEADOW_SONG_PERFORMED
	msgbox DewfordMeadow_Text_MelodyAnswers, MSGBOX_DEFAULT
	release
	end

DewfordMeadow_EventScript_AfterSong::
	setvar VAR_0x8004, LEGENDARY_SIGN_MELOETTA
	special GetSelectedLegendarySignState
	goto_if_eq VAR_RESULT, 2, DewfordMeadow_EventScript_MeloettaCaught
	msgbox DewfordMeadow_Text_AfterSong, MSGBOX_DEFAULT
	release
	end

DewfordMeadow_EventScript_MeloettaCaught::
	msgbox DewfordMeadow_Text_MeloettaCaught, MSGBOX_DEFAULT
	release
	end

DewfordMeadow_EventScript_NotYet::
	msgbox DewfordMeadow_Text_NotYet, MSGBOX_DEFAULT
	release
	end

DewfordMeadow_EventScript_NeedSinger::
	msgbox DewfordMeadow_Text_NeedSinger, MSGBOX_DEFAULT
	release
	end

DewfordMeadow_Text_LetsSing:
	.string "Your partner knows Sing! Let's see\n"
	.string "if the meadow answers our song.$"

DewfordMeadow_Text_MelodyAnswers:
	.string "That voice! Meloetta answered us\n"
	.string "from somewhere beyond the trees.\p"
	.string "Search this meadow's grass.\n"
	.string "It may appear when you least\l"
	.string "expect it.$"

DewfordMeadow_Text_AfterSong:
	.string "Our song has been answered! You can\n"
	.string "search this meadow for Meloetta.$"

DewfordMeadow_Text_MeloettaCaught:
	.string "You found Meloetta! I can still hear\n"
	.string "its song when the wind is right.$"

DewfordMeadow_Text_NotYet:
	.string "A melody sometimes drifts over this\n"
	.string "meadow. Come back once you've earned\l"
	.string "two Gym Badges, and we'll listen.$"

DewfordMeadow_Text_NeedSinger:
	.string "Meloetta answers songs. Any partner\n"
	.string "that knows Sing can join us.\p"
	.string "Bring one here and we'll try.$"
