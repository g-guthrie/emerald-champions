Route123_BerryMastersHouse_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route123_BerryMastersHouse_OnTransition
	.byte 0

Route123_BerryMastersHouse_OnTransition:
	setflag FLAG_LANDMARK_BERRY_MASTERS_HOUSE
	end

Route123_BerryMastersHouse_EventScript_BerryMaster::
	lock
	faceplayer
	message Route123_BerryMastersHouse_Text_GardenWelcome
	waitmessage
	dynmultipush Route123_BerryMastersHouse_Text_BerryGifts, 0
	dynmultipush Route123_BerryMastersHouse_Text_StoneTrades, 1
	dynmultipush Route123_BerryMastersHouse_Text_Leave, 2
	dynmultistack 8, 1, FALSE, 3, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, 0, Route123_BerryMastersHouse_EventScript_DailyBerries
	goto_if_eq VAR_RESULT, 1, Route123_BerryMastersHouse_EventScript_StoneTrades
	release
	end

Route123_BerryMastersHouse_EventScript_StoneTrades:
	special CountEmeraldChampionsGardenBerries
	buffernumberstring STR_VAR_1, VAR_RESULT
	buffernumberstring STR_VAR_2, VAR_0x8005
	message Route123_BerryMastersHouse_Text_StonePrices
	waitmessage
	dynmultipush Route123_BerryMastersHouse_Text_Baxcalibrite, 0
	dynmultipush Route123_BerryMastersHouse_Text_Dragoninite, 1
	dynmultipush Route123_BerryMastersHouse_Text_Tyranitarite, 2
	dynmultipush Route123_BerryMastersHouse_Text_WhichBerries, 3
	dynmultipush Route123_BerryMastersHouse_Text_Leave, 4
	dynmultistack 8, 1, FALSE, 4, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	goto_if_eq VAR_RESULT, 0, Route123_BerryMastersHouse_EventScript_Baxcalibrite
	goto_if_eq VAR_RESULT, 1, Route123_BerryMastersHouse_EventScript_Dragoninite
	goto_if_eq VAR_RESULT, 2, Route123_BerryMastersHouse_EventScript_Tyranitarite
	goto_if_eq VAR_RESULT, 3, Route123_BerryMastersHouse_EventScript_WhichBerries
	release
	end

Route123_BerryMastersHouse_EventScript_Baxcalibrite:
	setvar VAR_0x8004, ITEM_BAXCALIBRITE
	goto Route123_BerryMastersHouse_EventScript_ConfirmStone
Route123_BerryMastersHouse_EventScript_Dragoninite:
	setvar VAR_0x8004, ITEM_DRAGONINITE
	goto Route123_BerryMastersHouse_EventScript_ConfirmStone
Route123_BerryMastersHouse_EventScript_Tyranitarite:
	setvar VAR_0x8004, ITEM_TYRANITARITE
Route123_BerryMastersHouse_EventScript_ConfirmStone:
	bufferitemname STR_VAR_1, VAR_0x8004
	msgbox Route123_BerryMastersHouse_Text_ConfirmStone, MSGBOX_YESNO
	goto_if_ne VAR_RESULT, YES, Route123_BerryMastersHouse_EventScript_StoneTrades
	special TradeEmeraldChampionsGardenBerries
	goto_if_eq VAR_RESULT, EC_MEGA_BERRY_TRADE_ALREADY_DONE, Route123_BerryMastersHouse_EventScript_StoneAlreadyGiven
	goto_if_eq VAR_RESULT, EC_MEGA_BERRY_TRADE_NOT_ENOUGH, Route123_BerryMastersHouse_EventScript_MoreBerries
	goto_if_eq VAR_RESULT, EC_MEGA_BERRY_TRADE_BAG_FULL, Common_EventScript_ShowBagIsFull
	goto_if_ne VAR_RESULT, EC_MEGA_BERRY_TRADE_SUCCESS, Route123_BerryMastersHouse_EventScript_StoneTrades
	playfanfare MUS_OBTAIN_ITEM
	msgbox Route123_BerryMastersHouse_Text_StoneReceived, MSGBOX_DEFAULT
	waitfanfare
	goto Route123_BerryMastersHouse_EventScript_StoneTrades

Route123_BerryMastersHouse_EventScript_WhichBerries:
	msgbox Route123_BerryMastersHouse_Text_GardenBerries, MSGBOX_DEFAULT
	goto Route123_BerryMastersHouse_EventScript_StoneTrades
