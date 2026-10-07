@ The rival demonstrates their own search/catch. The player's party and Bag
@ are preserved; no demonstration Pokémon is awarded to the player.
EC_RivalDexNavTutorial_Begin::
	lockall
	goto_if_set STORY_REACHED_DEXNAV_LESSON_DONE, EC_RivalDexNavTutorial_AlreadyDone
	advance_story DEXNAV_LESSON
	getplayerxy VAR_0x8004, VAR_0x8005
	goto_if_eq VAR_0x8005, 19, EC_RivalDexNavTutorial_FromEntrance
	msgbox EC_RivalDexNavTutorial_Text_ComeAlong, MSGBOX_DEFAULT
	closemessage
	special PrepareRivalDexNavTutorial
	special WarpToRivalTutorialSpot
	goto EC_RivalDexNavTutorial_AtGrass

EC_RivalDexNavTutorial_FromEntrance:
	call_if_eq VAR_0x8004, 9, EC_RivalDexNavTutorial_JoinPath
	call_if_eq VAR_0x8004, 11, EC_RivalDexNavTutorial_JoinPathFromEast
	@ The rival waits beyond the entering camera's edge, already placed by the map.
	applymovement OBJ_EVENT_ID_PLAYER, EC_RivalDexNavTutorial_Movement_Approach
	waitmovement 0
	msgbox EC_RivalDexNavTutorial_Text_ComeAlong, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ROUTE101_RIVAL_TUTORIAL, EC_RivalDexNavTutorial_Movement_Lead
	applymovement OBJ_EVENT_ID_PLAYER, EC_RivalDexNavTutorial_Movement_Follow
	waitmovement 0
	special PrepareRivalDexNavTutorial
EC_RivalDexNavTutorial_AtGrass::
	addobject LOCALID_ROUTE101_RIVAL_TUTORIAL
	turnobject LOCALID_ROUTE101_RIVAL_TUTORIAL, DIR_WEST
	turnobject OBJ_EVENT_ID_PLAYER, DIR_EAST
	msgbox EC_RivalDexNavTutorial_Text_Select, MSGBOX_DEFAULT
	closemessage
	special StartRivalDexNavTutorial
	@ Only an allocation/setup failure returns here; a successful search starts
	@ the capture script below through the native encounter lifecycle.
EC_RivalDexNavTutorial_Abort::
	lockall
	special FinishRivalDexNavTutorial
	msgbox EC_RivalDexNavTutorial_Text_Retry, MSGBOX_DEFAULT
	releaseall
	end

EC_RivalDexNavTutorial_Capture::
	lockall
	playse SE_PIN
	applymovement LOCALID_ROUTE101_RIVAL_TUTORIAL, Common_Movement_ExclamationMark
	waitmovement 0
	waitse
	dowildbattle
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, EC_RivalDexNavTutorial_Abort
	turnobject LOCALID_ROUTE101_RIVAL_TUTORIAL, DIR_WEST
	msgbox EC_RivalDexNavTutorial_Text_Caught, MSGBOX_DEFAULT
	closemessage
	special ShowRivalPokerusTutorial
	msgbox EC_RivalDexNavTutorial_Text_Gold, MSGBOX_DEFAULT
	msgbox EC_RivalDexNavTutorial_Text_Chain, MSGBOX_DEFAULT
	closemessage
	advance_story DEXNAV_LESSON_DONE
	special FinishRivalDexNavTutorial
	applymovement LOCALID_ROUTE101_RIVAL_TUTORIAL, EC_RivalDexNavTutorial_Movement_Leave
	waitmovement 0
	removeobject LOCALID_ROUTE101_RIVAL_TUTORIAL
	releaseall
	end

EC_RivalDexNavTutorial_AlreadyDone:
	msgbox EC_RivalDexNavTutorial_Text_Practice, MSGBOX_DEFAULT
	releaseall
	end

EC_RivalDexNavTutorial_Movement_Leave:
	walk_down
	walk_down
	walk_down
	step_end

EC_RivalDexNavTutorial_Text_ComeAlong:
	.string "{RIVAL}: Come over here! I'll show you\n"
	.string "how to search this grass with DexNav.\p"
	.string "Watch my steps. You can practice\n"
	.string "with your own Pokémon afterward.$"

EC_RivalDexNavTutorial_Text_Select:
	.string "I'll choose Zigzagoon, then Search.\n"
	.string "The arrow points to its hiding spot.\p"
	.string "Now I'll hold A while moving.\n"
	.string "See how slowly I approach?\p"
	.string "No running or cycling near it!\n"
	.string "Even normal walking is too loud\l"
	.string "when you're right beside it.$"

EC_RivalDexNavTutorial_Text_Caught:
	.string "{RIVAL}: Got it! That's how you reach\n"
	.string "the Pokémon you chose.\p"
	.string "Oh! This one has Pokérus.\n"
	.string "That's a lucky find, not a promise\l"
	.string "that every search will have it!\p"
	.string "I'll open my new Zigzagoon's Summary.\n"
	.string "Watch for PKRS, then its gold stat.$"

EC_RivalDexNavTutorial_Text_Gold:
	.string "Its Adamant nature raises Attack.\n"
	.string "Pokérus adds to that bonus, so the\l"
	.string "Attack number is gold.\p"
	.string "The benefit stays after recovery,\n"
	.string "even if its nature changes later.\p"
	.string "This original carrier can infect\n"
	.string "two neighbors in my party. Those\l"
	.string "recipients can't pass it on.$"

EC_RivalDexNavTutorial_Text_Chain:
	.string "When YOU catch or defeat a searched\n"
	.string "Pokémon, your chain goes up by one.\p"
	.string "At chain 50, searches have a 50%\n"
	.string "shiny chance and 25% Pokérus chance.\p"
	.string "Run, lose, scare the target or leave\n"
	.string "the area, and the chain ends. Other\l"
	.string "battles break it too.\p"
	.string "You have 15 seconds to reach it.\n"
	.string "The countdown turns red when\l"
	.string "only five seconds remain.\p"
	.string "Take too long and your chain ends.\n"
	.string "Hold A and follow the arrow promptly.\p"
	.string "Birch can explain all the details\n"
	.string "again if you want. Happy hunting!$"

EC_RivalDexNavTutorial_Text_Retry:
	.string "Let's try that again when you're\n"
	.string "ready. Your Pokémon are all with you.$"

EC_RivalDexNavTutorial_Text_Practice:
	.string "Open DexNav, choose a Pokémon,\n"
	.string "then hold A as you approach it!$"

EC_RivalDexNavTutorial_JoinPath:
	applymovement OBJ_EVENT_ID_PLAYER, EC_RivalDexNavTutorial_Movement_JoinPath
	waitmovement 0
	return

EC_RivalDexNavTutorial_JoinPathFromEast:
	applymovement OBJ_EVENT_ID_PLAYER, EC_RivalDexNavTutorial_Movement_JoinPathFromEast
	waitmovement 0
	return

@ The ledge spans x8-11 at y13. Walk around its west end at x7.
EC_RivalDexNavTutorial_Movement_Approach:
	walk_up
	walk_up
	walk_up
	step_end

EC_RivalDexNavTutorial_Movement_Lead:
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	face_left
	step_end

EC_RivalDexNavTutorial_Movement_Follow:
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	face_right
	step_end

EC_RivalDexNavTutorial_Movement_JoinPath:
	walk_right
	step_end

EC_RivalDexNavTutorial_Movement_JoinPathFromEast:
	walk_left
	step_end
