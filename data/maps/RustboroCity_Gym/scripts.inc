RustboroCity_Gym_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RustboroCity_Gym_OnTransition
	.byte 0

RustboroCity_Gym_OnTransition:
	goto_if_unset FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GuideStateReady
	setvar VAR_RUSTBORO_GYM_GUIDE_STATE, 1
RustboroCity_Gym_EventScript_GuideStateReady:
	end

RustboroCity_Gym_EventScript_Roxanne::
	trainerbattle_double TRAINER_ROXANNE_1, RustboroCity_Gym_Text_RoxanneIntro, RustboroCity_Gym_Text_RoxanneDefeat, EmeraldChampions_Text_NeedTwoPokemon, RustboroCity_Gym_EventScript_RoxanneDefeated, NO_MUSIC
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_Gym_EventScript_RoxanneRematch
	goto_if_unset FLAG_RECEIVED_ROXANNE_OLD_AMBER, RustboroCity_Gym_EventScript_GiveRoxanneRewards
	goto_if_unset FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE, RustboroCity_Gym_EventScript_GiveLegacyOldAmber
	msgbox RustboroCity_Gym_Text_RoxannePostBattle, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_RoxanneDefeated::
	message RustboroCity_Gym_Text_ReceivedStoneBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox RustboroCity_Gym_Text_StoneBadgeInfoTakeThis, MSGBOX_DEFAULT
	setflag FLAG_DEFEATED_RUSTBORO_GYM
	setflag FLAG_BADGE01_GET
	setvar VAR_RUSTBORO_CITY_STATE, 1
	addvar VAR_PETALBURG_GYM_STATE, 1
	setvar VAR_0x8008, 1
	call Common_EventScript_SetGymTrainers
	call_if_eq VAR_PETALBURG_GYM_STATE, 6, Common_EventScript_ReadyPetalburgGymForBattle
	goto RustboroCity_Gym_EventScript_GiveRoxanneRewards
	end

RustboroCity_Gym_EventScript_GiveRoxanneRewards::
	goto_if_set FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE, RustboroCity_Gym_EventScript_GiveOldAmber
	giveitem ITEM_AERODACTYLITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE
RustboroCity_Gym_EventScript_GiveOldAmber::
	giveitem ITEM_OLD_AMBER
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_ROXANNE_OLD_AMBER
	msgbox RustboroCity_Gym_Text_ExplainRoxanneRewards, MSGBOX_DEFAULT
	release
	end

@ Saves created before Roxanne awarded Old Amber already have the legacy TM flag.
RustboroCity_Gym_EventScript_GiveLegacyOldAmber::
	giveitem ITEM_OLD_AMBER
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE
	msgbox RustboroCity_Gym_Text_ExplainRoxanneRewards, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_RoxanneRematch::
	trainerbattle_rematch_double TRAINER_ROXANNE_1, RustboroCity_Gym_Text_RoxannePreRematch, RustboroCity_Gym_Text_RoxanneRematchDefeat, RustboroCity_Gym_Text_RoxanneRematchNeedTwoMons
	msgbox RustboroCity_Gym_Text_RoxannePostRematch, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Josh::
	trainerbattle_double TRAINER_JOSH, RustboroCity_Gym_Text_JoshIntro, RustboroCity_Gym_Text_JoshDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox RustboroCity_Gym_Text_JoshPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Tommy::
	trainerbattle_double TRAINER_TOMMY, RustboroCity_Gym_Text_TommyIntro, RustboroCity_Gym_Text_TommyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox RustboroCity_Gym_Text_TommyPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Marc::
	trainerbattle_double TRAINER_MARC, RustboroCity_Gym_Text_MarcIntro, RustboroCity_Gym_Text_MarcDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox RustboroCity_Gym_Text_MarcPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_GymGuide::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_RUSTBORO_GYM, RustboroCity_Gym_EventScript_GymGuidePostVictory
	msgbox RustboroCity_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_GymGuidePostVictory::
	msgbox RustboroCity_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_LeftGymStatue::
	lockall
	goto_if_set FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GymStatueCertified
	goto RustboroCity_Gym_EventScript_GymStatue
	end

