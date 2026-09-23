.set LOCALID_SAILOR, 1
.set LOCALID_SS_TIDAL, 2

BirthIsland_Harbor_MapScripts:: @ 826805C
	.byte 0

BirthIsland_Harbor_EventScript_Sailor:: @ 826805D
	lock
	faceplayer
	specialvar VAR_RESULT, GetEmeraldChampionsFinaleStage
	goto_if_eq VAR_RESULT, EC_FINALE_DEOXYS, BirthIsland_Harbor_EventScript_PointInland
	call Common_EventScript_FinaleDirections
BirthIsland_Harbor_EventScript_OfferReturn:
	msgbox BirthIsland_Harbor_Text_SailorReturn, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq BirthIsland_Harbor_EventScript_AsYouLike
	msgbox EventTicket_Text_SailHome, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 30
	hideobjectat LOCALID_SAILOR, MAP_BIRTH_ISLAND_HARBOR
	setvar VAR_0x8004, LOCALID_SS_TIDAL
	call Common_EventScript_FerryDepartIsland
	warp MAP_LILYCOVE_CITY_HARBOR, 255, 8, 11
	waitstate
	release
	end

BirthIsland_Harbor_EventScript_AsYouLike:: @ 82680A2
	msgbox EventTicket_Text_AsYouLike, MSGBOX_DEFAULT
	release
	end

@ The shared Deoxys directions start at the Lilycove ferry, but the player is already here.
BirthIsland_Harbor_EventScript_PointInland:
	msgbox BirthIsland_Harbor_Text_TriangleInland, MSGBOX_DEFAULT
	goto BirthIsland_Harbor_EventScript_OfferReturn

BirthIsland_Harbor_Text_TriangleInland:
	.string "The strange triangle is just inland.\n"
	.string "Solve its puzzle, then catch or\l"
	.string "defeat Deoxys.\p"
	.string "If you flee, you can try again.$"

