DewfordTown_Gym_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DewfordTown_Gym_OnTransition
	.byte 0

DewfordTown_Gym_OnTransition:
	call DewfordTown_Gym_EventScript_SetFlashLevel
	end

DewfordTown_Gym_EventScript_SetFlashLevel::
	goto_if_defeated TRAINER_BRAWLY_1, DewfordTown_Gym_EventScript_SetLightsOn
	call DewfordTown_Gym_EventScript_CountTrainersDefeated
	copyvar VAR_0x8001, VAR_0x8000
	goto_if_eq VAR_0x8000, 0, DewfordTown_Gym_EventScript_SetFlashLevel7
	goto_if_eq VAR_0x8000, 1, DewfordTown_Gym_EventScript_SetFlashLevel6
	goto_if_eq VAR_0x8000, 2, DewfordTown_Gym_EventScript_SetFlashLevel5
	goto_if_eq VAR_0x8000, 3, DewfordTown_Gym_EventScript_SetFlashLevel4
	goto_if_eq VAR_0x8000, 4, DewfordTown_Gym_EventScript_SetFlashLevel3
	goto_if_eq VAR_0x8000, 5, DewfordTown_Gym_EventScript_SetFlashLevel2
	goto DewfordTown_Gym_EventScript_SetFlashLevel1

DewfordTown_Gym_EventScript_SetLightsOn::
	setflashlevel 0
	return

@ Brightest
DewfordTown_Gym_EventScript_SetFlashLevel1::
	setflashlevel 1
	return

DewfordTown_Gym_EventScript_SetFlashLevel2::
	setflashlevel 2
	return

DewfordTown_Gym_EventScript_SetFlashLevel3::
	setflashlevel 3
	return

DewfordTown_Gym_EventScript_SetFlashLevel4::
	setflashlevel 4
	return

DewfordTown_Gym_EventScript_SetFlashLevel5::
	setflashlevel 5
	return

DewfordTown_Gym_EventScript_SetFlashLevel6::
	setflashlevel 6
	return

@ Darkest
DewfordTown_Gym_EventScript_SetFlashLevel7::
	setflashlevel 7
	return

DewfordTown_Gym_EventScript_BrightenRoom::
	call DewfordTown_Gym_EventScript_CountTrainersDefeated
	nop1
	goto_if_eq VAR_0x8000, VAR_0x8001, DewfordTown_Gym_EventScript_NoLightChange
	copyvar VAR_0x8001, VAR_0x8000
	goto_if_eq VAR_0x8000, 1, DewfordTown_Gym_EventScript_AnimateFlash1Trainer
	goto_if_eq VAR_0x8000, 2, DewfordTown_Gym_EventScript_AnimateFlash2Trainers
	goto_if_eq VAR_0x8000, 3, DewfordTown_Gym_EventScript_AnimateFlash3Trainers
	goto_if_eq VAR_0x8000, 4, DewfordTown_Gym_EventScript_AnimateFlash4Trainers
	goto_if_eq VAR_0x8000, 5, DewfordTown_Gym_EventScript_AnimateFlash5Trainers
	goto_if_eq VAR_0x8000, 6, DewfordTown_Gym_EventScript_AnimateFlash6Trainers

DewfordTown_Gym_EventScript_NoLightChange::
	return

@ NOTE: A little confusingly, a larger animateflash param value is a smaller flash radius
DewfordTown_Gym_EventScript_AnimateFlash1Trainer::
	playse SE_SWITCH
	animateflash 6
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash2Trainers::
	playse SE_SWITCH
	animateflash 5
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash3Trainers::
	playse SE_SWITCH
	animateflash 4
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash4Trainers::
	playse SE_SWITCH
	animateflash 3
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash5Trainers::
	playse SE_SWITCH
	animateflash 2
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlash6Trainers::
	playse SE_SWITCH
	animateflash 1
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_AnimateFlashFullBrightness::
	playse SE_SWITCH
	animateflash 0
	call DewfordTown_Gym_EventScript_SetFlashLevel
	return

