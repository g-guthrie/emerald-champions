@ BEGIN EMERALD CHAMPIONS POKE VIAL QUEST
AshenWoods_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, AshenWoods_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, AshenWoods_OnTransition
	.byte 0

AshenWoods_OnResume:
	setstepcallback STEP_CB_ASH
	end

AshenWoods_OnTransition:
	setflag FLAG_LANDMARK_ASHEN_WOODS
	setflag FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY
	setflag FLAG_HIDE_ASHEN_WOODS_VIAL_BALL
	call_if_eq VAR_JAGGED_PASS_ASH_WEATHER, 1, AshenWoods_EventScript_SetWeatherAsh
	goto_if_eq VAR_CHANSEY_NURSE_STATE, 3, AshenWoods_EventScript_ShowVialChansey
	goto_if_eq VAR_CHANSEY_NURSE_STATE, 4, AshenWoods_EventScript_ShowVialChanseyPos1
	goto_if_eq VAR_CHANSEY_NURSE_STATE, 5, AshenWoods_EventScript_ShowVialChanseyPos2
	end

AshenWoods_EventScript_SetWeatherAsh::
	setweather WEATHER_VOLCANIC_ASH
	doweather
	return

AshenWoods_EventScript_ShowVialChansey::
	clearflag FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY
	setobjectxyperm LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 14, 29
	end

AshenWoods_EventScript_ShowVialChanseyPos1::
	clearflag FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY
	setobjectxyperm LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 6, 39
	end

AshenWoods_EventScript_ShowVialChanseyPos2::
	clearflag FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY
	setobjectxyperm LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 27, 43
	end

AshenWoods_EventScript_VialChanseyEscape1::
	lockall
	playse SE_PIN
	applymovement LOCALID_ASHEN_WOODS_VIAL_CHANSEY, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_ASHEN_WOODS_VIAL_CHANSEY, AshenWoods_Movement_VialChanseyJumpRight
	waitmovement 0
	playmoncry SPECIES_CHANSEY, CRY_MODE_NORMAL
	delay 40
	waitmoncry
	applymovement LOCALID_ASHEN_WOODS_VIAL_CHANSEY, AshenWoods_Movement_VialChanseyEscape1
	waitmovement 0
	setobjectxy LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 6, 39
	setobjectxyperm LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 6, 39
	setvar VAR_CHANSEY_NURSE_STATE, 4
	msgbox AshenWoods_Text_VialBlobKeepsRunning, MSGBOX_DEFAULT
	releaseall
	end

AshenWoods_EventScript_VialChanseyEscape2::
	lockall
	playse SE_PIN
	applymovement LOCALID_ASHEN_WOODS_VIAL_CHANSEY, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_ASHEN_WOODS_VIAL_CHANSEY, AshenWoods_Movement_VialChanseyJumpUp
	waitmovement 0
	playmoncry SPECIES_CHANSEY, CRY_MODE_NORMAL
	delay 40
	waitmoncry
	applymovement LOCALID_ASHEN_WOODS_VIAL_CHANSEY, AshenWoods_Movement_VialChanseyEscape2
	waitmovement 0
	setobjectxy LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 27, 43
	setobjectxyperm LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 27, 43
	setvar VAR_CHANSEY_NURSE_STATE, 5
	msgbox AshenWoods_Text_VialBlobCornered, MSGBOX_DEFAULT
	releaseall
	end

