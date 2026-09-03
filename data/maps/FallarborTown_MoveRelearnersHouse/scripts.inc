FallarborTown_MoveRelearnersHouse_MapScripts::
	.byte 0

@ Inclement's Evie and Ivy still run this house and still introduce themselves the
@ first time you meet them. Their old paid EV/IV programme is gone, so the pitch is
@ adapted to Champions' free preparation services.
FallarborTown_MoveRelearnersHouse_EventScript_IvyEvieIntro::
	goto_if_set FLAG_EC_MET_IVY_EVIE, FallarborTown_MoveRelearnersHouse_EventScript_IntroDone
	playse SE_PIN
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_ExclamationMark
	applymovement LOCALID_STAT_POINT_TRAINER, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_FacePlayer
	applymovement LOCALID_STAT_POINT_TRAINER, Common_Movement_FacePlayer
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
	.string "IVY: We run the training programs that\n"
	.string "seriously beef up your team!$"

FallarborTown_MoveRelearnersHouse_Text_IvyEvieIntro3:
	.string "EVIE: And these days it's all free.\p"
	.string "IVY tunes STAT POINTS, and I trade\n"
	.string "HEART SCALES for PP UPS.\p"
	.string "Come back as often as you like!$"

FallarborTown_MoveRelearnersHouse_EventScript_StatPointTrainer::
	lockall
	call FallarborTown_MoveRelearnersHouse_EventScript_IvyEvieIntro
	releaseall
	goto Common_EventScript_EmeraldChampionsStatPointEditor
	end

FallarborTown_MoveRelearnersHouse_EventScript_MoveRelearner::
	lockall
	call FallarborTown_MoveRelearnersHouse_EventScript_IvyEvieIntro
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_FacePlayer
	waitmovement 0
	goto_if_set FLAG_TEMP_1, FallarborTown_MoveRelearnersHouse_EventScript_AskExchange
	msgbox FallarborTown_MoveRelearnersHouse_Text_ImTheScaleCollector, MSGBOX_DEFAULT
	setflag FLAG_TEMP_1
	goto FallarborTown_MoveRelearnersHouse_EventScript_AskExchange
	end

FallarborTown_MoveRelearnersHouse_EventScript_AskExchange::
	checkitem ITEM_HEART_SCALE, 1
	goto_if_eq VAR_RESULT, FALSE, FallarborTown_MoveRelearnersHouse_EventScript_NoHeartScale
	msgbox FallarborTown_MoveRelearnersHouse_Text_TradeScaleForPPUp, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, FallarborTown_MoveRelearnersHouse_EventScript_MaybeLater
	checkitemspace ITEM_PP_UP, 1
	goto_if_eq VAR_RESULT, FALSE, FallarborTown_MoveRelearnersHouse_EventScript_NoRoom
	removeitem ITEM_HEART_SCALE, 1
	giveitem ITEM_PP_UP, 1
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull @ Space was checked before payment.
	msgbox FallarborTown_MoveRelearnersHouse_Text_ExchangeComplete, MSGBOX_DEFAULT
	goto FallarborTown_MoveRelearnersHouse_EventScript_AskExchange
	end

FallarborTown_MoveRelearnersHouse_EventScript_NoHeartScale::
	msgbox FallarborTown_MoveRelearnersHouse_Text_BringHeartScale, MSGBOX_DEFAULT
	releaseall
	end

FallarborTown_MoveRelearnersHouse_EventScript_NoRoom::
	msgbox FallarborTown_MoveRelearnersHouse_Text_NoRoomForPPUp, MSGBOX_DEFAULT
	releaseall
	end

FallarborTown_MoveRelearnersHouse_EventScript_MaybeLater::
	msgbox FallarborTown_MoveRelearnersHouse_Text_MaybeLater, MSGBOX_DEFAULT
	releaseall
	end

FallarborTown_MoveRelearnersHouse_Text_ImTheScaleCollector:
	.string "I'm a HEART SCALE collector.\p"
	.string "The tutors at every POKéMON CENTER\n"
	.string "already teach moves for free.\p"
	.string "I polish HEART SCALES into PP UPS,\n"
	.string "one for one.$"

FallarborTown_MoveRelearnersHouse_Text_TradeScaleForPPUp:
	.string "Trade one HEART SCALE for one PP UP?$"

FallarborTown_MoveRelearnersHouse_Text_ExchangeComplete:
	.string "A fine trade! I can polish another\n"
	.string "HEART SCALE whenever you're ready.$"

FallarborTown_MoveRelearnersHouse_Text_BringHeartScale:
	.string "Bring me a HEART SCALE and I'll trade\n"
	.string "you a PP UP.$"

FallarborTown_MoveRelearnersHouse_Text_NoRoomForPPUp:
	.string "Your BAG has no room for a PP UP.\n"
	.string "Make space before we trade.$"

FallarborTown_MoveRelearnersHouse_Text_MaybeLater:
	.string "No rush. Your HEART SCALE stays safe\n"
	.string "with you until you're ready.$"
