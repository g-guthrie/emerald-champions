.set LOCALID_COLE, 2
.set LOCALID_GERALD, 3
.set LOCALID_AXLE, 4
.set LOCALID_DANIELLE, 5

LavaridgeTown_Gym_1F_MapScripts:: @ 81FE6F4
	map_script MAP_SCRIPT_ON_TRANSITION, LavaridgeTown_Gym_1F_OnTransition
	.byte 0

LavaridgeTown_Gym_1F_OnTransition: @ 81FE6FA
	call LavaridgeTown_Gym_1F_EventScript_SetTrainerTempVars
	call LavaridgeTown_Gym_1F_EventScript_CheckBuryTrainers
	end

@ Unclear where/if these temp vars are getting checked
LavaridgeTown_Gym_1F_EventScript_SetTrainerTempVars:: @ 81FE705
	setvar VAR_TEMP_B, 0
	setvar VAR_TEMP_C, 0
	setvar VAR_TEMP_D, 0
	setvar VAR_TEMP_E, 0
	setvar VAR_TEMP_F, 0
	goto_if_defeated TRAINER_COLE LavaridgeTown_Gym_1F_EventScript_SetGeraldTempVar
	setvar VAR_TEMP_B, 1
LavaridgeTown_Gym_1F_EventScript_SetGeraldTempVar:: @ 81FE72C
	goto_if_defeated TRAINER_GERALD, LavaridgeTown_Gym_1F_EventScript_SetAxleTempVar
	setvar VAR_TEMP_C, 1
LavaridgeTown_Gym_1F_EventScript_SetAxleTempVar:: @ 81FE73A
	goto_if_defeated TRAINER_AXLE, LavaridgeTown_Gym_1F_EventScript_SetDanielleTempVar
	setvar VAR_TEMP_D, 1
LavaridgeTown_Gym_1F_EventScript_SetDanielleTempVar:: @ 81FE748
	goto_if_defeated TRAINER_DANIELLE, LavaridgeTown_Gym_1F_EventScript_EndSetTrainerTempVars
	setvar VAR_TEMP_E, 1
LavaridgeTown_Gym_1F_EventScript_EndSetTrainerTempVars:: @ 81FE756
	return

LavaridgeTown_Gym_1F_EventScript_CheckBuryTrainers:: @ 81FE757
	goto_if_defeated TRAINER_COLE, LavaridgeTown_Gym_1F_EventScript_CheckBuryGerald
	setobjectmovementtype LOCALID_COLE, MOVEMENT_TYPE_BURIED
LavaridgeTown_Gym_1F_EventScript_CheckBuryGerald:: @ 81FE764
	goto_if_defeated TRAINER_GERALD, LavaridgeTown_Gym_1F_EventScript_CheckBuryAxle
	setobjectmovementtype LOCALID_GERALD, MOVEMENT_TYPE_BURIED
LavaridgeTown_Gym_1F_EventScript_CheckBuryAxle:: @ 81FE771
	goto_if_defeated TRAINER_AXLE, LavaridgeTown_Gym_1F_EventScript_CheckBuryDanielle
	setobjectmovementtype LOCALID_AXLE, MOVEMENT_TYPE_BURIED
LavaridgeTown_Gym_1F_EventScript_CheckBuryDanielle:: @ 81FE77E
	goto_if_defeated TRAINER_DANIELLE, LavaridgeTown_Gym_1F_EventScript_EndCheckBuryTrainers
	setobjectmovementtype LOCALID_DANIELLE, MOVEMENT_TYPE_BURIED
LavaridgeTown_Gym_1F_EventScript_EndCheckBuryTrainers:: @ 81FE78B
	return

