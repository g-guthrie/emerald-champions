@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/TrainerTower_Lobby_Frlg/map.json
@

	.align 2

TrainerTower_Lobby_Frlg_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_NURSE_FRLG, 4, 9, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, TrainerTower_Lobby_EventScript_Nurse, 0
	object_event 2, OBJ_EVENT_GFX_UNION_ROOM_RECEPTIONIST, 14, 9, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, TrainerTower_Lobby_EventScript_MartClerk, 0
	object_event 3, OBJ_EVENT_GFX_UNION_ROOM_RECEPTIONIST, 11, 7, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NONE, 0, TrainerTower_Lobby_EventScript_Receptionist, 0
	object_event 4, OBJ_EVENT_GFX_COOLTRAINER_F, 12, 13, 3, MOVEMENT_TYPE_WANDER_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, TrainerTower_Lobby_EventScript_CooltrainerF, 0
	object_event 5, OBJ_EVENT_GFX_BALDING_MAN, 2, 12, 3, MOVEMENT_TYPE_LOOK_AROUND, 1, 1, TRAINER_TYPE_NONE, 0, TrainerTower_Lobby_EventScript_BaldingMan, 0

TrainerTower_Lobby_Frlg_MapWarps:
	warp_def 9, 2, 3, 1, MAP_TRAINER_TOWER_1F
	warp_def 9, 15, 3, 0, MAP_SEVEN_ISLAND_TRAINER_TOWER
	warp_def 17, 8, 0, 0, MAP_TRAINER_TOWER_ELEVATOR

TrainerTower_Lobby_Frlg_MapCoordEvents:
	coord_event 9, 7, 3, VAR_MAP_SCENE_TRAINER_TOWER, 0, TrainerTower_Lobby_EventScript_EntryTrigger

TrainerTower_Lobby_Frlg_MapBGEvents:
	bg_sign_event 8, 10, 0, BG_EVENT_PLAYER_FACING_NORTH, TrainerTower_Lobby_EventScript_ShowRecords

TrainerTower_Lobby_Frlg_MapEvents::
	map_events TrainerTower_Lobby_Frlg_ObjectEvents, TrainerTower_Lobby_Frlg_MapWarps, TrainerTower_Lobby_Frlg_MapCoordEvents, TrainerTower_Lobby_Frlg_MapBGEvents

