AbandonedShip_Room_B1F_MapScripts::
	.byte 0

AbandonedShip_Room_B1F_EventScript_Spiritomb::
	lock
	goto AbandonedShip_Room_B1F_EventScript_CheckOddKeystone

AbandonedShip_Room_B1F_EventScript_Empty::
	msgbox AbandonedShip_Room_B1F_Text_KeystoneMissing, MSGBOX_DEFAULT
	release
	end

AbandonedShip_Room_B1F_EventScript_CheckOddKeystone::
	checkitem ITEM_ODD_KEYSTONE, 1
	goto_if_eq VAR_RESULT, FALSE, AbandonedShip_Room_B1F_EventScript_Empty
	msgbox AbandonedShip_Room_B1F_Text_PutKeystoneInTrash, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, AbandonedShip_Room_B1F_EventScript_End
	closemessage
	setvar VAR_0x8004, SPECIES_SPIRITOMB
	setvar VAR_0x8005, 0
	special CreateEmeraldChampionsStaticLegendaryEncounter
	waitse
	playmoncry SPECIES_SPIRITOMB, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	special BattleSetup_StartLegendaryBattle
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, AbandonedShip_Room_B1F_EventScript_End
	removeitem ITEM_ODD_KEYSTONE, 1
	release
	end

AbandonedShip_Room_B1F_EventScript_End::
	closemessage
	release
	end

AbandonedShip_Room_B1F_Text_PutKeystoneInTrash:
	.string "The ODD KEYSTONE fits the cracked\n"
	.string "socket in this old bin.\p"
	.string "A whisper rises from the hull. Set\n"
	.string "the stone in place?$"

AbandonedShip_Room_B1F_Text_KeystoneMissing::
	.string "Cold air seeps from the old bin.\p"
	.string "A cracked socket in its base looks\n"
	.string "made for an ODD KEYSTONE.$"