DewfordTown_Gym_EventScript_CountTrainersDefeated::
	setvar VAR_0x8000, 0
	goto_if_not_defeated TRAINER_TAKAO, DewfordTown_Gym_EventScript_CheckJocelyn
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckJocelyn::
	goto_if_not_defeated TRAINER_JOCELYN, DewfordTown_Gym_EventScript_CheckLaura
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckLaura::
	goto_if_not_defeated TRAINER_LAURA, DewfordTown_Gym_EventScript_CheckBrenden
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckBrenden::
	goto_if_not_defeated TRAINER_BRENDEN, DewfordTown_Gym_EventScript_CheckCristian
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckCristian::
	goto_if_not_defeated TRAINER_CRISTIAN, DewfordTown_Gym_EventScript_CheckLilith
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_CheckLilith::
	goto_if_not_defeated TRAINER_LILITH, DewfordTown_Gym_EventScript_StopCountingTrainers
	addvar VAR_0x8000, 1
DewfordTown_Gym_EventScript_StopCountingTrainers::
	return

DewfordTown_Gym_EventScript_Brawly::
	trainerbattle_double TRAINER_BRAWLY_1, DewfordTown_Gym_Text_BrawlyIntro, DewfordTown_Gym_Text_BrawlyDefeat, EmeraldChampions_Text_NeedTwoPokemon, DewfordTown_Gym_EventScript_BrawlyDefeated, NO_MUSIC
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, DewfordTown_Gym_EventScript_BrawlyRematch
	goto_if_unset FLAG_RECEIVED_BRAWLY_LUCARIONITE, DewfordTown_Gym_EventScript_GiveLucarionite2
	goto_if_set FLAG_HIDE_GRANITE_CAVE_STEVEN, DewfordTown_Gym_EventScript_BrawlyJourneyContinues
	msgbox DewfordTown_Gym_Text_BrawlyPostBattle, MSGBOX_DEFAULT
	release
	end

DewfordTown_Gym_EventScript_BrawlyDefeated::
	call DewfordTown_Gym_EventScript_AnimateFlashFullBrightness
	message DewfordTown_Gym_Text_ReceivedKnuckleBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox DewfordTown_Gym_Text_KnuckleBadgeInfoTakeThis, MSGBOX_DEFAULT
	setflag FLAG_DEFEATED_DEWFORD_GYM
	setflag FLAG_BADGE02_GET
	addvar VAR_PETALBURG_GYM_STATE, 1
	call_if_eq VAR_PETALBURG_GYM_STATE, 6, Common_EventScript_ReadyPetalburgGymForBattle
	setvar VAR_0x8008, 2
	call Common_EventScript_SetGymTrainers
	call DewfordTown_Gym_EventScript_GiveLucarionite
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox DewfordTown_Gym_Text_RegisteredBrawly, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_BRAWLY_MATCH_CALL
	msgbox DewfordTown_Gym_Text_BrawlyPostBattle, MSGBOX_DEFAULT
	setvar VAR_ROXANNE_CALL_STEP_COUNTER, 0
	setflag FLAG_ENABLE_ROXANNE_FIRST_CALL
	release
	end

DewfordTown_Gym_EventScript_GiveLucarionite::
	giveuniqueitem ITEM_LUCARIONITE
	goto_if_eq VAR_RESULT, 0, Common_EventScript_BagIsFull
	msgbox DewfordTown_Gym_Text_ExplainLucarionite, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_BRAWLY_LUCARIONITE
	return

DewfordTown_Gym_EventScript_GiveLucarionite2:
	giveuniqueitem ITEM_LUCARIONITE
	goto_if_eq VAR_RESULT, 0, Common_EventScript_ShowBagIsFull
	msgbox DewfordTown_Gym_Text_ExplainLucarionite, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_BRAWLY_LUCARIONITE
	release
	end

