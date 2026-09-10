Seaspray_Cave_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Seaspray_Cave_OnTransition
	.byte 0

Seaspray_Cave_OnTransition:
	@ Emerald Champions: record the landmark so the PokeNav map lists it.
	setflag FLAG_LANDMARK_SEASPRAY_CAVE
	end

Seaspray_Cave_EventScript_Explorer::
	msgbox Seaspray_Cave_Text_Explorer, MSGBOX_NPC
	end
Seaspray_Cave_Text_Explorer:
	.string "The shifting tides exposed this\n"
	.string "cave, but they did not create it.\p"
	.string "Explore its deeper chambers.\n"
	.string "Unusual water POKéMON have made a\l"
	.string "home beneath the coast.$"

Seaspray_Cave_EventScript_DuskBallPack::
	finditem ITEM_DUSK_BALL, 5
	end

Seaspray_Cave_EventScript_TimerBallPack::
	finditem ITEM_TIMER_BALL, 5
	end
