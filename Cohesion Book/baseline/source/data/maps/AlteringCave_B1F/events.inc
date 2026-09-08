@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/AlteringCave_B1F/map.json
@

	.align 2

AlteringCave_B1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_BREAKABLE_ROCK, 2, 2, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_1
	object_event 2, OBJ_EVENT_GFX_BREAKABLE_ROCK, 3, 4, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_2
	object_event 3, OBJ_EVENT_GFX_BREAKABLE_ROCK, 4, 1, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_3
	object_event 4, OBJ_EVENT_GFX_BREAKABLE_ROCK, 6, 1, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_4
	object_event 5, OBJ_EVENT_GFX_BREAKABLE_ROCK, 35, 1, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_5
	object_event 6, OBJ_EVENT_GFX_BREAKABLE_ROCK, 37, 1, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_6
	object_event 7, OBJ_EVENT_GFX_BREAKABLE_ROCK, 38, 2, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_7
	object_event 8, OBJ_EVENT_GFX_BREAKABLE_ROCK, 37, 4, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_8
	object_event 9, OBJ_EVENT_GFX_BREAKABLE_ROCK, 35, 5, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, EventScript_RockSmash, FLAG_TEMP_9
	object_event 10, OBJ_EVENT_GFX_INCLEMENT_MEWTWO, 7, 13, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, AlteringCave_B1F_EventScript_Mewtwo, FLAG_EC_CAUGHT_MEWTWO
	object_event 11, OBJ_EVENT_GFX_LEAF, 21, 16, 3, MOVEMENT_TYPE_FACE_UP, 0, 0, TRAINER_TYPE_NONE, 0, NULL, FLAG_EC_DEFEATED_LEAF_ALTERING_CAVE
	object_event 12, OBJ_EVENT_GFX_MEGA_STONE, 35, 21, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, ITEM_MEWTWONITE_Y, Common_EventScript_FindItem, FLAG_EC_MEGA_REWARD_MEWTWONITE_Y

AlteringCave_B1F_MapWarps:
	warp_def 5, 7, 0, 7, MAP_ALTERING_CAVE

AlteringCave_B1F_MapCoordEvents:
	coord_event 21, 19, 0, VAR_TEMP_0, 0, AlteringCave_B1F_EventScript_Leaf

AlteringCave_B1F_MapEvents::
	map_events AlteringCave_B1F_ObjectEvents, AlteringCave_B1F_MapWarps, AlteringCave_B1F_MapCoordEvents, NULL