RustboroCity_Gym_EventScript_RightGymStatue::
	lockall
	goto_if_set FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GymStatueCertified
	goto RustboroCity_Gym_EventScript_GymStatue
	end

RustboroCity_Gym_EventScript_GymStatueCertified::
	msgbox RustboroCity_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_Gym_EventScript_GymStatue::
	msgbox RustboroCity_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_Gym_EventScript_RegisterRoxanne::
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

RustboroCity_Gym_EventScript_GuideTrigger1::
	lockall
	setvar VAR_TEMP_1, 1
	playse SE_PIN
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, RustboroCity_Gym_Movement_GuideApproach1
	waitmovement 0
	goto RustboroCity_Gym_EventScript_GuideIntroduction

RustboroCity_Gym_EventScript_GuideTrigger2::
	lockall
	setvar VAR_TEMP_1, 2
	playse SE_PIN
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, RustboroCity_Gym_Movement_GuideApproach2
	waitmovement 0
	applymovement LOCALID_PLAYER, RustboroCity_Gym_Movement_PlayerFaceLeft
	waitmovement 0
	goto RustboroCity_Gym_EventScript_GuideIntroduction

RustboroCity_Gym_EventScript_GuideTrigger3::
	lockall
	setvar VAR_TEMP_1, 3
	playse SE_PIN
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, RustboroCity_Gym_Movement_GuideApproach3
	waitmovement 0
	goto RustboroCity_Gym_EventScript_GuideIntroduction

RustboroCity_Gym_EventScript_GuideTrigger4::
	lockall
	setvar VAR_TEMP_1, 4
	playse SE_PIN
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_PLAYER, RustboroCity_Gym_Movement_PlayerFaceLeft
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, RustboroCity_Gym_Movement_GuideApproach4
	waitmovement 0
	applymovement LOCALID_PLAYER, RustboroCity_Gym_Movement_PlayerFaceUp
	waitmovement 0
	goto RustboroCity_Gym_EventScript_GuideIntroduction

RustboroCity_Gym_EventScript_GuideIntroduction::
	msgbox RustboroCity_Gym_Text_GuideIntroduction, MSGBOX_DEFAULT
	giveitem ITEM_FRESH_WATER, 5
	goto_if_eq VAR_RESULT, FALSE, RustboroCity_Gym_EventScript_GuideBagFull
	setvar VAR_RUSTBORO_GYM_GUIDE_STATE, 1
	goto_if_eq VAR_TEMP_1, 1, RustboroCity_Gym_EventScript_GuideReturn1
	goto_if_eq VAR_TEMP_1, 2, RustboroCity_Gym_EventScript_GuideReturn2
	goto_if_eq VAR_TEMP_1, 3, RustboroCity_Gym_EventScript_GuideReturn3
	goto RustboroCity_Gym_EventScript_GuideReturn4

RustboroCity_Gym_EventScript_GuideBagFull::
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_Gym_EventScript_GuideReturn1::
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, RustboroCity_Gym_Movement_GuideReturn1
	waitmovement 0
	releaseall
	end

RustboroCity_Gym_EventScript_GuideReturn2::
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, RustboroCity_Gym_Movement_GuideReturn2
	waitmovement 0
	releaseall
	end

RustboroCity_Gym_EventScript_GuideReturn3::
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, RustboroCity_Gym_Movement_GuideReturn3
	waitmovement 0
	releaseall
	end

RustboroCity_Gym_EventScript_GuideReturn4::
	applymovement LOCALID_RUSTBORO_GYM_GUIDE, RustboroCity_Gym_Movement_GuideReturn4
	waitmovement 0
	releaseall
	end

RustboroCity_Gym_Movement_GuideApproach1:
	walk_down
	face_right
	step_end

RustboroCity_Gym_Movement_GuideApproach2:
	walk_right
	step_end

RustboroCity_Gym_Movement_GuideApproach3:
	walk_right
	walk_right
	step_end

RustboroCity_Gym_Movement_GuideApproach4:
	walk_right
	walk_right
	walk_right
	walk_right
	face_down
	step_end

