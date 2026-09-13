GraniteCave_B2F_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, GraniteCave_B2F_SetFlashRadius
	.byte 0

@ Keep the original moderate ambient radius if the player misses the hiker;
@ the Flash license and Stone Badge open the wider radius.
GraniteCave_B2F_SetFlashRadius:
	special SetGraniteCaveFlashLevel
	end

GraniteCave_B2F_EventScript_TimerBallPack::
	finditem ITEM_TIMER_BALL, 5
	end
