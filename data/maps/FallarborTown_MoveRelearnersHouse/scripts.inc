FallarborTown_MoveRelearnersHouse_MapScripts::
	.byte 0

@ Inclement's Evie and Ivy still run this house and still introduce themselves the
@ first time you meet them. Their old paid EV/IV programme is gone, so the pitch is
@ adapted to Champions' free preparation services.
FallarborTown_MoveRelearnersHouse_EventScript_IvyEvieIntro::
	goto_if_set FLAG_EC_MET_IVY_EVIE, FallarborTown_MoveRelearnersHouse_EventScript_IntroDone
	playse SE_PIN
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_ExclamationMark
	applymovement LOCALID_EV_TRAINER, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_FacePlayer
	applymovement LOCALID_EV_TRAINER, Common_Movement_FacePlayer
	waitmovement 0
	msgbox FallarborTown_MoveRelearnersHouse_Text_IvyEvieIntro1, MSGBOX_DEFAULT
	msgbox FallarborTown_MoveRelearnersHouse_Text_IvyEvieIntro2, MSGBOX_DEFAULT
	msgbox FallarborTown_MoveRelearnersHouse_Text_IvyEvieIntro3, MSGBOX_DEFAULT
	setflag FLAG_EC_MET_IVY_EVIE
	return

FallarborTown_MoveRelearnersHouse_EventScript_IntroDone::
	return

FallarborTown_MoveRelearnersHouse_Text_IvyEvieIntro1:
	.string "Hey there, TRAINER!\p"
	.string "If you want your POKéMON to be the very\n"
	.string "best in all of HOENN, you've come to\l"
	.string "the right place!\p"
	.string "I'm EVIE, and this is my sister, IVY.$"

FallarborTown_MoveRelearnersHouse_Text_IvyEvieIntro2:
	.string "IVY: We ran the training programs that\n"
	.string "seriously beefed up your team!$"

FallarborTown_MoveRelearnersHouse_Text_IvyEvieIntro3:
	.string "EVIE: The POKéMON CENTERS took over\n"
	.string "our programmes, and they charge\l"
	.string "nothing at all.\p"
	.string "We just talk shop now!$"

@ Emerald Champions (C7): one EV editor. IVY no longer runs a second copy here; she
@ points at the free tutor beside every POKéMON CENTER's PC.
FallarborTown_MoveRelearnersHouse_EventScript_EvTrainer::
	lockall
	call FallarborTown_MoveRelearnersHouse_EventScript_IvyEvieIntro
	applymovement LOCALID_EV_TRAINER, Common_Movement_FacePlayer
	waitmovement 0
	msgbox FallarborTown_MoveRelearnersHouse_Text_EvRedirect, MSGBOX_DEFAULT
	releaseall
	end

FallarborTown_MoveRelearnersHouse_Text_EvRedirect:
	.string "IVY: I closed my programme.\p"
	.string "The tutor beside the PC in any\n"
	.string "POKéMON CENTER sets EVs for free.$"

FallarborTown_MoveRelearnersHouse_EventScript_MoveRelearner::
	lockall
	call FallarborTown_MoveRelearnersHouse_EventScript_IvyEvieIntro
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_FacePlayer
	waitmovement 0
	msgbox FallarborTown_MoveRelearnersHouse_Text_PreparationAdvice, MSGBOX_DEFAULT
	releaseall
	end

FallarborTown_MoveRelearnersHouse_Text_PreparationAdvice:
	.string "The tutors at every POKéMON CENTER\n"
	.string "teach your POKéMON moves for free.\p"
	.string "Choose moves that help your partners!\n"
	.string "An attack is not always the best play.$"
