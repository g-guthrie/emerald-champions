RustboroCity_Gym_MapScripts:: @ 8212F30
	.byte 0

RustboroCity_Gym_EventScript_Roxanne::
	goto_if_set FLAG_DAILY_REMATCH_ROXANNE, RustboroCity_Gym_EventScript_AlreadyRematched
	goto_if_set FLAG_SYS_GAME_CLEAR, RustboroCity_Gym_EventScript_RoxanneRematch
	trainerbattle_double TRAINER_ROXANNE_1, RustboroCity_Gym_Text_RoxanneIntro, RustboroCity_Gym_Text_RoxanneDefeat, RustboroCity_Gym_Text_RoxanneNotEnoughPokemon, RustboroCity_Gym_EventScript_RoxanneDefeated, NO_MUSIC
	goto_if_eq RustboroCity_Gym_EventScript_RoxanneRematch
	goto_if_unset FLAG_RECEIVED_TM39, RustboroCity_Gym_EventScript_GiveRockTomb
	msgbox RustboroCity_Gym_Text_RoxannePostBattle, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_RoxanneDefeated:: @ 8212F66
	message RustboroCity_Gym_Text_ReceivedStoneBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox RustboroCity_Gym_Text_StoneBadgeInfoTakeThis, MSGBOX_DEFAULT
	setflag FLAG_DEFEATED_RUSTBORO_GYM
	setflag FLAG_BADGE01_GET
	setvar VAR_RUSTBORO_CITY_STATE, 2
	addvar VAR_PETALBURG_GYM_STATE, 1
	setvar VAR_0x8008, 1
	call Common_EventScript_SetGymTrainers
	compare VAR_PETALBURG_GYM_STATE, 6
	call_if_eq Common_EventScript_ReadyPetalburgGymForBattle
	goto RustboroCity_Gym_EventScript_GiveRockTomb
	end

RustboroCity_Gym_EventScript_GiveRockTomb:: @ 8212FA4
	giveitem ITEM_EXPERT_BELT
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM39
	msgbox RustboroCity_Gym_Text_ExplainRockTomb, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_RoxanneRematch::
	trainerbattle_no_intro TRAINER_ROXANNE_5, RustboroCity_Gym_Text_RoxanneRematchDefeat
	setflag FLAG_DAILY_REMATCH_ROXANNE
	releaseall
	end

RustboroCity_Gym_EventScript_SetSingles::
	msgbox RustboroCity_Gym_Text_RoxanneAllowLegends, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq RustboroCity_Gym_EventScript_RoxanneSinglesRematch
	trainerbattle_no_intro TRAINER_ROXANNE_3, RustboroCity_Gym_Text_RoxanneRematchDefeat
	setflag FLAG_DAILY_REMATCH_ROXANNE
	releaseall
	end

RustboroCity_Gym_EventScript_SetDoubles::
	msgbox RustboroCity_Gym_Text_RoxanneAllowLegends, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq RustboroCity_Gym_EventScript_RoxanneDoublesRematch
	trainerbattle_no_intro TRAINER_ROXANNE_5, RustboroCity_Gym_Text_RoxanneRematchDefeat
	setflag FLAG_DAILY_REMATCH_ROXANNE
	releaseall
	end

RustboroCity_Gym_EventScript_RoxanneSinglesRematch::
	trainerbattle_no_intro TRAINER_ROXANNE_2, RustboroCity_Gym_Text_RoxanneRematchDefeat
	setflag FLAG_DAILY_REMATCH_ROXANNE
	releaseall
	end

RustboroCity_Gym_EventScript_RoxanneDoublesRematch::
	trainerbattle_no_intro TRAINER_ROXANNE_4, RustboroCity_Gym_Text_RoxanneRematchDefeat
	setflag FLAG_DAILY_REMATCH_ROXANNE
	releaseall
	end

