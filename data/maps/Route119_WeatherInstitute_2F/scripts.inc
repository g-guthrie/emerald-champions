Route119_WeatherInstitute_2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route119_WeatherInstitute_2F_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Route119_WeatherInstitute_2F_OnFrame
	.byte 0

Route119_WeatherInstitute_2F_OnFrame:
	map_script_2 VAR_TEMP_F, 0, Route119_WeatherInstitute_2F_EventScript_CheckArrival
	.2byte 0

Route119_WeatherInstitute_2F_EventScript_CheckArrival::
	setvar VAR_TEMP_F, 1
	goto_if_unset FLAG_EC_SURVEY_FLIGHT_ARRIVAL, Route119_WeatherInstitute_2F_EventScript_ArrivalEnd
	clearflag FLAG_EC_SURVEY_FLIGHT_ARRIVAL
	lockall
	msgbox Route111_RuinsExterior_Text_Returned, MSGBOX_DEFAULT
	releaseall
Route119_WeatherInstitute_2F_EventScript_ArrivalEnd::
	end

Route119_WeatherInstitute_2F_OnTransition:
	call_if_eq VAR_WEATHER_INSTITUTE_STATE, 0, Route119_WeatherInstitute_2F_EventScript_SetScientistPosAquaHere
	call_if_eq VAR_WEATHER_INSTITUTE_STATE, 1, Route119_WeatherInstitute_2F_EventScript_SetScientistPosAquaGone
	call_if_set FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, Route119_WeatherInstitute_2F_EventScript_SetScientistPosGameClear
	end

Route119_WeatherInstitute_2F_EventScript_SetScientistPosAquaHere::
	setobjectxyperm LOCALID_WEATHER_INSTITUTE_2F_SCIENTIST, 1, 6
	setobjectmovementtype LOCALID_WEATHER_INSTITUTE_2F_SCIENTIST, MOVEMENT_TYPE_FACE_RIGHT
	return

Route119_WeatherInstitute_2F_EventScript_SetScientistPosAquaGone::
	setobjectxyperm LOCALID_WEATHER_INSTITUTE_2F_SCIENTIST, 4, 6
	setobjectmovementtype LOCALID_WEATHER_INSTITUTE_2F_SCIENTIST, MOVEMENT_TYPE_FACE_RIGHT
	return

Route119_WeatherInstitute_2F_EventScript_SetScientistPosGameClear::
	setobjectxyperm LOCALID_WEATHER_INSTITUTE_2F_SCIENTIST, 2, 2
	setobjectmovementtype LOCALID_WEATHER_INSTITUTE_2F_SCIENTIST, MOVEMENT_TYPE_FACE_UP
	return

