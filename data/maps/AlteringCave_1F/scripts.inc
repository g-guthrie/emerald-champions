AlteringCave_1F_MapScripts::
	.byte 0

AlteringCave_1F_EventScript_Researcher::
	msgbox AlteringCave_1F_Text_Researcher, MSGBOX_NPC
	end

AlteringCave_1F_Text_Researcher:
	.string "DEVON called this cave an error in\n"
	.string "nature. I think it is a doorway.\p"
	.string "The same CHAMPION'S SIGN appears in\n"
	.string "every shape this cavern assumes.$"
.set EC_SIGN_HOOPA_ID, 17

AlteringCave_1F_EventScript_Hoopa::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_HOOPA_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, AlteringCave_1F_EventScript_HoopaDormant
	goto_if_eq VAR_RESULT, 1, AlteringCave_1F_EventScript_HoopaNeedsPartner
	goto_if_eq VAR_RESULT, 4, AlteringCave_1F_EventScript_HoopaCleanup
	msgbox AlteringCave_1F_Text_HoopaAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_HOOPA, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_HOOPA_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, AlteringCave_1F_EventScript_HoopaCaught
	msgbox AlteringCave_1F_Text_HoopaRemains, MSGBOX_DEFAULT
	release
	end

AlteringCave_1F_EventScript_HoopaDormant::
	msgbox AlteringCave_1F_Text_HoopaDormant, MSGBOX_DEFAULT
	release
	end

AlteringCave_1F_EventScript_HoopaNeedsPartner::
	msgbox AlteringCave_1F_Text_HoopaNeedsPartner, MSGBOX_DEFAULT
	release
	end

AlteringCave_1F_EventScript_HoopaCaught::
	setflag FLAG_EC_CAUGHT_HOOPA
AlteringCave_1F_EventScript_HoopaCleanup::
	setflag FLAG_EC_CAUGHT_HOOPA
	removeobject LOCALID_EC_HOOPA
	release
	end

AlteringCave_1F_Text_HoopaDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

AlteringCave_1F_Text_HoopaNeedsPartner:
	.string "The SIGN sketches UNOWN.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

AlteringCave_1F_Text_HoopaAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "HOOPA answers the challenge!$"

AlteringCave_1F_Text_HoopaRemains:
	.string "The SIGN remains bright. HOOPA\n"
	.string "will accept another challenge.$"
