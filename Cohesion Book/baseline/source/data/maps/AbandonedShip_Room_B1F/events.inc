@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/AbandonedShip_Room_B1F/map.json
@

	.align 2

AbandonedShip_Room_B1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_ITEM_BALL, 4, 4, 3, MOVEMENT_TYPE_FACE_DOWN, 5, 0, TRAINER_TYPE_NONE, ITEM_KELPSY_BERRY, Common_EventScript_FindItem, FLAG_EC_GARDEN_BUNDLE_ABANDONEDSHIP_ROOM_B1F_KELPSY_BERRY

AbandonedShip_Room_B1F_MapWarps:
	warp_def 4, 7, 3, 5, MAP_ABANDONED_SHIP_CORRIDORS_B1F
	warp_def 5, 7, 3, 5, MAP_ABANDONED_SHIP_CORRIDORS_B1F

AbandonedShip_Room_B1F_MapBGEvents:
	bg_sign_event 7, 7, 0, BG_EVENT_PLAYER_FACING_ANY, AbandonedShip_Room_B1F_EventScript_Spiritomb

AbandonedShip_Room_B1F_MapEvents::
	map_events AbandonedShip_Room_B1F_ObjectEvents, AbandonedShip_Room_B1F_MapWarps, NULL, AbandonedShip_Room_B1F_MapBGEvents

