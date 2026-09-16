LavaridgeTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, LavaridgeTown_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, LavaridgeTown_OnFrame
	.byte 0

LavaridgeTown_OnTransition:
	setflag FLAG_VISITED_LAVARIDGE_TOWN
	call_if_set FLAG_WHITEOUT_TO_LAVARIDGE, LavaridgeTown_EventScript_ClearLavaridgeWhiteOut
	call_if_set FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, LavaridgeTown_EventScript_ShowMtChimneyTrainers
	call Common_EventScript_SetupRivalGfxId
	call Common_EventScript_SetupRivalOnBikeGfxId
	call_if_eq VAR_LAVARIDGE_TOWN_STATE, 3, LavaridgeTown_EventScript_SetPendingRivalPos
	call_if_eq VAR_LAVARIDGE_TOWN_STATE, 1, LavaridgeTown_EventScript_CheckSetRivalPos
	call_if_eq VAR_LAVARIDGE_TOWN_STATE, 1, LavaridgeTown_EventScript_HideMapNamePopup
	end

@ Pending delivery is an optional conversation, never an on-frame loop.
LavaridgeTown_EventScript_SetPendingRivalPos::
	setobjectxyperm LOCALID_LAVARIDGE_RIVAL, 12, 16
	setobjectmovementtype LOCALID_LAVARIDGE_RIVAL, MOVEMENT_TYPE_FACE_LEFT
	clearflag FLAG_HIDE_LAVARIDGE_TOWN_RIVAL
	return

LavaridgeTown_EventScript_RetryGoGoggles::
	lockall
	faceplayer
	goto_if_ne VAR_LAVARIDGE_TOWN_STATE, 3, LavaridgeTown_EventScript_EndPendingTalk
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, LavaridgeTown_EventScript_MayGiveGoGoggles
	goto LavaridgeTown_EventScript_BrendanGiveGoGoggles

LavaridgeTown_EventScript_EndPendingTalk::
	releaseall
	end

LavaridgeTown_EventScript_ClearLavaridgeWhiteOut::
	clearflag FLAG_WHITEOUT_TO_LAVARIDGE
	return

LavaridgeTown_EventScript_CheckSetRivalPos::
	getplayerxy VAR_0x8004, VAR_0x8005
	goto_if_eq VAR_0x8004, 9, LavaridgeTown_EventScript_SetRivalPos
	return

LavaridgeTown_EventScript_SetRivalPos::
	setobjectxyperm LOCALID_LAVARIDGE_RIVAL, 11, 9
	setobjectxyperm LOCALID_LAVARIDGE_RIVAL_ON_BIKE, 9, 8
	setobjectmovementtype LOCALID_LAVARIDGE_RIVAL_ON_BIKE, MOVEMENT_TYPE_FACE_UP
	clearflag FLAG_HIDE_LAVARIDGE_TOWN_RIVAL
	return

LavaridgeTown_EventScript_ShowMtChimneyTrainers::
	clearflag FLAG_HIDE_MT_CHIMNEY_TRAINERS
	return

LavaridgeTown_EventScript_HideMapNamePopup::
	setflag FLAG_HIDE_MAP_NAME_POPUP
	return

LavaridgeTown_OnFrame:
	map_script_2 VAR_LAVARIDGE_TOWN_STATE, 1, LavaridgeTown_EventScript_RivalGiveGoGoggles
	.2byte 0

LavaridgeTown_EventScript_RivalGiveGoGoggles::
	lockall
	getplayerxy VAR_0x8008, VAR_0x8009
	call_if_eq VAR_0x8008, 9, LavaridgeTown_EventScript_RivalNoticePlayer
	call_if_ne VAR_0x8008, 9, LavaridgeTown_EventScript_RivalExitHerbShop
	delay 20
	checkplayergender
	call_if_eq VAR_RESULT, MALE, LavaridgeTown_EventScript_PlayMayMusic
	call_if_eq VAR_RESULT, FEMALE, LavaridgeTown_EventScript_PlayBrendanMusic
	call_if_eq VAR_0x8008, 9, LavaridgeTown_EventScript_RivalApproachPlayer1
	call_if_ne VAR_0x8008, 9, LavaridgeTown_EventScript_RivalApproachPlayer2
	checkplayergender
	goto_if_eq VAR_RESULT, MALE, LavaridgeTown_EventScript_MayGiveGoGoggles
	goto_if_eq VAR_RESULT, FEMALE, LavaridgeTown_EventScript_BrendanGiveGoGoggles
	end

