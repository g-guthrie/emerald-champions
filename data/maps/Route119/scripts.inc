.set LOCALID_RIVAL, 10
.set LOCALID_RIVAL_ON_BIKE, 19
.set LOCALID_SCOTT, 33

Route119_MapScripts:: @ 81F4424
	map_script MAP_SCRIPT_ON_RESUME, Route119_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, Route119_OnTransition
	.byte 0

Route119_OnResume: @ 81F442F
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, Route119_EventScript_TryRemoveKecleon
	end

Route119_EventScript_TryRemoveKecleon:: @ 81F4439
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_CAUGHT
	goto_if_ne Common_EventScript_NopReturn
	removeobject VAR_LAST_TALKED
	return

Route119_OnTransition: @ 81F444D
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupRivalOnBikeGfxId
	compare VAR_WEATHER_INSTITUTE_STATE, 1
	call_if_eq Route119_EventScript_MoveInstituteWorkersDownstairs
	special SetRoute119Weather
	end

Route119_EventScript_MoveInstituteWorkersDownstairs:: @ 81F4466
	setflag FLAG_HIDE_WEATHER_INSTITUTE_2F_WORKERS
	clearflag FLAG_HIDE_WEATHER_INSTITUTE_1F_WORKERS
	setvar VAR_WEATHER_INSTITUTE_STATE, 2
	return

Route119_EventScript_RivalTrigger1:: @ 81F4472
	setvar VAR_TEMP_1, 1
	goto Route119_EventScript_RivalEncounter
	end

Route119_EventScript_RivalTrigger2:: @ 81F447D
	setvar VAR_TEMP_1, 2
	goto Route119_EventScript_RivalEncounter
	end

Route119_EventScript_RivalEncounter:: @ 81F4488
	lockall
	addobject LOCALID_RIVAL_ON_BIKE
	checkplayergender
	compare VAR_RESULT, MALE
	call_if_eq Route119_EventScript_PlayMayMusic
	compare VAR_RESULT, FEMALE
	call_if_eq Route119_EventScript_PlayBrendanMusic
	delay 65
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_RivalEnter1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_RivalEnter2
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 30
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_SetRivalPos1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_SetRivalPos2
	removeobject LOCALID_RIVAL_ON_BIKE
	addobject LOCALID_RIVAL
	delay 30
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Route119_EventScript_BattleMay
	compare VAR_RESULT, FEMALE
	goto_if_eq Route119_EventScript_BattleBrendan
	releaseall
	end

Route119_EventScript_PlayMayMusic:: @ 81F4501
	playbgm MUS_ENCOUNTER_MAY, TRUE
	return

Route119_EventScript_PlayBrendanMusic:: @ 81F4506
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	return

Route119_EventScript_BattleMay:: @ 81F450B
	msgbox Route119_Text_MayIntro, MSGBOX_DEFAULT
	@ The rival keeps the starter the player left behind (the player takes two).
	special BufferEmeraldChampionsRivalBranch
	switch VAR_RESULT
	case 0, Route119_EventScript_BattleMayTreecko
	case 1, Route119_EventScript_BattleMayTorchic
	case 2, Route119_EventScript_BattleMayMudkip
	end

Route119_EventScript_BattleMayTreecko:: @ 81F453A
	trainerbattle_no_intro_double TRAINER_MAY_ROUTE_119_TREECKO, Route119_Text_MayDefeat
	goto Route119_EventScript_DefeatedMay
	end

Route119_EventScript_BattleMayTorchic:: @ 81F454A
	trainerbattle_no_intro_double TRAINER_MAY_ROUTE_119_TORCHIC, Route119_Text_MayDefeat
	goto Route119_EventScript_DefeatedMay
	end

Route119_EventScript_BattleMayMudkip:: @ 81F455A
	trainerbattle_no_intro_double TRAINER_MAY_ROUTE_119_MUDKIP, Route119_Text_MayDefeat
	goto Route119_EventScript_DefeatedMay
	end

Route119_EventScript_DefeatedMay:: @ 81F456A
	msgbox Route119_Text_MayPresentForYou, MSGBOX_DEFAULT
	call Route119_EventScript_GiveFlyHM
	msgbox Route119_Text_MayExplainFly, MSGBOX_DEFAULT
	goto Route119_EventScript_RivalExitScottArrive
	end

Route119_EventScript_BattleBrendan:: @ 81F4585
	msgbox Route119_Text_BrendanIntro, MSGBOX_DEFAULT
	@ The rival keeps the starter the player left behind (the player takes two).
	special BufferEmeraldChampionsRivalBranch
	switch VAR_RESULT
	case 0, Route119_EventScript_BattleBrendanTreecko
	case 1, Route119_EventScript_BattleBrendanTorchic
	case 2, Route119_EventScript_BattleBrendanMudkip
	end

