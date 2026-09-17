SootopolisCity_MysteryEventsHouse_1F_MapScripts::
	.byte 0

SootopolisCity_MysteryEventsHouse_1F_OnTransition:
	setmaplayoutindex LAYOUT_SOOTOPOLIS_CITY_MYSTERY_EVENTS_HOUSE_1F
	setobjectxyperm LOCALID_MYSTERY_EVENTS_OLD_MAN, 6, 4
	setobjectmovementtype LOCALID_MYSTERY_EVENTS_OLD_MAN, MOVEMENT_TYPE_FACE_RIGHT
	end

SootopolisCity_MysteryEventsHouse_1F_EventScript_SetTrainerVisitingLayout::
	setvar VAR_TEMP_1, 1
	setobjectxyperm LOCALID_MYSTERY_EVENTS_OLD_MAN, 3, 2
	setobjectmovementtype LOCALID_MYSTERY_EVENTS_OLD_MAN, MOVEMENT_TYPE_FACE_DOWN
	setmaplayoutindex LAYOUT_SOOTOPOLIS_CITY_MYSTERY_EVENTS_HOUSE_1F_STAIRS_UNBLOCKED
	return

SootopolisCity_MysteryEventsHouse_1F_EventScript_MoveOldManToDoor::
	setobjectxyperm LOCALID_MYSTERY_EVENTS_OLD_MAN, 2, 2
	setobjectmovementtype LOCALID_MYSTERY_EVENTS_OLD_MAN, MOVEMENT_TYPE_FACE_RIGHT
	return

SootopolisCity_MysteryEventsHouse_1F_OnFrame:
	map_script_2 VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 1, SootopolisCity_MysteryEventsHouse_1F_EventScript_OldManCommentOnBattle
	map_script_2 VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 2, SootopolisCity_MysteryEventsHouse_1F_EventScript_OldManCommentOnBattle
	map_script_2 VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 3, SootopolisCity_MysteryEventsHouse_1F_EventScript_OldManCommentOnBattle
	.2byte 0

SootopolisCity_MysteryEventsHouse_1F_EventScript_OldManCommentOnBattle::
	lockall
	call_if_eq VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 1, SootopolisCity_MysteryEventsHouse_1F_EventScript_BattleWonComment
	call_if_eq VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 2, SootopolisCity_MysteryEventsHouse_1F_EventScript_BattleLostComment
	call_if_eq VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 3, SootopolisCity_MysteryEventsHouse_1F_EventScript_BattleTiedComment
	setvar VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 0
	releaseall
	end

SootopolisCity_MysteryEventsHouse_1F_EventScript_BattleWonComment::
	msgbox SootopolisCity_MysteryEventsHouse_1F_Text_ThatWasSuperlative, MSGBOX_DEFAULT
	return

SootopolisCity_MysteryEventsHouse_1F_EventScript_BattleLostComment::
	msgbox SootopolisCity_MysteryEventsHouse_1F_Text_TooBadForYou, MSGBOX_DEFAULT
	return

SootopolisCity_MysteryEventsHouse_1F_EventScript_BattleTiedComment::
	msgbox SootopolisCity_MysteryEventsHouse_1F_Text_BrilliantStandoff, MSGBOX_DEFAULT
	return

SootopolisCity_MysteryEventsHouse_1F_Movement_PlayerExitStairs:
	walk_down
	step_end

SootopolisCity_MysteryEventsHouse_1F_Movement_OldManWalkBehindPlayer:
	walk_right
	walk_in_place_faster_down
	step_end

SootopolisCity_MysteryEventsHouse_1F_EventScript_OldMan::
	msgbox SootopolisCity_MysteryEventsHouse_1F_Text_CurrentBattles, MSGBOX_NPC
	end

SootopolisCity_MysteryEventsHouse_1F_Text_OnlyAmusementWatchingBattles:
	.string "When I was young, I traveled the world\n"
	.string "as a POKéMON TRAINER.\p"
	.string "Now that I've become an old buzzard,\n"
	.string "my only amusement is watching young\l"
	.string "TRAINERS battle.$"

SootopolisCity_MysteryEventsHouse_B1F_Text_MatchEndedUpDraw:
	.string "The match ended up a draw.$"

SootopolisCity_MysteryEventsHouse_1F_Text_BrilliantStandoff:
	.string "So, it became a standoff.\p"
	.string "It was a brilliant match in which\n"
	.string "neither side conceded a step!$"

SootopolisCity_MysteryEventsHouse_1F_Text_ThatWasSuperlative:
	.string "That was superlative!\p"
	.string "Why, it was like seeing myself in\n"
	.string "my youth again!$"

SootopolisCity_MysteryEventsHouse_1F_Text_TooBadForYou:
	.string "Ah, too bad for you!\p"
	.string "But it was a good match.\n"
	.string "I hope you can win next time.$"

SootopolisCity_MysteryEventsHouse_1F_Text_CurrentBattles::
	.string "I still enjoy a fine Pokémon\n"
	.string "battle. These old visitor records\l"
	.string "bring back good memories.\p"
	.string "For a new challenge, seek the\n"
	.string "League or the Frontier's CHAMPIONS\l"
	.string "CIRCUIT.$"
