RustboroCity_PokemonSchool_MapScripts::
	.byte 0

RustboroCity_PokemonSchool_EventScript_Blackboard::
	lockall
	msgbox RustboroCity_PokemonSchool_Text_BlackboardListsStatusChanges, MSGBOX_DEFAULT
	goto RustboroCity_PokemonSchool_EventScript_ChooseBlackboardTopic
	end

RustboroCity_PokemonSchool_EventScript_ChooseBlackboardTopic::
	message RustboroCity_PokemonSchool_Text_ReadWhichTopic
	waitmessage
	multichoicegrid 8, 1, MULTI_STATUS_INFO, 3, FALSE
	switch VAR_RESULT
	case 0, RustboroCity_PokemonSchool_EventScript_Poison
	case 1, RustboroCity_PokemonSchool_EventScript_Paralysis
	case 2, RustboroCity_PokemonSchool_EventScript_Sleep
	case 3, RustboroCity_PokemonSchool_EventScript_Burn
	case 4, RustboroCity_PokemonSchool_EventScript_Freeze
	case 5, RustboroCity_PokemonSchool_EventScript_ExitTopicSelect
	case MULTI_B_PRESSED, RustboroCity_PokemonSchool_EventScript_ExitTopicSelect
	end

RustboroCity_PokemonSchool_EventScript_Poison::
	msgbox RustboroCity_PokemonSchool_Text_ExplainPoison, MSGBOX_DEFAULT
	goto RustboroCity_PokemonSchool_EventScript_ChooseBlackboardTopic
	end

RustboroCity_PokemonSchool_EventScript_Paralysis::
	msgbox RustboroCity_PokemonSchool_Text_ExplainParalysis, MSGBOX_DEFAULT
	goto RustboroCity_PokemonSchool_EventScript_ChooseBlackboardTopic
	end

RustboroCity_PokemonSchool_EventScript_Sleep::
	msgbox RustboroCity_PokemonSchool_Text_ExplainSleep, MSGBOX_DEFAULT
	goto RustboroCity_PokemonSchool_EventScript_ChooseBlackboardTopic
	end

RustboroCity_PokemonSchool_EventScript_Burn::
	msgbox RustboroCity_PokemonSchool_Text_ExplainBurn, MSGBOX_DEFAULT
	goto RustboroCity_PokemonSchool_EventScript_ChooseBlackboardTopic
	end

RustboroCity_PokemonSchool_EventScript_Freeze::
	msgbox RustboroCity_PokemonSchool_Text_ExplainFreeze, MSGBOX_DEFAULT
	goto RustboroCity_PokemonSchool_EventScript_ChooseBlackboardTopic
	end

RustboroCity_PokemonSchool_EventScript_ExitTopicSelect::
	releaseall
	end

RustboroCity_PokemonSchool_EventScript_GameboyKid1::
	msgbox RustboroCity_PokemonSchool_Text_TradingRightNow, MSGBOX_NPC
	end

RustboroCity_PokemonSchool_EventScript_GameboyKid2::
	msgbox RustboroCity_PokemonSchool_Text_AlwaysWantedSeedot, MSGBOX_NPC
	end

RustboroCity_PokemonSchool_EventScript_RichBoy::
	msgbox RustboroCity_PokemonSchool_Text_PokemontCantUseManMadeItems, MSGBOX_NPC
	end

RustboroCity_PokemonSchool_EventScript_Lass::
	msgbox RustboroCity_PokemonSchool_Text_ConfusedPokemonAttacksItself, MSGBOX_NPC
	end

RustboroCity_PokemonSchool_EventScript_SchoolKidM::
	msgbox RustboroCity_PokemonSchool_Text_PokemonHealItselfWithBerry, MSGBOX_NPC
	end

RustboroCity_PokemonSchool_EventScript_StudentNotebook::
	msgbox RustboroCity_PokemonSchool_Text_StudentsNotes, MSGBOX_SIGN
	end

