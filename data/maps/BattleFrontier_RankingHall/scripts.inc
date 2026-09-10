BattleFrontier_RankingHall_MapScripts::
	.byte 0

BattleFrontier_RankingHall_EventScript_TowerSinglesRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_TowerDoublesRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_TowerMultisRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_TowerLinkRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_ArenaRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_PalaceRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_FactoryRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_DomeRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_PikeRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_PyramidRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_ShowRecords::
	special ShowRankingHallRecordsWindow
	waitbuttonpress
	special ScrollRankingHallRecordsWindow
	waitbuttonpress
	special RemoveRecordsWindow
	releaseall
	end

BattleFrontier_RankingHall_EventScript_Attendant::
	msgbox BattleFrontier_RankingHall_Text_ExplainRankingHall, MSGBOX_NPC
	end

BattleFrontier_RankingHall_EventScript_DomePikeFactoryRecordsSign::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_PalaceArenaPyramidRecordsSIgn::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_NinjaBoy::
	lock
	faceplayer
	msgbox BattleFrontier_RankingHall_Text_IsYourNameOnThisList, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, BattleFrontier_RankingHall_EventScript_NinjaBoyNameOnList
	msgbox BattleFrontier_RankingHall_Text_WorkHarderIfYouSawFriendsName, MSGBOX_DEFAULT
	release
	end

BattleFrontier_RankingHall_EventScript_NinjaBoyNameOnList::
	msgbox BattleFrontier_RankingHall_Text_WowThatsSuper, MSGBOX_DEFAULT
	release
	end

BattleFrontier_RankingHall_EventScript_Boy::
	msgbox BattleFrontier_RankingHall_Text_MyNamesNotUpThere, MSGBOX_NPC
	end

BattleFrontier_RankingHall_Text_ExplainRankingHall:
	.string "This hall celebrates the\n"
	.string "Frontier's competitive history.\p"
	.string "Today's boards show CHAMPIONS\n"
	.string "CIRCUIT results: the current run,\l"
	.string "best recorded run, and total wins.$"
BattleFrontier_RankingHall_Text_DomePikeFactoryRecords:
	.string "CHAMPIONS CIRCUIT RECORDS Current\n"
	.string "run, best recorded, total wins$"
BattleFrontier_RankingHall_Text_PalaceArenaPyramidRecords:
	.string "CHAMPIONS CIRCUIT RECORDS Current\n"
	.string "run, best recorded, total wins$"
BattleFrontier_RankingHall_Text_IsYourNameOnThisList:
	.string "Have you set a Circuit record\n"
	.string "you're proud of?$"

BattleFrontier_RankingHall_Text_WowThatsSuper:
	.string "Wow, that's super!\n"
	.string "I'll have to try harder, too!$"

BattleFrontier_RankingHall_Text_WorkHarderIfYouSawFriendsName:
	.string "I'm still working on mine, too!\p"
	.string "Maybe we can compare our best runs\n"
	.string "next time.$"

BattleFrontier_RankingHall_Text_MyNamesNotUpThere:
	.string "I keep checking my records, but I\n"
	.string "haven't entered a challenge yet.\p"
	.string "Funny how the numbers haven't\n"
	.string "moved…$"

