@ Emerald Champions: the Dome is a Champions Circuit registration desk. The tournament
@ challenge, its tourney tree and the rental pitches are gone; the two receptionists run
@ BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit from map.json.
BattleFrontier_BattleDomeLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattleDomeLobby_OnWarp
	.byte 0

BattleFrontier_BattleDomeLobby_OnWarp:
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattleDomeLobby_EventScript_TurnPlayerNorth
	.2byte 0

BattleFrontier_BattleDomeLobby_EventScript_TurnPlayerNorth::
	setvar VAR_TEMP_1, 1
	turnobject LOCALID_PLAYER, DIR_NORTH
	end

BattleFrontier_BattleDomeLobby_EventScript_Maniac::
	msgbox BattleFrontier_BattleDomeLobby_Text_DeskLikeAnyOther, MSGBOX_NPC
	end

BattleFrontier_BattleDomeLobby_EventScript_Lass::
	msgbox BattleFrontier_BattleDomeLobby_Text_SameCircuitEveryDesk, MSGBOX_NPC
	end

BattleFrontier_BattleDomeLobby_EventScript_FatMan::
	msgbox BattleFrontier_BattleDomeLobby_Text_LostSixStraight, MSGBOX_NPC
	end

BattleFrontier_BattleDomeLobby_EventScript_Man::
	msgbox BattleFrontier_BattleDomeLobby_Text_SixAtATimeNoItems, MSGBOX_NPC
	end

@ A few OutsideWest event scripts are inserted here instead, two of which are unused
BattleFrontier_OutsideWest_EventScript_Man3::
	msgbox BattleFrontier_OutsideWest_Text_LongDreamedAboutBattleFrontier, MSGBOX_NPC
	end

@ Unused
BattleFrontier_OutsideWest_EventScript_BattleDomeSign2::
	msgbox BattleFrontier_OutsideWest_Text_BattleDomeSign2, MSGBOX_NPC
	end

@ Unused
BattleFrontier_OutsideWest_EventScript_UnderConstructionSign::
	msgbox BattleFrontier_OutsideWest_Text_QuestionMarkUnderConstruction, MSGBOX_NPC
	end

BattleFrontier_BattleDomeLobby_EventScript_RulesBoard::
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN
	end

BattleFrontier_BattleDomeLobby_EventScript_ShowSinglesResults::
BattleFrontier_BattleDomeLobby_EventScript_ShowDoublesResults::
BattleFrontier_BattleDomeLobby_EventScript_ShowPrevTourneyTree::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_BattleDomeLobby_Text_DeskLikeAnyOther:
	.string "This desk registers you for the\n"
	.string "CHAMPIONS CIRCUIT, same as the rest.$"

BattleFrontier_BattleDomeLobby_Text_SameCircuitEveryDesk:
	.string "Every desk on the island books the\n"
	.string "same CIRCUIT. Take the nearest one.$"

BattleFrontier_BattleDomeLobby_Text_LostSixStraight:
	.string "Six of mine went down in four turns.\n"
	.string "I will register again tomorrow.$"

BattleFrontier_BattleDomeLobby_Text_SixAtATimeNoItems:
	.string "Six POKéMON, doubles, no BAG items.\n"
	.string "Prepare before you register.$"

BattleFrontier_BattleDomeLobby_Text_ReceivedPrize:
	.string "{PLAYER} received the prize\n"
	.string "{STR_VAR_1}.$"

@ Unused

BattleFrontier_OutsideWest_Text_LongDreamedAboutBattleFrontier:
	.string "The BATTLE FRONTIER…\n"
	.string "I've long dreamed about a place like it.$"

@ Functionally unused

BattleFrontier_OutsideWest_Text_BattleDomeSign2:
	.string "The BATTLE DOME\n"
	.string "Become the Unbeatable Superstar!$"

@ Functionally unused

BattleFrontier_OutsideWest_Text_QuestionMarkUnderConstruction:
	.string "The ??????\n"
	.string "Under Construction!$"