LavaridgeTown_Gym_1F_EventScript_Flannery:: @ 81FE78C
	goto_if_set FLAG_DAILY_REMATCH_FLANNERY, LavaridgeTown_Gym_1F_EventScript_AlreadyRematched
	goto_if_set FLAG_SYS_GAME_CLEAR, LavaridgeTown_Gym_1F_EventScript_FlanneryRematch
	trainerbattle_single TRAINER_FLANNERY_1, LavaridgeTown_Gym_1F_Text_FlanneryIntro, LavaridgeTown_Gym_1F_Text_FlanneryDefeat, LavaridgeTown_Gym_1F_EventScript_FlanneryDefeated, NO_MUSIC
	goto_if_unset FLAG_RECEIVED_TM50, LavaridgeTown_Gym_1F_EventScript_GiveOverheat2
	msgbox LavaridgeTown_Gym_1F_Text_FlanneryPostBattle, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Gym_1F_EventScript_FlanneryDefeated:: @ 81FE7C1
	message LavaridgeTown_Gym_1F_Text_ReceivedHeatBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox LavaridgeTown_Gym_1F_Text_ExplainHeatBadgeTakeThis, MSGBOX_DEFAULT
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox LavaridgeTown_Gym_1F_Text_RegisteredArcanine, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	msgbox LavaridgeTown_Gym_1F_Text_TakeThis, MSGBOX_DEFAULT
	setflag FLAG_WHITEOUT_TO_LAVARIDGE
	setflag FLAG_DEFEATED_LAVARIDGE_GYM
	setflag FLAG_BADGE04_GET
	addvar VAR_PETALBURG_GYM_STATE, 1
	compare VAR_PETALBURG_GYM_STATE, 6
	call_if_eq Common_EventScript_ReadyPetalburgGymForBattle
	setvar VAR_0x8008, 4
	call Common_EventScript_SetGymTrainers
	setflag FLAG_HIDE_VERDANTURF_TOWN_WANDAS_HOUSE_WALLY
	setvar VAR_LAVARIDGE_TOWN_STATE, 1
	call LavaridgeTown_Gym_1F_EventScript_GiveOverheat
	closemessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox LavaridgeTown_Gym_1F_Text_RegisteredFlannery, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_FLANNERY_MATCH_CALL
	release
	end

LavaridgeTown_Gym_1F_EventScript_GiveOverheat2:: @ 81FE81D
	giveitem ITEM_MAGMARIZER
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	msgbox LavaridgeTown_Gym_1F_Text_ExplainOverheat, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_TM50
	release
	end

LavaridgeTown_Gym_1F_EventScript_GiveOverheat:: @ 81FE841
	giveitem ITEM_MAGMARIZER
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_BagIsFull
	msgbox LavaridgeTown_Gym_1F_Text_ExplainOverheat, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_TM50
	return

LavaridgeTown_Gym_1F_EventScript_FlanneryRematch::
	lockall
	faceplayer
	message LavaridgeTown_Gym_1F_Text_FlanneryChooseFormat
	waitmessage
	multichoice 17, 6, MULTI_REMATCH_BATTLE_MODE, 0
	switch VAR_RESULT
	case 0, LavaridgeTown_Gym_1F_EventScript_SetSingles
	case 1, LavaridgeTown_Gym_1F_EventScript_SetDoubles
	case 2, LavaridgeTown_Gym_1F_EventScript_Exit
	case 127, LavaridgeTown_Gym_1F_EventScript_Exit

@ 2: singles, no legendaries
@ 3: singles, legendaries
@ 4: doubles, legendaries
@ 5: doubles, no legendaries

LavaridgeTown_Gym_1F_EventScript_SetSingles::
	msgbox LavaridgeTown_Gym_1F_Text_FlanneryAllowLegends, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq LavaridgeTown_Gym_1F_EventScript_FlannerySinglesRematch
	trainerbattle_no_intro TRAINER_FLANNERY_3, LavaridgeTown_Gym_1F_Text_FlanneryRematchDefeat
	setflag FLAG_DAILY_REMATCH_FLANNERY
	releaseall
	end

LavaridgeTown_Gym_1F_EventScript_SetDoubles::
	msgbox LavaridgeTown_Gym_1F_Text_FlanneryAllowLegends, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq LavaridgeTown_Gym_1F_EventScript_FlanneryDoublesRematch
	trainerbattle_no_intro TRAINER_FLANNERY_4, LavaridgeTown_Gym_1F_Text_FlanneryRematchDefeat
	setflag FLAG_DAILY_REMATCH_FLANNERY
	releaseall
	end

LavaridgeTown_Gym_1F_EventScript_FlannerySinglesRematch::
	trainerbattle_no_intro TRAINER_FLANNERY_2, LavaridgeTown_Gym_1F_Text_FlanneryRematchDefeat
	setflag FLAG_DAILY_REMATCH_FLANNERY
	releaseall
	end

LavaridgeTown_Gym_1F_EventScript_FlanneryDoublesRematch::
	trainerbattle_no_intro TRAINER_FLANNERY_5, LavaridgeTown_Gym_1F_Text_FlanneryRematchDefeat
	setflag FLAG_DAILY_REMATCH_FLANNERY
	releaseall
	end

