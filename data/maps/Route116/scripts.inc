Route116_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route116_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route116_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route116_OnFrame
	.byte 0

Route116_OnTransition:
	call_if_set FLAG_RECOVERED_DEVON_GOODS, Route116_EventScript_SetWandasBoyfriendPos
	call_if_eq VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_HideMapNamePopup
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_116_NORTH, AbnormalWeather_StartGroudonWeather
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_116_SOUTH, AbnormalWeather_StartGroudonWeather
	end

Route116_EventScript_SetWandasBoyfriendPos::
	setobjectxyperm LOCALID_ROUTE116_WANDAS_BF, 38, 10
	return

Route116_OnLoad:
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_116_NORTH, AbnormalWeather_EventScript_PlaceTilesRoute116North
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_116_SOUTH, AbnormalWeather_EventScript_PlaceTilesRoute116South
	end

Route116_OnFrame:
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route116_EventScript_WandasBoyfriend::
	lock
	faceplayer
	goto_if_set FLAG_RECOVERED_DEVON_GOODS, Route116_EventScript_BoyfriendGruntLeftTunnel
	goto_if_set FLAG_DEVON_GOODS_STOLEN, Route116_EventScript_BoyfriendGruntInTunnel
	msgbox Route116_Text_WantToDigTunnel, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_BoyfriendGruntLeftTunnel::
	msgbox Route116_Text_GoonHightailedItOutOfTunnel, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_BoyfriendGruntInTunnel::
	msgbox Route116_Text_DiggingTunnelWhenGoonOrderedMeOut, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_DevonEmployee::
	lock
	faceplayer
	goto_if_set FLAG_MET_DEVON_EMPLOYEE, Route116_EventScript_TryGiveRepeatBallAgain
	msgbox Route116_Text_ThankYouTokenOfAppreciation, MSGBOX_DEFAULT
	goto Route116_EventScript_GiveRepeatBall
	end

Route116_EventScript_GiveRepeatBall::
	setflag FLAG_MET_DEVON_EMPLOYEE
	giveitem ITEM_REPEAT_BALL
	goto_if_eq VAR_RESULT, FALSE, Route116_EventScript_NoRoomForRepeatBall
	msgbox Route116_Text_NewBallAvailableAtMart, MSGBOX_DEFAULT
	closemessage
	call_if_eq VAR_FACING, DIR_NORTH, Route116_EventScript_DevonEmployeeExit
	call_if_eq VAR_FACING, DIR_SOUTH, Route116_EventScript_DevonEmployeeExit
	call_if_eq VAR_FACING, DIR_WEST, Route116_EventScript_DevonEmployeeExit
	call_if_eq VAR_FACING, DIR_EAST, Route116_EventScript_DevonEmployeeExitEast
	removeobject VAR_LAST_TALKED
	clearflag FLAG_HIDE_RUSTBORO_CITY_DEVON_CORP_3F_EMPLOYEE
	setflag FLAG_RECEIVED_REPEAT_BALL
	release
	end

Route116_EventScript_DevonEmployeeExit::
	applymovement VAR_LAST_TALKED, Route116_Movement_DevonEmployeeExit
	waitmovement 0
	return

Route116_EventScript_DevonEmployeeExitEast::
	applymovement VAR_LAST_TALKED, Route116_Movement_DevonEmployeeExitEast
	waitmovement 0
	return

Route116_EventScript_TryGiveRepeatBallAgain::
	msgbox Route116_Text_TokenOfAppreciation, MSGBOX_DEFAULT
	goto Route116_EventScript_GiveRepeatBall
	end

Route116_EventScript_NoRoomForRepeatBall::
	msgbox Route116_Text_BagIsJamPacked, MSGBOX_DEFAULT
	release
	end

Route116_Movement_DevonEmployeeExit:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Route116_Movement_DevonEmployeeExitEast:
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Route116_EventScript_RouteSignRustboro::
	msgbox Route116_Text_RouteSignRustboro, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route116_EventScript_RusturfTunnelSign::
	msgbox Route116_Text_RusturfTunnelSign, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route116_EventScript_TunnelersRestHouseSign::
	msgbox Route116_Text_TunnelersRestHouse, MSGBOX_SIGN
	end

