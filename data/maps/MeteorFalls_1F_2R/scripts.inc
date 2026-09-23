MeteorFalls_1F_2R_MapScripts:: @ 822C4DD
	.byte 0

MeteorFalls_1F_2R_EventScript_Nicolas:: @ 822C4DE
	trainerbattle_double TRAINER_NICOLAS_1, MeteorFalls_1F_2R_Text_NicolasIntro, MeteorFalls_1F_2R_Text_NicolasDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MeteorFalls_1F_2R_Text_NicolasPostBattle, MSGBOX_AUTOCLOSE
	end

Meteor_Falls1F_2R_EventScript_DragonAscentTutor::
	lock
	faceplayer
	msgbox MoveTutor_Text_DragonAscentTeach, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MoveTutor_EventScript_DragonAscentDeclined
	call Meteor_Falls1F_2R_EventScript_RayquazaCheck
	msgbox MoveTutor_Text_DragonAscentWhichMon, MSGBOX_DEFAULT
	setvar VAR_0x8005, MOVE_DRAGON_ASCENT
	call MoveTutor_EventScript_OpenPartyMenu
	compare VAR_RESULT, 0
	goto_if_eq MoveTutor_EventScript_DragonAscentDeclined
	goto MoveTutor_EventScript_DragonAscentTaught
	end

Meteor_Falls1F_2R_EventScript_RayquazaCheck::
	checkPartyHasSpecies SPECIES_RAYQUAZA
	compare VAR_RESULT, FALSE
	goto_if_eq Meteor_Falls1F_2R_EventScript_DragonAscentNoFug
	return

Meteor_Falls1F_2R_EventScript_DragonAscentNoFug::
	msgbox MoveTutor_Text_DragonAscentNoFug, MSGBOX_DEFAULT
	release
	end

MoveTutor_EventScript_DragonAscentDeclined::
	msgbox MoveTutor_Text_DragonAscentDeclined, MSGBOX_DEFAULT
	release
	end

MoveTutor_EventScript_DragonAscentTaught::
	msgbox MoveTutor_Text_DragonAscentTaught, MSGBOX_DEFAULT
	release
	end

MoveTutor_EventScript_OpenPartyMenu::
	special ChooseMonForMoveTutor
	lock
	faceplayer
	return

MeteorFalls_1F_2R_Text_NicolasIntro: @ 822C608
	.string "This is where we Dragon users do our\n"
	.string "training.\p"
	.string "The Champion even visits.\n"
	.string "Now do you see how special it is here?$"

MeteorFalls_1F_2R_Text_NicolasDefeat: @ 822C678
	.string "Urgh!\n"
	.string "I didn't expect you to be so strong!$"

MeteorFalls_1F_2R_Text_NicolasPostBattle: @ 822C6A3
	.string "The road ahead remains long and harsh.\p"
	.string "When will my Pokémon and I become\n"
	.string "the best?$"

MoveTutor_Text_DragonAscentTeach:
	.string "Hmm… Do you want me to teach\n"
	.string "Dragon Ascent to your Pokémon?$"

MoveTutor_Text_DragonAscentWhichMon:
	.string "Which Pokémon do you want me to teach\n"
	.string "the move?$"

MoveTutor_Text_DragonAscentNoFug:
	.string "Hmm… Only Rayquaza can learn\n"
	.string "Dragon Ascent.\l"
	.string "You don't seem to have one.$"

MoveTutor_Text_DragonAscentDeclined:
	.string "Hmm… I see.\n"
	.string "Come back anytime you need my help.$"

MoveTutor_Text_DragonAscentTaught:
	.string "Dragon Ascent is a powerful move that\n"
	.string "only Rayquaza can use.\p"
	.string "They say its power is so strong that\n"
	.string "even a huge meteoroid can be broken\l"
	.string "into pieces with one attack.$"
