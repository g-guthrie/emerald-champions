MossdeepCity_StevensHouse_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, MossdeepCity_StevensHouse_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, MossdeepCity_StevensHouse_OnTransition
	.byte 0

MossdeepCity_StevensHouse_OnLoad:
	call_if_unset FLAG_SYS_GAME_CLEAR, MossdeepCity_StevensHouse_EventScript_HideStevensNote
	end

MossdeepCity_StevensHouse_EventScript_HideStevensNote::
	setmetatile 6, 4, METATILE_GenericBuilding_TableEdge, TRUE
	return

MossdeepCity_StevensHouse_OnTransition:
	call_if_eq VAR_STEVENS_HOUSE_STATE, 2, MossdeepCity_StevensHouse_EventScript_SetStevenPos
	end

MossdeepCity_StevensHouse_EventScript_SetStevenPos::
	setobjectxyperm LOCALID_STEVENS_HOUSE_STEVEN, 6, 5
	setobjectmovementtype LOCALID_STEVENS_HOUSE_STEVEN, MOVEMENT_TYPE_FACE_UP
	return

MossdeepCity_StevensHouse_EventScript_BeldumPokeball::
	lockall
	msgbox MossdeepCity_StevensHouse_Text_TakeBallContainingBeldum, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MossdeepCity_StevensHouse_EventScript_LeaveBeldum
	goto MossdeepCity_StevensHouse_EventScript_GiveBeldum
	end

MossdeepCity_StevensHouse_EventScript_LeaveBeldum::
	msgbox MossdeepCity_StevensHouse_Text_LeftPokeBallWhereItWas, MSGBOX_DEFAULT
	releaseall
	end

MossdeepCity_StevensHouse_EventScript_GiveBeldum::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_BELDUM
	setvar VAR_0x8004, SPECIES_BELDUM
	setvar VAR_0x8005, 5
	special GiveEmeraldChampionsPreparedPokemon
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY, MossdeepCity_StevensHouse_EventScript_SendBeldumParty
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, MossdeepCity_StevensHouse_EventScript_SendBeldumPC
	goto Common_EventScript_NoMoreRoomForPokemon
	end

MossdeepCity_StevensHouse_EventScript_SendBeldumParty::
	call MossdeepCity_StevensHouse_EventScript_ReceivedBeldumFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MossdeepCity_StevensHouse_EventScript_ReceivedBeldum
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto MossdeepCity_StevensHouse_EventScript_ReceivedBeldum
	end

MossdeepCity_StevensHouse_EventScript_SendBeldumPC::
	call MossdeepCity_StevensHouse_EventScript_ReceivedBeldumFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MossdeepCity_StevensHouse_EventScript_BeldumTransferredToPC
	call Common_EventScript_NameReceivedBoxMon
	goto MossdeepCity_StevensHouse_EventScript_BeldumTransferredToPC
	end

MossdeepCity_StevensHouse_EventScript_BeldumTransferredToPC::
	call Common_EventScript_TransferredToPC
	goto MossdeepCity_StevensHouse_EventScript_ReceivedBeldum
	end

MossdeepCity_StevensHouse_EventScript_ReceivedBeldumFanfare::
	bufferspeciesname STR_VAR_2, SPECIES_BELDUM
	removeobject LOCALID_STEVENS_HOUSE_BALL
	playfanfare MUS_OBTAIN_ITEM
	message MossdeepCity_StevensHouse_Text_ObtainedBeldum
	waitmessage
	waitfanfare
	bufferspeciesname STR_VAR_1, SPECIES_BELDUM
	return

MossdeepCity_StevensHouse_EventScript_ReceivedBeldum::
	setflag FLAG_HIDE_MOSSDEEP_CITY_STEVENS_HOUSE_BELDUM_POKEBALL
	setflag FLAG_RECEIVED_BELDUM
	releaseall
	end

MossdeepCity_StevensHouse_EventScript_RockDisplay::
	msgbox MossdeepCity_StevensHouse_Text_CollectionOfRareRocks, MSGBOX_SIGN
	end

MossdeepCity_StevensHouse_EventScript_Steven::
	lock
	faceplayer
	goto_if_set FLAG_BADGE08_GET, MossdeepCity_StevensHouse_EventScript_ResearchConversation
	call EC_RequireSpace
	msgbox MossdeepCity_StevensHouse_Text_UnderwateCavernBetweenMossdeepSootopolis, MSGBOX_DEFAULT
	release
	end

MossdeepCity_StevensHouse_EventScript_Letter::
	lockall
	msgbox MossdeepCity_StevensHouse_Text_LetterFromSteven, MSGBOX_DEFAULT
	releaseall
	end

MossdeepCity_StevensHouse_Text_UnderwateCavernBetweenMossdeepSootopolis:
	.string "STEVEN: Our next lead is the\n"
	.string "triangle on BIRTH ISLAND. Sail\l"
	.string "there from LILYCOVE HARBOR.\p"
	.string "Resolve DEOXYS, then bring STERN\n"
	.string "the SCANNER from the ABANDONED\l"
	.string "SHIP, on ROUTE 108.$"

MossdeepCity_StevensHouse_Text_TakeBallContainingBeldum:
	.string "{PLAYER} checked the POKé BALL.\p"
	.string "It contained the POKéMON\n"
	.string "BELDUM.\p"
	.string "Take the POKé BALL?$"

MossdeepCity_StevensHouse_Text_ObtainedBeldum:
	.string "{PLAYER} obtained a BELDUM.$"

@ Unused
MossdeepCity_StevensHouse_Text_NoSpaceForAnotherMon:
	.string "There is no space for another POKéMON.$"

