FortreeCity_Gym_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, FortreeCity_Gym_OnTransition
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, FortreeCity_Gym_OnWarp
	.byte 0

FortreeCity_Gym_OnTransition:
	special RotatingGate_InitPuzzle
	setflag FLAG_TEMP_HIDE_FOLLOWER @ Hide follower bc of rotating gates
	end

FortreeCity_Gym_OnWarp:
	map_script_2 VAR_TEMP_0, VAR_TEMP_0, FortreeCity_Gym_EventScript_InitRotatingGates
	.2byte 0

@ NOTE: This rotating gate puzzle makes use of VAR_TEMP_0 - VAR_TEMP_3
FortreeCity_Gym_EventScript_InitRotatingGates::
	special RotatingGate_InitPuzzleAndGraphics
	end

FortreeCity_Gym_EventScript_Winona::
	trainerbattle_double TRAINER_WINONA_1, FortreeCity_Gym_Text_WinonaIntro, FortreeCity_Gym_Text_WinonaDefeat, EmeraldChampions_Text_NeedTwoPokemon, FortreeCity_Gym_EventScript_WinonaDefeated, NO_MUSIC
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, FortreeCity_Gym_EventScript_WinonaRematch
	goto_if_unset FLAG_RECEIVED_WINONA_ALTARIANITE, FortreeCity_Gym_EventScript_GiveAltarianite2
	msgbox FortreeCity_Gym_Text_WinonaPostBattle, MSGBOX_DEFAULT
	release
	end

FortreeCity_Gym_EventScript_WinonaDefeated::
	message FortreeCity_Gym_Text_ReceivedFeatherBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox FortreeCity_Gym_Text_ExplainFeatherBadgeTakeThis, MSGBOX_DEFAULT
	setflag FLAG_DEFEATED_FORTREE_GYM
	setflag FLAG_BADGE06_GET
	setflag FLAG_HIDE_ROUTE_120_STEVEN
	setvar VAR_0x8008, 6
	call Common_EventScript_SetGymTrainers
	call FortreeCity_Gym_EventScript_GiveAltarianite
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox FortreeCity_Gym_Text_RegisteredWinona, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_WINONA_MATCH_CALL
	setvar VAR_SCOTT_FORTREE_CALL_STEP_COUNTER, 0
	setflag FLAG_SCOTT_CALL_FORTREE_GYM
	release
	end

FortreeCity_Gym_EventScript_GiveAltarianite2::
	giveitem ITEM_ALTARIANITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	msgbox FortreeCity_Gym_Text_ExplainAltarianite, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_WINONA_ALTARIANITE
	release
	end

FortreeCity_Gym_EventScript_GiveAltarianite::
	giveitem ITEM_ALTARIANITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_BagIsFull
	msgbox FortreeCity_Gym_Text_ExplainAltarianite, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_WINONA_ALTARIANITE
	return

FortreeCity_Gym_EventScript_WinonaRematch::
	trainerbattle_rematch_double TRAINER_WINONA_1, FortreeCity_Gym_Text_WinonaPreRematch, FortreeCity_Gym_Text_WinonaRematchDefeat, FortreeCity_Gym_Text_WinonaRematchNeedTwoMons
	msgbox FortreeCity_Gym_Text_WinonaPostRematch, MSGBOX_AUTOCLOSE
	end

FortreeCity_Gym_EventScript_Jared::
	trainerbattle_double TRAINER_JARED, FortreeCity_Gym_Text_JaredIntro, FortreeCity_Gym_Text_JaredDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox FortreeCity_Gym_Text_JaredPostBattle, MSGBOX_AUTOCLOSE
	end

FortreeCity_Gym_EventScript_Edwardo::
	trainerbattle_double TRAINER_EDWARDO, FortreeCity_Gym_Text_EdwardoIntro, FortreeCity_Gym_Text_EdwardoDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox FortreeCity_Gym_Text_EdwardoPostBattle, MSGBOX_AUTOCLOSE
	end

FortreeCity_Gym_EventScript_Flint::
	trainerbattle_double TRAINER_FLINT, FortreeCity_Gym_Text_FlintIntro, FortreeCity_Gym_Text_FlintDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox FortreeCity_Gym_Text_FlintPostBattle, MSGBOX_AUTOCLOSE
	end