RustboroCity_Gym_Movement_PlayerFaceLeft:
	face_left
	step_end

RustboroCity_Gym_Movement_PlayerFaceUp:
	face_up
	step_end

RustboroCity_Gym_Movement_GuideReturn1:
	walk_up
	face_down
	step_end

RustboroCity_Gym_Movement_GuideReturn2:
	walk_left
	face_down
	step_end

RustboroCity_Gym_Movement_GuideReturn3:
	walk_left
	walk_left
	face_down
	step_end

RustboroCity_Gym_Movement_GuideReturn4:
	walk_left
	walk_left
	walk_left
	walk_left
	face_down
	step_end

RustboroCity_Gym_Text_GuideIntroduction:
	.string "Yo! Welcome to your first GYM exam.\p"
	.string "ROXANNE battles with a full doubles team.\n"
	.string "Her students teach the pieces: field\l"
	.string "control, redirection, and reverse speed.\p"
	.string "Read each pair before attacking, and\n"
	.string "bring answers instead of one favorite.\p"
	.string "Take these FRESH WATERS for the road.\n"
	.string "Come see me after you earn the BADGE!$"

RustboroCity_Gym_Text_GymGuideAdvice:
	.string "Yo, how's it going?\p"
	.string "Listen, my friend!\n"
	.string "Would you like to become the CHAMPION?\p"
	.string "I'm no TRAINER, not me, but I can\n"
	.string "sure give you winning advice.\p"
	.string "That's settled, then! We'll aim for the\n"
	.string "POKéMON CHAMPIONSHIP together!\p"
	.string "It's your job as a TRAINER to collect\n"
	.string "GYM BADGES, am I right?\p"
	.string "But GYM LEADERS aren't pushovers!\p"
	.string "And that's where I come in!\n"
	.string "I'm here to provide expert advice!\p"
	.string "ROXANNE, the GYM LEADER, is a user\n"
	.string "of ROCK-type POKéMON.\p"
	.string "The ROCK type is very durable, but\n"
	.string "it can't stand WATER-type and\l"
	.string "GRASS-type moves.\p"
	.string "Come see me afterwards, if you beat\n"
	.string "the GYM LEADER.\p"
	.string "Well, go for it!$"

RustboroCity_Gym_Text_GymGuidePostVictory:
	.string "Whoa! What a breathtaking victory!\n"
	.string "My cheering must've worked!\p"
	.string "Great!\n"
	.string "Check your TRAINER CARD.\p"
	.string "The STONE BADGE you got should be\n"
	.string "properly recorded on it.\p"
	.string "In other words…\p"
	.string "You've climbed the first step on\n"
	.string "the stairs to the CHAMPIONSHIP!\p"
	.string "That's got to feel awesome!$"

RustboroCity_Gym_Text_JoshIntro:
	.string "STURDY means BONSLY and SHIELDON\n"
	.string "always live to swing back.\p"
	.string "Chip them first, or don't swing!$"

RustboroCity_Gym_Text_JoshDefeat:
	.string "You broke the wall and the chip!$"

RustboroCity_Gym_Text_JoshPostBattle:
	.string "STURDY guarantees NOSEPASS one more\n"
	.string "turn. Plan for the turn after it.$"

RustboroCity_Gym_Text_TommyIntro:
	.string "LILEEP drinks the water you aim at\n"
	.string "my rocks. SHUCKLE slows your feet.\p"
	.string "Go on -- find the dry path!$"

RustboroCity_Gym_Text_TommyDefeat:
	.string "You saw right through them!$"

RustboroCity_Gym_Text_TommyPostBattle:
	.string "Two small ROCKS still add up. Never\n"
	.string "underestimate a first-badge team.$"

RustboroCity_Gym_Text_MarcIntro:
	.string "AMAURA calls the snow, and the veil\n"
	.string "it raises blunts everything.\p"
	.string "Break the snow, or break through!$"

RustboroCity_Gym_Text_MarcDefeat:
	.string "You kept control of your choices!$"

RustboroCity_Gym_Text_MarcPostBattle:
	.string "ROXANNE combines every lesson: field\n"
	.string "control, reverse speed, and endurance.$"

