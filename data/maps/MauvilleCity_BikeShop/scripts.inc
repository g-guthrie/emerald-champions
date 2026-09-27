@ One Bicycle. Rydel gives ITEM_ACRO_BIKE, shown as the Bicycle; src/bike.c rides it
@ at the Mach Bike's top speed, so the one frame climbs sandy slopes and crosses
@ cracked floors as well as riding rails and hopping. There is nothing to choose or
@ swap. A save that already holds a Mach Bike trades it in for the Bicycle.
MauvilleCity_BikeShop_MapScripts:: @ 820EBBB
	.byte 0

MauvilleCity_BikeShop_EventScript_Rydel:: @ 820EBBC
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_BIKE, MauvilleCity_BikeShop_EventScript_AlreadyHasBike
	goto_if_set FLAG_DECLINED_BIKE, MauvilleCity_BikeShop_EventScript_SkipGreeting
	msgbox MauvilleCity_BikeShop_Text_RydelGreeting, MSGBOX_DEFAULT
MauvilleCity_BikeShop_EventScript_SkipGreeting:: @ 820EBF7
	msgbox MauvilleCity_BikeShop_Text_DidYouComeFromFarAway, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, MauvilleCity_BikeShop_EventScript_GiveBike
	setflag FLAG_DECLINED_BIKE
	msgbox MauvilleCity_BikeShop_Text_GuessYouDontNeedBike, MSGBOX_DEFAULT
	release
	end

MauvilleCity_BikeShop_EventScript_GiveBike::
	msgbox MauvilleCity_BikeShop_Text_TakeThisBicycle, MSGBOX_DEFAULT
	giveitem ITEM_ACRO_BIKE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_BIKE
	msgbox MauvilleCity_BikeShop_Text_OneFrameDoesItAll, MSGBOX_DEFAULT
	release
	end

MauvilleCity_BikeShop_EventScript_AlreadyHasBike::
	checkitem ITEM_MACH_BIKE
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_BikeShop_EventScript_TradeInMachBike
	msgbox MauvilleCity_BikeShop_Text_HappyYouLikeIt, MSGBOX_DEFAULT
	release
	end

MauvilleCity_BikeShop_EventScript_TradeInMachBike::
	msgbox MauvilleCity_BikeShop_Text_TradeInMachBike, MSGBOX_DEFAULT
	special SwapRegisteredBike
	removeitem ITEM_MACH_BIKE
	giveitem ITEM_ACRO_BIKE
	msgbox MauvilleCity_BikeShop_Text_OneFrameDoesItAll, MSGBOX_DEFAULT
	release
	end

MauvilleCity_BikeShop_EventScript_Assistant:: @ 820ED31
	msgbox MauvilleCity_BikeShop_Text_HandbooksAreInBack, MSGBOX_NPC
	end

@ Two shelves, one Bicycle: riding on the left, tricks on the right.
MauvilleCity_BikeShop_EventScript_MachBikeHandbook:: @ 820ED3A
	message MauvilleCity_BikeShop_Text_RidingHandbookWhichPage
	waitmessage
	goto MauvilleCity_BikeShop_EventScript_ChooseRidingHandbookPage
	end

MauvilleCity_BikeShop_EventScript_ChooseRidingHandbookPage::
	multichoice 0, 0, MULTI_MACH_BIKE_INFO, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_BikeShop_EventScript_HowToRide
	case 1, MauvilleCity_BikeShop_EventScript_SandySlopes
	case 2, MauvilleCity_BikeShop_EventScript_CrackedFloors
	case 3, MauvilleCity_BikeShop_EventScript_ExitHandbook
	case MULTI_B_PRESSED, MauvilleCity_BikeShop_EventScript_ExitHandbook
	end

MauvilleCity_BikeShop_EventScript_HowToRide:: @ 820ED88
	message MauvilleCity_BikeShop_Text_HowToRide
	waitmessage
	goto MauvilleCity_BikeShop_EventScript_ChooseRidingHandbookPage
	end

MauvilleCity_BikeShop_EventScript_SandySlopes:: @ 820EDA0
	message MauvilleCity_BikeShop_Text_SandySlopes
	waitmessage
	goto MauvilleCity_BikeShop_EventScript_ChooseRidingHandbookPage
	end

MauvilleCity_BikeShop_EventScript_CrackedFloors::
	message MauvilleCity_BikeShop_Text_CrackedFloors
	waitmessage
	goto MauvilleCity_BikeShop_EventScript_ChooseRidingHandbookPage
	end

MauvilleCity_BikeShop_EventScript_AcroBikeHandbook:: @ 820EDAE
	message MauvilleCity_BikeShop_Text_TricksHandbookWhichPage
	waitmessage
	goto MauvilleCity_BikeShop_EventScript_ChooseTricksHandbookPage
	end

MauvilleCity_BikeShop_EventScript_ChooseTricksHandbookPage::
	multichoice 0, 0, MULTI_ACRO_BIKE_INFO, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_BikeShop_EventScript_Wheelies
	case 1, MauvilleCity_BikeShop_EventScript_BunnyHops
	case 2, MauvilleCity_BikeShop_EventScript_Jumps
	case 3, MauvilleCity_BikeShop_EventScript_ExitHandbook
	case MULTI_B_PRESSED, MauvilleCity_BikeShop_EventScript_ExitHandbook
	end

MauvilleCity_BikeShop_EventScript_Wheelies:: @ 820EDFC
	message MauvilleCity_BikeShop_Text_Wheelies
	waitmessage
	goto MauvilleCity_BikeShop_EventScript_ChooseTricksHandbookPage
	end

