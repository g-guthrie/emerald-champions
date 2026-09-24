AbandonedShip_Room_B1F_MapScripts:: @ 82380A5
	.byte 0

AbandonedShip_Room_B1F_EventScript_Spiritomb::
	lock
	checkPartyHasSpecies SPECIES_LICKITUNG
	compare VAR_RESULT, TRUE
	goto_if_eq AbandonedShip_Room_B1F_EventScript_CheckOddKeystone
	checkPartyHasSpecies SPECIES_SLUGMA
	compare VAR_RESULT, TRUE
	goto_if_eq AbandonedShip_Room_B1F_EventScript_CheckOddKeystone
AbandonedShip_Room_B1F_EventScript_ItsEmpty::
	msgbox Text_EmptyTrashCan, MSGBOX_DEFAULT
	closemessage
	release
	end

AbandonedShip_Room_B1F_EventScript_CheckOddKeystone::
	checkitem ITEM_ODD_KEYSTONE, 1
	compare VAR_RESULT, FALSE
	goto_if_eq AbandonedShip_Room_B1F_EventScript_ItsEmpty
	msgbox AbandonedShip_Room_B1F_Text_PutKeystoneInTrash, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq AbandonedShip_Room_B1F_EventScript_End
	closemessage
	special GetLevelCapForScriptedGift
	setwildbattle SPECIES_SPIRITOMB, VAR_0x800A, ITEM_SMOKE_BALL
	waitse
	playmoncry SPECIES_SPIRITOMB, 2
	delay 40
	waitmoncry
	dowildbattle
	@ Only a capture or a knockout spends the Keystone. The player keeps it
	@ after running away, and after a whiteout (which never returns here).
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, AbandonedShip_Room_B1F_EventScript_SpendKeystone
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, AbandonedShip_Room_B1F_EventScript_SpendKeystone
	msgbox AbandonedShip_Room_B1F_Text_KeystoneRolledBack, MSGBOX_DEFAULT
	closemessage
	release
	end

AbandonedShip_Room_B1F_EventScript_SpendKeystone::
	removeitem ITEM_ODD_KEYSTONE, 1
	release
	end

AbandonedShip_Room_B1F_EventScript_End::
	closemessage
	release
	end

AbandonedShip_Room_B1F_Text_PutKeystoneInTrash:
	.string "Something is compelling you to toss\n"
	.string "the Odd Keystone into this trash can.\p"
	.string "Trash the Odd Keystone?$"

AbandonedShip_Room_B1F_Text_KeystoneRolledBack:
	.string "The Odd Keystone rolled back out\n"
	.string "of the trash can.\p"
	.string "{PLAYER} picked the Odd Keystone\n"
	.string "back up.$"