LavaridgeTown_Gym_1F_EventScript_AlreadyRematched::
	msgbox LavaridgeTown_Gym_1F_Text_FlanneryPostRematch, MSGBOX_AUTOCLOSE
	releaseall
	end

LavaridgeTown_Gym_1F_EventScript_Exit::
	releaseall
	end

LavaridgeTown_Gym_1F_EventScript_Cole:: @ 81FE87F
	trainerbattle TRAINER_BATTLE_CONTINUE_SCRIPT, TRAINER_COLE, LOCALID_COLE, LavaridgeTown_Gym_1F_Text_ColeIntro, LavaridgeTown_Gym_1F_Text_ColeDefeat, LavaridgeTown_Gym_EventScript_CheckTrainerScript
	msgbox LavaridgeTown_Gym_1F_Text_ColePostBattle MSGBOX_AUTOCLOSE
	end

LavaridgeTown_Gym_EventScript_CheckTrainerScript:: @ 81FE89A
	call LavaridgeTown_Gym_1F_EventScript_SetTrainerTempVars
	release
	special ShouldTryGetTrainerScript
	compare VAR_RESULT, 1
	goto_if_eq EventScript_GotoTrainerScript
	end

LavaridgeTown_Gym_1F_EventScript_Axle:: @ 81FE8AF
	trainerbattle TRAINER_BATTLE_CONTINUE_SCRIPT, TRAINER_AXLE, LOCALID_AXLE, LavaridgeTown_Gym_1F_Text_AxleIntro, LavaridgeTown_Gym_1F_Text_AxleDefeat, LavaridgeTown_Gym_EventScript_CheckTrainerScript
	msgbox LavaridgeTown_Gym_1F_Text_AxlePostBattle, MSGBOX_AUTOCLOSE
	end

LavaridgeTown_Gym_B1F_EventScript_Keegan:: @ 81FE8CA
	trainerbattle TRAINER_BATTLE_CONTINUE_SCRIPT, TRAINER_KEEGAN, LOCALID_KEEGAN, LavaridgeTown_Gym_B1F_Text_KeeganIntro, LavaridgeTown_Gym_B1F_Text_KeeganDefeat, LavaridgeTown_Gym_EventScript_CheckTrainerScript
	msgbox LavaridgeTown_Gym_B1F_Text_KeeganPostBattle, MSGBOX_AUTOCLOSE
	end

LavaridgeTown_Gym_1F_EventScript_Danielle:: @ 81FE8E5
	trainerbattle TRAINER_BATTLE_CONTINUE_SCRIPT, TRAINER_DANIELLE, LOCALID_DANIELLE, LavaridgeTown_Gym_1F_Text_DanielleIntro, LavaridgeTown_Gym_1F_Text_DanielleDefeat, LavaridgeTown_Gym_EventScript_CheckTrainerScript
	msgbox LavaridgeTown_Gym_1F_Text_DaniellePostBattle, MSGBOX_AUTOCLOSE
	end

LavaridgeTown_Gym_1F_EventScript_Gerald:: @ 81FE900
	trainerbattle TRAINER_BATTLE_CONTINUE_SCRIPT, TRAINER_GERALD, LOCALID_GERALD, LavaridgeTown_Gym_1F_Text_GeraldIntro, LavaridgeTown_Gym_1F_Text_GeraldDefeat, LavaridgeTown_Gym_EventScript_CheckTrainerScript
	msgbox LavaridgeTown_Gym_1F_Text_GeraldPostBattle, MSGBOX_AUTOCLOSE
	end

LavaridgeTown_Gym_B1F_EventScript_Jace:: @ 81FE91B
	trainerbattle TRAINER_BATTLE_CONTINUE_SCRIPT, TRAINER_JACE, LOCALID_JACE, LavaridgeTown_Gym_B1F_Text_JaceIntro, LavaridgeTown_Gym_B1F_Text_JaceDefeat, LavaridgeTown_Gym_EventScript_CheckTrainerScript
	msgbox LavaridgeTown_Gym_B1F_Text_JacePostBattle, MSGBOX_AUTOCLOSE
	end

LavaridgeTown_Gym_B1F_EventScript_Jeff:: @ 81FE936
	trainerbattle TRAINER_BATTLE_CONTINUE_SCRIPT, TRAINER_JEFF, LOCALID_JEFF, LavaridgeTown_Gym_B1F_Text_JeffIntro, LavaridgeTown_Gym_B1F_Text_JeffDefeat, LavaridgeTown_Gym_EventScript_CheckTrainerScript
	msgbox LavaridgeTown_Gym_B1F_Text_JeffPostBattle, MSGBOX_AUTOCLOSE
	end

