CaveOfOrigin_DianciesRoom_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Common_EventScript_RemoveOverworldAfterBattle
	.byte 0

CaveOfOrigin_DianciesRoom_EventScript_Diancie::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_DIANCIE, 2
	delay 40
	waitmoncry
	specialvar VAR_RESULT, GetHighestLevelInPlayerParty
	copyvar VAR_0x800A, VAR_RESULT
	setwildbattle SPECIES_DIANCIE, VAR_0x800A, ITEM_FAIRY_GEM
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	@ Only a capture resolves it. A knockout, escape, Red Card or Emergency
	@ Exit ending (B_OUTCOME_MON_TELEPORTED) retreats and stays catchable.
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, CaveOfOrigin_DianciesRoom_EventScript_RanFromDiancie
	setflag FLAG_DEFEATED_DIANCIE
	release
	end

CaveOfOrigin_DianciesRoom_EventScript_RanFromDiancie::
	setvar VAR_0x8004, SPECIES_DIANCIE
	goto Common_EventScript_LegendaryFlewAway
	end

CaveOfOrigin_DianciesRoom_EventScript_WallaceExhibition::
	lock
	faceplayer
	goto_if_unset FLAG_SYS_GAME_CLEAR, CaveOfOrigin_DianciesRoom_EventScript_WallaceNotReady
	goto_if_defeated TRAINER_WALLACE_DOUBLES_LEGENDS, CaveOfOrigin_DianciesRoom_EventScript_WallaceAfter
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, CaveOfOrigin_DianciesRoom_EventScript_WallaceDecline
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
	.string "Wallace: Listen to the water in\n"
	.string "this chamber. Even a quiet place\l"
	.string "has its own rhythm.\p"
	.string "We've met at the League. Here, I'd\n"
	.string "like to show you another side of\l"
	.string "my team!$"

CaveOfOrigin_DianciesRoom_Text_WallaceDefeat:
	.string "A magnificent performance!$"

CaveOfOrigin_DianciesRoom_Text_WallaceAfter:
	.string "Wallace: What a pleasure to battle\n"
	.string "you here! The crystals seemed to\l"
	.string "catch every flash of light.\p"
	.string "You needn't win an exhibition to\n"
	.string "meet Diancie or study this place.\l"
	.string "Take your time exploring!$"
CaveOfOrigin_DianciesRoom_Text_WallaceNotReady:
	.string "Wallace: Defeat me at the\n"
	.string "Pokémon League first. Then we can\l"
	.string "hold a special exhibition here.\p"
	.string "You're welcome to explore the\n"
	.string "chamber in the meantime.$"

CaveOfOrigin_DianciesRoom_Text_WallaceOffer::
	.string "Wallace: Would you join me for a\n"
	.string "special exhibition battle here?\p"
	.string "I've brought a different team from\n"
	.string "the one you faced at the League.$"

CaveOfOrigin_DianciesRoom_Text_WallaceDecline::
	.string "Wallace: Whenever your team is\n"
	.string "ready, I will be here.$"

CaveOfOrigin_DianciesRoom_EventScript_WallaceDecline::
	msgbox CaveOfOrigin_DianciesRoom_Text_WallaceDecline, MSGBOX_DEFAULT
	release
	end
