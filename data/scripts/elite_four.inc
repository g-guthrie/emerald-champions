PokemonLeague_EliteFour_SetAdvanceToNextRoomMetatiles::
	applymovement LOCALID_PLAYER, Common_Movement_Delay32
	waitmovement 0
	playse SE_DOOR
	setmetatile 6, 1, METATILE_EliteFour_OpenDoor_Frame, FALSE
	setmetatile 6, 2, METATILE_EliteFour_OpenDoor_Opening, FALSE
	setmetatile 0, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 1, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	setmetatile 2, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 3, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	setmetatile 4, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 8, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	setmetatile 9, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 10, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	setmetatile 11, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 12, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	special DrawWholeMapView
	return

PokemonLeague_EliteFour_EventScript_WalkInCloseDoor::
	applymovement LOCALID_PLAYER, Common_Movement_WalkUp6
	waitmovement 0
	playse SE_TRUCK_DOOR
	setmetatile 5, 12, METATILE_EliteFour_EntryDoor_ClosedTop, TRUE
	setmetatile 6, 12, METATILE_EliteFour_EntryDoor_ClosedTop, TRUE
	setmetatile 7, 12, METATILE_EliteFour_EntryDoor_ClosedTop, TRUE
	setmetatile 5, 13, METATILE_EliteFour_EntryDoor_ClosedBottom, TRUE
	setmetatile 6, 13, METATILE_EliteFour_EntryDoor_ClosedBottom, TRUE
	setmetatile 7, 13, METATILE_EliteFour_EntryDoor_ClosedBottom, TRUE
	special DrawWholeMapView
	return

@ Essentially unused, only necessary when re-entering an Elite Four room after defeating the member, which isnt normally possible
PokemonLeague_EliteFour_EventScript_ResetAdvanceToNextRoom::
	setmetatile 6, 1, METATILE_EliteFour_OpenDoor_Frame, FALSE
	setmetatile 6, 2, METATILE_EliteFour_OpenDoor_Opening, FALSE
	setmetatile 5, 12, METATILE_EliteFour_EntryDoor_ClosedTop, TRUE
	setmetatile 6, 12, METATILE_EliteFour_EntryDoor_ClosedTop, TRUE
	setmetatile 7, 12, METATILE_EliteFour_EntryDoor_ClosedTop, TRUE
	setmetatile 5, 13, METATILE_EliteFour_EntryDoor_ClosedBottom, TRUE
	setmetatile 6, 13, METATILE_EliteFour_EntryDoor_ClosedBottom, TRUE
	setmetatile 7, 13, METATILE_EliteFour_EntryDoor_ClosedBottom, TRUE
	setmetatile 0, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 1, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	setmetatile 2, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 3, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	setmetatile 4, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 8, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	setmetatile 9, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 10, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	setmetatile 11, 2, METATILE_EliteFour_RightSpotlightOff, TRUE
	setmetatile 12, 2, METATILE_EliteFour_LeftSpotlightOff, TRUE
	return

PokemonLeague_EliteFour_EventScript_CloseDoor::
	setmetatile 5, 12, METATILE_EliteFour_EntryDoor_ClosedTop, TRUE
	setmetatile 6, 12, METATILE_EliteFour_EntryDoor_ClosedTop, TRUE
	setmetatile 7, 12, METATILE_EliteFour_EntryDoor_ClosedTop, TRUE
	setmetatile 5, 13, METATILE_EliteFour_EntryDoor_ClosedBottom, TRUE
	setmetatile 6, 13, METATILE_EliteFour_EntryDoor_ClosedBottom, TRUE
	setmetatile 7, 13, METATILE_EliteFour_EntryDoor_ClosedBottom, TRUE
	return

@ Every Emerald Champions League battle is a Double Battle. The doors still
@ lock behind the player and the League still provides no automatic healing,
@ but a player who reaches the next room with fewer than two usable Pokemon
@ must not be trapped there without a Revive. Return TRUE when battle-ready,
@ FALSE when the player chooses to stay and use the Bag, and end the current
@ script through the native whiteout callback when the player retires.
PokemonLeague_EliteFour_EventScript_CheckReadyOrOfferRetire::
	special HasEnoughMonsForDoubleBattle
	goto_if_eq VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS, PokemonLeague_EliteFour_EventScript_Ready
	msgbox PokemonLeague_EliteFour_Text_RetirePrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, PokemonLeague_EliteFour_EventScript_Retire
	msgbox PokemonLeague_EliteFour_Text_Stay, MSGBOX_DEFAULT
	setvar VAR_RESULT, FALSE
	return

PokemonLeague_EliteFour_EventScript_Ready::
	setvar VAR_RESULT, TRUE
	return

PokemonLeague_EliteFour_EventScript_Retire::
	msgbox PokemonLeague_EliteFour_Text_Retired, MSGBOX_DEFAULT
	closemessage
	setrespawn HEAL_LOCATION_EVER_GRANDE_CITY_POKEMON_LEAGUE
	special Script_FadeOutMapMusic
	fadescreen FADE_TO_BLACK
	special SetCB2WhiteOut
	end

PokemonLeague_EliteFour_Text_RetirePrompt:
	.string "You need two battle-ready POKéMON.\p"
	.string "End this LEAGUE run and return to the\n"
	.string "lobby?$"

PokemonLeague_EliteFour_Text_Stay:
	.string "Then restore another POKéMON with your\n"
	.string "BAG and speak to me again.$"

PokemonLeague_EliteFour_Text_Retired:
	.string "Your LEAGUE run ends here. Rest, rebuild,\n"
	.string "and challenge us again.$"
