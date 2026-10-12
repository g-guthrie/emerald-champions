@ When a REPEL wears off, offer another of the same kind if the player has one;
@ otherwise offer the strongest kind still in the BAG.
EventScript_RepelWoreOff::
	lockall
	bufferitemname STR_VAR_1, ITEM_REPEL
	goto_if_eq VAR_LAST_REPEL_LURE_USED, ITEM_NONE, EventScript_RepelWoreOff_NameKnown
	bufferitemname STR_VAR_1, VAR_LAST_REPEL_LURE_USED
EventScript_RepelWoreOff_NameKnown:
	specialvar VAR_0x8004, GetRepelToOffer
	goto_if_eq VAR_0x8004, ITEM_NONE, EventScript_RepelWoreOff_NoneLeft
	bufferitemname STR_VAR_2, VAR_0x8004
	goto_if_ne VAR_0x8004, VAR_LAST_REPEL_LURE_USED, EventScript_RepelWoreOff_OfferOther
	msgbox Text_RepelWoreOffUseAnother, MSGBOX_YESNO
	goto EventScript_RepelWoreOff_Answer

EventScript_RepelWoreOff_OfferOther:
	msgbox Text_RepelWoreOffUseX, MSGBOX_YESNO
EventScript_RepelWoreOff_Answer:
	goto_if_eq VAR_RESULT, NO, EventScript_RepelWoreOff_End
	special UseRepelFromPrompt
	playse SE_REPEL
	waitse
	msgbox Text_PlayerUsedRepel, MSGBOX_DEFAULT
EventScript_RepelWoreOff_End:
	closemessage
	releaseall
	end

EventScript_RepelWoreOff_NoneLeft:
	msgbox Text_RepelWoreOff, MSGBOX_DEFAULT
	closemessage
	releaseall
	end

Text_RepelWoreOff:
	.string "{STR_VAR_1}'s effect wore off…$"

Text_RepelWoreOffUseAnother:
	.string "{STR_VAR_1}'s effect wore off…\n"
	.string "Use another?$"

Text_RepelWoreOffUseX:
	.string "{STR_VAR_1}'s effect wore off…\n"
	.string "Use a {STR_VAR_2}?$"

Text_PlayerUsedRepel:
	.string "{PLAYER} used the\n"
	.string "{STR_VAR_2}.$"