LavaridgeTown_Gym_B1F_EventScript_Eli:: @ 81FE951
	trainerbattle TRAINER_BATTLE_CONTINUE_SCRIPT, TRAINER_ELI, LOCALID_ELI, LavaridgeTown_Gym_B1F_Text_EliIntro, LavaridgeTown_Gym_B1F_Text_EliDefeat, LavaridgeTown_Gym_EventScript_CheckTrainerScript
	msgbox LavaridgeTown_Gym_B1F_Text_EliPostBattle, MSGBOX_AUTOCLOSE
	end

LavaridgeTown_Gym_1F_EventScript_GymGuide:: @ 81FE96C
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_LAVARIDGE_GYM, LavaridgeTown_Gym_1F_EventScript_GymGuidePostVictory
	goto_if_set FLAG_LAVARIDGE_GYM_ENERGY_ROOTS, LavaridgeTown_Gym_1F_EventScript_GymGuide2
	msgbox LavaridgeTown_Gym_1F_Text_GymGuideAdvice1, MSGBOX_DEFAULT
	giveitem ITEM_ENERGY_ROOT, 15
	setflag FLAG_LAVARIDGE_GYM_ENERGY_ROOTS
	msgbox LavaridgeTown_Gym_1F_Text_GymGuideAdvice2, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Gym_1F_EventScript_GymGuide2::
	msgbox LavaridgeTown_Gym_1F_Text_GymGuideAdvice2, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Gym_1F_EventScript_GymGuidePostVictory:: @ 81FE981
	msgbox LavaridgeTown_Gym_1F_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Gym_1F_EventScript_LeftGymStatue:: @ 81FE98B
	lockall
	goto_if_set FLAG_BADGE04_GET, LavaridgeTown_Gym_1F_EventScript_GymStatueCertified
	goto LavaridgeTown_Gym_1F_EventScript_GymStatue
	end

LavaridgeTown_Gym_1F_EventScript_RightGymStatue:: @ 81FE99B
	lockall
	goto_if_set FLAG_BADGE04_GET, LavaridgeTown_Gym_1F_EventScript_GymStatueCertified
	goto LavaridgeTown_Gym_1F_EventScript_GymStatue
	end

LavaridgeTown_Gym_1F_EventScript_GymStatueCertified:: @ 81FE9AB
	msgbox LavaridgeTown_Gym_1F_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

LavaridgeTown_Gym_1F_EventScript_GymStatue:: @ 81FE9B5
	msgbox LavaridgeTown_Gym_1F_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

LavaridgeTown_Gym_1F_Text_GymGuideAdvice1:
	.string "Hey, how's it going, Champion-\n"
	.string "bound {PLAYER}?\p"
	.string "Please, take this herbal medicine\n"
	.string "to help with your challenge.$"

LavaridgeTown_Gym_1F_Text_GymGuideAdvice2: @ 81FE9BF
	.string "Flannery battles at two speeds.\p"
	.string "Lilligant pulls Torkoal forward\n"
	.string "with After You and sun.\p"
	.string "Chip Torkoal to weaken Eruption,\n"
	.string "or replace its weather.\p"
	.string "Delphox may reverse the order.\n"
	.string "Taunt it, or exploit Trick Room.\p"
	.string "Heatran floats until its Balloon\n"
	.string "breaks. Mega Emboar has no Protect.$"

LavaridgeTown_Gym_1F_Text_GymGuidePostVictory: @ 81FEAB8
	.string "Yow! What a scorching-hot battle!$"

LavaridgeTown_Gym_1F_Text_ColeIntro: @ 81FEADE
	.string "Hot sand stores heat underfoot!\p"
	.string "Chandelure draws it in.\n"
	.string "Incineroar carries it forward,\l"
	.string "and Simisear waits for a flare!$"

LavaridgeTown_Gym_1F_Text_ColeDefeat: @ 81FEAFB
	.string "The stored heat escaped!$"

LavaridgeTown_Gym_1F_Text_ColePostBattle: @ 81FEB1C
	.string "Gerald can target Chandelure with\n"
	.string "Flamethrower to feed Flash Fire.\p"
	.string "Alone, each of my three still burns.$"

LavaridgeTown_Gym_1F_Text_AxleIntro: @ 81FEB7B
	.string "Pressure has to escape somewhere!\p"
	.string "Drifblim pops, Magcargo vents,\l"
	.string "Heatmor ejects, and Ho-Oh resets!$"

