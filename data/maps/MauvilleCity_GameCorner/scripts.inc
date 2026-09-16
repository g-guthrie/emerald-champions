MauvilleCity_GameCorner_MapScripts::
	.byte 0

	@ Mauville Starter Archive prices, paid in money over the counter.
	@ Doll ¥1,000, starter ¥10,000. The economy reader resolves the starter
	@ price from the literal setvar VAR_0x8006 below and the single removemoney.

MauvilleCity_GameCorner_EventScript_CoinsClerk::
	msgbox MauvilleCity_GameCorner_Text_ArchiveGreeting, MSGBOX_NPC
	end

MauvilleCity_GameCorner_EventScript_PrizeCornerDolls::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_DollCounter, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseDollPrizeMessage

MauvilleCity_GameCorner_EventScript_ChooseDollPrizeMessage::
	message MauvilleCity_GameCorner_Text_WhichDoll
	waitmessage
	setvar VAR_TEMP_1, 0
	showmoneybox 0, 0
	goto MauvilleCity_GameCorner_EventScript_ChooseDollPrize

MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize::
	message MauvilleCity_GameCorner_Text_WhichDoll
	waitmessage
	goto MauvilleCity_GameCorner_EventScript_ChooseDollPrize

MauvilleCity_GameCorner_EventScript_ChooseDollPrize::
	multichoice 12, 0, MULTI_GAME_CORNER_DOLLS, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_TreeckoDoll
	case 1, MauvilleCity_GameCorner_EventScript_TorchicDoll
	case 2, MauvilleCity_GameCorner_EventScript_MudkipDoll
	case 3, MauvilleCity_GameCorner_EventScript_CancelDollSelect
	goto MauvilleCity_GameCorner_EventScript_CancelDollSelect
	end

MauvilleCity_GameCorner_EventScript_TreeckoDoll::
	setvar VAR_TEMP_1, 1
	bufferdecorationname STR_VAR_1, DECOR_TREECKO_DOLL
	goto MauvilleCity_GameCorner_EventScript_ConfirmDollPrize

MauvilleCity_GameCorner_EventScript_TorchicDoll::
	setvar VAR_TEMP_1, 2
	bufferdecorationname STR_VAR_1, DECOR_TORCHIC_DOLL
	goto MauvilleCity_GameCorner_EventScript_ConfirmDollPrize

MauvilleCity_GameCorner_EventScript_MudkipDoll::
	setvar VAR_TEMP_1, 3
	bufferdecorationname STR_VAR_1, DECOR_MUDKIP_DOLL
	goto MauvilleCity_GameCorner_EventScript_ConfirmDollPrize

MauvilleCity_GameCorner_EventScript_ConfirmDollPrize::
	msgbox MauvilleCity_GameCorner_Text_SoYourChoiceIsX, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MauvilleCity_GameCorner_EventScript_CancelDollSelect
	switch VAR_TEMP_1
	case 1, MauvilleCity_GameCorner_EventScript_BuyTreeckoDoll
	case 2, MauvilleCity_GameCorner_EventScript_BuyTorchicDoll
	case 3, MauvilleCity_GameCorner_EventScript_BuyMudkipDoll
	end

MauvilleCity_GameCorner_EventScript_BuyTreeckoDoll::
	checkmoney 1000
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NotEnoughMoneyForDoll
	bufferdecorationname STR_VAR_2, DECOR_TREECKO_DOLL
	checkdecorspace DECOR_TREECKO_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForDoll
	removemoney 1000
	adddecoration DECOR_TREECKO_DOLL
	goto MauvilleCity_GameCorner_EventScript_DollBought

MauvilleCity_GameCorner_EventScript_BuyTorchicDoll::
	checkmoney 1000
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NotEnoughMoneyForDoll
	bufferdecorationname STR_VAR_2, DECOR_TORCHIC_DOLL
	checkdecorspace DECOR_TORCHIC_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForDoll
	removemoney 1000
	adddecoration DECOR_TORCHIC_DOLL
	goto MauvilleCity_GameCorner_EventScript_DollBought

