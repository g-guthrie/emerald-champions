SealedChamber_OuterRoom_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, SealedChamber_OuterRoom_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, SealedChamber_OuterRoom_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, SealedChamber_OuterRoom_OnLoad
	.byte 0

SealedChamber_OuterRoom_OnResume:
	setdivewarp MAP_UNDERWATER_SEALED_CHAMBER, 12, 44
	setescapewarp MAP_UNDERWATER_SEALED_CHAMBER, 12, 44
	end

SealedChamber_OuterRoom_OnTransition:
	setflag FLAG_LANDMARK_SEALED_CHAMBER
	end

SealedChamber_OuterRoom_OnLoad:
	call_if_unset FLAG_SYS_BRAILLE_DIG, SealedChamber_OuterRoom_EventScript_CloseInnerRoomEntrance
	end

SealedChamber_OuterRoom_EventScript_CloseInnerRoomEntrance::
	setmetatile 9, 1, METATILE_Cave_EntranceCover, TRUE
	setmetatile 10, 1, METATILE_Cave_EntranceCover, TRUE
	setmetatile 11, 1, METATILE_Cave_EntranceCover, TRUE
	setmetatile 9, 2, METATILE_Cave_SealedChamberBraille_Mid, TRUE
	setmetatile 10, 2, METATILE_Cave_SealedChamberBraille_Mid, TRUE
	setmetatile 11, 2, METATILE_Cave_SealedChamberBraille_Mid, TRUE
	return

SealedChamber_OuterRoom_EventScript_BrailleABC::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_ABC
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleGHI::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_GHI
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleMNO::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_MNO
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleTUV::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_TUV
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleDEF::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_DEF
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleJKL::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_JKL
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BraillePQRS::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_PQRS
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BraillePeriod::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_Period
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleWXYZ::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_WXYZ
	releaseall
	end

SealedChamber_OuterRoom_EventScript_BrailleComma::
	lockall
	braillemsgbox SealedChamber_OuterRoom_Braille_Comma
	releaseall
	end

@ DIG is capability-based like the obstacle moves: the wall asks for it, and any
@ party member whose species could learn DIG opens it. No battle slot is spent.
SealedChamber_OuterRoom_EventScript_InnerRoomEntranceWall::
SealedChamber_OuterRoom_EventScript_BrailleDigHere::
	lockall
	goto_if_set FLAG_SYS_BRAILLE_DIG, SealedChamber_OuterRoom_EventScript_HoleInWall
	checkfieldmove FIELD_MOVE_DIG, TRUE
	goto_if_eq VAR_RESULT, PARTY_SIZE, SealedChamber_OuterRoom_EventScript_NobodyCanDig
	setfieldeffectargument 0, VAR_RESULT
	bufferpartymonnick STR_VAR_1, VAR_RESULT
	msgbox SealedChamber_OuterRoom_Text_RearWallSoundsHollow, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SealedChamber_OuterRoom_EventScript_CancelDig
	msgbox SealedChamber_OuterRoom_Text_MonUsedDig, MSGBOX_DEFAULT
	closemessage
	goto EventScript_DigCommon

SealedChamber_OuterRoom_EventScript_CancelDig::
	closemessage
	releaseall
	end

SealedChamber_OuterRoom_EventScript_NobodyCanDig::
	msgbox SealedChamber_OuterRoom_Text_DigDirections, MSGBOX_DEFAULT
	releaseall
	end

SealedChamber_OuterRoom_EventScript_HoleInWall::
	msgbox gText_BigHoleInTheWall, MSGBOX_DEFAULT
	releaseall
	end


SealedChamber_OuterRoom_Text_RearWallSoundsHollow:
	.string "The rear wall sounds hollow.\n"
	.string "Use DIG?$"

SealedChamber_OuterRoom_Text_MonUsedDig:
	.string "{STR_VAR_1} used DIG!$"

SealedChamber_OuterRoom_Text_DigDirections:
	.string "The rear wall sounds hollow.\p"
	.string "No POKéMON with you could dig\n"
	.string "through it.\p"
	.string "Beyond it, the far wall's account\n"
	.string "opens the three ruins.$"