Route119_EventScript_BattleBrendanTreecko:: @ 81F45B4
	trainerbattle_no_intro_double TRAINER_BRENDAN_ROUTE_119_TREECKO, Route119_Text_BrendanDefeat
	goto Route119_EventScript_DefeatedBrendan
	end

Route119_EventScript_BattleBrendanTorchic:: @ 81F45C4
	trainerbattle_no_intro_double TRAINER_BRENDAN_ROUTE_119_TORCHIC, Route119_Text_BrendanDefeat
	goto Route119_EventScript_DefeatedBrendan
	end

Route119_EventScript_BattleBrendanMudkip:: @ 81F45D4
	trainerbattle_no_intro_double TRAINER_BRENDAN_ROUTE_119_MUDKIP, Route119_Text_BrendanDefeat
	goto Route119_EventScript_DefeatedBrendan
	end

Route119_EventScript_DefeatedBrendan:: @ 81F45E4
	msgbox Route119_Text_BrendanIllGiveYouThis, MSGBOX_DEFAULT
	call Route119_EventScript_GiveFlyHM
	msgbox Route119_Text_BrendanExplainFly, MSGBOX_DEFAULT
	goto Route119_EventScript_RivalExitScottArrive
	end

Route119_EventScript_GiveFlyHM:: @ 81F45FF
	setflag FLAG_RECEIVED_HM02
	return

Route119_EventScript_RivalExitScottArrive:: @ 81F460F
	closemessage
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_SetRivalPos1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_SetRivalPos2
	removeobject LOCALID_RIVAL
	addobject LOCALID_RIVAL_ON_BIKE
	delay 30
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_RivalExit1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_RivalExit2
	removeobject LOCALID_RIVAL_ON_BIKE
	setvar VAR_ROUTE119_STATE, 1
	savebgm MUS_DUMMY
	fadedefaultbgm
	delay 60
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_SetScottPos1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_SetScottPos2
	addobject LOCALID_SCOTT
	applymovement LOCALID_SCOTT, Route119_Movement_ScottEnter
	waitmovement 0
	addvar VAR_SCOTT_STATE, 1
	msgbox Route119_Text_ScottWayToGoBeSeeingYou, MSGBOX_DEFAULT
	closemessage
	compare VAR_TEMP_1, 1
	call_if_eq Route119_EventScript_ScottExit1
	compare VAR_TEMP_1, 2
	call_if_eq Route119_EventScript_ScottExit2
	removeobject LOCALID_SCOTT
	releaseall
	end

Route119_EventScript_SetScottPos1:: @ 81F46A0
	setobjectxyperm LOCALID_SCOTT, 27, 25
	return

Route119_EventScript_SetScottPos2:: @ 81F46A8
	setobjectxyperm LOCALID_SCOTT, 28, 25
	return

Route119_EventScript_ScottExit1:: @ 81F46B0
	applymovement LOCALID_SCOTT, Route119_Movement_ScottExit1
	waitmovement 0
	return

Route119_EventScript_ScottExit2:: @ 81F46BB
	applymovement LOCALID_SCOTT, Route119_Movement_ScottExit2
	waitmovement 0
	return

Route119_EventScript_RivalEnter1:: @ 81F46C6
	applymovement LOCALID_RIVAL_ON_BIKE, Route119_Movement_RivalEnter1
	waitmovement 0
	return

Route119_EventScript_RivalEnter2:: @ 81F46D1
	applymovement LOCALID_RIVAL_ON_BIKE, Route119_Movement_RivalEnter2
	waitmovement 0
	return

Route119_EventScript_RivalExit1:: @ 81F46DC
	applymovement OBJ_EVENT_ID_PLAYER, Route119_Movement_PlayerWatchRivalExit1
	applymovement LOCALID_RIVAL_ON_BIKE, Route119_Movement_RivalExit1
	waitmovement 0
	return

Route119_EventScript_RivalExit2:: @ 81F46EE
	applymovement OBJ_EVENT_ID_PLAYER, Route119_Movement_PlayerWatchRivalExit2
	applymovement LOCALID_RIVAL_ON_BIKE, Route119_Movement_RivalExit2
	waitmovement 0
	return

Route119_EventScript_SetRivalPos1:: @ 81F4700
	setobjectxyperm LOCALID_RIVAL, 25, 32
	setobjectxyperm LOCALID_RIVAL_ON_BIKE, 25, 32
	return

Route119_EventScript_SetRivalPos2:: @ 81F470F
	setobjectxyperm LOCALID_RIVAL, 26, 32
	setobjectxyperm LOCALID_RIVAL_ON_BIKE, 26, 32
	return

Route119_Movement_PlayerWatchRivalExit1: @ 81F471E
	delay_16
	walk_in_place_fast_right
	delay_8
	walk_in_place_fast_up
	step_end

