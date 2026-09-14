IslandCave_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, IslandCave_OnResume
	map_script MAP_SCRIPT_ON_LOAD, IslandCave_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, IslandCave_OnTransition
	.byte 0

IslandCave_OnResume:
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, IslandCave_EventScript_TryRemoveRegice
	end

IslandCave_EventScript_TryRemoveRegice::
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
	removeobject VAR_LAST_TALKED
	return

IslandCave_OnLoad:
	call_if_unset FLAG_SYS_BRAILLE_REGICE_COMPLETED, IslandCave_EventScript_HideRegiEntrance
	end

IslandCave_EventScript_HideRegiEntrance::
	setmetatile 7, 19, METATILE_Cave_EntranceCover, TRUE
	setmetatile 8, 19, METATILE_Cave_EntranceCover, TRUE
	setmetatile 9, 19, METATILE_Cave_EntranceCover, TRUE
	setmetatile 7, 20, METATILE_Cave_SealedChamberBraille_Mid, TRUE
	setmetatile 8, 20, METATILE_Cave_SealedChamberBraille_Mid, TRUE
	setmetatile 9, 20, METATILE_Cave_SealedChamberBraille_Mid, TRUE
	return

IslandCave_OnTransition:
	setflag FLAG_LANDMARK_ISLAND_CAVE
	call IslandCave_EventScript_ClearSteps
	call_if_unset FLAG_DEFEATED_REGICE, IslandCave_EventScript_ShowRegice
	end

IslandCave_EventScript_ShowRegice::
	clearflag FLAG_HIDE_REGICE
	return

IslandCave_EventScript_OpenRegiEntrance::
	setmetatile 7, 19, METATILE_Cave_SealedChamberEntrance_TopLeft, TRUE
	setmetatile 8, 19, METATILE_Cave_SealedChamberEntrance_TopMid, TRUE
	setmetatile 9, 19, METATILE_Cave_SealedChamberEntrance_TopRight, TRUE
	setmetatile 7, 20, METATILE_Cave_SealedChamberEntrance_BottomLeft, TRUE
	setmetatile 8, 20, METATILE_Cave_SealedChamberEntrance_BottomMid, FALSE
	setmetatile 9, 20, METATILE_Cave_SealedChamberEntrance_BottomRight, TRUE
	special DrawWholeMapView
	playse SE_BANG
	setflag FLAG_SYS_BRAILLE_REGICE_COMPLETED
	lockall
	call IslandCave_EventScript_ReadAccount
	releaseall
	end

IslandCave_EventScript_CaveEntranceMiddle::
	lockall
	call_if_set FLAG_TEMP_REGICE_PUZZLE_FAILED, IslandCave_EventScript_ClearSteps
	goto_if_set FLAG_SYS_BRAILLE_REGICE_COMPLETED, IslandCave_EventScript_BigHoleInWall
	msgbox IslandCave_Text_PuzzleDirections, MSGBOX_DEFAULT
	setflag FLAG_TEMP_REGICE_PUZZLE_STARTED
	special ShouldDoBrailleRegicePuzzle
	goto IslandCave_EventScript_CloseBrailleMsg
	end

IslandCave_EventScript_BigHoleInWall::
	call IslandCave_EventScript_ReadAccount
	releaseall
	end

IslandCave_EventScript_CaveEntranceSide::
	lockall
	goto_if_set FLAG_SYS_BRAILLE_REGICE_COMPLETED, IslandCave_EventScript_BigHoleInWall
	call_if_set FLAG_TEMP_REGICE_PUZZLE_FAILED, IslandCave_EventScript_ClearSteps
	msgbox IslandCave_Text_PuzzleDirections, MSGBOX_DEFAULT
	goto_if_set FLAG_SYS_BRAILLE_REGICE_COMPLETED, IslandCave_EventScript_CloseBrailleMsg
	setflag FLAG_TEMP_REGICE_PUZZLE_STARTED
	special ShouldDoBrailleRegicePuzzle
	goto IslandCave_EventScript_CloseBrailleMsg
	end

IslandCave_EventScript_CloseBrailleMsg::
	releaseall
	end

IslandCave_EventScript_ClearSteps::
	setvar VAR_REGICE_STEPS_1, 0
	setvar VAR_REGICE_STEPS_2, 0
	setvar VAR_REGICE_STEPS_3, 0
	clearflag FLAG_TEMP_REGICE_PUZZLE_FAILED
	return

IslandCave_EventScript_Regice::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_REGICE, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, SPECIES_REGICE
	setvar VAR_0x8005, 0
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special StartRegiBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, IslandCave_EventScript_CaughtRegice
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, IslandCave_EventScript_DefeatedRegice
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, IslandCave_EventScript_RanFromRegice
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, IslandCave_EventScript_RanFromRegice
	release
	end

IslandCave_EventScript_CaughtRegice::
	setflag FLAG_EC_RESOLVED_REGICE
	call EmeraldChampions_EventScript_ThreeRuinsProgress
	setflag FLAG_DEFEATED_REGICE
	release
	end

IslandCave_EventScript_DefeatedRegice::
	setflag FLAG_EC_RESOLVED_REGICE
	goto EmeraldChampions_EventScript_GuardianVictory
	end

IslandCave_EventScript_RanFromRegice::
	setvar VAR_0x8004, SPECIES_REGICE
	goto Common_EventScript_LegendaryFlewAway
	end


IslandCave_EventScript_ReadAccount::
	msgbox IslandCave_Text_RouteAccount, MSGBOX_DEFAULT
	call EmeraldChampions_EventScript_ThreeRuinsProgress
	return

IslandCave_Text_PuzzleDirections:
	.string "Read this wall, then make one\n"
	.string "complete lap beside the room's\l"
	.string "outer wall. Return to this spot.\p"
	.string "Stay next to the wall all the way.\n"
	.string "If you stray, read this again\l"
	.string "to begin another lap.$"

IslandCave_Text_RouteAccount:
	.string "The second account begins below\n"
	.string "a deep-water basin. The western\l"
	.string "surface currents are a false trail.\p"
	.string "The sea's power cannot be bound\n"
	.string "by an artifact alone.$"