Route119_WeatherInstitute_2F_EventScript_Grunt5::
	trainerbattle_double TRAINER_GRUNT_WEATHER_INST_5, Route119_WeatherInstitute_2F_Text_Grunt5Intro, Route119_WeatherInstitute_2F_Text_Grunt5Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_WeatherInstitute_2F_Text_Grunt5PostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_2F_EventScript_Grunt2::
	trainerbattle_double TRAINER_GRUNT_WEATHER_INST_2, Route119_WeatherInstitute_2F_Text_Grunt2Intro, Route119_WeatherInstitute_2F_Text_Grunt2Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_WeatherInstitute_2F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_2F_EventScript_Grunt3::
	trainerbattle_double TRAINER_GRUNT_WEATHER_INST_3, Route119_WeatherInstitute_2F_Text_Grunt3Intro, Route119_WeatherInstitute_2F_Text_Grunt3Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route119_WeatherInstitute_2F_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_2F_EventScript_Shelly::
	call EC_RequireGenerator
	trainerbattle_double TRAINER_SHELLY_WEATHER_INSTITUTE, Route119_WeatherInstitute_2F_Text_ShellyIntro, Route119_WeatherInstitute_2F_Text_ShellyDefeat, EmeraldChampions_Text_NeedTwoPokemon, Route119_WeatherInstitute_2F_EventScript_ShellyDefeated
	msgbox Route119_WeatherInstitute_2F_Text_ShellyPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_2F_EventScript_ShellyDefeated::
	msgbox Route119_WeatherInstitute_2F_Text_ShellyPostBattle, MSGBOX_DEFAULT
	closemessage
	addobject LOCALID_WEATHER_INSTITUTE_2F_GRUNT_3
	applymovement LOCALID_WEATHER_INSTITUTE_2F_GRUNT_3, Route119_WeatherInstitute_2F_Movement_GruntApproachShelly
	applymovement LOCALID_PLAYER, Route119_WeatherInstitute_2F_Movement_ShovePlayerOutOfWay
	waitmovement 0
	msgbox Route119_WeatherInstitute_2F_Text_TeamMagmaJustPassedBy, MSGBOX_DEFAULT
	closemessage
	playse SE_PIN
	applymovement LOCALID_WEATHER_INSTITUTE_2F_SHELLY, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_WEATHER_INSTITUTE_2F_SHELLY, Common_Movement_Delay48
	waitmovement 0
	msgbox Route119_WeatherInstitute_2F_Text_WeHaveToHurryToMtPyre, MSGBOX_DEFAULT
	closemessage
	setvar VAR_WEATHER_INSTITUTE_STATE, 1
	clearflag FLAG_HIDE_WEATHER_INSTITUTE_2F_WORKERS
	fadedefaultbgm
	fadescreen FADE_TO_BLACK
	setflag FLAG_HIDE_ROUTE_119_TEAM_AQUA
	setflag FLAG_HIDE_WEATHER_INSTITUTE_2F_AQUA_GRUNT_M
	removeobject LOCALID_WEATHER_INSTITUTE_2F_GRUNT_1
	removeobject LOCALID_WEATHER_INSTITUTE_2F_GRUNT_2
	removeobject LOCALID_WEATHER_INSTITUTE_2F_SHELLY
	removeobject LOCALID_WEATHER_INSTITUTE_2F_GRUNT_4
	removeobject LOCALID_WEATHER_INSTITUTE_2F_GRUNT_3
	applymovement LOCALID_PLAYER, Route119_WeatherInstitute_2F_Movement_PlayerReturnToPosition
	waitmovement 0
	fadescreen FADE_FROM_BLACK
	applymovement LOCALID_WEATHER_INSTITUTE_2F_SCIENTIST, Route119_WeatherInstitute_2F_Movement_ScientistApproachPlayer
	waitmovement 0
	copyobjectxytoperm LOCALID_WEATHER_INSTITUTE_2F_SCIENTIST
	goto Route119_WeatherInstitute_2F_EventScript_ReceiveCastform
	end

Route119_WeatherInstitute_2F_EventScript_ReceiveCastform::
	special LocateEmeraldChampionsCastform
	goto_if_ne VAR_RESULT, EC_CASTFORM_NONE, Route119_WeatherInstitute_2F_EventScript_OwnedCastform
	msgbox Route119_WeatherInstitute_2F_Text_ThanksPleaseTakePokemon, MSGBOX_DEFAULT
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_CASTFORM_NORMAL
	setvar VAR_0x8004, SPECIES_CASTFORM_NORMAL
	setvar VAR_0x8005, 25
	special GiveEmeraldChampionsPreparedPokemon
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY, Route119_WeatherInstitute_2F_EventScript_ReceiveCastformParty
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, Route119_WeatherInstitute_2F_EventScript_ReceiveCastformPC
	goto Common_EventScript_NoMoreRoomForPokemon
	end

Route119_WeatherInstitute_2F_EventScript_ReceiveCastformParty::
	call Route119_WeatherInstitute_2F_EventScript_ReceivedCastformFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route119_WeatherInstitute_2F_EventScript_ExplainCastform
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto Route119_WeatherInstitute_2F_EventScript_ExplainCastform
	end

Route119_WeatherInstitute_2F_EventScript_ReceiveCastformPC::
	call Route119_WeatherInstitute_2F_EventScript_ReceivedCastformFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route119_WeatherInstitute_2F_EventScript_SendCastformToPC
	call Common_EventScript_NameReceivedBoxMon
	goto Route119_WeatherInstitute_2F_EventScript_SendCastformToPC
	end

