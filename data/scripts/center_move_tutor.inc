@ The old man by the PC in every Hoenn POKéMON CENTER. He teaches, for free,
@ any level-up move of the Pokémon's current species/form, including moves
@ it hasn't reached yet (MOVE_RELEARNER_ALL_LEVEL_UP_MOVES). The flow mirrors
@ the Fallarbor MOVE TUTOR: party screen, then the move relearner screen.
Common_EventScript_CenterMoveTutor::
	lockall
	faceplayer
	msgbox CenterMoveTutor_Text_CanTeachAnyMove, MSGBOX_DEFAULT
	goto CenterMoveTutor_EventScript_ChooseMonNoMsg
	end

CenterMoveTutor_EventScript_ChooseMon::
	msgbox CenterMoveTutor_Text_TutorWhichMon, MSGBOX_DEFAULT
CenterMoveTutor_EventScript_ChooseMonNoMsg::
	setmoverelearnerstate MOVE_RELEARNER_ALL_LEVEL_UP_MOVES
	chooseboxmon SELECT_PC_MON_MOVE_RELEARNER
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, CenterMoveTutor_EventScript_ComeBackAnytime
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, CenterMoveTutor_EventScript_CantTeachEgg
	special Special_HasMoveToRelearn
	goto_if_eq VAR_RESULT, FALSE, CenterMoveTutor_EventScript_NoMoveToTeachMon
	msgbox CenterMoveTutor_Text_TeachWhichMove, MSGBOX_DEFAULT
	special TeachMoveRelearnerMove
	goto_if_eq VAR_RESULT, FALSE, CenterMoveTutor_EventScript_ChooseMon
	goto CenterMoveTutor_EventScript_ComeBackAnytime
	end

CenterMoveTutor_EventScript_NoMoveToTeachMon::
	msgbox CenterMoveTutor_Text_DontHaveMoveToTeachPokemon, MSGBOX_DEFAULT
	goto CenterMoveTutor_EventScript_ChooseMon
	end

CenterMoveTutor_EventScript_CantTeachEgg::
	msgbox CenterMoveTutor_Text_CantTeachEgg, MSGBOX_DEFAULT
	goto CenterMoveTutor_EventScript_ChooseMon
	end

CenterMoveTutor_EventScript_ComeBackAnytime::
	setmoverelearnerstate MOVE_RELEARNER_LEVEL_UP_MOVES
	msgbox CenterMoveTutor_Text_ComeBackAnytime, MSGBOX_DEFAULT
	releaseall
	end

CenterMoveTutor_Text_CanTeachAnyMove:
	.string "I can teach your POKéMON any move\n"
	.string "it can learn by leveling up--even\l"
	.string "moves it hasn't reached yet.\p"
	.string "Which POKéMON?$"

CenterMoveTutor_Text_TutorWhichMon:
	.string "Which POKéMON needs tutoring?$"

CenterMoveTutor_Text_TeachWhichMove:
	.string "Which move should I teach?$"

CenterMoveTutor_Text_DontHaveMoveToTeachPokemon:
	.string "Sorry…\p"
	.string "It doesn't appear as if I have any move\n"
	.string "I can teach that POKéMON.$"

CenterMoveTutor_Text_CantTeachEgg:
	.string "Hunh? There isn't a single move that\n"
	.string "I can teach an EGG.$"

CenterMoveTutor_Text_ComeBackAnytime:
	.string "If your POKéMON need to learn a move,\n"
	.string "come see me anytime.$"
