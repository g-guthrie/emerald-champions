@ Emerald Champions: the Battle Tower lobby is the Champions Circuit desk. The vanilla
@ Tower challenge (its Singles, Doubles, Multis and link-Multis entry paths, the
@ elevator choreography and the link handshake), the Apprentice and the Bravo Trainer
@ reporter are all gone. FRONTIER_MODE_SINGLES is no longer named anywhere here.
@ The Apprentice and Reporter objects are listed for removal from map.json; until then
@ this map hides them on transition.
BattleFrontier_BattleTowerLobby_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, BattleFrontier_BattleTowerLobby_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattleTowerLobby_OnWarp
	.byte 0

BattleFrontier_BattleTowerLobby_OnTransition:
	setflag FLAG_HIDE_APPRENTICE
	setflag FLAG_HIDE_BATTLE_TOWER_REPORTER
	end

BattleFrontier_BattleTowerLobby_OnWarp:
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattleTowerLobby_EventScript_PlayerFaceNorth
	.2byte 0

BattleFrontier_BattleTowerLobby_EventScript_PlayerFaceNorth::
	setvar VAR_TEMP_1, 1
	turnobject LOCALID_PLAYER, DIR_NORTH
	end

BattleFrontier_BattleTowerLobby_EventScript_Woman::
	msgbox BattleFrontier_BattleTowerLobby_Text_StreakIsRecorded, MSGBOX_NPC
	end

BattleFrontier_BattleTowerLobby_EventScript_Boy::
	msgbox BattleFrontier_BattleTowerLobby_Text_LeaveBetweenMatches, MSGBOX_NPC
	end

BattleFrontier_BattleTowerLobby_EventScript_ShowSinglesResults::
BattleFrontier_BattleTowerLobby_EventScript_ShowDoublesResults::
BattleFrontier_BattleTowerLobby_EventScript_ShowMultisResults::
BattleFrontier_BattleTowerLobby_EventScript_ShowLinkMultisResults::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_BattleTowerLobby_EventScript_RulesBoard::
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN
	end

BattleFrontier_BattleTowerLobby_Text_StreakIsRecorded:
	.string "Your wins in a row are recorded\n"
	.string "on the board behind me.$"

BattleFrontier_BattleTowerLobby_Text_LeaveBetweenMatches:
	.string "You may leave between matches.\n"
	.string "A loss is what ends the run.$"

BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit::
	lock
	faceplayer
	goto_if_unset FLAG_BADGE06_GET, BattleFrontier_BattleTowerLobby_EventScript_CircuitLocked
	special ChampionsCircuitTryGiveReward
	call_if_eq VAR_RESULT, 4, BattleFrontier_BattleTowerLobby_EventScript_CircuitRewardPending
	call_if_eq VAR_RESULT, 1, BattleFrontier_BattleTowerLobby_EventScript_CircuitRewardParty
	call_if_eq VAR_RESULT, 2, BattleFrontier_BattleTowerLobby_EventScript_CircuitRewardPC
	goto_if_eq VAR_RESULT, 3, BattleFrontier_BattleTowerLobby_EventScript_CircuitPendingRewardNoRoom
	goto_if_set FLAG_EC_CHAMPIONS_CIRCUIT_EXPLAINED, BattleFrontier_BattleTowerLobby_EventScript_CircuitWelcomeBack
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitWelcome, MSGBOX_DEFAULT
	setflag FLAG_EC_CHAMPIONS_CIRCUIT_EXPLAINED
	goto BattleFrontier_BattleTowerLobby_EventScript_CircuitEntryCheck

BattleFrontier_BattleTowerLobby_EventScript_CircuitWelcomeBack::
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitWelcomeBack, MSGBOX_DEFAULT

BattleFrontier_BattleTowerLobby_EventScript_CircuitEntryCheck::
	special ChampionsCircuitCanEnter
	goto_if_eq VAR_RESULT, FALSE, BattleFrontier_BattleTowerLobby_EventScript_CircuitNeedsParty
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitEnter, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, BattleFrontier_BattleTowerLobby_EventScript_CircuitCancel
	special ChampionsCircuitBegin
	closemessage
	setvar VAR_0x8006, 0
	warp MAP_BATTLE_FRONTIER_BATTLE_TOWER_CORRIDOR, 8, 1
	waitstate
	end

BattleFrontier_BattleTowerLobby_EventScript_CircuitRewardParty::
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitRewardParty, MSGBOX_DEFAULT
	return

BattleFrontier_BattleTowerLobby_EventScript_CircuitRewardPC::
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitRewardPC, MSGBOX_DEFAULT
	return

BattleFrontier_BattleTowerLobby_EventScript_CircuitRewardNoRoom::
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitRewardNoRoom, MSGBOX_DEFAULT
	return

BattleFrontier_BattleTowerLobby_EventScript_CircuitPendingRewardNoRoom::
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitRewardNoRoom, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleTowerLobby_EventScript_CircuitLocked::
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitLocked, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleTowerLobby_EventScript_CircuitNeedsParty::
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitNeedsParty, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleTowerLobby_EventScript_CircuitCancel::
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitCancel, MSGBOX_DEFAULT
	release
	end

BattleFrontier_BattleTowerLobby_Text_CircuitWelcome:
	.string "Welcome to the CHAMPIONS CIRCUIT!\p"
	.string "Every match is a full-team Double\n"
	.string "Battle with a competitive\l"
	.string "opponent.\p"
	.string "Your team uses a Lv. 100 baseline.\n"
	.string "Powerful legends enter at lower\l"
	.string "levels, as they do in the story.\p"
	.string "Opponents grow stronger as your\l"
	.string "run continues, up to Lv. 255.\p"
	.string "Every Frontier desk registers you\n"
	.string "for the central Tower arena.$"

