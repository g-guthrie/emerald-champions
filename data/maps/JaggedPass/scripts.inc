.set LOCALID_HIDEOUT_GUARD, 5
.set LOCALID_CHANSEY, 8

JaggedPass_MapScripts:: @ 8230656
	map_script MAP_SCRIPT_ON_RESUME, JaggedPass_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, JaggedPass_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, JaggedPass_OnLoad
	.byte 0

JaggedPass_OnResume: @ 8230666
	setstepcallback STEP_CB_ASH
	compare VAR_JAGGED_PASS_STATE, 0
	call_if_eq JaggedPass_EventScript_CheckHasMagmaEmblem
	end

JaggedPass_EventScript_CheckHasMagmaEmblem:: @ 8230674
	checkitem ITEM_MAGMA_EMBLEM, 1
	compare VAR_RESULT, TRUE
	goto_if_eq JaggedPass_EventScript_SetReadyToOpenHideout
	return

JaggedPass_EventScript_SetReadyToOpenHideout:: @ 8230685
	setvar VAR_JAGGED_PASS_STATE, 1
	return

JaggedPass_OnTransition: @ 823068B
	compare VAR_JAGGED_PASS_ASH_WEATHER, 1
	call_if_eq JaggedPass_EventScript_SetWeatherAsh
	end

JaggedPass_EventScript_SetWeatherAsh:: @ 8230697
	setweather WEATHER_VOLCANIC_ASH
	doweather
	return

JaggedPass_OnLoad: @ 823069C
	compare VAR_JAGGED_PASS_STATE, 1
	goto_if_le JaggedPass_EventScript_ConcealHideoutEntrance
	end

JaggedPass_EventScript_ConcealHideoutEntrance:: @ 82306A8
	setmetatile 16, 17, METATILE_Lavaridge_RockWall, 1
	setmetatile 16, 18, METATILE_Lavaridge_RockWall, 1
	end

JaggedPass_EventScript_OpenMagmaHideout:: @ 82306BB
	lockall
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 1  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	msgbox JaggedPass_Text_BoulderShakingInResponseToEmblem, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, 1   @ vertical pan
	setvar VAR_0x8005, 1   @ horizontal pan
	setvar VAR_0x8006, 16  @ num shakes
	setvar VAR_0x8007, 3   @ shake delay
	special ShakeCamera
	waitstate
	playse SE_EFFECTIVE
	setmetatile 16, 17, METATILE_Lavaridge_CaveEntrance_Top, 1
	setmetatile 16, 18, METATILE_Lavaridge_CaveEntrance_Bottom, 0
	special DrawWholeMapView
	delay 30
	setvar VAR_JAGGED_PASS_STATE, 2
	waitse
	releaseall
	end

JaggedPass_EventScript_MagmaHideoutGuard:: @ 8230718
	lockall
	goto_if_set FLAG_BEAT_MAGMA_GRUNT_JAGGED_PASS, JaggedPass_EventScript_GuardDefeated
	special HasEnoughMonsForDoubleBattle
	compare VAR_RESULT, PLAYER_HAS_TWO_USABLE_MONS
	goto_if_ne JaggedPass_EventScript_GuardNeedsTwoMons
	waitse
	playse SE_PIN
	applymovement LOCALID_HIDEOUT_GUARD, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_HIDEOUT_GUARD, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_HIDEOUT_GUARD, Common_Movement_FacePlayer
	waitmovement 0
	msgbox JaggedPass_Text_GruntIntro, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_GRUNT_JAGGED_PASS, JaggedPass_Text_GruntDefeat
	setflag FLAG_BEAT_MAGMA_GRUNT_JAGGED_PASS
	applymovement LOCALID_HIDEOUT_GUARD, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

JaggedPass_EventScript_GuardNeedsTwoMons::
	msgbox JaggedPass_Text_GuardNeedsTwoMons, MSGBOX_DEFAULT
	releaseall
	end

