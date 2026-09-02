Route117_PokemonDayCare_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route117_PokemonDayCare_OnTransition
	.byte 0

Route117_PokemonDayCare_OnTransition:
	setflag FLAG_LANDMARK_POKEMON_DAYCARE
	end

Route117_PokemonDayCare_EventScript_TogepiEgg::
	lock
	faceplayer
	goto_if_set FLAG_EC_RECEIVED_DAYCARE_TOGEPI_EGG, Route117_PokemonDayCare_EventScript_TogepiEggAfter
	msgbox Route117_PokemonDayCare_Text_TogepiEggOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route117_PokemonDayCare_EventScript_TogepiEggDeclined
	specialvar VAR_RESULT, CalculatePlayerPartyCount
	goto_if_ge VAR_RESULT, PARTY_SIZE, Route117_PokemonDayCare_EventScript_TogepiEggNoRoom
	giveegg SPECIES_TOGEPI
	setflag FLAG_EC_RECEIVED_DAYCARE_TOGEPI_EGG
	msgbox Route117_PokemonDayCare_Text_TogepiEggReceived, MSGBOX_DEFAULT
	goto Route117_PokemonDayCare_EventScript_TryGiveOvalCharm

Route117_PokemonDayCare_EventScript_TryGiveOvalCharm::
	goto_if_set FLAG_EC_RECEIVED_OVAL_CHARM, Route117_PokemonDayCare_EventScript_TogepiEggEnd
	msgbox Route117_PokemonDayCare_Text_OvalCharmGift, MSGBOX_DEFAULT
	giveitem ITEM_OVAL_CHARM
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_RECEIVED_OVAL_CHARM
Route117_PokemonDayCare_EventScript_TogepiEggEnd::
	release
	end

Route117_PokemonDayCare_EventScript_TogepiEggAfter::
	msgbox Route117_PokemonDayCare_Text_TogepiEggAfter, MSGBOX_DEFAULT
	goto Route117_PokemonDayCare_EventScript_TryGiveOvalCharm

Route117_PokemonDayCare_EventScript_TogepiEggDeclined::
	msgbox Route117_PokemonDayCare_Text_TogepiEggDeclined, MSGBOX_DEFAULT
	release
	end

Route117_PokemonDayCare_EventScript_TogepiEggNoRoom::
	msgbox Route117_PokemonDayCare_Text_TogepiEggNoRoom, MSGBOX_DEFAULT
	release
	end

Route117_PokemonDayCare_Text_TogepiEggOffer:
	.string "The DAY CARE found an unusual EGG.\p"
	.string "It needs a TRAINER who understands\n"
	.string "support POKéMON. Will you take it?$"

Route117_PokemonDayCare_Text_TogepiEggReceived:
	.string "Take good care of TOGEPI. Its strength\n"
	.string "comes from helping a partner.$"

Route117_PokemonDayCare_Text_TogepiEggAfter:
	.string "Breeding still reveals EGG moves and\n"
	.string "unusual partners, even without grind.$"

Route117_PokemonDayCare_Text_TogepiEggDeclined:
	.string "The EGG will be safe here until you\n"
	.string "change your mind.$"

Route117_PokemonDayCare_Text_TogepiEggNoRoom:
	.string "Your party is full. Make one open slot,\n"
	.string "then return for the EGG.$"

Route117_PokemonDayCare_Text_OvalCharmGift:
	.string "Take this OVAL CHARM as well. EGGS\n"
	.string "turn up far more often with it.$"
