MossdeepCity_House1_MapScripts::
	.byte 0

MossdeepCity_House1_EventScript_BlackBelt::
	lock
	faceplayer
	bufferleadmonspeciesname STR_VAR_1
	msgbox MossdeepCity_House1_Text_HmmYourPokemon, MSGBOX_DEFAULT
	specialvar VAR_RESULT, GetPokeblockNameByMonNature
	goto_if_eq VAR_RESULT, 0, MossdeepCity_House1_EventScript_NeutralNature
	msgbox MossdeepCity_House1_Text_ItLikesXPokeblocks, MSGBOX_DEFAULT
	release
	end

MossdeepCity_House1_EventScript_NeutralNature::
	msgbox MossdeepCity_House1_Text_DoesntLikeOrDislikePokeblocks, MSGBOX_DEFAULT
	release
	end

MossdeepCity_House1_EventScript_Woman::
	msgbox MossdeepCity_House1_Text_HusbandCanTellPokeblockMonLikes, MSGBOX_NPC
	end

MossdeepCity_House1_Text_HmmYourPokemon:
	.string "Hmm!\n"
	.string "Your {STR_VAR_1}…$"

MossdeepCity_House1_Text_ItLikesXPokeblocks:
	.string "It likes {STR_VAR_1}S,\n"
	.string "doesn't it?\p"
	.string "No, I'm positive of it! It definitely\n"
	.string "likes {STR_VAR_1}S!$"

MossdeepCity_House1_Text_DoesntLikeOrDislikePokeblocks:
	.string "It doesn't appear to like or dislike\n"
	.string "any {POKEBLOCK}S.$"

MossdeepCity_House1_Text_HusbandCanTellPokeblockMonLikes:
	.string "My husband can tell what kind of\n"
	.string "{POKEBLOCK}S a POKéMON likes at a glance.\p"
	.string "CYNTHIA is our guest while she studies\n"
	.string "MOSSDEEP's myths and meteor records.$"

MossdeepCity_House1_EventScript_CynthiaEC::
	lock
	faceplayer
	goto_if_unset FLAG_SYS_GAME_CLEAR, MossdeepCity_House1_EventScript_CynthiaNotReady
	goto_if_defeated TRAINER_CYNTHIA_1, MossdeepCity_House1_EventScript_CynthiaReward
	msgbox MossdeepCity_House1_Text_CynthiaChallenge, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MossdeepCity_House1_EventScript_CynthiaDecline
	trainerbattle_no_intro_double TRAINER_CYNTHIA_1, MossdeepCity_House1_Text_CynthiaDefeat, EmeraldChampions_Text_NeedTwoPokemon

MossdeepCity_House1_EventScript_CynthiaReward::
	checkitem ITEM_SHINY_CHARM, 1
	goto_if_eq VAR_RESULT, TRUE, MossdeepCity_House1_EventScript_CynthiaAfter
	giveitem ITEM_SHINY_CHARM
	goto_if_eq VAR_RESULT, FALSE, MossdeepCity_House1_EventScript_CynthiaBagFull

MossdeepCity_House1_EventScript_CynthiaAfter::
	msgbox MossdeepCity_House1_Text_CynthiaAfter, MSGBOX_DEFAULT
	release
	end

MossdeepCity_House1_EventScript_CynthiaNotReady::
	msgbox MossdeepCity_House1_Text_CynthiaNotReady, MSGBOX_DEFAULT
	release
	end

MossdeepCity_House1_EventScript_CynthiaDecline::
	msgbox MossdeepCity_House1_Text_CynthiaDecline, MSGBOX_DEFAULT
	release
	end

MossdeepCity_House1_EventScript_CynthiaBagFull::
	msgbox MossdeepCity_House1_Text_CynthiaBagFull, MSGBOX_DEFAULT
	release
	end

MossdeepCity_House1_Text_CynthiaChallenge:
	.string "I'm CYNTHIA. I study myths, but battles\n"
	.string "reveal how a Trainer reasons under fire.\p"
	.string "You are HOENN's Champion. Will you face\n"
	.string "a Champion from another region?$"

MossdeepCity_House1_Text_CynthiaDefeat:
	.string "You removed the shelter, denied sleep,\n"
	.string "and still saved an answer for GARCHOMP.$"

MossdeepCity_House1_Text_CynthiaAfter:
	.string "Take this SHINY CHARM. Let your next\n"
	.string "journey reveal something extraordinary.$"

MossdeepCity_House1_Text_CynthiaNotReady:
	.string "These kind hosts gave me a quiet place\n"
	.string "to study HOENN's meteor records.\p"
	.string "Your journey continues. Return after\n"
	.string "the HOENN LEAGUE knows your name.$"

MossdeepCity_House1_Text_CynthiaDecline:
	.string "A good challenge improves by waiting.$"

MossdeepCity_House1_Text_CynthiaBagFull:
	.string "Make room for the SHINY CHARM.\n"
	.string "I will keep it safe.$"