DewfordTown_Gym_EventScript_BrawlyRematch::
	trainerbattle_rematch_double TRAINER_BRAWLY_1, DewfordTown_Gym_Text_BrawlyPreRematch, DewfordTown_Gym_Text_BrawlyRematchDefeat, DewfordTown_Gym_Text_BrawlyRematchNeedTwoMons
	msgbox DewfordTown_Gym_Text_BrawlyPostRematch, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_Takao::
	trainerbattle_double TRAINER_TAKAO, DewfordTown_Gym_Text_TakaoIntro, DewfordTown_Gym_Text_TakaoDefeat, EmeraldChampions_Text_NeedTwoPokemon, DewfordTown_Gym_EventScript_TakaoBrightenRoom
	msgbox DewfordTown_Gym_Text_TakaoPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_TakaoBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Jocelyn::
	trainerbattle_double TRAINER_JOCELYN, DewfordTown_Gym_Text_JocelynIntro, DewfordTown_Gym_Text_JocelynDefeat, EmeraldChampions_Text_NeedTwoPokemon, DewfordTown_Gym_EventScript_JocelynBrightenRoom
	msgbox DewfordTown_Gym_Text_JocelynPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_JocelynBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Laura::
	trainerbattle_double TRAINER_LAURA, DewfordTown_Gym_Text_LauraIntro, DewfordTown_Gym_Text_LauraDefeat, EmeraldChampions_Text_NeedTwoPokemon, DewfordTown_Gym_EventScript_LauraBrightenRoom
	msgbox DewfordTown_Gym_Text_LauraPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_LauraBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Brenden::
	trainerbattle_double TRAINER_BRENDEN, DewfordTown_Gym_Text_BrendenIntro, DewfordTown_Gym_Text_BrendenDefeat, EmeraldChampions_Text_NeedTwoPokemon, DewfordTown_Gym_EventScript_BrendenBrightenRoom
	msgbox DewfordTown_Gym_Text_BrendenPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_BrendenBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Cristian::
	trainerbattle_double TRAINER_CRISTIAN, DewfordTown_Gym_Text_CristianIntro, DewfordTown_Gym_Text_CristianDefeat, EmeraldChampions_Text_NeedTwoPokemon, DewfordTown_Gym_EventScript_CristianBrightenRoom
	msgbox DewfordTown_Gym_Text_CristianPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_CristianBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_Lilith::
	trainerbattle_double TRAINER_LILITH, DewfordTown_Gym_Text_LilithIntro, DewfordTown_Gym_Text_LilithDefeat, EmeraldChampions_Text_NeedTwoPokemon, DewfordTown_Gym_EventScript_LilithBrightenRoom
	msgbox DewfordTown_Gym_Text_LilithPostBattle, MSGBOX_AUTOCLOSE
	end

DewfordTown_Gym_EventScript_LilithBrightenRoom::
	call DewfordTown_Gym_EventScript_BrightenRoom
	release
	end

DewfordTown_Gym_EventScript_GymGuide::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_DEWFORD_GYM, DewfordTown_Gym_EventScript_GymGuidePostVictory
	msgbox DewfordTown_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

DewfordTown_Gym_EventScript_GymGuidePostVictory::
	msgbox DewfordTown_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

DewfordTown_Gym_EventScript_LeftGymStatue::
	lockall
	goto_if_set FLAG_BADGE02_GET, DewfordTown_Gym_EventScript_GymStatueCertified
	goto DewfordTown_Gym_EventScript_GymStatue
	end

DewfordTown_Gym_EventScript_RightGymStatue::
	lockall
	goto_if_set FLAG_BADGE02_GET, DewfordTown_Gym_EventScript_GymStatueCertified
	goto DewfordTown_Gym_EventScript_GymStatue
	end

