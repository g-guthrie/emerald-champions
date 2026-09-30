@ All finale callers share the Center guide's live trainer/ticket decision tree.
@ Callers retain their lock; this special reads progress without unlocking it.
Common_EventScript_FinaleDirections::
	special BufferCenterGuideObjective
	msgbox gStringVar4, MSGBOX_DEFAULT
	return

Common_EventScript_FinaleWait::
	call Common_EventScript_FinaleDirections
	releaseall
	end

Common_EventScript_RecordDeoxysFinale::
	setflag FLAG_EC_FINALE_DEOXYS_RESOLVED
	return