RustboroCity_Gym_Text_RoxanneIntro:
	.string "I am ROXANNE, RUSTBORO's GYM LEADER.\p"
	.string "Rock is not one answer. It is endurance,\n"
	.string "redirection, reverse speed, and pressure.\p"
	.string "My students taught you the pieces. Now\n"
	.string "solve the complete first examination.$"

RustboroCity_Gym_Text_RoxanneDefeat:
	.string "You controlled the room, respected the\n"
	.string "drain, and kept an answer for my ace.\p"
	.string "That is understanding, not memorization.\n"
	.string "Please accept the STONE BADGE.$"

RustboroCity_Gym_Text_ReceivedStoneBadge:
	.string "{PLAYER} received the STONE BADGE\n"
	.string "from ROXANNE.$"

RustboroCity_Gym_Text_StoneBadgeInfoTakeThis:
	.string "The STONE BADGE lets your LEVELER train\n"
	.string "a party up to Lv. 20.\p"
	.string "It also enables POKéMON to use CUT\n"
	.string "outside of battle.\p"
	.string "It also marks your first true step\n"
	.string "toward the POKéMON LEAGUE.\p"
	.string "Please take this with you, too.$"
RustboroCity_Gym_Text_ExplainRoxanneRewards:
	.string "Aerodactylite lets Aerodactyl Mega\n"
	.string "Evolve once you possess a Mega Ring.\p"
	.string "DEVON can restore that OLD AMBER.\n"
	.string "Consider Aerodactyl your first Mega\l"
	.string "project.$"

RustboroCity_Gym_Text_RoxannePostBattle:
	.string "No later Gym will ask the same question.\n"
	.string "Keep changing the shape of your team.$"

RustboroCity_Gym_Text_GymStatue:
	.string "RUSTBORO CITY POKéMON GYM$"

RustboroCity_Gym_Text_GymStatueCertified:
	.string "RUSTBORO CITY POKéMON GYM\p"
	.string "ROXANNE'S CERTIFIED TRAINERS:\n"
	.string "{PLAYER}$"

RustboroCity_Gym_Text_RoxanneRegisterCall:
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "ROXANNE: Oh, hello, {PLAYER}?\n"
	.string "This is ROXANNE of RUSTBORO CITY.\p"
	.string "I heard from BRAWLY that you're\n"
	.string "moving up through the ranks of\l"
	.string "TRAINERS at an incredible clip.\p"
	.string "Your progress gives my students\n"
	.string "an excellent model to study.\p"
	.string "I hope you'll share what you learn\n"
	.string "as your journey continues!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

RustboroCity_Gym_Text_RegisteredRoxanne:
	.string "Registered GYM LEADER ROXANNE\n"
	.string "in the POKéNAV.$"

RustboroCity_Gym_Text_RoxannePreRematch:
	.string "ROXANNE: I'm so glad to see you again.\n"
	.string "I'm ROXANNE, the GYM LEADER here.\p"
	.string "I'm sure we've both experienced many\n"
	.string "battles since we last met.\p"
	.string "I would like to see how much better\n"
	.string "we've become. Let us battle!$"

RustboroCity_Gym_Text_RoxanneRematchDefeat:
	.string "Grr…\n"
	.string "Again, I have lost…$"

RustboroCity_Gym_Text_RoxannePostRematch:
	.string "ROXANNE: I still have much to learn\n"
	.string "when it comes to battling POKéMON.\p"
	.string "That awareness makes me love battling\n"
	.string "all that much more!$"

RustboroCity_Gym_Text_RoxanneRematchNeedTwoMons:
	.string "ROXANNE: I'm so glad to see you again.\n"
	.string "I'm ROXANNE, the GYM LEADER here.\p"
	.string "I'm sure we've both experienced many\n"
	.string "battles since we last met.\p"
	.string "I would like to see how much better\n"
	.string "we've become…\p"
	.string "Oh, no…\n"
	.string "You only have one POKéMON with you?\p"
	.string "Please return with at least one more\n"
	.string "POKéMON.$"