MossdeepCity_StevensHouse_Text_LeftPokeBallWhereItWas:
	.string "{PLAYER} left the POKé BALL where\n"
	.string "it was.$"

MossdeepCity_StevensHouse_Text_LetterFromSteven:
	.string "It's a letter.\p"
	.string "… … … … … …\p"
	.string "To {PLAYER}{KUN}…\p"
	.string "I've decided to do a little soul-\n"
	.string "searching and train on the road.\p"
	.string "I don't plan to return home for some\n"
	.string "time.\p"
	.string "I have a favor to ask of you.\p"
	.string "I want you to take the POKé BALL on\n"
	.string "the desk.\p"
	.string "Inside it is a BELDUM, my favorite\n"
	.string "POKéMON.\p"
	.string "I'm counting on you.\p"
	.string "May our paths cross someday.\p"
	.string "STEVEN STONE$"

MossdeepCity_StevensHouse_Text_CollectionOfRareRocks:
	.string "It's a collection of rare rocks and\n"
	.string "stones assembled by STEVEN.$"

MossdeepCity_StevensHouse_EventScript_ReviewResearch::
	goto_if_set FLAG_EC_REPORT_C48_COMPLETE, Common_EventScript_NopReturn
	goto_if_unset FLAG_EC_RESOLVED_DIANCIE, MossdeepCity_StevensHouse_EventScript_ResearchPending
	goto_if_unset FLAG_EC_RESOLVED_JIRACHI, MossdeepCity_StevensHouse_EventScript_ResearchPending
	goto_if_unset FLAG_EC_SURVEYED_ORIGIN_CHAMBER, MossdeepCity_StevensHouse_EventScript_ResearchPending
	goto_if_unset FLAG_EC_SURVEYED_METEOR_CHAMBER, MossdeepCity_StevensHouse_EventScript_ResearchPending
	setflag FLAG_EC_STEVEN_RESEARCH_CONCLUSION
	completemilestone FLAG_EC_REPORT_C48_COMPLETE
	buffermilestonetext FLAG_EC_REPORT_C48_COMPLETE
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	delay 30
	faceplayer
	msgbox MossdeepCity_StevensHouse_Text_ResearchConclusion, MSGBOX_DEFAULT
	call_if_eq VAR_RESULT, 3, MossdeepCity_StevensHouse_EventScript_GrantAndCapLine
	call_if_eq VAR_RESULT, 2, MossdeepCity_StevensHouse_EventScript_GrantLine
	call_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_PrintMilestoneCapLine
	msgbox MossdeepCity_StevensHouse_Text_ResearchDirections, MSGBOX_DEFAULT
	return

MossdeepCity_StevensHouse_EventScript_GrantAndCapLine::
	msgbox MossdeepCity_StevensHouse_Text_GrantAndCapLine, MSGBOX_DEFAULT
	return

MossdeepCity_StevensHouse_EventScript_GrantLine::
	msgbox MossdeepCity_StevensHouse_Text_GrantLine, MSGBOX_DEFAULT
	return

MossdeepCity_StevensHouse_EventScript_ResearchPending::
	msgbox MossdeepCity_StevensHouse_Text_ResearchPending, MSGBOX_DEFAULT
	return

MossdeepCity_StevensHouse_Text_ResearchPending:
	.string "The chamber beneath SOOTOPOLIS\n"
	.string "and JIRACHI's room deep inside\l"
	.string "METEOR FALLS complete our record.\p"
	.string "Resolve both encounters, then bring\n"
	.string "me your findings. Catching them\l"
	.string "is your own choice.$"

MossdeepCity_StevensHouse_Text_ResearchConclusion:
	.string "When we met, you carried a letter.\n"
	.string "Since then, you've stood where\l"
	.string "its authors could only imagine.\p"
	.string "The teams thought understanding\n"
	.string "a POKéMON meant controlling it.\l"
	.string "You've seen how much they missed.\p"
	.string "The crystal light and distant\n"
	.string "song belong to living places,\l"
	.string "not machines to command.$"

MossdeepCity_StevensHouse_Text_GrantAndCapLine:
	.string "DEVON has sent your final grant:\n"
	.string "¥{STR_VAR_2}. Your level cap is now {STR_VAR_1}.$"

MossdeepCity_StevensHouse_Text_GrantLine:
	.string "DEVON has sent your final grant:\n"
	.string "¥{STR_VAR_2}.$"

MossdeepCity_StevensHouse_Text_ResearchDirections:
	.string "SURF east from ROUTE 128 to\n"
	.string "EVER GRANDE. Climb the WATERFALL;\l"
	.string "VICTORY ROAD leads to the LEAGUE.$"

MossdeepCity_StevensHouse_EventScript_ResearchConversation::
	goto_if_set FLAG_EC_REPORT_C48_COMPLETE, MossdeepCity_StevensHouse_EventScript_LeagueSendoff
	call MossdeepCity_StevensHouse_EventScript_ReviewResearch
	release
	end
MossdeepCity_StevensHouse_EventScript_LeagueSendoff::
	msgbox MossdeepCity_StevensHouse_Text_LeagueSendoff, MSGBOX_DEFAULT
	release
	end

MossdeepCity_StevensHouse_Text_LeagueSendoff:
	.string "Our research is in good hands.\n"
	.string "Your next challenge is your own.\p"
	.string "SURF east from ROUTE 128 to\n"
	.string "EVER GRANDE. Climb the WATERFALL,\l"
	.string "then take VICTORY ROAD to the LEAGUE.$"