RustboroCity_Gym_EventScript_AlreadyRematched::
	msgbox RustboroCity_Gym_Text_RoxannePostRematch, MSGBOX_AUTOCLOSE
	releaseall
	end

RustboroCity_Gym_EventScript_Exit::
	releaseall
	end

RustboroCity_Gym_EventScript_Josh:: @ 8212FE3
	trainerbattle_double TRAINER_JOSH, RustboroCity_Gym_Text_JoshIntro, RustboroCity_Gym_Text_JoshDefeat, RustboroCity_Gym_Text_JoshNotEnoughPokemon
	msgbox RustboroCity_Gym_Text_JoshPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Tommy:: @ 8212FFA
	trainerbattle_double TRAINER_TOMMY, RustboroCity_Gym_Text_TommyIntro, RustboroCity_Gym_Text_TommyDefeat, RustboroCity_Gym_Text_TommyNotEnoughPokemon
	msgbox RustboroCity_Gym_Text_TommyPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Marc:: @ 8213011
	trainerbattle_single TRAINER_MARC, RustboroCity_Gym_Text_MarcIntro, RustboroCity_Gym_Text_MarcDefeat
	msgbox RustboroCity_Gym_Text_MarcPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_GymGuide:: @ 8213028
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_RUSTBORO_GYM, RustboroCity_Gym_EventScript_GymGuidePostVictory
	msgbox RustboroCity_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_GymGuidePostVictory:: @ 821303D
	msgbox RustboroCity_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_LeftGymStatue:: @ 8213047
	lockall
	goto_if_set FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GymStatueCertified
	goto RustboroCity_Gym_EventScript_GymStatue
	end

RustboroCity_Gym_EventScript_RightGymStatue:: @ 8213057
	lockall
	goto_if_set FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GymStatueCertified
	goto RustboroCity_Gym_EventScript_GymStatue
	end

RustboroCity_Gym_EventScript_GymStatueCertified:: @ 8213067
	msgbox RustboroCity_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_Gym_EventScript_GymStatue:: @ 8213071
	msgbox RustboroCity_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_Gym_EventScript_RegisterRoxanne:: @ 821307B
	lockall
	pokenavcall RustboroCity_Gym_Text_RoxanneRegisterCall
	waitmessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox RustboroCity_Gym_Text_RegisteredRoxanne, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_ROXANNE_MATCH_CALL
	clearflag FLAG_ENABLE_ROXANNE_FIRST_CALL
	releaseall
	end

Rustboro_Gym_Guide_Trigger1::
	lockall
	setvar VAR_TEMP_1, 1
	playse SE_PIN
	applymovement 4, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 4, Common_Movement_Delay48
	waitmovement 0
	applymovement 4, Rustboro_Gym_Guide_Move1
	waitmovement 0
	goto Rustboro_Gym_Guide_Speech

Rustboro_Gym_Guide_Trigger2::
	lockall
	setvar VAR_TEMP_1, 2
	playse SE_PIN
	applymovement 4, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 4, Common_Movement_Delay48
	waitmovement 0
	applymovement 4, Rustboro_Gym_Guide_Move2
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Rustboro_Gym_Player_Move2
	waitmovement 0
	goto Rustboro_Gym_Guide_Speech

Rustboro_Gym_Guide_Trigger3::
	lockall
	setvar VAR_TEMP_1, 3
	playse SE_PIN
	applymovement 4, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 4, Common_Movement_Delay48
	waitmovement 0
	applymovement 4, Rustboro_Gym_Guide_Move3
	waitmovement 0
	goto Rustboro_Gym_Guide_Speech

Rustboro_Gym_Guide_Trigger4::
	lockall
	setvar VAR_TEMP_1, 4
	playse SE_PIN
	applymovement 4, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement 4, Common_Movement_Delay48
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Rustboro_Gym_Player_Move_Face
	applymovement 4, Rustboro_Gym_Guide_Move4
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, Rustboro_Gym_Player_Move4
	waitmovement 0
	goto Rustboro_Gym_Guide_Speech

