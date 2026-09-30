.set NATURE_CHANGER_COST, 1

SlateportCity_NameRatersHouse_MapScripts::
	.byte 0

SlateportCity_NameRatersHouse_NatureChanger::
	lockall
	faceplayer
	call_if_unset FLAG_MET_NATURE_CHANGER, SlateportCity_NameRatersHouse_NatureChangerIntro
	specialvar VAR_0x8008, GetNatureChangerBerry
	bufferitemname 0, VAR_0x8008
	msgbox NatureChanger_Text_DailyBerry, MSGBOX_DEFAULT
	msgbox NatureChanger_Text_ChoosePokemon, MSGBOX_DEFAULT
	special ChoosePartyMon
	compare VAR_0x8004, PARTY_SIZE
	goto_if_ge Nature_Changer_No
	specialvar VAR_RESULT, ScriptGetPartyMonSpecies
	compare VAR_RESULT, SPECIES_EGG
	goto_if_eq Nature_Changer_Egg
	special BufferChosenMonNature
	bufferpartymonnick 0, VAR_0x8004
	msgbox NatureChanger_Text_MonHasNature, MSGBOX_DEFAULT
	goto NatureChanger_2

SlateportCity_NameRatersHouse_NatureChangerIntro::
	msgbox NatureChanger_Text_Intro
	setflag FLAG_MET_NATURE_CHANGER
	return

NatureChanger_2::
	specialvar VAR_0x8008, GetNatureChangerBerry
	bufferitemname 1, VAR_0x8008
	msgbox NatureChanger_Text_HowAboutIt, MSGBOX_YESNO
	compare VAR_RESULT, 1
	goto_if_eq Nature_Changer_Yes
	compare VAR_RESULT, 0
	goto_if_eq Nature_Changer_No

Nature_Changer_Yes::
	checkitem VAR_0x8008, NATURE_CHANGER_COST
	compare VAR_RESULT, 1
	goto_if_eq Nature_Changer_Stat_Up
	compare VAR_RESULT, 0
	goto_if_eq Nature_Changer_Berry_Poor

Nature_Changer_No::
	msgbox NatureChanger_Text_SomeOtherTime, MSGBOX_DEFAULT
	releaseall
	end

Nature_Changer_Egg::
	msgbox NatureChanger_Text_ThisIsEgg, MSGBOX_DEFAULT
	releaseall
	end

Nature_Changer_Berry_Poor::
	msgbox NatureChanger_Text_NeedMoreBerries, MSGBOX_DEFAULT
	releaseall
	end

Nature_Changer_Stat_Up::
	message NatureChanger_Text_WhichColorPrefer
	waitmessage
	multichoice 0, 0, MULTI_FLAVORS, 0
	switch VAR_RESULT
	case 0, Nature_Up_ATK
	case 1, Nature_Up_DEF
	case 2, Nature_Up_SPATK
	case 3, Nature_Up_SPDEF
	case 4, Nature_Up_SPEED
	case 5, Nature_Changer_No
	case 127, Nature_Changer_No

Nature_Up_ATK::
	setvar VAR_0x8005, 0
	goto Nature_Changer_Stat_Down

Nature_Up_DEF::
	setvar VAR_0x8005, 1
	goto Nature_Changer_Stat_Down
	
Nature_Up_SPEED::
	setvar VAR_0x8005, 2
	goto Nature_Changer_Stat_Down
	
Nature_Up_SPATK::
	setvar VAR_0x8005, 3
	goto Nature_Changer_Stat_Down
	
Nature_Up_SPDEF::
	setvar VAR_0x8005, 4
	goto Nature_Changer_Stat_Down
	
Nature_Changer_Stat_Down::
	message NatureChanger_Text_WhichColorDislike
	waitmessage
	multichoice 0, 0, MULTI_FLAVORS, 0
	switch VAR_RESULT
	case 0, Nature_Down_ATK
	case 1, Nature_Down_DEF
	case 2, Nature_Down_SPATK
	case 3, Nature_Down_SPDEF
	case 4, Nature_Down_SPEED
	case 5, Nature_Changer_No
	case 127, Nature_Changer_Stat_Up

Nature_Down_ATK::
	setvar VAR_0x8006, 0
	goto Nature_Changer_Nature_Change

Nature_Down_DEF::
	setvar VAR_0x8006, 1
	goto Nature_Changer_Nature_Change
	
Nature_Down_SPEED::
	setvar VAR_0x8006, 2
	goto Nature_Changer_Nature_Change
	
