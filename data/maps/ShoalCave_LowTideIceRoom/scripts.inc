ShoalCave_LowTideIceRoom_MapScripts::
	.byte 0

@ BEGIN EMERALD CHAMPIONS LEGENDARY ROOTS
.set EC_SIGN_ARTICUNO_ID, 53

ShoalCave_LowTideIceRoom_EventScript_Articuno::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_ARTICUNO_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 6, Common_EventScript_LegendaryNeedsDevon
	goto_if_eq VAR_RESULT, 5, Common_EventScript_LegendaryDefeatedAtShrine
	goto_if_eq VAR_RESULT, 0, ShoalCave_LowTideIceRoom_EventScript_ArticunoDormant
	goto_if_eq VAR_RESULT, 1, ShoalCave_LowTideIceRoom_EventScript_ArticunoNeedsPartner
	goto_if_eq VAR_RESULT, 4, ShoalCave_LowTideIceRoom_EventScript_ArticunoCleanup
	msgbox ShoalCave_LowTideIceRoom_Text_ArticunoAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_ARTICUNO, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_ARTICUNO_ID
	msgbox gText_LegendaryOneShotWarning, MSGBOX_DEFAULT
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_LegendaryDefeated
	goto ShoalCave_LowTideIceRoom_EventScript_ArticunoCaught
	end

ShoalCave_LowTideIceRoom_EventScript_ArticunoDormant::
	msgbox ShoalCave_LowTideIceRoom_Text_ArticunoDormant, MSGBOX_DEFAULT
	release
	end

ShoalCave_LowTideIceRoom_EventScript_ArticunoNeedsPartner::
	msgbox ShoalCave_LowTideIceRoom_Text_ArticunoNeedsPartner, MSGBOX_DEFAULT
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
	.string "A CHAMPION'S SIGN marks the ice altar.\p"
	.string "Its light is dormant. HOENN's story\n"
	.string "has not yet reached this place.$"

ShoalCave_LowTideIceRoom_Text_ArticunoNeedsPartner:
	.string "The SIGN sketches FROSLASS.\p"
	.string "Bring that POKéMON's family to the\n"
	.string "dream researcher at DEVON CORP. 2F\l"
	.string "in RUSTBORO to translate this Sign.$"

ShoalCave_LowTideIceRoom_Text_ArticunoAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "ARTICUNO answers the challenge!$"



@ END EMERALD CHAMPIONS LEGENDARY ROOTS