Rustboro_Gym_Guide_Speech::
	msgbox RustboroCity_Gym_Text_Tips, MSGBOX_DEFAULT
	giveitem ITEM_FRESH_WATER, 5
	setvar VAR_RUSTBORO_CITY_STATE, 1
	compare VAR_TEMP_1, 1
	goto_if_eq Rustboro_Gym_Guide_Walk_Away1
	compare VAR_TEMP_1, 2
	goto_if_eq Rustboro_Gym_Guide_Walk_Away2
	compare VAR_TEMP_1, 3
	goto_if_eq Rustboro_Gym_Guide_Walk_Away3
	compare VAR_TEMP_1, 4
	goto_if_eq Rustboro_Gym_Guide_Walk_Away4
	releaseall
	end

Rustboro_Gym_Guide_Walk_Away1::
	applymovement 4, Rustboro_Gym_Guide_Move5
	waitmovement 0
	releaseall
	end

Rustboro_Gym_Guide_Walk_Away2::
	applymovement 4, Rustboro_Gym_Guide_Move6
	waitmovement 0
	releaseall
	end

Rustboro_Gym_Guide_Walk_Away3::
	applymovement 4, Rustboro_Gym_Guide_Move7
	waitmovement 0
	releaseall
	end

Rustboro_Gym_Guide_Walk_Away4::
	applymovement 4, Rustboro_Gym_Guide_Move8
	waitmovement 0
	releaseall
	end

Rustboro_Gym_Guide_Move1::
	walk_down
	face_right
	step_end

Rustboro_Gym_Guide_Move2::
	walk_right
	step_end

Rustboro_Gym_Guide_Move3::
	walk_right
	walk_right
	step_end

Rustboro_Gym_Guide_Move4::
	walk_right
	walk_right
	walk_right
	walk_right
	face_down
	step_end

Rustboro_Gym_Player_Move2::
	face_left
	step_end

Rustboro_Gym_Player_Move4::
	face_up
	step_end

Rustboro_Gym_Player_Move_Face::
	face_left
	step_end

Rustboro_Gym_Guide_Move5::
	walk_up
	face_down
	step_end

Rustboro_Gym_Guide_Move6::
	walk_left
	face_down
	step_end

Rustboro_Gym_Guide_Move7::
	walk_left
	walk_left
	face_down
	step_end

Rustboro_Gym_Guide_Move8::
	walk_left
	walk_left
	walk_left
	walk_left
	face_down
	step_end

RustboroCity_Gym_Text_Tips::
	.string "Yo, how's it going?\p"
	.string "Listen, my friend!\n"
	.string "Would you like to become the Champion?\p"
	.string "I'm no Trainer, not me, but I can\n"
	.string "sure give you winning advice.\p"
	.string "That's settled, then! We'll aim for the\n"
	.string "Pokémon Championship together!\p"
	.string "It's your job as a Trainer to collect\n"
	.string "Gym Badges, am I right?\p"
	.string "But Gym Leaders aren't pushovers!\p"
	.string "And that's where I come in!\n"
	.string "I'm here to provide expert advice!\p"
	.string "Roxanne, the Gym Leader, is a user\n"
	.string "of Rock-type Pokémon.\p"
	.string "She has a full team of six, with good\n"
	.string "type variety.\p"
	.string "Roxanne protects her team before\n"
	.string "raising its Attack with Swagger.\p"
	.string "Bring a way to remove boosts or\n"
	.string "guard against spread attacks.\p"
	.string "Come see me afterwards, if you beat\n"
	.string "the Gym Leader.\p"
	.string "Oh, and take these bottles of\n"
	.string "Fresh Water for your challenge!$"

