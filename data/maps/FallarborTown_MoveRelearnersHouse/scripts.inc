FallarborTown_MoveRelearnersHouse_MapScripts::
	.byte 0

FallarborTown_MoveRelearnersHouse_EventScript_MoveRelearner::
	lockall
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
