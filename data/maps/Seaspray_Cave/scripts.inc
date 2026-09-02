Seaspray_Cave_MapScripts::
	.byte 0

Seaspray_Cave_EventScript_Explorer::
	msgbox Seaspray_Cave_Text_Explorer, MSGBOX_NPC
	end
Seaspray_Cave_Text_Explorer:
	.string "AQUA's tides exposed this cave, but they\n"
	.string "did not create it.\p"
	.string "At the frozen floor, space bends around\n"
	.string "a SIGN shaped like KINGDRA.$"

Seaspray_Cave_EventScript_DuskBallPack::
	finditem ITEM_DUSK_BALL, 5
	end

Seaspray_Cave_EventScript_TimerBallPack::
	finditem ITEM_TIMER_BALL, 5
	end
