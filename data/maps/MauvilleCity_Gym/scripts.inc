MauvilleCity_Gym_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, MauvilleCity_Gym_OnLoad
	.byte 0

MauvilleCity_Gym_OnLoad:
	goto_if_set FLAG_DEFEATED_MAUVILLE_GYM, MauvilleCity_Gym_EventScript_DeactivatePuzzle
	switch VAR_MAUVILLE_GYM_STATE
	case 0, MauvilleCity_Gym_EventScript_UpdateBarriers
	case 1, MauvilleCity_Gym_EventScript_Switch1Pressed
	case 2, MauvilleCity_Gym_EventScript_Switch2Pressed
	case 3, MauvilleCity_Gym_EventScript_Switch3Pressed
	case 4, MauvilleCity_Gym_EventScript_Switch4Pressed
	end

MauvilleCity_Gym_EventScript_UpdateBarriers::
	goto_if_set FLAG_MAUVILLE_GYM_BARRIERS_STATE, MauvilleCity_Gym_EventScript_SetAltBarriers
	end

MauvilleCity_Gym_EventScript_SetAltBarriers::
	setmetatile 3, 11, METATILE_MauvilleGym_RedBeamV1_On, TRUE
	setmetatile 3, 12, METATILE_MauvilleGym_RedBeamV2_On, TRUE
	setmetatile 3, 13, METATILE_MauvilleGym_PoleTop_On, TRUE
	setmetatile 4, 10, METATILE_MauvilleGym_RedBeamH1_On, FALSE
	setmetatile 5, 10, METATILE_MauvilleGym_RedBeamH2_On, FALSE
	setmetatile 4, 11, METATILE_MauvilleGym_RedBeamH3_On, TRUE
	setmetatile 5, 11, METATILE_MauvilleGym_RedBeamH4_On, TRUE
	setmetatile 7, 10, METATILE_MauvilleGym_RedBeamH1_On, FALSE
	setmetatile 8, 10, METATILE_MauvilleGym_RedBeamH2_On, FALSE
	setmetatile 7, 11, METATILE_MauvilleGym_RedBeamH3_On, TRUE
	setmetatile 8, 11, METATILE_MauvilleGym_RedBeamH4_On, TRUE
	setmetatile 4, 13, METATILE_MauvilleGym_GreenBeamH1_Off, FALSE
	setmetatile 5, 13, METATILE_MauvilleGym_GreenBeamH2_Off, FALSE
	setmetatile 4, 14, METATILE_MauvilleGym_GreenBeamH3_Off, FALSE
	setmetatile 5, 14, METATILE_MauvilleGym_GreenBeamH4_Off, FALSE
	setmetatile 1, 10, METATILE_MauvilleGym_GreenBeamH1_Off, FALSE
	setmetatile 2, 10, METATILE_MauvilleGym_GreenBeamH2_Off, FALSE
	setmetatile 1, 11, METATILE_MauvilleGym_GreenBeamH3_Off, FALSE
	setmetatile 2, 11, METATILE_MauvilleGym_GreenBeamH4_Off, FALSE
	setmetatile 6, 8, METATILE_MauvilleGym_PoleBottom_On, TRUE
	setmetatile 6, 9, METATILE_MauvilleGym_FloorTile, FALSE
	setmetatile 6, 10, METATILE_MauvilleGym_PoleTop_Off, FALSE
	setmetatile 4, 6, METATILE_MauvilleGym_GreenBeamH1_Off, FALSE
	setmetatile 5, 6, METATILE_MauvilleGym_GreenBeamH2_Off, FALSE
	setmetatile 4, 7, METATILE_MauvilleGym_GreenBeamH3_Off, FALSE
	setmetatile 5, 7, METATILE_MauvilleGym_GreenBeamH4_Off, FALSE
	end

MauvilleCity_Gym_EventScript_Switch1Pressed::
	setvar VAR_0x8004, 0
	special MauvilleGymPressSwitch
	goto MauvilleCity_Gym_EventScript_UpdateBarriers
	end

MauvilleCity_Gym_EventScript_Switch2Pressed::
	setvar VAR_0x8004, 1
	special MauvilleGymPressSwitch
	goto MauvilleCity_Gym_EventScript_UpdateBarriers
	end

