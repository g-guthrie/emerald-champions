SootopolisCity_MysteryEventsHouse_B1F_MapScripts:: @ 8227E4F
	map_script MAP_SCRIPT_ON_TRANSITION, SootopolisCity_MysteryEventsHouse_B1F_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SootopolisCity_MysteryEventsHouse_B1F_OnFrame
	.byte 0

@ Emerald Champions: the e-Reader is retired. This engine carries no e-Reader
@ code, so an e-Reader Trainer can never be loaded into the save and the battle
@ this room existed for cannot happen. The room, its warps and the old man
@ upstairs are all untouched; the basement simply sends the player back up.
SootopolisCity_MysteryEventsHouse_B1F_OnTransition: @ 8227E5A
	setvar VAR_TEMP_1, 0
	end

SootopolisCity_MysteryEventsHouse_B1F_OnFrame: @ 8227E5E
	map_script_2 VAR_TEMP_1, 0, SootopolisCity_MysteryEventsHouse_B1F_EventScript_BattleVisitingTrainer
	.2byte 0

@ LoadPlayerParty undoes the ReducePlayerPartyToSelectedMons that the upstairs
@ script performs before sending the player down here, so the player always
@ leaves this room with their full party.
SootopolisCity_MysteryEventsHouse_B1F_EventScript_BattleVisitingTrainer:: @ 8227E68
	lockall
	setvar VAR_TEMP_1, 1
	special LoadPlayerParty
	setvar VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 0
	warp MAP_SOOTOPOLIS_CITY_MYSTERY_EVENTS_HOUSE_1F, 255, 3, 1
	waitstate
	releaseall
	end

SootopolisCity_MysteryEventsHouse_B1F_EventScript_BattleTie:: @ 8227ECF
	setvar VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 3
	msgbox SootopolisCity_MysteryEventsHouse_B1F_Text_MatchEndedUpDraw, MSGBOX_DEFAULT
	return

SootopolisCity_MysteryEventsHouse_B1F_EventScript_BattleWon:: @ 8227EDD
	setvar VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 1
	special ShowFieldMessageStringVar4
	waitmessage
	waitbuttonpress
	return

SootopolisCity_MysteryEventsHouse_B1F_EventScript_BattleLost:: @ 8227EE8
	setvar VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE, 2
	special ShowFieldMessageStringVar4
	waitmessage
	waitbuttonpress
	return

SootopolisCity_MysteryEventsHouse_B1F_Movement_PlayerEnterBasement: @ 8227EF3
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	step_end

SootopolisCity_MysteryEventsHouse_B1F_Movement_PlayerExitBasement: @ 8227EF9
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	delay_8
	step_end

