@ All unused / leftover scripts from RS
CaveOfOrigin_EventScript_LegendaryCry::
	lockall
	waitse
	playmoncry SPECIES_KYOGRE, 2  @ SPECIES_GROUDON in Ruby
	waitmoncry
	setvar VAR_TEMP_5, 1
	releaseall
	end

CaveOfOrigin_EventScript_DisableTriggers::
	setvar VAR_TEMP_1, 1
	setvar VAR_TEMP_2, 1
	setvar VAR_TEMP_3, 1
	setvar VAR_TEMP_4, 1
	setvar VAR_TEMP_5, 1
	return
