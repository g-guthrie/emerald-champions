Route117_EventScript_DaycareMan::
	lock
	faceplayer
	special GetDaycareMonNicknames
	specialvar VAR_RESULT, GetDaycareState
	goto_if_eq VAR_RESULT, DAYCARE_EGG_WAITING, Route117_EventScript_DaycareEggWaiting
	goto_if_eq VAR_RESULT, DAYCARE_ONE_MON, Route117_EventScript_CheckOnOneMon
	goto_if_eq VAR_RESULT, DAYCARE_TWO_MONS, Route117_EventScript_CheckOnTwoMons
	msgbox Route117_Text_SeeWifeIfYoudLikeMeToRaiseMon, MSGBOX_DEFAULT
	release
	end

Route117_EventScript_DaycareEggWaiting::
	msgbox Route117_Text_DoYouWantEgg, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route117_EventScript_DaycareAcceptEgg
	msgbox Route117_Text_IWillKeepDoYouWantIt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route117_EventScript_DaycareAcceptEgg
	msgbox Route117_Text_IllKeepIt, MSGBOX_DEFAULT
	special RejectEggFromDayCare
	release
	end

Route117_EventScript_DaycareAcceptEgg::
	specialvar VAR_RESULT, CalculatePlayerPartyCount
	goto_if_ne VAR_RESULT, PARTY_SIZE, Route117_EventScript_DaycareReceiveEgg
	msgbox Route117_Text_YouHaveNoRoomForIt, MSGBOX_DEFAULT
	release
	end

Route117_EventScript_DaycareReceiveEgg::
	message Route117_Text_ReceivedEgg
	playfanfare MUS_LEVEL_UP
	waitfanfare
	waitbuttonpress
	msgbox Route117_Text_TakeGoodCareOfIt, MSGBOX_DEFAULT
	special GiveEggFromDaycare
	release
	end

Route117_EventScript_CheckMonReceivedMail::
	specialvar VAR_RESULT, CheckDaycareMonReceivedMail
	call_if_eq VAR_RESULT, 1, Route117_EventScript_MonReceivedMail
	return

Route117_EventScript_MonReceivedMail::
	msgbox Route117_Text_FriendlyWithOtherTrainersMon, MSGBOX_DEFAULT
	return

Route117_EventScript_CheckOnOneMon::
	special GetDaycareMonNicknames
	msgbox Route117_Text_YourMonIsDoingFine, MSGBOX_DEFAULT
	setvar VAR_0x8004, 0
	call Route117_EventScript_CheckMonReceivedMail
	release
	end

Route117_EventScript_CheckOnTwoMons::
	special GetDaycareMonNicknames
	msgbox Route117_Text_YourMonsAreDoingFine, MSGBOX_DEFAULT
	special SetDaycareCompatibilityString
	special ShowFieldMessageStringVar4
	waitmessage
	waitbuttonpress
	setvar VAR_0x8004, 0
	call Route117_EventScript_CheckMonReceivedMail
	setvar VAR_0x8004, 1
	call Route117_EventScript_CheckMonReceivedMail
	release
	end

Route117_PokemonDayCare_EventScript_DaycareWoman::
	lock
	faceplayer
	specialvar VAR_RESULT, GetDaycareState
	goto_if_eq VAR_RESULT, DAYCARE_EGG_WAITING, Route117_PokemonDayCare_EventScript_EggWaiting
	goto_if_eq VAR_RESULT, DAYCARE_ONE_MON, Route117_PokemonDayCare_EventScript_OneMonInDaycare
	goto_if_eq VAR_RESULT, DAYCARE_TWO_MONS, Route117_PokemonDayCare_EventScript_TwoMonsInDaycare
	msgbox Route117_PokemonDayCare_Text_WouldYouLikeUsToRaiseAMon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route117_PokemonDayCare_EventScript_GiveMonToRaise
	msgbox Route117_PokemonDayCare_Text_FineThenComeAgain, MSGBOX_DEFAULT
	release
	end

