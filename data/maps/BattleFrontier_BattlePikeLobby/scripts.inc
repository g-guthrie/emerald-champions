@ Emerald Champions: the Pike is a Champions Circuit registration desk. Its room
@ challenge is retired; the receptionist runs
@ BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit from map.json.
BattleFrontier_BattlePikeLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattlePikeLobby_OnWarp
	.byte 0

BattleFrontier_BattlePikeLobby_OnWarp:
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattlePikeLobby_EventScript_TurnPlayerNorth
	.2byte 0

BattleFrontier_BattlePikeLobby_EventScript_TurnPlayerNorth::
	setvar VAR_TEMP_1, 1
	turnobject LOCALID_PLAYER, DIR_NORTH
	end

BattleFrontier_BattlePikeLobby_EventScript_RulesBoard::
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN
	end

BattleFrontier_BattlePikeLobby_EventScript_ShowResults::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_BattlePikeLobby_EventScript_Hiker::
	msgbox BattleFrontier_BattlePikeLobby_Text_NoMoreGuessingDoors, MSGBOX_NPC
	end

BattleFrontier_BattlePikeLobby_EventScript_Twin::
	msgbox BattleFrontier_BattlePikeLobby_Text_DeskTakesMyName, MSGBOX_NPC
	end

BattleFrontier_BattlePikeLobby_EventScript_Beauty::
	msgbox BattleFrontier_BattlePikeLobby_Text_OneLossEndsIt, MSGBOX_NPC
	end

@ Still called by the Pike corridor and battle_pike.inc.
BattleFrontier_BattlePike_EventScript_CloseCurtain::
	playse SE_PIKE_CURTAIN_CLOSE
	special CloseBattlePikeCurtain
	waitse
	return

BattleFrontier_BattlePikeLobby_Text_NoMoreGuessingDoors:
	.string "No more guessing behind curtains.\n"
	.string "You register and you battle.$"

BattleFrontier_BattlePikeLobby_Text_DeskTakesMyName:
	.string "The desk takes my name and sends me\n"
	.string "straight to the CIRCUIT.$"

BattleFrontier_BattlePikeLobby_Text_OneLossEndsIt:
	.string "One loss ends a run.\n"
	.string "Your best streak is kept.$"
