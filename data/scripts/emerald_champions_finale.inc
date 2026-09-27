@ Shared directions for the post-League campaign. Callers retain their lock.
Common_EventScript_FinaleDirections::
	specialvar VAR_RESULT, GetEmeraldChampionsFinaleStage
	switch VAR_RESULT
	case EC_FINALE_LEAGUE, Common_EventScript_FinaleLeague
	case EC_FINALE_WALLY, Common_EventScript_FinaleWally
	case EC_FINALE_VOYAGE, Common_EventScript_FinaleVoyage
	case EC_FINALE_STEVEN, Common_EventScript_FinaleSteven
	case EC_FINALE_DEOXYS, Common_EventScript_FinaleDeoxys
	case EC_FINALE_BUFFEL, Common_EventScript_FinaleBuffel
	msgbox Common_Text_FinaleComplete, MSGBOX_DEFAULT
	return

Common_EventScript_FinaleWait::
	call Common_EventScript_FinaleDirections
	releaseall
	end

Common_EventScript_FinaleLeague:
	msgbox Common_Text_FinaleLeague, MSGBOX_DEFAULT
	return
Common_EventScript_FinaleWally:
	msgbox Common_Text_FinaleWally, MSGBOX_DEFAULT
	return
Common_EventScript_FinaleVoyage:
	msgbox Common_Text_FinaleVoyage, MSGBOX_DEFAULT
	return
Common_EventScript_FinaleSteven:
	msgbox Common_Text_FinaleSteven, MSGBOX_DEFAULT
	return
Common_EventScript_FinaleDeoxys:
	msgbox Common_Text_FinaleDeoxys, MSGBOX_DEFAULT
	return
Common_EventScript_FinaleBuffel:
	msgbox Common_Text_FinaleBuffel, MSGBOX_DEFAULT
	return

Common_Text_FinaleLeague:
	.string "Become the League Champion first.\p"
	.string "Then Wally will be waiting at\n"
	.string "Victory Road's north exit.$"
Common_Text_FinaleWally:
	.string "Your final expedition starts with\n"
	.string "Wally at Victory Road's north exit.\p"
	.string "He's built a Double Battle team to\n"
	.string "challenge the new Champion!$"
Common_Text_FinaleVoyage:
	.string "Sail aboard the S.S. Tidal between\n"
	.string "Slateport and Lilycove.\p"
	.string "Defeat Colton, Micah, Thomas, Naomi,\n"
	.string "and the pair Lea and Jed aboard.\p"
	.string "Resting in your cabin bed heals your\n"
	.string "team and brings the ship into port.\p"
	.string "If anyone is left, board again to\n"
	.string "finish. Then seek Steven deep in\l"
	.string "Meteor Falls.$"
@ His cave (MeteorFalls_1F_1R 4,2, open after the Hall of Fame) sits beside
@ the top of B1F's western ladder; B1F is reached from the room above the falls.
Common_Text_FinaleSteven:
	.string "Visit Steven deep in Meteor Falls.\p"
	.string "Climb the falls inside, go down a\n"
	.string "ladder, then take the western ladder\l"
	.string "up to his cave.\p"
	.string "A victory there earns your Aurora\n"
	.string "Ticket for the Lilycove ferry.\p"
	.string "If your Bag was full, make room\n"
	.string "and speak to Steven again.$"
Common_Text_FinaleDeoxys:
	.string "Take your Aurora Ticket to the\n"
	.string "Lilycove ferry for Birth Island.\p"
	.string "Solve its strange triangle puzzle,\n"
	.string "then catch or defeat Deoxys.\p"
	.string "If you flee, you can try again.$"
Common_Text_FinaleBuffel:
	.string "Your last opponent is Buffel,\n"
	.string "upstairs in Lilycove's motel.\p"
	.string "Prepare your best six partners.\n"
	.string "This is your final trial!$"
Common_Text_FinaleComplete:
	.string "You became Champion, and then you\n"
	.string "conquered the final trial, too!\p"
	.string "Your journey through Hoenn will be\n"
	.string "talked about for years to come!\p"
	.string "The League remains open whenever\n"
	.string "you want to battle again.$"

Common_EventScript_RecordDeoxysFinale::
	setflag FLAG_EC_FINALE_DEOXYS_RESOLVED
	return
