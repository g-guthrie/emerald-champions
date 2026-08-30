MauvilleCity_GameCorner_MapScripts::
	.byte 0

	@ Game Corner prices
	.set DOLL_COINS, 1000

	.set COINS_PRICE_50,  1000
	.set COINS_PRICE_500, 10000

MauvilleCity_GameCorner_EventScript_CoinsClerk::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_ThisIsMauvilleGameCorner, MSGBOX_DEFAULT
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NeedCoinCase
	message MauvilleCity_GameCorner_Text_WereYouLookingForCoins
	waitmessage
	showmoneybox 0, 0
	showcoinsbox 0, 5
	goto MauvilleCity_GameCorner_EventScript_ChooseCoinsDefault50

MauvilleCity_GameCorner_EventScript_ChooseCoinsDefault50::
	multichoicedefault 15, 0, MULTI_GAME_CORNER_COINS, 0, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_Buy50Coins
	case 1, MauvilleCity_GameCorner_EventScript_Buy500Coins
	goto MauvilleCity_GameCorner_EventScript_CancelBuyCoins
	end

@ Unused
MauvilleCity_GameCorner_EventScript_ChooseCoinsDefault500::
	multichoicedefault 15, 0, MULTI_GAME_CORNER_COINS, 1, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_Buy50Coins
	case 1, MauvilleCity_GameCorner_EventScript_Buy500Coins
	goto MauvilleCity_GameCorner_EventScript_CancelBuyCoins
	end

MauvilleCity_GameCorner_EventScript_Buy50Coins::
	checkcoins VAR_TEMP_1
	goto_if_ge VAR_TEMP_1, (MAX_COINS + 1 - 50), MauvilleCity_GameCorner_EventScript_NoRoomForCoins
	checkmoney COINS_PRICE_50
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NotEnoughMoney
	addcoins 50
	removemoney COINS_PRICE_50
	updatemoneybox
	updatecoinsbox
	playse SE_SHOP
	msgbox MauvilleCity_GameCorner_Text_ThankYouHereAreYourCoins, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox
	release
	end

MauvilleCity_GameCorner_EventScript_Buy500Coins::
	checkcoins VAR_TEMP_1
	goto_if_ge VAR_TEMP_1, (MAX_COINS + 1 - 500), MauvilleCity_GameCorner_EventScript_NoRoomForCoins
	checkmoney COINS_PRICE_500
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NotEnoughMoney
	addcoins 500
	removemoney COINS_PRICE_500
	updatemoneybox
	updatecoinsbox
	playse SE_SHOP
	msgbox MauvilleCity_GameCorner_Text_ThankYouHereAreYourCoins, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox
	release
	end

MauvilleCity_GameCorner_EventScript_NeedCoinCase::
	msgbox MauvilleCity_GameCorner_Text_NeedCoinCaseForCoins, MSGBOX_DEFAULT
	release
	end

MauvilleCity_GameCorner_EventScript_NotEnoughMoney::
	msgbox MauvilleCity_GameCorner_Text_DontHaveEnoughMoney, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox
	release
	end

MauvilleCity_GameCorner_EventScript_CancelBuyCoins::
	msgbox MauvilleCity_GameCorner_Text_DontNeedCoinsThen, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox
	release
	end

MauvilleCity_GameCorner_EventScript_NoRoomForCoins::
	msgbox MauvilleCity_GameCorner_Text_CoinCaseIsFull, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox
	release
	end

MauvilleCity_GameCorner_EventScript_PrizeCornerDolls::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_ExchangeCoinsForPrizes, MSGBOX_DEFAULT
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_GameCorner_EventScript_ChooseDollPrizeMessage
	release
	end

MauvilleCity_GameCorner_EventScript_ChooseDollPrizeMessage::
	message MauvilleCity_GameCorner_Text_WhichPrize
	waitmessage
	setvar VAR_TEMP_1, 0
	showcoinsbox 0, 0
	goto MauvilleCity_GameCorner_EventScript_ChooseDollPrize

MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize::
	message MauvilleCity_GameCorner_Text_WhichPrize
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
	checkcoins VAR_TEMP_2
	goto_if_lt VAR_TEMP_2, DOLL_COINS, MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForDoll
	bufferdecorationname STR_VAR_2, DECOR_TREECKO_DOLL
	checkdecorspace DECOR_TREECKO_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForDoll
	removecoins DOLL_COINS
	adddecoration DECOR_TREECKO_DOLL
	updatecoinsbox
	playse SE_SHOP
	msgbox MauvilleCity_GameCorner_Text_SendToYourHomePC, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize
	end