LavaridgeTown_EventScript_MayGiveGoGoggles::
	msgbox LavaridgeTown_Text_MayNiceBadgesTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_GO_GOGGLES
	goto_if_eq VAR_RESULT, FALSE, LavaridgeTown_EventScript_ArchiveGoGogglesMay
	goto LavaridgeTown_EventScript_MayReceivedGoGoggles
	end

LavaridgeTown_EventScript_ArchiveGoGogglesMay::
	addpcitem ITEM_GO_GOGGLES
	goto_if_eq VAR_RESULT, FALSE, LavaridgeTown_EventScript_GoGogglesNoRoom
	msgbox LavaridgeTown_Text_GoGogglesSentToPC, MSGBOX_DEFAULT

LavaridgeTown_EventScript_MayReceivedGoGoggles::
	setflag FLAG_RECEIVED_GO_GOGGLES
	msgbox LavaridgeTown_Text_ExplainGoGoggles, MSGBOX_DEFAULT
	call LavaridgeTown_EventScript_MountainHandoff
	goto LavaridgeTown_EventScript_RivalExit
	end

LavaridgeTown_EventScript_BrendanGiveGoGoggles::
	msgbox LavaridgeTown_Text_BrendanNiceBadgesTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_GO_GOGGLES
	goto_if_eq VAR_RESULT, FALSE, LavaridgeTown_EventScript_ArchiveGoGogglesBrendan
	goto LavaridgeTown_EventScript_BrendanReceivedGoGoggles
	end

LavaridgeTown_EventScript_ArchiveGoGogglesBrendan::
	addpcitem ITEM_GO_GOGGLES
	goto_if_eq VAR_RESULT, FALSE, LavaridgeTown_EventScript_GoGogglesNoRoom
	msgbox LavaridgeTown_Text_GoGogglesSentToPC, MSGBOX_DEFAULT

LavaridgeTown_EventScript_BrendanReceivedGoGoggles::
	setflag FLAG_RECEIVED_GO_GOGGLES
	msgbox LavaridgeTown_Text_ExplainGoGoggles, MSGBOX_DEFAULT
	call LavaridgeTown_EventScript_MountainHandoff
	goto LavaridgeTown_EventScript_RivalExit
	end

LavaridgeTown_EventScript_GoGogglesNoRoom::
	setvar VAR_LAVARIDGE_TOWN_STATE, 3
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	msgbox LavaridgeTown_Text_GoGogglesNoRoom, MSGBOX_DEFAULT
	savebgm MUS_DUMMY
	fadedefaultbgm
	releaseall
	end

LavaridgeTown_EventScript_RivalExit::
	goto_if_eq VAR_LAVARIDGE_TOWN_STATE, 3, LavaridgeTown_EventScript_EndPendingDelivery
	closemessage
	removeobject LOCALID_LAVARIDGE_RIVAL
	addobject LOCALID_LAVARIDGE_RIVAL_ON_BIKE
	delay 30
	call_if_eq VAR_0x8008, 9, LavaridgeTown_EventScript_RivalExit1
	call_if_ne VAR_0x8008, 9, LavaridgeTown_EventScript_RivalExit2
	removeobject LOCALID_LAVARIDGE_RIVAL_ON_BIKE
	setvar VAR_LAVARIDGE_TOWN_STATE, 2
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	savebgm MUS_DUMMY
	fadedefaultbgm
	releaseall
	end

LavaridgeTown_EventScript_EndPendingDelivery::
	closemessage
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_LAVARIDGE_RIVAL
	removeobject LOCALID_LAVARIDGE_RIVAL_ON_BIKE
	setvar VAR_LAVARIDGE_TOWN_STATE, 2
	clearflag FLAG_HIDE_MAP_NAME_POPUP
	savebgm MUS_DUMMY
	fadedefaultbgm
	fadescreen FADE_FROM_BLACK
	releaseall
	end

LavaridgeTown_EventScript_PlayMayMusic::
	playbgm MUS_ENCOUNTER_MAY, TRUE
	return

LavaridgeTown_EventScript_PlayBrendanMusic::
	playbgm MUS_ENCOUNTER_BRENDAN, TRUE
	return

LavaridgeTown_EventScript_RivalNoticePlayer::
	applymovement LOCALID_LAVARIDGE_RIVAL, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	playse SE_PIN
	applymovement LOCALID_LAVARIDGE_RIVAL, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_LAVARIDGE_RIVAL, Common_Movement_Delay48
	waitmovement 0
	return

