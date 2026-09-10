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
	end

AshenWoods_EventScript_ShowVialChanseyPos1::
	clearflag FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY
	setobjectxy LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 6, 40
	copyobjectxytoperm LOCALID_ASHEN_WOODS_VIAL_CHANSEY
	end

AshenWoods_EventScript_ShowVialChanseyPos2::
	clearflag FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY
	setobjectxy LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 27, 43
	copyobjectxytoperm LOCALID_ASHEN_WOODS_VIAL_CHANSEY
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
	setobjectxy LOCALID_ASHEN_WOODS_VIAL_CHANSEY, 6, 40
	copyobjectxytoperm LOCALID_ASHEN_WOODS_VIAL_CHANSEY
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
	copyobjectxytoperm LOCALID_ASHEN_WOODS_VIAL_CHANSEY
	setvar VAR_CHANSEY_NURSE_STATE, 5
	msgbox AshenWoods_Text_VialBlobCornered, MSGBOX_DEFAULT
	releaseall
	end

AshenWoods_EventScript_VialChansey::
	lockall
	faceplayer
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
	releaseall
	end

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
	.string "The ash feeds roots tougher than stone.\n"
	.string "Can your team outlast mine?$"
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
	.string "MOLTRES waits beyond EMBER PATH.\n"
	.string "Strength alone will not reach it.$"
AshenWoods_Text_RomanIntro:
	.string "One drop of water can wake a mountain.\n"
	.string "Interrupt the engine if you can!$"
AshenWoods_Text_RomanDefeat:
	.string "You stopped the engine before it ran.$"
AshenWoods_Text_RomanAfter:
	.string "The best doubles plans are visible.\n"
	.string "The hard part is answering in time.$"
AshenWoods_Text_ElmerIntro:
	.string "Every insect here wins differently.\n"
	.string "Show me four different answers!$"
AshenWoods_Text_ElmerDefeat:
	.string "You changed pace with every swarm.$"
AshenWoods_Text_ElmerAfter:
	.string "A Champion needs a toolbox, not a\n"
	.string "single unbeatable hammer.$"
AshenWoods_Text_Caretaker:
	.string "GROUDON's heat burned this place.\n"
	.string "Rain returned it to life.\p"
	.string "The old wardens say RAYQUAZA is not\n"
	.string "a conqueror, but Hoenn's balance.$"