MauvilleCity_BikeShop_EventScript_BunnyHops:: @ 820EE08
	message MauvilleCity_BikeShop_Text_BunnyHops
	waitmessage
	goto MauvilleCity_BikeShop_EventScript_ChooseTricksHandbookPage
	end

MauvilleCity_BikeShop_EventScript_Jumps:: @ 820EE14
	message MauvilleCity_BikeShop_Text_Jumps
	waitmessage
	goto MauvilleCity_BikeShop_EventScript_ChooseTricksHandbookPage
	end

MauvilleCity_BikeShop_EventScript_ExitHandbook::
	release
	end

MauvilleCity_BikeShop_Text_RydelGreeting: @ 820EE22
	.string "Well, well, what have we here?\n"
	.string "A most energetic customer!\p"
	.string "Me? You may call me Rydel.\n"
	.string "I'm the owner of this cycle shop.$"

MauvilleCity_BikeShop_Text_DidYouComeFromFarAway: @ 820EE99
	.string "Rydel: Your Running Shoes…\n"
	.string "They're awfully filthy.\p"
	.string "Did you come from far away?$"

MauvilleCity_BikeShop_Text_GuessYouDontNeedBike: @ 820EEE8
	.string "Rydel: Is that right?\p"
	.string "Then, I guess you have no need for\n"
	.string "my Bicycle.$"

MauvilleCity_BikeShop_Text_TakeThisBicycle:
	.string "Rydel: Hm, hm… … … … …\p"
	.string "You're saying that you came all this\n"
	.string "way from Littleroot?\p"
	.string "My goodness!\n"
	.string "That's ridiculously far!\p"
	.string "With one of my Bikes, you could go\n"
	.string "anywhere with the wind at your back!\p"
	.string "I'll tell you what!\n"
	.string "Take my Bicycle!$"

MauvilleCity_BikeShop_Text_OneFrameDoesItAll:
	.string "Rydel: One frame does it all.\p"
	.string "It's fast enough to climb sandy\n"
	.string "slopes and cross cracked floors.\p"
	.string "Hold the B Button for wheelies and\n"
	.string "hops, and it rides narrow rails.\p"
	.string "The handbooks in the back tell you\n"
	.string "the rest. Enjoy the ride!$"

MauvilleCity_BikeShop_Text_TradeInMachBike:
	.string "Rydel: Oh! That's one of my old\n"
	.string "Mach Bikes.\p"
	.string "I only build one Bicycle now.\n"
	.string "Let me swap it for you!$"

MauvilleCity_BikeShop_Text_HappyYouLikeIt: @ 820F2C4
	.string "Rydel: Good, good!\n"
	.string "I'm happy that you like it!$"

MauvilleCity_BikeShop_Text_HandbooksAreInBack: @ 820F3C3
	.string "I'm learning about Bikes while\n"
	.string "I work here.\p"
	.string "If you need advice on how to ride your\n"
	.string "Bicycle, there're two handbooks in\l"
	.string "the back.$"

MauvilleCity_BikeShop_Text_RidingHandbookWhichPage:
	.string "It's a handbook on riding the\n"
	.string "Bicycle.\p"
	.string "Which page do you want to read?$"

MauvilleCity_BikeShop_Text_HowToRide: @ 820F487
	.string "The Bicycle moves in the direction\n"
	.string "the + Control Pad is pressed.\p"
	.string "It rides at full speed right away.\n"
	.string "Release the + Control Pad to stop.\p"
	.string "Want to read a different page?$"

MauvilleCity_BikeShop_Text_SandySlopes: @ 820F61A
	.string "There are small sandy slopes\n"
	.string "throughout the Hoenn region.\p"
	.string "The loose, crumbly sand makes it\n"
	.string "impossible to climb normally.\p"
	.string "But if you ride straight up on the\n"
	.string "Bicycle, you can zip up a sandy slope.\p"
	.string "Want to read a different page?$"

MauvilleCity_BikeShop_Text_CrackedFloors:
	.string "Some old floors are cracked. Walk\n"
	.string "on one and you'll fall right through.\p"
	.string "Ride over it on the Bicycle, and\n"
	.string "it only gives way behind you.\p"
	.string "Want to read a different page?$"

MauvilleCity_BikeShop_Text_TricksHandbookWhichPage:
	.string "It's a handbook on Bicycle tricks.\p"
	.string "Which page do you want to read?$"

MauvilleCity_BikeShop_Text_Wheelies: @ 820F72F
	.string "Press the B Button while riding,\n"
	.string "and the front wheel lifts up.\p"
	.string "You can zip around with the front\n"
	.string "wheel up using the + Control Pad.\p"
	.string "This technique is called a wheelie.\p"
	.string "Want to read a different page?$"

MauvilleCity_BikeShop_Text_BunnyHops: @ 820F7F5
	.string "Keeping the B Button pressed,\n"
	.string "your Bicycle can hop on the spot.\p"
	.string "This technique is called a bunny hop.\p"
	.string "You can ride while hopping, too.\p"
	.string "Want to read a different page?$"

MauvilleCity_BikeShop_Text_Jumps: @ 820F898
	.string "Press the B Button and the + Control\n"
	.string "Pad at the same time to jump.\p"
	.string "Press the + Control Pad to the side\n"
	.string "to jump sideways.\p"
	.string "Press it backwards to make the\n"
	.string "Bicycle change directions while\l"
	.string "jumping.\p"
	.string "Want to read a different page?$"