MauvilleCity_GameCorner_EventScript_BuyMudkipDoll::
	checkmoney 1000
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NotEnoughMoneyForDoll
	bufferdecorationname STR_VAR_2, DECOR_MUDKIP_DOLL
	checkdecorspace DECOR_MUDKIP_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForDoll
	removemoney 1000
	adddecoration DECOR_MUDKIP_DOLL
	goto MauvilleCity_GameCorner_EventScript_DollBought

MauvilleCity_GameCorner_EventScript_DollBought::
	updatemoneybox
	playse SE_SHOP
	msgbox MauvilleCity_GameCorner_Text_SendToYourHomePC, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize
	end

MauvilleCity_GameCorner_EventScript_NotEnoughMoneyForDoll::
	msgbox MauvilleCity_GameCorner_Text_DontHaveEnoughMoney, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize
	end

MauvilleCity_GameCorner_EventScript_NoRoomForDoll::
	call Common_EventScript_NoRoomForDecor
	goto MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize
	end

MauvilleCity_GameCorner_EventScript_CancelDollSelect::
	msgbox MauvilleCity_GameCorner_Text_OhIsThatSo, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

MauvilleCity_GameCorner_EventScript_PrizeCornerPokemon::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_ArchiveClerk, MSGBOX_DEFAULT
	setvar VAR_0x8005, 0
	showmoneybox 0, 0
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_ChooseStarterRegion::
	message MauvilleCity_GameCorner_Text_WhichStarterRegion
	waitmessage
	setvar VAR_0x8004, SCROLL_MULTI_STARTER_REGIONS
	special ShowScrollableMultichoice
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, MauvilleCity_GameCorner_EventScript_CancelPokemonPrize
	copyvar VAR_0x8005, VAR_RESULT
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_StarterKanto
	case 1, MauvilleCity_GameCorner_EventScript_StarterJohto
	case 2, MauvilleCity_GameCorner_EventScript_StarterHoenn
	case 3, MauvilleCity_GameCorner_EventScript_StarterSinnoh
	case 4, MauvilleCity_GameCorner_EventScript_StarterUnova
	case 5, MauvilleCity_GameCorner_EventScript_StarterKalos
	case 6, MauvilleCity_GameCorner_EventScript_StarterAlola
	case 7, MauvilleCity_GameCorner_EventScript_StarterGalar
	case 8, MauvilleCity_GameCorner_EventScript_StarterPaldea
	goto MauvilleCity_GameCorner_EventScript_CancelPokemonPrize