JaggedPass_EventScript_GuardDefeated:: @ 8230766
	applymovement LOCALID_HIDEOUT_GUARD, Common_Movement_FacePlayer
	waitmovement 0
	msgbox JaggedPass_Text_GoWhereverYouWant, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_HIDEOUT_GUARD, Common_Movement_FaceOriginalDirection
	waitmovement 0
	releaseall
	end

JaggedPass_EventScript_Chansey::
	lockall
	applymovement LOCALID_CHANSEY, Common_Movement_FacePlayer
	waitmovement 0
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 11
	goto_if_eq JaggedPass_EventScript_ChanseyShove
	applymovement LOCALID_CHANSEY, JaggedPass_Movement_ChanseyJumpRight
	waitmovement 0
	waitse
	playmoncry SPECIES_CHANSEY, 0
	delay 40
	waitmoncry
	applymovement LOCALID_CHANSEY, JaggedPass_Movement_ChanseyEscape
	waitmovement 0
	removeObject LOCALID_CHANSEY
	setvar VAR_CHANSEY_NURSE_STATE, 3
	releaseall
	end

JaggedPass_EventScript_ChanseyShove::
	applymovement LOCALID_CHANSEY, JaggedPass_Movement_ChanseyJumpLeft
	waitmovement 0
	waitse
	playmoncry SPECIES_CHANSEY, 0
	delay 40
	waitmoncry
	applymovement LOCALID_CHANSEY, JaggedPass_Movement_ChanseyShove
	applymovement OBJ_EVENT_ID_PLAYER, JaggedPass_Movement_PlayerShove
	waitmovement 0
	applymovement LOCALID_CHANSEY, JaggedPass_Movement_ChanseyEscape
	waitmovement 0
	removeObject LOCALID_CHANSEY
	setvar VAR_CHANSEY_NURSE_STATE, 3
	releaseall
	end

JaggedPass_Movement_ChanseyJumpRight:
	jump_in_place_right
	delay_4
	jump_in_place_right
	step_end

JaggedPass_Movement_ChanseyJumpLeft:
	jump_in_place_left
	delay_4
	jump_in_place_left
	step_end

JaggedPass_Movement_ChanseyEscape:
	walk_left
	walk_left
	walk_left
	walk_left
	jump_left
	walk_up
	delay_4
	step_end

JaggedPass_Movement_ChanseyShove:
	walk_left
	jump_in_place_down
	walk_left
	walk_left
	walk_left
	jump_left
	walk_up
	delay_4
	step_end

JaggedPass_Movement_PlayerShove:
 	walk_in_place_fastest_up
    lock_facing_direction
	walk_down
	jump_down
	unlock_facing_direction
	step_end

JaggedPass_EventScript_Eric:: @ 8230785
	trainerbattle_single TRAINER_ERIC, JaggedPass_Text_EricIntro, JaggedPass_Text_EricDefeat
	msgbox JaggedPass_Text_EricPostBattle, MSGBOX_AUTOCLOSE
	end

JaggedPass_EventScript_Diana:: @ 823079C
	trainerbattle_double TRAINER_DIANA_1, JaggedPass_Text_DianaIntro, JaggedPass_Text_DianaDefeat, JaggedPass_Text_DianaNeedTwoMons, JaggedPass_EventScript_RegisterDiana
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq JaggedPass_EventScript_DianaRematch
	msgbox JaggedPass_Text_DianaPostBattle, MSGBOX_DEFAULT
	release
	end

JaggedPass_EventScript_RegisterDiana:: @ 82307C8
	special PlayerFaceTrainerAfterBattle
	msgbox JaggedPass_Text_DianaRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_DIANA_1
	release
	end

JaggedPass_EventScript_DianaRematch:: @ 82307E4
	trainerbattle_rematch_double TRAINER_DIANA_1, JaggedPass_Text_DianaRematchIntro, JaggedPass_Text_DianaRematchDefeat, JaggedPass_Text_DianaNeedTwoMons
	msgbox JaggedPass_Text_DianaPostRematch, MSGBOX_AUTOCLOSE
	end