LavaridgeTown_Gym_1F_Text_AxleDefeat: @ 81FEBC0
	.string "All my pressure escaped at once!$"

LavaridgeTown_Gym_1F_Text_AxlePostBattle: @ 81FEBE1
	.string "Policy triggers Unburden. White Herb\n"
	.string "cleans up Simple Shell Smash.\p"
	.string "Eject Pack pivots after Overheat,\l"
	.string "Regenerator rewards Ho-Oh's exit.$"

LavaridgeTown_Gym_B1F_Text_KeeganIntro: @ 81FEC31
	.string "Heat tests willpower, not weather!\p"
	.string "Vigoroth and Magmortar won't sleep.\n"
	.string "Typhlosion must stay healthy to keep\l"
	.string "Eruption at full strength!$"

LavaridgeTown_Gym_B1F_Text_KeeganDefeat: @ 81FECE3
	.string "Your will outlasted mine!$"

LavaridgeTown_Gym_B1F_Text_KeeganPostBattle: @ 81FED02
	.string "Vital Spirit blocks sleep. Eviolite\n"
	.string "and Assault Vest reward endurance.\p"
	.string "Chip Typhlosion early to weaken\n"
	.string "its Choice Scarf Eruption.$"

LavaridgeTown_Gym_1F_Text_GeraldIntro: @ 81FED72
	.string "392 degrees is only the first test!\p"
	.string "Arcanine ignites, Charizard lifts,\l"
	.string "and Reshiram brings the furnace!$"

LavaridgeTown_Gym_1F_Text_GeraldDefeat: @ 81FED9E
	.string "Your answer burned hotter!$"

LavaridgeTown_Gym_1F_Text_GeraldPostBattle: @ 81FEDBB
	.string "Flamethrower can boost an allied\n"
	.string "Flash Fire user without harm.\p"
	.string "Without Cole, my three attack alone.$"

LavaridgeTown_Gym_1F_Text_DanielleIntro: @ 81FEE22
	.string "Strength can still be beautiful!\p"
	.string "Volcarona dances; Oricorio copies,\l"
	.string "then Rapidash and Froslass perform!$"

LavaridgeTown_Gym_1F_Text_DanielleDefeat: @ 81FEE42
	.string "You broke the whole performance!$"

LavaridgeTown_Gym_1F_Text_DaniellePostBattle: @ 81FEE5D
	.string "Dancer copies Quiver Dance, but\n"
	.string "Haze or Taunt can stop both.\p"
	.string "Choice Band and Focus Sash make the\n"
	.string "second act a different challenge.$"

LavaridgeTown_Gym_B1F_Text_JaceIntro: @ 81FEE9D
	.string "The mountain pulls; my fire answers!\p"
	.string "Eli bends gravity so Lampent's\n"
	.string "Inferno can light the chamber!$"

LavaridgeTown_Gym_B1F_Text_JaceDefeat: @ 81FEED5
	.string "You cooled every layer!$"

LavaridgeTown_Gym_B1F_Text_JacePostBattle: @ 81FEF07
	.string "Gravity steadies Inferno.\p"
	.string "Then Guts Flareon and Gouging Fire\n"
	.string "punish hesitation.$"

LavaridgeTown_Gym_B1F_Text_JeffIntro: @ 81FEF60
	.string "See how the flames wait to blaze?\p"
	.string "Spinda trades Contrary to Pyroar;\n"
	.string "then Overheat burns even higher!$"

LavaridgeTown_Gym_B1F_Text_JeffDefeat: @ 81FEFA7
	.string "You stopped the flashover!$"

LavaridgeTown_Gym_B1F_Text_JeffPostBattle: @ 81FEFC2
	.string "Iron Ball slows Pyroar, so Spinda's\n"
	.string "Skill Swap lands before Overheat.\p"
	.string "Stop Spinda, then prepare for\n"
	.string "Cinderace and Centiskorch.$"

LavaridgeTown_Gym_B1F_Text_EliIntro: @ 81FF025
	.string "A volcano's pull is as dangerous\n"
	.string "as its flame!\p"
	.string "Claydol grounds you; Coalossal marks\n"
	.string "you; Turtonator traps your swing!$"

LavaridgeTown_Gym_B1F_Text_EliDefeat: @ 81FF05F
	.string "You escaped the pressure chamber!$"

