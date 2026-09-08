CaveOfOrigin_DianciesRoom_MapScripts::
	.byte 0

CaveOfOrigin_DianciesRoom_EventScript_WallaceExhibition::
	lock
	faceplayer
	goto_if_unset FLAG_SYS_GAME_CLEAR, CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady
	goto_if_defeated TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CaveOfOrigin_DianciesRoom_EventScript_WallaceDecline
	call EventScript_CheckStoryDoubleBattleParty
	trainerbattle_double TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_Text_WallaceIntro, CaveOfOrigin_DianciesRoom_Text_WallaceDefeat, EmeraldChampions_Text_NeedTwoPokemon
	goto CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter

CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceAfter, MSGBOX_DEFAULT
	release
	end

CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceNotReady, MSGBOX_DEFAULT
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
	.string "The chamber has heard two\n"
	.string "Champions answer one another.\p"
	.string "The DIANCITE beside its guardian\n"
	.string "belongs to your exploration. Our\l"
	.string "battle was its own reward.$"
CaveOfOrigin_DianciesRoom_Text_WallaceNotReady:
	.string "First defeat me at the POKéMON LEAGUE.\n"
	.string "Then return for the unrestricted battle.$"

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
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_LegendaryResting
	goto CaveOfOrigin_DianciesRoom_EventScript_DiancieCaught
	end

CaveOfOrigin_DianciesRoom_EventScript_DiancieCaught::
	setflag FLAG_EC_CAUGHT_DIANCIE
	removeobject LOCALID_EC_DIANCIE
	release
	end

CaveOfOrigin_DianciesRoom_Text_WallaceOffer::
	.string "Would you face my unrestricted\n"
	.string "rain exhibition here?\p"
	.string "The chamber's guardian and its\n"
	.string "stone will wait while we battle.$"

CaveOfOrigin_DianciesRoom_Text_WallaceDecline::
	.string "Whenever your team is ready. I\n"
	.string "will be here.$"

CaveOfOrigin_DianciesRoom_EventScript_WallaceDecline::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceDecline, MSGBOX_DEFAULT
	release
	end
