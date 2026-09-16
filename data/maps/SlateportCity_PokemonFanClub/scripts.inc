SlateportCity_PokemonFanClub_MapScripts::
	.byte 0

SlateportCity_PokemonFanClub_EventScript_FurfrouStylist::
	lock
	faceplayer
	msgbox SlateportCity_PokemonFanClub_Text_FurfrouOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, SlateportCity_PokemonFanClub_EventScript_FurfrouExit
	msgbox SlateportCity_PokemonFanClub_Text_FurfrouChoose, MSGBOX_DEFAULT
	special ChoosePartyMon
	goto_if_ge VAR_0x8004, PARTY_SIZE, SlateportCity_PokemonFanClub_EventScript_FurfrouExit
	specialvar VAR_RESULT, IsSelectedMonFurfrou
	goto_if_eq VAR_RESULT, FALSE, SlateportCity_PokemonFanClub_EventScript_FurfrouInvalid
	copyvar VAR_0x8007, VAR_0x8004
	setvar VAR_0x8004, SCROLL_MULTI_FURFROU_TRIMS
	setvar VAR_0x8005, 0
	special ShowScrollableMultichoice
	switch VAR_RESULT
	case 0, SlateportCity_PokemonFanClub_EventScript_FurfrouHeart
	case 1, SlateportCity_PokemonFanClub_EventScript_FurfrouStar
	case 2, SlateportCity_PokemonFanClub_EventScript_FurfrouDiamond
	case 3, SlateportCity_PokemonFanClub_EventScript_FurfrouDebutante
	case 4, SlateportCity_PokemonFanClub_EventScript_FurfrouMatron
	case 5, SlateportCity_PokemonFanClub_EventScript_FurfrouDandy
	case 6, SlateportCity_PokemonFanClub_EventScript_FurfrouLaReine
	case 7, SlateportCity_PokemonFanClub_EventScript_FurfrouKabuki
	case 8, SlateportCity_PokemonFanClub_EventScript_FurfrouPharaoh
	case 9, SlateportCity_PokemonFanClub_EventScript_FurfrouNatural
	case 10, SlateportCity_PokemonFanClub_EventScript_FurfrouExit
	case MULTI_B_PRESSED, SlateportCity_PokemonFanClub_EventScript_FurfrouExit
	goto SlateportCity_PokemonFanClub_EventScript_FurfrouExit
	end

SlateportCity_PokemonFanClub_EventScript_FurfrouHeart::
	setvar VAR_0x8005, SPECIES_FURFROU_HEART
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim
SlateportCity_PokemonFanClub_EventScript_FurfrouStar::
	setvar VAR_0x8005, SPECIES_FURFROU_STAR
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim
SlateportCity_PokemonFanClub_EventScript_FurfrouDiamond::
	setvar VAR_0x8005, SPECIES_FURFROU_DIAMOND
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim
SlateportCity_PokemonFanClub_EventScript_FurfrouDebutante::
	setvar VAR_0x8005, SPECIES_FURFROU_DEBUTANTE
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim
SlateportCity_PokemonFanClub_EventScript_FurfrouMatron::
	setvar VAR_0x8005, SPECIES_FURFROU_MATRON
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim
SlateportCity_PokemonFanClub_EventScript_FurfrouDandy::
	setvar VAR_0x8005, SPECIES_FURFROU_DANDY
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim
SlateportCity_PokemonFanClub_EventScript_FurfrouLaReine::
	setvar VAR_0x8005, SPECIES_FURFROU_LA_REINE
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim
SlateportCity_PokemonFanClub_EventScript_FurfrouKabuki::
	setvar VAR_0x8005, SPECIES_FURFROU_KABUKI
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim
SlateportCity_PokemonFanClub_EventScript_FurfrouPharaoh::
	setvar VAR_0x8005, SPECIES_FURFROU_PHARAOH
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim
SlateportCity_PokemonFanClub_EventScript_FurfrouNatural::
	setvar VAR_0x8005, SPECIES_FURFROU
	goto SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim

