Route111_Text_MakingRoomUseTMToMakeYourOwn: @ 82762C9
	.string "What's that?\n"
	.string "What am I doing?\p"
	.string "I'm thinking about making my own room\n"
	.string "here using a Pokémon move.\p"
	.string "A Rare Candy will help you prepare.\n"
	.string "Would you like it?$"

Route111_Text_ExplainSecretPower: @ 827636E
	.string "A Pokémon that knows Secret Power\n"
	.string "can open a Secret Base.\p"
	.string "Try a large tree with hanging vines,\n"
	.string "or a rock wall with a small indent.\p"
	.string "If you want Secret Power for battles,\n"
	.string "a Pokémon Center tutor can teach it\l"
	.string "to a compatible partner.\p"
	.string "I'm off to find a place of my own!$"

Route111_Text_DontWantThis: @ 827655C
	.string "Oh, you don't want this?\n"
	.string "If you change your mind, tell me, okay?$"

Route111_Text_DontHaveAnyRoom: @ 827659D
	.string "Oh, you don't have any room for this.\p"
	.string "I'll hold on to it, so come back for it\n"
	.string "another time, okay?$"

Route111_EventScript_SecretPowerMan:: @ 82765FF
	lock
	faceplayer
	msgbox Route111_Text_MakingRoomUseTMToMakeYourOwn, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route111_EventScript_GiveSecretPower
	msgbox Route111_Text_DontWantThis, MSGBOX_DEFAULT
	release
	end

Route111_EventScript_GiveSecretPower:: @ 827661E
	giveitem ITEM_RARE_CANDY
	compare VAR_RESULT, FALSE
	goto_if_eq Route111_EventScript_NoRoomForSecretPower
	msgbox Route111_Text_ExplainSecretPower, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_RECEIVED_SECRET_POWER
	compare VAR_FACING, DIR_WEST
	call_if_eq Route111_EventScript_SecretPowerManExit
	compare VAR_FACING, DIR_EAST
	call_if_eq Route111_EventScript_SecretPowerManExit
	compare VAR_FACING, DIR_NORTH
	call_if_eq Route111_EventScript_SecretPowerManExitNorth
	removeobject VAR_LAST_TALKED
	release
	end

Route111_EventScript_SecretPowerManExit:: @ 827666A
	applymovement VAR_LAST_TALKED, Route111_Movement_SecretPowerManExit
	waitmovement 0
	return

Route111_EventScript_SecretPowerManExitNorth:: @ 8276675
	applymovement VAR_LAST_TALKED, Route111_Movement_SecretPowerManExitNorth
	waitmovement 0
	return

Route111_EventScript_NoRoomForSecretPower:: @ 8276680
	msgbox Route111_Text_DontHaveAnyRoom, MSGBOX_DEFAULT
	release
	end

Route111_Movement_SecretPowerManExit: @ 827668A
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	step_end

Route111_Movement_SecretPowerManExitNorth: @ 8276696
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	step_end
