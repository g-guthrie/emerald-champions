CaveOfOrigin_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CaveOfOrigin_1F_OnTransition
	.byte 0

CaveOfOrigin_1F_OnTransition:
	call_if_set FLAG_UNUSED_RS_LEGENDARY_BATTLE_DONE, CaveOfOrigin_EventScript_DisableTriggers
	end

CaveOfOrigin_1F_EventScript_Carbink::
	lock
	faceplayer
	playmoncry SPECIES_CARBINK, CRY_MODE_NORMAL
	waitmoncry
	msgbox CaveOfOrigin_1F_Text_CarbinkStuck, MSGBOX_DEFAULT
	release
	end

CaveOfOrigin_1F_Text_CarbinkStuck:
	.string "This CARBINK is guarding an old ladder.\p"
	.string "Its crystals echo with the energy of\n"
	.string "eight BADGES. Come back when they shine.$"
