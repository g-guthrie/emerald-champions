Common_EventScript_EmeraldChampionsMoveTutor::
	lockall
	faceplayer
	message EmeraldChampions_Text_MoveTutorHowCanHelp
	waitmessage
	dynmultipush EmeraldChampions_Text_BuildBattleSet, 0
	dynmultipush EmeraldChampions_Text_LearnMove, 1
	dynmultipush EmeraldChampions_Text_Exit, 2
	dynmultistack 0, 0, FALSE, 3, 0, 0, DYN_MULTICHOICE_CB_NONE
	closemessage
	switch VAR_RESULT
	case 0, EmeraldChampions_EventScript_BattleSetChooseMon
	case 1, EmeraldChampions_EventScript_OpenMoveRelearner
	case 2, EmeraldChampions_EventScript_MoveTutorExit
	case MULTI_B_PRESSED, EmeraldChampions_EventScript_MoveTutorExit

EmeraldChampions_EventScript_OpenMoveRelearner:
	releaseall
	goto Common_EventScript_MoveRelearner

EmeraldChampions_EventScript_BattleSetChooseMon:
	msgbox EmeraldChampions_Text_ChooseBattleSetMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EmeraldChampions_EventScript_MoveTutorExit
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleSetEgg
	copyvar VAR_0x800A, VAR_0x8004
	special GetSelectedMonEmeraldChampionsBattleSetCount
	goto_if_eq VAR_RESULT, 0, EmeraldChampions_EventScript_BattleSetMissing
	goto_if_eq VAR_RESULT, 1, EmeraldChampions_EventScript_BattleSetUseDefault

EmeraldChampions_EventScript_BattleSetChooseStyle:
	msgbox EmeraldChampions_Text_ChooseBattleSetStyle, MSGBOX_DEFAULT
	setvar VAR_0x8004, SCROLL_MULTI_EMERALD_CHAMPIONS_BATTLE_SET
	special ShowScrollableMultichoice
	copyvar VAR_0x8006, VAR_RESULT
	special GetSelectedMonEmeraldChampionsBattleSetCount
	goto_if_ge VAR_0x8006, VAR_RESULT, EmeraldChampions_EventScript_BattleSetChooseMon
	copyvar VAR_0x8005, VAR_0x8006
	goto EmeraldChampions_EventScript_BattleSetConfirm

EmeraldChampions_EventScript_BattleSetUseDefault:
	setvar VAR_0x8005, 0

EmeraldChampions_EventScript_BattleSetConfirm:
	copyvar VAR_0x8004, VAR_0x800A
	bufferpartymonnick 0, VAR_0x8004
	special BufferSelectedMonEmeraldChampionsBattleSetName
	goto_if_eq VAR_RESULT, TRUE, EmeraldChampions_EventScript_BattleSetConfirmMega
	msgbox EmeraldChampions_Text_ConfirmBattleSet, MSGBOX_YESNO
	goto EmeraldChampions_EventScript_BattleSetConfirmResult

EmeraldChampions_EventScript_BattleSetConfirmMega:
	msgbox EmeraldChampions_Text_ConfirmMegaBattleSet, MSGBOX_YESNO

EmeraldChampions_EventScript_BattleSetConfirmResult:
	goto_if_eq VAR_RESULT, NO, EmeraldChampions_EventScript_BattleSetChooseStyle
	special ApplySelectedMonEmeraldChampionsBattleSet
	goto_if_eq VAR_RESULT, EC_BATTLE_SET_SPECIAL_ITEM_EQUIPPED, EmeraldChampions_EventScript_BattleSetSpecialItem
	goto_if_eq VAR_RESULT, EC_BATTLE_SET_MEGA, EmeraldChampions_EventScript_BattleSetMegaSuccess
	goto_if_ne VAR_RESULT, EC_BATTLE_SET_SUCCESS, EmeraldChampions_EventScript_BattleSetMissing
	playfanfare MUS_LEVEL_UP
	msgbox EmeraldChampions_Text_BattleSetSuccess, MSGBOX_DEFAULT
	releaseall
	end

EmeraldChampions_EventScript_BattleSetMegaSuccess:
	playfanfare MUS_LEVEL_UP
	msgbox EmeraldChampions_Text_MegaBattleSetSuccess, MSGBOX_DEFAULT
	releaseall
	end

EmeraldChampions_EventScript_BattleSetEgg:
	msgbox EmeraldChampions_Text_BattleSetEgg, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_BattleSetChooseMon

EmeraldChampions_EventScript_BattleSetMissing:
	msgbox EmeraldChampions_Text_BattleSetMissing, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_BattleSetChooseMon

EmeraldChampions_EventScript_BattleSetSpecialItem:
	msgbox EmeraldChampions_Text_BattleSetSpecialItem, MSGBOX_DEFAULT
	goto EmeraldChampions_EventScript_BattleSetChooseMon

EmeraldChampions_EventScript_MoveTutorExit:
	msgbox EmeraldChampions_Text_MoveTutorGoodbye, MSGBOX_DEFAULT
	releaseall
	end

Common_EventScript_EmeraldChampionsBattleVendor::
	lock
	faceplayer
	message EmeraldChampions_Text_BattleVendorWelcome
	waitmessage
	special OpenEmeraldChampionsBattleItemMart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

EmeraldChampions_Text_BattleVendorWelcome:
	.string "Competitive held items are free here.\p"
	.string "Take as many as your teams need!$"

EmeraldChampions_Text_MoveTutorHowCanHelp:
	.string "How can I help prepare your Pokémon?$"

EmeraldChampions_Text_BuildBattleSet:
	.string "Build a Battle Set$"

EmeraldChampions_Text_LearnMove:
	.string "Learn a Move$"

EmeraldChampions_Text_Exit:
	.string "Exit$"

EmeraldChampions_Text_ChooseBattleSetMon:
	.string "Choose a Pokémon for a complete\n"
	.string "competitive battle set.$"

EmeraldChampions_Text_ChooseBattleSetStyle:
	.string "Which battle style should I build?$"

EmeraldChampions_Text_ConfirmBattleSet:
	.string "I'll give {STR_VAR_1} the\n"
	.string "{STR_VAR_2} set.\p"
	.string "This updates all four moves, nature,\n"
	.string "Ability, Stat Points, and held item.\p"
	.string "Continue?$"

EmeraldChampions_Text_ConfirmMegaBattleSet:
	.string "I'll give {STR_VAR_1} the\n"
	.string "{STR_VAR_2} set.\p"
	.string "It is designed for {STR_VAR_3}, but\n"
	.string "I will not supply that Mega Stone.\p"
	.string "Continue?$"

EmeraldChampions_Text_BattleSetSuccess:
	.string "All set! {STR_VAR_1}'s moves, nature,\n"
	.string "Ability, Stat Points, and held item\l"
	.string "now match the {STR_VAR_2} set.$"

EmeraldChampions_Text_MegaBattleSetSuccess:
	.string "All set! {STR_VAR_1} is ready for\n"
	.string "the {STR_VAR_2} role.\p"
	.string "Use this set with {STR_VAR_3}.$"

EmeraldChampions_Text_BattleSetEgg:
	.string "An Egg can't use a battle set.$"

EmeraldChampions_Text_BattleSetMissing:
	.string "I don't have a finished battle set\n"
	.string "for that Pokémon yet.$"

EmeraldChampions_Text_BattleSetSpecialItem:
	.string "I won't discard that special item.\p"
	.string "Remove it before changing sets.$"

EmeraldChampions_Text_MoveTutorGoodbye:
	.string "Come back whenever you want to\n"
	.string "change your team.$"