Route119_WeatherInstitute_2F_EventScript_SendCastformToPC::
	call Common_EventScript_TransferredToPC
	goto Route119_WeatherInstitute_2F_EventScript_ExplainCastform
	end

Route119_WeatherInstitute_2F_EventScript_ReceivedCastformFanfare::
	playfanfare MUS_OBTAIN_ITEM
	message Route119_WeatherInstitute_2F_Text_PlayerReceivedCastform
	waitmessage
	waitfanfare
	bufferspeciesname STR_VAR_1, SPECIES_CASTFORM_NORMAL
	return

Route119_WeatherInstitute_2F_EventScript_ExplainCastform::
	msgbox Route119_WeatherInstitute_2F_Text_PokemonChangesWithWeather, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_CASTFORM
	goto Route119_WeatherInstitute_2F_EventScript_TryGiveRevealGlass
	end

Route119_WeatherInstitute_2F_EventScript_TryGiveRevealGlass::
	goto_if_set FLAG_EC_RECEIVED_REVEAL_GLASS, Route119_WeatherInstitute_2F_EventScript_OfferSurvey
	checkitem ITEM_REVEAL_GLASS, 1
	goto_if_eq VAR_RESULT, TRUE, Route119_WeatherInstitute_2F_EventScript_RecordRevealGlass
	giveitem ITEM_REVEAL_GLASS
	goto_if_eq VAR_RESULT, FALSE, Route119_WeatherInstitute_2F_EventScript_RevealGlassBagFull
	msgbox Route119_WeatherInstitute_2F_Text_RevealGlassPurpose, MSGBOX_DEFAULT
Route119_WeatherInstitute_2F_EventScript_RecordRevealGlass::
	setflag FLAG_EC_RECEIVED_REVEAL_GLASS
	goto Route119_WeatherInstitute_2F_EventScript_OfferSurvey
	end

Route119_WeatherInstitute_2F_EventScript_Release::
	release
	end

Route119_WeatherInstitute_2F_EventScript_RevealGlassBagFull::
	msgbox Route119_WeatherInstitute_2F_Text_RevealGlassBagFull, MSGBOX_DEFAULT
	goto Route119_WeatherInstitute_2F_EventScript_OfferSurvey

Route119_WeatherInstitute_2F_EventScript_ScientistMentionWeather::
	goto_if_set FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, Route119_WeatherInstitute_2F_EventScript_TryStartAbnormalWeather
	goto_if_set FLAG_EC_REPORT_C30_COMPLETE, Route119_WeatherInstitute_2F_EventScript_SurveyConclusion
	msgbox Route119_WeatherInstitute_2F_Text_ChangingWeatherRidiculous, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_2F_EventScript_TryStartAbnormalWeather::
	setvar VAR_0x8004, 0
	call_if_set FLAG_DEFEATED_KYOGRE, Route119_WeatherInstitute_2F_EventScript_LegendaryDefeated
	call_if_set FLAG_DEFEATED_GROUDON, Route119_WeatherInstitute_2F_EventScript_LegendaryDefeated
	goto_if_eq VAR_0x8004, 2, Route119_WeatherInstitute_2F_EventScript_NoAbnormalWeather  @ Both defeated
	call_if_eq VAR_ABNORMAL_WEATHER_LOCATION, ABNORMAL_WEATHER_NONE, Route119_WeatherInstitute_2F_EventScript_CreateAbnormalWeather
	call_if_unset FLAG_TEMP_2, Route119_WeatherInstitute_2F_EventScript_CreateAbnormalWeather
	specialvar VAR_RESULT, GetAbnormalWeatherMapNameAndType
	goto_if_eq VAR_RESULT, 1, Route119_WeatherInstitute_2F_EventScript_KyogreWeather
	msgbox Route119_WeatherInstitute_2F_Text_GroudonWeather, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_2F_EventScript_KyogreWeather::
	msgbox Route119_WeatherInstitute_2F_Text_KyogreWeather, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_2F_EventScript_CreateAbnormalWeather::
	special CreateAbnormalWeatherEvent
	setflag FLAG_TEMP_2
	return

