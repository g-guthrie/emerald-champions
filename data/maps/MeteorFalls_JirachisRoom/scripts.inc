MeteorFalls_JirachisRoom_MapScripts::
	.byte 0

MeteorFalls_JirachisRoom_EventScript_Jirachi::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, SPECIES_JIRACHI
	setvar VAR_0x8005, 2
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MeteorFalls_JirachisRoom_EventScript_JirachiCaught
	msgbox MeteorFalls_JirachisRoom_Text_JirachiRemains, MSGBOX_DEFAULT
	release
	end

MeteorFalls_JirachisRoom_EventScript_JirachiCaught::
	setflag FLAG_EC_CAUGHT_JIRACHI
	removeobject LOCALID_EC_JIRACHI
	release
	end

MeteorFalls_JirachisRoom_Text_JirachiRemains:
	.string "JIRACHI accepts the challenge, but the\n"
	.string "sanctuary will hold its place for you.$"

.set EC_SIGN_COSMOG_ID, 54

MeteorFalls_JirachisRoom_EventScript_Cosmog::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_COSMOG_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, MeteorFalls_JirachisRoom_EventScript_CosmogDormant
	goto_if_eq VAR_RESULT, 1, MeteorFalls_JirachisRoom_EventScript_CosmogNeedsPartner
	goto_if_eq VAR_RESULT, 4, MeteorFalls_JirachisRoom_EventScript_CosmogCleanup
	msgbox MeteorFalls_JirachisRoom_Text_CosmogAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_COSMOG, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_COSMOG_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MeteorFalls_JirachisRoom_EventScript_CosmogCaught
	msgbox MeteorFalls_JirachisRoom_Text_CosmogRemains, MSGBOX_DEFAULT
	release
	end

MeteorFalls_JirachisRoom_EventScript_CosmogDormant::
	msgbox MeteorFalls_JirachisRoom_Text_CosmogDormant, MSGBOX_DEFAULT
	release
	end

MeteorFalls_JirachisRoom_EventScript_CosmogNeedsPartner::
	msgbox MeteorFalls_JirachisRoom_Text_CosmogNeedsPartner, MSGBOX_DEFAULT
	release
	end

MeteorFalls_JirachisRoom_EventScript_CosmogCaught::
	setflag FLAG_EC_CAUGHT_COSMOG
MeteorFalls_JirachisRoom_EventScript_CosmogCleanup::
	setflag FLAG_EC_CAUGHT_COSMOG
	removeobject LOCALID_EC_COSMOG
	release
	end

MeteorFalls_JirachisRoom_Text_CosmogDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

MeteorFalls_JirachisRoom_Text_CosmogNeedsPartner:
	.string "The SIGN sketches JIRACHI.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

MeteorFalls_JirachisRoom_Text_CosmogAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "COSMOG answers the challenge!$"

MeteorFalls_JirachisRoom_Text_CosmogRemains:
	.string "The SIGN remains bright. COSMOG\n"
	.string "will accept another challenge.$"
