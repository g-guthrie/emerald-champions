Route111_RuinsExterior_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Route111_RuinsExterior_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route111_RuinsExterior_OnFrame
	.byte 0

Route111_RuinsExterior_OnFrame:
	map_script_2 VAR_TEMP_F, 0, Route111_RuinsExterior_EventScript_CheckArrival
	.2byte 0

Route111_RuinsExterior_EventScript_CheckArrival::
	setvar VAR_TEMP_F, 1
	goto_if_unset FLAG_EC_SURVEY_FLIGHT_ARRIVAL, Route111_RuinsExterior_EventScript_ArrivalEnd
	clearflag FLAG_EC_SURVEY_FLIGHT_ARRIVAL
	lockall
	msgbox Route111_RuinsExterior_Text_Arrival, MSGBOX_DEFAULT
	releaseall
Route111_RuinsExterior_EventScript_ArrivalEnd::
	end

Route111_RuinsExterior_EventScript_Archaeologist::
	lockall
	faceplayer
	call Route111_RuinsExterior_EventScript_CheckMegaGifts
	goto_if_set FLAG_EC_REPORT_C30_COMPLETE, Route111_RuinsExterior_EventScript_SurveyDone
	call EC_RequireGenerator
	goto_if_unset FLAG_HIDE_ROUTE_119_TEAM_AQUA, Route111_RuinsExterior_EventScript_Briefing
	msgbox Route111_RuinsExterior_Text_Archaeologist, MSGBOX_DEFAULT
	setvar VAR_0x8004, SPECIES_CASTFORM
	special DoesPlayerPartyHaveSelectedSpeciesFamily
	goto_if_eq VAR_RESULT, FALSE, Route111_RuinsExterior_EventScript_NeedCastform
	setvar VAR_0x8004, LEGENDARY_SIGN_LANDORUS
	special TryUnlockLocalLegendaryDiscovery
	setvar VAR_0x8004, LEGENDARY_SIGN_LANDORUS
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 4, Route111_RuinsExterior_EventScript_RecordSurvey
	goto_if_eq VAR_RESULT, 5, Route111_RuinsExterior_EventScript_Resting
	goto_if_lt VAR_RESULT, 2, Route111_RuinsExterior_EventScript_NeedCastform
	special PrepareEmeraldChampionsCastformSurvey
	goto_if_eq VAR_RESULT, FALSE, Route111_RuinsExterior_EventScript_NeedCastform
	closemessage
	dofieldeffect FLDEFF_FIELD_MOVE_SHOW_MON_INIT
	waitfieldeffect FLDEFF_FIELD_MOVE_SHOW_MON
	playmoncry SPECIES_CASTFORM_NORMAL, CRY_MODE_NORMAL
	waitmoncry
	msgbox Route111_RuinsExterior_Text_WindAnswers, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route111_RuinsExterior_EventScript_OfferReturn
	closemessage
	playmoncry SPECIES_LANDORUS, CRY_MODE_ENCOUNTER
	waitmoncry
	setvar VAR_0x8004, LEGENDARY_SIGN_LANDORUS
	special CreateSelectedLegendarySignEncounter
	goto_if_eq VAR_RESULT, FALSE, Route111_RuinsExterior_EventScript_Resting
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, Route111_RuinsExterior_EventScript_RecordSurvey
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, Route111_RuinsExterior_EventScript_RecordSurvey
	goto Route111_RuinsExterior_EventScript_Resting
	end

Route111_RuinsExterior_EventScript_RecordSurvey::
	setflag FLAG_EC_RESOLVED_LANDORUS
	completemilestone FLAG_EC_REPORT_C30_COMPLETE
	buffermilestonetext FLAG_EC_REPORT_C30_COMPLETE
	msgbox Route111_RuinsExterior_Text_SurveyRecorded, MSGBOX_DEFAULT
	call_if_eq VAR_RESULT, 3, Route111_RuinsExterior_EventScript_SurveyAndCapLine
	call_if_eq VAR_RESULT, 2, Route111_RuinsExterior_EventScript_SurveyLine
	call_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_PrintMilestoneCapLine
	goto Route111_RuinsExterior_EventScript_OfferReturn
	end

Route111_RuinsExterior_EventScript_SurveyAndCapLine::
	msgbox Route111_RuinsExterior_Text_SurveyAndCapLine, MSGBOX_DEFAULT
	return

Route111_RuinsExterior_EventScript_SurveyLine::
	msgbox Route111_RuinsExterior_Text_SurveyLine, MSGBOX_DEFAULT
	return

Route111_RuinsExterior_EventScript_SurveyDone::
	msgbox Route111_RuinsExterior_Text_SurveyDone, MSGBOX_DEFAULT
	goto Route111_RuinsExterior_EventScript_OfferReturn
	end