MauvilleCity_Gym_EventScript_Switch3Pressed::
	setvar VAR_0x8004, 2
	special MauvilleGymPressSwitch
	goto MauvilleCity_Gym_EventScript_UpdateBarriers
	end

MauvilleCity_Gym_EventScript_Switch4Pressed::
	setvar VAR_0x8004, 3
	special MauvilleGymPressSwitch
	goto MauvilleCity_Gym_EventScript_UpdateBarriers
	end

MauvilleCity_Gym_EventScript_DeactivatePuzzle::
	special MauvilleGymDeactivatePuzzle
	end

MauvilleCity_Gym_EventScript_Wattson::
	call EC_RequireMuseum
	call EC_RequireWally
	trainerbattle_double TRAINER_WATTSON_1, MauvilleCity_Gym_Text_WattsonIntro, MauvilleCity_Gym_Text_WattsonDefeat, EmeraldChampions_Text_NeedTwoPokemon, MauvilleCity_Gym_EventScript_WattsonDefeated, NO_MUSIC
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_Gym_EventScript_WattsonRematch
	call_if_unset FLAG_RECEIVED_WATTSON_MANECTITE, MauvilleCity_Gym_EventScript_GiveManectite
	goto_if_unset FLAG_RECEIVED_HM_STRENGTH, MauvilleCity_Gym_EventScript_ReunionHandoff
	goto_if_eq VAR_NEW_MAUVILLE_STATE, 2, MauvilleCity_Gym_EventScript_CompletedNewMauville
	goto_if_set FLAG_BADGE04_GET, MauvilleCity_Gym_EventScript_WattsonJourneyContinues
	goto_if_set FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, MauvilleCity_Gym_EventScript_WattsonLavaridge
	goto_if_set FLAG_MET_ARCHIE_METEOR_FALLS, MauvilleCity_Gym_EventScript_WattsonMtChimney
	call MauvilleCity_Gym_EventScript_NextNorthernStop
	release
	end

MauvilleCity_Gym_EventScript_WattsonMtChimney::
	msgbox MauvilleCity_Gym_Text_WattsonMtChimney, MSGBOX_DEFAULT
	release
	end

MauvilleCity_Gym_EventScript_WattsonLavaridge::
	msgbox MauvilleCity_Gym_Text_WattsonLavaridge, MSGBOX_DEFAULT
	release
	end

MauvilleCity_Gym_EventScript_WattsonDefeated::
	message MauvilleCity_Gym_Text_ReceivedDynamoBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	buffermilestonetext FLAG_BADGE03_GET
	msgbox MauvilleCity_Gym_Text_ExplainDynamoBadgeTakeThis, MSGBOX_DEFAULT
	setvar VAR_SLATEPORT_OUTSIDE_MUSEUM_STATE, 3
	clearflag FLAG_HIDE_VERDANTURF_TOWN_SCOTT
	setflag FLAG_DEFEATED_MAUVILLE_GYM
	completemilestone FLAG_BADGE03_GET
	addvar VAR_PETALBURG_GYM_STATE, 1
	call_if_eq VAR_PETALBURG_GYM_STATE, 6, Common_EventScript_ReadyPetalburgGymForBattle
	setvar VAR_0x8008, 3
	call Common_EventScript_SetGymTrainers
	special MauvilleGymDeactivatePuzzle
	special DrawWholeMapView
	playse SE_UNLOCK
	call MauvilleCity_Gym_EventScript_GiveManectite
	closemessage
	delay 30
	closemessage
	delay 30
	call MauvilleCity_Gym_EventScript_NextNorthernStop
	release
	end

MauvilleCity_Gym_EventScript_GiveManectite::
	checkitem ITEM_MEGA_RING, 1
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_Gym_EventScript_GiveManectiteNoRing
	giveuniqueitem ITEM_MANECTITE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_BagIsFull
	msgbox MauvilleCity_Gym_Text_ExplainManectite, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_WATTSON_MANECTITE
	return

MauvilleCity_Gym_EventScript_GiveManectiteNoRing::
	return