LavaridgeTown_EventScript_RivalExitHerbShop::
	opendoor 12, 15
	waitdooranim
	addobject LOCALID_LAVARIDGE_RIVAL
	applymovement LOCALID_LAVARIDGE_RIVAL, LavaridgeTown_Movement_RivalExitHerbShop
	waitmovement 0
	closedoor 12, 15
	waitdooranim
	applymovement LOCALID_LAVARIDGE_RIVAL, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	playse SE_PIN
	applymovement LOCALID_LAVARIDGE_RIVAL, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_LAVARIDGE_RIVAL, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	return

LavaridgeTown_EventScript_RivalApproachPlayer1::
	applymovement LOCALID_LAVARIDGE_RIVAL, LavaridgeTown_Movement_RivalApproachPlayer1
	waitmovement 0
	return

LavaridgeTown_EventScript_RivalApproachPlayer2::
	applymovement LOCALID_LAVARIDGE_RIVAL, LavaridgeTown_Movement_RivalApproachPlayer2
	waitmovement 0
	return

LavaridgeTown_EventScript_RivalExit1::
	applymovement LOCALID_PLAYER, LavaridgeTown_Movement_PlayerWatchRivalExit
	applymovement LOCALID_LAVARIDGE_RIVAL_ON_BIKE, LavaridgeTown_Movement_RivalExit1
	waitmovement 0
	return

LavaridgeTown_EventScript_RivalExit2::
	applymovement LOCALID_LAVARIDGE_RIVAL_ON_BIKE, LavaridgeTown_Movement_RivalExit2
	waitmovement 0
	return

LavaridgeTown_Movement_RivalExit2:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

LavaridgeTown_Movement_PlayerWatchRivalExit:
	delay_16
	delay_8
	walk_in_place_faster_right
	step_end

LavaridgeTown_Movement_RivalExit1:
	walk_fast_down
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

LavaridgeTown_Movement_RivalApproachPlayer2:
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

LavaridgeTown_Movement_RivalApproachPlayer1:
	walk_left
	walk_left
	walk_up
	step_end

LavaridgeTown_Movement_RivalExitHerbShop:
	walk_down
	step_end

LavaridgeTown_EventScript_HotSpringsTrigger::
	specialvar VAR_RESULT, GetPlayerFacingDirection
	goto_if_eq VAR_RESULT, DIR_SOUTH, LavaridgeTown_EventScript_EnteredHotSprings
	end

LavaridgeTown_EventScript_EnteredHotSprings::
	incrementgamestat GAME_STAT_ENTERED_HOT_SPRINGS
	end

LavaridgeTown_EventScript_ExpertM::
	msgbox LavaridgeTown_Text_HotSpringsNeverRunDry, MSGBOX_NPC
	end

LavaridgeTown_EventScript_OldMan::
	msgbox LavaridgeTown_Text_PokemonNippedBackside, MSGBOX_SIGN
	end

LavaridgeTown_EventScript_Twin::
	msgbox LavaridgeTown_Text_BatheInHotSpringsEveryDay, MSGBOX_NPC
	end

LavaridgeTown_EventScript_HotSpringsOldWoman1::
	msgbox LavaridgeTown_Text_IfPokemonInHotSprings, MSGBOX_NPC
	end

LavaridgeTown_EventScript_HotSpringsOldWoman2::
	msgbox LavaridgeTown_Text_HotSpringsClaims, MSGBOX_NPC
	end

LavaridgeTown_EventScript_ExpertF::
	msgbox LavaridgeTown_Text_OhYouLikeHotSprings, MSGBOX_NPC
	end

LavaridgeTown_EventScript_EggWoman::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_LAVARIDGE_EGG, LavaridgeTown_EventScript_ReceivedEgg
	msgbox LavaridgeTown_Text_HaveEggWillYouTakeIt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, LavaridgeTown_EventScript_DeclineEgg
	getpartysize
	goto_if_eq VAR_RESULT, PARTY_SIZE, LavaridgeTown_EventScript_NoRoomForEgg
	msgbox LavaridgeTown_Text_HopeYoullWalkPlentyWithEgg, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_LAVARIDGE_EGG
	playfanfare MUS_OBTAIN_ITEM
	message LavaridgeTown_Text_ReceivedTheEgg
	waitfanfare
	giveegg SPECIES_WYNAUT
	goto LavaridgeTown_EventScript_MegaGift_EMBOARITE
	end

LavaridgeTown_EventScript_ReceivedEgg::
	msgbox LavaridgeTown_Text_EverySoOftenEggFoundAtDayCare, MSGBOX_DEFAULT