LavaridgeTown_Gym_B1F_Text_EliPostBattle: @ 81FF09A
	.string "Gravity aids Inferno and Rock Slide.\p"
	.string "Tar Shot adds a Fire weakness;\n"
	.string "special attacks bypass Shell Trap.$"

LavaridgeTown_Gym_1F_Text_FlanneryIntro: @ 81FF0DC
	.string "Welcome! I am Flannery, the\n"
	.string "Lavaridge Gym Leader!\p"
	.string "My grandfather taught me strength,\n"
	.string "but this Gym taught me timing.\p"
	.string "A slow flame can surge first--\n"
	.string "or reverse the order entirely.\p"
	.string "Read the temperature, or burn!$"

LavaridgeTown_Gym_1F_Text_FlanneryDefeat: @ 81FF233
	.string "You controlled the heat and pace!\p"
	.string "I don't need to imitate anyone.\n"
	.string "I'll keep battling my own way.\p"
	.string "You earned the Heat Badge!$"

LavaridgeTown_Gym_1F_Text_ReceivedHeatBadge: @ 81FF32F
	.string "{PLAYER} received the Heat Badge\n"
	.string "from Flannery.$"

LavaridgeTown_Gym_1F_Text_ExplainHeatBadgeTakeThis: @ 81FF359
	.string "If you have a Heat Badge, all Pokémon\n"
	.string "up to Level 60, even those you get in\l"
	.string "trades from other people, will obey\l"
	.string "you completely.\p"
	.string "And, it lets Pokémon use the HM move\n"
	.string "Strength outside of battle.\p"
	.string "I'll even let you call on my Arcanine\n"
	.string "to use Strength for you.$"

LavaridgeTown_Gym_1F_Text_TakeThis:
	.string "This is a token of my appreciation.\n"
	.string "Don't be shy about taking it!$"
LavaridgeTown_Gym_1F_Text_ExplainOverheat:
	.string "The Magmarizer evolves Magmar.\p"
	.string "Level it while holding this item.$"

LavaridgeTown_Gym_1F_Text_RegisteredFlannery: @ 81FF517
	.string "Registered Gym Leader Flannery\n"
	.string "in the PokéNav.$"

LavaridgeTown_Gym_1F_Text_RegisteredArcanine: @ 81FF517
	.string "Registered Flannery's Arcanine\n"
	.string "in the PokéNav.$"

LavaridgeTown_Gym_1F_Text_FlanneryPostBattle: @ 81FF546
	.string "Your timing reminds me of Norman.\p"
	.string "Petalburg's Gym now awaits you.$"

LavaridgeTown_Gym_1F_Text_GymStatue: @ 81FF5A9
	.string "Lavaridge Town Pokémon Gym$"

LavaridgeTown_Gym_1F_Text_GymStatueCertified: @ 81FF5C4
	.string "Lavaridge Town Pokémon Gym\p"
	.string "FLANNERY's Certified Trainers:\n"
	.string "{PLAYER}$"

LavaridgeTown_Gym_1F_Text_FlanneryPreRematch: @ 81FF601
	.string "Flannery: Let's exchange superhot\n"
	.string "moves in another battle!$"

LavaridgeTown_Gym_1F_Text_FlanneryRematchDefeat: @ 81FF69F
	.string "Whew!\n"
	.string "On the verge of eruption!$"

LavaridgeTown_Gym_1F_Text_FlanneryPostRematch: @ 81FF6BF
	.string "Flannery: I lost the match,\n"
	.string "but I'm completely satisfied.\p"
	.string "It's not often I get to enjoy a battle\n"
	.string "this heated.\p"
	.string "Let's have another one like this\n"
	.string "again sometime!$"

LavaridgeTown_Gym_1F_Text_FlanneryRematchNeedTwoMons: @ 81FF75E
	.string "Oh, wait. Do you only have one Pokémon\n"
	.string "that can battle?\p"
	.string "I hate to say this, but when you come\n"
	.string "see me, bring at least two Pokémon.$"

LavaridgeTown_Gym_1F_Text_FlanneryChooseFormat:
	.string "Flannery: Losing a battle isn't going\n"
	.string "to deflate me.\p"
	.string "I love Pokémon.\n"
	.string "I love to battle.\l"
	.string "And… I love this Gym!\p"
	.string "So… Let's choose some rules for our\n"
	.string "rematch: singles or doubles?$"

LavaridgeTown_Gym_1F_Text_FlanneryAllowLegends:
	.string "Flannery: And can I turn up the heat\n"
	.string "with my Legendary Pokémon?$"