MauvilleCity_GameCorner_EventScript_StarterKanto::
	multichoice 11, 0, MULTI_EC_STARTER_ARCHIVE_KANTO, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectBulbasaur
	case 1, MauvilleCity_GameCorner_EventScript_SelectCharmander
	case 2, MauvilleCity_GameCorner_EventScript_SelectSquirtle
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_StarterJohto::
	multichoice 11, 0, MULTI_EC_STARTER_ARCHIVE_JOHTO, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectChikorita
	case 1, MauvilleCity_GameCorner_EventScript_SelectCyndaquil
	case 2, MauvilleCity_GameCorner_EventScript_SelectTotodile
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_StarterHoenn::
	multichoice 11, 0, MULTI_EC_STARTER_ARCHIVE_HOENN, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectTreecko
	case 1, MauvilleCity_GameCorner_EventScript_SelectTorchic
	case 2, MauvilleCity_GameCorner_EventScript_SelectMudkip
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_StarterSinnoh::
	multichoice 11, 0, MULTI_EC_STARTER_ARCHIVE_SINNOH, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectTurtwig
	case 1, MauvilleCity_GameCorner_EventScript_SelectChimchar
	case 2, MauvilleCity_GameCorner_EventScript_SelectPiplup
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_StarterUnova::
	multichoice 11, 0, MULTI_EC_STARTER_ARCHIVE_UNOVA, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectSnivy
	case 1, MauvilleCity_GameCorner_EventScript_SelectTepig
	case 2, MauvilleCity_GameCorner_EventScript_SelectOshawott
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_StarterKalos::
	multichoice 11, 0, MULTI_EC_STARTER_ARCHIVE_KALOS, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectChespin
	case 1, MauvilleCity_GameCorner_EventScript_SelectFennekin
	case 2, MauvilleCity_GameCorner_EventScript_SelectFroakie
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_StarterAlola::
	multichoice 11, 0, MULTI_EC_STARTER_ARCHIVE_ALOLA, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectRowlet
	case 1, MauvilleCity_GameCorner_EventScript_SelectLitten
	case 2, MauvilleCity_GameCorner_EventScript_SelectPopplio
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_StarterGalar::
	multichoice 11, 0, MULTI_EC_STARTER_ARCHIVE_GALAR, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectGrookey
	case 1, MauvilleCity_GameCorner_EventScript_SelectScorbunny
	case 2, MauvilleCity_GameCorner_EventScript_SelectSobble
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_StarterPaldea::
	multichoice 11, 0, MULTI_EC_STARTER_ARCHIVE_PALDEA, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectSprigatito
	case 1, MauvilleCity_GameCorner_EventScript_SelectFuecoco
	case 2, MauvilleCity_GameCorner_EventScript_SelectQuaxly
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_SelectBulbasaur::
	setvar VAR_0x8004, SPECIES_BULBASAUR
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectCharmander::
	setvar VAR_0x8004, SPECIES_CHARMANDER
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectSquirtle::
	setvar VAR_0x8004, SPECIES_SQUIRTLE
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectChikorita::
	setvar VAR_0x8004, SPECIES_CHIKORITA
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectCyndaquil::
	setvar VAR_0x8004, SPECIES_CYNDAQUIL
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectTotodile::
	setvar VAR_0x8004, SPECIES_TOTODILE
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectTreecko::
	setvar VAR_0x8004, SPECIES_TREECKO
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectTorchic::
	setvar VAR_0x8004, SPECIES_TORCHIC
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectMudkip::
	setvar VAR_0x8004, SPECIES_MUDKIP
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectTurtwig::
	setvar VAR_0x8004, SPECIES_TURTWIG
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectChimchar::
	setvar VAR_0x8004, SPECIES_CHIMCHAR
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectPiplup::
	setvar VAR_0x8004, SPECIES_PIPLUP
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectSnivy::
	setvar VAR_0x8004, SPECIES_SNIVY
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectTepig::
	setvar VAR_0x8004, SPECIES_TEPIG
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectOshawott::
	setvar VAR_0x8004, SPECIES_OSHAWOTT
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectChespin::
	setvar VAR_0x8004, SPECIES_CHESPIN
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectFennekin::
	setvar VAR_0x8004, SPECIES_FENNEKIN
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectFroakie::
	setvar VAR_0x8004, SPECIES_FROAKIE
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectRowlet::
	setvar VAR_0x8004, SPECIES_ROWLET
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectLitten::
	setvar VAR_0x8004, SPECIES_LITTEN
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectPopplio::
	setvar VAR_0x8004, SPECIES_POPPLIO
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectGrookey::
	setvar VAR_0x8004, SPECIES_GROOKEY
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectScorbunny::
	setvar VAR_0x8004, SPECIES_SCORBUNNY
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectSobble::
	setvar VAR_0x8004, SPECIES_SOBBLE
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectSprigatito::
	setvar VAR_0x8004, SPECIES_SPRIGATITO
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectFuecoco::
	setvar VAR_0x8004, SPECIES_FUECOCO
	goto MauvilleCity_GameCorner_EventScript_PrepareStarterPrize