Route119_WeatherInstitute_2F_EventScript_LegendaryDefeated::
	addvar VAR_0x8004, 1
	return

Route119_WeatherInstitute_2F_EventScript_NoAbnormalWeather::
	msgbox Route119_WeatherInstitute_2F_Text_NoAbnormalWeather, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_2F_Movement_GruntApproachShelly:
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	step_end

Route119_WeatherInstitute_2F_Movement_ShovePlayerOutOfWay:
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	ride_water_current_up
	walk_in_place_faster_down
	step_end

Route119_WeatherInstitute_2F_Movement_PlayerReturnToPosition:
	slide_down
	walk_in_place_faster_left
	step_end

Route119_WeatherInstitute_2F_Movement_ScientistApproachPlayer:
	walk_right
	walk_right
	walk_right
	step_end

Route119_WeatherInstitute_2F_EventScript_WeatherScientist::
	lock
	faceplayer
	goto_if_unset FLAG_HIDE_ROUTE_119_TEAM_AQUA, Route119_WeatherInstitute_2F_EventScript_ScientistMentionWeather
	goto_if_set FLAG_EC_REPORT_C30_COMPLETE, Route119_WeatherInstitute_2F_EventScript_TryGiveRevealGlass
	goto Route119_WeatherInstitute_2F_EventScript_ReceiveCastform
	end

Route119_WeatherInstitute_2F_EventScript_OwnedCastform::
	setflag FLAG_RECEIVED_CASTFORM
	switch VAR_RESULT
	case EC_CASTFORM_PARTY, Route119_WeatherInstitute_2F_EventScript_TryGiveRevealGlass
	case EC_CASTFORM_BOX, Route119_WeatherInstitute_2F_EventScript_CastformInBox
	case EC_CASTFORM_DAYCARE, Route119_WeatherInstitute_2F_EventScript_CastformInDaycare
	msgbox Route119_WeatherInstitute_2F_Text_CastformEgg, MSGBOX_DEFAULT
	goto Route119_WeatherInstitute_2F_EventScript_OfferSurvey
	end

Route119_WeatherInstitute_2F_EventScript_CastformInBox::
	msgbox Route119_WeatherInstitute_2F_Text_CastformInBox, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route119_WeatherInstitute_2F_EventScript_OfferSurvey
Route119_WeatherInstitute_2F_EventScript_OpenStorage::
	special ShowPokemonStorageSystemPC
	goto Route119_WeatherInstitute_2F_EventScript_Release
	end

Route119_WeatherInstitute_2F_EventScript_CastformInDaycare::
	msgbox Route119_WeatherInstitute_2F_Text_CastformInDaycare, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route119_WeatherInstitute_2F_EventScript_OfferSurvey
	getpartysize
	goto_if_eq VAR_RESULT, PARTY_SIZE, Route119_WeatherInstitute_2F_EventScript_OpenStorage
	special LocateEmeraldChampionsCastform
	special TakePokemonFromDaycare
	msgbox Route119_WeatherInstitute_2F_Text_CastformRetrieved, MSGBOX_DEFAULT
	goto Route119_WeatherInstitute_2F_EventScript_TryGiveRevealGlass
	end

Route119_WeatherInstitute_2F_EventScript_OfferSurvey::
	goto_if_set FLAG_EC_REPORT_C30_COMPLETE, Route119_WeatherInstitute_2F_EventScript_ScientistMentionWeather
	msgbox Route119_WeatherInstitute_2F_Text_OfferSurvey, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route119_WeatherInstitute_2F_EventScript_Release
	msgbox Route119_WeatherInstitute_2F_Text_Departure, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	playse SE_M_FLY
	waitse
	setflag FLAG_EC_SURVEY_FLIGHT_ARRIVAL
	warp MAP_ROUTE111_RUINS_EXTERIOR, 13, 12
	waitstate
	releaseall
	end

Route119_WeatherInstitute_2F_Text_CastformInBox:
	.string "Your CASTFORM is in BOX {STR_VAR_1}.\n"
	.string "Open the PC to bring it along?$"

