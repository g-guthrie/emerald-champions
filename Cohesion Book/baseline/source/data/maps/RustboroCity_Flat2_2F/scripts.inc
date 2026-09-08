RustboroCity_Flat2_2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RustboroCity_Flat2_2F_OnTransition
	.byte 0

RustboroCity_Flat2_2F_OnTransition:
	goto_if_set FLAG_ITEM_RUSTBORO_FLOAT_STONE, RustboroCity_Flat2_2F_EventScript_ShowHiker
	setflag FLAG_HIDE_RUSTBORO_FLAT_HIKER
	end

RustboroCity_Flat2_2F_EventScript_ShowHiker:
	clearflag FLAG_HIDE_RUSTBORO_FLAT_HIKER
	removeobject LOCALID_RUSTBORO_FLAT_ACE
	end

RustboroCity_Flat2_2F_EventScript_OldMan::
	msgbox RustboroCity_Flat2_2F_Text_DevonWasTinyInOldDays, MSGBOX_NPC
	end

RustboroCity_Flat2_2F_EventScript_NinjaBoy::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_PREMIER_BALL_RUSTBORO, RustboroCity_Flat2_2F_EventScript_GavePremierBall
	msgbox RustboroCity_Flat2_2F_Text_MyDaddyMadeThisYouCanHaveIt, MSGBOX_DEFAULT
	giveitem ITEM_PREMIER_BALL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_PREMIER_BALL_RUSTBORO
	release
	end

RustboroCity_Flat2_2F_EventScript_GavePremierBall::
	msgbox RustboroCity_Flat2_2F_Text_GoingToWorkAtDevonToo, MSGBOX_DEFAULT
	release
	end

RustboroCity_Flat2_2F_EventScript_GiveFloatStone::
	lock
	faceplayer
	goto_if_set FLAG_ITEM_RUSTBORO_FLOAT_STONE, RustboroCity_Flat2_2F_EventScript_GaveFloatStone
	msgbox RustboroCity_Flat2_2F_Text_MyDadsAceAtDevon, MSGBOX_DEFAULT
	giveitem ITEM_METAL_COAT
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_ITEM_RUSTBORO_FLOAT_STONE
	msgbox RustboroCity_Flat2_2F_Text_PokemonHoldFloatStone, MSGBOX_DEFAULT
	release
	end

RustboroCity_Flat2_2F_EventScript_GaveFloatStone::
	msgbox RustboroCity_Flat2_2F_Text_PokemonHoldFloatStone, MSGBOX_DEFAULT
	release
	end

RustboroCity_Flat2_2F_EventScript_FatMan::
	msgbox RustboroCity_Flat2_2F_Text_PutOnWeight, MSGBOX_NPC
	end

RustboroCity_Flat2_2F_Text_DevonWasTinyInOldDays:
	.string "Way back in the old days, DEVON was just\n"
	.string "a teeny, tiny company.$"

RustboroCity_Flat2_2F_Text_MyDaddyMadeThisYouCanHaveIt:
	.string "My daddy's working at the CORPORATION.\p"
	.string "My daddy made this!\n"
	.string "But I can't use it, so you can have it.$"

RustboroCity_Flat2_2F_Text_GoingToWorkAtDevonToo:
	.string "My daddy's working at the CORPORATION.\p"
	.string "When I grow up, I'm going to work for\n"
	.string "DEVON, too.$"

RustboroCity_Flat2_2F_Text_MyDadsAceAtDevon:
	.string "My dad is a real ace at DEVON.\p"
	.string "Here. Take something an ACE TRAINER\n"
	.string "like me knows how to use.$"

RustboroCity_Flat2_2F_Text_PokemonHoldFloatStone:
	.string "A METAL COAT lets some POKéMON evolve\n"
	.string "when traded. DEVON refines them.$"

RustboroCity_Flat2_2F_Text_PutOnWeight:
	.string "For some reason, I've put on a lot of\n"
	.string "weight recently…$"
