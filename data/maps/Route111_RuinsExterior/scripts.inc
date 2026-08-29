Route111_RuinsExterior_MapScripts::
	.byte 0

Route111_RuinsExterior_EventScript_Archaeologist::
	msgbox Route111_RuinsExterior_Text_Archaeologist, MSGBOX_NPC
	end
Route111_RuinsExterior_Text_Archaeologist:
	.string "The desert was a proving ground for\n"
	.string "Hoenn's ancient Champions.\p"
	.string "Bring CASTFORM to the summit. The land\n"
	.string "itself may recognize a weather master.$"
.set EC_SIGN_LANDORUS_ID, 20

Route111_RuinsExterior_EventScript_Landorus::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_LANDORUS_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, Route111_RuinsExterior_EventScript_LandorusDormant
	goto_if_eq VAR_RESULT, 1, Route111_RuinsExterior_EventScript_LandorusNeedsPartner
	goto_if_eq VAR_RESULT, 4, Route111_RuinsExterior_EventScript_LandorusCleanup
	msgbox Route111_RuinsExterior_Text_LandorusAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_LANDORUS, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_LANDORUS_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, Route111_RuinsExterior_EventScript_LandorusCaught
	msgbox Route111_RuinsExterior_Text_LandorusRemains, MSGBOX_DEFAULT
	release
	end

Route111_RuinsExterior_EventScript_LandorusDormant::
	msgbox Route111_RuinsExterior_Text_LandorusDormant, MSGBOX_DEFAULT
	release
	end

Route111_RuinsExterior_EventScript_LandorusNeedsPartner::
	msgbox Route111_RuinsExterior_Text_LandorusNeedsPartner, MSGBOX_DEFAULT
	release
	end

Route111_RuinsExterior_EventScript_LandorusCaught::
	setflag FLAG_EC_CAUGHT_LANDORUS
Route111_RuinsExterior_EventScript_LandorusCleanup::
	setflag FLAG_EC_CAUGHT_LANDORUS
	removeobject LOCALID_EC_LANDORUS
	release
	end

Route111_RuinsExterior_Text_LandorusDormant:
	.string "A CHAMPION'S SIGN is carved here.\p"
	.string "Its light is dormant. Hoenn's story\n"
	.string "has not yet reached this place.$"

Route111_RuinsExterior_Text_LandorusNeedsPartner:
	.string "The SIGN sketches CASTFORM.\p"
	.string "Bring that Pokémon's family here, and\n"
	.string "the hidden challenger may answer.$"

Route111_RuinsExterior_Text_LandorusAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "LANDORUS answers the challenge!$"

Route111_RuinsExterior_Text_LandorusRemains:
	.string "The SIGN remains bright. LANDORUS\n"
	.string "will accept another challenge.$"