MauvilleCity_GameCorner_EventScript_SelectQuaxly::
	setvar VAR_0x8004, SPECIES_QUAXLY

MauvilleCity_GameCorner_EventScript_PrepareStarterPrize::
	setvar VAR_0x8006, 10000

MauvilleCity_GameCorner_EventScript_PreparePokemonPrize::
	special IsEmeraldChampionsGameCornerPokemonClaimed
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_GameCorner_EventScript_AlreadyReceivedPokemonPrize
	goto MauvilleCity_GameCorner_EventScript_ConfirmPokemonPrize

MauvilleCity_GameCorner_EventScript_ConfirmPokemonPrize::
	bufferspeciesname STR_VAR_1, VAR_0x8004
	msgbox MauvilleCity_GameCorner_Text_ConfirmPokemonPrize, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MauvilleCity_GameCorner_EventScript_ChooseStarterRegion
	checkmoney 10000
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NotEnoughMoneyForPokemon
	special GiveEmeraldChampionsGameCornerPokemon
	goto_if_eq VAR_RESULT, EC_GAME_CORNER_PRIZE_ALREADY_CAUGHT, MauvilleCity_GameCorner_EventScript_AlreadyCaughtLegendary
	goto_if_eq VAR_RESULT, MON_CANT_GIVE, MauvilleCity_GameCorner_EventScript_NoRoomForPokemon
	goto_if_eq VAR_RESULT, EC_GAME_CORNER_PRIZE_SET_FAILED, MauvilleCity_GameCorner_EventScript_PokemonPrizeSetFailed
	removemoney 10000
	updatemoneybox
	playse SE_SHOP

MauvilleCity_GameCorner_EventScript_PokemonPrizeReceived::
	msgbox MauvilleCity_GameCorner_Text_PokemonPrizeReceived, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_AlreadyReceivedPokemonPrize::
	msgbox MauvilleCity_GameCorner_Text_AlreadyReceivedPokemonPrize, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_NotEnoughMoneyForPokemon::
	msgbox MauvilleCity_GameCorner_Text_DontHaveEnoughMoney, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_NoRoomForPokemon::
	msgbox gText_NoMoreRoomForPokemon, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_PokemonPrizeSetFailed::
	msgbox MauvilleCity_GameCorner_Text_PokemonPrizeSetFailed, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_EventScript_CancelPokemonPrize::
	hidemoneybox
	release
	end

MauvilleCity_GameCorner_EventScript_AlreadyCaughtLegendary::
	msgbox MauvilleCity_GameCorner_Text_AlreadyCaughtLegendary, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseStarterRegion

MauvilleCity_GameCorner_Text_ArchiveGreeting:
	.string "This is the MAUVILLE STARTER\n"
	.string "ARCHIVE. Everything here is paid for\l"
	.string "the ordinary way, over the counter.$"

MauvilleCity_GameCorner_Text_ArchiveClerk:
	.string "The Archive keeps a hatchling of\n"
	.string "every region's first partner.\p"
	.string "Ten thousand each; they are worth it.$"

MauvilleCity_GameCorner_Text_DollCounter:
	.string "Starter DOLLS for your SECRET BASE.\n"
	.string "One thousand each.$"

MauvilleCity_GameCorner_Text_WhichDoll:
	.string "Which DOLL would you like?$"

MauvilleCity_GameCorner_Text_ConfirmPokemonPrize:
	.string "Take {STR_VAR_1} home for ¥10,000?$"

MauvilleCity_GameCorner_Text_PokemonPrizeReceived:
	.string "{STR_VAR_1} arrived battle-ready!\p"
	.string "Its moves, nature, Ability, and item\n"
	.string "match a competitive set.$"

MauvilleCity_GameCorner_Text_AlreadyReceivedPokemonPrize:
	.string "That hatchling has already left\n"
	.string "with you.$"

MauvilleCity_GameCorner_Text_WhichStarterRegion:
	.string "Which region should I open?$"

