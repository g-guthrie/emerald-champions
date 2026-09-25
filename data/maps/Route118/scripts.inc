.set LOCALID_STEVEN, 14

Route118_MapScripts:: @ 81F3DBC
	map_script MAP_SCRIPT_ON_TRANSITION, Route118_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Route118_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route118_OnFrame
	.byte 0

Route118_OnTransition: @ 81F3DCC
	call GabbyAndTy_EventScript_UpdateLocation
	compare VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	call_if_eq AbnormalWeather_EventScript_HideMapNamePopup
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_118_EAST
	call_if_eq AbnormalWeather_StartGroudonWeather
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_118_WEST
	call_if_eq AbnormalWeather_StartGroudonWeather
	end

Route118_OnLoad: @ 81F3DF3
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_118_EAST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute118East
	compare VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_ROUTE_118_WEST
	call_if_eq AbnormalWeather_EventScript_PlaceTilesRoute118West
	end

Route118_OnFrame: @ 81F3E0A
	map_script_2 VAR_SHOULD_END_ABNORMAL_WEATHER, 1, AbnormalWeather_EventScript_EndEventAndCleanup_1
	.2byte 0

Route118_EventScript_Girl:: @ 81F3E73
	msgbox Route118_Text_CanCrossRiversWithSurf, MSGBOX_NPC
	end

Route118_EventScript_RouteSignMauville:: @ 81F3E7C
	msgbox Route118_Text_RouteSignMauville, MSGBOX_SIGN
	end

Route118_EventScript_RouteSign119:: @ 81F3E85
	msgbox Route118_Text_RouteSign119, MSGBOX_SIGN
	end

Route118_EventScript_StevenTrigger0:: @ 81F3E8E
	lockall
	setvar VAR_0x8008, 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceUp
	waitmovement 0
	applymovement LOCALID_STEVEN, Route118_Movement_StevenApproachLedge0
	waitmovement 0
	goto Route118_EventScript_StevenTrigger
	end

Route118_EventScript_StevenTrigger1:: @ 81F3EAE
	lockall
	setvar VAR_0x8008, 1
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceUp
	waitmovement 0
	goto Route118_EventScript_StevenTrigger
	end

Route118_EventScript_StevenTrigger2:: @ 81F3EC4
	lockall
	setvar VAR_0x8008, 2
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceUp
	waitmovement 0
	applymovement LOCALID_STEVEN, Route118_Movement_StevenApproachLedge2
	waitmovement 0
	goto Route118_EventScript_StevenTrigger
	end

Route118_EventScript_StevenTrigger:: @ 81F3EE4
	playse SE_LEDGE
	applymovement LOCALID_STEVEN, Route118_Movement_StevenJumpLedge
	waitmovement 0
	delay 30
	msgbox Route118_Text_StevenQuestions, MSGBOX_DEFAULT
	closemessage
	compare VAR_0x8008, 0
	call_if_eq Route118_EventScript_StevenExit0
	compare VAR_0x8008, 1
	call_if_eq Route118_EventScript_StevenExit1
	compare VAR_0x8008, 2
	call_if_eq Route118_EventScript_StevenExit2
	setvar VAR_ROUTE118_STATE, 1
	removeobject LOCALID_STEVEN
	releaseall
	end

Route118_EventScript_StevenExit0:: @ 81F3F28
	applymovement OBJ_EVENT_ID_PLAYER, Route118_Movement_PlayerWatchStevenExit
	applymovement LOCALID_STEVEN, Route118_Movement_StevenExit0
	waitmovement 0
	return

Route118_EventScript_StevenExit1:: @ 81F3F3A
	applymovement OBJ_EVENT_ID_PLAYER, Route118_Movement_PlayerWatchStevenExit
	applymovement LOCALID_STEVEN, Route118_Movement_StevenExit1
	waitmovement 0
	return

Route118_EventScript_StevenExit2:: @ 81F3F4C
	applymovement OBJ_EVENT_ID_PLAYER, Route118_Movement_PlayerWatchStevenExit
	applymovement LOCALID_STEVEN, Route118_Movement_StevenExit2
	waitmovement 0
	return

Route118_Movement_PlayerWatchStevenExit: @ 81F3F5E
	delay_16
	walk_in_place_fast_right
	step_end

Route118_Movement_StevenApproachLedge0: @ 81F3F61
	walk_left
	step_end

Route118_Movement_StevenApproachLedge2: @ 81F3F63
	walk_right
	step_end

Route118_Movement_StevenJumpLedge: @ 81F3F65
	jump_2_down
	delay_16
	walk_down
	step_end

Route118_Movement_StevenExit0: @ 81F3F69
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Route118_Movement_StevenExit1: @ 81F3F74
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Route118_Movement_StevenExit2: @ 81F3F7E
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Route118_EventScript_Rose:: @ 81F3F87
	trainerbattle_double TRAINER_ROSE_1, Route118_Text_RoseIntro, Route118_Text_RoseDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route118_Text_RosePostBattle, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_Barny:: @ 81F3FE9
	@ This retired encounter has no campaign battle.
	msgbox Route118_Text_BarnyPostBattle, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_Wade:: @ 81F4000
	@ This retired encounter has no campaign battle.
	msgbox Route118_Text_WadePostBattle, MSGBOX_AUTOCLOSE
	end