Route119_Movement_PlayerWatchRivalExit2: @ 81F4723
	delay_16
	walk_in_place_fast_left
	delay_8
	walk_in_place_fast_up
	step_end

Route119_Movement_RivalEnter1: @ 81F4728
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	step_end

Route119_Movement_RivalEnter2: @ 81F4732
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	step_end

Route119_Movement_RivalExit1: @ 81F473D
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

Route119_Movement_RivalExit2: @ 81F4747
	walk_fast_left
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

Route119_Movement_ScottEnter: @ 81F4752
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_down
	step_end

Route119_Movement_ScottExit1: @ 81F475A
	walk_up
	walk_right
	walk_right
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

Route119_Movement_ScottExit2: @ 81F4763
	walk_up
	walk_right
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

Route119_EventScript_CyclingTriathleteM:: @ 81F476B
	msgbox Route119_Text_TallGrassSnaresBikeTires, MSGBOX_NPC
	end

Route119_EventScript_RouteSignFortree:: @ 81F4774
	msgbox Route119_Text_RouteSignFortree, MSGBOX_SIGN
	goto Route119_EventScript_ShowRouteRoster

Route119_EventScript_ShowRouteRoster::
	call Common_EventScript_ShowRouteRoster
	setvar VAR_0x8004, LEGENDARY_SIGN_ARTICUNO_GALAR
	call Common_EventScript_ShowRouteLegend
	setvar VAR_0x8004, LEGENDARY_SIGN_TORNADUS
	call Common_EventScript_ShowRouteLegend
	releaseall
	end

Route119_EventScript_WeatherInstituteSign:: @ 81F477D
	msgbox Route119_Text_WeatherInstitute, MSGBOX_SIGN
	end

Route119_EventScript_Donald:: @ 81F479D
	trainerbattle_double TRAINER_DONALD, Route119_Text_DonaldIntro, Route119_Text_DonaldDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_Text_DonaldPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Taylor:: @ 81F47B4
	trainerbattle_double TRAINER_TAYLOR, Route119_Text_TaylorIntro, Route119_Text_TaylorDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_Text_TaylorPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Doug:: @ 81F47CB
	trainerbattle_double TRAINER_DOUG, Route119_Text_DougIntro, Route119_Text_DougDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_Text_DougPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Jackson:: @ 81F4810
	trainerbattle_double TRAINER_JACKSON_1, Route119_Text_JacksonIntro, Route119_Text_JacksonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_Text_JacksonPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Catherine:: @ 81F4872
	trainerbattle_double TRAINER_CATHERINE_1, Route119_Text_CatherineIntro, Route119_Text_CatherineDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_Text_CatherinePostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Hugh:: @ 81F48D4
	@ This retired encounter has no campaign battle.
	msgbox Route119_Text_HughPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Phil:: @ 81F48EB
	trainerbattle_double TRAINER_PHIL, Route119_Text_PhilIntro, Route119_Text_PhilDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_Text_PhilPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Hideo:: @ 81F4930
	trainerbattle_double TRAINER_HIDEO, Route119_Text_HideoIntro, Route119_Text_HideoDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_Text_HideoPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_Dayton:: @ 81F4975
	@ This retired encounter has no campaign battle.
	msgbox Route119_Text_DaytonPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_EventScript_BridgeAquaGrunt1:: @ 81F49A3
	lock
	faceplayer
	msgbox Route119_Text_StayAwayFromWeatherInstitute, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

Route119_EventScript_BridgeAquaGrunt2:: @ 81F49BA
	lock
	faceplayer
	msgbox Route119_Text_DontGoNearWeatherInstitute, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

Route119_EventScript_Boy1:: @ 81F49D1
	msgbox Route119_Text_ThoughtFlyByCatchingBirdMons, MSGBOX_NPC
	end

Route119_EventScript_Boy2:: @ 81F49DA
	msgbox Route119_Text_CanYourMonMakeSecretBase, MSGBOX_NPC
	end

Route119_EventScript_TrainerTipsDecoration:: @ 81F49E3
	msgbox Route119_Text_TrainerTipsDecoration, MSGBOX_SIGN
	end

Route119_EventScript_ScottWonAtFortreeGymCall:: @ 81F49EC
	lockall
	pokenavcall Route119_Text_ScottYouWonAtFortreeGym
	waitmessage
	clearflag FLAG_SCOTT_CALL_FORTREE_GYM
	addvar VAR_SCOTT_STATE, 1
	releaseall
	end

Route119_Text_MayIntro: @ 81F49FD
	.string "May: {PLAYER}{KUN}!\n"
	.string "Where were you? I was looking for you!\p"
	.string "How much stronger have you gotten?\n"
	.string "Let me check for you!\p"
	.string "Ready with your Pokémon?\n"
	.string "Of course you are! Go!$"

