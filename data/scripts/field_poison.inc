EventScript_FieldPoison::
	lockall
	special TryFieldPoisonWhiteOut
	waitstate
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
	waitstate
	fadescreen FADE_TO_BLACK
	call_if_set FLAG_WHITEOUT_TO_LAVARIDGE, EventScript_SetRespawnLavaridgePkmnCenter
	special SetCB2WhiteOut
	waitstate
	end

EventScript_SetRespawnLavaridgePkmnCenter::
	setrespawn HEAL_LOCATION_LAVARIDGE_TOWN
	return

EventScript_FrontierFieldWhiteOut::
	@ Emerald Champions: the Battle Pike / Pyramid / Trainer Hill retire paths are gone
	@ (their C drivers were deleted and their label targets live in .inc files that are
	@ no longer assembled), so a white-out here is just an ordinary white-out.
	message gText_PlayerWhitedOut
	waitmessage
	waitbuttonpress
	special Script_FadeOutMapMusic
	waitstate
	fadescreen FADE_TO_BLACK
	special SetCB2WhiteOut
	waitstate
	end
