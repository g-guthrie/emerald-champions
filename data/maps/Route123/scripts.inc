Route123_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route123_OnTransition
	.byte 0

Route123_OnTransition:
	special SetRoute123Weather
	end

Route123_EventScript_SweetAppleGirl::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_ROUTE123_SWEET_APPLE, Route123_EventScript_ReceivedSweetApple
	msgbox Route123_Text_LoveGrassMonsHaveAny, MSGBOX_DEFAULT
	special IsGrassTypeInParty
	goto_if_eq VAR_RESULT, FALSE, Route123_EventScript_NoGrassMons
	msgbox Route123_Text_YouLikeGrassMonsTooHaveThis, MSGBOX_DEFAULT
	giveitem ITEM_SWEET_APPLE
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_ROUTE123_SWEET_APPLE
	msgbox Route123_Text_CheckTreesWithMyGrassMon, MSGBOX_DEFAULT
	release
	end

Route123_EventScript_NoGrassMons::
	release
	end

Route123_EventScript_ReceivedSweetApple::
	msgbox Route123_Text_CheckTreesWithMyGrassMon, MSGBOX_DEFAULT
	release
	end

Route123_EventScript_RouteSign::
	msgbox Route123_Text_RouteSign, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route123_EventScript_RouteSignMtPyre::
	msgbox Route123_Text_RouteSignMtPyre, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route123_EventScript_BerryMastersHouseSign::
	msgbox Route123_Text_BerryMastersHouse, MSGBOX_SIGN
	end

Route123_EventScript_Wendy::
	trainerbattle_double TRAINER_WENDY, Route123_Text_WendyIntro, Route123_Text_WendyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_WendyPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Braxton::
	trainerbattle_double TRAINER_BRAXTON, Route123_Text_BraxtonIntro, Route123_Text_BraxtonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_BraxtonPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Violet::
	msgbox Route123_Text_VioletPostBattle, MSGBOX_NPC
	end

Route123_EventScript_Cameron::
	msgbox Route123_Text_CameronPostBattle, MSGBOX_NPC
	end

Route123_EventScript_Jacki::
	msgbox Route123_Text_JackiPostBattle, MSGBOX_NPC
	end

Route123_EventScript_Miu::
	trainerbattle_double TRAINER_MIU_AND_YUKI, Route123_Text_MiuIntro, Route123_Text_MiuDefeat, Route123_Text_MiuNotEnoughMons
	msgbox Route123_Text_MiuPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Yuki::
	trainerbattle_double TRAINER_MIU_AND_YUKI, Route123_Text_YukiIntro, Route123_Text_YukiDefeat, Route123_Text_YukiNotEnoughMons
	msgbox Route123_Text_YukiPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Kindra::
	msgbox Route123_Text_KINDRA_LocalConversation, MSGBOX_NPC
	end

Route123_EventScript_Frederick::
	trainerbattle_double TRAINER_FREDRICK, Route123_Text_FrederickIntro, Route123_Text_FrederickDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_FrederickPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Alberto::
	msgbox Route123_Text_AlbertoPostBattle, MSGBOX_NPC
	end

Route123_EventScript_Ed::
	msgbox Route123_Text_EdPostBattle, MSGBOX_NPC
	end

Route123_EventScript_Kayley::
	trainerbattle_double TRAINER_KAYLEY, Route123_Text_KayleyIntro, Route123_Text_KayleyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_KayleyPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Jonas::
	msgbox Route123_Text_JONAS_LocalConversation, MSGBOX_NPC
	end

Route123_EventScript_Jazmyn::
	trainerbattle_double TRAINER_JAZMYN, Route123_Text_JazmynIntro, Route123_Text_JazmynDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route123_Text_JazmynPostBattle, MSGBOX_AUTOCLOSE
	end

Route123_EventScript_Davis::
	msgbox Route123_Text_DavisPostBattle, MSGBOX_NPC
	end

Route123_EventScript_Fernando::
	msgbox Route123_Text_FernandoPostBattle, MSGBOX_NPC
	end

Route123_Text_LoveGrassMonsHaveAny:
	.string "I love GRASS-type POKéMON!\p"
	.string "Do you have any GRASS-type POKéMON?$"

Route123_Text_YouLikeGrassMonsTooHaveThis:
	.string "Oh? You like GRASS-type POKéMON,\n"
	.string "too!\p"
	.string "Here, take this SWEET APPLE from\n"
	.string "the orchard. Use it on APPLIN to\l"
	.string "evolve it into APPLETUN.$"

Route123_Text_CheckTreesWithMyGrassMon:
	.string "I check trees with my GRASS-type\n"
	.string "POKéMON. I'm like a tree doctor.$"

Route123_Text_RouteSign:
	.string "{RIGHT_ARROW} ROUTE 123\n"
	.string "{LEFT_ARROW} ROUTE 118$"

Route123_Text_RouteSignMtPyre:
	.string "{UP_ARROW} MT. PYRE\p"
	.string "Travelers report TAPU BULU in the\n"
	.string "grass on ROUTE 123. It has a 3%\l"
	.string "wild encounter chance.$"
Route123_Text_BerryMastersHouse:
	.string "BERRY MASTER'S HOUSE$"

Route123_Text_KINDRA_LocalConversation:
	.string "That shadow is following you.\n"
	.string "Most shadows do. Carry on.$"

Route123_Text_JONAS_LocalConversation:
	.string "My smoke bomb worked perfectly.\n"
	.string "Finding my lunch afterward did not.$"
