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
	goto_if_set FLAG_RECEIVED_RED_OR_BLUE_ORB, FortreeCity_Gym_EventScript_WinonaAfterPyre
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
	call_if_unset FLAG_RECEIVED_RED_OR_BLUE_ORB, FortreeCity_Gym_EventScript_PyreHandoff
	setflag FLAG_RECEIVED_WINONA_ALTARIANITE
	release
	end

FortreeCity_Gym_EventScript_GiveAltarianite::
	giveitem ITEM_ALTARIANITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_BagIsFull
	msgbox FortreeCity_Gym_Text_ExplainAltarianite, MSGBOX_DEFAULT
	call_if_unset FLAG_RECEIVED_RED_OR_BLUE_ORB, FortreeCity_Gym_EventScript_PyreHandoff
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
	.string "MIND. NOCTOWL keeps the pressure on.\p"
	.string "Which one will you stop first?$"

FortreeCity_Gym_Text_JaredDefeat:
	.string "We couldn't hold the sky…$"

FortreeCity_Gym_Text_JaredPostBattle:
	.string "TAUNT can stop CALM MIND. But don't\n"
	.string "forget the attacker beside SWOOBAT.$"

FortreeCity_Gym_Text_EdwardoIntro:
	.string "JUMPLUFF puts you to sleep, and\n"
	.string "HONCHKROW grows with every fall.\p"
	.string "Give it nothing to feed on!$"

FortreeCity_Gym_Text_EdwardoDefeat:
	.string "We lost our lift…$"

FortreeCity_Gym_Text_EdwardoPostBattle:
	.string "TAILWIND lasts four turns. Stall the\n"
	.string "wind, then punish what it carried.$"

FortreeCity_Gym_Text_FlintIntro:
	.string "MULTISCALE DRAGONITE, SPEED BOOST\n"
	.string "YANMEGA, and dragons in reserve.\p"
	.string "WINONA needn't see you if I win here!$"

FortreeCity_Gym_Text_FlintDefeat:
	.string "WINONA, I… The wind broke!$"

FortreeCity_Gym_Text_FlintPostBattle:
	.string "Speed stacked on speed still dies to\n"
	.string "priority. EXTREME SPEED is coming.$"

FortreeCity_Gym_Text_AshleyIntro:
	.string "PIDGEOT's HURRICANE never misses\n"
	.string "under NO GUARD. PELIPPER brings\l"
	.string "the rain.\p"
	.string "Position, then power!$"

FortreeCity_Gym_Text_AshleyDefeat:
	.string "Even our sky wasn't enough!$"

FortreeCity_Gym_Text_AshleyPostBattle:
	.string "Changing the rain slows MANTINE.\n"
	.string "But NO GUARD works in any weather.$"

FortreeCity_Gym_Text_HumbertoIntro:
	.string "GLISCOR heals through its own poison,\n"
	.string "and CORVIKNIGHT returns what you send.\p"
	.string "Lower our stats. I dare you!$"

FortreeCity_Gym_Text_HumbertoDefeat:
	.string "Even steel wings can buckle…$"

FortreeCity_Gym_Text_HumbertoPostBattle:
	.string "Special attacks test our defenses.\n"
	.string "ICE is especially hard on GLISCOR.$"

FortreeCity_Gym_Text_DariusIntro:
	.string "TORNADUS gives TAILWIND priority.\n"
	.string "BRAVIARY rides the gust behind it.\p"
	.string "Four turns. Outlast them!$"

FortreeCity_Gym_Text_DariusDefeat:
	.string "Our wind has run its course.$"

FortreeCity_Gym_Text_DariusPostBattle:
	.string "A rival TAILWIND can contest mine.\n"
	.string "TRICK ROOM can turn speed against us.$"

FortreeCity_Gym_Text_WinonaIntro:
	.string "I am WINONA, FORTREE's GYM\n"
	.string "LEADER.\p"
	.string "My POKéMON and I have flown\n"
	.string "through clear skies and sudden\l"
	.string "storms. We know how quickly the\l"
	.string "wind can turn.\p"
	.string "Show me how your team meets a\n"
	.string "changing sky!$"

FortreeCity_Gym_Text_WinonaDefeat:
	.string "You've carried your team through\n"
	.string "our strongest winds. The FEATHER\l"
	.string "BADGE is yours.$"

FortreeCity_Gym_Text_ReceivedFeatherBadge:
	.string "{PLAYER} received the FEATHER BADGE\n"
	.string "from WINONA.$"

FortreeCity_Gym_Text_ExplainFeatherBadgeTakeThis:
	.string "The FEATHER BADGE lets your LEVELER\n"
	.string "train a party up to Lv. 60.\p"
	.string "It also enables FLY outside of battle.\n"
	.string "Your FLIGHT BEACON works from now on.\l"
	.string "Use it from the BAG to revisit towns.\p"
	.string "And this…\n"
	.string "This is a gift from me.$"
FortreeCity_Gym_Text_ExplainAltarianite:
	.string "Altarianite lets Altaria Mega Evolve.\n"
	.string "Let it hold the stone in battle.$"

FortreeCity_Gym_Text_PyreHandoff:
	.string "AQUA has been seen near MT. PYRE.\n"
	.string "Follow ROUTES 120 and 121 east,\l"
	.string "then SURF south on ROUTE 122 to\l"
	.string "find it.\p"
	.string "Discover what they are disturbing\n"
	.string "there. The skies over HOENN are\l"
	.string "unsettled.$"

FortreeCity_Gym_Text_RegisteredWinona:
	.string "Registered GYM LEADER WINONA\n"
	.string "in the POKéNAV.$"

FortreeCity_Gym_Text_WinonaPostBattle:
	.string "WINONA: A strong wind can carry\n"
	.string "you far, but you must choose\l"
	.string "where to go.\p"
	.string "To follow AQUA to MT. PYRE, head\n"
	.string "east through ROUTES 120 and 121,\l"
	.string "then SURF south on ROUTE 122.\p"
	.string "I hope the people there are safe.$"

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

FortreeCity_Gym_EventScript_WinonaAfterPyre::
	msgbox FortreeCity_Gym_Text_WinonaAfterPyre, MSGBOX_DEFAULT
	release
	end

FortreeCity_Gym_Text_WinonaAfterPyre:
	.string "WINONA: A strong wind can carry\n"
	.string "you far, but you must choose\l"
	.string "where to go.\p"
	.string "May your journey bring you clear\n"
	.string "skies.$"

FortreeCity_Gym_EventScript_PyreHandoff::
	msgbox FortreeCity_Gym_Text_PyreHandoff, MSGBOX_DEFAULT
	return