MauvilleCity_Gym_EventScript_CompletedNewMauville::
	msgbox MauvilleCity_Gym_Text_WattsonGoForthAndEndeavor, MSGBOX_DEFAULT
	release
	end

MauvilleCity_Gym_EventScript_WattsonRematch::
	trainerbattle_rematch_double TRAINER_WATTSON_1, MauvilleCity_Gym_Text_WattsonPreRematch, MauvilleCity_Gym_Text_WattsonRematchDefeat, MauvilleCity_Gym_Text_WattsonRematchNeedTwoMons
	msgbox MauvilleCity_Gym_Text_WattsonPostRematch, MSGBOX_AUTOCLOSE
	end

MauvilleCity_Gym_EventScript_Switch1::
	lockall
	goto_if_set FLAG_DEFEATED_MAUVILLE_GYM, MauvilleCity_Gym_EventScript_SwitchDoNothing
	goto_if_eq VAR_MAUVILLE_GYM_STATE, 1, MauvilleCity_Gym_EventScript_SwitchDoNothing
	setvar VAR_MAUVILLE_GYM_STATE, 1
	setvar VAR_0x8004, 0
	goto MauvilleCity_Gym_EventScript_PressFloorSwitch
	end

MauvilleCity_Gym_EventScript_Switch2::
	lockall
	goto_if_set FLAG_DEFEATED_MAUVILLE_GYM, MauvilleCity_Gym_EventScript_SwitchDoNothing
	goto_if_eq VAR_MAUVILLE_GYM_STATE, 2, MauvilleCity_Gym_EventScript_SwitchDoNothing
	setvar VAR_MAUVILLE_GYM_STATE, 2
	setvar VAR_0x8004, 1
	goto MauvilleCity_Gym_EventScript_PressFloorSwitch
	end

MauvilleCity_Gym_EventScript_Switch3::
	lockall
	goto_if_set FLAG_DEFEATED_MAUVILLE_GYM, MauvilleCity_Gym_EventScript_SwitchDoNothing
	goto_if_eq VAR_MAUVILLE_GYM_STATE, 3, MauvilleCity_Gym_EventScript_SwitchDoNothing
	setvar VAR_MAUVILLE_GYM_STATE, 3
	setvar VAR_0x8004, 2
	goto MauvilleCity_Gym_EventScript_PressFloorSwitch
	end

MauvilleCity_Gym_EventScript_Switch4::
	lockall
	goto_if_set FLAG_DEFEATED_MAUVILLE_GYM, MauvilleCity_Gym_EventScript_SwitchDoNothing
	goto_if_eq VAR_MAUVILLE_GYM_STATE, 4, MauvilleCity_Gym_EventScript_SwitchDoNothing
	setvar VAR_MAUVILLE_GYM_STATE, 4
	setvar VAR_0x8004, 3
	goto MauvilleCity_Gym_EventScript_PressFloorSwitch
	end

MauvilleCity_Gym_EventScript_PressFloorSwitch::
	special MauvilleGymSetDefaultBarriers
	special MauvilleGymPressSwitch
	special DrawWholeMapView
	playse SE_UNLOCK
	goto_if_set FLAG_MAUVILLE_GYM_BARRIERS_STATE, MauvilleCity_Gym_EventScript_ClearBarriersAltState
	goto_if_unset FLAG_MAUVILLE_GYM_BARRIERS_STATE, MauvilleCity_Gym_EventScript_SetBarriersAltState
	releaseall
	end

MauvilleCity_Gym_EventScript_SwitchDoNothing::
	releaseall
	end

MauvilleCity_Gym_EventScript_SetBarriersAltState::
	setflag FLAG_MAUVILLE_GYM_BARRIERS_STATE
	releaseall
	end

MauvilleCity_Gym_EventScript_ClearBarriersAltState::
	clearflag FLAG_MAUVILLE_GYM_BARRIERS_STATE
	releaseall
	end

MauvilleCity_Gym_EventScript_Kirk::
	trainerbattle_double TRAINER_KIRK, MauvilleCity_Gym_Text_KirkIntro, MauvilleCity_Gym_Text_KirkDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MauvilleCity_Gym_Text_KirkPostBattle, MSGBOX_AUTOCLOSE
	end

