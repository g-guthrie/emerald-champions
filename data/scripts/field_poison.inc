EventScript_FieldPoison::
	lockall
	special TryFieldPoisonWhiteOut
	compare VAR_RESULT, FLDPSN_WHITEOUT
	goto_if_eq EventScript_FieldWhiteOut
	compare VAR_RESULT, FLDPSN_FRONTIER_WHITEOUT
	goto_if_eq EventScript_FrontierFieldWhiteOut
	releaseall
	end

EventScript_FieldWhiteOut::
	message gText_PlayerWhitedOut
	waitmessage
	waitbuttonpress
	special Script_FadeOutMapMusic
	fadescreen FADE_TO_BLACK
	call_if_set FLAG_WHITEOUT_TO_LAVARIDGE, EventScript_SetRespawnLavaridgePkmnCenter
	special SetCB2WhiteOut
	end

EventScript_SetRespawnLavaridgePkmnCenter::
	setrespawn HEAL_LOCATION_LAVARIDGE_TOWN
	return

EventScript_FrontierFieldWhiteOut::
	message gText_PlayerWhitedOut
	waitmessage
	waitbuttonpress
	pike_inchallenge
	compare VAR_RESULT, TRUE
	goto_if_eq BattleFrontier_BattlePike_EventScript_Retire
	pyramid_getlocation
	compare VAR_RESULT, 1  @ On Pyramid floor
	goto_if_eq BattleFrontier_BattlePyramid_EventScript_WarpToLobbyLost
	compare VAR_RESULT, 2  @ On Pyramid peak
	goto_if_eq BattleFrontier_BattlePyramid_EventScript_WarpToLobbyLost
	trainerhill_inchallenge
	compare VAR_RESULT, TRUE
	goto_if_eq TrainerHill_1F_EventScript_Lost
	special Script_FadeOutMapMusic
	fadescreen FADE_TO_BLACK
	special SetCB2WhiteOut
	end
