NewMauville_Inside_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, NewMauville_Inside_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, NewMauville_Inside_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, NewMauville_Inside_OnLoad
	.byte 0

NewMauville_Inside_OnResume:
	call_if_eq VAR_TEMP_1, 1, NewMauville_Inside_EventScript_SetBarrierStateBlueButton
	call_if_eq VAR_TEMP_2, 1, NewMauville_Inside_EventScript_SetBarrierStateGreenButton
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, NewMauville_Inside_EventScript_TryRemoveVoltorb
	end

NewMauville_Inside_EventScript_TryRemoveVoltorb::
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
	removeobject VAR_LAST_TALKED
	return

NewMauville_Inside_OnTransition:
	setvar VAR_TEMP_1, 0
	setvar VAR_TEMP_2, 0
	call_if_unset FLAG_DEFEATED_VOLTORB_1_NEW_MAUVILLE, NewMauville_Inside_EventScript_ShowVoltorb1
	call_if_unset FLAG_DEFEATED_VOLTORB_2_NEW_MAUVILLE, NewMauville_Inside_EventScript_ShowVoltorb2
	call_if_unset FLAG_DEFEATED_VOLTORB_3_NEW_MAUVILLE, NewMauville_Inside_EventScript_ShowVoltorb3
	end

NewMauville_Inside_EventScript_ShowVoltorb1::
	clearflag FLAG_HIDE_NEW_MAUVILLE_VOLTORB_1
	return

NewMauville_Inside_EventScript_ShowVoltorb2::
	clearflag FLAG_HIDE_NEW_MAUVILLE_VOLTORB_2
	return

NewMauville_Inside_EventScript_ShowVoltorb3::
	clearflag FLAG_HIDE_NEW_MAUVILLE_VOLTORB_3
	return

NewMauville_Inside_OnLoad:
	call_if_eq VAR_NEW_MAUVILLE_STATE, 2, NewMauville_Inside_EventScript_SetGeneratorOffMetatiles
	call_if_eq VAR_NEW_MAUVILLE_STATE, 4, NewMauville_Inside_EventScript_SetGeneratorOffMetatiles
	call_if_eq VAR_NEW_MAUVILLE_STATE, 6, NewMauville_Inside_EventScript_SetGeneratorOffMetatiles
	end

NewMauville_Inside_EventScript_BlueButton::
	lockall
	setvar VAR_TEMP_1, 1
	setvar VAR_TEMP_2, 0
	playse SE_PIN
	call NewMauville_Inside_EventScript_SetBarrierStateBlueButton
	special DrawWholeMapView
	releaseall
	end

NewMauville_Inside_EventScript_GreenButton::
	lockall
	setvar VAR_TEMP_1, 0
	setvar VAR_TEMP_2, 1
	playse SE_PIN
	call NewMauville_Inside_EventScript_SetBarrierStateGreenButton
	special DrawWholeMapView
	releaseall
	end