MauvilleCity_Gym_EventScript_Shawn::
	trainerbattle_double TRAINER_SHAWN, MauvilleCity_Gym_Text_ShawnIntro, MauvilleCity_Gym_Text_ShawnDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MauvilleCity_Gym_Text_ShawnPostBattle, MSGBOX_AUTOCLOSE
	end

MauvilleCity_Gym_EventScript_Ben::
	trainerbattle_double TRAINER_BEN, MauvilleCity_Gym_Text_BenIntro, MauvilleCity_Gym_Text_BenDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MauvilleCity_Gym_Text_BenPostBattle, MSGBOX_AUTOCLOSE
	end

MauvilleCity_Gym_EventScript_Vivian::
	trainerbattle_double TRAINER_VIVIAN, MauvilleCity_Gym_Text_VivianIntro, MauvilleCity_Gym_Text_VivianDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MauvilleCity_Gym_Text_VivianPostBattle, MSGBOX_AUTOCLOSE
	end

MauvilleCity_Gym_EventScript_Angelo::
	trainerbattle_double TRAINER_ANGELO, MauvilleCity_Gym_Text_AngeloIntro, MauvilleCity_Gym_Text_AngeloDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MauvilleCity_Gym_Text_AngeloPostBattle, MSGBOX_AUTOCLOSE
	end

MauvilleCity_Gym_EventScript_GymGuide::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_MAUVILLE_GYM, MauvilleCity_Gym_EventScript_GymGuidePostVictory
	msgbox MauvilleCity_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

MauvilleCity_Gym_EventScript_GymGuidePostVictory::
	msgbox MauvilleCity_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

MauvilleCity_Gym_EventScript_LeftGymStatue::
	lockall
	goto_if_set FLAG_BADGE03_GET, MauvilleCity_Gym_EventScript_GymStatueCertified
	goto MauvilleCity_Gym_EventScript_GymStatue
	end

MauvilleCity_Gym_EventScript_RightGymStatue::
	lockall
	goto_if_set FLAG_BADGE03_GET, MauvilleCity_Gym_EventScript_GymStatueCertified
	goto MauvilleCity_Gym_EventScript_GymStatue
	end

MauvilleCity_Gym_EventScript_GymStatueCertified::
	msgbox MauvilleCity_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

MauvilleCity_Gym_EventScript_GymStatue::
	msgbox MauvilleCity_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

MauvilleCity_Gym_Text_GymGuideAdvice:
	.string "Hey, how's it going, CHAMPION-\n"
	.string "bound {PLAYER}?\p"
	.string "WATTSON, the LEADER of MAUVILLE\n"
	.string "GYM, uses ELECTRIC-type POKéMON.\p"
	.string "If you challenge him with WATER-type\n"
	.string "POKéMON, he'll zap them! Bzzt!\p"
	.string "And, he's put in switch-controlled\n"
	.string "doors all over his GYM! Eccentric!\p"
	.string "Hey, go for it!$"

MauvilleCity_Gym_Text_GymGuidePostVictory:
	.string "Whoa, you're electrifying!\n"
	.string "You've powered the door open!$"

MauvilleCity_Gym_Text_KirkIntro:
	.string "PINCURCHIN floors the whole stage,\n"
	.string "and EMOLGA turns bolts into speed.\p"
	.string "Find the current's blind spot!$"

MauvilleCity_Gym_Text_KirkDefeat:
	.string "You shut down my amplifier!$"

MauvilleCity_Gym_Text_KirkPostBattle:
	.string "ELECTRIC TERRAIN protects grounded\n"
	.string "Pokémon from sleep.\p"
	.string "A Flying Pokémon or an AIR BALLOON\n"
	.string "changes which benefits apply.$"

MauvilleCity_Gym_Text_ShawnIntro:
	.string "Some of my Pokémon float above\n"
	.string "Ground attacks. Others use the\l"
	.string "terrain beneath them.\p"
	.string "Can you tell which answer fits\n"
	.string "each pair?$"

MauvilleCity_Gym_Text_ShawnDefeat:
	.string "You broke our live connection…$"

