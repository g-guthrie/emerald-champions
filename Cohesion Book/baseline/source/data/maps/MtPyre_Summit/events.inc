@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/MtPyre_Summit/map.json
@

	.align 2

MtPyre_Summit_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_OLD_MAN, 22, 5, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, MtPyre_Summit_EventScript_OldMan, 0
	object_event 2, OBJ_EVENT_GFX_ARCHIE, 23, 6, 3, MOVEMENT_TYPE_FACE_UP, 0, 0, TRAINER_TYPE_NONE, 0, 0x0, FLAG_HIDE_MT_PYRE_SUMMIT_ARCHIE
	object_event 3, OBJ_EVENT_GFX_EXPERT_F, 23, 5, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, MtPyre_Summit_EventScript_OldLady, 0
	object_event 4, OBJ_EVENT_GFX_AQUA_MEMBER_M, 21, 20, 3, MOVEMENT_TYPE_FACE_RIGHT, 0, 0, TRAINER_TYPE_NORMAL, 3, MtPyre_Summit_EventScript_Grunt1, FLAG_HIDE_MT_PYRE_SUMMIT_TEAM_AQUA
	object_event 5, OBJ_EVENT_GFX_AQUA_MEMBER_M, 25, 23, 3, MOVEMENT_TYPE_FACE_LEFT, 0, 0, TRAINER_TYPE_NORMAL, 3, MtPyre_Summit_EventScript_Grunt2, FLAG_HIDE_MT_PYRE_SUMMIT_TEAM_AQUA
	object_event 6, OBJ_EVENT_GFX_AQUA_MEMBER_M, 21, 16, 3, MOVEMENT_TYPE_FACE_RIGHT, 0, 0, TRAINER_TYPE_NORMAL, 3, MtPyre_Summit_EventScript_Grunt3, FLAG_HIDE_MT_PYRE_SUMMIT_TEAM_AQUA
	object_event 7, OBJ_EVENT_GFX_AQUA_MEMBER_F, 25, 16, 3, MOVEMENT_TYPE_FACE_LEFT, 1, 1, TRAINER_TYPE_NORMAL, 3, MtPyre_Summit_EventScript_Grunt4, FLAG_HIDE_MT_PYRE_SUMMIT_TEAM_AQUA
	object_event 8, OBJ_EVENT_GFX_MAXIE, 23, 6, 3, MOVEMENT_TYPE_FACE_UP, 1, 1, TRAINER_TYPE_NONE, 0, 0x0, FLAG_HIDE_MT_PYRE_SUMMIT_MAXIE
	object_event 9, OBJ_EVENT_GFX_GOLD_ITEM_BALL, 38, 21, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, ITEM_BEAST_BALL, Common_EventScript_FindItem, FLAG_EC_ITEM_MT_PYRE_SUMMIT_BEAST_BALL
	object_event 10, OBJ_EVENT_GFX_AQUA_MEMBER_M, 21, 11, 3, MOVEMENT_TYPE_FACE_RIGHT, 0, 0, TRAINER_TYPE_NONE, 0, MtPyre_Summit_EventScript_Matt, FLAG_HIDE_MT_PYRE_SUMMIT_TEAM_AQUA

MtPyre_Summit_MapWarps:
	warp_def 22, 31, 3, 1, MAP_MT_PYRE_EXTERIOR
	warp_def 23, 31, 3, 1, MAP_MT_PYRE_EXTERIOR
	warp_def 24, 31, 3, 1, MAP_MT_PYRE_EXTERIOR

MtPyre_Summit_MapCoordEvents:
	coord_event 22, 7, 3, VAR_MT_PYRE_STATE, 0, MtPyre_Summit_EventScript_TeamAquaTrigger0
	coord_event 23, 7, 3, VAR_MT_PYRE_STATE, 0, MtPyre_Summit_EventScript_TeamAquaTrigger1
	coord_event 24, 7, 3, VAR_MT_PYRE_STATE, 0, MtPyre_Summit_EventScript_TeamAquaTrigger2
	coord_event 22, 9, 3, VAR_MT_PYRE_STATE, 2, MtPyre_Summit_EventScript_ArchieMaxieTrigger0
	coord_event 23, 9, 3, VAR_MT_PYRE_STATE, 2, MtPyre_Summit_EventScript_ArchieMaxieTrigger1
	coord_event 24, 9, 3, VAR_MT_PYRE_STATE, 2, MtPyre_Summit_EventScript_ArchieMaxieTrigger2

MtPyre_Summit_MapBGEvents:
	bg_hidden_item_event 9, 25, 0, ITEM_DUSK_BALL, FLAG_HIDDEN_ITEM_MT_PYRE_SUMMIT_DUSK_BALL, 1, FALSE
	bg_hidden_item_event 37, 7, 3, ITEM_ULTRA_BALL, FLAG_HIDDEN_ITEM_MT_PYRE_SUMMIT_ULTRA_BALL, 1, FALSE
	bg_sign_event 23, 10, 3, BG_EVENT_PLAYER_FACING_NORTH, MtPyre_Summit_EventScript_Darkrai

MtPyre_Summit_MapEvents::
	map_events MtPyre_Summit_ObjectEvents, MtPyre_Summit_MapWarps, MtPyre_Summit_MapCoordEvents, MtPyre_Summit_MapBGEvents

