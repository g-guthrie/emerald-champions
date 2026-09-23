VerdanturfTown_FriendshipRatersHouse_MapScripts:: @ 8203030
	.byte 0

VerdanturfTown_FriendshipRatersHouse_EventScript_FriendshipRater:: @ 8203031
	lock
	faceplayer
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_SeeHowMuchPokemonLikesYou, MSGBOX_DEFAULT
	specialvar VAR_RESULT, GetLeadMonFriendshipScore
	switch VAR_RESULT
	case 0, VerdanturfTown_FriendshipRatersHouse_EventScript_DetestsYou
	case 1, VerdanturfTown_FriendshipRatersHouse_EventScript_VeryWary
	case 2, VerdanturfTown_FriendshipRatersHouse_EventScript_NotUsedToYou
	case 3, VerdanturfTown_FriendshipRatersHouse_EventScript_GettingUsedToYou
	case 4, VerdanturfTown_FriendshipRatersHouse_EventScript_LikesYouQuiteALot
	case 5, VerdanturfTown_FriendshipRatersHouse_EventScript_VeryHappy
	case 6, VerdanturfTown_FriendshipRatersHouse_EventScript_AdoresYou
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_DetestsYou:: @ 8203094
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_DetestsYou, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_VeryWary:: @ 820309E
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_VeryWary, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_NotUsedToYou:: @ 82030A8
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_NotUsedToYou, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_GettingUsedToYou:: @ 82030B2
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_GettingUsedToYou, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_LikesYouQuiteALot:: @ 82030BC
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_LikesYouQuiteALot, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_VeryHappy:: @ 82030C6
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_VeryHappy, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_AdoresYou:: @ 82030D0
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_AdoresYou, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_Pikachu:: @ 82030DA
	lock
	faceplayer
	waitse
	playmoncry SPECIES_PIKACHU, 0
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_Pikachu, MSGBOX_DEFAULT
	waitmoncry
	release
	end


VerdanturfTown_FriendshipRatersHouse_EventScript_Trader:: @ 8203C11
	lock
	faceplayer
	goto_if_set FLAG_PACIFIDLOG_NPC_TRADE_COMPLETED, VerdanturfTown_FriendshipRatersHouse_EventScript_TradeCompleted
	setvar VAR_0x8008, INGAME_TRADE_CYCLIZAR
	@ This engine reads the trade index from VAR_0x8005 and the party slot from
	@ VAR_0x8004; Inclement passes them the other way round.
	copyvar VAR_0x8005, VAR_0x8008
	specialvar VAR_RESULT, GetInGameTradeSpeciesInfo
	copyvar VAR_0x8009, VAR_RESULT
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_WillingToTradeIt, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq VerdanturfTown_FriendshipRatersHouse_EventScript_DeclineTrade
	special ChoosePartyMon
	copyvar VAR_0x800A, VAR_0x8004
	compare VAR_0x8004, 255
	goto_if_eq VerdanturfTown_FriendshipRatersHouse_EventScript_DeclineTrade
	copyvar VAR_0x8004, VAR_0x800A
	specialvar VAR_RESULT, GetTradeSpecies
	copyvar VAR_0x800B, VAR_RESULT
	compare VAR_RESULT, VAR_0x8009
	goto_if_ne VerdanturfTown_FriendshipRatersHouse_EventScript_NotRequestedMon
	copyvar VAR_0x8004, VAR_0x800A
	specialvar VAR_RESULT, CanReceiveInGameTradePokemon
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_NPCTradeRestrictedParty
	call Common_EventScript_ReturnNPCTradeItem
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_NPCTradeCancelled
	copyvar VAR_0x8004, VAR_0x800A
	copyvar VAR_0x8005, VAR_0x8008
	specialvar VAR_RESULT, GetInGameTradeSpeciesInfo @ re-buffer species names over the item name
	special CreateInGameTradePokemon
	special DoInGameTradeScene
	bufferspeciesname 0, VAR_0x8009
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_ItsSubtlyDifferentThankYou, MSGBOX_DEFAULT
	setflag FLAG_PACIFIDLOG_NPC_TRADE_COMPLETED
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_DeclineTrade:: @ 8203C93
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_NotDesperateOrAnything, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_NotRequestedMon:: @ 8203C9D
	bufferspeciesname 0, VAR_0x8009
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_WontAcceptAnyLessThanRealMon, MSGBOX_DEFAULT
	release
	end

