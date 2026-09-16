MirageTower_4F_MapScripts::
	.byte 0

MirageTower_4F_EventScript_RootFossil::
	call EC_RequireDesert
	lock
	faceplayer
	msgbox MirageTower_4F_Text_TakeRootFossil, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MirageTower_4F_EventScript_LeaveRootFossil
	giveitem ITEM_ROOT_FOSSIL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	closemessage
	setflag FLAG_HIDE_MIRAGE_TOWER_ROOT_FOSSIL
	setflag FLAG_HIDE_MIRAGE_TOWER_CLAW_FOSSIL
	removeobject LOCALID_MIRAGE_ROOT_FOSSIL
	delay 30
	setflag FLAG_CHOSE_ROOT_FOSSIL
	goto MirageTower_4F_EventScript_CollapseMirageTower
	end

MirageTower_4F_EventScript_LeaveRootFossil::
	msgbox MirageTower_4F_Text_LeftRootFossilAlone, MSGBOX_DEFAULT
	release
	end

MirageTower_4F_EventScript_ClawFossil::
	call EC_RequireDesert
	lock
	faceplayer
	msgbox MirageTower_4F_Text_TakeClawFossil, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MirageTower_4F_EventScript_LeaveClawFossil
	giveitem ITEM_CLAW_FOSSIL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	closemessage
	setflag FLAG_HIDE_MIRAGE_TOWER_CLAW_FOSSIL
	setflag FLAG_HIDE_MIRAGE_TOWER_ROOT_FOSSIL
	removeobject LOCALID_MIRAGE_CLAW_FOSSIL
	delay 30
	setflag FLAG_CHOSE_CLAW_FOSSIL
	goto MirageTower_4F_EventScript_CollapseMirageTower
	end

MirageTower_4F_EventScript_LeaveClawFossil::
	msgbox MirageTower_4F_Text_LeaveClawFossilAlone, MSGBOX_DEFAULT
	release
	end

MirageTower_4F_EventScript_CollapseMirageTower::
	setvar VAR_0x8004, 1   @ vertical pan
	setvar VAR_0x8005, 1   @ horizontal pan
	setvar VAR_0x8006, 32  @ num shakes
	setvar VAR_0x8007, 2   @ shake delay
	special ShakeCamera
	waitstate
	special DoMirageTowerCeilingCrumble
	setvar VAR_MIRAGE_TOWER_STATE, 1
	clearflag FLAG_LANDMARK_MIRAGE_TOWER
	warp MAP_ROUTE111, 19, 59
	waitstate
	release
	end

MirageTower_4F_Text_TakeRootFossil:
	.string "You found the ROOT FOSSIL.\p"
	.string "Taking it will collapse the tower\n"
	.string "and seal ALL the ruins below.\l"
	.string "You can NEVER return.\p"
	.string "The fossils, the ODD KEYSTONE and\n"
	.string "the PROTECTOR still down there\l"
	.string "will be lost for good.\p"
	.string "Everything else that lives here\n"
	.string "can also be found in the open\l"
	.string "desert and the DESERT UNDERPASS.\p"
	.string "If you still want those finds,\n"
	.string "leave the fossil and explore now.\p"
	.string "Permanently seal the ruins and\n"
	.string "take the ROOT FOSSIL?$"

MirageTower_4F_Text_LeftRootFossilAlone:
	.string "{PLAYER} left the ROOT FOSSIL alone.$"

MirageTower_4F_Text_TakeClawFossil:
	.string "You found the CLAW FOSSIL.\p"
	.string "Taking it will collapse the tower\n"
	.string "and seal ALL the ruins below.\l"
	.string "You can NEVER return.\p"
	.string "The fossils, the ODD KEYSTONE and\n"
	.string "the PROTECTOR still down there\l"
	.string "will be lost for good.\p"
	.string "Everything else that lives here\n"
	.string "can also be found in the open\l"
	.string "desert and the DESERT UNDERPASS.\p"
	.string "If you still want those finds,\n"
	.string "leave the fossil and explore now.\p"
	.string "Permanently seal the ruins and\n"
	.string "take the CLAW FOSSIL?$"

MirageTower_4F_Text_LeaveClawFossilAlone:
	.string "{PLAYER} left the CLAW FOSSIL alone.$"

