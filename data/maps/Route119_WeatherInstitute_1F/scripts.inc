Route119_WeatherInstitute_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route119_WeatherInstitute_1F_OnTransition
	.byte 0

Route119_WeatherInstitute_1F_OnTransition:
	call_if_eq VAR_WEATHER_INSTITUTE_STATE, 0, Route119_WeatherInstitute_1F_EventScript_SetLittleBoyPos
	end

Route119_WeatherInstitute_1F_EventScript_SetLittleBoyPos::
	setobjectxyperm LOCALID_WEATHER_INSTITUTE_LITTLE_BOY, 0, 5
	setobjectmovementtype LOCALID_WEATHER_INSTITUTE_LITTLE_BOY, MOVEMENT_TYPE_FACE_RIGHT
	return

Route119_WeatherInstitute_1F_EventScript_LittleBoy::
	lock
	faceplayer
	special GetPlayerBigGuyGirlString
	goto_if_eq VAR_WEATHER_INSTITUTE_STATE, 0, Route119_WeatherInstitute_1F_EventScript_LittleBoyTeamAquaHere
	msgbox Route119_WeatherInstitute_1F_Text_WowYoureStrong, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_1F_EventScript_LittleBoyTeamAquaHere::
	msgbox Route119_WeatherInstitute_1F_Text_EveryoneWentUpstairs, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_1F_EventScript_InstituteWorker1::
	lock
	faceplayer
	goto_if_unset FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, Route119_WeatherInstitute_1F_EventScript_StudyingRain
	setvar VAR_0x8004, 0
	call_if_set FLAG_DEFEATED_KYOGRE, Route119_WeatherInstitute_1F_EventScript_LegendaryDefeated
	call_if_set FLAG_DEFEATED_GROUDON, Route119_WeatherInstitute_1F_EventScript_LegendaryDefeated
	goto_if_eq VAR_0x8004, 2, Route119_WeatherInstitute_1F_EventScript_StudyingRain  @ Both defeated
	msgbox Route119_WeatherInstitute_1F_Text_NoticingAbnormalWeather, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_1F_EventScript_LegendaryDefeated::
	addvar VAR_0x8004, 1
	return

Route119_WeatherInstitute_1F_EventScript_StudyingRain::
	msgbox Route119_WeatherInstitute_1F_Text_ProfStudyingRain, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_1F_EventScript_InstituteWorker2::
	msgbox Route119_WeatherInstitute_1F_Text_WhatWereAquasUpTo, MSGBOX_NPC
	end

Route119_WeatherInstitute_1F_EventScript_Bed::
	lockall
	msgbox Route119_WeatherInstitute_1F_Text_TakeRestInBed, MSGBOX_DEFAULT
	closemessage
	call Common_EventScript_OutOfCenterPartyHeal
	releaseall
	end

Route119_WeatherInstitute_1F_EventScript_Grunt1::
	trainerbattle_double TRAINER_GRUNT_WEATHER_INST_1, Route119_WeatherInstitute_1F_Text_Grunt1Intro, Route119_WeatherInstitute_1F_Text_Grunt1Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_WeatherInstitute_1F_Text_Grunt1PostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_1F_EventScript_Grunt4::
	trainerbattle_double TRAINER_GRUNT_WEATHER_INST_4, Route119_WeatherInstitute_1F_Text_Grunt4Intro, Route119_WeatherInstitute_1F_Text_Grunt4Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_WeatherInstitute_1F_Text_Grunt4PostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_1F_Text_Grunt1Intro:
	.string "These maps show storms all over\n"
	.string "HOENN. ARCHIE wants to know where\l"
	.string "they begin.\p"
	.string "He'll get his answers. You're not\n"
	.string "getting upstairs!$"

Route119_WeatherInstitute_1F_Text_Grunt1Defeat:
	.string "I couldn't keep you out…$"

Route119_WeatherInstitute_1F_Text_Grunt1PostBattle:
	.string "These maps connect the strange\n"
	.string "storms to the ocean's depths.\p"
	.string "SHELLY took the researchers\n"
	.string "upstairs. ARCHIE wants their\l"
	.string "readings.$"

Route119_WeatherInstitute_1F_Text_Grunt4Intro:
	.string "You crossed all that rain to get\n"
	.string "here? Should've stayed where it\l"
	.string "was dry!\p"
	.string "Now you've got TEAM AQUA to deal\n"
	.string "with!$"

Route119_WeatherInstitute_1F_Text_Grunt4Defeat:
	.string "So much for scaring you off…$"

Route119_WeatherInstitute_1F_Text_Grunt4PostBattle:
	.string "There's a bed over there. You\n"
	.string "might want to rest.\p"
	.string "SHELLY's upstairs with the\n"
	.string "researchers. She's tougher than I\l"
	.string "am.$"

Route119_WeatherInstitute_1F_Text_EveryoneWentUpstairs:
	.string "Those AQUA people took the researchers\n"
	.string "upstairs while I was asleep!\p"
	.string "Can you go up and help them?$"

Route119_WeatherInstitute_1F_Text_WowYoureStrong:
	.string "Wow, you're really strong!\p"
	.string "I wish I could be a POKéMON TRAINER\n"
	.string "like you!$"

Route119_WeatherInstitute_1F_Text_ProfStudyingRain:
	.string "The PROFESSOR loves rain.\n"
	.string "That's a fact.\p"
	.string "But if it keeps raining, people will be in\n"
	.string "trouble. That's another fact.\p"
	.string "And thus, the PROFESSOR is studying\n"
	.string "if the rain can be put to good use.$"

Route119_WeatherInstitute_1F_Text_NoticingAbnormalWeather:
	.string "On the 2nd floor of the INSTITUTE,\n"
	.string "we study the weather patterns over\l"
	.string "the HOENN region.\p"
	.string "We've been noticing temporary and\n"
	.string "isolated cases of droughts and\l"
	.string "heavy rain lately…$"

Route119_WeatherInstitute_1F_Text_WhatWereAquasUpTo:
	.string "You saved our staff and records.\n"
	.string "AQUA wanted to know what is\l"
	.string "causing these violent changes in\l"
	.string "weather.$"

Route119_WeatherInstitute_1F_Text_TakeRestInBed:
	.string "There's a bed…\n"
	.string "Let's take a rest.$"
