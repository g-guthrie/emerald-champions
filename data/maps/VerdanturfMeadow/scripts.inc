VerdanturfMeadow_MapScripts::
	.byte 0

VerdanturfMeadow_EventScript_Warden::
	msgbox VerdanturfMeadow_Text_Warden, MSGBOX_NPC
	end
VerdanturfMeadow_Text_Warden:
	.string "This meadow heals after every storm.\p"
	.string "Bring ROSERADE after you master the heat.\n"
	.string "A tiny guardian may reveal itself.$"
.set EC_SIGN_SHAYMIN_ID, 33

VerdanturfMeadow_EventScript_Shaymin::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_SHAYMIN_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, VerdanturfMeadow_EventScript_ShayminDormant
	goto_if_eq VAR_RESULT, 1, VerdanturfMeadow_EventScript_ShayminNeedsPartner
	goto_if_eq VAR_RESULT, 4, VerdanturfMeadow_EventScript_ShayminCleanup
	msgbox VerdanturfMeadow_Text_ShayminAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_SHAYMIN, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_SHAYMIN_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, VerdanturfMeadow_EventScript_ShayminCaught
	msgbox VerdanturfMeadow_Text_ShayminRemains, MSGBOX_DEFAULT
	release
	end

VerdanturfMeadow_EventScript_ShayminDormant::
	msgbox VerdanturfMeadow_Text_ShayminDormant, MSGBOX_DEFAULT
	release
	end

VerdanturfMeadow_EventScript_ShayminNeedsPartner::
	msgbox VerdanturfMeadow_Text_ShayminNeedsPartner, MSGBOX_DEFAULT
	release
	end

VerdanturfMeadow_EventScript_ShayminCaught::
	setflag FLAG_EC_CAUGHT_SHAYMIN
VerdanturfMeadow_EventScript_ShayminCleanup::
	setflag FLAG_EC_CAUGHT_SHAYMIN
	removeobject LOCALID_EC_SHAYMIN
	release
	end

VerdanturfMeadow_Text_ShayminDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

VerdanturfMeadow_Text_ShayminNeedsPartner:
	.string "The SIGN sketches ROSERADE.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

VerdanturfMeadow_Text_ShayminAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "SHAYMIN answers the challenge!$"

VerdanturfMeadow_Text_ShayminRemains:
	.string "The SIGN remains bright. SHAYMIN\n"
	.string "will accept another challenge.$"

.set EC_SIGN_ENAMORUS_ID, 55

VerdanturfMeadow_EventScript_Enamorus::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_ENAMORUS_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, VerdanturfMeadow_EventScript_EnamorusDormant
	goto_if_eq VAR_RESULT, 1, VerdanturfMeadow_EventScript_EnamorusNeedsPartner
	goto_if_eq VAR_RESULT, 4, VerdanturfMeadow_EventScript_EnamorusCleanup
	msgbox VerdanturfMeadow_Text_EnamorusAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_ENAMORUS, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_ENAMORUS_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, VerdanturfMeadow_EventScript_EnamorusCaught
	msgbox VerdanturfMeadow_Text_EnamorusRemains, MSGBOX_DEFAULT
	release
	end

VerdanturfMeadow_EventScript_EnamorusDormant::
	msgbox VerdanturfMeadow_Text_EnamorusDormant, MSGBOX_DEFAULT
	release
	end

VerdanturfMeadow_EventScript_EnamorusNeedsPartner::
	msgbox VerdanturfMeadow_Text_EnamorusNeedsPartner, MSGBOX_DEFAULT
	release
	end

VerdanturfMeadow_EventScript_EnamorusCaught::
	setflag FLAG_EC_CAUGHT_ENAMORUS
VerdanturfMeadow_EventScript_EnamorusCleanup::
	setflag FLAG_EC_CAUGHT_ENAMORUS
	removeobject LOCALID_EC_ENAMORUS
	release
	end

VerdanturfMeadow_Text_EnamorusDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

VerdanturfMeadow_Text_EnamorusNeedsPartner:
	.string "The SIGN sketches SHAYMIN.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

VerdanturfMeadow_Text_EnamorusAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "ENAMORUS answers the challenge!$"

VerdanturfMeadow_Text_EnamorusRemains:
	.string "The SIGN remains bright. ENAMORUS\n"
	.string "will accept another challenge.$"

.set EC_SIGN_FEZANDIPITI_ID, 56

VerdanturfMeadow_EventScript_Fezandipiti::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_FEZANDIPITI_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, VerdanturfMeadow_EventScript_FezandipitiDormant
	goto_if_eq VAR_RESULT, 1, VerdanturfMeadow_EventScript_FezandipitiNeedsPartner
	goto_if_eq VAR_RESULT, 4, VerdanturfMeadow_EventScript_FezandipitiCleanup
	msgbox VerdanturfMeadow_Text_FezandipitiAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_FEZANDIPITI, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_FEZANDIPITI_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, VerdanturfMeadow_EventScript_FezandipitiCaught
	msgbox VerdanturfMeadow_Text_FezandipitiRemains, MSGBOX_DEFAULT
	release
	end

VerdanturfMeadow_EventScript_FezandipitiDormant::
	msgbox VerdanturfMeadow_Text_FezandipitiDormant, MSGBOX_DEFAULT
	release
	end

VerdanturfMeadow_EventScript_FezandipitiNeedsPartner::
	msgbox VerdanturfMeadow_Text_FezandipitiNeedsPartner, MSGBOX_DEFAULT
	release
	end

VerdanturfMeadow_EventScript_FezandipitiCaught::
	setflag FLAG_EC_CAUGHT_FEZANDIPITI
VerdanturfMeadow_EventScript_FezandipitiCleanup::
	setflag FLAG_EC_CAUGHT_FEZANDIPITI
	removeobject LOCALID_EC_FEZANDIPITI
	release
	end

VerdanturfMeadow_Text_FezandipitiDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

VerdanturfMeadow_Text_FezandipitiNeedsPartner:
	.string "The SIGN sketches ALTARIA.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

VerdanturfMeadow_Text_FezandipitiAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "FEZANDIPITI answers the challenge!$"

VerdanturfMeadow_Text_FezandipitiRemains:
	.string "The SIGN remains bright. FEZANDIPITI\n"
	.string "will accept another challenge.$"
