ShoalCave_LowTideIceRoom_MapScripts::
	.byte 0

@ BEGIN EMERALD CHAMPIONS LEGENDARY ROOTS
.set EC_SIGN_ARTICUNO_ID, 53

ShoalCave_LowTideIceRoom_EventScript_Articuno::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_ARTICUNO_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 5, Common_EventScript_LegendaryRestingAtShrine
	goto_if_eq VAR_RESULT, 0, ShoalCave_LowTideIceRoom_EventScript_ArticunoDormant
	goto_if_eq VAR_RESULT, 1, ShoalCave_LowTideIceRoom_EventScript_ArticunoNeedsPartner
	goto_if_eq VAR_RESULT, 4, ShoalCave_LowTideIceRoom_EventScript_ArticunoCleanup
	msgbox ShoalCave_LowTideIceRoom_Text_ArticunoAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_ARTICUNO, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_ARTICUNO_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_LegendaryResting
	goto ShoalCave_LowTideIceRoom_EventScript_ArticunoCaught
	end

ShoalCave_LowTideIceRoom_EventScript_ArticunoDormant::
	msgbox ShoalCave_LowTideIceRoom_Text_ArticunoDormant, MSGBOX_DEFAULT
	release
	end

ShoalCave_LowTideIceRoom_EventScript_ArticunoNeedsPartner::
	special ResearchSelectedLegendarySign
	msgbox gStringVar4, MSGBOX_DEFAULT
	release
	end

ShoalCave_LowTideIceRoom_EventScript_ArticunoCaught::
	setflag FLAG_EC_CAUGHT_ARTICUNO
ShoalCave_LowTideIceRoom_EventScript_ArticunoCleanup::
	setflag FLAG_EC_CAUGHT_ARTICUNO
	removeobject LOCALID_EC_ARTICUNO
	release
	end

ShoalCave_LowTideIceRoom_Text_ArticunoDormant:
	.string "ARTICUNO rests in the frozen\n"
	.string "chamber. Return with the FEATHER\l"
	.string "BADGE to challenge it.$"

ShoalCave_LowTideIceRoom_Text_ArticunoNeedsPartner:
	.string "ARTICUNO accepts challengers with\n"
	.string "the FEATHER BADGE. No partner\l"
	.string "species is required.$"

ShoalCave_LowTideIceRoom_Text_ArticunoAwakens:
	.string "ARTICUNO spreads its icy wings!\n"
	.string "It accepts your challenge!$"



@ END EMERALD CHAMPIONS LEGENDARY ROOTS
