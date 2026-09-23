.set LOCALID_STEVEN, 1

MeteorFalls_StevensCave_MapScripts:: @ 823B181
	.byte 0

MeteorFalls_StevensCave_EventScript_Steven:: @ 823B182
	lock
	specialvar VAR_RESULT, GetEmeraldChampionsFinaleStage
	goto_if_lt VAR_RESULT, EC_FINALE_STEVEN, MeteorFalls_StevensCave_EventScript_NotYet
	goto_if_defeated TRAINER_STEVEN, MeteorFalls_StevensCave_EventScript_Defeated
	waitse
	playse SE_PIN
	applymovement LOCALID_STEVEN, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_STEVEN, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_STEVEN, Common_Movement_FacePlayer
	waitmovement 0
	msgbox MeteorFalls_StevensCave_Text_ShouldKnowHowGoodIAmExpectWorst, MSGBOX_DEFAULT
	trainerbattle_no_intro_double TRAINER_STEVEN, MeteorFalls_StevensCave_Text_StevenDefeat
	msgbox MeteorFalls_StevensCave_Text_MyPredictionCameTrue, MSGBOX_DEFAULT
	goto_if_unset FLAG_RECEIVED_AURORA_TICKET, MeteorFalls_StevensCave_EventScript_GiveAuroraTicket
	release
	end

MeteorFalls_StevensCave_EventScript_NotYet::
	faceplayer
	msgbox MeteorFalls_StevensCave_Text_NotYet, MSGBOX_DEFAULT
	call Common_EventScript_FinaleDirections
	release
	end

MeteorFalls_StevensCave_EventScript_Defeated::
	applymovement LOCALID_STEVEN, Common_Movement_FacePlayer
	waitmovement 0
	goto_if_unset FLAG_RECEIVED_AURORA_TICKET, MeteorFalls_StevensCave_EventScript_GiveAuroraTicket
	call Common_EventScript_FinaleDirections
	release
	end

MeteorFalls_StevensCave_EventScript_GiveAuroraTicket:
	msgbox MeteorFalls_StevensCave_Text_BirthIsland, MSGBOX_DEFAULT
	giveitem ITEM_AURORA_TICKET
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_AURORA_TICKET
	setflag FLAG_ENABLE_SHIP_BIRTH_ISLAND
	call Common_EventScript_FinaleDirections
	release
	end

MeteorFalls_StevensCave_Text_ShouldKnowHowGoodIAmExpectWorst: @ 823B1E1
	.string "Steven: Oh, wow, {PLAYER}{KUN}.\n"
	.string "I'm amazed you knew where to find me.\p"
	.string "Do you, uh… maybe think of me as\n"
	.string "just a rock maniac?\p"
	.string "No, that can't be right.\p"
	.string "We battled alongside each other at\n"
	.string "the Mossdeep Space Center.\p"
	.string "You should have a very good idea\n"
	.string "about how good I am.\p"
	.string "Okay, {PLAYER}{KUN}, if you're going to mount\n"
	.string "a serious challenge, expect the worst!$"

MeteorFalls_StevensCave_Text_StevenDefeat: @ 823B32D
	.string "You…\n"
	.string "I had no idea you'd become so strong…$"

MeteorFalls_StevensCave_Text_MyPredictionCameTrue: @ 823B358
	.string "Steven: Come to think of it, ever since\n"
	.string "our paths first crossed in Granite\l"
	.string "Cave in Dewford, I had this feeling.\p"
	.string "I thought that you would eventually\n"
	.string "become the Champion.\p"
	.string "My predictions usually come true.\p"
	.string "Now you're ready for Birth Island.\n"
	.string "Its strange stone needs someone\l"
	.string "with your determination.$"

MeteorFalls_StevensCave_Text_NotYet:
	.string "Steven: Oh, {PLAYER}{KUN}. I'm still\n"
	.string "studying the stones down here.\p"
	.string "Come find me once you've reached\n"
	.string "my part of the expedition.$"

MeteorFalls_StevensCave_Text_BirthIsland:
	.string "Please investigate the strange stone\n"
	.string "that's appeared on Birth Island.\p"
	.string "Here, take this ticket to the harbor\n"
	.string "in Lilycove.\p"
	.string "Perhaps you'll find something useful…$"
