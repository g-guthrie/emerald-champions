EventScript_StartDexNavBattle::
	lock
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER Common_Movement_ExclamationMark
	waitmovement 0
	waitse
	dowildbattle
	release
	end

EventScript_NotFoundNearby::
	msgbox sText_NotFoundNearby, MSGBOX_SIGN
	end

EventScript_DexNavNotAvailable::
	msgbox sText_DexNavNotAvailable, MSGBOX_SIGN
	end

EventScript_DexNavStartSurfing::
	msgbox sText_DexNavStartSurfing, MSGBOX_SIGN
	end

EventScript_DexNavStepOnLand::
	msgbox sText_DexNavStepOnLand, MSGBOX_SIGN
	end

EventScript_DexNavGetOffBike::
	msgbox sText_DexNavGetOffBike, MSGBOX_SIGN
	end

@ The same, when R will repeat the search (no Bag item owns R): the player
@ need not open the DexNav again.
EventScript_DexNavStartSurfingRepeat::
	msgbox sText_DexNavStartSurfingRepeat, MSGBOX_SIGN
	end

EventScript_DexNavStepOnLandRepeat::
	msgbox sText_DexNavStepOnLandRepeat, MSGBOX_SIGN
	end

EventScript_DexNavGetOffBikeRepeat::
	msgbox sText_DexNavGetOffBikeRepeat, MSGBOX_SIGN
	end

EventScript_MovedTooFast::
	msgbox sText_TryMovingSlower, MSGBOX_SIGN
	call DexNav_ReportChainEnd
	end

EventScript_DexNavTimedOut::
	msgbox sText_DexNavTimedOut, MSGBOX_SIGN
	call DexNav_ReportChainEnd
	end

EventScript_PokemonGotAway::
	msgbox sText_PokemonGotAway, MSGBOX_SIGN
	call DexNav_ReportChainEnd
	end

EventScript_LostSignal::
	msgbox sText_LostSignal, MSGBOX_SIGN
	call DexNav_ReportChainEnd
	end

EventScript_TooDark::
	msgbox sText_TooDark, MSGBOX_SIGN
	end

DexNav_ReportChainEnd:
	compare VAR_0x8004, 0
	goto_if_eq DexNav_ReportChainEnd_Return
	msgbox sText_DexNavChainEnded, MSGBOX_SIGN
DexNav_ReportChainEnd_Return:
	return

sText_NotFoundNearby:
	.string "No reachable hiding spot is nearby.\n"
	.string "Move closer to its habitat and retry.$"

sText_DexNavNotAvailable:
	.string "{STR_VAR_1} can't be searched\n"
	.string "here right now. Open DexNav to\l"
	.string "choose an available Pokémon.$"

sText_DexNavStartSurfing:
	.string "Start Surfing on the water first.\n"
	.string "Then search again from the water.$"

sText_DexNavStepOnLand:
	.string "Step onto land before searching\n"
	.string "for this Pokémon.$"

sText_DexNavGetOffBike:
	.string "Get off your Bike before searching.\n"
	.string "Hold A while moving to sneak.$"

sText_DexNavStartSurfingRepeat:
	.string "Start Surfing on the water first,\n"
	.string "then press {R_BUTTON} to search for it.$"

sText_DexNavStepOnLandRepeat:
	.string "Step onto land first, then\n"
	.string "press {R_BUTTON} to search for it.$"

sText_DexNavGetOffBikeRepeat:
	.string "Get off your Bike first, then\n"
	.string "press {R_BUTTON} to search for it.\p"
	.string "Hold A while moving to sneak.$"

sText_TryMovingSlower:
	.string "The Pokémon got away!\n"
	.string "Hold A while moving to sneak.\l"
	.string "Don't run or cycle near the spot.$"

sText_DexNavTimedOut:
	.string "The Pokémon got away!\n"
	.string "Reach the spot within 15 seconds.$"

sText_PokemonGotAway:
	.string "The Pokémon got away!$"

sText_DexNavChainEnded:
	.string "Chain {STR_VAR_1} ended.$"

sText_LostSignal:
	.string "The signal was lost!\n"
	.string "Stay closer to the hiding spot.$"

sText_TooDark:
	.string "It's too dark to search\nfor a Pokémon!$"
