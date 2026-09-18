Route123_MapScripts:: @ 81F6147
	map_script MAP_SCRIPT_ON_TRANSITION, Route123_OnTransition
	.byte 0

Route123_OnTransition: @ 81F614D
	special SetRoute123Weather
	end

Route123_EventScript_GigaDrainGirl:: @ 81F6151
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TM19, Route123_EventScript_ReceivedGigaDrain
	msgbox Route123_Text_LoveGrassMonsHaveAny, MSGBOX_DEFAULT
	special IsGrassTypeInParty
	compare VAR_RESULT, FALSE
	goto_if_eq Route123_EventScript_NoGrassMons
	msgbox Route123_Text_YouLikeGrassMonsTooHaveThis, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM19
	msgbox Route123_Text_CheckTreesWithMyGrassMon, MSGBOX_DEFAULT
	release
	end

Route123_EventScript_NoGrassMons:: @ 81F619E
	release
	end

Route123_EventScript_ReceivedGigaDrain:: @ 81F61A0
	msgbox Route123_Text_CheckTreesWithMyGrassMon, MSGBOX_DEFAULT
	release
	end

Route123_EventScript_RouteSign:: @ 81F61AA
	msgbox Route123_Text_RouteSign, MSGBOX_SIGN
	end

Route123_EventScript_RouteSignMtPyre:: @ 81F61B3
	msgbox Route123_Text_RouteSignMtPyre, MSGBOX_SIGN
	end

Route123_EventScript_BerryMastersHouseSign:: @ 81F61BC
	msgbox Route123_Text_BerryMastersHouse, MSGBOX_SIGN
	end

Route123_EventScript_Wendy:: @ 81F61C5
	trainerbattle_double TRAINER_WENDY, Route123_Text_WendyIntro, Route123_Text_WendyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_WendyPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Braxton:: @ 81F61DC
	trainerbattle_double TRAINER_BRAXTON, Route123_Text_BraxtonIntro, Route123_Text_BraxtonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_BraxtonPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Violet:: @ 81F61F3
	@ This retired encounter has no campaign battle.
	msgbox Route123_Text_VioletPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Cameron:: @ 81F620A
	@ This retired encounter has no campaign battle.
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route123_EventScript_RematchCameron
	msgbox Route123_Text_CameronPostBattle, MSGBOX_DEFAULT
	release
	end

Route123_EventScript_RegisterCameron:: @ 81F6236
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route123_Text_CameronRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_CAMERON_1
	release
	end

Route123_EventScript_RematchCameron:: @ 81F6255
	@ This retired encounter has no campaign battle.
	msgbox Route123_Text_CameronPostRematch, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Jacki:: @ 81F626C
	@ This retired encounter has no campaign battle.
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route123_EventScript_RematchJacki
	msgbox Route123_Text_JackiPostBattle, MSGBOX_DEFAULT
	release
	end

Route123_EventScript_RegisterJacki:: @ 81F6298
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route123_Text_JackiRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_JACKI_1
	release
	end

Route123_EventScript_RematchJacki:: @ 81F62B7
	@ This retired encounter has no campaign battle.
	msgbox Route123_Text_JackiPostRematch, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Miu:: @ 81F62CE
	trainerbattle_double TRAINER_MIU_AND_YUKI, Route123_Text_MiuIntro, Route123_Text_MiuDefeat, Route123_Text_MiuNotEnoughMons
	msgbox Route123_Text_MiuPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Yuki:: @ 81F62E9
	trainerbattle_double TRAINER_MIU_AND_YUKI, Route123_Text_YukiIntro, Route123_Text_YukiDefeat, Route123_Text_YukiNotEnoughMons
	msgbox Route123_Text_YukiPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Kindra:: @ 81F6304
	@ This retired encounter has no campaign battle.
	msgbox Route123_Text_KindraPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Frederick:: @ 81F631B
	trainerbattle_double TRAINER_FREDRICK, Route123_Text_FrederickIntro, Route123_Text_FrederickDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_FrederickPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Alberto:: @ 81F6332
	@ This retired encounter has no campaign battle.
	msgbox Route123_Text_AlbertoPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Ed:: @ 81F6349
	@ This retired encounter has no campaign battle.
	msgbox Route123_Text_EdPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Kayley:: @ 81F6360
	trainerbattle_double TRAINER_KAYLEY, Route123_Text_KayleyIntro, Route123_Text_KayleyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_KayleyPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Jonas:: @ 81F6377
	@ This retired encounter has no campaign battle.
	msgbox Route123_Text_JonasPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Jazmyn:: @ 81F638E
	trainerbattle_double TRAINER_JAZMYN, Route123_Text_JazmynIntro, Route123_Text_JazmynDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_JazmynPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Davis:: @ 81F63A5
	@ This retired encounter has no campaign battle.
	msgbox Route123_Text_DavisPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Fernando:: @ 81F63BC
	@ This retired encounter has no campaign battle.
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq Route123_EventScript_RematchFernando
	msgbox Route123_Text_FernandoPostBattle, MSGBOX_DEFAULT
	release
	end

Route123_EventScript_RegisterFernando:: @ 81F63E8
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox Route123_Text_FernandoRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_FERNANDO_1
	release
	end

Route123_EventScript_RematchFernando:: @ 81F6407
	@ This retired encounter has no campaign battle.
	msgbox Route123_Text_FernandoPostRematch, MSGBOX_AUTOCLOSE
	end

Route123_Text_LoveGrassMonsHaveAny: @ 81F641E
	.string "I love Grass-type Pokémon!\p"
	.string "Do you have any Grass-type Pokémon?$"

Route123_Text_YouLikeGrassMonsTooHaveThis: @ 81F645D
	.string "Oh?\p"
	.string "You like Grass-type Pokémon, too,\n"
	.string "don't you?\p"
	.string "I'm so happy, you can have this!\n"
	.string "It's a token of our friendship.$"

Route123_Text_CheckTreesWithMyGrassMon: @ 81F64CF
	.string "I check trees with my Grass-type\n"
	.string "Pokémon. I'm like a tree doctor.$"

Route123_Text_RouteSign: @ 81F6511
	.string "{RIGHT_ARROW} Route 123\n"
	.string "{LEFT_ARROW} Route 118$"

Route123_Text_RouteSignMtPyre: @ 81F6529
	.string "{UP_ARROW} Mt. Pyre\n"
	.string "“Forbidden to the faint of heart.”$"

Route123_Text_BerryMastersHouse: @ 81F6557
	.string "Berry Master's House$"