JaggedPass_EventScript_Ethan:: @ 82307FB
	trainerbattle_double TRAINER_ETHAN_1, JaggedPass_Text_EthanIntro, JaggedPass_Text_EthanDefeat, JaggedPass_Text_EthanNeedTwoMons, JaggedPass_EventScript_RegisterEthan
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq JaggedPass_EventScript_EthanRematch
	msgbox JaggedPass_Text_EthanPostBattle, MSGBOX_DEFAULT
	release
	end

JaggedPass_EventScript_RegisterEthan:: @ 8230827
	special PlayerFaceTrainerAfterBattle
	msgbox JaggedPass_Text_EthanRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_ETHAN_1
	release
	end

JaggedPass_EventScript_EthanRematch:: @ 8230843
	trainerbattle_rematch_double TRAINER_ETHAN_1, JaggedPass_Text_EthanRematchIntro, JaggedPass_Text_EthanRematchDefeat, JaggedPass_Text_EthanNeedTwoMons
	msgbox JaggedPass_Text_EthanPostRematch, MSGBOX_AUTOCLOSE
	end

JaggedPass_EventScript_Julio:: @ 823085A
	trainerbattle_single TRAINER_JULIO, JaggedPass_Text_JulioIntro, JaggedPass_Text_JulioDefeat
	msgbox JaggedPass_Text_JulioPostBattle, MSGBOX_AUTOCLOSE
	end

JaggedPass_EventScript_Autumn:: @ 8230871
	trainerbattle_single TRAINER_AUTUMN, JaggedPass_Text_AutumnIntro, JaggedPass_Text_AutumnDefeat
	msgbox JaggedPass_Text_AutumnPostBattle, MSGBOX_AUTOCLOSE
	end

JaggedPass_Text_EricIntro: @ 8230888
	.string "These three harden late!\n"
	.string "Pupitar, Metang, and Hakamo-o still\l"
	.string "have their final forms ahead.\p"
	.string "Jagged Pass is where they earn them!$"

JaggedPass_Text_EricDefeat: @ 82308FF
	.string "Losing left me bitter!$"

JaggedPass_Text_EricPostBattle: @ 8230916
	.string "Pupitar evolves at 55. Metang and\n"
	.string "Hakamo-o evolve at 45.\p"
	.string "Eviolite, two resist Berries, and\n"
	.string "their own strength carry them now.$"

JaggedPass_Text_DianaIntro: @ 8230974
	.string "This jagged picnic needs a plan!\p"
	.string "Delibird chills the basket.\n"
	.string "Vespiquen guards it.\p"
	.string "Gourgeist lights the picnic.\n"
	.string "Eelektross powers every tool!$"

JaggedPass_Text_DianaDefeat: @ 82309B5
	.string "Ohhh, no!\n"
	.string "You broke up the whole course!$"

JaggedPass_Text_DianaPostBattle: @ 82309D8
	.string "Delibird handles tempo.\n"
	.string "Vespiquen brings Intimidate and\l"
	.string "Tailwind.\p"
	.string "Gourgeist burns and drains.\n"
	.string "Eelektross pivots with Volt Switch.$"

JaggedPass_Text_DianaRegister: @ 8230A2C
	.string "Come back for another course!\n"
	.string "I change the menu every rematch.$"

JaggedPass_Text_DianaRematchIntro: @ 8230A76
	.string "The menu changed! Delibird may buy\n"
	.string "Polteageist a Shell Smash turn,\l"
	.string "or I may serve a control course.\p"
	.string "Let's see what you can digest!$"

JaggedPass_Text_DianaRematchDefeat: @ 8230AAA
	.string "You cleared every plate again!$"

JaggedPass_Text_DianaPostRematch: @ 8230AD7
	.string "Fake Out can shelter Shell Smash.\n"
	.string "Tailwind or Icy Wind changes tempo,\l"
	.string "and my final menu mixes all six.$"

JaggedPass_Text_DianaNeedTwoMons:
	.string "A full picnic needs two healthy\n"
	.string "Pokémon. Come back with a partner!$"

