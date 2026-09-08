@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/AbandonedShip_HiddenFloorRooms/map.json
@

	.align 2

AbandonedShip_HiddenFloorRooms_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_ITEM_BALL, 41, 4, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, ITEM_LUXURY_BALL, Common_EventScript_FindItem, FLAG_ITEM_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_6_LUXURY_BALL
	object_event 2, OBJ_EVENT_GFX_ITEM_BALL, 16, 10, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, ITEM_SCANNER, Common_EventScript_FindItem, FLAG_ITEM_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_2_SCANNER
	object_event 3, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 5, 11, 3, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, ITEM_MASTER_BALL, Common_EventScript_FindItem, FLAG_ITEM_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_1_MASTER_BALL
	object_event 4, OBJ_EVENT_GFX_ITEM_BALL, 31, 11, 3, MOVEMENT_TYPE_WALK_IN_PLACE_DOWN, 1, 1, TRAINER_TYPE_NONE, ITEM_WATER_STONE, Common_EventScript_FindItem, FLAG_ITEM_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_3_WATER_STONE
	object_event 5, OBJ_EVENT_GFX_ITEM_BALL, 16, 6, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, ITEM_OLD_SEA_MAP, Common_EventScript_FindItem, FLAG_RECEIVED_OLD_SEA_MAP

AbandonedShip_HiddenFloorRooms_MapWarps:
	warp_def 6, 14, 3, 0, MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS
	warp_def 7, 14, 3, 0, MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS
	warp_def 21, 14, 3, 1, MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS
	warp_def 22, 14, 3, 1, MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS
	warp_def 36, 14, 3, 2, MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS
	warp_def 37, 14, 3, 2, MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS
	warp_def 6, 1, 3, 3, MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS
	warp_def 21, 1, 3, 4, MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS
	warp_def 36, 1, 3, 5, MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS

AbandonedShip_HiddenFloorRooms_MapBGEvents:
	bg_hidden_item_event 42, 10, 3, ITEM_KEY_TO_ROOM_1, FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_1_KEY, 1, FALSE
	bg_hidden_item_event 20, 5, 3, ITEM_KEY_TO_ROOM_2, FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_2_KEY, 1, FALSE
	bg_hidden_item_event 1, 12, 3, ITEM_KEY_TO_ROOM_4, FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_4_KEY, 1, FALSE
	bg_hidden_item_event 1, 2, 0, ITEM_KEY_TO_ROOM_6, FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_6_KEY, 1, FALSE
	bg_sign_event 8, 5, 3, BG_EVENT_PLAYER_FACING_ANY, AbandonedShip_HiddenFloorRooms_EventScript_Trash
	bg_sign_event 11, 3, 3, BG_EVENT_PLAYER_FACING_ANY, AbandonedShip_HiddenFloorRooms_EventScript_Trash
	bg_sign_event 10, 10, 3, BG_EVENT_PLAYER_FACING_ANY, AbandonedShip_HiddenFloorRooms_EventScript_Trash
	bg_sign_event 16, 3, 3, BG_EVENT_PLAYER_FACING_ANY, AbandonedShip_HiddenFloorRooms_EventScript_Trash
	bg_sign_event 25, 2, 3, BG_EVENT_PLAYER_FACING_ANY, AbandonedShip_HiddenFloorRooms_EventScript_Trash
	bg_sign_event 24, 6, 3, BG_EVENT_PLAYER_FACING_ANY, AbandonedShip_HiddenFloorRooms_EventScript_Trash

AbandonedShip_HiddenFloorRooms_MapEvents::
	map_events AbandonedShip_HiddenFloorRooms_ObjectEvents, AbandonedShip_HiddenFloorRooms_MapWarps, NULL, AbandonedShip_HiddenFloorRooms_MapBGEvents