Route123_BerryMastersHouse_EventScript_MoreBerries:
	msgbox Route123_BerryMastersHouse_Text_MoreBerries, MSGBOX_DEFAULT
	goto Route123_BerryMastersHouse_EventScript_StoneTrades
Route123_BerryMastersHouse_EventScript_StoneAlreadyGiven:
	msgbox Route123_BerryMastersHouse_Text_StoneAlreadyGiven, MSGBOX_DEFAULT
	goto Route123_BerryMastersHouse_EventScript_StoneTrades

Route123_BerryMastersHouse_Text_GardenWelcome:
	.string "A good garden always has something\n"
	.string "to share. What brings you here?$"
Route123_BerryMastersHouse_Text_BerryGifts:
	.string "Berry gifts$"
Route123_BerryMastersHouse_Text_StoneTrades:
	.string "Mega Stone trades$"
Route123_BerryMastersHouse_Text_Leave:
	.string "See you later$"
Route123_BerryMastersHouse_Text_Baxcalibrite:
	.string "Baxcalibrite$"
Route123_BerryMastersHouse_Text_Dragoninite:
	.string "Dragoninite$"
Route123_BerryMastersHouse_Text_Tyranitarite:
	.string "Tyranitarite$"
Route123_BerryMastersHouse_Text_WhichBerries:
	.string "Which berries?$"
Route123_BerryMastersHouse_Text_StonePrices:
	.string "Each stone costs {STR_VAR_2} garden berries.\n"
	.string "You have {STR_VAR_1}. One of each stone!$"
Route123_BerryMastersHouse_Text_ConfirmStone:
	.string "Trade {STR_VAR_2} garden berries for\n"
	.string "a {STR_VAR_1}?$"
Route123_BerryMastersHouse_Text_StoneReceived:
	.string "You traded the berries and received\n"
	.string "a {STR_VAR_1}!$"
Route123_BerryMastersHouse_Text_StoneAlreadyGiven:
	.string "That one's already yours! I only\n"
	.string "found one of each in these fields.$"
Route123_BerryMastersHouse_Text_MoreBerries:
	.string "Keep gathering as you travel. Plant\n"
	.string "a few, and your harvest will grow!$"
Route123_BerryMastersHouse_Text_GardenBerries:
	.string "Razz, Bluk, Nanab, Wepear and Pinap\n"
	.string "are perfect for my garden.\p"
	.string "Pomeg, Kelpsy, Qualot, Hondew, Grepa\n"
	.string "and Tamato count, too. Any mixture!\p"
	.string "I found these stones while digging.\n"
	.string "Let's put them to better use.$"

Route123_BerryMastersHouse_EventScript_DailyBerries::
	lock
	faceplayer
	dotimebasedevents
	goto_if_set FLAG_DAILY_BERRY_MASTER_RECEIVED_BERRY, Route123_BerryMastersHouse_EventScript_ReceivedBerryToday
	msgbox Route123_BerryMastersHouse_Text_YoureDeservingOfBerry, MSGBOX_DEFAULT
	giverandomberry BERRY_ID_POMEG, BERRY_ID_NOMEL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_DAILY_BERRY_MASTER_RECEIVED_BERRY
	msgbox Route123_BerryMastersHouse_Text_WhyBeStingyTakeAnother, MSGBOX_DEFAULT
	giverandomberry BERRY_ID_POMEG, BERRY_ID_NOMEL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	msgbox Route123_BerryMastersHouse_Text_VisitPrettyPetalFlowerShop, MSGBOX_DEFAULT
	release
	end

Route123_BerryMastersHouse_EventScript_ReceivedBerryToday::
	msgbox Route123_BerryMastersHouse_Text_DoneForToday, MSGBOX_DEFAULT
	release
	end

Route123_BerryMastersHouse_EventScript_BerryMastersWife::
	lock
	faceplayer
	dotimebasedevents
	goto_if_set FLAG_DAILY_BERRY_MASTERS_WIFE, Route123_BerryMastersHouse_EventScript_ReceivedWifeBerryToday
	msgbox Route123_BerryMastersHouse_Text_HeardAGoodSayingLately, MSGBOX_DEFAULT
	setvar VAR_0x8004, EASY_CHAT_TYPE_GOOD_SAYING
	call Common_ShowEasyChatScreen
	lock
	faceplayer
	goto_if_eq VAR_RESULT, TRUE, Route123_BerryMastersHouse_EventScript_GavePhrase
	goto_if_eq VAR_RESULT, FALSE, Route123_BerryMastersHouse_EventScript_CancelPhrase
	end

