@ Emerald Champions: this Battle Tent room is retired. The tent's Battle Arena driver
@ (src/battle_arena.c) and the Battle Frontier maps it borrowed its scripts from are
@ gone from this tree, so the challenge that used to run here cannot run at all.
@ This map has no warp events of its own, so rather than leave the player stranded
@ in a dead room, entering it shows a short message and returns them to the Battle
@ Tent lobby with the challenge cleanly cancelled.

FallarborTown_BattleTentBattleRoom_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, FallarborTown_BattleTentBattleRoom_OnFrame
	.byte 0

FallarborTown_BattleTentBattleRoom_OnFrame:
	map_script_2 VAR_TEMP_0, 0, FallarborTown_BattleTentBattleRoom_EventScript_ReturnToLobby
	.2byte 0

FallarborTown_BattleTentBattleRoom_EventScript_ReturnToLobby::
	lockall
	setvar VAR_TEMP_0, 1
	msgbox FallarborTown_BattleTentBattleRoom_Text_RoomClosed, MSGBOX_DEFAULT
	closemessage
	frontier_set FRONTIER_DATA_CHALLENGE_STATUS, 0
	frontier_set FRONTIER_DATA_BATTLE_NUM, 0
	special LoadPlayerParty
	special HealPlayerParty
	warp MAP_FALLARBOR_TOWN_BATTLE_TENT_LOBBY, 255, 6, 6
	waitstate
	releaseall
	end

FallarborTown_BattleTentBattleRoom_Text_RoomClosed:
	.string "The battle room is dark and the\n"
	.string "stands are empty.\p"
	.string "No matches are being held here.$"