DewfordTown_Gym_EventScript_GymStatueCertified::
	msgbox DewfordTown_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

DewfordTown_Gym_EventScript_GymStatue::
	msgbox DewfordTown_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

DewfordTown_Gym_Text_GymGuideAdvice:
	.string "BRAWLY brings a powerful Fighting\n"
	.string "team, but his partners do more\l"
	.string "than attack.\p"
	.string "PACHIRISU draws single-target\n"
	.string "moves aside. His fighters use\l"
	.string "speed, priority and support to\l"
	.string "create openings.\p"
	.string "Normal Pokémon risk Fighting\n"
	.string "damage, but a useful support role\l"
	.string "can still earn a place on your\l"
	.string "team.\p"
	.string "The GYM grows brighter as you\n"
	.string "defeat its trainers. Make every\l"
	.string "action count!$"

DewfordTown_Gym_Text_GymGuidePostVictory:
	.string "Whoah! It's turned too bright in here!\p"
	.string "Your future is bright, too!$"

DewfordTown_Gym_Text_TakaoIntro:
	.string "GRAPPLOCT can hold a target in\n"
	.string "place. WOBBUFFET makes switching\l"
	.string "difficult too.\p"
	.string "Let us see how you handle the\n"
	.string "pressure!$"

DewfordTown_Gym_Text_TakaoDefeat:
	.string "You won a tough battle!$"

DewfordTown_Gym_Text_TakaoPostBattle:
	.string "HITMONLEE's WHITE HERB can clear\n"
	.string "its stat drops and activate\l"
	.string "UNBURDEN.\p"
	.string "Speed can change in the middle of\n"
	.string "a turn. Watch the whole board.$"

DewfordTown_Gym_Text_JocelynIntro:
	.string "TAILWIND can change the race in an\n"
	.string "instant.\p"
	.string "Four turns of pressure. Can your\n"
	.string "team handle them?$"

DewfordTown_Gym_Text_JocelynDefeat:
	.string "You took this battle!$"

DewfordTown_Gym_Text_JocelynPostBattle:
	.string "TAILWIND lasts four turns.\n"
	.string "BRELOOM's POISON HEAL can keep it\l"
	.string "going while the field changes.\p"
	.string "Plan for the partners still in\n"
	.string "reserve.$"

DewfordTown_Gym_Text_LauraIntro:
	.string "FAKE OUT steals the first turn, and\n"
	.string "GUTS makes my own burn a weapon.\p"
	.string "Can you afford to blink?$"

DewfordTown_Gym_Text_LauraDefeat:
	.string "That was a hard-fought win!$"

DewfordTown_Gym_Text_LauraPostBattle:
	.string "Blank FAKE OUT, pressure the Flame Orb,\n"
	.string "and never switch on instinct alone.$"

DewfordTown_Gym_Text_LilithIntro:
	.string "INTIMIDATE softens you, then SCRAGGY\n"
	.string "dances into the space it made.\p"
	.string "We build on every opening!$"

DewfordTown_Gym_Text_LilithDefeat:
	.string "You brought a winning team!$"

DewfordTown_Gym_Text_LilithPostBattle:
	.string "A paired battle is two complete teams,\n"
	.string "not one trick repeated twice.$"

DewfordTown_Gym_Text_BrendenIntro:
	.string "MACHOP's NO GUARD beats accuracy\n"
	.string "tricks. SAWK has MOLD BREAKER.\p"
	.string "There is no safe angle here!$"

DewfordTown_Gym_Text_BrendenDefeat:
	.string "We gave it everything!$"

DewfordTown_Gym_Text_BrendenPostBattle:
	.string "Do not let one physical wall become your\n"
	.string "answer to an entire FIGHTING school.$"

DewfordTown_Gym_Text_CristianIntro:
	.string "FALINKS strikes its own partner, and\n"
	.string "JUSTIFIED turns the blow into power.\p"
	.string "Hurt us and we sharpen!$"

