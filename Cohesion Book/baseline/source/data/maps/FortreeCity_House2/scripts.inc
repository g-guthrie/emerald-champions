FortreeCity_House2_MapScripts::
	.byte 0

FortreeCity_House2_EventScript_DubiousDiscGiver::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_FORTREE_DUBIOUS_DISC, FortreeCity_House2_EventScript_ExplainDubiousDisc
	call_if_unset FLAG_MET_HIDDEN_POWER_GIVER, FortreeCity_House2_EventScript_Greeting
	msgbox FortreeCity_House2_Text_CoinInWhichHand, MSGBOX_DEFAULT
	multichoice 21, 8, MULTI_RIGHTLEFT, TRUE
	switch VAR_RESULT
	case 1, FortreeCity_House2_EventScript_WrongGuess
	msgbox FortreeCity_House2_Text_CorrectTryAgainWhichHand, MSGBOX_DEFAULT
	multichoice 21, 8, MULTI_RIGHTLEFT, TRUE
	switch VAR_RESULT
	case 1, FortreeCity_House2_EventScript_WrongGuess
	msgbox FortreeCity_House2_Text_CorrectTryAgainWhichHand2, MSGBOX_DEFAULT
	multichoice 21, 8, MULTI_RIGHTLEFT, TRUE
	switch VAR_RESULT
	case 0, FortreeCity_House2_EventScript_WrongGuess
	msgbox FortreeCity_House2_Text_YourHiddenPowerHasAwoken, MSGBOX_DEFAULT
	giveitem ITEM_DUBIOUS_DISC
	goto_if_eq VAR_RESULT, 0, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_FORTREE_DUBIOUS_DISC
	msgbox FortreeCity_House2_Text_ExplainDubiousDisc, MSGBOX_DEFAULT
	release
	end

FortreeCity_House2_EventScript_Greeting::
	msgbox FortreeCity_House2_Text_HiddenPowersArousedByNature, MSGBOX_DEFAULT
	setflag FLAG_MET_HIDDEN_POWER_GIVER
	return

FortreeCity_House2_EventScript_ExplainDubiousDisc::
	msgbox FortreeCity_House2_Text_ExplainDubiousDisc, MSGBOX_DEFAULT
	release
	end

FortreeCity_House2_EventScript_WrongGuess::
	msgbox FortreeCity_House2_Text_YouGuessedWrong, MSGBOX_DEFAULT
	release
	end

FortreeCity_House2_Text_HiddenPowersArousedByNature:
	.string "People… POKéMON…\p"
	.string "Their hidden powers are aroused by\n"
	.string "living in natural environments…$"

FortreeCity_House2_Text_CoinInWhichHand:
	.string "Let this old woman see if your hidden\n"
	.string "power has awoken…\p"
	.string "I hold a coin in my hand.\p"
	.string "Now, tell me, have I palmed it in\n"
	.string "the right hand? Or in the left?$"

FortreeCity_House2_Text_CorrectTryAgainWhichHand:
	.string "Oh! Yes, correct!\p"
	.string "We shall try again.\p"
	.string "In which hand have I palmed the coin?\n"
	.string "The right or left?$"

FortreeCity_House2_Text_CorrectTryAgainWhichHand2:
	.string "Oh! Yes, correct!\p"
	.string "We shall try again.\p"
	.string "In which hand have I palmed the coin?\n"
	.string "The right or left?$"
FortreeCity_House2_Text_YourHiddenPowerHasAwoken:
	.string "Your insight has awoken!\p"
	.string "Take this Dubious Disc for Porygon2.$"
FortreeCity_House2_Text_ExplainDubiousDisc:
	.string "A DUBIOUS DISC evolves PORYGON2.\n"
	.string "Use it from the Bag when ready.$"

FortreeCity_House2_Text_YouGuessedWrong:
	.string "No, too bad.\n"
	.string "You guessed wrong.$"
