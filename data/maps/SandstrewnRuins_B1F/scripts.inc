SandstrewnRuins_B1F_MapScripts::
	.byte 0

.set EC_SIGN_ZYGARDE_ID, 52

SandstrewnRuins_B1F_EventScript_Zygarde::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_ZYGARDE_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, SandstrewnRuins_B1F_EventScript_ZygardeDormant
	goto_if_eq VAR_RESULT, 1, SandstrewnRuins_B1F_EventScript_ZygardeNeedsPartner
	goto_if_eq VAR_RESULT, 4, SandstrewnRuins_B1F_EventScript_ZygardeCleanup
	msgbox SandstrewnRuins_B1F_Text_ZygardeAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_ZYGARDE, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_ZYGARDE_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, SandstrewnRuins_B1F_EventScript_ZygardeCaught
	msgbox SandstrewnRuins_B1F_Text_ZygardeRemains, MSGBOX_DEFAULT
	release
	end

SandstrewnRuins_B1F_EventScript_ZygardeDormant::
	msgbox SandstrewnRuins_B1F_Text_ZygardeDormant, MSGBOX_DEFAULT
	release
	end

SandstrewnRuins_B1F_EventScript_ZygardeNeedsPartner::
	msgbox SandstrewnRuins_B1F_Text_ZygardeNeedsPartner, MSGBOX_DEFAULT
	release
	end

SandstrewnRuins_B1F_EventScript_ZygardeCaught::
	setflag FLAG_EC_CAUGHT_ZYGARDE
SandstrewnRuins_B1F_EventScript_ZygardeCleanup::
	setflag FLAG_EC_CAUGHT_ZYGARDE
	removeobject LOCALID_EC_ZYGARDE
	release
	end

SandstrewnRuins_B1F_Text_ZygardeDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

SandstrewnRuins_B1F_Text_ZygardeNeedsPartner:
	.string "The SIGN sketches LANDORUS.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

SandstrewnRuins_B1F_Text_ZygardeAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "ZYGARDE answers the challenge!$"

SandstrewnRuins_B1F_Text_ZygardeRemains:
	.string "The SIGN remains bright. ZYGARDE\n"
	.string "will accept another challenge.$"
