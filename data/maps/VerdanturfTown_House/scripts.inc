VerdanturfTown_House_MapScripts::
	.byte 0

VerdanturfTown_House_EventScript_Woman1::
	msgbox VerdanturfTown_House_Text_TrainersGatherAtPokemonLeague, MSGBOX_NPC
	end

VerdanturfTown_House_EventScript_Woman2::
	msgbox VerdanturfTown_House_Text_DefeatEliteFourInARow, MSGBOX_NPC
	end

VerdanturfTown_House_Text_TrainersGatherAtPokemonLeague:
	.string "Far away, deep in EVER GRANDE CITY,\n"
	.string "is the POKéMON LEAGUE.\p"
	.string "The TRAINERS who gather there are\n"
	.string "all frighteningly well skilled.$"

VerdanturfTown_House_Text_DefeatEliteFourInARow:
	.string "The ELITE FOUR face challengers\n"
	.string "one after another, with the\l"
	.string "CHAMPION waiting beyond them.\p"
	.string "Imagine standing at that first\n"
	.string "door! I'd need a moment to catch\l"
	.string "my breath.$"


VerdanturfTown_House_EventScript_MegaGift_GALLADITE::
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, VerdanturfTown_House_EventScript_Woman2
	goto_if_set FLAG_EC_MEGA_GIFT_GALLADITE, VerdanturfTown_House_EventScript_Woman2
	lock
	faceplayer
	msgbox VerdanturfTown_House_EventScript_MegaGift_GALLADITE_Text, MSGBOX_DEFAULT
	giveuniqueitem ITEM_GALLADITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_MEGA_GIFT_GALLADITE
	release
	goto VerdanturfTown_House_EventScript_Woman2

VerdanturfTown_House_EventScript_MegaGift_GALLADITE_Text:
	.string "Strength can be quiet. A Gallade\n"
	.string "uses its blades to protect others.\p"
	.string "I think this stone belongs with\n"
	.string "someone who understands that.$"
