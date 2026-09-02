AquaHideout_B2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, AquaHideout_B2F_OnTransition
	.byte 0

AquaHideout_B2F_OnTransition:
	call_if_set FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, AquaHideout_B2F_EventScript_PreventMattNoticing
	end

AquaHideout_B2F_EventScript_PreventMattNoticing::
	setvar VAR_TEMP_1, 1
	return

AquaHideout_B2F_EventScript_MattNoticePlayer::
	lockall
	setvar VAR_0x8008, LOCALID_AQUA_HIDEOUT_MATT
	playse SE_PIN
	applymovement VAR_0x8008, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement VAR_0x8008, Common_Movement_FacePlayer
	waitmovement 0
	setvar VAR_TEMP_1, 1
	releaseall
	end

AquaHideout_B2F_EventScript_Matt::
	trainerbattle_double TRAINER_MATT, AquaHideout_B2F_Text_MattIntro, AquaHideout_B2F_Text_MattDefeat, EmeraldChampions_Text_NeedTwoPokemon, AquaHideout_B2F_EventScript_SubmarineEscape
	msgbox AquaHideout_B2F_Text_MattPostBattle, MSGBOX_DEFAULT
	release
	end

AquaHideout_B2F_EventScript_SubmarineEscape::
	setvar VAR_0x8008, LOCALID_AQUA_HIDEOUT_MATT
	setvar VAR_0x8009, LOCALID_AQUA_HIDEOUT_SUBMARINE
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	delay 20
	applymovement VAR_0x8008, Common_Movement_FacePlayer
	waitmovement 0
	msgbox AquaHideout_B2F_Text_OurBossGotThroughHisPreparations, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_0x8008, Common_Movement_WalkInPlaceFasterLeft
	applymovement VAR_0x8009, AquaHideout_B2F_Movement_SumbarineDepartLeft
	waitmovement 0
	removeobject VAR_0x8009
	delay 20
	applymovement VAR_0x8008, Common_Movement_FacePlayer
	waitmovement 0
	msgbox AquaHideout_B2F_Text_MattPostBattle, MSGBOX_DEFAULT
	setflag FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE
	setflag FLAG_HIDE_LILYCOVE_CITY_AQUA_GRUNTS
	release
	end

AquaHideout_B2F_Movement_SumbarineDepartLeft:
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

@ Unused
AquaHideout_B2F_Movement_SumbarineDepartRight:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

AquaHideout_B2F_EventScript_Grunt4::
	trainerbattle_double TRAINER_GRUNT_AQUA_HIDEOUT_4, AquaHideout_B2F_Text_Grunt4Intro, AquaHideout_B2F_Text_Grunt4Defeat, EmeraldChampions_Text_NeedTwoPokemon, AquaHideout_B2F_EventScript_Grunt4Defeated
	msgbox AquaHideout_B2F_Text_Grunt4PostBattle, MSGBOX_AUTOCLOSE
	end

AquaHideout_B2F_EventScript_Grunt4Defeated::
	msgbox AquaHideout_B2F_Text_Grunt4PostBattle, MSGBOX_DEFAULT
	release
	end

AquaHideout_B2F_EventScript_Grunt6::
	trainerbattle_double TRAINER_GRUNT_AQUA_HIDEOUT_6, AquaHideout_B2F_Text_Grunt6Intro, AquaHideout_B2F_Text_Grunt6Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox AquaHideout_B2F_Text_Grunt6PostBattle, MSGBOX_AUTOCLOSE
	end

AquaHideout_B2F_EventScript_Grunt8::
	trainerbattle_double TRAINER_GRUNT_AQUA_HIDEOUT_8, AquaHideout_B2F_Text_Grunt8Intro, AquaHideout_B2F_Text_Grunt8Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox AquaHideout_B2F_Text_Grunt8PostBattle, MSGBOX_AUTOCLOSE
	end

AquaHideout_B2F_Text_MattIntro:
	.string "MATT: You crossed the entire hideout\n"
	.string "before the tide changed. Impressive.\p"
	.string "ARCHIE needs one final current of time.\n"
	.string "My job is to make every turn cost you.$"

AquaHideout_B2F_Text_MattDefeat:
	.string "You refused every delay I offered…$"

AquaHideout_B2F_Text_OurBossGotThroughHisPreparations:
	.string "MATT: You won the battle. ARCHIE still\n"
	.string "finished the dive sequence.$"

AquaHideout_B2F_Text_MattPostBattle:
	.string "MATT: ARCHIE is headed for a cavern\n"
	.string "beneath the sea beyond LILYCOVE.\p"
	.string "You can cross the ocean. Finding one\n"
	.string "moving current inside it is harder.$"

AquaHideout_B2F_Text_Grunt4Intro:
	.string "Waiting is part of an ambush. You finally\n"
	.string "entered the current I was assigned.$"

AquaHideout_B2F_Text_Grunt4Defeat:
	.string "You turned my ambush back on me…$"

AquaHideout_B2F_Text_Grunt4PostBattle:
	.string "The delay still served ARCHIE. Whether it\n"
	.string "was enough is happening below us now.$"

AquaHideout_B2F_Text_Grunt6Intro:
	.string "Warp panels, the HIDEOUT's pride\n"
	.string "and joy!\p"
	.string "Every panel changes your position and\n"
	.string "every battle changes your resources.\p"
	.string "AQUA wins by making the field move.$"

AquaHideout_B2F_Text_Grunt6Defeat:
	.string "What's wrong with you?\n"
	.string "You're not tired at all!$"

AquaHideout_B2F_Text_Grunt6PostBattle:
	.string "That reminds me… I can't remember\n"
	.string "where I put the MASTER BALL.\p"
	.string "If I fail to guard it, our BOSS will\n"
	.string "chew me out…$"

AquaHideout_B2F_Text_Grunt8Intro:
	.string "First thing they taught me here was how\n"
	.string "the warp panels connect.\p"
	.string "Second thing was doubles. I'm better\n"
	.string "at the panels.$"

AquaHideout_B2F_Text_Grunt8Defeat:
	.string "Too busy thinking about panels…$"

AquaHideout_B2F_Text_Grunt8PostBattle:
	.string "I'll learn to battle properly. Someday.\n"
	.string "The panels are just more fun.$"
