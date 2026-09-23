Route111_RuinsExterior_MapScripts::
	.byte 0

@ The Center lead sends players here with a Castform after the fifth badge.
Route111_RuinsExterior_EventScript_Archaeologist::
	lock
	faceplayer
	setvar VAR_0x8004, LEGENDARY_SIGN_LANDORUS
	special GetSelectedLegendarySignState
	goto_if_eq VAR_RESULT, 2, Route111_RuinsExterior_EventScript_LandorusCaught
	goto_if_unset FLAG_BADGE05_GET, Route111_RuinsExterior_EventScript_NotYet
	special TryUnlockLocalLegendaryDiscovery
	goto_if_eq VAR_RESULT, TRUE, Route111_RuinsExterior_EventScript_CastformReading
	goto_if_set FLAG_EC_RESOLVED_LANDORUS, Route111_RuinsExterior_EventScript_AfterSurvey
	msgbox Route111_RuinsExterior_Text_NeedCastform, MSGBOX_DEFAULT
	release
	end

Route111_RuinsExterior_EventScript_CastformReading::
	msgbox Route111_RuinsExterior_Text_CastformReading, MSGBOX_DEFAULT
	closemessage
	playmoncry SPECIES_CASTFORM, CRY_MODE_NORMAL
	waitmoncry
	applymovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	waitmovement 0
	setflag FLAG_EC_RESOLVED_LANDORUS
	msgbox Route111_RuinsExterior_Text_LandorusStirs, MSGBOX_DEFAULT
	release
	end

Route111_RuinsExterior_EventScript_AfterSurvey::
	msgbox Route111_RuinsExterior_Text_LandorusStirs, MSGBOX_DEFAULT
	release
	end

Route111_RuinsExterior_EventScript_LandorusCaught::
	msgbox Route111_RuinsExterior_Text_LandorusCaught, MSGBOX_DEFAULT
	release
	end

Route111_RuinsExterior_EventScript_NotYet::
	msgbox Route111_RuinsExterior_Text_NotYet, MSGBOX_DEFAULT
	release
	end

Route111_RuinsExterior_Text_NeedCastform:
	.string "The storms over these ruins follow\n"
	.string "an old pattern. A Castform could\l"
	.string "read it for us.\p"
	.string "The Weather Institute on Route 119\n"
	.string "studies them. Bring one along!$"

Route111_RuinsExterior_Text_CastformReading:
	.string "Your Castform is reacting to the\n"
	.string "ruins! Let's see what it senses.$"

Route111_RuinsExterior_Text_LandorusStirs:
	.string "The pattern points to Landorus.\n"
	.string "It roams the grass around these\l"
	.string "ruins now. Keep searching!$"

Route111_RuinsExterior_Text_LandorusCaught:
	.string "Landorus travels with you now.\n"
	.string "These ruins have told their story.$"

Route111_RuinsExterior_Text_NotYet:
	.string "I'm surveying these ruins. The\n"
	.string "weather here is strange.\p"
	.string "Come back once you've earned five\n"
	.string "Gym Badges. I could use the help.$"