RustboroCity_PokemonSchool_EventScript_Teacher::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_SCHOOL_SUN_STONE, RustboroCity_PokemonSchool_EventScript_GaveSunStone
	call_if_eq VAR_FACING, DIR_EAST, RustboroCity_PokemonSchool_EventScript_TeacherCheckOnStudentsEast
	call_if_eq VAR_FACING, DIR_WEST, RustboroCity_PokemonSchool_EventScript_TeacherCheckOnStudentsWest
	msgbox RustboroCity_PokemonSchool_Text_StudentsWhoStudyEveryTool, MSGBOX_DEFAULT
	giveitem ITEM_SUN_STONE
	goto_if_eq VAR_RESULT, 0, Common_EventScript_ShowBagIsFull
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	setflag FLAG_RECEIVED_SCHOOL_SUN_STONE
	release
	end

RustboroCity_PokemonSchool_EventScript_TeacherCheckOnStudentsEast::
	applymovement VAR_LAST_TALKED, RustboroCity_PokemonSchool_Movement_TeacherCheckOnStudentsEast
	waitmovement 0
	return

RustboroCity_PokemonSchool_EventScript_TeacherCheckOnStudentsWest::
	applymovement VAR_LAST_TALKED, RustboroCity_PokemonSchool_Movement_TeacherCheckOnStudentsWest
	waitmovement 0
	return

RustboroCity_PokemonSchool_EventScript_GaveSunStone::
	msgbox RustboroCity_PokemonSchool_Text_ExplainSunStone, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	release
	end

RustboroCity_PokemonSchool_Movement_TeacherCheckOnStudentsWest:
	walk_left
	walk_down
	walk_down
	walk_right
	walk_in_place_faster_down
	delay_16
	delay_16
	delay_16
	walk_down
	walk_in_place_faster_left
	delay_16
	delay_16
	walk_right
	delay_16
	delay_16
	delay_8
	walk_up
	walk_left
	walk_left
	walk_up
	walk_up
	walk_right
	step_end

RustboroCity_PokemonSchool_Movement_TeacherCheckOnStudentsEast:
	walk_right
	walk_right
	walk_down
	walk_down
	walk_left
	walk_left
	walk_in_place_faster_down
	delay_16
	delay_16
	delay_16
	walk_down
	walk_in_place_faster_left
	delay_16
	delay_16
	walk_right
	delay_16
	delay_16
	delay_8
	walk_up
	walk_right
	walk_up
	walk_up
	walk_left
	walk_left
	step_end

RustboroCity_PokemonSchool_EventScript_Scott::
	lock
	faceplayer
	goto_if_set FLAG_MET_SCOTT_AFTER_OBTAINING_STONE_BADGE, RustboroCity_PokemonSchool_EventScript_ScottWatchStudents
	goto_if_set FLAG_MET_SCOTT_RUSTBORO, RustboroCity_PokemonSchool_EventScript_ScottSpokeAlready
	goto_if_set FLAG_BADGE01_GET, RustboroCity_PokemonSchool_EventScript_ScottGreetHasBadge
	msgbox RustboroCity_PokemonSchool_Text_ScottMetAlreadyCut, MSGBOX_DEFAULT
	addvar VAR_SCOTT_STATE, 1
	setflag FLAG_MET_SCOTT_RUSTBORO
	release
	end

RustboroCity_PokemonSchool_EventScript_ScottSpokeAlready::
	goto_if_set FLAG_BADGE01_GET, RustboroCity_PokemonSchool_EventScript_ScottNoticeBadge
	msgbox RustboroCity_PokemonSchool_Text_StudentTalentLevelUnknown, MSGBOX_DEFAULT
	release
	end

RustboroCity_PokemonSchool_EventScript_ScottGreetHasBadge::
	msgbox RustboroCity_PokemonSchool_Text_ScottMetAlreadyStoneBadge, MSGBOX_DEFAULT
	goto RustboroCity_PokemonSchool_EventScript_MetScottAfterBadge
	end

RustboroCity_PokemonSchool_EventScript_ScottNoticeBadge::
	msgbox RustboroCity_PokemonSchool_Text_ScottStoneBadge, MSGBOX_DEFAULT
	goto RustboroCity_PokemonSchool_EventScript_MetScottAfterBadge
	end