Route116_EventScript_TrainerTipsBToStopEvolution::
	msgbox Route116_Text_TrainerTipsBToStopEvolution, MSGBOX_SIGN
	end

Route116_EventScript_TrainerTipsBagHasPockets::
	msgbox Route116_Text_TrainerTipsBagHasPockets, MSGBOX_SIGN
	end

Route116_EventScript_Briney::
	lock
	faceplayer
	msgbox Route116_Text_ScoundrelMadeOffWithPeeko, MSGBOX_DEFAULT
	setvar VAR_ROUTE116_STATE, 2
	release
	end

Route116_EventScript_BrineyTrigger::
	lockall
	applymovement LOCALID_ROUTE116_BRINEY, Common_Movement_WalkInPlaceFasterRight
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	msgbox Route116_Text_ScoundrelMadeOffWithPeeko, MSGBOX_DEFAULT
	setvar VAR_ROUTE116_STATE, 2
	releaseall
	end

Route116_EventScript_DuskStoneSeeker::
	lock
	faceplayer
	checkitem ITEM_DUSK_STONE
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_PlayerHasDuskStone
	specialvar VAR_RESULT, FoundRoute116DuskStone
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_DuskStoneAlreadyUsed
	msgbox Route116_Text_CanYouHelpMeFindDuskStone, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_DuskStoneAlreadyUsed::
	msgbox Route116_Text_DuskStoneAlreadyUsed, MSGBOX_DEFAULT
	closemessage
	goto Route116_EventScript_DuskStoneSeekerExit
	end

Route116_EventScript_PlayerHasDuskStone::
	msgbox Route116_Text_CanYouHelpMeFindDuskStone, MSGBOX_DEFAULT
	msgbox Route116_Text_MayISeeThatDuskStone, MSGBOX_DEFAULT
	specialvar VAR_RESULT, FoundRoute116DuskStone
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_DuskStoneFound
	msgbox Route116_Text_DuskStoneNotYetFound, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_DuskStoneFound::
	msgbox Route116_Text_DuskStoneIsYours, MSGBOX_DEFAULT
	closemessage
	goto Route116_EventScript_DuskStoneSeekerExit
	end

Route116_EventScript_DuskStoneSeekerExit::
	delay 20
	call_if_eq VAR_FACING, DIR_NORTH, Route116_EventScript_DuskStoneSeekerExitNormal
	call_if_eq VAR_FACING, DIR_SOUTH, Route116_EventScript_DuskStoneSeekerExitNormal
	call_if_eq VAR_FACING, DIR_WEST, Route116_EventScript_DuskStoneSeekerExitNormal
	call_if_eq VAR_FACING, DIR_EAST, Route116_EventScript_DuskStoneSeekerExitEast
	setflag FLAG_HIDE_ROUTE_116_DUSK_STONE_SEEKER
	removeobject VAR_LAST_TALKED
	release
	end

Route116_EventScript_DuskStoneSeekerExitNormal::
	applymovement VAR_LAST_TALKED, Route116_Movement_DuskStoneSeekerExit
	waitmovement 0
	return

Route116_EventScript_DuskStoneSeekerExitEast::
	applymovement VAR_LAST_TALKED, Route116_Movement_DuskStoneSeekerExitEast
	waitmovement 0
	return

Route116_Movement_DuskStoneSeekerExit:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Route116_Movement_DuskStoneSeekerExitEast:
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

Route116_EventScript_Joey::
	trainerbattle_double TRAINER_JOEY, Route116_Text_JoeyIntro, Route116_Text_JoeyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route116_Text_JoeyPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Jose::
	trainerbattle_double TRAINER_JOSE, Route116_Text_JoseIntro, Route116_Text_JoseDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route116_Text_JosePostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Jerry::
	trainerbattle_single TRAINER_JERRY_1, Route116_Text_JerryIntro, Route116_Text_JerryDefeat, Route116_EventScript_TryRegisterJerryAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_RematchJerry
	setvar VAR_0x8004, TRAINER_JERRY_1
	specialvar VAR_RESULT, IsTrainerRegistered
	goto_if_eq VAR_RESULT, FALSE, Route116_EventScript_TryRegisterJerry
	msgbox Route116_Text_JerryPostBattle, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_TryRegisterJerryAfterBattle::
	goto_if_set FLAG_HAS_MATCH_CALL, Route116_EventScript_RegisterJerryAfterBattle
	release
	end

