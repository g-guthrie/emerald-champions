AbandonedShip_Room_B1F_MapScripts::
	.byte 0

AbandonedShip_Room_B1F_EventScript_Spiritomb::
	lock
	checkspecies SPECIES_LICKITUNG
	goto_if_eq VAR_RESULT, TRUE, AbandonedShip_Room_B1F_EventScript_CheckOddKeystone
	checkspecies SPECIES_SLUGMA
	goto_if_eq VAR_RESULT, TRUE, AbandonedShip_Room_B1F_EventScript_CheckOddKeystone
AbandonedShip_Room_B1F_EventScript_Empty::
	msgbox Text_EmptyTrashCan, MSGBOX_DEFAULT
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
	.string "Something compels you to place the\n"
	.string "ODD KEYSTONE in this trash can.\p"
	.string "Place the ODD KEYSTONE?$"