RustboroCity_PokemonSchool_EventScript_MetScottAfterBadge::
	addvar VAR_SCOTT_STATE, 1
	setflag FLAG_MET_SCOTT_AFTER_OBTAINING_STONE_BADGE
	release
	end

RustboroCity_PokemonSchool_EventScript_ScottWatchStudents::
	msgbox RustboroCity_PokemonSchool_Text_StudentTalentLevelUnknown, MSGBOX_DEFAULT
	release
	end

RustboroCity_PokemonSchool_Text_BlackboardListsStatusChanges:
	.string "The blackboard lists status changes\n"
	.string "that may affect POKéMON in battle.$"

RustboroCity_PokemonSchool_Text_ReadWhichTopic:
	.string "Which topic do you want to read?$"

RustboroCity_PokemonSchool_Text_ExplainPoison:
	.string "If a POKéMON is poisoned, it steadily\n"
	.string "loses HP during battle.\p"
	.string "Poison remains after battle, but it no\n"
	.string "longer causes damage while traveling.\p"
	.string "Heal it using an ANTIDOTE.$"

RustboroCity_PokemonSchool_Text_ExplainParalysis:
	.string "If a POKéMON becomes paralyzed,\n"
	.string "its SPEED drops.\p"
	.string "It may also not be able to move while\n"
	.string "it is in battle.\p"
	.string "Paralysis remains after battle.\n"
	.string "Cure it using a PARLYZ HEAL.$"

RustboroCity_PokemonSchool_Text_ExplainSleep:
	.string "Sleep prevents most moves until\n"
	.string "the Pokémon wakes.\p"
	.string "SLEEP TALK and SNORE are\n"
	.string "exceptions. A sleeping Pokémon\l"
	.string "stays asleep after battle;\l"
	.string "AWAKENING cures it.$"
RustboroCity_PokemonSchool_Text_ExplainBurn:
	.string "A burn normally lowers damage from\n"
	.string "physical attacks and causes HP\l"
	.string "loss.\p"
	.string "Some Abilities change that result.\n"
	.string "GUTS attackers can benefit from\l"
	.string "burn!\p"
	.string "BURN HEAL removes a burn outside a\n"
	.string "trainer battle.$"
RustboroCity_PokemonSchool_Text_ExplainFreeze:
	.string "A frozen Pokémon usually cannot\n"
	.string "move. It may thaw, and certain\l"
	.string "moves can help it thaw as well.\p"
	.string "Freezing remains after battle. An\n"
	.string "ICE HEAL removes it.$"
RustboroCity_PokemonSchool_Text_StudentsWhoStudyEveryTool:
	.string "Welcome! Today's lesson is\n"
	.string "preparing a team without grinding.\p"
	.string "EVs are effort values you assign\n"
	.string "to strengthen particular stats.\l"
	.string "The CENTER TUTOR edits them free.\p"
	.string "You have 510 EVs to distribute,\n"
	.string "with at most 252 in one stat.\l"
	.string "You can't maximize everything!\p"
	.string "For a simple fast attacker, try\n"
	.string "252 in Speed, 252 in its attacking\l"
	.string "stat, and 4 in HP.\p"
	.string "A Battle Set can assign them for\n"
	.string "you. Change them anytime to suit\l"
	.string "your plan.\p"
	.string "Evolution is another tool.\n"
	.string "This SUN STONE evolves certain\l"
	.string "POKéMON. Try it on a suitable one!$"
RustboroCity_PokemonSchool_Text_ExplainSunStone:
	.string "EVs let you specialize: up to\n"
	.string "252 per stat, 510 total.\l"
	.string "Edit them freely at the TUTOR.\p"
	.string "A SUN STONE works differently:\n"
	.string "use it from the BAG to evolve\l"
	.string "a compatible POKéMON.$"

RustboroCity_PokemonSchool_Text_TradingRightNow:
	.string "I'm trading POKéMON with my friend\n"
	.string "right now.$"

RustboroCity_PokemonSchool_Text_AlwaysWantedSeedot:
	.string "I always wanted a SEEDOT, and\n"
	.string "I'm finally getting one!$"

