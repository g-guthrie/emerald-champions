PetalburgWoods_3_MapScripts::
	.byte 0

PetalburgWoods_3_EventScript_Ranger::
	msgbox PetalburgWoods_3_Text_Ranger, MSGBOX_NPC
	end
PetalburgWoods_3_Text_Ranger:
	.string "Time folds strangely in this grove.\p"
	.string "The old stories pair CELEBI with a\n"
	.string "kind healer whose line ends in BLISSEY.$"
.set EC_SIGN_CELEBI_ID, 5

PetalburgWoods_3_EventScript_Celebi::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_CELEBI_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, PetalburgWoods_3_EventScript_CelebiDormant
	goto_if_eq VAR_RESULT, 1, PetalburgWoods_3_EventScript_CelebiNeedsPartner
	goto_if_eq VAR_RESULT, 4, PetalburgWoods_3_EventScript_CelebiCleanup
	msgbox PetalburgWoods_3_Text_CelebiAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_CELEBI, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_CELEBI_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, PetalburgWoods_3_EventScript_CelebiCaught
	msgbox PetalburgWoods_3_Text_CelebiRemains, MSGBOX_DEFAULT
	release
	end

PetalburgWoods_3_EventScript_CelebiDormant::
	msgbox PetalburgWoods_3_Text_CelebiDormant, MSGBOX_DEFAULT
	release
	end

PetalburgWoods_3_EventScript_CelebiNeedsPartner::
	msgbox PetalburgWoods_3_Text_CelebiNeedsPartner, MSGBOX_DEFAULT
	release
	end

PetalburgWoods_3_EventScript_CelebiCaught::
	setflag FLAG_EC_CAUGHT_CELEBI
PetalburgWoods_3_EventScript_CelebiCleanup::
	setflag FLAG_EC_CAUGHT_CELEBI
	removeobject LOCALID_EC_CELEBI
	release
	end

PetalburgWoods_3_Text_CelebiDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

PetalburgWoods_3_Text_CelebiNeedsPartner:
	.string "The SIGN sketches BLISSEY.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

PetalburgWoods_3_Text_CelebiAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "CELEBI answers the challenge!$"

PetalburgWoods_3_Text_CelebiRemains:
	.string "The SIGN remains bright. CELEBI\n"
	.string "will accept another challenge.$"
