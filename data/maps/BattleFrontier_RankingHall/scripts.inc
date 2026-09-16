@ Emerald Champions: the twelve duplicate boards are collapsed to one Circuit record
@ board (TowerDoublesRecords) and one Mega register plaque (DomePikeFactoryRecordsSign).
@ The ten remaining sign objects read the plaque until they are removed from map.json.
BattleFrontier_RankingHall_MapScripts::
	.byte 0

BattleFrontier_RankingHall_EventScript_TowerDoublesRecords::
	special ChampionsCircuitBufferRecord
	msgbox BattleFrontier_BattleTowerLobby_Text_CurrentCircuitRecord, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_TowerSinglesRecords::
BattleFrontier_RankingHall_EventScript_TowerMultisRecords::
BattleFrontier_RankingHall_EventScript_TowerLinkRecords::
BattleFrontier_RankingHall_EventScript_ArenaRecords::
BattleFrontier_RankingHall_EventScript_PalaceRecords::
BattleFrontier_RankingHall_EventScript_FactoryRecords::
BattleFrontier_RankingHall_EventScript_DomeRecords::
BattleFrontier_RankingHall_EventScript_PikeRecords::
BattleFrontier_RankingHall_EventScript_PyramidRecords::
BattleFrontier_RankingHall_EventScript_PalaceArenaPyramidRecordsSIgn::
BattleFrontier_RankingHall_EventScript_DomePikeFactoryRecordsSign::
	msgbox BattleFrontier_RankingHall_Text_MegaRegisterPlaque, MSGBOX_SIGN
	end

BattleFrontier_RankingHall_EventScript_Attendant::
	msgbox BattleFrontier_RankingHall_Text_ExplainRankingHall, MSGBOX_NPC
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
	.string "One board now, and one plaque.\p"
	.string "The board shows your CIRCUIT run,\n"
	.string "your best and your total wins.$"

BattleFrontier_RankingHall_Text_MegaRegisterPlaque:
	.string "MEGA REGISTER\p"
	.string "Ninety-nine stones are recorded in\n"
	.string "HOENN. A RING wakes them; a partner\l"
	.string "that trusts you does the rest.$"

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
	.string "I keep checking the board, but I\n"
	.string "haven't entered a run yet.\p"
	.string "Funny how the numbers haven't\n"
	.string "moved…$"