SlateportCity_PokemonFanClub_EventScript_ApplyFurfrouTrim::
	copyvar VAR_0x8004, VAR_0x8007
	bufferpartymonnick STR_VAR_1, VAR_0x8004
	fadescreen FADE_TO_BLACK
	special ChangeSelectedMonSpecies
	fadescreen FADE_FROM_BLACK
	msgbox SlateportCity_PokemonFanClub_Text_FurfrouComplete, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_FurfrouInvalid::
	msgbox SlateportCity_PokemonFanClub_Text_FurfrouInvalid, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_FurfrouExit::
	msgbox SlateportCity_PokemonFanClub_Text_FurfrouExit, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_Text_FurfrouOffer:
	.string "A fine FURFROU deserves a fine\n"
	.string "trim! Would yours like a new cut?$"

SlateportCity_PokemonFanClub_Text_FurfrouChoose:
	.string "Choose the FURFROU you want styled.$"

SlateportCity_PokemonFanClub_Text_FurfrouComplete:
	.string "All done! {STR_VAR_1} has a lovely\n"
	.string "new look.$"

SlateportCity_PokemonFanClub_Text_FurfrouInvalid:
	.string "I can only style FURFROU, including\n"
	.string "one that already has a trim.$"

SlateportCity_PokemonFanClub_Text_FurfrouExit:
	.string "Come back whenever FURFROU wants a\n"
	.string "different look.$"

SlateportCity_PokemonFanClub_EventScript_Chairman::
	lock
	faceplayer
	call_if_unset FLAG_MET_SLATEPORT_FANCLUB_CHAIRMAN, SlateportCity_PokemonFanClub_EventScript_MeetChairman
	msgbox SlateportCity_PokemonFanClub_Text_AllowMeToExamineYourPokemon, MSGBOX_DEFAULT
	bufferleadmonspeciesname STR_VAR_1
	specialvar VAR_RESULT, GetLeadMonFriendshipScore
	msgbox SlateportCity_PokemonFanClub_Text_HmHmISee, MSGBOX_DEFAULT
	goto_if_ge VAR_RESULT, FRIENDSHIP_200_TO_254, SlateportCity_PokemonFanClub_EventScript_ChairmanDevoted
	goto_if_ge VAR_RESULT, FRIENDSHIP_150_TO_199, SlateportCity_PokemonFanClub_EventScript_ChairmanFond
	msgbox SlateportCity_PokemonFanClub_Text_ChairmanStillWary, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_ChairmanFond::
	msgbox SlateportCity_PokemonFanClub_Text_ChairmanFond, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_ChairmanDevoted::
	msgbox SlateportCity_PokemonFanClub_Text_ChairmanDevoted, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_MeetChairman::
	msgbox SlateportCity_PokemonFanClub_Text_MeetChairman, MSGBOX_DEFAULT
	setflag FLAG_MET_SLATEPORT_FANCLUB_CHAIRMAN
	return

SlateportCity_PokemonFanClub_EventScript_SootheBellWoman::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_SOOTHE_BELL, SlateportCity_PokemonFanClub_EventScript_ReceivedSootheBell
	msgbox SlateportCity_PokemonFanClub_Text_ShowMePokemonThatLoveYou, MSGBOX_DEFAULT
	specialvar VAR_RESULT, GetLeadMonFriendshipScore
	goto_if_ge VAR_RESULT, FRIENDSHIP_150_TO_199, SlateportCity_PokemonFanClub_EventScript_GiveSootheBell
	release
	end

SlateportCity_PokemonFanClub_EventScript_GiveSootheBell::
	playse SE_PIN
	applymovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement VAR_LAST_TALKED, Common_Movement_Delay48
	waitmovement 0
	msgbox SlateportCity_PokemonFanClub_Text_PokemonAdoresYou, MSGBOX_DEFAULT
	giveitem ITEM_SOOTHE_BELL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_SOOTHE_BELL
	release
	end

SlateportCity_PokemonFanClub_EventScript_ReceivedSootheBell::
	msgbox SlateportCity_PokemonFanClub_Text_TreatPokemonWithLove, MSGBOX_DEFAULT
	release
	end

SlateportCity_PokemonFanClub_EventScript_Man::
	msgbox SlateportCity_PokemonFanClub_Text_PokemonDontLikeFainting, MSGBOX_NPC
	end

SlateportCity_PokemonFanClub_EventScript_Twin::
	msgbox SlateportCity_PokemonFanClub_Text_MonEnjoyedLuxuryBall, MSGBOX_NPC
	end