DewfordTown_Gym_Text_CristianDefeat:
	.string "Our formation could not hold!$"

DewfordTown_Gym_Text_CristianPostBattle:
	.string "FALINKS can power up JUSTIFIED\n"
	.string "before its partner attacks.\p"
	.string "GALLADE and LUCARIO can both use\n"
	.string "that opening. Always check who is\l"
	.string "active.$"

DewfordTown_Gym_Text_BrawlyIntro:
	.string "Back from SLATEPORT? Right on!\p"
	.string "I'm BRAWLY, DEWFORD's GYM LEADER!\p"
	.string "My team draws attacks aside,\n"
	.string "then uses the space to set up.\p"
	.string "Don't give us time to build momentum.\n"
	.string "Once we're rolling, we're hard to stop!$"

DewfordTown_Gym_Text_BrawlyDefeat:
	.string "Whoa! You knocked me off balance!\p"
	.string "Take the KNUCKLE BADGE. You earned it!$"

DewfordTown_Gym_Text_ReceivedKnuckleBadge:
	.string "{PLAYER} received the KNUCKLE BADGE\n"
	.string "from BRAWLY.$"

DewfordTown_Gym_Text_KnuckleBadgeInfoTakeThis:
	.string "The KNUCKLE BADGE lets your LEVELER\n"
	.string "train a party up to Lv. 30.\p"
	.string "It also enables the hidden move FLASH\n"
	.string "outside of battle.\p"
	.string "Take this rare MEGA STONE, too.$"
DewfordTown_Gym_Text_ExplainLucarionite:
	.string "Lucarionite lets Lucario Mega Evolve.\n"
	.string "You'll soon receive the Ring it needs.$"

DewfordTown_Gym_Text_RegisteredBrawly:
	.string "Registered GYM LEADER BRAWLY\n"
	.string "in the POKéNAV.$"

DewfordTown_Gym_Text_BrawlyPostBattle:
	.string "Return to STEVEN in GRANITE CAVE,\n"
	.string "northwest of town, for your MEGA RING.\p"
	.string "Your new BADGE lets you use FLASH\n"
	.string "to light the way inside.$"

DewfordTown_Gym_Text_GymStatue:
	.string "DEWFORD TOWN POKéMON GYM$"

DewfordTown_Gym_Text_GymStatueCertified:
	.string "DEWFORD TOWN POKéMON GYM\p"
	.string "BRAWLY'S CERTIFIED TRAINERS:\n"
	.string "{PLAYER}$"

DewfordTown_Gym_Text_BrawlyPreRematch:
	.string "BRAWLY: A wave may draw back, but it\n"
	.string "always returns to the shore.\p"
	.string "A giant wave of a talent like you…\n"
	.string "I knew you would return!\p"
	.string "Show me how much higher\n"
	.string "you've gone!$"

DewfordTown_Gym_Text_BrawlyRematchDefeat:
	.string "Wow!\n"
	.string "Swamped again!$"

DewfordTown_Gym_Text_BrawlyPostRematch:
	.string "BRAWLY: Battling is vast and it's deep.\n"
	.string "You can't see the bottom.\l"
	.string "It's just like the sea of HOENN!$"

DewfordTown_Gym_Text_BrawlyRematchNeedTwoMons:
	.string "BRAWLY: A wave may draw back, but it\n"
	.string "always returns to the shore.\p"
	.string "A giant wave of a talent like you…\n"
	.string "I knew you would return!\p"
	.string "Oh? You only have one POKéMON\n"
	.string "that can battle.\p"
	.string "I'm sorry, but I'd like you to come back\n"
	.string "with at least two POKéMON.$"

DewfordTown_Gym_EventScript_BrawlyJourneyContinues::
	msgbox DewfordTown_Gym_Text_BrawlyPostRematch, MSGBOX_DEFAULT
	release
	end
