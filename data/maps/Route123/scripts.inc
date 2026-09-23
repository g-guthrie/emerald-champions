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
	giveitem ITEM_STAR_PIECE
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
	goto Route123_EventScript_ShowRouteRoster

Route123_EventScript_ShowRouteRoster::
	call Common_EventScript_ShowRouteRoster
	setvar VAR_0x8004, LEGENDARY_SIGN_TAPU_BULU
	call Common_EventScript_ShowRouteLegend
	setvar VAR_0x8004, LEGENDARY_SIGN_MUNKIDORI
	call Common_EventScript_ShowRouteLegend
	releaseall
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

Route123_Text_LoveGrassMonsHaveAny: @ 81F641E
	.string "I love Grass-type Pokémon!\p"
	.string "Do you have any Grass-type Pokémon?$"

Route123_Text_YouLikeGrassMonsTooHaveThis: @ 81F645D
	.string "Oh?\p"
	.string "You like Grass-type Pokémon, too,\n"
	.string "don't you?\p"
	.string "Take this Star Piece as a token\n"
	.string "of our friendship!\p"
	.string "Sell it at a Mart to help pay\n"
	.string "for your team's supplies.$"

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