FortreeCity_Gym_EventScript_Ashley::
	trainerbattle_double TRAINER_ASHLEY, FortreeCity_Gym_Text_AshleyIntro, FortreeCity_Gym_Text_AshleyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox FortreeCity_Gym_Text_AshleyPostBattle, MSGBOX_AUTOCLOSE
	end

FortreeCity_Gym_EventScript_Humberto::
	trainerbattle_double TRAINER_HUMBERTO, FortreeCity_Gym_Text_HumbertoIntro, FortreeCity_Gym_Text_HumbertoDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox FortreeCity_Gym_Text_HumbertoPostBattle, MSGBOX_AUTOCLOSE
	end

FortreeCity_Gym_EventScript_Darius::
	trainerbattle_double TRAINER_DARIUS, FortreeCity_Gym_Text_DariusIntro, FortreeCity_Gym_Text_DariusDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox FortreeCity_Gym_Text_DariusPostBattle, MSGBOX_AUTOCLOSE
	end

FortreeCity_Gym_EventScript_GymGuide::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_FORTREE_GYM, FortreeCity_Gym_EventScript_GymGuidePostVictory
	msgbox FortreeCity_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

FortreeCity_Gym_EventScript_GymGuidePostVictory::
	msgbox FortreeCity_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

FortreeCity_Gym_EventScript_LeftGymStatue::
	lockall
	goto_if_set FLAG_BADGE06_GET, FortreeCity_Gym_EventScript_GymStatueCertified
	goto FortreeCity_Gym_EventScript_GymStatue
	end

FortreeCity_Gym_EventScript_RightGymStatue::
	lockall
	goto_if_set FLAG_BADGE06_GET, FortreeCity_Gym_EventScript_GymStatueCertified
	goto FortreeCity_Gym_EventScript_GymStatue
	end

FortreeCity_Gym_EventScript_GymStatueCertified::
	msgbox FortreeCity_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

FortreeCity_Gym_EventScript_GymStatue::
	msgbox FortreeCity_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

FortreeCity_Gym_Text_GymGuideAdvice:
	.string "Yo, how's it going, CHAMPION-\n"
	.string "bound {PLAYER}?\p"
	.string "FORTREE GYM LEADER WINONA is\n"
	.string "a master of FLYING-type POKéMON.\p"
	.string "She's waiting at the back of this GYM,\n"
	.string "behind the rotating doors.\p"
	.string "She's waiting for new challengers\n"
	.string "who are trying to take wing!\p"
	.string "Okay, go for it!$"

FortreeCity_Gym_Text_GymGuidePostVictory:
	.string "You did it!\n"
	.string "You've achieved liftoff!$"

FortreeCity_Gym_Text_JaredIntro:
	.string "SWOOBAT's SIMPLE doubles every CALM\n"
	.string "MIND. NOCTOWL puts you to sleep first.\p"
	.string "Wake up before STORED POWER lands!$"

FortreeCity_Gym_Text_JaredDefeat:
	.string "You broke the boost before it grew!$"

FortreeCity_Gym_Text_JaredPostBattle:
	.string "TAUNT or a fast hit stops SIMPLE setup.\n"
	.string "Never give a SIMPLE user a free turn.$"

FortreeCity_Gym_Text_EdwardoIntro:
	.string "DRIFBLIM sets TAILWIND, JUMPLUFF puts\n"
	.string "you to sleep, STARAPTOR ends the game.\p"
	.string "Which turn will you spend on which?$"

FortreeCity_Gym_Text_EdwardoDefeat:
	.string "You outlasted my wind…$"

FortreeCity_Gym_Text_EdwardoPostBattle:
	.string "TAILWIND lasts four turns. Stall the\n"
	.string "wind, then punish what it carried.$"

FortreeCity_Gym_Text_FlintIntro:
	.string "GUTS SWELLOW, SPEED BOOST YANMEGA, and\n"
	.string "a TAILWIND behind them.\p"
	.string "WINONA needn't see you if I win here!$"

FortreeCity_Gym_Text_FlintDefeat:
	.string "WINONA, I… The wind broke!$"

FortreeCity_Gym_Text_FlintPostBattle:
	.string "Speed stacked on speed still dies to\n"
	.string "priority. EXTREME SPEED is coming.$"

FortreeCity_Gym_Text_AshleyIntro:
	.string "PIDGEOT's HURRICANE never misses under\n"
	.string "NO GUARD. SWANNA brings the rain.\p"
	.string "Position, then power!$"

FortreeCity_Gym_Text_AshleyDefeat:
	.string "You changed the weather on me!$"