VerdanturfTown_FriendshipRatersHouse_EventScript_TradeCompleted:: @ 8203CAB
	msgbox VerdanturfTown_FriendshipRatersHouse_Text_ReallyWantedToGetBagon, MSGBOX_DEFAULT
	release
	end
	
VerdanturfTown_FriendshipRatersHouse_Text_WillingToTradeIt: @ 8203CBE
	.string "Check out this {STR_VAR_2}!\p"
	.string "It's the {STR_VAR_2} that I caught\n"
	.string "yesterday to celebrate my birthday!\p"
	.string "Oh, I can see that you want it!\n"
	.string "After all, it's priceless!\p"
	.string "I'll tell you what. I might be willing\n"
	.string "to trade it for an Igglybuff.$"

VerdanturfTown_FriendshipRatersHouse_Text_ItsSubtlyDifferentThankYou: @ 8203D87
	.string "Oh, so this is an Igglybuff?\p"
	.string "It's so round and soft… Nothing\n"
	.string "like my Cyclizar at all.\p"
	.string "Thank you!$"

VerdanturfTown_FriendshipRatersHouse_Text_WontAcceptAnyLessThanRealMon: @ 8203DDE
	.string "No, no, no! I won't accept any\n"
	.string "less than a real {STR_VAR_1}!$"

VerdanturfTown_FriendshipRatersHouse_Text_NotDesperateOrAnything: @ 8203E12
	.string "Oh, so you're not going to go through\n"
	.string "with this?\p"
	.string "That's cool. I'm not desperate to make\n"
	.string "a trade or anything.$"

VerdanturfTown_FriendshipRatersHouse_Text_ReallyWantedToGetBagon: @ 8203E7F
	.string "I know I could go looking for one\n"
	.string "on my own, but…\p"
	.string "But I really wanted to get an\n"
	.string "Igglybuff that another Trainer caught…$"

VerdanturfTown_FriendshipRatersHouse_Text_SeeHowMuchPokemonLikesYou: @ 82030ED
	.string "Let me see your Pokémon.\n"
	.string "I'll check to see how much it likes you.\p"
	.string "Oh.\n"
	.string "Your Pokémon…$"

VerdanturfTown_FriendshipRatersHouse_Text_AdoresYou: @ 8203141
	.string "It adores you.\n"
	.string "It can't possibly love you any more.\l"
	.string "I even feel happy seeing it.$"

VerdanturfTown_FriendshipRatersHouse_Text_VeryHappy: @ 8203192
	.string "It seems to be very happy.\n"
	.string "It obviously likes you a whole lot.$"

VerdanturfTown_FriendshipRatersHouse_Text_LikesYouQuiteALot: @ 82031D1
	.string "It likes you quite a lot.\n"
	.string "It seems to want to be babied a little.$"

VerdanturfTown_FriendshipRatersHouse_Text_GettingUsedToYou: @ 8203213
	.string "It's getting used to you.\n"
	.string "It seems to believe in you.$"

VerdanturfTown_FriendshipRatersHouse_Text_NotUsedToYou: @ 8203249
	.string "It's not very used to you yet.\n"
	.string "It neither loves nor hates you.$"

VerdanturfTown_FriendshipRatersHouse_Text_VeryWary: @ 8203288
	.string "It's very wary.\n"
	.string "It has scary viciousness in its eyes.\l"
	.string "It doesn't like you much at all.$"

VerdanturfTown_FriendshipRatersHouse_Text_DetestsYou: @ 82032DF
	.string "This is a little hard for me to say…\p"
	.string "Your Pokémon simply detests you.\n"
	.string "Doesn't that make you uncomfortable?$"

VerdanturfTown_FriendshipRatersHouse_Text_Pikachu: @ 820334A
	.string "Pikachu: Pika pika!$"

