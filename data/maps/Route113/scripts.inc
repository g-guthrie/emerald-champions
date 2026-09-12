Route113_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Route113_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, Route113_OnTransition
	.byte 0

Route113_OnResume:
	setstepcallback STEP_CB_ASH
	end

Route113_OnTransition:
	clearflag FLAG_FORCE_MIRAGE_TOWER_VISIBLE
	call Route113_EventScript_CheckSetAshWeather
	end

Route113_EventScript_CheckSetAshWeather::
	getplayerxy VAR_TEMP_0, VAR_TEMP_1
	goto_if_lt VAR_TEMP_0, 19, Route113_EventScript_DontSetAshWeather
	goto_if_gt VAR_TEMP_0, 84, Route113_EventScript_DontSetAshWeather
	setweather WEATHER_VOLCANIC_ASH
	return

Route113_EventScript_DontSetAshWeather::
	return

Route113_EventScript_Gentleman::
	msgbox Route113_Text_AshCanBeFashionedIntoGlass, MSGBOX_NPC
	end

Route113_EventScript_NinjaBoy::
	msgbox Route113_Text_FunWalkingThroughAsh, MSGBOX_NPC
	end

Route113_EventScript_RouteSign111::
	msgbox Route113_Text_RouteSign111, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route113_EventScript_RouteSignFallarbor::
	msgbox Route113_Text_RouteSignFallarbor, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route113_EventScript_GlassWorkshopSign::
	msgbox Route113_Text_GlassWorkshopSign, MSGBOX_SIGN
	end

Route113_EventScript_TrainerTipsRegisterKeyItems::
	msgbox Route113_Text_TrainerTipsRegisterKeyItems, MSGBOX_SIGN
	end

Route113_EventScript_Jaylen::
	trainerbattle_double TRAINER_JAYLEN, Route113_Text_JaylenIntro, Route113_Text_JaylenDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route113_Text_JaylenPostBattle, MSGBOX_AUTOCLOSE
	end

Route113_EventScript_Dillon::
	trainerbattle_double TRAINER_DILLON, Route113_Text_DillonIntro, Route113_Text_DillonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route113_Text_DillonPostBattle, MSGBOX_AUTOCLOSE
	end

Route113_EventScript_Madeline::
	msgbox Route113_Text_MadelinePostBattle, MSGBOX_NPC
	end

Route113_EventScript_Lao::
	msgbox Route113_Text_LaoPostBattle, MSGBOX_NPC
	end

Route113_EventScript_Lung::
	trainerbattle_double TRAINER_LUNG, Route113_Text_LungIntro, Route113_Text_LungDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route113_Text_LungPostBattle, MSGBOX_AUTOCLOSE
	end

Route113_EventScript_Tori::
	trainerbattle_double TRAINER_TORI_AND_TIA, Route113_Text_ToriIntro, Route113_Text_ToriDefeat, Route113_Text_ToriNotEnoughMons
	msgbox Route113_Text_ToriPostBattle, MSGBOX_AUTOCLOSE
	end

Route113_EventScript_Tia::
	trainerbattle_double TRAINER_TORI_AND_TIA, Route113_Text_TiaIntro, Route113_Text_TiaDefeat, Route113_Text_TiaNotEnoughMons
	msgbox Route113_Text_TiaPostBattle, MSGBOX_AUTOCLOSE
	end

Route113_EventScript_Sophie::
	trainerbattle_double TRAINER_SOPHIE, Route113_Text_SophieIntro, Route113_Text_SophieDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route113_Text_SophiePostBattle, MSGBOX_AUTOCLOSE
	end

Route113_EventScript_Coby::
	trainerbattle_double TRAINER_COBY, Route113_Text_CobyIntro, Route113_Text_CobyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route113_Text_CobyPostBattle, MSGBOX_AUTOCLOSE
	end

Route113_EventScript_Lawrence::
	trainerbattle_double TRAINER_LAWRENCE, Route113_Text_LawrenceIntro, Route113_Text_LawrenceDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route113_Text_LawrencePostBattle, MSGBOX_AUTOCLOSE
	end

Route113_EventScript_Wyatt::
	trainerbattle_double TRAINER_WYATT, Route113_Text_WyattIntro, Route113_Text_WyattDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route113_Text_WyattPostBattle, MSGBOX_AUTOCLOSE
	end

Route113_Text_AshCanBeFashionedIntoGlass:
	.string "Wahahaha! Today's technology is a\n"
	.string "wondrous thing!\p"
	.string "Take this volcanic ash here.\n"
	.string "It can be fashioned into glass.$"

Route113_Text_FunWalkingThroughAsh:
	.string "PAWNIARD leave little tracks in the ash.\n"
	.string "They become BISHARP at Lv. 52.\p"
	.string "Let BISHARP hold a LEADER'S CREST,\n"
	.string "then level it up for KINGAMBIT.\l"
	.string "The crest is used up when it evolves.\p"
	.string "LILYCOVE's evolution department\n"
	.string "sells LEADER'S CRESTS.$"

Route113_Text_RouteSign111:
	.string "ROUTE 113\n"
	.string "{RIGHT_ARROW} ROUTE 111$"

Route113_Text_RouteSignFallarbor:
	.string "ROUTE 113\n"
	.string "{LEFT_ARROW} FALLARBOR TOWN$"

Route113_Text_TrainerTipsRegisterKeyItems:
	.string "TRAINER TIPS\p"
	.string "You may register one of the KEY ITEMS\n"
	.string "in your BAG as SELECT.\p"
	.string "Simply press SELECT to use\n"
	.string "the registered item conveniently.$"

Route113_Text_GlassWorkshopSign:
	.string "GLASS WORKSHOP\n"
	.string "“Turning Volcanic Ash into Glass Items”$"