NewMauville_Inside_EventScript_SetBarrierStateBlueButton::
	setmetatile 23, 34, METATILE_BikeShop_Barrier_Hidden_Top, TRUE
	setmetatile 23, 35, METATILE_BikeShop_Barrier_Hidden_Bottom, TRUE
	setmetatile 23, 36, METATILE_BikeShop_Floor_Shadow_Top, FALSE
	setmetatile 23, 37, METATILE_BikeShop_Wall_Edge_Top, FALSE
	setmetatile 10, 16, METATILE_BikeShop_Barrier_Hidden_Top, TRUE
	setmetatile 10, 17, METATILE_BikeShop_Barrier_Hidden_Bottom, TRUE
	setmetatile 10, 18, METATILE_BikeShop_Floor_Shadow_Top, FALSE
	setmetatile 10, 19, METATILE_BikeShop_Wall_Edge_Top, FALSE
	setmetatile 10, 0, METATILE_BikeShop_Barrier_Hidden_Top, TRUE
	setmetatile 10, 1, METATILE_BikeShop_Barrier_Hidden_Bottom, TRUE
	setmetatile 10, 2, METATILE_BikeShop_Floor_Shadow_Top, FALSE
	setmetatile 10, 3, METATILE_BikeShop_Wall_Edge_Top, FALSE
	setmetatile 37, 33, METATILE_BikeShop_Barrier_Green_Top, TRUE
	setmetatile 37, 34, METATILE_BikeShop_Barrier_Green_TopMid, TRUE
	setmetatile 37, 35, METATILE_BikeShop_Barrier_Green_BottomMid, TRUE
	setmetatile 37, 36, METATILE_BikeShop_Barrier_Green_Bottom, TRUE
	setmetatile 28, 22, METATILE_BikeShop_Barrier_Green_Top, TRUE
	setmetatile 28, 23, METATILE_BikeShop_Barrier_Green_TopMid, TRUE
	setmetatile 28, 24, METATILE_BikeShop_Barrier_Green_BottomMid, TRUE
	setmetatile 28, 25, METATILE_BikeShop_Barrier_Green_Bottom, TRUE
	setmetatile 10, 24, METATILE_BikeShop_Barrier_Green_Top, TRUE
	setmetatile 10, 25, METATILE_BikeShop_Barrier_Green_TopMid, TRUE
	setmetatile 10, 26, METATILE_BikeShop_Barrier_Green_BottomMid, TRUE
	setmetatile 10, 27, METATILE_BikeShop_Barrier_Green_Bottom, TRUE
	setmetatile 21, 2, METATILE_BikeShop_Barrier_Green_Top, TRUE
	setmetatile 21, 3, METATILE_BikeShop_Barrier_Green_TopMid, TRUE
	setmetatile 21, 4, METATILE_BikeShop_Barrier_Green_BottomMid, TRUE
	setmetatile 21, 5, METATILE_BikeShop_Barrier_Green_Bottom, TRUE
	setmetatile 6, 11, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 13, 10, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 16, 22, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 4, 26, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 30, 38, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 2, 11, METATILE_BikeShop_Button_Green, FALSE
	setmetatile 17, 10, METATILE_BikeShop_Button_Green, FALSE
	setmetatile 25, 18, METATILE_BikeShop_Button_Green, FALSE
	setmetatile 18, 36, METATILE_BikeShop_Button_Green, FALSE
	return

NewMauville_Inside_EventScript_SetBarrierStateGreenButton::
	setmetatile 23, 34, METATILE_BikeShop_Barrier_Blue_Top, TRUE
	setmetatile 23, 35, METATILE_BikeShop_Barrier_Blue_TopMid, TRUE
	setmetatile 23, 36, METATILE_BikeShop_Barrier_Blue_BottomMid, TRUE
	setmetatile 23, 37, METATILE_BikeShop_Barrier_Blue_Bottom, TRUE
	setmetatile 10, 16, METATILE_BikeShop_Barrier_Blue_Top, TRUE
	setmetatile 10, 17, METATILE_BikeShop_Barrier_Blue_TopMid, TRUE
	setmetatile 10, 18, METATILE_BikeShop_Barrier_Blue_BottomMid, TRUE
	setmetatile 10, 19, METATILE_BikeShop_Barrier_Blue_Bottom, TRUE
	setmetatile 10, 0, METATILE_BikeShop_Barrier_Blue_Top, TRUE
	setmetatile 10, 1, METATILE_BikeShop_Barrier_Blue_TopMid, TRUE
	setmetatile 10, 2, METATILE_BikeShop_Barrier_Blue_BottomMid, TRUE
	setmetatile 10, 3, METATILE_BikeShop_Barrier_Blue_Bottom, TRUE
	setmetatile 37, 33, METATILE_BikeShop_Barrier_Hidden_Top, TRUE
	setmetatile 37, 34, METATILE_BikeShop_Barrier_Hidden_Bottom, TRUE
	setmetatile 37, 35, METATILE_BikeShop_Floor_Shadow_Top, FALSE
	setmetatile 37, 36, METATILE_BikeShop_Wall_Edge_Top, FALSE
	setmetatile 28, 22, METATILE_BikeShop_Barrier_Hidden_Top, TRUE
	setmetatile 28, 23, METATILE_BikeShop_Barrier_Hidden_Bottom, TRUE
	setmetatile 28, 24, METATILE_BikeShop_Floor_Shadow_Top, FALSE
	setmetatile 28, 25, METATILE_BikeShop_Wall_Edge_Top, FALSE
	setmetatile 10, 24, METATILE_BikeShop_Barrier_Hidden_Top, TRUE
	setmetatile 10, 25, METATILE_BikeShop_Barrier_Hidden_Bottom, TRUE
	setmetatile 10, 26, METATILE_BikeShop_Floor_Shadow_Top, FALSE
	setmetatile 10, 27, METATILE_BikeShop_Wall_Edge_Top, FALSE
	setmetatile 21, 2, METATILE_BikeShop_Barrier_Hidden_Top, TRUE
	setmetatile 21, 3, METATILE_BikeShop_Barrier_Hidden_Bottom, TRUE
	setmetatile 21, 4, METATILE_BikeShop_Floor_Shadow_Top, FALSE
	setmetatile 21, 5, METATILE_BikeShop_Wall_Edge_Top, FALSE
	setmetatile 2, 11, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 17, 10, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 25, 18, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 18, 36, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 6, 11, METATILE_BikeShop_Button_Blue, FALSE
	setmetatile 13, 10, METATILE_BikeShop_Button_Blue, FALSE
	setmetatile 16, 22, METATILE_BikeShop_Button_Blue, FALSE
	setmetatile 4, 26, METATILE_BikeShop_Button_Blue, FALSE
	setmetatile 30, 38, METATILE_BikeShop_Button_Blue, FALSE
	return

