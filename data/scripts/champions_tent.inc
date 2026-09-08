ChampionsTent_EventScript_Attendant::
	special ChampionsTentTryGivePrize
	goto_if_eq VAR_RESULT, 1, ChampionsTent_EventScript_ReceivedPrize
	goto_if_eq VAR_RESULT, 2, ChampionsTent_EventScript_PrizeFull
ChampionsTent_EventScript_Choose::
	message ChampionsTent_Text_Challenge
	waitmessage
	multichoice 17, 6, MULTI_CHALLENGEINFO, FALSE
	switch VAR_RESULT
	case 0, ChampionsTent_EventScript_Enter
	case 1, ChampionsTent_EventScript_Info
	goto ChampionsTent_EventScript_Exit

ChampionsTent_EventScript_Info::
	msgbox ChampionsTent_Text_Rules, MSGBOX_DEFAULT
	goto ChampionsTent_EventScript_Choose

ChampionsTent_EventScript_Enter::
	special ChampionsTentCanEnter
	goto_if_eq VAR_RESULT, FALSE, ChampionsTent_EventScript_NeedParty
	special ChampionsTentBegin
ChampionsTent_EventScript_NextBattle::
	special ChampionsTentGenerateOpponent
	goto_if_eq VAR_RESULT, 0, ChampionsTent_EventScript_NoOpponent
	msgbox ChampionsTent_Text_Opponent, MSGBOX_DEFAULT
	closemessage
	special BattleSetup_StartChampionsCircuitBattle
	special ChampionsTentHandleBattleResult
	goto_if_eq VAR_RESULT, 0, ChampionsTent_EventScript_Lost
	goto_if_eq VAR_RESULT, 2, ChampionsTent_EventScript_Complete
	msgbox ChampionsTent_Text_Continue, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, ChampionsTent_EventScript_NextBattle
	special ChampionsTentEnd
	msgbox ChampionsTent_Text_Thanks, MSGBOX_DEFAULT
	goto ChampionsTent_EventScript_Exit

ChampionsTent_EventScript_Complete::
	special ChampionsTentEnd
	special ChampionsTentTryGivePrize
	goto_if_eq VAR_RESULT, 2, ChampionsTent_EventScript_PrizeFull
	goto_if_eq VAR_RESULT, 0, ChampionsTent_EventScript_Exit
ChampionsTent_EventScript_ReceivedPrize::
	msgbox ChampionsTent_Text_Prize, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	goto ChampionsTent_EventScript_Exit

ChampionsTent_EventScript_Lost::
	special ChampionsTentEnd
	msgbox ChampionsTent_Text_Thanks, MSGBOX_DEFAULT
	goto ChampionsTent_EventScript_Exit

ChampionsTent_EventScript_NoOpponent::
	special ChampionsTentEnd
	msgbox ChampionsTent_Text_NoOpponent, MSGBOX_DEFAULT
	goto ChampionsTent_EventScript_Exit

ChampionsTent_EventScript_NeedParty::
	msgbox ChampionsTent_Text_NeedParty, MSGBOX_DEFAULT
	goto ChampionsTent_EventScript_Exit

ChampionsTent_EventScript_PrizeFull::
	msgbox ChampionsTent_Text_PrizeFull, MSGBOX_DEFAULT
ChampionsTent_EventScript_Exit::
	closemessage
	release
	end

ChampionsTent_Text_Challenge:
	.string "Take on three DOUBLE BATTLES?$"
ChampionsTent_Text_Rules:
	.string "Bring six Pokémon ready to battle.\n"
	.string "EGGS cannot enter.\p"
	.string "Your team fights at your current\n"
	.string "level cap. Your chosen builds stay.\p"
	.string "Win three battles for a prize.\n"
	.string "You may retire between battles.\p"
	.string "Your party is restored afterward.\n"
	.string "These wins are separate from the\l"
	.string "CHAMPIONS CIRCUIT.$"
ChampionsTent_Text_Opponent:
	.string "Battle {STR_VAR_2}: {STR_VAR_1}!\n"
	.string "The opposing team is Lv. {STR_VAR_3}.$"
ChampionsTent_Text_Continue:
	.string "Well played! Your team is healed.\n"
	.string "Continue the exhibition?$"
ChampionsTent_Text_Thanks:
	.string "Thank you for battling!\n"
	.string "Come back with your next idea.$"
ChampionsTent_Text_Prize:
	.string "Three wins! Here is your prize:\n"
	.string "{PLAYER} received {STR_VAR_1}!$"
ChampionsTent_Text_PrizeFull:
	.string "Your prize is waiting here.\n"
	.string "Make room in your BAG and return.$"
ChampionsTent_Text_NeedParty:
	.string "Bring six Pokémon able to battle.\n"
	.string "Prepare at a POKéMON CENTER, then\l"
	.string "come back when you are ready.$"
ChampionsTent_Text_NoOpponent:
	.string "The next team is not ready yet.\n"
	.string "Please try the exhibition again.$"