Route117_PokemonDayCare_EventScript_GiveMonToRaise::
	msgbox Route117_PokemonDayCare_Text_WhichMonShouldWeRaise, MSGBOX_DEFAULT
	chooseboxmon SELECT_PC_MON_DAYCARE
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, Route117_PokemonDayCare_EventScript_ComeAgain
	goto_if_eq VAR_0x8004, PC_MON_CHOSEN, Route117_PokemonDayCare_EventScript_DepositSelectedMon
	specialvar VAR_RESULT, CountPartyAliveNonEggMons_IgnoreVar0x8004Slot
	goto_if_eq VAR_RESULT, 0, Route117_PokemonDayCare_EventScript_OnlyOneAliveMon
Route117_PokemonDayCare_EventScript_DepositSelectedMon::
	specialvar VAR_0x8005, GetSelectedMonNicknameAndSpecies
	waitse
	playmoncry VAR_0x8005, CRY_MODE_NORMAL
	msgbox Route117_PokemonDayCare_Text_WellRaiseYourMon, MSGBOX_DEFAULT
	waitmoncry
	special StoreSelectedPokemonInDaycare
	callnative UpdateFollowingPokemon
	incrementgamestat GAME_STAT_USED_DAYCARE
	specialvar VAR_RESULT, GetDaycareState
	goto_if_eq VAR_RESULT, DAYCARE_ONE_MON, Route117_PokemonDayCare_EventScript_CanRaiseOneMore
	release
	end

Route117_PokemonDayCare_EventScript_ComeAgain::
	msgbox Route117_PokemonDayCare_Text_ComeAgain, MSGBOX_DEFAULT
	release
	end

Route117_PokemonDayCare_EventScript_CanRaiseOneMore::
	msgbox Route117_PokemonDayCare_Text_WeCanRaiseOneMore, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route117_PokemonDayCare_EventScript_GiveMonToRaise
	goto Route117_PokemonDayCare_EventScript_ComeAgain
	end

Route117_PokemonDayCare_EventScript_OnlyOneAliveMon::
	msgbox Route117_PokemonDayCare_Text_WhatWillYouBattleWith, MSGBOX_DEFAULT
	release
	end

Route117_PokemonDayCare_EventScript_EggWaiting::
	msgbox Route117_PokemonDayCare_Text_HusbandWasLookingForYou, MSGBOX_DEFAULT
	release
	end

Route117_PokemonDayCare_EventScript_OneMonInDaycare::
	msgbox Route117_PokemonDayCare_Text_GoodToSeeYou, MSGBOX_DEFAULT
	msgbox Route117_PokemonDayCare_Text_WeCanRaiseOneMore, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route117_PokemonDayCare_EventScript_GiveMonToRaise
	msgbox Route117_PokemonDayCare_Text_TakeYourMonBack, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route117_PokemonDayCare_EventScript_TryRetrieveMon
	goto Route117_PokemonDayCare_EventScript_ComeAgain
	end

Route117_PokemonDayCare_EventScript_TryRetrieveMon::
	specialvar VAR_RESULT, CalculatePlayerPartyCount
	goto_if_eq VAR_RESULT, PARTY_SIZE, Route117_PokemonDayCare_EventScript_NoRoom
	specialvar VAR_RESULT, GetDaycareState
	setvar VAR_0x8004, 0
	goto_if_eq VAR_RESULT, DAYCARE_ONE_MON, Route117_PokemonDayCare_EventScript_CostPrompt
	special ShowDaycareLevelMenu
	copyvar VAR_0x8004, VAR_RESULT
	goto_if_eq VAR_RESULT, DAYCARE_EXITED_LEVEL_MENU, Route117_PokemonDayCare_EventScript_ComeAgain
	goto Route117_PokemonDayCare_EventScript_CostPrompt
	end