MauvilleCity_Gym_Text_ShawnPostBattle:
	.string "BELLIBOLT can set ELECTRIC TERRAIN\n"
	.string "for RAICHU's SURGE SURFER.\p"
	.string "MEGA AMPHAROS brings a different\n"
	.string "pace and a Dragon typing.$"

MauvilleCity_Gym_Text_BenIntro:
	.string "Send electricity, and LANTURN\n"
	.string "drinks it. Send water, and\l"
	.string "HELIOLISK drinks that.\p"
	.string "What else can your partners do?$"

MauvilleCity_Gym_Text_BenDefeat:
	.string "Our circuit couldn't hold!$"

MauvilleCity_Gym_Text_BenPostBattle:
	.string "CHARJABUG powers special attacks\n"
	.string "with BATTERY. DISCHARGE can even\l"
	.string "heal LANTURN.\p"
	.string "That same move is not safe beside\n"
	.string "every partner.$"

MauvilleCity_Gym_Text_VivianIntro:
	.string "PLUSLE and MINUN amplify each other\n"
	.string "just by standing side by side.\p"
	.string "Split us, or lose to both!$"

MauvilleCity_Gym_Text_VivianDefeat:
	.string "Our partnership wasn't enough!$"

MauvilleCity_Gym_Text_VivianPostBattle:
	.string "Do not press ELECTRIC attacks into the\n"
	.string "Ability that wants to absorb them.$"

MauvilleCity_Gym_Text_AngeloIntro:
	.string "JOLTIK makes THUNDER more\n"
	.string "reliable, and TOXTRICITY turns\l"
	.string "sound into power.\p"
	.string "We are ready to take our chances!$"

MauvilleCity_Gym_Text_AngeloDefeat:
	.string "Our relay lost its charge…$"

MauvilleCity_Gym_Text_AngeloPostBattle:
	.string "WATTREL can set TAILWIND, and\n"
	.string "JOLTIK can slow opponents with\l"
	.string "ELECTROWEB.\p"
	.string "Those moves change speed in\n"
	.string "different ways.$"

MauvilleCity_Gym_Text_WattsonIntro:
	.string "Wahahahah! I am WATTSON!\p"
	.string "ELECTRODE sends the current.\n"
	.string "ELECTIVIRE turns it into speed.\p"
	.string "I know how my partners work\n"
	.string "together. Show me what yours can\l"
	.string "do!$"

MauvilleCity_Gym_Text_WattsonDefeat:
	.string "Wahahah! What a battle!\p"
	.string "You have earned the DYNAMO BADGE!$"

MauvilleCity_Gym_Text_ReceivedDynamoBadge:
	.string "{PLAYER} received the DYNAMO BADGE\n"
	.string "from WATTSON.$"

MauvilleCity_Gym_Text_ExplainDynamoBadgeTakeThis:
	.string "The DYNAMO BADGE lets your LEVELER\n"
	.string "train a party up to Lv. {STR_VAR_1}.\p"
	.string "It also enables ROCK SMASH outside\n"
	.string "of battle.\p"
	.string "Hmm…\n"
	.string "You should take this, too!$"
MauvilleCity_Gym_Text_ExplainManectite:
	.string "Manectite lets Manectric Mega Evolve.\n"
	.string "Let it hold the stone in battle.$"

MauvilleCity_Gym_Text_WattsonPostBattle:
	.string "Head north on ROUTE 111. My crew\n"
	.string "will let you through with that BADGE.\p"
	.string "Turn west onto ROUTE 112 and go\n"
	.string "through FIERY PATH to ROUTE 113.\p"
	.string "Follow it west to FALLARBOR TOWN.\n"
	.string "Ask there about the trouble at\p"
	.string "METEOR FALLS, down ROUTE 114.$"

MauvilleCity_Gym_Text_WattsonMtChimney:
	.string "Head north on ROUTE 111, then west\n"
	.string "onto ROUTE 112.\p"
	.string "Take the cable car up MT. CHIMNEY.\n"
	.string "Stop TEAM MAGMA at the crater.$"