LavaridgeTown_EventScript_MegaGift_EMBOARITE::
	goto_if_unset FLAG_BADGE04_GET, LavaridgeTown_EventScript_EggWomanRelease
	goto_if_set FLAG_EC_MEGA_GIFT_EMBOARITE, LavaridgeTown_EventScript_EggWomanRelease
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, LavaridgeTown_EventScript_EggWomanRelease
	msgbox LavaridgeTown_Text_MegaGift_EMBOARITE, MSGBOX_DEFAULT
	giveuniqueitem ITEM_EMBOARITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_EMBOARITE
LavaridgeTown_EventScript_EggWomanRelease::
	release
	end

LavaridgeTown_EventScript_NoRoomForEgg::
	msgbox LavaridgeTown_Text_NoRoomForThisEgg, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_EventScript_DeclineEgg::
	msgbox LavaridgeTown_Text_AsYouWishThen, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_EventScript_TownSign::
	msgbox LavaridgeTown_Text_TownSign, MSGBOX_SIGN
	end

LavaridgeTown_EventScript_GymSign::
	msgbox LavaridgeTown_Text_GymSign, MSGBOX_SIGN
	end

LavaridgeTown_EventScript_HerbShopSign::
	msgbox LavaridgeTown_Text_HerbShopSign, MSGBOX_SIGN
	end

LavaridgeTown_Text_MayNiceBadgesTakeThis:
	.string "MAY: {PLAYER}{KUN}! You stopped MAGMA\n"
	.string "and earned the HEAT BADGE!\p"
	.string "The hot springs finally feel\n"
	.string "peaceful again.\p"
	.string "Here, these will help with the\n"
	.string "sandstorm on ROUTE 111.$"

LavaridgeTown_Text_GoGogglesSentToPC:
	.string "Your KEY ITEMS pocket is full, so the\n"
	.string "GO-GOGGLES were sent to your PC.$"

LavaridgeTown_Text_GoGogglesNoRoom:
	.string "Your KEY ITEMS pocket and PC are\n"
	.string "full. Make room, then talk to me.\p"
	.string "If you leave town, I will wait\n"
	.string "outside the HERB SHOP.$"

LavaridgeTown_Text_BrendanNiceBadgesTakeThis:
	.string "BRENDAN: Hey, {PLAYER}!\n"
	.string "You stopped MAXIE at the summit!\p"
	.string "And now you have the HEAT BADGE.\n"
	.string "You have been busy!\p"
	.string "Take these for the sandstorm\n"
	.string "on ROUTE 111.$"

LavaridgeTown_Text_HaveEggWillYouTakeIt:
	.string "I have here an EGG.\p"
	.string "I'd hoped to hatch it by covering it in\n"
	.string "hot sand by the hot springs.\l"
	.string "But that doesn't seem to be enough…\p"
	.string "I've heard it would be best if it were\n"
	.string "kept together with POKéMON and\l"
	.string "carried about.\p"
	.string "You are a TRAINER, yes?\n"
	.string "And your POKéMON radiate vitality.\p"
	.string "So, what say you?\n"
	.string "Will you take this EGG to hatch?$"

LavaridgeTown_Text_HopeYoullWalkPlentyWithEgg:
	.string "Good! I hope you'll walk plenty with\n"
	.string "this here EGG!$"

LavaridgeTown_Text_ReceivedTheEgg:
	.string "{PLAYER} received the EGG.$"

LavaridgeTown_Text_NoRoomForThisEgg:
	.string "Oh? You've too many POKéMON.\n"
	.string "There's no room for this EGG…$"

LavaridgeTown_Text_AsYouWishThen:
	.string "As you wish, then…\p"
	.string "If you have a change of heart about\n"
	.string "hatching this EGG, I will be here.$"

LavaridgeTown_Text_EverySoOftenEggFoundAtDayCare:
	.string "Every so often, an EGG will be found at\n"
	.string "the POKéMON DAY CARE.\p"
	.string "Or at least that's how the rumor goes.$"

LavaridgeTown_Text_MegaGift_EMBOARITE:
	.string "This old stone has soaked in the\n"
	.string "spring for years and years.\p"
	.string "Feel it. It comes out hot to the\n"
	.string "touch. Take it, won't you?$"

LavaridgeTown_Text_HotSpringsNeverRunDry:
	.string "We draw as much hot water as we need,\n"
	.string "and yet the hot springs never run dry.\p"
	.string "Isn't it magical?\p"
	.string "These hot springs appear near active\n"
	.string "volcanoes. Veins of water under the\l"
	.string "ground are heated by magma to well up\l"
	.string "as hot springs.$"

LavaridgeTown_Text_PokemonNippedBackside:
	.string "Being buried in this hot sand is…\n"
	.string "Sigh…\p"
	.string "So warm and heavenly…\p"
	.string "Eh? Gyaah! Ouch!\p"
	.string "A POKéMON nipped my backside!$"

