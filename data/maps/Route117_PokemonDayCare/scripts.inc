Route117_PokemonDayCare_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route117_PokemonDayCare_OnTransition
	.byte 0

Route117_PokemonDayCare_OnTransition:
	setflag FLAG_LANDMARK_POKEMON_DAYCARE
	end

Route117_PokemonDayCare_EventScript_TogepiEgg::
	lock
	faceplayer
	call_if_set FLAG_EC_HATCHED_DAYCARE_EGG, Route117_PokemonDayCare_EventScript_HatchReward
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
	goto_if_set FLAG_RECEIVED_WINSTRATE_KANGASKHANITE, Route117_PokemonDayCare_EventScript_RewardAfter
	goto_if_set FLAG_ITEM_ROUTE_120_KANGASKHANITE, Route117_PokemonDayCare_EventScript_RewardAfter
	msgbox Route117_PokemonDayCare_Text_TogepiEggAfter, MSGBOX_DEFAULT
	goto Route117_PokemonDayCare_EventScript_TryGiveOvalCharm

Route117_PokemonDayCare_EventScript_RewardAfter::
	msgbox Route117_PokemonDayCare_Text_RewardAfter, MSGBOX_DEFAULT
	goto Route117_PokemonDayCare_EventScript_TryGiveOvalCharm

Route117_PokemonDayCare_EventScript_HatchReward::
	goto_if_set FLAG_RECEIVED_WINSTRATE_KANGASKHANITE, Route117_PokemonDayCare_EventScript_HatchRewardReturn
	goto_if_set FLAG_ITEM_ROUTE_120_KANGASKHANITE, Route117_PokemonDayCare_EventScript_HatchRewardReturn
	msgbox Route117_PokemonDayCare_Text_HatchReward, MSGBOX_DEFAULT
	giveuniqueitem ITEM_KANGASKHANITE
	goto_if_eq VAR_RESULT, FALSE, Route117_PokemonDayCare_EventScript_HatchRewardBagFull
	setflag FLAG_RECEIVED_WINSTRATE_KANGASKHANITE
	setflag FLAG_ITEM_ROUTE_120_KANGASKHANITE
Route117_PokemonDayCare_EventScript_HatchRewardReturn::
	return

Route117_PokemonDayCare_EventScript_HatchRewardBagFull::
	msgbox Route117_PokemonDayCare_Text_HatchRewardBagFull, MSGBOX_DEFAULT
	return

Route117_PokemonDayCare_EventScript_TogepiEggDeclined::
	msgbox Route117_PokemonDayCare_Text_TogepiEggDeclined, MSGBOX_DEFAULT
	release
	end

Route117_PokemonDayCare_EventScript_TogepiEggNoRoom::
	msgbox Route117_PokemonDayCare_Text_TogepiEggNoRoom, MSGBOX_DEFAULT
	release
	end

Route117_PokemonDayCare_Text_TogepiEggOffer:
	.string "MEGA KANGASKHAN fights with its child.\n"
	.string "Their Ability is PARENTAL BOND.\p"
	.string "Leave two compatible adult POKéMON\n"
	.string "here, then hatch the EGG they produce.\p"
	.string "I'll give you a KANGASKHANITE!\n"
	.string "DITTO in the grass can help you breed.\p"
	.string "Want this TOGEPI EGG as a gift?\n"
	.string "This gift won't count for the stone.$"

Route117_PokemonDayCare_Text_TogepiEggReceived:
	.string "Take TOGEPI along on your adventure.\p"
	.string "For the stone, hatch an EGG produced\n"
	.string "by two POKéMON you leave here.$"

Route117_PokemonDayCare_Text_TogepiEggAfter:
	.string "Leave two compatible POKéMON\n"
	.string "here, then hatch the EGG they\l"
	.string "produce.\p"
	.string "One is enough for a\n"
	.string "KANGASKHANITE! Gift EGGS,\l"
	.string "including TOGEPI, don't count.$"

Route117_PokemonDayCare_Text_HatchReward:
	.string "You raised an EGG from two parents!\n"
	.string "Now you've helped their family grow.\p"
	.string "KANGASKHAN's child grows brave enough\n"
	.string "to fight beside its mother.\p"
	.string "Their PARENTAL BOND lets the child\n"
	.string "follow up its mother's attacks.\p"
	.string "This KANGASKHANITE is yours. Keep it\n"
	.string "for a KANGASKHAN from the SAFARI ZONE.$"

Route117_PokemonDayCare_Text_RewardAfter:
	.string "KANGASKHAN and its child battle\n"
	.string "together when it Mega Evolves.\p"
	.string "You'll find KANGASKHAN in the\n"
	.string "SAFARI ZONE. Take good care of them!$"

Route117_PokemonDayCare_Text_HatchRewardBagFull:
	.string "Make room in your BAG. I'll keep\n"
	.string "your KANGASKHANITE safe until then.$"

Route117_PokemonDayCare_Text_TogepiEggDeclined:
	.string "The EGG will be safe here until you\n"
	.string "change your mind.$"

Route117_PokemonDayCare_Text_TogepiEggNoRoom:
	.string "Your party is full. Make one open slot,\n"
	.string "then return for the EGG.$"

Route117_PokemonDayCare_Text_OvalCharmGift:
	.string "If you ever want to breed POKéMON,\n"
	.string "this OVAL CHARM helps EGGS turn up.$"