NewMauville_Inside_EventScript_RedButton::
	lockall
	msgbox NewMauville_Inside_Text_SteppedOnSwitchGeneratorStopped, MSGBOX_DEFAULT
	call NewMauville_Inside_EventScript_SetGeneratorOffMetatiles
	setvar VAR_NEW_MAUVILLE_STATE, 2
	releaseall
	end

NewMauville_Inside_EventScript_RotomStirs::
	lockall
	playmoncry SPECIES_ROTOM, CRY_MODE_ENCOUNTER
	waitmoncry
	msgbox NewMauville_Inside_Text_GeneratorRestarted, MSGBOX_DEFAULT
	call NewMauville_Inside_EventScript_SetGeneratorOnMetatiles
	setvar VAR_NEW_MAUVILLE_STATE, 3
	releaseall
	end

NewMauville_Inside_EventScript_RedButtonAgain::
	lockall
	msgbox NewMauville_Inside_Text_SteppedOnSwitchGeneratorStopped, MSGBOX_DEFAULT
	call NewMauville_Inside_EventScript_SetGeneratorOffMetatiles
	setvar VAR_NEW_MAUVILLE_STATE, 4
	releaseall
	end

NewMauville_Inside_EventScript_RotomStirsAgain::
	lockall
	playmoncry SPECIES_ROTOM, CRY_MODE_ENCOUNTER
	waitmoncry
	msgbox NewMauville_Inside_Text_GeneratorRestartedAgain, MSGBOX_DEFAULT
	call NewMauville_Inside_EventScript_SetGeneratorOnMetatiles
	setvar VAR_NEW_MAUVILLE_STATE, 5
	releaseall
	end

NewMauville_Inside_EventScript_RotomEncounter::
	lockall
	setmetatile 33, 6, METATILE_BikeShop_Button_Pressed, FALSE
	special DrawWholeMapView
	playmoncry SPECIES_ROTOM, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_Delay48
	waitmovement 0
	msgbox NewMauville_Inside_Text_RotomAppears, MSGBOX_DEFAULT
	setvar VAR_0x8004, SPECIES_ROTOM
	setvar VAR_0x8005, 0
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, NewMauville_Inside_EventScript_RotomCaught
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, NewMauville_Inside_EventScript_RotomResolved
	setvar VAR_NEW_MAUVILLE_STATE, 5
	goto_if_unset FLAG_EC_RESOLVED_ROTOM, NewMauville_Inside_EventScript_RotomStillWaiting
	setvar VAR_NEW_MAUVILLE_STATE, 6
	call NewMauville_Inside_EventScript_SetGeneratorOffMetatiles
