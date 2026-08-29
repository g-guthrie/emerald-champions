Seaspray_Cave_B1F_MapScripts::
	.byte 0

.set EC_SIGN_PALKIA_ID, 25

Seaspray_Cave_B1F_EventScript_Palkia::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_PALKIA_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, Seaspray_Cave_B1F_EventScript_PalkiaDormant
	goto_if_eq VAR_RESULT, 1, Seaspray_Cave_B1F_EventScript_PalkiaNeedsPartner
	goto_if_eq VAR_RESULT, 4, Seaspray_Cave_B1F_EventScript_PalkiaCleanup
	msgbox Seaspray_Cave_B1F_Text_PalkiaAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_PALKIA, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_PALKIA_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, Seaspray_Cave_B1F_EventScript_PalkiaCaught
	msgbox Seaspray_Cave_B1F_Text_PalkiaRemains, MSGBOX_DEFAULT
	release
	end

Seaspray_Cave_B1F_EventScript_PalkiaDormant::
	msgbox Seaspray_Cave_B1F_Text_PalkiaDormant, MSGBOX_DEFAULT
	release
	end

Seaspray_Cave_B1F_EventScript_PalkiaNeedsPartner::
	msgbox Seaspray_Cave_B1F_Text_PalkiaNeedsPartner, MSGBOX_DEFAULT
	release
	end

Seaspray_Cave_B1F_EventScript_PalkiaCaught::
	setflag FLAG_EC_CAUGHT_PALKIA
Seaspray_Cave_B1F_EventScript_PalkiaCleanup::
	setflag FLAG_EC_CAUGHT_PALKIA
	removeobject LOCALID_EC_PALKIA
	release
	end

Seaspray_Cave_B1F_Text_PalkiaDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

Seaspray_Cave_B1F_Text_PalkiaNeedsPartner:
	.string "The SIGN sketches KINGDRA.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

Seaspray_Cave_B1F_Text_PalkiaAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "PALKIA answers the challenge!$"

Seaspray_Cave_B1F_Text_PalkiaRemains:
	.string "The SIGN remains bright. PALKIA\n"
	.string "will accept another challenge.$"
