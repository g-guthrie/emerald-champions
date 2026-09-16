@ Emerald Champions: the Arena is a Champions Circuit registration desk. Its judged
@ three-turn challenge is retired; the receptionist runs
@ BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit from map.json.
@ The strings below the scripts are still read by FallarborTown_BattleTentLobby.
BattleFrontier_BattleArenaLobby_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleFrontier_BattleArenaLobby_OnWarp
	.byte 0

BattleFrontier_BattleArenaLobby_OnWarp:
	map_script_2 VAR_TEMP_1, 0, BattleFrontier_BattleArenaLobby_EventScript_TurnPlayerNorth
	.2byte 0

BattleFrontier_BattleArenaLobby_EventScript_TurnPlayerNorth::
	setvar VAR_TEMP_1, 1
	turnobject LOCALID_PLAYER, DIR_NORTH
	end

BattleFrontier_BattleArenaLobby_EventScript_RulesBoard::
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRules, MSGBOX_SIGN
	end

BattleFrontier_BattleArenaLobby_EventScript_ShowResults::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_BattleArenaLobby_EventScript_Youngster::
	msgbox BattleFrontier_BattleArenaLobby_Text_NoJudgesAnyMore, MSGBOX_NPC
	end

BattleFrontier_BattleArenaLobby_EventScript_Man::
	msgbox BattleFrontier_BattleArenaLobby_Text_WinOrLoseNothingElse, MSGBOX_NPC
	end

BattleFrontier_BattleArenaLobby_EventScript_Camper::
	msgbox BattleFrontier_BattleArenaLobby_Text_FerryFromLilycove, MSGBOX_NPC
	end

BattleFrontier_BattleArenaLobby_EventScript_Woman::
	msgbox BattleFrontier_BattleArenaLobby_Text_HealBeforeYouRegister, MSGBOX_NPC
	end

BattleFrontier_BattleArenaLobby_Text_NoJudgesAnyMore:
	.string "There are no judges here now.\n"
	.string "You win the battle or you don't.$"

BattleFrontier_BattleArenaLobby_Text_WinOrLoseNothingElse:
	.string "One loss ends the run.\n"
	.string "That is the whole rule.$"

BattleFrontier_BattleArenaLobby_Text_FerryFromLilycove:
	.string "The ferry from LILYCOVE is the only\n"
	.string "way onto this island.$"

BattleFrontier_BattleArenaLobby_Text_HealBeforeYouRegister:
	.string "Your team is restored between\n"
	.string "CIRCUIT battles. Held items are not.$"

BattleFrontier_BattleArenaLobby_Text_ExplainBattleRules:
	.string "The Set KO Tourney has special rules,\n"
	.string "unlike standard battles.\p"
	.string "First, one battle lasts only\n"
	.string "three turns.\p"
	.string "If the outcome is not decided after\n"
	.string "three turns, it goes to judging.\p"
	.string "The REFEREE will choose the winner\n"
	.string "based on how the POKéMON battled.\p"
	.string "Also, a POKéMON cannot be switched out\n"
	.string "until its battle's outcome is decided.$"

BattleFrontier_BattleArenaLobby_Text_ExplainMindRules:
	.string "The first judging factor is “Mind.”\n"
	.string "This factor evaluates how aggressive\l"
	.string "the battlers were.\p"
	.string "The judging is based on how often\n"
	.string "the TRAINERS ordered the use of\l"
	.string "offensive moves.$"

BattleFrontier_BattleArenaLobby_Text_ExplainSkillRules:
	.string "The second judging factor is “Skill.”\n"
	.string "This factor evaluates how effectively\l"
	.string "POKéMON moves were used.\p"
	.string "If a move worked successfully,\n"
	.string "the Skill rating goes up.\p"
	.string "If a move failed, the Skill rating\n"
	.string "is lowered.\p"
	.string "If it was an offensive move, the Skill\n"
	.string "rating goes up if the move was “super\l"
	.string "effective” and goes down if it was\l"
	.string "“not very effective.”\p"
	.string "The Skill rating will not go up for\n"
	.string "moves such as PROTECT and DETECT.\p"
	.string "If the opponent used PROTECT or\n"
	.string "DETECT, and your POKéMON failed to\l"
	.string "hit with a move, its Skill rating will not\l"
	.string "go down.$"

BattleFrontier_BattleArenaLobby_Text_ExplainBodyRules:
	.string "The third judging factor is “Body.”\n"
	.string "This factor is based on how much HP\l"
	.string "remained at the end of a battle.\p"
	.string "The rating takes into account how much\n"
	.string "HP a POKéMON had at the start of\l"
	.string "battle, and what remained at the end.$"

BattleFrontier_BattleArenaLobby_Text_ReadWhichHeading:
	.string "Which heading do you want to read?$"