MauvilleCity_Gym_Text_WattsonLavaridge:
	.string "Take ROUTE 112's cable car up\n"
	.string "MT. CHIMNEY, then descend JAGGED PASS.\p"
	.string "That leads to LAVARIDGE TOWN.\n"
	.string "Challenge FLANNERY at the GYM.$"

MauvilleCity_Gym_Text_WattsonGoForthAndEndeavor:
	.string "Wahahahah!\n"
	.string "Go forth and endeavor, youngster!$"

MauvilleCity_Gym_Text_GymStatue:
	.string "MAUVILLE CITY POKéMON GYM$"

MauvilleCity_Gym_Text_GymStatueCertified:
	.string "MAUVILLE CITY POKéMON GYM\p"
	.string "WATTSON'S CERTIFIED TRAINERS:\n"
	.string "{PLAYER}$"

MauvilleCity_Gym_Text_WattsonPreRematch:
	.string "WATTSON: Ah-ha! Here at last!\n"
	.string "I know what you want.\l"
	.string "You want to battle my POKéMON!\p"
	.string "Wahahahaha!\p"
	.string "I'll make sparks fly from you!\n"
	.string "Don't say I didn't warn you!$"

MauvilleCity_Gym_Text_WattsonRematchDefeat:
	.string "Oof…\n"
	.string "Our batteries ran dry…$"

MauvilleCity_Gym_Text_WattsonPostRematch:
	.string "WATTSON: We'll have to recharge our\n"
	.string "batteries again.\p"
	.string "When we're fully charged up, we'll\n"
	.string "gladly accept your challenge.\p"
	.string "So, come back again sometime,\n"
	.string "won't you?$"

MauvilleCity_Gym_Text_WattsonRematchNeedTwoMons:
	.string "WATTSON: Ah-ha! Here at last!\n"
	.string "I know what you want.\l"
	.string "You want to battle my POKéMON!\p"
	.string "Wahahahaha!\p"
	.string "Oops! Wait!\n"
	.string "You've only one POKéMON with you!\p"
	.string "Come see me with two or more\n"
	.string "POKéMON, all right?$"

MauvilleCity_Gym_EventScript_WattsonJourneyContinues::
	msgbox MauvilleCity_Gym_Text_WattsonPostRematch, MSGBOX_DEFAULT
	release
	end

MauvilleCity_Gym_EventScript_ReunionHandoff::
	call MauvilleCity_Gym_EventScript_NextNorthernStop
	release
	end

MauvilleCity_Gym_EventScript_NextNorthernStop::
	goto_if_unset FLAG_RECEIVED_HM_ROCK_SMASH, MauvilleCity_Gym_EventScript_FindRockSmash
	goto_if_unset FLAG_RECEIVED_HM_STRENGTH, MauvilleCity_Gym_EventScript_FindStrength
	msgbox MauvilleCity_Gym_Text_WattsonPostBattle, MSGBOX_DEFAULT
	return
MauvilleCity_Gym_EventScript_FindRockSmash::
	msgbox MauvilleCity_Gym_Text_FindRockSmash, MSGBOX_DEFAULT
	return
MauvilleCity_Gym_EventScript_FindStrength::
	msgbox MauvilleCity_Gym_Text_FindStrength, MSGBOX_DEFAULT
	return

MauvilleCity_Gym_Text_FindRockSmash:
	.string "Visit the ROCK SMASH man in\n"
	.string "MAUVILLE's southeastern house.\p"
	.string "His license and your DYNAMO BADGE\n"
	.string "let a compatible partner break rocks.\p"
	.string "Galarian FARFETCH'D on ROUTE 117\n"
	.string "can help. Head west to VERDANTURF and\l"
	.string "reunite the couple in RUSTURF TUNNEL.$"
MauvilleCity_Gym_Text_FindStrength:
	.string "Head west along ROUTE 117 to\n"
	.string "VERDANTURF. Enter RUSTURF TUNNEL.\p"
	.string "Use ROCK SMASH on the rocks\n"
	.string "separating the couple there.\p"
	.string "Galarian FARFETCH'D can do it.\n"
	.string "Find one in ROUTE 117's grass.\p"
	.string "The tunneler will give you STRENGTH.\p"
	.string "Then return to MAUVILLE and head\n"
	.string "north on ROUTE 111.$"
