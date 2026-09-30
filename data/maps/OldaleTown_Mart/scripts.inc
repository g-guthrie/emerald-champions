OldaleTown_Mart_MapScripts:: @ 81FC23F
	map_script MAP_SCRIPT_ON_FRAME_TABLE, OldaleTown_Mart_OnFrame
	.byte 0

OldaleTown_Mart_OnFrame:
	map_script_2 VAR_TEMP_1, 0, OldaleTown_Mart_EventScript_CheckEntrance
	.2byte 0

@ A save resumed elsewhere in the Mart waits until the player reaches the door.
@ 0 = arrival unchecked; 1 = entrance armed; 2 = handled for this visit.
OldaleTown_Mart_EventScript_CheckEntrance:
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_EC_OLDALE_SHOP_TUTORIAL_COMPLETE, OldaleTown_Mart_EventScript_AlreadyIntroduced
	getplayerxy VAR_0x8004, VAR_0x8005
	goto_if_eq VAR_0x8005, 7, OldaleTown_Mart_EventScript_EnterFromDoor
	goto_if_ne VAR_0x8005, 6, OldaleTown_Mart_EventScript_Release
	goto_if_eq VAR_0x8004, 3, OldaleTown_Mart_EventScript_FirstShopping
	goto_if_eq VAR_0x8004, 4, OldaleTown_Mart_EventScript_FirstShopping
	goto OldaleTown_Mart_EventScript_Release

@ The real door warp arrives on y7; the scene starts one step inside at y6.
OldaleTown_Mart_EventScript_EnterFromDoor:
	goto_if_eq VAR_0x8004, 3, OldaleTown_Mart_EventScript_StepInside
	goto_if_eq VAR_0x8004, 4, OldaleTown_Mart_EventScript_StepInside
	goto OldaleTown_Mart_EventScript_Release
OldaleTown_Mart_EventScript_StepInside:
	setvar VAR_TEMP_1, 2
	applymovement OBJ_EVENT_ID_PLAYER, OldaleTown_Mart_Movement_StepInside
	waitmovement 0
	goto OldaleTown_Mart_EventScript_FirstShopping
OldaleTown_Mart_Movement_StepInside:
	walk_up
	step_end

OldaleTown_Mart_EventScript_AlreadyIntroduced:
	setvar VAR_TEMP_1, 2
	goto OldaleTown_Mart_EventScript_Release

@ Both entrance tiles meet in the clear aisle before the nurse approaches.
@ The permanent flag also hides her after the single completed introduction.
OldaleTown_Mart_EventScript_FirstShopping::
	lockall
	setvar VAR_TEMP_1, 2
	goto_if_set FLAG_EC_OLDALE_SHOP_TUTORIAL_COMPLETE, OldaleTown_Mart_EventScript_Release
	getplayerxy VAR_0x8004, VAR_0x8005
	call_if_eq VAR_0x8004, 4, OldaleTown_Mart_EventScript_JoinAisle
	applymovement LOCALID_OLDALE_MART_VISITING_NURSE, OldaleTown_Mart_Movement_NurseApproach
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceRight
	waitmovement 0
	msgbox OldaleTown_Mart_Text_NurseWelcome, MSGBOX_DEFAULT
	call EventScript_PkmnCenterNurse_GiveToolsForTour
	goto_if_eq VAR_0x8007, FALSE, OldaleTown_Mart_EventScript_TourPending
	msgbox OldaleTown_Mart_Text_OpenTools, MSGBOX_DEFAULT
	closemessage
	special StartInitialToolsBagTutorial
	goto_if_eq VAR_RESULT, FALSE, OldaleTown_Mart_EventScript_TourPending
	call EventScript_PkmnCenterNurse_ExplainLeveler
	msgbox gText_EmeraldChampionsLocalGuide, MSGBOX_DEFAULT
	msgbox OldaleTown_Mart_Text_VisitCounter, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_OLDALE_MART_VISITING_NURSE, OldaleTown_Mart_Movement_ToCounter
	applymovement OBJ_EVENT_ID_PLAYER, OldaleTown_Mart_Movement_ToCounter
	waitmovement 0
	msgbox OldaleTown_Mart_Text_ClerkLesson, MSGBOX_DEFAULT
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceRight
	waitmovement 0
	msgbox OldaleTown_Mart_Text_NurseGoodbye, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_OLDALE_MART_VISITING_NURSE, OldaleTown_Mart_Movement_NurseExit
	waitmovement 0
	removeobject LOCALID_OLDALE_MART_VISITING_NURSE
	setflag FLAG_EC_OLDALE_SHOP_TUTORIAL_COMPLETE
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceLeft
	waitmovement 0
