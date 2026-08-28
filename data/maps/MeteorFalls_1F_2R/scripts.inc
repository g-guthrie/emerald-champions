MeteorFalls_1F_2R_MapScripts:: @ 822C4DD
	.byte 0

MeteorFalls_1F_2R_EventScript_Nicolas:: @ 822C4DE
	trainerbattle_double TRAINER_NICOLAS_1, MeteorFalls_1F_2R_Text_NicolasIntro, MeteorFalls_1F_2R_Text_NicolasDefeat, MeteorFalls_1F_2R_Text_NicolasNotEnoughMons, MeteorFalls_1F_2R_EventScript_RegisterNicolas
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq MeteorFalls_1F_2R_EventScript_RematchNicolas
	msgbox MeteorFalls_1F_2R_Text_NicolasPostBattle, MSGBOX_DEFAULT
	release
	end

MeteorFalls_1F_2R_EventScript_RegisterNicolas:: @ 822C50A
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox MeteorFalls_1F_2R_Text_NicolasRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_NICOLAS_1
	release
	end

MeteorFalls_1F_2R_EventScript_RematchNicolas:: @ 822C529
	trainerbattle_rematch_double TRAINER_NICOLAS_1, MeteorFalls_1F_2R_Text_NicolasRematchIntro, MeteorFalls_1F_2R_Text_NicolasRematchDefeat, MeteorFalls_1F_2R_Text_NicolasRematchNotEnoughMons
	msgbox MeteorFalls_1F_2R_Text_NicolasPostRematch, MSGBOX_AUTOCLOSE
	end

MeteorFalls_1F_2R_EventScript_John:: @ 822C540
	trainerbattle_double TRAINER_JOHN_AND_JAY_1, MeteorFalls_1F_2R_Text_JohnIntro, MeteorFalls_1F_2R_Text_JohnDefeat, MeteorFalls_1F_2R_Text_JohnNotEnoughMons, MeteorFalls_1F_2R_EventScript_RegisterJohn
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq MeteorFalls_1F_2R_EventScript_RematchJohn
	msgbox MeteorFalls_1F_2R_Text_JohnPostBattle, MSGBOX_DEFAULT
	release
	end

MeteorFalls_1F_2R_EventScript_RegisterJohn:: @ 822C570
	msgbox MeteorFalls_1F_2R_Text_JohnRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_JOHN_AND_JAY_1
	release
	end

MeteorFalls_1F_2R_EventScript_RematchJohn:: @ 822C589
	trainerbattle_rematch_double TRAINER_JOHN_AND_JAY_1, MeteorFalls_1F_2R_Text_JohnRematchIntro, MeteorFalls_1F_2R_Text_JohnRematchDefeat, MeteorFalls_1F_2R_Text_JohnRematchNotEnoughMons
	msgbox MeteorFalls_1F_2R_Text_JohnPostRematch, MSGBOX_AUTOCLOSE
	end

MeteorFalls_1F_2R_EventScript_Jay:: @ 822C5A4
	trainerbattle_double TRAINER_JOHN_AND_JAY_1, MeteorFalls_1F_2R_Text_JayIntro, MeteorFalls_1F_2R_Text_JayDefeat, MeteorFalls_1F_2R_Text_JayNotEnoughMons, MeteorFalls_1F_2R_EventScript_RegisterJay
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq MeteorFalls_1F_2R_EventScript_RematchJay
	msgbox MeteorFalls_1F_2R_Text_JayPostBattle, MSGBOX_DEFAULT
	release
	end

MeteorFalls_1F_2R_EventScript_RegisterJay:: @ 822C5D4
	msgbox MeteorFalls_1F_2R_Text_JohnRegister, MSGBOX_DEFAULT  @ John speaks for both during register
	register_matchcall TRAINER_JOHN_AND_JAY_1
	release
	end

MeteorFalls_1F_2R_EventScript_RematchJay:: @ 822C5ED
	trainerbattle_rematch_double TRAINER_JOHN_AND_JAY_1, MeteorFalls_1F_2R_Text_JayRematchIntro, MeteorFalls_1F_2R_Text_JayRematchDefeat, MeteorFalls_1F_2R_Text_JayRematchNotEnoughMons
	msgbox MeteorFalls_1F_2R_Text_JayPostRematch, MSGBOX_AUTOCLOSE
	end

Meteor_Falls1F_2R_EventScript_DragonAscentTutor::
	lock
	faceplayer
	msgbox MoveTutor_Text_DragonAscentTeach, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq MoveTutor_EventScript_DragonAscentDeclined
	call Meteor_Falls1F_2R_EventScript_RayquazaCheck
	msgbox MoveTutor_Text_DragonAscentWhichMon, MSGBOX_DEFAULT
	setvar VAR_0x8005, TUTOR_MOVE_DRAGON_ASCENT
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
	waitstate
	lock
	faceplayer
	return

MeteorFalls_1F_2R_Text_NicolasIntro: @ 822C608
	.string "Each dragon here trains its own way!\n"
	.string "Roots endure, ancient wings sharpen,\l"
	.string "voltage bites, and venom adapts!\p"
	.string "Show me which lesson needs work!$"

MeteorFalls_1F_2R_Text_NicolasDefeat: @ 822C678
	.string "Urgh!\n"
	.string "I didn't expect you to be so strong!$"