AshenWoods_EventScript_VialChansey::
	lockall
	faceplayer
	goto_if_eq VAR_CHANSEY_NURSE_STATE, 3, AshenWoods_EventScript_VialApproachFromEast
	goto_if_eq VAR_CHANSEY_NURSE_STATE, 4, AshenWoods_EventScript_VialApproachFromNorth
	goto_if_ne VAR_CHANSEY_NURSE_STATE, 5, AshenWoods_EventScript_VialEndInteraction
	applymovement LOCALID_ASHEN_WOODS_VIAL_CHANSEY, AshenWoods_Movement_VialChanseyJumpRight
	waitmovement 0
	playmoncry SPECIES_CHANSEY, CRY_MODE_NORMAL
	delay 40
	waitmoncry
	msgbox AshenWoods_Text_VialBlobNowhereToRun, MSGBOX_DEFAULT
	checkitem ITEM_HEAL_BALL
	goto_if_eq VAR_RESULT, FALSE, AshenWoods_EventScript_VialNoHealBall
	msgbox AshenWoods_Text_VialCatchBlob, MSGBOX_DEFAULT
	removeitem ITEM_HEAL_BALL
	setflag FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY
	removeobject LOCALID_ASHEN_WOODS_VIAL_CHANSEY
	clearflag FLAG_HIDE_ASHEN_WOODS_VIAL_BALL
	addobject LOCALID_ASHEN_WOODS_VIAL_BALL
	playse SE_BALL_THROW
	waitse
	playse SE_BALL_BOUNCE_1
	waitse
	playse SE_BALL_BOUNCE_2
	waitse
	playse SE_RG_BALL_CLICK
	waitse
	msgbox AshenWoods_Text_VialBlobCaught, MSGBOX_DEFAULT
	setflag FLAG_HIDE_ASHEN_WOODS_VIAL_BALL
	removeobject LOCALID_ASHEN_WOODS_VIAL_BALL
	setvar VAR_CHANSEY_NURSE_STATE, 6
AshenWoods_EventScript_VialEndInteraction::
	releaseall
	end

AshenWoods_EventScript_VialApproachFromEast::
	msgbox AshenWoods_Text_VialApproachFromEast, MSGBOX_DEFAULT
	goto AshenWoods_EventScript_VialEndInteraction

AshenWoods_EventScript_VialApproachFromNorth::
	msgbox AshenWoods_Text_VialApproachFromNorth, MSGBOX_DEFAULT
	goto AshenWoods_EventScript_VialEndInteraction

AshenWoods_EventScript_VialNoHealBall::
	msgbox AshenWoods_Text_VialNoHealBall, MSGBOX_DEFAULT
	releaseall
	end

AshenWoods_Movement_VialChanseyJumpRight:
	jump_in_place_right
	delay_4
	jump_in_place_right
	step_end

AshenWoods_Movement_VialChanseyJumpUp:
	jump_in_place_up
	delay_4
	jump_in_place_up
	step_end

AshenWoods_Movement_VialChanseyEscape1:
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	face_up
	step_end

AshenWoods_Movement_VialChanseyEscape2:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

AshenWoods_Text_VialBlobKeepsRunning:
	.string "Blob disappeared deeper into the ash.\n"
	.string "The chase is not over yet.$"

AshenWoods_Text_VialBlobCornered:
	.string "Blob reached the far edge of the woods.\n"
	.string "There is nowhere else to run.$"

AshenWoods_Text_VialBlobNowhereToRun:
	.string "Blob watches the HEAL BALL.\n"
	.string "This is your chance to bring it home.$"

AshenWoods_Text_VialApproachFromEast:
	.string "Blob keeps looking for an escape.\n"
	.string "Try approaching from the east.$"

AshenWoods_Text_VialApproachFromNorth:
	.string "Blob is still too restless to catch.\n"
	.string "Try the narrow trail to the north.$"

AshenWoods_Text_VialNoHealBall:
	.string "You need one of the nurse's HEAL BALLS.\n"
	.string "Return to her on ROUTE 111 for another.$"

AshenWoods_Text_VialCatchBlob:
	.string "{PLAYER} threw the HEAL BALL at Blob!$"

AshenWoods_Text_VialBlobCaught:
	.string "Click! Blob was caught!\p"
	.string "The nurse is waiting on ROUTE 111.$"
@ END EMERALD CHAMPIONS POKE VIAL QUEST