JaggedPass_Text_EthanIntro: @ 8230B10
	.string "I found a rhythm for this trail!\p"
	.string "Toxtricity plays loud because\n"
	.string "Soundproof Bouffalant can take it.\p"
	.string "Rillaboom and Mr. Rime keep time!$"

JaggedPass_Text_EthanDefeat: @ 8230B50
	.string "You broke my rhythm!$"

JaggedPass_Text_EthanPostBattle: @ 8230B93
	.string "Boomburst hits foes and my ally.\n"
	.string "Soundproof shields Bouffalant.\p"
	.string "Overdrive is the safer refrain when\n"
	.string "another partner takes the stage.$"

JaggedPass_Text_EthanRegister: @ 8230BC6
	.string "I'll learn a new verse every climb.\n"
	.string "Register me for the next rematch!$"

JaggedPass_Text_EthanRematchIntro: @ 8230C28
	.string "The chorus changed!\n"
	.string "The drum and dancer may lead,\l"
	.string "or the bugs may take the melody.\p"
	.string "My final verse uses every voice!$"

JaggedPass_Text_EthanRematchDefeat: @ 8230C60
	.string "You silenced the whole chorus!$"

JaggedPass_Text_EthanPostRematch: @ 8230C94
	.string "Rillaboom brings Grassy Surge.\n"
	.string "Mr. Rime erases screens.\p"
	.string "Yanmega gains Speed while Kricketune\n"
	.string "waits for Swarm. Listen, then move!$"

JaggedPass_Text_EthanNeedTwoMons:
	.string "A full chorus needs two healthy\n"
	.string "Pokémon. Bring another voice!$"

JaggedPass_Text_GruntIntro: @ 8230CCB
	.string "Wah! You found the wrong footing!\n"
	.string "This pass swallows intruders whole.\p"
	.string "We pin you down, punish escape,\n"
	.string "then vanish before you hit back!$"

JaggedPass_Text_GruntDefeat: @ 8230D2D
	.string "Urrrgh…\p"
	.string "You read every retreat.\p"
	.string "I should've ducked into our Hideout\n"
	.string "right away…$"

JaggedPass_Text_GoWhereverYouWant: @ 8230D65
	.string "Okay, oh-kay! You're strong!\p"
	.string "Stunfisk held the ground.\n"
	.string "Thievul punished your switches.\p"
	.string "Golisopod retreated.\n"
	.string "Dugtrio sealed the exits.\p"
	.string "Go wherever you want!$"

JaggedPass_Text_GuardNeedsTwoMons:
	.string "This guard drill needs two healthy\n"
	.string "Pokémon. Come back prepared!$"

JaggedPass_Text_BoulderShakingInResponseToEmblem: @ 8230DBA
	.string "Oh! This boulder is shaking in response\n"
	.string "to the Magma Emblem!$"

JaggedPass_Text_JulioIntro: @ 8230DF7
	.string "Downhill riding is all momentum!\p"
	.string "Dragapult, Jolteon, and Dodrio\n"
	.string "never touch the brakes!$"

JaggedPass_Text_JulioDefeat: @ 8230E38
	.string "I hit the brakes too late!$"

JaggedPass_Text_JulioPostBattle: @ 8230E57
	.string "Choice Specs, Life Orb, Choice Band:\n"
	.string "every leg trades safety for speed.\p"
	.string "Autumn's strength gives my speed\n"
	.string "a safe line in a double battle.$"

JaggedPass_Text_AutumnIntro: @ 8230E8E
	.string "I climb this hill every day.\n"
	.string "Strength starts with sure footing!\p"
	.string "Chesnaught braces.\n"
	.string "Conkeldurr drives forward.\p"
	.string "Avalugg refuses to slide!$"

JaggedPass_Text_AutumnDefeat: @ 8230ECD
	.string "My footing gave way!$"

JaggedPass_Text_AutumnPostBattle: @ 8230EE3
	.string "Sturdy lets Chesnaught hold once.\n"
	.string "Avalugg does the same.\p"
	.string "Flame Orb wakes Conkeldurr's Guts.\p"
	.string "I anchor Julio's speed in a pair.$"