OldaleTown_Mart_EventScript_Release:
	releaseall
	end

@ Keep a saved incomplete visit at the authored approach position.
OldaleTown_Mart_EventScript_TourPending:
	closemessage
	applymovement LOCALID_OLDALE_MART_VISITING_NURSE, OldaleTown_Mart_Movement_NurseReturn
	waitmovement 0
	goto OldaleTown_Mart_EventScript_Release

OldaleTown_Mart_EventScript_JoinAisle:
	applymovement OBJ_EVENT_ID_PLAYER, OldaleTown_Mart_Movement_JoinAisle
	waitmovement 0
	return

OldaleTown_Mart_EventScript_Nurse::
	msgbox OldaleTown_Mart_Text_ToolsPending, MSGBOX_NPC
	end

OldaleTown_Mart_Movement_JoinAisle:
	walk_left
	step_end

OldaleTown_Mart_Movement_NurseApproach:
	walk_down
	face_left
	step_end

OldaleTown_Mart_Movement_NurseReturn:
	walk_up
	face_down
	step_end

OldaleTown_Mart_Movement_ToCounter:
	walk_up
	walk_up
	walk_up
	face_left
	step_end

OldaleTown_Mart_Movement_NurseExit:
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

OldaleTown_Mart_Text_NurseWelcome:
	.string "NURSE: Hello! I help new Trainers\n"
	.string "get ready for the road.\p"
	.string "Before you shop, let's look at\n"
	.string "your travel tools together.$"

OldaleTown_Mart_Text_OpenTools:
	.string "NURSE: Here's my Key Items pocket.\n"
	.string "Watch the descriptions as I scroll.$"

OldaleTown_Mart_Text_VisitCounter:
	.string "NURSE: These tools stay with you.\n"
	.string "Now let's meet the shop clerk!$"

OldaleTown_Mart_Text_ClerkLesson:
	.string "CLERK: Welcome! Our stock grows\n"
	.string "as you earn Gym Badges.\p"
	.string "We sell items for Pokémon to hold.\n"
	.string "A Mental Herb, for example, frees\l"
	.string "its holder from Taunt.\p"
	.string "Choose an item in your Bag, then\n"
	.string "GIVE it to a Pokémon before battle.\p"
	.string "With your Regenerator, a held Berry\n"
	.string "its own holder eats in battle comes\l"
	.string "back afterward.\p"
	.string "Knock Off lasts only that battle.\n"
	.string "Thief and Covet steal for good.\p"
	.string "Browse an item's description before\n"
	.string "buying. You can press B to leave.\p"
	.string "For Poké Balls, medicine, or more\n"
	.string "held items you've found, visit the\l"
	.string "clerk in a Pokémon Center.$"

OldaleTown_Mart_Text_NurseGoodbye:
	.string "NURSE: I'll head back to the Center.\n"
	.string "Speak to the clerk when you're ready!$"

OldaleTown_Mart_Text_ToolsPending:
	.string "NURSE: Come back when you're ready.\n"
	.string "I'll show you your travel tools!$"

OldaleTown_Mart_EventScript_Woman::
	msgbox OldaleTown_Mart_Text_BuyPokeBallsAtCenter, MSGBOX_NPC
	end

OldaleTown_Mart_EventScript_Boy:: @ 81FC2A9
	msgbox OldaleTown_Mart_Text_RestoreHPWithPotion, MSGBOX_NPC
	end

OldaleTown_Mart_Text_BuyPokeBallsAtCenter:
	.string "This Mart sells held items.\p"
	.string "I'm going to the Pokémon Center\n"
	.string "to buy Poké Balls for catching!$"

OldaleTown_Mart_Text_RestoreHPWithPotion: @ 81FC338
	.string "If a Pokémon gets hurt and loses its HP\n"
	.string "and faints, it won't be able to battle.\p"
	.string "To prevent your Pokémon from fainting,\n"
	.string "restore its HP with a Potion.$"