NewMauville_Inside_EventScript_RotomStillWaiting::
	msgbox NewMauville_Inside_Text_RotomRemains, MSGBOX_DEFAULT
	releaseall
	end

NewMauville_Inside_EventScript_RotomCaught::
	setflag FLAG_EC_CAUGHT_ROTOM
NewMauville_Inside_EventScript_RotomResolved::
	setflag FLAG_EC_RESOLVED_ROTOM
	setvar VAR_NEW_MAUVILLE_STATE, 6
	call NewMauville_Inside_EventScript_SetGeneratorOffMetatiles
	msgbox NewMauville_Inside_Text_RotomResolved, MSGBOX_DEFAULT
	releaseall
	end

NewMauville_Inside_EventScript_SetGeneratorOffMetatiles::
	setmetatile 33, 6, METATILE_BikeShop_Button_Pressed, FALSE
	setmetatile 32, 2, METATILE_BikeShop_Generator_Off_Tile0, TRUE
	setmetatile 33, 2, METATILE_BikeShop_Generator_Off_Tile1, TRUE
	setmetatile 34, 2, METATILE_BikeShop_Generator_Off_Tile2, TRUE
	setmetatile 35, 2, METATILE_BikeShop_Generator_Off_Tile3, TRUE
	setmetatile 32, 3, METATILE_BikeShop_Generator_Off_Tile4, TRUE
	setmetatile 33, 3, METATILE_BikeShop_Generator_Off_Tile5, TRUE
	setmetatile 34, 3, METATILE_BikeShop_Generator_Off_Tile6, TRUE
	setmetatile 35, 3, METATILE_BikeShop_Generator_Off_Tile7, TRUE
	special DrawWholeMapView
	return

NewMauville_Inside_EventScript_SetGeneratorOnMetatiles::
	setmetatile 33, 6, METATILE_BikeShop_Button_Red, FALSE
	setmetatile 32, 2, METATILE_BikeShop_Generator_On_Tile0, TRUE
	setmetatile 33, 2, METATILE_BikeShop_Generator_On_Tile1, TRUE
	setmetatile 34, 2, METATILE_BikeShop_Generator_On_Tile2, TRUE
	setmetatile 35, 2, METATILE_BikeShop_Generator_On_Tile3, TRUE
	setmetatile 32, 3, METATILE_BikeShop_Generator_On_Tile4, TRUE
	setmetatile 33, 3, METATILE_BikeShop_Generator_On_Tile5, TRUE
	setmetatile 34, 3, METATILE_BikeShop_Generator_On_Tile6, TRUE
	setmetatile 35, 3, METATILE_BikeShop_Generator_On_Tile7, TRUE
	special DrawWholeMapView
	return

NewMauville_Inside_EventScript_Generator::
	lockall
	goto_if_set FLAG_EC_RESOLVED_ROTOM, NewMauville_Inside_EventScript_RotomReturn
	goto_if_eq VAR_NEW_MAUVILLE_STATE, 2, NewMauville_Inside_EventScript_GeneratorOff
	goto_if_eq VAR_NEW_MAUVILLE_STATE, 4, NewMauville_Inside_EventScript_GeneratorOff
	goto_if_eq VAR_NEW_MAUVILLE_STATE, 6, NewMauville_Inside_EventScript_GeneratorOff
	msgbox NewMauville_Inside_Text_GeneratorRadiatingHeat, MSGBOX_DEFAULT
	releaseall
	end

NewMauville_Inside_EventScript_GeneratorOff::
	msgbox NewMauville_Inside_Text_GeneratorQuietedDown, MSGBOX_DEFAULT
	releaseall
	end