Route116_EventScript_RegisterJerryAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route116_Text_JerryRegister2, MSGBOX_DEFAULT
	register_matchcall TRAINER_JERRY_1
	release
	end

Route116_EventScript_TryRegisterJerry::
	goto_if_set FLAG_HAS_MATCH_CALL, Route116_EventScript_RegisterJerry
	msgbox Route116_Text_JerryPostBattle, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_RegisterJerry::
	msgbox Route116_Text_JerryRegister1, MSGBOX_DEFAULT
	register_matchcall TRAINER_JERRY_1
	release
	end

Route116_EventScript_RematchJerry::
	trainerbattle_rematch TRAINER_JERRY_1, Route116_Text_JerryRematchIntro, Route116_Text_JerryRematchDefeat
	msgbox Route116_Text_JerryPostRematch, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Clark::
	trainerbattle_double TRAINER_CLARK, Route116_Text_ClarkIntro, Route116_Text_ClarkDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route116_Text_ClarkPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Janice::
	trainerbattle_double TRAINER_JANICE, Route116_Text_JaniceIntro, Route116_Text_JaniceDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route116_Text_JanicePostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Karen::
	trainerbattle_double TRAINER_KAREN_1, Route116_Text_KarenIntro, Route116_Text_KarenDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route116_EventScript_TryRegisterKarenAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route116_EventScript_RematchKaren
	setvar VAR_0x8004, TRAINER_KAREN_1
	specialvar VAR_RESULT, IsTrainerRegistered
	goto_if_eq VAR_RESULT, FALSE, Route116_EventScript_TryRegisterKaren
	msgbox Route116_Text_KarenPostBattle, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_TryRegisterKarenAfterBattle::
	goto_if_set FLAG_HAS_MATCH_CALL, Route116_EventScript_RegisterKarenAfterBattle
	release
	end

Route116_EventScript_RegisterKarenAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route116_Text_KarenRegister2, MSGBOX_DEFAULT
	register_matchcall TRAINER_KAREN_1
	release
	end

Route116_EventScript_TryRegisterKaren::
	goto_if_set FLAG_HAS_MATCH_CALL, Route116_EventScript_RegisterKaren
	msgbox Route116_Text_KarenPostBattle, MSGBOX_DEFAULT
	release
	end

Route116_EventScript_RegisterKaren::
	msgbox Route116_Text_KarenRegister1, MSGBOX_DEFAULT
	register_matchcall TRAINER_KAREN_1
	release
	end

Route116_EventScript_RematchKaren::
	trainerbattle_rematch_double TRAINER_KAREN_1, Route116_Text_KarenRematchIntro, Route116_Text_KarenRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route116_Text_KarenPostRematch, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Sarah::
	trainerbattle_double TRAINER_SARAH, Route116_Text_SarahIntro, Route116_Text_SarahDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route116_Text_SarahPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Dawson::
	trainerbattle_single TRAINER_DAWSON, Route116_Text_DawsonIntro, Route116_Text_DawsonDefeat
	msgbox Route116_Text_DawsonPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Devan::
	trainerbattle_double TRAINER_DEVAN, Route116_Text_DevanIntro, Route116_Text_DevanDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route116_Text_DevanPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_EventScript_Johnson::
	trainerbattle_double TRAINER_JOHNSON, Route116_Text_JohnsonIntro, Route116_Text_JohnsonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route116_Text_JohnsonPostBattle, MSGBOX_AUTOCLOSE
	end

Route116_Text_ScoundrelMadeOffWithPeeko:
	.string "Ohhh, what am I to do?\p"
	.string "We were on our walk, PEEKO and I, when\n"
	.string "we were jumped by an odd thug…\p"
	.string "The scoundrel made off with my\n"
	.string "darling PEEKO!\p"
	.string "Wrrrooooooaaaar! PEEKO!$"

Route116_Text_WantToDigTunnel:
	.string "Nnn… Roar!\p"
	.string "I want to dig that tunnel!$"

