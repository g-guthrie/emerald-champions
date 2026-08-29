PetalburgWoods_2_MapScripts::
	.byte 0

PetalburgWoods_2_EventScript_Ranger::
	msgbox PetalburgWoods_2_Text_Ranger, MSGBOX_NPC
	end
PetalburgWoods_2_Text_Ranger:
	.string "These paths returned when the first\n"
	.string "CHAMPION'S SIGN lit up.\p"
	.string "A swordsman of the forest appears only\n"
	.string "beside a fully grown BRELOOM.$"
.set EC_SIGN_VIRIZION_ID, 43

PetalburgWoods_2_EventScript_Virizion::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_VIRIZION_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, PetalburgWoods_2_EventScript_VirizionDormant
	goto_if_eq VAR_RESULT, 1, PetalburgWoods_2_EventScript_VirizionNeedsPartner
	goto_if_eq VAR_RESULT, 4, PetalburgWoods_2_EventScript_VirizionCleanup
	msgbox PetalburgWoods_2_Text_VirizionAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_VIRIZION, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_VIRIZION_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, PetalburgWoods_2_EventScript_VirizionCaught
	msgbox PetalburgWoods_2_Text_VirizionRemains, MSGBOX_DEFAULT
	release
	end

PetalburgWoods_2_EventScript_VirizionDormant::
	msgbox PetalburgWoods_2_Text_VirizionDormant, MSGBOX_DEFAULT
	release
	end

PetalburgWoods_2_EventScript_VirizionNeedsPartner::
	msgbox PetalburgWoods_2_Text_VirizionNeedsPartner, MSGBOX_DEFAULT
	release
	end

PetalburgWoods_2_EventScript_VirizionCaught::
	setflag FLAG_EC_CAUGHT_VIRIZION
PetalburgWoods_2_EventScript_VirizionCleanup::
	setflag FLAG_EC_CAUGHT_VIRIZION
	removeobject LOCALID_EC_VIRIZION
	release
	end

PetalburgWoods_2_Text_VirizionDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

PetalburgWoods_2_Text_VirizionNeedsPartner:
	.string "The SIGN sketches BRELOOM.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

PetalburgWoods_2_Text_VirizionAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "VIRIZION answers the challenge!$"

PetalburgWoods_2_Text_VirizionRemains:
	.string "The SIGN remains bright. VIRIZION\n"
	.string "will accept another challenge.$"

.set EC_SIGN_WO_CHIEN_ID, 68

PetalburgWoods_2_EventScript_WoChien::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_WO_CHIEN_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, PetalburgWoods_2_EventScript_WoChienDormant
	goto_if_eq VAR_RESULT, 1, PetalburgWoods_2_EventScript_WoChienNeedsPartner
	goto_if_eq VAR_RESULT, 4, PetalburgWoods_2_EventScript_WoChienCleanup
	msgbox PetalburgWoods_2_Text_WoChienAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_WO_CHIEN, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_WO_CHIEN_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, PetalburgWoods_2_EventScript_WoChienCaught
	msgbox PetalburgWoods_2_Text_WoChienRemains, MSGBOX_DEFAULT
	release
	end

PetalburgWoods_2_EventScript_WoChienDormant::
	msgbox PetalburgWoods_2_Text_WoChienDormant, MSGBOX_DEFAULT
	release
	end

PetalburgWoods_2_EventScript_WoChienNeedsPartner::
	msgbox PetalburgWoods_2_Text_WoChienNeedsPartner, MSGBOX_DEFAULT
	release
	end

PetalburgWoods_2_EventScript_WoChienCaught::
	setflag FLAG_EC_CAUGHT_WO_CHIEN
PetalburgWoods_2_EventScript_WoChienCleanup::
	setflag FLAG_EC_CAUGHT_WO_CHIEN
	removeobject LOCALID_EC_WO_CHIEN
	release
	end

PetalburgWoods_2_Text_WoChienDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

PetalburgWoods_2_Text_WoChienNeedsPartner:
	.string "The SIGN sketches TREVENANT.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

PetalburgWoods_2_Text_WoChienAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "WO CHIEN answers the challenge!$"

PetalburgWoods_2_Text_WoChienRemains:
	.string "The SIGN remains bright. WO CHIEN\n"
	.string "will accept another challenge.$"