SlateportCity_PokemonFanClub_EventScript_Skitty::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_SKITTY, CRY_MODE_NORMAL
	msgbox SlateportCity_PokemonFanClub_Text_Skitty, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SlateportCity_PokemonFanClub_EventScript_Zigzagoon::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZIGZAGOON, CRY_MODE_NORMAL
	msgbox SlateportCity_PokemonFanClub_Text_Zigzagoon, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SlateportCity_PokemonFanClub_EventScript_Azumarill::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_AZUMARILL, CRY_MODE_NORMAL
	msgbox SlateportCity_PokemonFanClub_Text_Azumarill, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SlateportCity_PokemonFanClub_Text_MeetChairman:
	.string "Er-hem! I am the CHAIRMAN of the\n"
	.string "POKéMON FAN CLUB!\p"
	.string "No one can best me when it comes\n"
	.string "to raising POKéMON. No one!\p"
	.string "We gather here to show off our\n"
	.string "POKéMON, and have others show\l"
	.string "us theirs.$"

SlateportCity_PokemonFanClub_Text_AllowMeToExamineYourPokemon:
	.string "Please! Allow me to see how your\n"
	.string "lead POKéMON feels about you.\p"
	.string "The kindly eyes of a TRAINER that\n"
	.string "nurtured a POKéMON… the very\l"
	.string "thought fills my heart with joy.$"

SlateportCity_PokemonFanClub_Text_HmHmISee:
	.string "Hm, hm…\n"
	.string "I see…$"

SlateportCity_PokemonFanClub_Text_ShowMePokemonThatLoveYou:
	.string "I love seeing POKéMON that love\n"
	.string "their TRAINERS.\p"
	.string "POKéMON are very sensitive to\n"
	.string "the feelings of their TRAINERS.\p"
	.string "If you treat your POKéMON with love\n"
	.string "and care, they'll love you back.\p"
	.string "When your POKéMON grow to love you,\n"
	.string "please come show me.$"

SlateportCity_PokemonFanClub_Text_PokemonAdoresYou:
	.string "Your POKéMON really adores you.\p"
	.string "For you, a most compassionate\n"
	.string "TRAINER, a gift from the FAN CLUB!$"

SlateportCity_PokemonFanClub_Text_TreatPokemonWithLove:
	.string "POKéMON are very sensitive to\n"
	.string "the feelings of their TRAINERS.\p"
	.string "If you treat your POKéMON with love\n"
	.string "and care, they'll love you back.$"

SlateportCity_PokemonFanClub_Text_PokemonDontLikeFainting:
	.string "If you keep letting a POKéMON faint\n"
	.string "in battle, it'll come to resent it.\p"
	.string "Soon, it will become less trusting\n"
	.string "of the TRAINER.\p"
	.string "In other words, it certainly won't\n"
	.string "like you very much.$"

SlateportCity_PokemonFanClub_Text_MonEnjoyedLuxuryBall:
	.string "Do POKéMON remember how they were\n"
	.string "caught?\p"
	.string "Mine became friendly so quickly after\n"
	.string "I caught it in a LUXURY BALL.$"

SlateportCity_PokemonFanClub_Text_Skitty:
	.string "SKITTY: Fffnyaaaah…$"

SlateportCity_PokemonFanClub_Text_Zigzagoon:
	.string "ZIGZAGOON: Kyuuu…$"

SlateportCity_PokemonFanClub_Text_Azumarill:
	.string "AZUMARILL: Marimari?$"

SlateportCity_PokemonFanClub_Text_ChairmanStillWary:
	.string "Hmmm… {STR_VAR_1} is still taking\n"
	.string "your measure.\p"
	.string "Walk with it. Win with it. Keep it\n"
	.string "out of the red. It will come round.$"

SlateportCity_PokemonFanClub_Text_ChairmanFond:
	.string "Ah! {STR_VAR_1} is fond of you.\p"
	.string "A POKéMON that likes its TRAINER\n"
	.string "fights a little harder. Mark my words!$"

SlateportCity_PokemonFanClub_Text_ChairmanDevoted:
	.string "Magnificent! {STR_VAR_1} adores you.\p"
	.string "I have seen nothing like it all\n"
	.string "season. Do not let that slip!$"
