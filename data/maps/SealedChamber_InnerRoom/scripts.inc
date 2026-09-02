SealedChamber_InnerRoom_MapScripts::
	.byte 0

SealedChamber_InnerRoom_EventScript_BrailleBackWall::
	lockall
	braillemsgbox SealedChamber_InnerRoom_Braille_FirstWailordLastRelicanth
	goto_if_set FLAG_REGI_DOORS_OPENED, SealedChamber_InnerRoom_EventScript_NoEffect
	specialvar VAR_RESULT, CheckRelicanthWailord
	goto_if_eq VAR_RESULT, FALSE, SealedChamber_InnerRoom_EventScript_NoEffect
	fadeoutbgm 0
	playse SE_TRUCK_MOVE
	special DoSealedChamberShakingEffect_Long
	delay 40
	special DoSealedChamberShakingEffect_Short
	playse SE_DOOR
	delay 40
	special DoSealedChamberShakingEffect_Short
	playse SE_DOOR
	delay 40
	special DoSealedChamberShakingEffect_Short
	playse SE_DOOR
	delay 40
	msgbox gText_DoorOpenedFarAway, MSGBOX_DEFAULT
	closemessage
	fadeinbgm 0
	setflag FLAG_REGI_DOORS_OPENED
	releaseall
	end

SealedChamber_InnerRoom_EventScript_NoEffect::
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart1::
	lockall
	braillemsgbox SealedChamber_InnerRoom_Braille_InThisCaveWeHaveLived
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart2::
	lockall
	braillemsgbox SealedChamber_InnerRoom_Braille_WeOweAllToThePokemon
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart3::
	lockall
	braillemsgbox SealedChamber_InnerRoom_Braille_ButWeSealedThePokemonAway
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart4::
	lockall
	braillemsgbox SealedChamber_InnerRoom_Braille_WeFearedIt
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart5::
	lockall
	braillemsgbox SealedChamber_InnerRoom_Braille_ThoseWithCourageHope
	releaseall
	end

SealedChamber_InnerRoom_EventScript_BrailleStoryPart6::
	lockall
	braillemsgbox SealedChamber_InnerRoom_Braille_OpenDoorEternalPokemonWaits
	releaseall
	end

@ BEGIN EMERALD CHAMPIONS LEGENDARY ROOTS
.set EC_SIGN_REGIGIGAS_ID, 66

SealedChamber_InnerRoom_EventScript_RegigigasEC::
	lock
	msgbox SealedChamber_InnerRoom_Text_RegigigasStatue, MSGBOX_DEFAULT
	closemessage
	braillemsgbox SealedChamber_InnerRoom_Braille_Regigigas
	setvar VAR_0x8004, SPECIES_REGICE
	special DoesPlayerPartyHaveSelectedSpeciesFamily
	goto_if_eq VAR_RESULT, FALSE, SealedChamber_InnerRoom_EventScript_RegigigasStatue
	setvar VAR_0x8004, SPECIES_REGIROCK
	special DoesPlayerPartyHaveSelectedSpeciesFamily
	goto_if_eq VAR_RESULT, FALSE, SealedChamber_InnerRoom_EventScript_RegigigasStatue
	setvar VAR_0x8004, SPECIES_REGISTEEL
	special DoesPlayerPartyHaveSelectedSpeciesFamily
	goto_if_eq VAR_RESULT, FALSE, SealedChamber_InnerRoom_EventScript_RegigigasStatue
	waitse
	playmoncry SPECIES_REGIGIGAS, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_REGIGIGAS_ID
	special TryUnlockSelectedLegendarySign
	msgbox SealedChamber_InnerRoom_Text_RegigigasECAwakens, MSGBOX_DEFAULT
	setvar VAR_0x8004, EC_SIGN_REGIGIGAS_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, SealedChamber_InnerRoom_EventScript_RegigigasECCaught
	msgbox SealedChamber_InnerRoom_Text_RegigigasECRemains, MSGBOX_DEFAULT
	release
	end

SealedChamber_InnerRoom_EventScript_RegigigasStatue::
	release
	end

SealedChamber_InnerRoom_EventScript_RegigigasECCaught::
	setflag FLAG_EC_CAUGHT_REGIGIGAS
	removeobject LOCALID_EC_REGIGIGAS
	release
	end

SealedChamber_InnerRoom_Text_RegigigasStatue:
	.string "It's a statue of a Pokémon.\n"
	.string "It exudes tremendous power…\p"
	.string "There's something engraved on it…$"

SealedChamber_InnerRoom_Text_RegigigasECAwakens:
	.string "The three REGIS answer together!\n"
	.string "The colossal statue begins to move!$"

SealedChamber_InnerRoom_Text_RegigigasECRemains:
	.string "REGIGIGAS returns to its stone stance.\n"
	.string "The three seals remain open.$"


@ END EMERALD CHAMPIONS LEGENDARY ROOTS
