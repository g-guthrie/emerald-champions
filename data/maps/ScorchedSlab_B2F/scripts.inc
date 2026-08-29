ScorchedSlab_B2F_MapScripts::
	.byte 0

ScorchedSlab_B2F_EventScript_Warden::
	msgbox ScorchedSlab_B2F_Text_Warden, MSGBOX_NPC
	end
ScorchedSlab_B2F_Text_Warden:
	.string "The slab is a lock between two fires.\p"
	.string "HEATRAN guards the earth below. A white\n"
	.string "dragon answers only after time itself.$"
.set EC_SIGN_RESHIRAM_ID, 32

ScorchedSlab_B2F_EventScript_Reshiram::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_RESHIRAM_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, ScorchedSlab_B2F_EventScript_ReshiramDormant
	goto_if_eq VAR_RESULT, 1, ScorchedSlab_B2F_EventScript_ReshiramNeedsPartner
	goto_if_eq VAR_RESULT, 4, ScorchedSlab_B2F_EventScript_ReshiramCleanup
	msgbox ScorchedSlab_B2F_Text_ReshiramAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_RESHIRAM, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_RESHIRAM_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, ScorchedSlab_B2F_EventScript_ReshiramCaught
	msgbox ScorchedSlab_B2F_Text_ReshiramRemains, MSGBOX_DEFAULT
	release
	end

ScorchedSlab_B2F_EventScript_ReshiramDormant::
	msgbox ScorchedSlab_B2F_Text_ReshiramDormant, MSGBOX_DEFAULT
	release
	end

ScorchedSlab_B2F_EventScript_ReshiramNeedsPartner::
	msgbox ScorchedSlab_B2F_Text_ReshiramNeedsPartner, MSGBOX_DEFAULT
	release
	end

ScorchedSlab_B2F_EventScript_ReshiramCaught::
	setflag FLAG_EC_CAUGHT_RESHIRAM
ScorchedSlab_B2F_EventScript_ReshiramCleanup::
	setflag FLAG_EC_CAUGHT_RESHIRAM
	removeobject LOCALID_EC_RESHIRAM
	release
	end

ScorchedSlab_B2F_Text_ReshiramDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

ScorchedSlab_B2F_Text_ReshiramNeedsPartner:
	.string "The SIGN sketches DIALGA.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

ScorchedSlab_B2F_Text_ReshiramAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "RESHIRAM answers the challenge!$"

ScorchedSlab_B2F_Text_ReshiramRemains:
	.string "The SIGN remains bright. RESHIRAM\n"
	.string "will accept another challenge.$"
