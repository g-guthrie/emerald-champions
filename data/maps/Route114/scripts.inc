Route114_MapScripts:: @ 81F252F
	map_script MAP_SCRIPT_ON_TRANSITION, Route114_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route114_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route114_OnFrame
	.byte 0

Route114_OnTransition: @ 81F253F
	compare VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	call_if_eq AbnormalWeather_EventScript_HideMapNamePopup
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_114_NORTH
	call_if_eq AbnormalWeather_StartGroudonWeather
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_114_SOUTH
	call_if_eq AbnormalWeather_StartGroudonWeather
	end

Route114_OnLoad: @ 81F2561
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_114_NORTH
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute114North
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_114_SOUTH
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute114South
	end

Route114_OnFrame: @ 81F2578
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route114_EventScript_GoodRodFisherman:: @ 81F3E14
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_GOOD_ROD, Route114_EventScript_ReceivedGoodRod
	msgbox Route114_Text_YouAgreeGoodRodIsGood, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route114_EventScript_DeclineGoodRod
	msgbox Route114_Text_IdenticalMindsTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_GOOD_ROD
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_GOOD_ROD
	msgbox Route114_Text_TryYourLuckFishing, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_DeclineGoodRod:: @ 81F3E5F
	msgbox Route114_Text_DontYouLikeToFish, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_ReceivedGoodRod:: @ 81F3E69
	msgbox Route114_Text_TryCatchingMonWithGoodRod, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_Man:: @ 81F2582
	lock
	faceplayer
	dotimebasedevents
	goto_if_set FLAG_DAILY_ROUTE_114_RECEIVED_BERRY, Route114_EventScript_ReceivedBerry
	msgbox Route114_Text_CollectBattleBerries, MSGBOX_DEFAULT
	random NUM_ROUTE_114_MAN_BERRIES
	addvar VAR_RESULT, NUM_ROUTE_114_MAN_BERRIES_SKIPPED
	addvar VAR_RESULT, FIRST_BERRY_INDEX
	giveitem VAR_RESULT
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_DAILY_ROUTE_114_RECEIVED_BERRY
	msgbox Route114_Text_GrowBattleBerries, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_ReceivedBerry:: @ 81F25C7
	msgbox Route114_Text_ComeBackForBerries, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_RoarGentleman:: @ 81F25D1
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TM05, Route114_EventScript_ReceivedPearl
	msgbox Route114_Text_RoarGentlemanGift, MSGBOX_DEFAULT
	giveitem ITEM_BIG_PEARL
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM05
	msgbox Route114_Text_PearlAndRoarTutor, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_ReceivedPearl:: @ 81F2608
	msgbox Route114_Text_PearlAndRoarTutor, MSGBOX_DEFAULT
	release
	end

Route114_EventScript_Poochyena:: @ 81F2612
	lock
	faceplayer
	waitse
	playmoncry SPECIES_POOCHYENA, 2
	msgbox Route114_Text_Poochyena, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Route114_EventScript_MeteorFallsSign:: @ 81F2625
	msgbox Route114_Text_MeteorFallsSign, MSGBOX_SIGN
	end

Route114_EventScript_FossilManiacsHouseSign:: @ 81F262E
	msgbox Route114_Text_FossilManiacsHouseSign, MSGBOX_SIGN
	end

Route114_EventScript_LanettesHouseSign:: @ 81F2637
	msgbox Route114_Text_LanettesHouse, MSGBOX_SIGN
	end

Route114_EventScript_Lenny:: @ 81F2640
	trainerbattle_double TRAINER_LENNY, Route114_Text_LennyIntro, Route114_Text_LennyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route114_Text_LennyPostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Lucas:: @ 81F2657
	trainerbattle_double TRAINER_LUCAS_1, Route114_Text_LucasIntro, Route114_Text_LucasDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route114_Text_LucasPostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Nancy:: @ 81F2685
	@ This retired encounter has no campaign battle.
	msgbox Route114_Text_NancyPostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Steve:: @ 81F269C
	trainerbattle_double TRAINER_STEVE_1, Route114_Text_SteveIntro, Route114_Text_SteveDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route114_Text_StevePostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Charlotte:: @ 81F27DB
	trainerbattle_double TRAINER_CHARLOTTE, Route114_Text_CharlotteIntro, Route114_Text_CharlotteDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route114_Text_CharlottePostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_Kai:: @ 81F27F2
	trainerbattle_double TRAINER_KAI, Route114_Text_KaiIntro, Route114_Text_KaiDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route114_Text_KaiPostBattle, MSGBOX_AUTOCLOSE
	end

Route114_Text_RoarGentlemanGift:
	.string "My Pokémon never stops barking!\p"
	.string "I found this Big Pearl while taking\n"
	.string "it for a walk. Please, take it!$"

Route114_Text_PearlAndRoarTutor:
	.string "Sell that Big Pearl for supplies.\p"
	.string "If you want your partner to learn\n"
	.string "Roar, visit the Move Tutor.$"

Route114_Text_Poochyena: @ 81F28A6
	.string "Bow! Bowwow!$"

Route114_Text_MeteorFallsSign: @ 81F28B3
	.string "Meteor Falls\n"
	.string "Rustboro City Through Here$"

Route114_Text_FossilManiacsHouseSign: @ 81F28DB
	.string "Fossil Maniac's House\n"
	.string "“Fossils gratefully accepted!”$"

Route114_Text_LanettesHouse: @ 81F2910
	.string "Lanette's House$"

Route114_Text_YouAgreeGoodRodIsGood: @ 81F427B
	.string "Hmm!\n"
	.string "A Good Rod is really good!\p"
	.string "Wouldn't you agree?$"

Route114_Text_IdenticalMindsTakeThis: @ 81F42AF
	.string "Hmm!\n"
	.string "We're of identical minds!\p"
	.string "Hmm!\n"
	.string "Take this Good Rod!$"

Route114_Text_TryYourLuckFishing: @ 81F42E7
	.string "Wherever there's water, try your luck\n"
	.string "at fishing.$"

Route114_Text_DontYouLikeToFish: @ 81F4319
	.string "Don't you like to fish?$"

Route114_Text_TryCatchingMonWithGoodRod: @ 81F4331
	.string "Try catching all sorts of Pokémon\n"
	.string "with your Good Rod.$"