MeteorFalls_1F_2R_Text_NicolasPostBattle: @ 822C6A3
	.string "Harvest restores Exeggutor's Berry.\n"
	.string "Wide Lens steadies Dracozolt.\l"
	.string "Naganadel grows after a knockout.\p"
	.string "Roaring Moon needs one safe turn.$"

MeteorFalls_1F_2R_Text_NicolasRegister: @ 822C6F6
	.string "I want to study your power.\n"
	.string "Let me register you in my PokéNav.$"

MeteorFalls_1F_2R_Text_NicolasRematchIntro: @ 822C73F
	.string "We added two dragon disciplines!\n"
	.string "Dragalge clears boosts with Haze,\l"
	.string "and Regidrago rewards full health.\p"
	.string "Test the lesson we trained today!$"

MeteorFalls_1F_2R_Text_NicolasRematchDefeat: @ 822C7AD
	.string "Urgh!\n"
	.string "I didn't expect you to be so strong!$"

MeteorFalls_1F_2R_Text_NicolasPostRematch: @ 822C7D8
	.string "White Herb restores Draco Meteor.\n"
	.string "Haze can erase our own boosts, too,\l"
	.string "so timing matters.\p"
	.string "Dragon Energy weakens after damage.$"

MeteorFalls_1F_2R_Text_NicolasNotEnoughMons:
	.string "A double lesson needs two ready\n"
	.string "Pokémon. Return when you have them!$"

MeteorFalls_1F_2R_Text_NicolasRematchNotEnoughMons:
	.string "Each rematch is a double lesson.\n"
	.string "Bring two Pokémon ready to battle!$"

MeteorFalls_1F_2R_Text_JohnIntro: @ 822C843
	.string "John: Fifty years taught us this:\n"
	.string "a partner changes your best move.\p"
	.string "Plusle and Minun amplify each other.\n"
	.string "We trust them to open together!$"

MeteorFalls_1F_2R_Text_JohnDefeat: @ 822C89C
	.string "John: Oh, my.\n"
	.string "We've lost, dear wife.$"

MeteorFalls_1F_2R_Text_JohnPostBattle: @ 822C8C1
	.string "John: Plus and Minus raise Sp. Atk.\n"
	.string "Fake Tears and Helping Hand multiply\l"
	.string "the pressure without changing Speed.$"

MeteorFalls_1F_2R_Text_JohnNotEnoughMons: @ 822C92B
	.string "John: A double battle needs two\n"
	.string "ready Pokémon. Return prepared!$"

MeteorFalls_1F_2R_Text_JohnRegister: @ 822C99C
	.string "John: May we register you for\n"
	.string "another battle someday?$"

MeteorFalls_1F_2R_Text_JayIntro: @ 822C9E6
	.string "Jay: Fifty years taught us trust.\n"
	.string "Volbeat grows, Illumise supports,\l"
	.string "and each protects the other's turn.$"

MeteorFalls_1F_2R_Text_JayDefeat: @ 822CA44
	.string "Jay: Oh, dear.\n"
	.string "We've lost, my dear husband.$"

MeteorFalls_1F_2R_Text_JayPostBattle: @ 822CA70
	.string "Jay: Prankster makes support quick.\n"
	.string "Tail Glow powers Volbeat.\n"
	.string "Struggle Bug softens both foes.$"

MeteorFalls_1F_2R_Text_JayNotEnoughMons: @ 822CACD
	.string "Jay: Please bring two ready Pokémon\n"
	.string "if you'd like to battle us.$"

MeteorFalls_1F_2R_Text_JohnRematchIntro: @ 822CB47
	.string "John: Our newest pair became strong\n"
	.string "by trading places.\p"
	.string "Accelgor gained speed; Escavalier\n"
	.string "gained armor. Face them together!$"

MeteorFalls_1F_2R_Text_JohnRematchDefeat: @ 822CBA0
	.string "John: Oh, my.\n"
	.string "We've lost, dear wife.$"

MeteorFalls_1F_2R_Text_JohnPostRematch: @ 822CBC5
	.string "John: Acid Spray opens Sp. Def.\n"
	.string "Overcoat ignores powder, and Occa\l"
	.string "softens Escavalier's Fire weakness.$"

MeteorFalls_1F_2R_Text_JohnRematchNotEnoughMons: @ 822CC27
	.string "John: A double battle needs two\n"
	.string "ready Pokémon. Return prepared!$"

MeteorFalls_1F_2R_Text_JayRematchIntro: @ 822CC98
	.string "Jay: Karrablast and Shelmet traded\n"
	.string "places to become our newest pair.\p"
	.string "Every rematch joins two lessons.\n"
	.string "Can you separate them?$"

MeteorFalls_1F_2R_Text_JayRematchDefeat: @ 822CD08
	.string "Jay: Oh, dear.\n"
	.string "We've lost, my dear husband.$"

MeteorFalls_1F_2R_Text_JayPostRematch: @ 822CD34
	.string "Jay: Accelgor strips Sp. Def, while\n"
	.string "Escavalier strikes physically.\p"
	.string "Our final team keeps three pairs.$"

MeteorFalls_1F_2R_Text_JayRematchNotEnoughMons: @ 822CDAB
	.string "Jay: Please bring two ready Pokémon\n"
	.string "if you'd like to battle us.$"

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