RustboroCity_Gym_Text_GymGuideAdvice::
	.string "That's all I've got for now,\n"
	.string "Champ-to-be!\p"
	.string "If you want more advice, I\n"
	.string "suggest bringing as many Pokémon\l"
	.string "as you can!\p"
	.string "You could also ask around the town.\p"
	.string "People might have something that\n"
	.string "could help you, like a Steel-type\l"
	.string "Pokémon they're willing to trade!\p"
	.string "That Pokémon might even be holding\n"
	.string "something that the local Nidoran\l"
	.string "really like (Hint, hint)!$"

RustboroCity_Gym_Text_GymGuidePostVictory: @ 82132E2
	.string "Whoa! What a breathtaking victory!\n"
	.string "My cheering must've worked!\p"
	.string "Great!\n"
	.string "Check your Trainer Card.\p"
	.string "The Stone Badge you got should be\n"
	.string "properly recorded on it.\p"
	.string "In other words…\p"
	.string "You've climbed the first step on\n"
	.string "the stairs to the Championship!\p"
	.string "That's got to feel awesome!$"

RustboroCity_Gym_Text_JoshIntro: @ 82133E9
	.string "Small rocks can carry big lessons!\n"
	.string "Shuckle, share your defenses!$"

RustboroCity_Gym_Text_JoshDefeat: @ 821342D
	.string "You broke through our lesson!$"

RustboroCity_Gym_Text_JoshPostBattle: @ 8213447
	.string "Guard Split lends Shuckle's Defense\n"
	.string "to my frail Cranidos.\p"
	.string "Water feeds Lileep's Storm Drain,\n"
	.string "and physical hits make Glimmet\l"
	.string "scatter Toxic Spikes.$"

RustboroCity_Gym_Text_JoshNotEnoughPokemon:
	.string "You need two healthy Pokémon\n"
	.string "for this doubles lesson!$"

RustboroCity_Gym_Text_TommyIntro: @ 8213486
	.string "A good lesson is worth repeating!\n"
	.string "Tyrunt, show them. Oranguru,\l"
	.string "make it do that again!$"

RustboroCity_Gym_Text_TommyDefeat: @ 82134C6
	.string "You stopped me mid-lesson!$"

RustboroCity_Gym_Text_TommyPostBattle: @ 82134E6
	.string "Instruct makes my partner repeat\n"
	.string "its last move right away.\p"
	.string "Nosepass uses Wide Guard to block\n"
	.string "attacks aimed at both of us.$"

RustboroCity_Gym_Text_TommyNotEnoughPokemon:
	.string "Bring two healthy Pokémon.\n"
	.string "This is a doubles lesson!$"

RustboroCity_Gym_Text_MarcIntro: @ 8213533
	.string "Every layer underfoot tells a story.\p"
	.string "Choose one bad step, and the whole\n"
	.string "cliff shifts!$"

RustboroCity_Gym_Text_MarcDefeat: @ 8213589
	.string "You read every layer!$"

RustboroCity_Gym_Text_MarcPostBattle: @ 82135C0
	.string "Shieldon scatters sharp stones,\n"
	.string "then Woobat trades away its Scarf.\p"
	.string "If Cursed Body seals that one move,\n"
	.string "you must choose whether to switch.$"

RustboroCity_Gym_Text_RoxanneIntro: @ 821362A
	.string "Hello, I am Roxanne, the Rustboro\n"
	.string "Pokémon Gym Leader.\p"
	.string "I became a Gym Leader to apply\n"
	.string "what I learned at the Pokémon\l"
	.string "Trainer's School in battle.\p"
	.string "Today's examination is confidence.\p"
	.string "I will protect my team first,\n"
	.string "then turn Swagger into strength.\p"
	.string "Show me how you answer preparation\n"
	.string "and pressure at the same time.$"

RustboroCity_Gym_Text_RoxanneDefeat: @ 821370B
	.string "So…\n"
	.string "I lost…\p"
	.string "You broke my protection and kept\n"
	.string "your strategy flexible.\p"
	.string "I still have much more to learn.\p"
	.string "The Pokémon League's rules state\n"
	.string "that Trainers are to be given this\l"
	.string "if they defeat a Gym Leader.\p"
	.string "Please accept the official Pokémon\n"
	.string "League Stone Badge.$"

