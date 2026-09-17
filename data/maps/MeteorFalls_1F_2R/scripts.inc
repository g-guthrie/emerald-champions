MeteorFalls_1F_2R_MapScripts::
	.byte 0

MeteorFalls_1F_2R_EventScript_MoveSpecialist::
	msgbox MeteorFalls_1F_2R_Text_MoveSpecialist, MSGBOX_NPC
	end

MeteorFalls_1F_2R_Text_MoveSpecialist:
	.string "I once guarded the secret of\n"
	.string "DRAGON ASCENT here.\p"
	.string "The POKéMON CENTER specialist now\n"
	.string "teaches every legal move.$"

MeteorFalls_1F_2R_EventScript_Nicolas::
	trainerbattle_double TRAINER_NICOLAS_1, MeteorFalls_1F_2R_Text_NicolasIntro, MeteorFalls_1F_2R_Text_NicolasDefeat, EmeraldChampions_Text_NeedTwoPokemon, MeteorFalls_1F_2R_EventScript_RegisterNicolas
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, MeteorFalls_1F_2R_EventScript_RematchNicolas
	setvar VAR_0x8004, LEGENDARY_SIGN_REGIDRAGO
	special GetSelectedLegendarySignState
	goto_if_eq VAR_RESULT, 2, MeteorFalls_1F_2R_EventScript_NicolasRegidragoCaught
	msgbox MeteorFalls_1F_2R_Text_NicolasPostBattle, MSGBOX_DEFAULT
	release
	end

MeteorFalls_1F_2R_EventScript_NicolasRegidragoCaught::
	msgbox MeteorFalls_1F_2R_Text_NicolasRegidragoCaught, MSGBOX_DEFAULT
	release
	end

MeteorFalls_1F_2R_EventScript_RegisterNicolas::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox MeteorFalls_1F_2R_Text_NicolasRegister, MSGBOX_DEFAULT
	release
	end

MeteorFalls_1F_2R_EventScript_RematchNicolas::
	trainerbattle_rematch_double TRAINER_NICOLAS_1, MeteorFalls_1F_2R_Text_NicolasRematchIntro, MeteorFalls_1F_2R_Text_NicolasRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MeteorFalls_1F_2R_Text_NicolasPostRematch, MSGBOX_AUTOCLOSE
	end

MeteorFalls_1F_2R_EventScript_John::
	msgbox MeteorFalls_1F_2R_Text_JohnPostBattle, MSGBOX_NPC
	end

MeteorFalls_1F_2R_EventScript_Jay::
	msgbox MeteorFalls_1F_2R_Text_JayPostBattle, MSGBOX_NPC
	end

MeteorFalls_1F_2R_Text_NicolasIntro:
	.string "This is where we DRAGON users do our\n"
	.string "training.\p"
	.string "The CHAMPION even visits.\n"
	.string "Now do you see how special it is here?$"

MeteorFalls_1F_2R_Text_NicolasDefeat:
	.string "Urgh!\n"
	.string "I didn't expect you to be so strong!$"

MeteorFalls_1F_2R_Text_NicolasPostBattle:
	.string "With the FEATHER BADGE and\n"
	.string "REGIROCK recorded as caught,\l"
	.string "REGIDRAGO can appear on this\l"
	.string "floor.\p"
	.string "It has a 1% wild encounter\n"
	.string "chance. SWEET SCENT raises that\l"
	.string "to 25%.\p"
	.string "FALLARBOR's CENTER guide knows\n"
	.string "the discoveries in the other\l"
	.string "METEOR FALLS rooms.$"
MeteorFalls_1F_2R_Text_NicolasRegidragoCaught:
	.string "You've caught REGIDRAGO!\n"
	.string "What a partner to train beside.\p"
	.string "FALLARBOR's CENTER guide knows\n"
	.string "the legends in the other rooms.$"
MeteorFalls_1F_2R_Text_NicolasRegister:
	.string "I want to know more about your\n"
	.string "power. I train here every day.$"

MeteorFalls_1F_2R_Text_NicolasRematchIntro:
	.string "Since we met, we have trained hard\n"
	.string "with our sights on number one.\p"
	.string "Help us see how much stronger we've\n"
	.string "become!$"

MeteorFalls_1F_2R_Text_NicolasRematchDefeat:
	.string "Urgh!\n"
	.string "I didn't expect you to be so strong!$"

MeteorFalls_1F_2R_Text_NicolasPostRematch:
	.string "You've obviously kept up your\n"
	.string "POKéMON training.\p"
	.string "So long as you remain strong, I, too,\n"
	.string "can become stronger!$"

MeteorFalls_1F_2R_Text_JohnPostBattle:
	.string "JOHN: We've been married for\n"
	.string "fifty years.\p"
	.string "Come to think of it, I've yet to beat\n"
	.string "my dear wife in a battle.$"

MeteorFalls_1F_2R_Text_JayPostBattle:
	.string "JAY: Fifty years of marriage…\p"
	.string "If we ever argued, we always settled\n"
	.string "it with a POKéMON battle…$"