MauvilleCity_GameCorner_Text_PokemonPrizeSetFailed:
	.string "That battle set is unavailable.\n"
	.string "Nothing was charged.$"

MauvilleCity_GameCorner_Text_AlreadyCaughtLegendary:
	.string "You have already befriended this\n"
	.string "Pokémon. Nothing was charged.$"

MauvilleCity_GameCorner_Text_DontHaveEnoughMoney:
	.string "Um… You don't appear to have\n"
	.string "enough money…$"

MauvilleCity_GameCorner_Text_SendToYourHomePC:
	.string "Thank you!\n"
	.string "We'll send it to your PC at home.$"

MauvilleCity_GameCorner_Text_OhIsThatSo:
	.string "Oh, is that so?\n"
	.string "Do come back for one.$"

MauvilleCity_GameCorner_Text_SoYourChoiceIsX:
	.string "So your choice is {STR_VAR_1}?$"

MauvilleCity_GameCorner_EventScript_Woman2::
	msgbox MauvilleCity_GameCorner_Text_QuietNowInHere, MSGBOX_NPC
	end

MauvilleCity_GameCorner_EventScript_Gentleman::
	msgbox MauvilleCity_GameCorner_Text_CameForTheHatchlings, MSGBOX_NPC
	end

MauvilleCity_GameCorner_EventScript_Girl::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_STARTER_DOLL, MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll
	msgbox MauvilleCity_GameCorner_Text_GotTwoOfSameDollWantOne, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MauvilleCity_GameCorner_EventScript_DeclineStarterDoll
	switch VAR_STARTER_MON
	case 0, MauvilleCity_GameCorner_EventScript_GiveTreeckoDoll
	case 1, MauvilleCity_GameCorner_EventScript_GiveTorchicDoll
	case 2, MauvilleCity_GameCorner_EventScript_GiveMudkipDoll
	end

MauvilleCity_GameCorner_EventScript_GiveTreeckoDoll::
	bufferdecorationname STR_VAR_2, DECOR_TREECKO_DOLL
	checkdecorspace DECOR_TREECKO_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForStarterDoll
	msgbox MauvilleCity_GameCorner_Text_HereYouGo2, MSGBOX_DEFAULT
	givedecoration DECOR_TREECKO_DOLL
	setflag FLAG_RECEIVED_STARTER_DOLL
	goto MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll
	end

MauvilleCity_GameCorner_EventScript_GiveTorchicDoll::
	bufferdecorationname STR_VAR_2, DECOR_TORCHIC_DOLL
	checkdecorspace DECOR_TORCHIC_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForStarterDoll
	msgbox MauvilleCity_GameCorner_Text_HereYouGo2, MSGBOX_DEFAULT
	givedecoration DECOR_TORCHIC_DOLL
	setflag FLAG_RECEIVED_STARTER_DOLL
	goto MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll
	end

MauvilleCity_GameCorner_EventScript_GiveMudkipDoll::
	bufferdecorationname STR_VAR_2, DECOR_MUDKIP_DOLL
	checkdecorspace DECOR_MUDKIP_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForStarterDoll
	msgbox MauvilleCity_GameCorner_Text_HereYouGo2, MSGBOX_DEFAULT
	givedecoration DECOR_MUDKIP_DOLL
	setflag FLAG_RECEIVED_STARTER_DOLL
	goto MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll
	end

MauvilleCity_GameCorner_EventScript_NoRoomForStarterDoll::
	call Common_EventScript_NoRoomForDecor
	msgbox MauvilleCity_GameCorner_Text_YouWantItButNotNow, MSGBOX_DEFAULT
	release
	end

MauvilleCity_GameCorner_EventScript_DeclineStarterDoll::
	msgbox MauvilleCity_GameCorner_Text_DontBeNegative, MSGBOX_DEFAULT
	release
	end

MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll::
	msgbox MauvilleCity_GameCorner_Text_SavingForAHatchling, MSGBOX_DEFAULT
	release
	end