Route118_EventScript_Deandre:: @ 81F40A7
	trainerbattle_double TRAINER_DEANDRE, Route118_Text_DeandreIntro, Route118_Text_DeandreDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route118_Text_DeandrePostBattle, MSGBOX_AUTOCLOSE
	end

Route114_EventScript_MagikarpGuy::
	lock
	faceplayer
Route118_EventScript_CheckGyaradositeState::
	goto_if_set FLAG_ROUTE118_GYARADOSITE, Route118_EventScript_ReceivedGyaradosite
	goto_if_defeated TRAINER_MAGIKARP_GUY, Route118_EventScript_GiveGyaradosite
Route118_EventScript_OfferMagikarpChallenge::
	msgbox Route118_Text_DoYouLiekKarp, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route118_EventScript_ILuurveKarp
	compare VAR_RESULT, NO
	goto_if_eq Route118_EventScript_DontLikeKarp
	release
	end

Route118_EventScript_ReceivedGyaradosite::
	msgbox Route118_Text_KarpBetterThanGyara, MSGBOX_DEFAULT
	release
	end

Route118_EventScript_ILuurveKarp::
	msgbox Route118_Text_ILuurveKarp, MSGBOX_DEFAULT
	closemessage
	msgbox Route118_Text_ReadyForKarpFite, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route118_EventScript_CheckParty
	release
	end

Route118_EventScript_DontLikeKarp::
	msgbox Route118_Text_DontLikeKarp, MSGBOX_DEFAULT
	release
	end

Route118_EventScript_CheckParty::
	specialvar VAR_RESULT, CheckMagikarpBattle
	compare VAR_RESULT, FALSE
	goto_if_eq Route118_EventScript_NeedMoreKarp
	msgbox Route118_Text_KarpFiteStart, MSGBOX_DEFAULT
	trainerbattle_no_intro_double TRAINER_MAGIKARP_GUY, Route118_Text_MagikarpDefeat
Route118_EventScript_GiveGyaradosite::
	msgbox Route118_Text_TakeGyaradosite, MSGBOX_DEFAULT
	giveitem ITEM_GYARADOSITE
Route118_EventScript_CheckGyaradositeReceipt::
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_ROUTE118_GYARADOSITE
	goto Route118_EventScript_ReceivedGyaradosite

Route118_EventScript_NeedMoreKarp::
	msgbox Route118_Text_NeedMoreKarp, MSGBOX_DEFAULT
	release
	end

Route118_Text_DoYouLiekKarp:
	.string "Do you like the Pokémon Magikarp?$"

Route118_Text_DontLikeKarp:
	.string "Oh… That's disappointing.\n"
	.string "I was hoping you were a fellow\l"
	.string "Magikarp aficionado…$"

Route118_Text_ILuurveKarp:
	.string "You do?!\n"
	.string "That's great!\p"
	.string "Not many people appreciate the true\n"
	.string "majesty of Magikarp…\p"
	.string "Oh, I know! If you can show me you\n"
	.string "truly love Magikarp, I'll give you this\l"
	.string "Gyaradosite I found!\p"
	.string "Here's what I want you to do…\p"
	.string "You must defeat my team of Magikarp\n"
	.string "all stars…\p"
	.string "With your own team of Magikarp!$"

Route118_Text_ReadyForKarpFite:
	.string "So, are you ready for our magical\n"
	.string "showdown?$"

Route118_Text_NeedMoreKarp:
	.string "Hmm… You don't have enough Magikarp\n"
	.string "to challenge me.\p"
	.string "Please come back once you've fished up\n"
	.string "a full team of them!$"

Route118_Text_KarpFiteStart:
	.string "Oho! Then let us begin!$"

Route118_Text_MagikarpDefeat:
	.string "Ah! Your love for Magikarp is clear!$"

Route118_Text_TakeGyaradosite:
	.string "Here, take this Gyaradosite.\n"
	.string "You earned it!$"

Route118_Text_KarpBetterThanGyara:
	.string "Gyarados might not be Magikarp, but\n"
	.string "it's almost as good!$"

Route118_Text_StevenQuestions: @ 81F40BE
	.string "Steven: Hi, {PLAYER}{KUN}!\p"
	.string "It's me, Steven!\n"
	.string "We met in the cave near Dewford.\p"
	.string "Have you met many kinds of Pokémon\n"
	.string "since I last saw you?\p"
	.string "There are many, many kinds of Pokémon\n"
	.string "in this wide world of ours.\p"
	.string "If you wanted to raise only your\n"
	.string "favorites, that's fine.\p"
	.string "Perhaps you find it more fun to raise\n"
	.string "all sorts of Pokémon.\p"
	.string "As a Trainer of Pokémon,\n"
	.string "what do you think?\p"
	.string "Of course, it's not any of my business.\p"
	.string "It would be nice if we were to meet\n"
	.string "again somewhere.$"

Route118_Text_CanCrossRiversWithSurf: @ 81F4367
	.string "With the Balance Badge and Surf\n"
	.string "authorization, you can cross rivers\l"
	.string "and the sea.\p"
	.string "Bring a Pokémon able to learn\n"
	.string "Surf; it needn't know the move.\p"
	.string "Face the water and press A!$"

Route118_Text_RouteSignMauville: @ 81F43F4
	.string "Route 118\n"
	.string "{LEFT_ARROW} Mauville City$"

Route118_Text_RouteSign119: @ 81F440E
	.string "Route 118\n"
	.string "{UP_ARROW} Route 119$"
