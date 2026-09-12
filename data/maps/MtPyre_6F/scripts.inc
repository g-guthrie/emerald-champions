.set EC_SIGN_PECHARUNT_ID, 65

MtPyre_6F_MapScripts::
	.byte 0

MtPyre_6F_EventScript_Valerie::
	msgbox MtPyre_6F_Text_ValeriePostBattle, MSGBOX_NPC
	end

MtPyre_6F_EventScript_Cedric::
	msgbox MtPyre_6F_Text_CedricPostBattle, MSGBOX_NPC
	end

MtPyre_6F_Text_ValeriePostBattle:
	.string "Perhaps the power is from the spirits\n"
	.string "of POKéMON in fitful sleep here…$"

MtPyre_6F_Text_CedricPostBattle::
	.string "It is easy to lose your way here.\n"
	.string "Take your time on these stairs.$"

MtPyre_6F_EventScript_Pecharunt::
	lock
	setvar VAR_0x8004, EC_SIGN_PECHARUNT_ID
	special GetSelectedLegendarySignState
	goto_if_eq VAR_RESULT, 3, Common_EventScript_LegendaryRestingAtShrine
	goto_if_eq VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntQuiet
	setvar VAR_0x8004, 64
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, 63
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, 56
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, EC_SIGN_PECHARUNT_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 5, Common_EventScript_LegendaryRestingAtShrine
	goto_if_eq VAR_RESULT, 0, MtPyre_6F_EventScript_PecharuntDormant
	goto_if_eq VAR_RESULT, 1, MtPyre_6F_EventScript_PecharuntNeedsPartner
	msgbox MtPyre_6F_Text_PecharuntAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_PECHARUNT, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_PECHARUNT_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_LegendaryRestingAtShrine
	goto MtPyre_6F_EventScript_PecharuntCaught
	end

MtPyre_6F_EventScript_PecharuntDormant::
	msgbox MtPyre_6F_Text_PecharuntDormant, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_PecharuntNeedsPartner::
	msgbox MtPyre_6F_Text_PecharuntNeedsPartner, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_PecharuntQuiet::
	msgbox MtPyre_6F_Text_PecharuntQuiet, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_PecharuntCaught::
	setflag FLAG_EC_CAUGHT_PECHARUNT
	release
	end

MtPyre_6F_Text_PecharuntDormant:
	.string "PECHARUNT waits here. Earn the\n"
	.string "MIND BADGE and catch OKIDOGI,\l"
	.string "MUNKIDORI and FEZANDIPITI to\l"
	.string "challenge it.$"

MtPyre_6F_Text_PecharuntNeedsPartner:
	.string "The three masks belong to OKIDOGI,\n"
	.string "MUNKIDORI and FEZANDIPITI.\p"
	.string "Catch all three, then return here.\n"
	.string "They do not need to travel with you.$"

MtPyre_6F_Text_PecharuntAwakens:
	.string "The three masks flare! A poisoned mist\n"
	.string "spills from the tombstone--PECHARUNT!$"

MtPyre_6F_Text_PecharuntQuiet:
	.string "The tombstone is still. Three faded\n"
	.string "masks record the trial you completed.$"



@ END EMERALD CHAMPIONS LEGENDARY ROOTS