BattleFrontier_BattleTowerLobby_Text_CircuitWelcomeBack:
	.string "Welcome back to the CHAMPIONS CIRCUIT.\n"
	.string "Are your six Pokémon ready?$"

BattleFrontier_BattleTowerLobby_Text_CircuitEnter:
	.string "Your team uses a Lv. 100 baseline.\n"
	.string "Powerful legends keep their\l"
	.string "relative level caps.\p"
	.string "Held items and HP return after\n"
	.string "every match.\p"
	.string "Your difficulty sets the starting\n"
	.string "opponent levels. Begin a new run?$"

BattleFrontier_BattleTowerLobby_Text_CircuitOpponent:
	.string "Match {STR_VAR_2}: your opponent has\n"
	.string "prepared {STR_VAR_1}.\p"
	.string "Their lead is Lv. {STR_VAR_3}.\n"
	.string "The next battle begins now!$"

BattleFrontier_BattleTowerLobby_Text_CircuitWon:
	.string "Victory! Your team has been fully\n"
	.string "restored. Continue the run?$"

BattleFrontier_BattleTowerLobby_Text_CircuitRewardParty:
	.string "Circuit milestone reached!\p"
	.string "{STR_VAR_1} joined your party.$"

BattleFrontier_BattleTowerLobby_Text_CircuitRewardPC:
	.string "Circuit milestone reached!\p"
	.string "{STR_VAR_1} was sent to the PC.$"

BattleFrontier_BattleTowerLobby_Text_CircuitRewardNoRoom:
	.string "A Circuit reward is waiting, but\n"
	.string "both your party and PC are full.\p"
	.string "Make room, then speak to this\n"
	.string "desk again.$"

BattleFrontier_BattleTowerLobby_Text_CircuitLocked:
	.string "The CHAMPIONS CIRCUIT welcomes\n"
	.string "challengers with the FEATHER\l"
	.string "BADGE.\p"
	.string "Return when you've earned it!$"

BattleFrontier_BattleTowerLobby_Text_CircuitNeedsParty:
	.string "The Circuit requires six healthy\n"
	.string "Pokémon and no Eggs.$"

BattleFrontier_BattleTowerLobby_Text_CircuitCancel:
	.string "Prepare any six you like. The Circuit\n"
	.string "will be waiting.$"

BattleFrontier_BattleTowerLobby_Text_CircuitLost:
	.string "Your streak ends at {STR_VAR_2} wins.\p"
	.string "Your original\n"
	.string "team and levels have been restored.$"

BattleFrontier_BattleTowerLobby_Text_CircuitRetire:
	.string "Your streak ends at {STR_VAR_2} wins.\p"
	.string "Your original team and levels have\n"
	.string "been restored.$"

BattleFrontier_BattleTowerLobby_Text_CircuitGenerationFailed:
	.string "The next opponent could not be\n"
	.string "prepared. Your team was restored.$"

BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord::
	.string "CHAMPIONS CIRCUIT\p"
	.string "Current run: {STR_VAR_1}\p"
	.string "Best recorded: {STR_VAR_2}\p"
	.string "Total wins: {STR_VAR_3}$"

BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules::
	.string "CHAMPIONS CIRCUIT\p"
	.string "Bring six healthy Pokémon and no\n"
	.string "Eggs. Every match is doubles; you\l"
	.string "control your own team.\p"
	.string "Your team uses a Lv. 100 baseline.\n"
	.string "Powerful legends enter at lower\l"
	.string "levels, as they do in the story.\p"
	.string "Opponent levels begin from your\l"
	.string "difficulty and rise with wins.\p"
	.string "After each battle, your party and\n"
	.string "held items are restored. You can\l"
	.string "leave between matches.\p"
	.string "Every Frontier desk registers you\n"
	.string "for the central Tower arena.\l"
	.string "Milestone rewards remain recorded.$"

BattleFrontier_EventScript_SaveBattle::
	frontier_savebattle
	goto_if_eq VAR_RESULT, FALSE, BattleFrontier_EventScript_BattleSaveFailed
	playse SE_SAVE
	msgbox BattleFrontier_BattleTowerLobby_Text_BattleRecordedOnPass, MSGBOX_DEFAULT
	goto BattleFrontier_BattleTowerLobby_EventScript_EndSaveBattle

BattleFrontier_EventScript_BattleSaveFailed::
	msgbox BattleFrontier_BattleTowerBattleRoom_Text_RecordCouldntBeSaved, MSGBOX_DEFAULT

BattleFrontier_BattleTowerLobby_EventScript_EndSaveBattle::
	return

BattleFrontier_EventScript_GetCantRecordBattle::
	frontier_get FRONTIER_DATA_RECORD_DISABLED
	return

BattleFrontier_EventScript_GetLvlMode::
	frontier_get FRONTIER_DATA_LVL_MODE
	return

BattleFrontier_Text_ObtainedXBattlePoints:
	.string "{PLAYER} obtained\n"
	.string "{STR_VAR_1} Battle Point(s).$"

BattleFrontier_BattleTowerLobby_Text_ReceivedPrize:
	.string "{PLAYER} received the prize\n"
	.string "{STR_VAR_1}.$"

BattleFrontier_BattleTowerLobby_EventScript_CircuitRewardPending::
	msgbox BattleFrontier_BattleTowerLobby_Text_CircuitRewardPending, MSGBOX_DEFAULT
	setvar VAR_RESULT, 0
	return

BattleFrontier_BattleTowerLobby_Text_CircuitRewardPending:
	.string "Your reward remains recorded.\p"
	.string "Review this Pokémon's legendary lead\n"
	.string "for any remaining conditions.\p"
	.string "Your wins will not be lost.$"
