RustboroCity_Gym_MapScripts:: @ 8212F30
	.byte 0

RustboroCity_Gym_EventScript_Roxanne::
	trainerbattle_double TRAINER_ROXANNE_1, RustboroCity_Gym_Text_RoxanneIntro, RustboroCity_Gym_Text_RoxanneDefeat, EmeraldChampions_Text_NeedTwoPokemon, RustboroCity_Gym_EventScript_RoxanneDefeated, NO_MUSIC
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
	@ FLAG_RECEIVED_TM39 is Delphoxite's only receipt (Norman may give it
	@ to a Fennekin pair); see src/mega_stone_rewards.c.
	goto_if_unset FLAG_RECEIVED_TM39, RustboroCity_Gym_EventScript_GiveRockTomb
	release
	end

RustboroCity_Gym_EventScript_GiveRockTomb:: @ 8212FA4
	giveitem ITEM_DELPHOXITE
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM39
	msgbox RustboroCity_Gym_Text_ExplainRockTomb, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_Josh:: @ 8212FE3
	trainerbattle_double TRAINER_JOSH, RustboroCity_Gym_Text_JoshIntro, RustboroCity_Gym_Text_JoshDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox RustboroCity_Gym_Text_JoshPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Tommy:: @ 8212FFA
	trainerbattle_double TRAINER_TOMMY, RustboroCity_Gym_Text_TommyIntro, RustboroCity_Gym_Text_TommyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox RustboroCity_Gym_Text_TommyPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Marc:: @ 8213011
	trainerbattle_double TRAINER_MARC, RustboroCity_Gym_Text_MarcIntro, RustboroCity_Gym_Text_MarcDefeat, EmeraldChampions_Text_NeedTwoPokemon
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
	compare VAR_RESULT, FALSE
	goto_if_eq Rustboro_Gym_Guide_BagFull
	setvar VAR_RUSTBORO_CITY_STATE, 1
	goto Rustboro_Gym_Guide_FinishSpeech

Rustboro_Gym_Guide_BagFull::
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
Rustboro_Gym_Guide_FinishSpeech::
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
	.string "Her Pokémon are a few levels above\n"
	.string "what yours can reach for now, so\l"
	.string "train yours as high as they'll go!\p"
	.string "Grass and Steel moves hit most of\n"
	.string "her team hard!\p"
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
	.string "You could also ask around the town.\n"
	.string "People here like to help out!$"

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
	.string "Don't take us Gym Trainers lightly!\n"
	.string "I'll show you why we're better!$"

RustboroCity_Gym_Text_JoshDefeat: @ 821342D
	.string "You were too good for me…$"

RustboroCity_Gym_Text_JoshPostBattle: @ 8213447
	.string "You haven't seen anything of the Rock\n"
	.string "type's terrifying power!$"

RustboroCity_Gym_Text_TommyIntro: @ 8213486
	.string "If you can't beat me, you won't stand\n"
	.string "a chance against Roxanne!$"

RustboroCity_Gym_Text_TommyDefeat: @ 82134C6
	.string "Wow! You've got some potential!$"

RustboroCity_Gym_Text_TommyPostBattle: @ 82134E6
	.string "Roxanne is stronger than me by\n"
	.string "several times.\p"
	.string "You'd better be on your guard!$"

RustboroCity_Gym_Text_MarcIntro: @ 8213533
	.string "We might be in the middle of town,\n"
	.string "but so what?\p"
	.string "My Rock Pokémon need room to\n"
	.string "rampage!$"

RustboroCity_Gym_Text_MarcDefeat: @ 8213589
	.string "Oh, man oh man!\n"
	.string "Our challenger is one feisty customer!$"

RustboroCity_Gym_Text_MarcPostBattle: @ 82135C0
	.string "I have to hand it to our Leader.\p"
	.string "It took smarts and sense for her to\n"
	.string "pick the Rock type at her young age.$"

RustboroCity_Gym_Text_RoxanneIntro: @ 821362A
	.string "Hello, I am Roxanne, the Rustboro\n"
	.string "Pokémon Gym Leader.\p"
	.string "I became a Gym Leader so that I might\n"
	.string "apply what I learned at the Pokémon\l"
	.string "Trainer's School in battle.\p"
	.string "Would you kindly demonstrate how you\n"
	.string "battle, and with which Pokémon?$"

RustboroCity_Gym_Text_RoxanneDefeat: @ 821370B
	.string "So…\n"
	.string "I lost…\p"
	.string "It seems that I still have much more\n"
	.string "to learn…\p"
	.string "I understand.\p"
	.string "The Pokémon League's rules state\n"
	.string "that Trainers are to be given this\l"
	.string "if they defeat a Gym Leader.\p"
	.string "Please accept the official Pokémon\n"
	.string "League Stone Badge.$"

RustboroCity_Gym_Text_ReceivedStoneBadge: @ 82137EC
	.string "{PLAYER} received the Stone Badge\n"
	.string "from Roxanne.$"

RustboroCity_Gym_Text_StoneBadgeInfoTakeThis: @ 8213816
	.string "With the Stone Badge, your Pokémon\n"
	.string "can grow up to Lv. 20!\p"
	.string "It also lets you use Cut. The Cutter\n"
	.string "beside the Pokémon Center has\l"
	.string "the license.\p"
	.string "Please take this Mega Stone, too.$"

RustboroCity_Gym_Text_ExplainRockTomb: @ 82138B1
	.string "This is Delphoxite. Give it to\n"
	.string "Delphox to Mega Evolve once you\l"
	.string "have a Mega Ring.$"

RustboroCity_Gym_Text_RoxannePostBattle: @ 82139A7
	.string "Since you are so strong, you should\n"
	.string "challenge other Gym Leaders.\p"
	.string "By battling many Trainers, you should\n"
	.string "learn many things.$"

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
	.string "Your battle showed my students\n"
	.string "how much teamwork matters.\p"
	.string "Keep discovering new partners\n"
	.string "and learning from your battles.\p"
	.string "I look forward to hearing more\n"
	.string "about your journey!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"
