Route134_MapScripts:: @ 81F7644
	map_script MAP_SCRIPT_ON_RESUME, Route134_OnResume
	.byte 0

Route134_OnResume: @ 81F764A
	setdivewarp MAP_UNDERWATER_ROUTE134, 255, 8, 6
	end

Route134_EventScript_Aaron:: @ 81F7698
	trainerbattle_double TRAINER_AARON, Route134_Text_AaronIntro, Route134_Text_AaronDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route134_Text_AaronPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Hitoshi:: @ 81F76AF
	@ This retired encounter has no campaign battle.
	msgbox Route134_Text_HitoshiPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Hudson:: @ 81F76C6
	trainerbattle_double TRAINER_HUDSON, Route134_Text_HudsonIntro, Route134_Text_HudsonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route134_Text_HudsonPostBattle, MSGBOX_AUTOCLOSE
	end

Route134_EventScript_Marley:: @ 81F76F4
	trainerbattle_double TRAINER_MARLEY, Route134_Text_MarleyIntro, Route134_Text_MarleyDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route134_Text_MarleyPostBattle, MSGBOX_AUTOCLOSE
	end
