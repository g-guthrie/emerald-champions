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
	.string "MATT: You made it through the\n"
	.string "whole hideout? Ha! You've got\l"
	.string "guts.\p"
	.string "ARCHIE's nearly ready to sail. I\n"
	.string "just need to keep you busy a\l"
	.string "little longer!$"

AquaHideout_B2F_Text_MattDefeat:
	.string "Ha… You don't give up, do you?$"

AquaHideout_B2F_Text_OurBossGotThroughHisPreparations:
	.string "MATT: You beat me, but you're too\n"
	.string "late! ARCHIE's ready to dive!$"

AquaHideout_B2F_Text_MattPostBattle:
	.string "MATT: ARCHIE's headed for a cavern\n"
	.string "beneath ROUTE 128.\p"
	.string "Go on, chase him! You'll need DIVE\n"
	.string "to follow a submarine down there.\p"
	.string "You'll need MOSSDEEP's BADGE and\n"
	.string "DIVE. Ask STEVEN for help, if you\l"
	.string "can find him! ARCHIE will be miles\l"
	.string "ahead.$"

AquaHideout_B2F_Text_Grunt4Intro:
	.string "I've been waiting here so long my\n"
	.string "feet have gone numb.\p"
	.string "At last! An intruder! I knew this\n"
	.string "post mattered!$"

AquaHideout_B2F_Text_Grunt4Defeat:
	.string "All that waiting, and I still\n"
	.string "lost…$"

AquaHideout_B2F_Text_Grunt4PostBattle:
	.string "Even a little delay helps ARCHIE\n"
	.string "get the submarine ready.\p"
	.string "That's what I'm putting in my\n"
	.string "report, anyway.$"

AquaHideout_B2F_Text_Grunt6Intro:
	.string "Warp panels! The pride of our\n"
	.string "hideout!\p"
	.string "I know exactly where you came\n"
	.string "from. Let's see if you can get\l"
	.string "past me!$"

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