RustboroCity_Gym_Text_RoxanneNotEnoughPokemon:
	.string "A doubles examination requires\n"
	.string "two healthy Pokémon.$"

RustboroCity_Gym_Text_ReceivedStoneBadge: @ 82137EC
	.string "{PLAYER} received the Stone Badge\n"
	.string "from Roxanne.$"

RustboroCity_Gym_Text_StoneBadgeInfoTakeThis: @ 8213816
	.string "The Stone Badge enables your Pokémon\n"
	.string "to use the HM move Cut outside\l"
	.string "of battle.\p"
	.string "Please take this with you, too.$"

RustboroCity_Gym_Text_ExplainRockTomb: @ 82138B1
	.string "That Expert Belt strengthens moves\n"
	.string "when they strike a foe's weakness.\p"
	.string "Use your knowledge of type matchups\n"
	.string "to get the most from it!$"

RustboroCity_Gym_Text_RoxannePostBattle: @ 82139A7
	.string "Safeguard prevents confusion,\n"
	.string "so Swagger can raise Attack safely.\p"
	.string "Taunt, Haze, Wide Guard, or focused\n"
	.string "attacks can break that lesson.$"

RustboroCity_Gym_Text_GymStatue: @ 8213A21
	.string "Rustboro City Pokémon Gym$"

RustboroCity_Gym_Text_GymStatueCertified: @ 8213A3B
	.string "Rustboro City Pokémon Gym\p"
	.string "Roxanne's certified Trainers:\n"
	.string "{PLAYER}$"

RustboroCity_Gym_Text_RoxanneRegisterCall: @ 8213A76
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "Roxanne: Oh, hello, {PLAYER}?\n"
	.string "This is Roxanne of Rustboro City.\p"
	.string "I heard from Brawly that you're\n"
	.string "moving up through the ranks of\l"
	.string "Trainers at an incredible clip.\p"
	.string "Well, I'd rather not think that\n"
	.string "I can't beat you.\p"
	.string "I promise, I will shape up my Gym\n"
	.string "so that you may challenge us again.\p"
	.string "I hope you'll be looking forward to\n"
	.string "that occasion as much as me!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

RustboroCity_Gym_Text_RegisteredRoxanne: @ 8213C01
	.string "Registered Gym Leader Roxanne\n"
	.string "in the PokéNav.$"

RustboroCity_Gym_Text_RoxannePreRematch: @ 8213C2F
	.string "Roxanne: Let us battle!$"

RustboroCity_Gym_Text_RoxanneRematchDefeat: @ 8213CF9
	.string "Grr…\n"
	.string "Again, I have lost…$"

RustboroCity_Gym_Text_RoxannePostRematch: @ 8213D12
	.string "Roxanne: I still have much to learn\n"
	.string "when it comes to battling Pokémon.\p"
	.string "That awareness makes me love battling\n"
	.string "all that much more!$"

RustboroCity_Gym_Text_RoxanneRematchNeedTwoMons: @ 8213D93
	.string "Roxanne: Oh, no…\n"
	.string "You only have one Pokémon with you?\p"
	.string "Please return with at least one more\n"
	.string "Pokémon.$"

RustboroCity_Gym_Text_RoxanneChooseFormat:
	.string "Roxanne: I'm so glad to see you again.\n"
	.string "I'm Roxanne, the Gym Leader here.\p"
	.string "I'm sure we've both experienced many\n"
	.string "battles since we last met.\p"
	.string "I would like to see how much better\n"
	.string "we've become.\p"
	.string "Would you prefer a single battle\n"
	.string "or a double battle?$"

RustboroCity_Gym_Text_RoxanneAllowLegends:
	.string "And would you like me to battle with\n"
	.string "Legendary Pokémon?$"
