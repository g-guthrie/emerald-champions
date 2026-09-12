SandstrewnRuins_B1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SandstrewnRuins_B1F_OnTransition
	.byte 0

SandstrewnRuins_B1F_OnTransition:
	setflag FLAG_EC_SURVEYED_DESERT_DEPTHS
	end
