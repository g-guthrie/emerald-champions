SSTidalLowerDeck_MapScripts::
	.byte 0

SSTidalLowerDeck_EventScript_Phillip::
	msgbox SSTidalLowerDeck_Text_PhillipPostBattle, MSGBOX_NPC
	end

SSTidalLowerDeck_EventScript_Leonard::
	msgbox SSTidalLowerDeck_Text_LeonardPostBattle, MSGBOX_NPC
	end

SSTidalLowerDeck_Text_PhillipPostBattle:
	.string "We're the CLEANUP BROTHERS!\p"
	.string "The old one dumps the detergent,\n"
	.string "and the young one does the scrubbing!$"

SSTidalLowerDeck_Text_LeonardPostBattle:
	.string "We're the CLEANUP BROTHERS!\p"
	.string "The old one dumps the detergent,\n"
	.string "and the young one does the scrubbing!$"