@ Emerald Champions (C7): board is free, so there is no fee prompt and no money check.
@ GetDaycareCostAndPrepareString still buffers the nickname.
Route117_PokemonDayCare_EventScript_CostPrompt::
	@ VAR_0x8004 holds the board slot here. A PC Pokemon left on the board must
	@ not bypass the party rule on its way back.
	specialvar VAR_RESULT, CanTakeDaycareMonWithinPartyRule
	goto_if_eq VAR_RESULT, FALSE, Route117_PokemonDayCare_EventScript_PartyRuleRefusal
	special GetDaycareCostAndPrepareString
	msgbox Route117_PokemonDayCare_Text_NoChargeTakeItBack, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route117_PokemonDayCare_EventScript_RetrieveMon
	goto Route117_PokemonDayCare_EventScript_ComeAgain
	end

Route117_PokemonDayCare_EventScript_RetrieveMon::
	applymovement LOCALID_DAYCARE_LADY, Route117_PokemonDayCare_Movement_RetrieveDaycareMon
	waitmovement 0
	specialvar VAR_RESULT, TakePokemonFromDaycare
	msgbox Route117_PokemonDayCare_Text_HeresYourMon, MSGBOX_DEFAULT
	waitse
	playmoncry VAR_RESULT, CRY_MODE_NORMAL
	msgbox Route117_PokemonDayCare_Text_TookBackMon, MSGBOX_DEFAULT
	waitmoncry
	specialvar VAR_RESULT, GetDaycareState
	goto_if_eq VAR_RESULT, DAYCARE_ONE_MON, Route117_PokemonDayCare_EventScript_AskRetrieveOtherMon
	goto Route117_PokemonDayCare_EventScript_ComeAgain
	end

Route117_PokemonDayCare_EventScript_AskRetrieveOtherMon::
	msgbox Route117_PokemonDayCare_Text_TakeOtherOneBackToo, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route117_PokemonDayCare_EventScript_TryRetrieveMon
	goto Route117_PokemonDayCare_EventScript_ComeAgain
	end

Route117_PokemonDayCare_EventScript_PartyRuleRefusal::
	msgbox Route117_PokemonDayCare_Text_PartyRuleRefusal, MSGBOX_DEFAULT
	goto Route117_PokemonDayCare_EventScript_ComeAgain
	end

Route117_PokemonDayCare_EventScript_NoRoom::
	msgbox Route117_PokemonDayCare_Text_YourTeamIsFull, MSGBOX_DEFAULT
	release
	end

Route117_PokemonDayCare_Movement_RetrieveDaycareMon:
	delay_16
	delay_16
	face_left
	delay_16
	delay_16
	face_right
	delay_16
	delay_16
	face_up
	walk_slow_up
	set_invisible
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	face_down
	set_visible
	walk_slow_down
	step_end

Route117_PokemonDayCare_EventScript_TwoMonsInDaycare::
	msgbox Route117_PokemonDayCare_Text_GoodToSeeYou, MSGBOX_DEFAULT
	msgbox Route117_PokemonDayCare_Text_TakeYourMonBack, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, Route117_PokemonDayCare_EventScript_TryRetrieveMon
	msgbox Route117_PokemonDayCare_Text_ComeAgain, MSGBOX_DEFAULT
	release
	end

EventScript_EggHatch::
	lockall
	msgbox Text_EggHatchHuh, MSGBOX_DEFAULT
	special EggHatch
	releaseall
	end

Route117_Text_SeeWifeIfYoudLikeMeToRaiseMon:
	.string "I'm the Day-Care Man.\p"
	.string "I help take care of the precious\n"
	.string "Pokémon of Trainers.\p"
	.string "If you'd like me to raise your Pokémon,\n"
	.string "have a word with my wife.$"

Route117_Text_DoYouWantEgg:
	.string "Ah, it's you!\p"
	.string "We were raising your Pokémon,\n"
	.string "and my goodness, were we surprised!\p"
	.string "Your Pokémon had an Egg!\p"
	.string "We don't know how it got there,\n"
	.string "but your Pokémon had it.\p"
	.string "You do want it, yes?$"

Route117_Text_YourMonIsDoingFine:
	.string "Ah, it's you! Good to see you.\n"
	.string "Your {STR_VAR_1}'s doing fine.$"