MauvilleCity_GameCorner_EventScript_BuyTorchicDoll::
	checkcoins VAR_TEMP_2
	goto_if_lt VAR_TEMP_2, DOLL_COINS, MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForDoll
	bufferdecorationname STR_VAR_2, DECOR_TORCHIC_DOLL
	checkdecorspace DECOR_TORCHIC_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForDoll
	removecoins DOLL_COINS
	adddecoration DECOR_TORCHIC_DOLL
	updatecoinsbox
	playse SE_SHOP
	msgbox MauvilleCity_GameCorner_Text_SendToYourHomePC, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize
	end

MauvilleCity_GameCorner_EventScript_BuyMudkipDoll::
	checkcoins VAR_TEMP_2
	goto_if_lt VAR_TEMP_2, DOLL_COINS, MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForDoll
	bufferdecorationname STR_VAR_2, DECOR_MUDKIP_DOLL
	checkdecorspace DECOR_MUDKIP_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForDoll
	removecoins DOLL_COINS
	adddecoration DECOR_MUDKIP_DOLL
	updatecoinsbox
	playse SE_SHOP
	msgbox MauvilleCity_GameCorner_Text_SendToYourHomePC, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize
	end

MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForDoll::
	msgbox MauvilleCity_GameCorner_Text_NotEnoughCoins, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize
	end

MauvilleCity_GameCorner_EventScript_NoRoomForDoll::
	call Common_EventScript_NoRoomForDecor
	goto MauvilleCity_GameCorner_EventScript_ReturnToChooseDollPrize
	end

MauvilleCity_GameCorner_EventScript_CancelDollSelect::
	msgbox MauvilleCity_GameCorner_Text_OhIsThatSo, MSGBOX_DEFAULT
	hidecoinsbox
	release
	end

MauvilleCity_GameCorner_EventScript_PrizeCornerPokemon::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_ExchangeCoinsForPrizes, MSGBOX_DEFAULT
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_GameCorner_EventScript_OpenPokemonPrizeMenu
	release
	end

MauvilleCity_GameCorner_EventScript_OpenPokemonPrizeMenu::
	setvar VAR_0x8005, 0
	showcoinsbox 0, 0

MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage::
	message MauvilleCity_GameCorner_Text_WhichPrize
	waitmessage
	multichoice 10, 0, MULTI_GAME_CORNER_POKEMON, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_SelectGenesect
	case 1, MauvilleCity_GameCorner_EventScript_SelectPoipole
	case 2, MauvilleCity_GameCorner_EventScript_ChooseStarterRegion
	goto MauvilleCity_GameCorner_EventScript_CancelPokemonPrize

MauvilleCity_GameCorner_EventScript_SelectGenesect::
	setvar VAR_0x8004, SPECIES_GENESECT
	setvar VAR_0x8006, 7500
	goto MauvilleCity_GameCorner_EventScript_PreparePokemonPrize

MauvilleCity_GameCorner_EventScript_SelectPoipole::
	setvar VAR_0x8004, SPECIES_POIPOLE
	setvar VAR_0x8006, 6500
	goto MauvilleCity_GameCorner_EventScript_PreparePokemonPrize

MauvilleCity_GameCorner_EventScript_ChooseStarterRegion::
	message MauvilleCity_GameCorner_Text_WhichStarterRegion
	waitmessage
	setvar VAR_0x8004, SCROLL_MULTI_STARTER_REGIONS
	special ShowScrollableMultichoice
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage
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
	goto MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage

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
	setvar VAR_0x8006, 500

MauvilleCity_GameCorner_EventScript_PreparePokemonPrize::
	special IsEmeraldChampionsGameCornerPokemonClaimed
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_GameCorner_EventScript_AlreadyReceivedPokemonPrize
	goto MauvilleCity_GameCorner_EventScript_ConfirmPokemonPrize

MauvilleCity_GameCorner_EventScript_ConfirmPokemonPrize::
	bufferspeciesname STR_VAR_1, VAR_0x8004
	msgbox MauvilleCity_GameCorner_Text_ConfirmPokemonPrize, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage
	checkcoins VAR_TEMP_2
	goto_if_lt VAR_TEMP_2, VAR_0x8006, MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForPokemon
	special GiveEmeraldChampionsGameCornerPokemon
	goto_if_eq VAR_RESULT, MON_CANT_GIVE, MauvilleCity_GameCorner_EventScript_NoRoomForPokemon
	goto_if_eq VAR_RESULT, EC_GAME_CORNER_PRIZE_SET_FAILED, MauvilleCity_GameCorner_EventScript_PokemonPrizeSetFailed
	removecoins VAR_0x8006
	updatecoinsbox
	playse SE_SHOP