Route123_BerryMastersHouse_EventScript_CancelPhrase::
	msgbox Route123_BerryMastersHouse_Text_Ah, MSGBOX_DEFAULT
	msgbox Route123_BerryMastersHouse_Text_JoyNeverGoesOutOfMyLife, MSGBOX_DEFAULT
	release
	end

Route123_BerryMastersHouse_EventScript_GavePhrase::
	goto_if_eq VAR_0x8004, NOT_SPECIAL_PHRASE, Route123_BerryMastersHouse_EventScript_GiveNormalBerry
	goto_if_eq VAR_0x8004, PHRASE_GREAT_BATTLE, Route123_BerryMastersHouse_EventScript_GiveSpelonBerry
	goto_if_eq VAR_0x8004, PHRASE_CHALLENGE_CONTEST, Route123_BerryMastersHouse_EventScript_GivePamtreBerry
	goto_if_eq VAR_0x8004, PHRASE_OVERWHELMING_LATIAS, Route123_BerryMastersHouse_EventScript_GiveWatmelBerry
	goto_if_eq VAR_0x8004, PHRASE_COOL_LATIOS, Route123_BerryMastersHouse_EventScript_GiveDurinBerry
	goto_if_eq VAR_0x8004, PHRASE_SUPER_HUSTLE, Route123_BerryMastersHouse_EventScript_GiveBelueBerry
	end

Route123_BerryMastersHouse_EventScript_GiveNormalBerry::
	msgbox Route123_BerryMastersHouse_Text_GoodSayingTakeThis, MSGBOX_DEFAULT
	giverandomberry BERRY_ID_CHERI, BERRY_ID_SITRUS
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	goto Route123_BerryMastersHouse_EventScript_GaveBerry
	release
	end

Route123_BerryMastersHouse_EventScript_GiveSpelonBerry::
	goto_if_set FLAG_RECEIVED_SPELON_BERRY, Route123_BerryMastersHouse_EventScript_GiveNormalBerry
	msgbox Route123_BerryMastersHouse_Text_InspirationalTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_SPELON_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_SPELON_BERRY
	goto Route123_BerryMastersHouse_EventScript_GaveBerry
	end

Route123_BerryMastersHouse_EventScript_GivePamtreBerry::
	goto_if_set FLAG_RECEIVED_PAMTRE_BERRY, Route123_BerryMastersHouse_EventScript_GiveNormalBerry
	msgbox Route123_BerryMastersHouse_Text_InspirationalTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_PAMTRE_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_PAMTRE_BERRY
	goto Route123_BerryMastersHouse_EventScript_GaveBerry
	end

Route123_BerryMastersHouse_EventScript_GiveWatmelBerry::
	goto_if_set FLAG_RECEIVED_WATMEL_BERRY, Route123_BerryMastersHouse_EventScript_GiveNormalBerry
	msgbox Route123_BerryMastersHouse_Text_InspirationalTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_WATMEL_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_WATMEL_BERRY
	goto Route123_BerryMastersHouse_EventScript_GaveBerry
	end

Route123_BerryMastersHouse_EventScript_GiveDurinBerry::
	goto_if_set FLAG_RECEIVED_DURIN_BERRY, Route123_BerryMastersHouse_EventScript_GiveNormalBerry
	msgbox Route123_BerryMastersHouse_Text_InspirationalTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_DURIN_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_DURIN_BERRY
	goto Route123_BerryMastersHouse_EventScript_GaveBerry
	end

Route123_BerryMastersHouse_EventScript_GiveBelueBerry::
	goto_if_set FLAG_RECEIVED_BELUE_BERRY, Route123_BerryMastersHouse_EventScript_GiveNormalBerry
	msgbox Route123_BerryMastersHouse_Text_InspirationalTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_BELUE_BERRY
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_BELUE_BERRY
	goto Route123_BerryMastersHouse_EventScript_GaveBerry
	end

Route123_BerryMastersHouse_EventScript_ReceivedWifeBerryToday::
	msgbox Route123_BerryMastersHouse_Text_JoyNeverGoesOutOfMyLife, MSGBOX_DEFAULT
	release
	end

Route123_BerryMastersHouse_EventScript_GaveBerry::
	setflag FLAG_DAILY_BERRY_MASTERS_WIFE
	msgbox Route123_BerryMastersHouse_Text_JoyNeverGoesOutOfMyLife, MSGBOX_DEFAULT
	release
	end

