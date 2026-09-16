Underwater_SeafloorCavern_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Underwater_SeafloorCavern_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, Underwater_SeafloorCavern_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, Underwater_SeafloorCavern_OnLoad
	.byte 0

Underwater_SeafloorCavern_OnTransition:
	setflag FLAG_LANDMARK_SEAFLOOR_CAVERN
	goto_if_set FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, Underwater_SeafloorCavern_EventScript_HideSubmarine
	end

Underwater_SeafloorCavern_EventScript_HideSubmarine::
	setflag FLAG_HIDE_UNDERWATER_SEA_FLOOR_CAVERN_STOLEN_SUBMARINE
	end

Underwater_SeafloorCavern_OnLoad:
	call_if_set FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, Underwater_SeafloorCavern_EventScript_SetSubmarineGoneMetatiles
	end

Underwater_SeafloorCavern_EventScript_SetSubmarineGoneMetatiles::
	setmetatile 5, 3, METATILE_Underwater_RockWall, TRUE
	setmetatile 6, 3, METATILE_Underwater_RockWall, TRUE
	setmetatile 7, 3, METATILE_Underwater_RockWall, TRUE
	setmetatile 8, 3, METATILE_Underwater_RockWall, TRUE
	setmetatile 5, 4, METATILE_Underwater_FloorShadow, FALSE
	setmetatile 6, 4, METATILE_Underwater_FloorShadow, FALSE
	setmetatile 7, 4, METATILE_Underwater_FloorShadow, FALSE
	setmetatile 8, 4, METATILE_Underwater_FloorShadow, FALSE
	setmetatile 5, 5, METATILE_Underwater_FloorShadow, FALSE
	setmetatile 6, 5, METATILE_Underwater_FloorShadow, FALSE
	setmetatile 7, 5, METATILE_Underwater_FloorShadow, FALSE
	setmetatile 8, 5, METATILE_Underwater_FloorShadow, FALSE
	return

Underwater_SeafloorCavern_OnResume:
	setdivewarp MAP_SEAFLOOR_CAVERN_ENTRANCE, 10, 17
	end

Underwater_SeafloorCavern_EventScript_CheckStolenSub::
	lock
	msgbox Underwater_SeafloorCavern_Text_SubExplorer1, MSGBOX_SIGN
	goto_if_set FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, Underwater_SeafloorCavern_EventScript_CheckStolenSub_Done
	goto_if_set FLAG_EC_MEGA_GIFT_TATSUGIRINITE, Underwater_SeafloorCavern_EventScript_CheckStolenSub_Done
	msgbox Underwater_SeafloorCavern_Text_StoneInIntake, MSGBOX_DEFAULT
	call Underwater_SeafloorCavern_EventScript_MegaGift_TATSUGIRINITE
Underwater_SeafloorCavern_EventScript_CheckStolenSub_Done::
	release
	end

@ Shared giver: also called from SlateportCity_Harbor's Capt. Stern as the
@ fallback once the submarine story is over, if the player never examined
@ the hull while it was still here.
Underwater_SeafloorCavern_EventScript_MegaGift_TATSUGIRINITE::
	goto_if_set FLAG_EC_MEGA_GIFT_TATSUGIRINITE, Underwater_SeafloorCavern_EventScript_MegaGift_TATSUGIRINITE_Done
	giveuniqueitem ITEM_TATSUGIRINITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_TATSUGIRINITE
Underwater_SeafloorCavern_EventScript_MegaGift_TATSUGIRINITE_Done::
	return

Underwater_SeafloorCavern_Text_SubExplorer1:
	.string "“SUBMARINE EXPLORER 1” is painted\n"
	.string "on the hull.\p"
	.string "This is the submarine TEAM AQUA\n"
	.string "stole in SLATEPORT!\p"
	.string "TEAM AQUA must have gone\n"
	.string "ashore here.$"

Underwater_SeafloorCavern_Text_StoneInIntake:
	.string "A small stone is lodged in the\n"
	.string "intake, loose enough to pull\l"
	.string "free.$"