MauvilleCity_GameCorner_EventScript_PokefanM::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_MauvilleSomethingForEveryone, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_OldMan::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_RememberTheTables, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_Cook::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_EasyToLoseTrackOfTime, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_Man::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_SavedUpForACharmander, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_NPCReturnToSlots::
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

MauvilleCity_GameCorner_EventScript_Maniac::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_MachinesWentQuiet, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_Woman::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_NineRegionsOnTheShelf, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

@ The twelve slot cabinets and the two roulette tables are switched off. The objects
@ themselves are listed for removal from MauvilleCity_GameCorner/map.json.
MauvilleCity_GameCorner_EventScript_SlotMachine0::
MauvilleCity_GameCorner_EventScript_SlotMachine1::
MauvilleCity_GameCorner_EventScript_SlotMachine2::
MauvilleCity_GameCorner_EventScript_SlotMachine3::
MauvilleCity_GameCorner_EventScript_SlotMachine4::
MauvilleCity_GameCorner_EventScript_SlotMachine5::
MauvilleCity_GameCorner_EventScript_SlotMachine6::
MauvilleCity_GameCorner_EventScript_SlotMachine7::
MauvilleCity_GameCorner_EventScript_SlotMachine8::
MauvilleCity_GameCorner_EventScript_SlotMachine9::
MauvilleCity_GameCorner_EventScript_SlotMachine10::
MauvilleCity_GameCorner_EventScript_SlotMachine11::
MauvilleCity_GameCorner_EventScript_MachinesOff::
	msgbox MauvilleCity_GameCorner_Text_MachinesAreSwitchedOff, MSGBOX_SIGN
	end

MauvilleCity_GameCorner_Text_MachinesAreSwitchedOff:
	.string "The machines are switched off.$"

MauvilleCity_GameCorner_Text_QuietNowInHere:
	.string "It is much quieter in here than\n"
	.string "it used to be.$"

MauvilleCity_GameCorner_Text_CameForTheHatchlings:
	.string "I came for a hatchling from a region\n"
	.string "I will never visit.$"

MauvilleCity_GameCorner_Text_GotTwoOfSameDollWantOne:
	.string "I made a mistake and got two of\n"
	.string "the same DOLLS.\p"
	.string "Would you like one of them?$"

MauvilleCity_GameCorner_Text_HereYouGo2:
	.string "Here you go!$"

MauvilleCity_GameCorner_Text_YouWantItButNotNow:
	.string "Huh?\n"
	.string "You want it, but not right now?$"

MauvilleCity_GameCorner_Text_DontBeNegative:
	.string "Oh, don't be so negative!\n"
	.string "You can have this!$"

MauvilleCity_GameCorner_Text_SavingForAHatchling:
	.string "I am saving up for a hatchling.\n"
	.string "Ten thousand is a lot of battles.$"

MauvilleCity_GameCorner_Text_MauvilleSomethingForEveryone:
	.string "MAUVILLE has something for\n"
	.string "everyone.\p"
	.string "For me, it's the ARCHIVE.$"

MauvilleCity_GameCorner_Text_RememberTheTables:
	.string "I remember when this floor was\n"
	.string "all tables and noise.\p"
	.string "The shelves are better company.$"

MauvilleCity_GameCorner_Text_EasyToLoseTrackOfTime:
	.string "It's easy to lose track of time in here.\n"
	.string "I should get back to work.$"

MauvilleCity_GameCorner_Text_SavedUpForACharmander:
	.string "I saved for a CHARMANDER and I have\n"
	.string "not regretted a single battle.$"

MauvilleCity_GameCorner_Text_MachinesWentQuiet:
	.string "The cabinets went quiet years ago.\n"
	.string "Nobody has come to haul them out.$"

MauvilleCity_GameCorner_Text_NineRegionsOnTheShelf:
	.string "Nine regions on one shelf.\n"
	.string "Choose carefully; you pay for each.$"