Route119_WeatherInstitute_2F_Text_CastformInDaycare:
	.string "Your CASTFORM is at the DAY CARE.\n"
	.string "Shall I arrange its return?\l"
	.string "The Institute covers the fee.\p"
	.string "If your party is full, we'll open\n"
	.string "the PC first. Speak to me again\l"
	.string "when you have room.$"

Route119_WeatherInstitute_2F_Text_CastformRetrieved:
	.string "CASTFORM has returned to your\n"
	.string "party, ready for the survey.$"

Route119_WeatherInstitute_2F_Text_CastformEgg:
	.string "You already own a CASTFORM Egg.\n"
	.string "Hatch it and bring CASTFORM along\l"
	.string "for the survey.$"

Route119_WeatherInstitute_2F_Text_OfferSurvey:
	.string "Our research flier can take you\n"
	.string "to the elevated desert ruins.\p"
	.string "Bring CASTFORM to our researcher.\n"
	.string "He'll arrange the return trip,\l"
	.string "even if you need another party.\p"
	.string "Leave for the weather survey?$"

Route119_WeatherInstitute_2F_Text_Grunt2Intro:
	.string "The researchers say CASTFORM\n"
	.string "changes with the weather. ARCHIE\l"
	.string "wants to know why!\p"
	.string "You can't go into the research\n"
	.string "wing!$"

Route119_WeatherInstitute_2F_Text_Grunt2Defeat:
	.string "I couldn't stop you…$"

Route119_WeatherInstitute_2F_Text_Grunt2PostBattle:
	.string "CASTFORM does not command the\n"
	.string "weather. It survives by changing\l"
	.string "with it.$"

Route119_WeatherInstitute_2F_Text_Grunt3Intro:
	.string "Weather records, rare POKéMON…\n"
	.string "This place has everything ARCHIE\l"
	.string "asked for!\p"
	.string "You're not taking it away from\n"
	.string "us!$"

Route119_WeatherInstitute_2F_Text_Grunt3Defeat:
	.string "But I was guarding the records…$"

Route119_WeatherInstitute_2F_Text_Grunt3PostBattle:
	.string "We've watched strange storms\n"
	.string "gather out at sea. ARCHIE says\l"
	.string "these records will tell us what\l"
	.string "they mean.$"

Route119_WeatherInstitute_2F_Text_Grunt5Intro:
	.string "What are you after? The records?\n"
	.string "The weather POKéMON?\p"
	.string "Whatever it is, TEAM AQUA got\n"
	.string "here first!$"

Route119_WeatherInstitute_2F_Text_Grunt5Defeat:
	.string "I couldn't hold my post…$"

Route119_WeatherInstitute_2F_Text_Grunt5PostBattle:
	.string "The storm records led us to old\n"
	.string "stories about MT. PYRE. ARCHIE\l"
	.string "thinks its ORBS are the key.$"

Route119_WeatherInstitute_2F_Text_ShellyIntro:
	.string "SHELLY: We came for weather\n"
	.string "records. They show something\l"
	.string "powerful stirring in the ocean.\p"
	.string "You should have stayed out of\n"
	.string "this, {PLAYER}. I'm not letting\l"
	.string "you interfere with ARCHIE's work!$"

Route119_WeatherInstitute_2F_Text_ShellyDefeat:
	.string "You've beaten us here, too…$"

Route119_WeatherInstitute_2F_Text_ShellyPostBattle:
	.string "SHELLY: You're as persistent as\n"
	.string "ARCHIE said.\p"
	.string "We have what we need. These\n"
	.string "records point to MT. PYRE.$"

Route119_WeatherInstitute_2F_Text_TeamMagmaJustPassedBy:
	.string "TEAM MAGMA just passed below us. They\n"
	.string "were searching for an ancient BLUE ORB.\p"
	.string "They are headed for MT. PYRE!$"

Route119_WeatherInstitute_2F_Text_WeHaveToHurryToMtPyre:
	.string "What?! MAGMA is heading for the\n"
	.string "BLUE ORB?\p"
	.string "SHELLY: Then we can't waste\n"
	.string "another moment here. AQUA! To MT.\l"
	.string "PYRE!$"

