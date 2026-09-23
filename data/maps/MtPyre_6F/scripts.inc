MtPyre_6F_MapScripts:: @ 8231D3A
	.byte 0

MtPyre_6F_EventScript_Valerie:: @ 8231D3B
	msgbox MtPyre_6F_Text_ValeriePostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_6F_Text_ValeriePostBattle: @ 8231DFC
	.string "Perhaps the power is from the spirits\n"
	.string "of Pokémon in fitful sleep here…$"

MtPyre_6F_EventScript_Pecharunt::
	lockall
	setvar VAR_0x8004, LEGENDARY_SIGN_PECHARUNT
	special GetSelectedLegendarySignState
	goto_if_eq VAR_RESULT, 3, Common_EventScript_LegendaryRestingAtShrine
	goto_if_eq VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntQuiet
	setvar VAR_0x8004, LEGENDARY_SIGN_OKIDOGI
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, LEGENDARY_SIGN_MUNKIDORI
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, LEGENDARY_SIGN_FEZANDIPITI
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, LEGENDARY_SIGN_PECHARUNT
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 5, Common_EventScript_LegendaryRestingAtShrine
	goto_if_eq VAR_RESULT, 0, MtPyre_6F_EventScript_PecharuntDormant
	goto_if_eq VAR_RESULT, 1, MtPyre_6F_EventScript_PecharuntDormant
	special CreateSelectedLegendarySignEncounter
	goto_if_eq VAR_RESULT, FALSE, MtPyre_6F_EventScript_PecharuntDormant
	msgbox MtPyre_6F_Text_PecharuntAwakens, MSGBOX_DEFAULT
	playmoncry SPECIES_PECHARUNT, CRY_MODE_ENCOUNTER
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_LegendaryRestingAtShrine
	releaseall
	end

MtPyre_6F_EventScript_PecharuntDormant::
	msgbox MtPyre_6F_Text_PecharuntDormant, MSGBOX_DEFAULT
	releaseall
	end

MtPyre_6F_EventScript_PecharuntQuiet::
	msgbox MtPyre_6F_Text_PecharuntQuiet, MSGBOX_DEFAULT
	releaseall
	end

MtPyre_6F_Text_PecharuntDormant:
	.string "Three masks adorn the tombstone.\p"
	.string "Earn the Mind Badge and catch\n"
	.string "Okidogi, Munkidori and Fezandipiti\l"
	.string "to awaken Pecharunt here.$"

MtPyre_6F_Text_PecharuntAwakens:
	.string "The three masks flare!\n"
	.string "Pecharunt emerges from the mist!$"

MtPyre_6F_Text_PecharuntQuiet:
	.string "The tombstone is still.\n"
	.string "The three masks have faded.$"