NewMauville_Inside_EventScript_GeneratorIntake::
	lockall
	goto_if_lt VAR_NEW_MAUVILLE_STATE, 2, NewMauville_Inside_EventScript_GeneratorIntakeOn
	goto_if_set FLAG_EC_MEGA_GIFT_EELEKTROSSITE, NewMauville_Inside_EventScript_GeneratorIntakeOff
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, NewMauville_Inside_EventScript_GeneratorIntakeOff
	msgbox NewMauville_Inside_Text_StoneJammedInIntake, MSGBOX_DEFAULT
	giveuniqueitem ITEM_EELEKTROSSITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_EELEKTROSSITE
	releaseall
	end

NewMauville_Inside_EventScript_GeneratorIntakeOn::
	msgbox NewMauville_Inside_Text_GeneratorRadiatingHeat, MSGBOX_DEFAULT
	releaseall
	end

NewMauville_Inside_EventScript_GeneratorIntakeOff::
	msgbox NewMauville_Inside_Text_GeneratorQuietedDown, MSGBOX_DEFAULT
	releaseall
	end

NewMauville_Inside_EventScript_Voltorb1::
	lock
	faceplayer
	setwildbattle SPECIES_VOLTORB, 25
	waitse
	playmoncry SPECIES_VOLTORB, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	dowildbattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, NewMauville_Inside_EventScript_DefeatedVoltorb1
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, NewMauville_Inside_EventScript_DefeatedVoltorb1
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, NewMauville_Inside_EventScript_DefeatedVoltorb1
	setflag FLAG_DEFEATED_VOLTORB_1_NEW_MAUVILLE
	release
	end

NewMauville_Inside_EventScript_DefeatedVoltorb1::
	setflag FLAG_DEFEATED_VOLTORB_1_NEW_MAUVILLE
	goto Common_EventScript_RemoveStaticPokemon
	end

NewMauville_Inside_EventScript_Voltorb2::
	lock
	faceplayer
	setwildbattle SPECIES_VOLTORB, 25
	waitse
	playmoncry SPECIES_VOLTORB, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	dowildbattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, NewMauville_Inside_EventScript_DefeatedVoltorb2
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, NewMauville_Inside_EventScript_DefeatedVoltorb2
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, NewMauville_Inside_EventScript_DefeatedVoltorb2
	setflag FLAG_DEFEATED_VOLTORB_2_NEW_MAUVILLE
	release
	end

NewMauville_Inside_EventScript_DefeatedVoltorb2::
	setflag FLAG_DEFEATED_VOLTORB_2_NEW_MAUVILLE
	goto Common_EventScript_RemoveStaticPokemon
	end

NewMauville_Inside_EventScript_Voltorb3::
	lock
	faceplayer
	setwildbattle SPECIES_VOLTORB, 25
	waitse
	playmoncry SPECIES_VOLTORB, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	dowildbattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, NewMauville_Inside_EventScript_DefeatedVoltorb3
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, NewMauville_Inside_EventScript_DefeatedVoltorb3
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, NewMauville_Inside_EventScript_DefeatedVoltorb3
	setflag FLAG_DEFEATED_VOLTORB_3_NEW_MAUVILLE
	release
	end

NewMauville_Inside_EventScript_DefeatedVoltorb3::
	setflag FLAG_DEFEATED_VOLTORB_3_NEW_MAUVILLE
	goto Common_EventScript_RemoveStaticPokemon
	end

NewMauville_Inside_Text_GeneratorRadiatingHeat:
	.string "The generator is radiating heat that\n"
	.string "can be felt even at a distance.\p"
	.string "It looks like it should be turned off\n"
	.string "as soon as possible.$"

NewMauville_Inside_Text_GeneratorQuietedDown:
	.string "The generator quieted down.$"

NewMauville_Inside_Text_StoneJammedInIntake:
	.string "Something is jammed in the intake.\n"
	.string "It's a stone shaped like an eel's\l"
	.string "coil.$"

NewMauville_Inside_Text_SteppedOnSwitchGeneratorStopped:
	.string "{PLAYER} stepped on the switch.\p"
	.string "Click…\p"
	.string "… … … … … … … …\n"
	.string "… … … … … … … …\p"
	.string "The generator appears to have\n"
	.string "stopped…$"

NewMauville_Inside_Text_GeneratorRestarted:
	.string "Bzzzt! Something laughed inside the\n"
	.string "generator, and it restarted itself!$"