Route119_WeatherInstitute_2F_Text_ThanksPleaseTakePokemon:
	.string "You saved us, and our research.\n"
	.string "Thank you!\p"
	.string "This CASTFORM has helped us study\n"
	.string "the weather. I think it would\l"
	.string "enjoy traveling with you. Please\l"
	.string "take it!$"

@ Unused
Route119_WeatherInstitute_2F_Text_NoRoomForPokemon:
	.string "Hm? You don't seem to have any room\n"
	.string "for this POKéMON.$"

Route119_WeatherInstitute_2F_Text_PlayerReceivedCastform:
	.string "{PLAYER} received CASTFORM!$"

Route119_WeatherInstitute_2F_Text_PokemonChangesWithWeather:
	.string "CASTFORM changes with the\n"
	.string "weather. Let it show you what a\l"
	.string "storm can do.\p"
	.string "Our records link TORNADUS to\n"
	.string "ROUTE 119 and THUNDURUS to\l"
	.string "ROUTE 110.\p"
	.string "The route signs list their\n"
	.string "requirements and mark any\l"
	.string "partners you've already caught.\p"
	.string "A researcher at ROUTE 111's\n"
	.string "elevated ruins also needs\l"
	.string "CASTFORM for a weather survey.$"
Route119_WeatherInstitute_2F_Text_RevealGlassPurpose:
	.string "This REVEAL GLASS changes the forms of\n"
	.string "TORNADUS, THUNDURUS, and LANDORUS.\p"
	.string "The northern bridge is clear now.\n"
	.string "Follow ROUTE 119 to FORTREE's GYM.$"

Route119_WeatherInstitute_2F_Text_RevealGlassBagFull:
	.string "The REVEAL GLASS is ready for you, but\n"
	.string "your KEY ITEMS pocket is full.\p"
	.string "I will keep it here until you have room.\n"
	.string "Our survey flight is still available.$"

Route119_WeatherInstitute_2F_Text_ChangingWeatherRidiculous:
	.string "Our instruments track unusual\n"
	.string "weather across HOENN.\p"
	.string "POKéMON CENTER guides track local\n"
	.string "legendary leads. Look near the PC\l"
	.string "to hear where to search.\p"
	.string "The road north leads to FORTREE.\n"
	.string "You're welcome to rest here.$"
Route119_WeatherInstitute_2F_Text_GroudonWeather:
	.string "I track weather patterns over\n"
	.string "the HOENN region.\p"
	.string "Presently, a drought has been recorded\n"
	.string "in {STR_VAR_1}.\p"
	.string "Could that mean, somewhere near\n"
	.string "{STR_VAR_1}…$"

Route119_WeatherInstitute_2F_Text_KyogreWeather:
	.string "I track weather patterns over\n"
	.string "the HOENN region.\p"
	.string "Presently, heavy rainfall has been\n"
	.string "recorded over {STR_VAR_1}.\p"
	.string "Could that mean, somewhere near\n"
	.string "{STR_VAR_1}…$"

Route119_WeatherInstitute_2F_Text_NoAbnormalWeather:
	.string "Abnormal weather conditions are\n"
	.string "no longer being reported.\p"
	.string "The occasional rainfall is a blessing,\n"
	.string "really.$"

Route119_WeatherInstitute_2F_Text_Departure:
	.string "Our research flier will take you\n"
	.string "to the elevated ruins. Keep your\l"
	.string "CASTFORM close during the ascent.$"

Route119_WeatherInstitute_2F_EventScript_SurveyConclusion::
	msgbox Route119_WeatherInstitute_2F_Text_SurveyConclusion, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_2F_Text_SurveyConclusion:
	.string "Now we can separate local weather\n"
	.string "from the disturbance offshore.\l"
	.string "This is what AQUA tried to take.\p"
	.string "STEVEN is following the old\n"
	.string "account east. Show him what you\l"
	.string "saw when your paths meet.\p"
	.string "Continue north on ROUTE 119 to\n"
	.string "FORTREE. After your rival, meet\l"
	.string "STEVEN on ROUTE 120, east of town.\p"
	.string "His DEVON SCOPE investigation\n"
	.string "comes before WINONA's GYM.$"
