
Route117_MapScripts:: @ 81F397D
	map_script MAP_SCRIPT_ON_TRANSITION, Route117_OnTransition
	.byte 0

Route117_OnTransition: @ 81F3983
	call Route117_EventScript_TryMoveDayCareMan
	end

Route117_EventScript_TryMoveDayCareMan:: @ 81F3989
	goto_if_unset FLAG_PENDING_DAYCARE_EGG, Route117_EventScript_StopMoveDayCareMan
	setobjectxyperm LOCALID_DAYCARE_MAN, 47, 6
Route117_EventScript_StopMoveDayCareMan:: @ 81F3999
	return

Route117_EventScript_Woman:: @ 81F399A
	msgbox Route117_Text_ArentTheseFlowersPretty, MSGBOX_NPC
	end

Route117_EventScript_LittleBoy:: @ 81F39A3
	msgbox Route117_Text_AirIsTastyHere, MSGBOX_NPC
	end

Route117_EventScript_Girl:: @ 81F39AC
	msgbox Route117_Text_DayCarePokemonHadNewMove, MSGBOX_NPC
	end

Route117_EventScript_RouteSignVerdanturf:: @ 81F39B5
	msgbox Route117_Text_RouteSignVerdanturf, MSGBOX_SIGN
	end

Route117_EventScript_RouteSignMauville:: @ 81F39BE
	msgbox Route117_Text_RouteSignMauville, MSGBOX_SIGN
	end

Route117_EventScript_DayCareSign:: @ 81F39C7
	msgbox Route117_Text_DayCareSign, MSGBOX_SIGN
	end

Route117_EventScript_Isaac:: @ 81F39D0
	trainerbattle_double TRAINER_ISAAC_1, Route117_Text_IsaacIntro, Route117_Text_IsaacDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route117_Text_IsaacPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Lydia:: @ 81F3A32
	trainerbattle_double TRAINER_LYDIA_1, Route117_Text_LydiaIntro, Route117_Text_LydiaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route117_Text_LydiaPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Dylan:: @ 81F3A94
	msgbox Route117_Text_DylanPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Maria:: @ 81F3AF6
	trainerbattle_double TRAINER_MARIA_1, Route117_Text_MariaIntro, Route117_Text_MariaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route117_Text_MariaPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Derek:: @ 81F3B58
	trainerbattle_double TRAINER_DEREK, Route117_Text_DerekIntro, Route117_Text_DerekDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route117_Text_DerekPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Anna:: @ 81F3B6F
	trainerbattle_double TRAINER_ANNA_AND_MEG_1, Route117_Text_AnnaIntro, Route117_Text_AnnaDefeat, Route117_Text_AnnaNotEnoughMons
	msgbox Route117_Text_AnnaPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Meg:: @ 81F3BD3
	trainerbattle_double TRAINER_ANNA_AND_MEG_1, Route117_Text_MegIntro, Route117_Text_MegDefeat, Route117_Text_MegNotEnoughMons
	msgbox Route117_Text_MegPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Melina:: @ 81F3C37
	trainerbattle_double TRAINER_MELINA, Route117_Text_MelinaIntro, Route117_Text_MelinaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route117_Text_MelinaPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Brandi:: @ 81F3C4E
	trainerbattle_double TRAINER_BRANDI, Route117_Text_BrandiIntro, Route117_Text_BrandiDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route117_Text_BrandiPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_EventScript_Aisha:: @ 81F3C65
	trainerbattle_double TRAINER_AISHA, Route117_Text_AishaIntro, Route117_Text_AishaDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route117_Text_AishaPostBattle, MSGBOX_AUTOCLOSE
	end

Route117_Text_DayCarePokemonHadNewMove: @ 81F3C7C
	.string "I left my Pokémon at the Day Care.\p"
	.string "When I got it back, it had a new move\n"
	.string "that I didn't teach it.\l"
	.string "I was really, really surprised.$"

Route117_Text_ArentTheseFlowersPretty: @ 81F3CFD
	.string "What do you think?\n"
	.string "Aren't these flowers pretty?\p"
	.string "I planted them all!$"

Route117_Text_AirIsTastyHere: @ 81F3D41
	.string "The air is tasty here!$"

Route117_Text_RouteSignVerdanturf: @ 81F3D58
	.string "Route 117\n"
	.string "{LEFT_ARROW} Verdanturf Town$"

Route117_Text_RouteSignMauville: @ 81F3D74
	.string "Route 117\n"
	.string "{RIGHT_ARROW} Mauville City$"

Route117_Text_DayCareSign: @ 81F3D8E
	.string "Pokémon Day Care\n"
	.string "“Let us raise your Pokémon.”$"