Route111_RuinsExterior_EventScript_NeedCastform::
	msgbox Route111_RuinsExterior_Text_NeedCastform, MSGBOX_DEFAULT
	goto Route111_RuinsExterior_EventScript_OfferReturn
	end

Route111_RuinsExterior_EventScript_Resting::
	msgbox Route111_RuinsExterior_Text_Resting, MSGBOX_DEFAULT
Route111_RuinsExterior_EventScript_OfferReturn::
	msgbox Route111_RuinsExterior_Text_OfferReturn, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route111_RuinsExterior_EventScript_End
	closemessage
	fadescreen FADE_TO_BLACK
	playse SE_M_FLY
	waitse
	setflag FLAG_EC_SURVEY_FLIGHT_ARRIVAL
	warp MAP_ROUTE119_WEATHER_INSTITUTE_2F, 5, 6
	waitstate
Route111_RuinsExterior_EventScript_End::
	releaseall
	end

Route111_RuinsExterior_EventScript_Briefing::
	msgbox Route111_RuinsExterior_Text_Briefing, MSGBOX_DEFAULT
	releaseall
	end

@ Flygonite: unconditional besides having the Mega Ring, so it's reachable
@ whether or not the player ever collapses the tower.
Route111_RuinsExterior_EventScript_CheckMegaGifts::
	goto_if_set FLAG_EC_MEGA_GIFT_FLYGONITE, Route111_RuinsExterior_EventScript_CheckMegaGifts_Done
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, Route111_RuinsExterior_EventScript_CheckMegaGifts_Done
	msgbox Route111_RuinsExterior_Text_MegaGift_FLYGONITE, MSGBOX_DEFAULT
	giveuniqueitem ITEM_FLYGONITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_FLYGONITE
Route111_RuinsExterior_EventScript_CheckMegaGifts_Done::
	return

Route111_RuinsExterior_Text_Archaeologist:
	.string "These marks follow the storms,\n"
	.string "but our instruments only show\l"
	.string "half the pattern.\p"
	.string "Let CASTFORM show us the rest.$"

Route111_RuinsExterior_Text_WindAnswers:
	.string "CASTFORM shifts with the wind.\n"
	.string "Something circles above the ruins!\p"
	.string "LANDORUS has answered the survey.\n"
	.string "Are you ready to meet it?$"

Route111_RuinsExterior_Text_SurveyRecorded:
	.string "There. The wind changes around\n"
	.string "the old stonework. The Institute\l"
	.string "needs to see this record.$"

Route111_RuinsExterior_Text_SurveyAndCapLine:
	.string "Your survey earns ¥{STR_VAR_2}.\n"
	.string "Your level cap is now {STR_VAR_1}.$"

Route111_RuinsExterior_Text_SurveyLine:
	.string "Your survey earns ¥{STR_VAR_2}.$"

Route111_RuinsExterior_Text_SurveyDone:
	.string "The survey is complete. LANDORUS\n"
	.string "may still appear in the wild if\l"
	.string "you haven't caught it.$"

Route111_RuinsExterior_Text_NeedCastform:
	.string "Bring an awake CASTFORM in your\n"
	.string "party and the BALANCE BADGE.\p"
	.string "The Institute can help you find\n"
	.string "your CASTFORM or arrange a\l"
	.string "replacement if it is gone.$"

Route111_RuinsExterior_Text_Resting:
	.string "The wind is settling. We can\n"
	.string "resume when LANDORUS returns.\l"
	.string "Our transport is still available.$"

Route111_RuinsExterior_Text_OfferReturn:
	.string "Take the research flier back to\n"
	.string "the WEATHER INSTITUTE?$"

Route111_RuinsExterior_Text_Briefing:
	.string "Storms leave strange patterns\n"
	.string "across these ruins. Visit the\l"
	.string "WEATHER INSTITUTE on ROUTE 119.\p"
	.string "Their team can arrange a survey\n"
	.string "when they're ready.$"

Route111_RuinsExterior_Text_MegaGift_FLYGONITE:
	.string "The sand at the tower's foot\n"
	.string "gave this up.\p"
	.string "No good to an archaeologist.\n"
	.string "You should have it instead.$"

Route111_RuinsExterior_Text_Arrival:
	.string "The flier settles beside the ruins.\p"
	.string "The archaeologist is just east\n"
	.string "of the landing. Speak to him with\l"
	.string "CASTFORM ready in your party.$"
Route111_RuinsExterior_Text_Returned:
	.string "Back at the WEATHER INSTITUTE.\p"
	.string "The survey team can help you\n"
	.string "prepare or arrange another flight.$"

Route111_RuinsExterior_OnLoad:
	goto_if_lt VAR_MIRAGE_TOWER_STATE, 2, Route111_RuinsExterior_EntranceOpen
	setmetatile 13, 11, METATILE_Mauville_MirageTower_Tile4, TRUE
Route111_RuinsExterior_EntranceOpen:
	end
