GraniteCave_B2F_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, GraniteCave_B2F_SetFlashRadius
	.byte 0

@ Inclement gives B1F/B2F a moderate ambient radius so Steven's Letter can be
@ delivered before Brawly's badge unlocks Flash.
GraniteCave_B2F_SetFlashRadius:
	setflashlevel 4
	end

GraniteCave_B2F_EventScript_TimerBallPack::
	finditem ITEM_TIMER_BALL, 5
	end