Nature_Down_SPATK::
	setvar VAR_0x8006, 3
	goto Nature_Changer_Nature_Change
	
Nature_Down_SPDEF::
	setvar VAR_0x8006, 4
	goto Nature_Changer_Nature_Change

@ The Nature changes first: VAR_RESULT is FALSE when the choice would change
@ nothing, and then the Berries stay in the Bag and the player picks again.
Nature_Changer_Nature_Change::
	bufferpartymonnick 0, VAR_0x8004
	special ChangePokemonNature
	goto_if_eq VAR_RESULT, FALSE, Nature_Changer_Same_Nature
	removeitem VAR_0x8008, NATURE_CHANGER_COST
	msgbox NatureChanger_Text_SpecialPokeblock, MSGBOX_DEFAULT
	fadescreen 1
	special BufferChosenMonNature
	specialvar VAR_0x8007, GetSelectedMonNicknameAndSpecies
	waitse
	msgbox NatureChanger_Text_Pause, MSGBOX_AUTOCLOSE
	playmoncry VAR_0x8007, 0
	waitmoncry
	fadescreen 0
	playfanfare 0x173
	waitfanfare
	msgbox NatureChanger_Text_MonHasNature1, MSGBOX_AUTOCLOSE
	waitfanfare
	msgbox NatureChanger_Text_ItIsDone, MSGBOX_AUTOCLOSE
	releaseall
	end

Nature_Changer_Same_Nature::
	msgbox NatureChanger_Text_SameNature, MSGBOX_DEFAULT
	goto Nature_Changer_Stat_Up

NatureChanger_Text_Intro:
	.string "Ohohoho! You are a Trainer, yes?\p"
	.string "I was once a great Pokéblock\n"
	.string "chef in Kalos!\p"
	.string "Most Natures raise one stat and\n"
	.string "lower another. Neutral ones don't.\p"
	.string "Pokérus can change those effects.\n"
	.string "Its benefit follows a new Nature!\p"
	.string "For just one Berry, my special\n"
	.string "Pokéblock gives it a new Nature!$"

NatureChanger_Text_DailyBerry:
	.string "Today's ingredient is\n"
	.string "one {STR_VAR_1}!\p"
	.string "My ingredients grow on Routes\n"
	.string "102, 103, 104 and 116.\p"
	.string "Today's choice stays the same\n"
	.string "all day. Grow a few of each!$"

NatureChanger_Text_ChoosePokemon:
	.string "Which Pokémon shall try my\n"
	.string "special Pokéblock?$"

NatureChanger_Text_MonHasNature:
	.string "Ohohoho! {STR_VAR_1}'s Nature\n"
	.string "is {STR_VAR_2}.\p"
	.string "It {STR_VAR_3}.$"

NatureChanger_Text_HowAboutIt:
	.string "Shall I give it a new Nature\n"
	.string "for one {STR_VAR_2}?$"

NatureChanger_Text_SomeOtherTime:
	.string "No?!\n"
	.string "Surely you cannot mean this!$"

NatureChanger_Text_ThisIsEgg:
	.string "This is but an Egg!\p"
	.string "An Egg cannot eat my\n"
	.string "special Pokéblock!$"

NatureChanger_Text_NeedMoreBerries:
	.string "You need one {STR_VAR_2}.\n"
	.string "Bring one for my Pokéblock!$"

NatureChanger_Text_WhichColorPrefer:
	.string "Which stat should the new Nature\n"
	.string "raise?$"

NatureChanger_Text_WhichColorDislike:
	.string "Choose its other Nature stat.\n"
	.string "Spring treatment boosts it instead.\p"
	.string "The same stat twice cancels out.$"

NatureChanger_Text_SameNature:
	.string "Ohohoho! That would not change\n"
	.string "{STR_VAR_1}'s stats. Choose again!$"

NatureChanger_Text_SpecialPokeblock:
	.string "Ohohoho! I will make a special\n"
	.string "Pokéblock from your Berry.\p"
	.string "Wait just a moment…$"

NatureChanger_Text_Pause:
	.string "… … … … … …\n"
	.string "… … … … … …$"

NatureChanger_Text_ItIsDone:
	.string "Oho! It is done!\p"
	.string "May it help your partner\n"
	.string "in the battles ahead!$"

NatureChanger_Text_MonHasNature1:
	.string "{STR_VAR_1} ate the special\n"
	.string "Pokéblock.\p"
	.string "Its Nature is now {STR_VAR_2}!\n"
	.string "It {STR_VAR_3}.$"