Route117_Text_IllKeepIt:
	.string "Well then, I'll keep it.\n"
	.string "Thanks!$"

Route117_Text_YouHaveNoRoomForIt:
	.string "You have no room for it…\n"
	.string "Come back when you've made room.$"

Route117_Text_ReceivedEgg:
	.string "{PLAYER} received the Egg from\n"
	.string "the Day-Care Man.$"

Route117_Text_TakeGoodCareOfIt:
	.string "Take good care of it.$"

Route117_Text_FriendlyWithOtherTrainersMon:
	.string "By the way, about your {STR_VAR_1},\n"
	.string "it seemed to be friendly with\l"
	.string "{STR_VAR_2}'s {STR_VAR_3}.\p"
	.string "I may even have seen it receiving\n"
	.string "a piece of Mail.$"

Route117_Text_YourMonsAreDoingFine:
	.string "Ah, it's you! Your {STR_VAR_1} and\n"
	.string "{STR_VAR_2} are doing fine.$"

Route117_Text_IWillKeepDoYouWantIt:
	.string "I really will keep it.\n"
	.string "You do want this, yes?$"

Route117_PokemonDayCare_Text_WouldYouLikeUsToRaiseAMon:
	.string "I'm the Day-Care Lady.\p"
	.string "We raise Pokémon free of charge.\p"
	.string "Would you like us to raise one?$"

Route117_PokemonDayCare_Text_WhichMonShouldWeRaise:
	.string "Choose a Pokémon from your party\n"
	.string "or PC for us to raise.$"

Route117_PokemonDayCare_Text_WellRaiseYourMon:
	.string "Fine, we'll raise your {STR_VAR_1}\n"
	.string "for a while.\p"
	.string "Come back for it later.$"

Route117_PokemonDayCare_Text_WeCanRaiseOneMore:
	.string "We can raise two of your Pokémon.\n"
	.string "Would you like us to raise one more?$"

Route117_PokemonDayCare_Text_HusbandWasLookingForYou:
	.string "My husband was looking for you.$"

Route117_PokemonDayCare_Text_FineThenComeAgain:
	.string "Oh, fine, then.\n"
	.string "Come again.$"

Route117_PokemonDayCare_Text_TakeOtherOneBackToo:
	.string "Will you take back the other one,\n"
	.string "too?$"

Route117_PokemonDayCare_Text_ComeAgain:
	.string "Fine.\n"
	.string "Come again.$"

Route117_PokemonDayCare_Text_GoodToSeeYou:
	.string "Ah, it's you! Good to see you.\n"
	.string "Your Pokémon can only be doing good!$"

Route117_PokemonDayCare_Text_PartyRuleRefusal:
	.string "Oh, dear. Your team already has one\n"
	.string "of that special kind with it.\p"
	.string "A party may hold 1 Legendary or\n"
	.string "Mythical, 1 Ultra Beast and\l"
	.string "1 Paradox Pokémon.\p"
	.string "Store that one in the PC first,\n"
	.string "then come back for this one.$"

Route117_PokemonDayCare_Text_YourTeamIsFull:
	.string "Your Pokémon team is full.\n"
	.string "Make room, then come see me.$"

Route117_PokemonDayCare_Text_NoChargeTakeItBack:
	.string "There is no charge for raising\n"
	.string "{STR_VAR_1}.\p"
	.string "Will you take it back?$"

Route117_PokemonDayCare_Text_HeresYourMon:
	.string "Perfect!\n"
	.string "Here's your Pokémon.$"

Route117_PokemonDayCare_Text_TookBackMon:
	.string "{PLAYER} took back {STR_VAR_1} from\n"
	.string "the Day-Care Lady.$"

Route117_PokemonDayCare_Text_TakeYourMonBack:
	.string "Will you take your Pokémon back?$"

Route117_PokemonDayCare_Text_WhatWillYouBattleWith:
	.string "If you leave me that Pokémon,\n"
	.string "what will you battle with?\p"
	.string "Come back another time.$"

Text_EggHatchHuh:
	.string "Huh?$"
