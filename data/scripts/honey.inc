EventScript_HoneyEncounter::
	specialvar VAR_RESULT, HoneyWildEncounter
	compare VAR_RESULT, FALSE
	goto_if_eq EventScript_FailSweetScent
	waitstate
	releaseall
	end
