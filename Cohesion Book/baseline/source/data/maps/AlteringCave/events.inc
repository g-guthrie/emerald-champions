@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/AlteringCave/map.json
@

	.align 2

AlteringCave_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_BREAKABLE_ROCK, 5, 20, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_1
	object_event 2, OBJ_EVENT_GFX_BREAKABLE_ROCK, 7, 21, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_2
	object_event 3, OBJ_EVENT_GFX_BREAKABLE_ROCK, 9, 18, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_3
	object_event 4, OBJ_EVENT_GFX_BREAKABLE_ROCK, 11, 21, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_4
	object_event 5, OBJ_EVENT_GFX_BREAKABLE_ROCK, 13, 21, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_5
	object_event 6, OBJ_EVENT_GFX_BREAKABLE_ROCK, 14, 20, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_6

AlteringCave_MapWarps:
	warp_def 33, 21, 0, 0, MAP_ROUTE103
	warp_def 2, 4, 0, 0, MAP_ALTERING_CAVE_1F
	warp_def 5, 15, 0, 2, MAP_ALTERING_CAVE_1F
	warp_def 10, 2, 0, 1, MAP_ALTERING_CAVE_1F
	warp_def 24, 11, 0, 3, MAP_ALTERING_CAVE_1F
	warp_def 30, 10, 0, 4, MAP_ALTERING_CAVE_1F
	warp_def 34, 2, 0, 5, MAP_ALTERING_CAVE_1F
	warp_def 1, 8, 0, 0, MAP_ALTERING_CAVE_B1F

AlteringCave_MapEvents::
	map_events AlteringCave_ObjectEvents, AlteringCave_MapWarps, NULL, NULL

