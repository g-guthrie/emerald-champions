@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/MeteorFalls_JirachisRoom/map.json
@

	.align 2

MeteorFalls_JirachisRoom_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_INCLEMENT_JIRACHI, 7, 6, 3, MOVEMENT_TYPE_NONE, 0, 0, TRAINER_TYPE_NONE, 0, MeteorFalls_JirachisRoom_EventScript_Jirachi, FLAG_EC_CAUGHT_JIRACHI

MeteorFalls_JirachisRoom_MapWarps:
	warp_def 7, 10, 0, 1, MAP_METEOR_FALLS_B1F_2R

MeteorFalls_JirachisRoom_MapEvents::
	map_events MeteorFalls_JirachisRoom_ObjectEvents, MeteorFalls_JirachisRoom_MapWarps, NULL, NULL