FortreeCity_Gym_Text_AshleyPostBattle:
	.string "Your own weather move overwrites mine.\n"
	.string "The sky belongs to whoever set it last.$"

FortreeCity_Gym_Text_HumbertoIntro:
	.string "GLISCOR heals through poison and sets\n"
	.string "up. MANDIBUZZ taunts your answer.\p"
	.string "Then MINIOR breaks its shell!$"

FortreeCity_Gym_Text_HumbertoDefeat:
	.string "You hit MINIOR before the shell came off!$"

FortreeCity_Gym_Text_HumbertoPostBattle:
	.string "SHELL SMASH needs one free turn.\n"
	.string "FAKE OUT or a spread attack denies it.$"

FortreeCity_Gym_Text_DariusIntro:
	.string "EMOLGA's LIGHTNING ROD swallows every\n"
	.string "ELECTRIC move aimed at my birds.\p"
	.string "Attack through it, if you can!$"

FortreeCity_Gym_Text_DariusDefeat:
	.string "You aimed at the rod itself!$"

FortreeCity_Gym_Text_DariusPostBattle:
	.string "Remove the redirect first. The birds\n"
	.string "behind it are only fast, not sturdy.$"

FortreeCity_Gym_Text_WinonaIntro:
	.string "I am WINONA, FORTREE's GYM LEADER.\p"
	.string "The sky controls position: wind decides\n"
	.string "speed, wings evade Ground, snow hides\l"
	.string "the final horizon.\p"
	.string "Preserve the answer you will need after\n"
	.string "the weather changes.$"

FortreeCity_Gym_Text_WinonaDefeat:
	.string "You denied the wind without losing sight\n"
	.string "of the snow. Accept the FEATHER BADGE.$"

FortreeCity_Gym_Text_ReceivedFeatherBadge:
	.string "{PLAYER} received the FEATHER BADGE\n"
	.string "from WINONA.$"

FortreeCity_Gym_Text_ExplainFeatherBadgeTakeThis:
	.string "The FEATHER BADGE lets your LEVELER\n"
	.string "train a party up to Lv. 60.\p"
	.string "It also enables FLY outside\n"
	.string "of battle.\p"
	.string "And this…\n"
	.string "This is a gift from me.$"
FortreeCity_Gym_Text_ExplainAltarianite:
	.string "Altarianite lets Altaria Mega Evolve.\n"
	.string "Let it hold the stone in battle.$"

FortreeCity_Gym_Text_RegisteredWinona:
	.string "Registered GYM LEADER WINONA\n"
	.string "in the POKéNAV.$"

FortreeCity_Gym_Text_WinonaPostBattle:
	.string "A flying team is not one species label.\n"
	.string "It is freedom of position. Remember that.$"

FortreeCity_Gym_Text_GymStatue:
	.string "FORTREE CITY POKéMON GYM$"

FortreeCity_Gym_Text_GymStatueCertified:
	.string "FORTREE CITY POKéMON GYM\p"
	.string "WINONA'S CERTIFIED TRAINERS:\n"
	.string "{PLAYER}$"

FortreeCity_Gym_Text_WinonaPreRematch:
	.string "WINONA: We humans can never escape\n"
	.string "gravity's pull on the ground.\p"
	.string "But by striving for excellence,\n"
	.string "we have the power to soar!\p"
	.string "You have scaled the pinnacle as\n"
	.string "the reigning CHAMPION.\p"
	.string "Please, allow me to see your power\n"
	.string "at full flight!$"

FortreeCity_Gym_Text_WinonaRematchDefeat:
	.string "I failed to reach your height again…$"

FortreeCity_Gym_Text_WinonaPostRematch:
	.string "WINONA: Even though I have lost,\n"
	.string "the wings of my heart remain unbroken.\p"
	.string "I can rise and soar again and\n"
	.string "yet again.\p"
	.string "I am convinced of it!$"

FortreeCity_Gym_Text_WinonaRematchNeedTwoMons:
	.string "WINONA: We humans can never escape\n"
	.string "gravity's pull on the ground.\p"
	.string "But by striving for excellence,\n"
	.string "we have the power to soar!\p"
	.string "You have scaled the pinnacle as\n"
	.string "the reigning CHAMPION.\p"
	.string "Please, allow me to see your power\n"
	.string "at full flight!\p"
	.string "Oh, no… Do you have but one\n"
	.string "POKéMON with you?\p"
	.string "Please return with two POKéMON\n"
	.string "at the very least.$"