AshenWoods_EventScript_Alannah::
	trainerbattle_double TRAINER_ALANNAH, AshenWoods_Text_AlannahIntro, AshenWoods_Text_AlannahDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox AshenWoods_Text_AlannahAfter, MSGBOX_AUTOCLOSE
	end
AshenWoods_EventScript_Martin::
	trainerbattle_double TRAINER_MARTIN, AshenWoods_Text_MartinIntro, AshenWoods_Text_MartinDefeat, EmeraldChampions_Text_NeedTwoPokemon
	goto_if_set FLAG_EC_CAUGHT_MOLTRES, AshenWoods_EventScript_MoltresCaught
	msgbox AshenWoods_Text_MartinAfter, MSGBOX_AUTOCLOSE
	end
AshenWoods_EventScript_Roman::
	trainerbattle_double TRAINER_ROMAN, AshenWoods_Text_RomanIntro, AshenWoods_Text_RomanDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox AshenWoods_Text_RomanAfter, MSGBOX_AUTOCLOSE
	end
AshenWoods_EventScript_Elmer::
	trainerbattle_double TRAINER_ELMER, AshenWoods_Text_ElmerIntro, AshenWoods_Text_ElmerDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox AshenWoods_Text_ElmerAfter, MSGBOX_AUTOCLOSE
	end
AshenWoods_EventScript_Caretaker::
	msgbox AshenWoods_Text_Caretaker, MSGBOX_NPC
	end

AshenWoods_Text_AlannahIntro:
	.string "The ash feeds roots tougher than\n"
	.string "stone. Can your team outlast\l"
	.string "mine?$"
AshenWoods_Text_AlannahDefeat:
	.string "Your answer grew through the ash.$"
AshenWoods_Text_AlannahAfter:
	.string "The forest rewards adaptation, not one\n"
	.string "perfect type chart.$"
AshenWoods_Text_MartinIntro:
	.string "A firebird circles whenever the sun\n"
	.string "breaks through. Face the whole blaze!$"
AshenWoods_Text_MartinDefeat:
	.string "You extinguished the wildfire cleanly.$"
AshenWoods_Text_MartinAfter:
	.string "MOLTRES roosts in EMBER PATH.\n"
	.string "Follow the trail and keep your eyes\l"
	.string "up. Those feathers catch the light!$"
AshenWoods_Text_RomanIntro:
	.string "The rocks hiss when rain hits them.\n"
	.string "A little water, all that heat...\l"
	.string "Now watch what my POKéMON can do!$"
AshenWoods_Text_RomanDefeat:
	.string "Whew! You've left us steaming!$"
AshenWoods_Text_RomanAfter:
	.string "I could listen to the forest hiss\n"
	.string "after rain all day. Even the mountain\l"
	.string "needs to cool off sometimes.$"
AshenWoods_Text_ElmerIntro:
	.string "Look closely at the ash. Tiny tracks\n"
	.string "everywhere! My bugs are tougher\l"
	.string "than they look. Care to see?$"
AshenWoods_Text_ElmerDefeat:
	.string "You changed pace with every swarm.$"
AshenWoods_Text_ElmerAfter:
	.string "Some insects burrow when the ash\n"
	.string "falls. Others just shake it off.\l"
	.string "I always find something new here.$"
AshenWoods_Text_Caretaker:
	.string "Heat scorched these woods, but the\n"
	.string "roots survived. See the new leaves?\p"
	.string "With the weather changing so much,\n"
	.string "POKéMON we seldom saw are appearing\l"
	.string "among the ash.\p"
	.string "The old wardens say RAYQUAZA keeps\n"
	.string "land and sea in balance. I hope\l"
	.string "they are right.$"

AshenWoods_EventScript_MoltresCaught::
	msgbox AshenWoods_Text_MoltresCaught, MSGBOX_AUTOCLOSE
	end

AshenWoods_Text_MoltresCaught:
	.string "MOLTRES found a partner in you!\p"
	.string "EMBER PATH still glows at sunset.\n"
	.string "I always look up, out of habit.$"
