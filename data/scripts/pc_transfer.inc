@ VAR_0x8004 here is used by ChangePokemonNickname
Common_EventScript_GetGiftMonPartySlot::
	getpartysize
	subvar VAR_RESULT, 1
	copyvar VAR_0x8004, VAR_RESULT
	return

Common_EventScript_NameReceivedBoxMon::
	fadescreen FADE_TO_BLACK
	special ChangeBoxPokemonNickname
	waitstate
	lock
	faceplayer
	return

Common_EventScript_TransferredToPC::
	bufferboxname 0, VAR_PC_BOX_TO_SEND_MON
	bufferspeciesname 1, VAR_TEMP_1
	call_if_unset FLAG_SYS_PC_LANETTE, EventScript_TransferredSomeonesPC
	call_if_set FLAG_SYS_PC_LANETTE, EventScript_TransferredLanettesPC
	call Common_EventScript_OfferLegendaryGiftSwap
	return

Common_EventScript_OfferLegendaryGiftSwap::
	specialvar VAR_RESULT, GetBoxedLegendaryGiftSwapStatus
	compare VAR_RESULT, 0
	goto_if_eq Common_EventScript_LegendaryGiftSwapDone
	compare VAR_RESULT, 2
	goto_if_eq Common_EventScript_LegendaryGiftHasMail
	msgbox Common_Text_ReplacePartyLegendary, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Common_EventScript_LegendaryGiftSwapDone
	specialvar VAR_RESULT, SwapBoxedLegendaryGiftWithParty
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_LegendaryGiftSwapDone
	bufferboxname 2, VAR_PC_BOX_TO_SEND_MON
	msgbox Common_Text_SwappedPartyLegendary, MSGBOX_DEFAULT
	return

Common_EventScript_LegendaryGiftHasMail:
	msgbox Common_Text_PartyLegendaryHasMail, MSGBOX_DEFAULT
Common_EventScript_LegendaryGiftSwapDone:
	setvar VAR_RESULT, FALSE
	return

Common_Text_ReplacePartyLegendary:
	.string "Send {STR_VAR_1} to the PC and put\n"
	.string "{STR_VAR_2} in your party?$"

Common_Text_SwappedPartyLegendary:
	.string "{STR_VAR_2} joined your party!\p"
	.string "{STR_VAR_1} was sent to\n"
	.string "{STR_VAR_3}.$"

Common_Text_PartyLegendaryHasMail:
	.string "{STR_VAR_1} has Mail attached.\n"
	.string "Remove it before swapping.\p"
	.string "Your gift is safe in the PC.$"

EventScript_TransferredSomeonesPC::
	specialvar VAR_RESULT, ShouldShowBoxWasFullMessage
	compare VAR_RESULT, 1
	goto_if_eq EventScript_SomeonesPCBoxFull
	msgbox gText_PkmnTransferredSomeonesPC, MSGBOX_DEFAULT
	return

EventScript_SomeonesPCBoxFull::
	specialvar VAR_RESULT, GetPCBoxToSendMon
	bufferboxname 2, VAR_RESULT
	msgbox gText_PkmnTransferredSomeonesPCBoxFull, MSGBOX_DEFAULT
	return

EventScript_TransferredLanettesPC::
	specialvar VAR_RESULT, ShouldShowBoxWasFullMessage
	compare VAR_RESULT, TRUE
	goto_if_eq EventScript_LanettesPCBoxFull
	msgbox gText_PkmnTransferredLanettesPC, MSGBOX_DEFAULT
	return

EventScript_LanettesPCBoxFull::
	specialvar VAR_RESULT, GetPCBoxToSendMon
	bufferboxname 2, VAR_RESULT
	msgbox gText_PkmnTransferredLanettesPCBoxFull, MSGBOX_DEFAULT
	return

Common_EventScript_NoMoreRoomForPokemon::
	msgbox gText_NoMoreRoomForPokemon, MSGBOX_DEFAULT
	release
	end
