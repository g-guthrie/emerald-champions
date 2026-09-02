CaveOfOrigin_DianciesRoom_MapScripts::
	.byte 0

CaveOfOrigin_DianciesRoom_EventScript_WallaceExhibition::
	lock
	faceplayer
	goto_if_unset FLAG_SYS_GAME_CLEAR, CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady
	goto_if_defeated TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_EventScript_WallaceReward
	trainerbattle_double TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_Text_WallaceIntro, CaveOfOrigin_DianciesRoom_Text_WallaceDefeat, EmeraldChampions_Text_NeedTwoPokemon

CaveOfOrigin_DianciesRoom_EventScript_WallaceReward::
	checkitem ITEM_OVAL_CHARM, 1
	goto_if_eq VAR_RESULT, TRUE, CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter
	giveitem ITEM_OVAL_CHARM
	goto_if_eq VAR_RESULT, FALSE, CaveOfOrigin_DianciesRoom_EventScript_WallaceBagFull

CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceAfter, MSGBOX_DEFAULT
	release
	end

CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceNotReady, MSGBOX_DEFAULT
	release
	end

CaveOfOrigin_DianciesRoom_EventScript_WallaceBagFull::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceBagFull, MSGBOX_DEFAULT
	release
	end

CaveOfOrigin_DianciesRoom_Text_WallaceIntro:
	.string "This crystal chamber records HOENN's\n"
	.string "deepest answer to pressure.\p"
	.string "As Champion, face my unrestricted rain\n"
	.string "exhibition. Nothing here is ceremonial.$"

CaveOfOrigin_DianciesRoom_Text_WallaceDefeat:
	.string "You found calm inside impossible rain.$"

CaveOfOrigin_DianciesRoom_Text_WallaceAfter:
	.string "Take this OVAL CHARM. Let the next\n"
	.string "generation inherit what you learned.$"

CaveOfOrigin_DianciesRoom_Text_WallaceNotReady:
	.string "First defeat me at the POKéMON LEAGUE.\n"
	.string "Then return for the unrestricted battle.$"

CaveOfOrigin_DianciesRoom_Text_WallaceBagFull:
	.string "Make room for the OVAL CHARM and return.\n"
	.string "A Champion's reward will wait.$"
CaveOfOrigin_DianciesRoom_EventScript_Diancie::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_DIANCIE, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, SPECIES_DIANCIE
	setvar VAR_0x8005, 2
	special CreateEmeraldChampionsStaticLegendaryEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, CaveOfOrigin_DianciesRoom_EventScript_DiancieCaught
	msgbox CaveOfOrigin_DianciesRoom_Text_DiancieRemains, MSGBOX_DEFAULT
	release
	end

CaveOfOrigin_DianciesRoom_EventScript_DiancieCaught::
	setflag FLAG_EC_CAUGHT_DIANCIE
	removeobject LOCALID_EC_DIANCIE
	release
	end

CaveOfOrigin_DianciesRoom_Text_DiancieRemains:
	.string "DIANCIE accepts the challenge, but the\n"
	.string "sanctuary will hold its place for you.$"