Route119_Text_MayDefeat: @ 81F4A98
	.string "Achah!\n"
	.string "{PLAYER}{KUN}, you're strong!\p"
	.string "I was worried that you might be\n"
	.string "struggling with your training.$"

Route119_Text_MayPresentForYou: @ 81F4AF3
	.string "May: But I had absolutely nothing to\n"
	.string "worry about! Keep it up!\p"
	.string "I can authorize your Flight Beacon!$"

Route119_Text_MayExplainFly:
	.string "May: Once you earn the Feather Badge,\n"
	.string "your Flight Beacon can take you home.\p"
	.string "Visit Littleroot for a change!\p"
	.string "I bet your mom's worried about you,\n"
	.string "{PLAYER}{KUN}.\p"
	.string "Well, let's meet again somewhere!$"

Route119_Text_BrendanIntro: @ 81F4C9A
	.string "Brendan: {PLAYER}! So this is where\n"
	.string "you've been looking for Pokémon?\p"
	.string "Let me see how good you got.\n"
	.string "I'll test you!\p"
	.string "Now!\n"
	.string "It's a battle, so battle!$"

Route119_Text_BrendanDefeat: @ 81F4D24
	.string "Hmm…\n"
	.string "You've gotten pretty darn decent.$"

Route119_Text_BrendanIllGiveYouThis: @ 81F4D4B
	.string "Brendan: I'd say you're good enough\n"
	.string "to search for Pokémon anywhere.\p"
	.string "I'll authorize your Flight Beacon.\p"
	.string "Earn the Feather Badge, and you can\n"
	.string "use it to revisit towns you know.$"

Route119_Text_BrendanExplainFly: @ 81F4DB5
	.string "Brendan: Anyway, I have to move along.$"

Route119_Text_ScottWayToGoBeSeeingYou: @ 81F4E60
	.string "Scott: Hahahah!\n"
	.string "Way to go, {PLAYER}{KUN}!\p"
	.string "I just passed by a Trainer riding\n"
	.string "a Bike.\p"
	.string "You just beat that Trainer, didn't you?\n"
	.string "It was pretty obvious that you did.\p"
	.string "The kid looked really upset with\n"
	.string "a face all red from anger.\p"
	.string "I sure seem to run into you often.\n"
	.string "Are you off to Fortree Gym next?\p"
	.string "I expect that you'll do well at the Gym.\n"
	.string "Well, I'll be seeing you!$"

Route119_Text_ScottYouWonAtFortreeGym: @ 81F4FBA
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "Scott: Hiya, {PLAYER}{KUN}, it's me!\p"
	.string "Just as I thought, you won at\n"
	.string "the Fortree Gym.\p"
	.string "Your strength…\n"
	.string "Perhaps you really are the Trainer\l"
	.string "that I've been searching for.\p"
	.string "Remember, you have a fan in me.\n"
	.string "I'll be cheering from the sidelines.\l"
	.string "Keep up the great work!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

Route119_Text_StayAwayFromWeatherInstitute: @ 81F50EB
	.string "We're standing lookout here.\p"
	.string "Hey, you! Stay away from the Weather\n"
	.string "Institute. It's not safe.$"

Route119_Text_DontGoNearWeatherInstitute: @ 81F5147
	.string "Lookout duty is surprisingly boring.\p"
	.string "Hey, you! Please don't go near the\n"
	.string "Weather Institute.$"

Route119_Text_ThoughtFlyByCatchingBirdMons: @ 81F51A2
	.string "I thought flying meant hanging\n"
	.string "on to a whole flock of birds!\p"
	.string "Your Flight Beacon sounds much\n"
	.string "easier. It works once your rival\l"
	.string "authorizes it and you earn the\l"
	.string "Feather Badge.$"

Route119_Text_TallGrassSnaresBikeTires: @ 81F5261
	.string "Tch…\n"
	.string "It's a no-go…\p"
	.string "The tall grass snares Bike tires.\n"
	.string "There's no way you can cycle here.$"

Route119_Text_CanYourMonMakeSecretBase: @ 81F52B9
	.string "Can your Pokémon use its Secret Power\n"
	.string "on a big pile of grass and make a\l"
	.string "Secret Base?$"

Route119_Text_RouteSignFortree: @ 81F530E
	.string "Route 119\n"
	.string "{RIGHT_ARROW} Fortree City$"

Route119_Text_WeatherInstitute: @ 81F5327
	.string "Weather Institute$"

Route119_Text_TrainerTipsDecoration: @ 81F5339
	.string "Trainer Tips\p"
	.string "Up to sixteen decorations and\n"
	.string "furniture items can be placed in\l"
	.string "a Secret Base.\p"
	.string "Pick your favorite items and create\n"
	.string "your very own Secret Base exactly the\l"
	.string "way you want.$"