Route116_Text_DiggingTunnelWhenGoonOrderedMeOut:
	.string "Nnn… Roar!\n"
	.string "What's going on?\p"
	.string "I was digging the tunnel without any\n"
	.string "tools when some goon ordered me out!\p"
	.string "That tunnel's filled with POKéMON\n"
	.string "that react badly to loud noises.\l"
	.string "They could cause an uproar.\p"
	.string "That's why we stopped using heavy\n"
	.string "equipment for tunneling…\p"
	.string "I'm worried that the goon will do\n"
	.string "something stupid and startle the\l"
	.string "POKéMON into an uproar.$"

Route116_Text_GoonHightailedItOutOfTunnel:
	.string "Nnn… Roar!\p"
	.string "That goofy goon hightailed it out of\n"
	.string "the tunnel! I can go back to digging!$"

Route116_Text_ThankYouTokenOfAppreciation:
	.string "Oh! It's you!\p"
	.string "You're that person who not only helped\n"
	.string "me in PETALBURG WOODS, but also got\l"
	.string "back my stolen package and then even\l"
	.string "graciously delivered it to SLATEPORT!\p"
	.string "CAPT. STERN informed us that the\n"
	.string "package arrived, too!\p"
	.string "I thank you very, very much!\p"
	.string "Why, if I were a rain cloud, I would\n"
	.string "shower you with gratitude and hail\l"
	.string "you as a hero!\p"
	.string "For you, the most wonderful TRAINER,\n"
	.string "I bear good news!\p"
	.string "Recently, our company developed\n"
	.string "a new kind of POKé BALL.\p"
	.string "As a token of our appreciation, this\n"
	.string "is our gift to our wonderful TRAINER!$"

Route116_Text_NewBallAvailableAtMart:
	.string "Our new POKé BALL will be available\n"
	.string "at the POKéMON MART in RUSTBORO.\p"
	.string "Please do try it out!\n"
	.string "Thank you and bye-bye!$"

Route116_Text_BagIsJamPacked:
	.string "Your BAG is jam-packed.\n"
	.string "I can't give you this REPEAT BALL.$"

Route116_Text_TokenOfAppreciation:
	.string "As a token of our appreciation for\n"
	.string "your delivering our package, I have\l"
	.string "a gift of a new kind of POKé BALL\l"
	.string "for our most wonderful TRAINER!$"

Route116_Text_CanYouHelpMeFindDuskStone:
	.string "I dropped a dark stone with a purple\n"
	.string "gleam…\p"
	.string "Can you help me find it?$"

Route116_Text_MayISeeThatDuskStone:
	.string "That DUSK STONE!\n"
	.string "May I see it for a second?$"

Route116_Text_DuskStoneIsYours:
	.string "Hmm… Mine bore a crescent scratch.\n"
	.string "This one is different.\p"
	.string "It seems to have chosen you. Keep it--\n"
	.string "some POKéMON evolve in its darkness.$"

Route116_Text_DuskStoneAlreadyUsed:
	.string "The purple gleam that was here is gone.\p"
	.string "Perhaps a TRAINER found it and gave it\n"
	.string "purpose. I'll search on.$"

Route116_Text_DuskStoneNotYetFound:
	.string "A lovely DUSK STONE, but not the one\n"
	.string "gleaming somewhere along this path.$"

Route116_Text_RouteSignRustboro:
	.string "ROUTE 116\n"
	.string "{LEFT_ARROW} RUSTBORO CITY$"

Route116_Text_RusturfTunnelSign:
	.string "RUSTURF TUNNEL\n"
	.string "“Linking RUSTBORO and VERDANTURF\p"
	.string "“The tunnel project has been\n"
	.string "canceled.”$"

Route116_Text_TunnelersRestHouse:
	.string "TUNNELER'S REST HOUSE$"

Route116_Text_TrainerTipsBToStopEvolution:
	.string "TRAINER TIPS\p"
	.string "If you want to stop a POKéMON from\n"
	.string "evolving, press the B Button while it\l"
	.string "is trying to evolve.\l"
	.string "The startled POKéMON will stop.\p"
	.string "This is called an evolution cancel.$"

Route116_Text_TrainerTipsBagHasPockets:
	.string "TRAINER TIPS\p"
	.string "Your BAG has several POCKETS.\p"
	.string "Items you obtain are automatically\n"
	.string "placed in the appropriate POCKETS.\p"
	.string "No TRAINER can afford to be without\n"
	.string "a BAG of their own.$"
