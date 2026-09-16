@ Emerald Champions: the Pyramid is a Champions Circuit registration desk. The floor
@ quest, the Battle Bag and the hint machine are retired; the receptionist runs
@ BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit from map.json.
BattleFrontier_BattlePyramidLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattleDomeLobby_OnWarp
	.byte 0

BattleFrontier_BattlePyramidLobby_EventScript_RulesBoard::
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN
	end

BattleFrontier_BattlePyramidLobby_EventScript_ShowResults::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_BattlePyramidLobby_EventScript_HintGiver::
	lockall
	applymovement LOCALID_PYRAMID_LOBBY_HINT_GIVER, Common_Movement_FacePlayer
	waitmovement 0
	msgbox BattleFrontier_BattlePyramidLobby_Text_CircuitObserver, MSGBOX_DEFAULT
	releaseall
	end

BattleFrontier_BattlePyramidLobby_EventScript_Woman::
	msgbox BattleFrontier_BattlePyramidLobby_Text_NoFloorsToClimb, MSGBOX_NPC
	end

BattleFrontier_BattlePyramidLobby_EventScript_FatMan::
	msgbox BattleFrontier_BattlePyramidLobby_Text_KeepYourOwnBag, MSGBOX_NPC
	end

BattleFrontier_BattlePyramidLobby_Text_CircuitObserver::
	.string "I once read the PYRAMID's omens.\p"
	.string "The CIRCUIT asks a different\n"
	.string "question: how will your team\l"
	.string "answer the board before it?\p"
	.string "A prediction helps. A sound\n"
	.string "fallback helps even more.$"

BattleFrontier_BattlePyramidLobby_Text_NoFloorsToClimb:
	.string "There are no floors to climb now.\n"
	.string "The desk books CIRCUIT battles.$"

BattleFrontier_BattlePyramidLobby_Text_KeepYourOwnBag:
	.string "You keep your own BAG now.\n"
	.string "You simply cannot open it in a match.$"