MauvilleCity_GameCorner_EventScript_PokemonPrizeReceived::
	msgbox MauvilleCity_GameCorner_Text_PokemonPrizeReceived, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage

MauvilleCity_GameCorner_EventScript_AlreadyReceivedPokemonPrize::
	msgbox MauvilleCity_GameCorner_Text_AlreadyReceivedPokemonPrize, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage

MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForPokemon::
	msgbox MauvilleCity_GameCorner_Text_NotEnoughCoins, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage

MauvilleCity_GameCorner_EventScript_NoRoomForPokemon::
	msgbox gText_NoMoreRoomForPokemon, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage

MauvilleCity_GameCorner_EventScript_PokemonPrizeSetFailed::
	msgbox MauvilleCity_GameCorner_Text_PokemonPrizeSetFailed, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChoosePokemonPrizeMessage

MauvilleCity_GameCorner_EventScript_CancelPokemonPrize::
	hidecoinsbox
	release
	end

MauvilleCity_GameCorner_Text_ConfirmPokemonPrize:
	.string "Exchange your Coins for {STR_VAR_1}?$"

MauvilleCity_GameCorner_Text_PokemonPrizeReceived:
	.string "{STR_VAR_1} arrived battle-ready!\p"
	.string "Its moves, nature, Ability, and item\n"
	.string "match a competitive set.$"

MauvilleCity_GameCorner_Text_AlreadyReceivedPokemonPrize:
	.string "That unique prize has already been\n"
	.string "claimed.$"

MauvilleCity_GameCorner_Text_WhichStarterRegion:
	.string "Starters cost 500 Coins each.\n"
	.string "Which region should I open?$"

MauvilleCity_GameCorner_Text_PokemonPrizeSetFailed:
	.string "That battle set is unavailable.\n"
	.string "No Coins or prize claim were used.$"

MauvilleCity_GameCorner_EventScript_Woman2::
	msgbox MauvilleCity_GameCorner_Text_CoinsAreNeededToPlay, MSGBOX_NPC
	end

MauvilleCity_GameCorner_EventScript_Gentleman::
	msgbox MauvilleCity_GameCorner_Text_RouletteOnlyLuck, MSGBOX_NPC
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
	msgbox MauvilleCity_GameCorner_Text_CantWinJackpot, MSGBOX_DEFAULT
	release
	end

MauvilleCity_GameCorner_EventScript_PokefanM::
	lock
	faceplayer
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_GameCorner_EventScript_TryGive20Coins
	msgbox MauvilleCity_GameCorner_Text_NeedCoinCaseGoNextDoor, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_TryGive20Coins::
	goto_if_set FLAG_RECEIVED_20_COINS, MauvilleCity_GameCorner_EventScript_PokefanMNormal
	checkcoins VAR_TEMP_1
	goto_if_ge VAR_TEMP_1, 1, MauvilleCity_GameCorner_EventScript_PokefanMNormal @ Only give 20 coins if player has no coins
	setflag FLAG_RECEIVED_20_COINS
	addcoins 20
	msgbox MauvilleCity_GameCorner_Text_LuckOnlyLastSoLongTakeCoins, MSGBOX_DEFAULT
	playse SE_SHOP
	goto MauvilleCity_GameCorner_EventScript_PokefanMNormal
	end

MauvilleCity_GameCorner_EventScript_PokefanMNormal::
	msgbox MauvilleCity_GameCorner_Text_MauvilleSomethingForEveryone, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_OldMan::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_RouletteTablesDifferentRates, MSGBOX_DEFAULT
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
	msgbox MauvilleCity_GameCorner_Text_UpTo3CoinsCanBeUsed, MSGBOX_DEFAULT
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
	msgbox MauvilleCity_GameCorner_Text_DifficultToStopOn7, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_Woman::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_HeresSomeSlotsInfo, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_SlotMachine0::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 0
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine1::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 1
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine2::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 2
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine3::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 3
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine4::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 4
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine5::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 5
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine6::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 6
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine7::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 7
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine8::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 8
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine9::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 9
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine10::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 10
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine11::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 11
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_NoCoinCase::
	msgbox MauvilleCity_GameCorner_Text_CantPlayWithNoCoinCase, MSGBOX_DEFAULT
	releaseall
	end

MauvilleCity_GameCorner_Text_ThisIsMauvilleGameCorner:
	.string "This is MAUVILLE GAME CORNER.$"

MauvilleCity_GameCorner_Text_NeedCoinCaseForCoins:
	.string "Okay, you wanted some COINS for\n"
	.string "the games?\p"
	.string "But you don't have a COIN CASE for\n"
	.string "stowing the COINS.$"

MauvilleCity_GameCorner_Text_WereYouLookingForCoins:
	.string "Were you looking for COINS?\p"
	.string "It's ¥1000 for 50 COINS.\n"
	.string "Would you like some?$"