NewMauville_Inside_Text_GeneratorRestartedAgain:
	.string "The same cry rings through the plant.\p"
	.string "Whatever is inside wants a battle.$"

NewMauville_Inside_Text_RotomAppears:
	.string "The generator bursts open!\n"
	.string "ROTOM challenges your team!$"

NewMauville_Inside_Text_RotomRemains:
	.string "ROTOM slips back into the generator.\n"
	.string "It is still waiting for your answer.$"

NewMauville_Inside_Text_RotomCaught:
	.string "ROTOM is safely out of the\n"
	.string "generator.\p"
	.string "The ROTOM CATALOG changes ROTOM\n"
	.string "into its five appliances.\p"
	.string "The sign near the entrance lists\n"
	.string "this plant's wild POKéMON.\p"
	.string "It shows legendary requirements\n"
	.string "and marks partners you've caught.\p"
	.string "The local guide in MAUVILLE's\n"
	.string "POKéMON CENTER can help, too.$"
@ BEGIN EMERALD CHAMPIONS LEGENDARY ROOTS
.set EC_SIGN_ZAPDOS_ID, 69

NewMauville_Inside_EventScript_Zapdos::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_ZAPDOS_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 5, Common_EventScript_LegendaryRestingAtShrine
	goto_if_eq VAR_RESULT, 0, NewMauville_Inside_EventScript_ZapdosDormant
	goto_if_eq VAR_RESULT, 1, NewMauville_Inside_EventScript_ZapdosNeedsPartner
	goto_if_eq VAR_RESULT, 4, NewMauville_Inside_EventScript_ZapdosCleanup
	msgbox NewMauville_Inside_Text_ZapdosAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_ZAPDOS, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_ZAPDOS_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_LegendaryResting
	goto NewMauville_Inside_EventScript_ZapdosCaught
	end

NewMauville_Inside_EventScript_ZapdosDormant::
	msgbox NewMauville_Inside_Text_ZapdosDormant, MSGBOX_DEFAULT
	release
	end

NewMauville_Inside_EventScript_ZapdosNeedsPartner::
	special ResearchSelectedLegendarySign
	msgbox gStringVar4, MSGBOX_DEFAULT
	release
	end

NewMauville_Inside_EventScript_ZapdosCaught::
	setflag FLAG_EC_CAUGHT_ZAPDOS
NewMauville_Inside_EventScript_ZapdosCleanup::
	setflag FLAG_EC_CAUGHT_ZAPDOS
	removeobject LOCALID_EC_ZAPDOS
	release
	end

NewMauville_Inside_Text_ZapdosDormant:
	.string "ZAPDOS watches over the\n"
	.string "generator. Return with the\l"
	.string "BALANCE BADGE to challenge it.$"

NewMauville_Inside_Text_ZapdosNeedsPartner:
	.string "ZAPDOS accepts challengers with\n"
	.string "the BALANCE BADGE. No partner\l"
	.string "species is required.$"

NewMauville_Inside_Text_ZapdosAwakens:
	.string "ZAPDOS crackles with electricity!\n"
	.string "It accepts your challenge!$"



@ END EMERALD CHAMPIONS LEGENDARY ROOTS

NewMauville_Inside_EventScript_RotomReturn::
	lockall
	goto_if_unset FLAG_EC_RESOLVED_ROTOM, NewMauville_Inside_EventScript_RotomReturnEnd
	goto_if_set FLAG_EC_CAUGHT_ROTOM, NewMauville_Inside_EventScript_GeneratorOff
	msgbox NewMauville_Inside_Text_RotomReturn, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, NewMauville_Inside_EventScript_RotomReturnEnd
	goto NewMauville_Inside_EventScript_RotomEncounter
NewMauville_Inside_EventScript_RotomReturnEnd::
	releaseall
	end
NewMauville_Inside_Text_RotomReturn:
	.string "A quiet hum comes from inside.\n"
	.string "Invite ROTOM out again?$"

NewMauville_Inside_Text_RotomResolved:
	.string "The generator is quiet at last.\n"
	.string "WATTSON will want these readings.$"