LavaridgeTown_Text_OhYouLikeHotSprings:
	.string "Oh, you like hot springs, do you?\p"
	.string "That's surprising for one as young\n"
	.string "as you.$"

LavaridgeTown_Text_BatheInHotSpringsEveryDay:
	.string "I bathe in the hot springs every day.\p"
	.string "I want to become a beautiful and strong\n"
	.string "GYM LEADER like FLANNERY.$"

LavaridgeTown_Text_IfPokemonInHotSprings:
	.string "If people put POKéMON in hot springs,\n"
	.string "it might be seriously strange.\p"
	.string "Why, it might be an electric bath, or\n"
	.string "a bubble bath, or even a lava bath…$"

LavaridgeTown_Text_HotSpringsClaims:
	.string "They're claiming that these hot springs\n"
	.string "are good for calming nervous tension,\l"
	.string "relieving aching muscles, solving\l"
	.string "romantic problems, and attracting\l"
	.string "money…$"

LavaridgeTown_Text_TownSign:
	.string "LAVARIDGE TOWN\p"
	.string "“POKéMON CENTER HOT SPRINGS\n"
	.string "An excellent place for relaxing!”$"

LavaridgeTown_Text_GymSign:
	.string "LAVARIDGE TOWN POKéMON GYM\n"
	.string "LEADER: FLANNERY\l"
	.string "“One with a fiery passion that burns!”$"

LavaridgeTown_Text_HerbShopSign:
	.string "POKéMON HERB SHOP\n"
	.string "“Bitter taste--better cure!”$"

LavaridgeTown_Text_ExplainGoGoggles:
	.string "Carry the GO-GOGGLES in your BAG\n"
	.string "when you explore ROUTE 111's desert.\p"
	.string "First, let's see what is troubling\n"
	.string "the woods below MT. CHIMNEY.$"

LavaridgeTown_EventScript_MountainHandoff::
	goto_if_set FLAG_EC_REPORT_C26_COMPLETE, LavaridgeTown_EventScript_NormanHandoff
	goto_if_lt VAR_CHANSEY_NURSE_STATE, 7, LavaridgeTown_EventScript_RescueHandoff
	goto_if_set FLAG_EC_RESOLVED_MOLTRES, LavaridgeTown_EventScript_DesertHandoff
	goto LavaridgeTown_EventScript_MoltresHandoff
LavaridgeTown_EventScript_RescueHandoff::
	msgbox LavaridgeTown_Text_RescueHandoff, MSGBOX_DEFAULT
	return
LavaridgeTown_EventScript_MoltresHandoff::
	msgbox LavaridgeTown_Text_MoltresHandoff, MSGBOX_DEFAULT
	return
LavaridgeTown_EventScript_DesertHandoff::
	msgbox LavaridgeTown_Text_DesertHandoff, MSGBOX_DEFAULT
	return
LavaridgeTown_EventScript_NormanHandoff::
	msgbox LavaridgeTown_Text_NormanHandoff, MSGBOX_DEFAULT
	return

LavaridgeTown_Text_RescueHandoff:
	.string "Help the nurse on southern ROUTE 111\n"
	.string "bring her CHANSEY home.\p"
	.string "Follow its trail up ROUTE 112, then\n"
	.string "take the cable car to MT. CHIMNEY.\p"
	.string "Descend JAGGED PASS and take its\n"
	.string "western entrance into EMBER PATH.\p"
	.string "The western passage leads into\n"
	.string "ASHEN WOODS. Find CHANSEY there,\l"
	.string "then return to the nurse.$"
LavaridgeTown_Text_MoltresHandoff:
	.string "From ASHEN WOODS, take the north\n"
	.string "exit into EMBER PATH.\p"
	.string "Follow the trail to MOLTRES's roost.\n"
	.string "Win its battle or catch it to\l"
	.string "resolve the disturbance.$"
LavaridgeTown_Text_DesertHandoff:
	.string "Take your GO-GOGGLES into the\n"
	.string "ROUTE 111 desert. Enter MIRAGE TOWER.\p"
	.string "Its basement leads to SANDSTREWN\n"
	.string "RUINS. Speak to the archaeologist.\p"
	.string "Explore the lowest chamber, then\n"
	.string "return to him at the tower entrance.$"
LavaridgeTown_Text_NormanHandoff:
	.string "NORMAN is ready at PETALBURG GYM.\n"
	.string "Return through MAUVILLE, then west\l"
	.string "through RUSTURF TUNNEL.\p"
	.string "From RUSTBORO, follow the woods\n"
	.string "south to PETALBURG CITY.$"