MauvilleCity_GameCorner_Text_ThankYouHereAreYourCoins:
	.string "Thank you very much!\n"
	.string "Here are your COINS!$"

MauvilleCity_GameCorner_Text_DontHaveEnoughMoney:
	.string "Um… You don't appear to have\n"
	.string "enough money…$"

MauvilleCity_GameCorner_Text_CoinCaseIsFull:
	.string "Oh?\n"
	.string "Your COIN CASE is full.$"

MauvilleCity_GameCorner_Text_DontNeedCoinsThen:
	.string "Oh… You don't need COINS, then?\n"
	.string "Good luck on your adventure!$"

MauvilleCity_GameCorner_Text_ExchangeCoinsForPrizes:
	.string "Welcome.\p"
	.string "You can exchange your COINS for\n"
	.string "prizes here.$"

MauvilleCity_GameCorner_Text_WhichPrize:
	.string "Which prize would you like?$"

MauvilleCity_GameCorner_Text_SendToYourHomePC:
	.string "Thank you!\n"
	.string "We'll send it to your PC at home.$"

MauvilleCity_GameCorner_Text_NotEnoughCoins:
	.string "You don't have enough COINS.$"

@ Unused
MauvilleCity_GameCorner_Text_NoRoomForPlacingDecor:
	.string "There isn't any room available for\n"
	.string "placing {STR_VAR_1}.$"

MauvilleCity_GameCorner_Text_OhIsThatSo:
	.string "Oh, is that so? \n"
	.string "You need to save some COINS before\l"
	.string "coming back here.$"

MauvilleCity_GameCorner_Text_SoYourChoiceIsX:
	.string "So your choice is {STR_VAR_1}?$"

@ Unused
MauvilleCity_GameCorner_Text_CantCarryAnyMore:
	.string "Oh, you can't carry any more than that.$"

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

MauvilleCity_GameCorner_Text_CantWinJackpot:
	.string "There's a prize I want, but I can't win\n"
	.string "the jackpot.$"

MauvilleCity_GameCorner_Text_NeedCoinCaseGoNextDoor:
	.string "Hey, kid, if you want to play here,\n"
	.string "you need a COIN CASE.\p"
	.string "I think the young lady next door\n"
	.string "had one. Go see her!$"

MauvilleCity_GameCorner_Text_LuckOnlyLastSoLongTakeCoins:
	.string "My luck can only last so long.\n"
	.string "This is too much for me.\l"
	.string "Here, take some COINS!$"

MauvilleCity_GameCorner_Text_MauvilleSomethingForEveryone:
	.string "MAUVILLE has something for\n"
	.string "everyone.\p"
	.string "For me, it's the GAME CORNER.$"

MauvilleCity_GameCorner_Text_RouletteTablesDifferentRates:
	.string "The ROULETTE tables have different\n"
	.string "rates.\p"
	.string "Check your COINS if you're going to\n"
	.string "pick a table.$"

MauvilleCity_GameCorner_Text_EasyToLoseTrackOfTime:
	.string "It's easy to lose track of time in here. \n"
	.string "I should get back to work.$"

MauvilleCity_GameCorner_Text_CoinsAreNeededToPlay:
	.string "COINS are needed to play here\n"
	.string "in the GAME CORNER.$"

MauvilleCity_GameCorner_Text_RouletteOnlyLuck:
	.string "This ROULETTE thing…\n"
	.string "It's rather demanding.\p"
	.string "Win or lose, it's only by luck.$"

MauvilleCity_GameCorner_Text_UpTo3CoinsCanBeUsed:
	.string "Up to three COINS can be used to play\n"
	.string "the SLOTS.$"

MauvilleCity_GameCorner_Text_DifficultToStopOn7:
	.string "It's very difficult to make it stop\n"
	.string "right on “7.”\p"
	.string "If it stops on “7” during the REEL TIME\n"
	.string "bonus game, you'll receive extra COINS.$"

MauvilleCity_GameCorner_Text_HeresSomeSlotsInfo:
	.string "Here's some information for you\n"
	.string "about the SLOTS.\p"
	.string "The more lightning bolts you stock,\n"
	.string "the more REEL TIME chances you get.\p"
	.string "In a game with the maximum five\n"
	.string "REEL TIME chances…\p"
	.string "It is possible to receive four regular\n"
	.string "bonuses, then a big bonus.\p"
	.string "That would total 660 COINS, but it's\n"
	.string "very difficult to get.$"

MauvilleCity_GameCorner_Text_CantPlayWithNoCoinCase:
	.string "You can't play if you don't have\n"
	.string "a COIN CASE.$"
