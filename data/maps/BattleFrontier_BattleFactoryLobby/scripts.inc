@ Emerald Champions: the Factory is a Champions Circuit registration desk. Rentals and
@ swaps are retired; the two receptionists run
@ BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit from map.json.
@ The string below the scripts is still read by SlateportCity_BattleTentLobby.
BattleFrontier_BattleFactoryLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattleFactoryLobby_OnWarp
	.byte 0

BattleFrontier_BattleFactoryLobby_OnWarp:
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattleFactoryLobby_EventScript_TurnPlayerNorth
	.2byte 0

BattleFrontier_BattleFactoryLobby_EventScript_TurnPlayerNorth::
	setvar VAR_TEMP_1, 1
	turnobject LOCALID_PLAYER, DIR_NORTH
	end

BattleFrontier_BattleFactoryLobby_EventScript_RulesBoard::
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN
	end

BattleFrontier_BattleFactoryLobby_EventScript_ShowSinglesResults::
BattleFrontier_BattleFactoryLobby_EventScript_ShowDoublesResults::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_BattleFactoryLobby_EventScript_Woman::
	msgbox BattleFrontier_BattleFactoryLobby_Text_NoRentalsHere, MSGBOX_NPC
	end

BattleFrontier_BattleFactoryLobby_EventScript_Camper::
	msgbox BattleFrontier_BattleFactoryLobby_Text_BringYourOwnSix, MSGBOX_NPC
	end

BattleFrontier_BattleFactoryLobby_EventScript_Picnicker::
	msgbox BattleFrontier_BattleFactoryLobby_Text_RaisedThemMyself, MSGBOX_NPC
	end

BattleFrontier_BattleFactoryLobby_EventScript_FatMan::
	msgbox BattleFrontier_BattleFactoryLobby_Text_PointsBuyStonesAndBalls, MSGBOX_NPC
	end

BattleFrontier_BattleFactoryLobby_Text_NoRentalsHere:
	.string "Nobody lends POKéMON here any more.\n"
	.string "This is a registration desk.$"

BattleFrontier_BattleFactoryLobby_Text_BringYourOwnSix:
	.string "Bring your own six.\n"
	.string "They fight at level 100 regardless.$"

BattleFrontier_BattleFactoryLobby_Text_RaisedThemMyself:
	.string "I raised every one of mine.\n"
	.string "That is the point, isn't it?$"

BattleFrontier_BattleFactoryLobby_Text_PointsBuyStonesAndBalls:
	.string "BATTLE POINTS buy evolution items\n"
	.string "and rare BALLS at the EXCHANGE.$"

BattleFrontier_BattleFactoryLobby_Text_ReadWhichHeading:
	.string "Which heading do you want to read?$"