RustboroCity_PokemonSchool_Text_PokemontCantUseManMadeItems:
	.string "I used the TUTOR's Other Services\n"
	.string "and chose Bonding. You can set\l"
	.string "friendship there for free!\p"
	.string "It helps with friendship-based\n"
	.string "evolutions. Meet the other\l"
	.string "conditions, then use the LEVELER.$"

RustboroCity_PokemonSchool_Text_ConfusedPokemonAttacksItself:
	.string "I changed Difficulty in OPTIONS\n"
	.string "while trying a new team.\p"
	.string "Easy lowers opponent levels by\n"
	.string "four, Medium by two, and Hard\l"
	.string "uses their full battle levels.\p"
	.string "Their plans stay just as clever!\n"
	.string "I still have to learn how my\l"
	.string "partners work together.$"

RustboroCity_PokemonSchool_Text_PokemonHealItselfWithBerry:
	.string "My partner ate its held BERRY…\n"
	.string "and had it back after battle!\p"
	.string "All held items are restored\n"
	.string "after battle, including items\l"
	.string "from Battle Sets.\p"
	.string "An eaten BERRY is still gone for\n"
	.string "the rest of that fight, unless\l"
	.string "a move or Ability recovers it.\p"
	.string "POTIONS and BALLS used from the\n"
	.string "BAG are still spent. I underlined\l"
	.string "that bit in my notes!$"

RustboroCity_PokemonSchool_Text_StudentsNotes:
	.string "It's this student's notebook…\n"
	.string "Move effectiveness symbols:\p"
	.string "{CIRCLE_HOLLOW} Normal: usual damage.\n"
	.string "{CIRCLE_DOT} Super effective: double.\p"
	.string "{STAR} Four times normal damage.\n"
	.string "{BIG_MULT_X} No effect: immune.\p"
	.string "{TRIANGLE} Resisted: half damage.\n"
	.string "{TRIANGLE_UPSIDE_DOWN} Resisted: quarter damage.\p"
	.string "Status moves have no symbol.\n"
	.string "In doubles, the move list shows\l"
	.string "the better matchup of the two.\p"
	.string "Check again when choosing your\n"
	.string "target! The symbol is a matchup\l"
	.string "guide, not an exact damage total.\p"
	.string "L shows foe types. If L is set\n"
	.string "to act as A in OPTIONS, change\l"
	.string "that setting to use this panel.\p"
	.string "In TRAINER battles, R on a move\n"
	.string "opens its details. In catchable\l"
	.string "wild battles, R throws a BALL!$"

RustboroCity_PokemonSchool_Text_ScottMetAlreadyCut:
	.string "SCOTT: We met in PETALBURG, didn't\n"
	.string "we? I'm looking for exceptional\l"
	.string "Trainers.\p"
	.string "The CUTTER nearby has the CUT HM.\n"
	.string "Collect it and the STONE BADGE.\p"
	.string "Then a compatible party member can\n"
	.string "clear thin trees without keeping\l"
	.string "CUT in its battle moves.$"
RustboroCity_PokemonSchool_Text_StudentTalentLevelUnknown:
	.string "SCOTT: Hmm…\p"
	.string "The talent levels of the students here\n"
	.string "are unknown. The potential's there.$"

RustboroCity_PokemonSchool_Text_ScottStoneBadge:
	.string "SCOTT: Oh, what's that?\p"
	.string "It's a STONE BADGE, isn't it?\n"
	.string "That's pretty impressive, I'd say.\p"
	.string "But, you know, I would've loved to see\n"
	.string "you in battle.\p"
	.string "It's hard to tell what you're like as\n"
	.string "a TRAINER from a LEAGUE BADGE.$"

RustboroCity_PokemonSchool_Text_ScottMetAlreadyStoneBadge:
	.string "SCOTT: There you are!\n"
	.string "How did your first GYM battle go?\p"
	.string "A STONE BADGE! Well done.\p"
	.string "I wish I'd seen the battle myself.\n"
	.string "A BADGE shows that you won; I want\l"
	.string "to see how you and your team did it.$"
