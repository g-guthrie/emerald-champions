MossdeepCity_House1_MapScripts::
	.byte 0

MossdeepCity_House1_EventScript_BlackBelt::
	lock
	faceplayer
	bufferleadmonspeciesname STR_VAR_1
	msgbox MossdeepCity_House1_Text_HmmYourPokemon, MSGBOX_DEFAULT
	msgbox MossdeepCity_House1_Text_ItLikesXPokeblocks, MSGBOX_DEFAULT
	release
	end

MossdeepCity_House1_EventScript_Woman::
	msgbox MossdeepCity_House1_Text_HusbandCanTellPokeblockMonLikes, MSGBOX_NPC
	end

MossdeepCity_House1_Text_HmmYourPokemon:
	.string "Hmm!\n"
	.string "Your {STR_VAR_1}…$"

MossdeepCity_House1_Text_ItLikesXPokeblocks:
	.string "A nature like that wants a BERRY\n"
	.string "with a bite to it.\p"
	.string "No, I'm positive of it! Hand it a\n"
	.string "bitter one and watch.$"

MossdeepCity_House1_Text_HusbandCanTellPokeblockMonLikes:
	.string "My husband can tell which BERRY a\n"
	.string "POKéMON likes at a glance.\p"
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
	.string "What a battle! You and your POKéMON\n"
	.string "have given me something new to study.$"

MossdeepCity_House1_Text_CynthiaAfter:
	.string "The SHINY CHARM is yours.\n"
	.string "May your next journey reveal\l"
	.string "something extraordinary.$"

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
