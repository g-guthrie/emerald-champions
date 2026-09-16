@ Emerald Champions: the Apprentice is retired with the vanilla Battle Tower. The
@ object stays in map.json until the owner removes it and the Tower lobby hides it on
@ transition, so this script is unreachable; it keeps a single line so the map.json
@ reference still resolves. The apprentice_* script macros and src/apprentice.c are
@ no longer reached from any script.
BattleFrontier_BattleTowerLobby_EventScript_Apprentice::
	msgbox Apprentice_Text_PreparingForCircuit, MSGBOX_NPC
	end

Apprentice_Text_PreparingForCircuit:
	.string "I'm putting together my first\n"
	.string "six-POKéMON CIRCUIT team!\p"
	.string "I picked my two opening partners.\n"
	.string "Now I'm looking for teammates who\l"
	.string "can help when they need to switch.\p"
	.string "Six strong POKéMON are a start.\n"
	.string "I want mine to work together!$"
