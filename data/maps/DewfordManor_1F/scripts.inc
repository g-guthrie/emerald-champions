DewfordManor_1F_MapScripts::
	.byte 0

DewfordManor_1F_EventScript_Historian::
	msgbox DewfordManor_1F_Text_Historian, MSGBOX_NPC
	end
DewfordManor_1F_Text_Historian:
	.string "This manor housed Hoenn's first study\n"
	.string "of MEGA EVOLUTION.\p"
	.string "The researchers found that a stone is\n"
	.string "only a key. Trust supplies the power.\p"
	.string "Their final entry mentions a crystal\n"
	.string "chamber beneath SOOTOPOLIS.$"
.set EC_SIGN_MELOETTA_ID, 59

DewfordManor_1F_EventScript_Meloetta::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_MELOETTA_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, DewfordManor_1F_EventScript_MeloettaDormant
	goto_if_eq VAR_RESULT, 1, DewfordManor_1F_EventScript_MeloettaNeedsPartner
	goto_if_eq VAR_RESULT, 4, DewfordManor_1F_EventScript_MeloettaCleanup
	msgbox DewfordManor_1F_Text_MeloettaAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_MELOETTA, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_MELOETTA_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, DewfordManor_1F_EventScript_MeloettaCaught
	msgbox DewfordManor_1F_Text_MeloettaRemains, MSGBOX_DEFAULT
	release
	end

DewfordManor_1F_EventScript_MeloettaDormant::
	msgbox DewfordManor_1F_Text_MeloettaDormant, MSGBOX_DEFAULT
	release
	end

DewfordManor_1F_EventScript_MeloettaNeedsPartner::
	msgbox DewfordManor_1F_Text_MeloettaNeedsPartner, MSGBOX_DEFAULT
	release
	end

DewfordManor_1F_EventScript_MeloettaCaught::
	setflag FLAG_EC_CAUGHT_MELOETTA
DewfordManor_1F_EventScript_MeloettaCleanup::
	setflag FLAG_EC_CAUGHT_MELOETTA
	removeobject LOCALID_EC_MELOETTA
	release
	end

DewfordManor_1F_Text_MeloettaDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

DewfordManor_1F_Text_MeloettaNeedsPartner:
	.string "The SIGN sketches EXPLOUD.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

DewfordManor_1F_Text_MeloettaAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "MELOETTA answers the challenge!$"

DewfordManor_1F_Text_MeloettaRemains:
	.string "The SIGN remains bright. MELOETTA\n"
	.string "will accept another challenge.$"

.set EC_SIGN_MUNKIDORI_ID, 63

DewfordManor_1F_EventScript_Munkidori::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_MUNKIDORI_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, DewfordManor_1F_EventScript_MunkidoriDormant
	goto_if_eq VAR_RESULT, 1, DewfordManor_1F_EventScript_MunkidoriNeedsPartner
	goto_if_eq VAR_RESULT, 4, DewfordManor_1F_EventScript_MunkidoriCleanup
	msgbox DewfordManor_1F_Text_MunkidoriAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_MUNKIDORI, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_MUNKIDORI_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, DewfordManor_1F_EventScript_MunkidoriCaught
	msgbox DewfordManor_1F_Text_MunkidoriRemains, MSGBOX_DEFAULT
	release
	end

DewfordManor_1F_EventScript_MunkidoriDormant::
	msgbox DewfordManor_1F_Text_MunkidoriDormant, MSGBOX_DEFAULT
	release
	end

DewfordManor_1F_EventScript_MunkidoriNeedsPartner::
	msgbox DewfordManor_1F_Text_MunkidoriNeedsPartner, MSGBOX_DEFAULT
	release
	end

DewfordManor_1F_EventScript_MunkidoriCaught::
	setflag FLAG_EC_CAUGHT_MUNKIDORI
DewfordManor_1F_EventScript_MunkidoriCleanup::
	setflag FLAG_EC_CAUGHT_MUNKIDORI
	removeobject LOCALID_EC_MUNKIDORI
	release
	end

DewfordManor_1F_Text_MunkidoriDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

DewfordManor_1F_Text_MunkidoriNeedsPartner:
	.string "The SIGN sketches ORANGURU.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

DewfordManor_1F_Text_MunkidoriAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "MUNKIDORI answers the challenge!$"

DewfordManor_1F_Text_MunkidoriRemains:
	.string "The SIGN remains bright. MUNKIDORI\n"
	.string "will accept another challenge.$"
