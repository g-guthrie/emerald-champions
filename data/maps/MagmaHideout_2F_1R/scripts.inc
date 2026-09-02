MagmaHideout_2F_1R_MapScripts::
	.byte 0

MagmaHideout_2F_1R_EventScript_Grunt14::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_14, MagmaHideout_2F_1R_Text_Grunt14Intro, MagmaHideout_2F_1R_Text_Grunt14Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_2F_1R_Text_Grunt14PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_2F_1R_EventScript_Grunt3::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_3, MagmaHideout_2F_1R_Text_Grunt3Intro, MagmaHideout_2F_1R_Text_Grunt3Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_2F_1R_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_2F_1R_EventScript_Grunt4::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_4, MagmaHideout_2F_1R_Text_Grunt4Intro, MagmaHideout_2F_1R_Text_Grunt4Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_2F_1R_Text_Grunt4PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_2F_1R_EventScript_Grunt5::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_5, MagmaHideout_2F_1R_Text_Grunt5Intro, MagmaHideout_2F_1R_Text_Grunt5Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_2F_1R_Text_Grunt5PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_2F_1R_Text_Grunt14Intro:
	.string "Hey! No hood, no business here.\p"
	.string "I joined MAGMA because my town floods\n"
	.string "every spring. Solid ground, that's all\l"
	.string "I ever asked for. Now beat it!$"

MagmaHideout_2F_1R_Text_Grunt14Defeat:
	.string "Aiyiyi… My hood and my pride, both.$"

MagmaHideout_2F_1R_Text_Grunt14PostBattle:
	.string "Go on, then. Just… if you see MAXIE,\n"
	.string "ask him if the ground is worth this.$"

MagmaHideout_2F_1R_Text_Grunt3Intro:
	.string "Hold it! You think I'll just let you\n"
	.string "stroll past a CLAYDOL and a GARBODOR?$"

MagmaHideout_2F_1R_Text_Grunt3Defeat:
	.string "Ooh wow! Okay. I concede.$"

MagmaHideout_2F_1R_Text_Grunt3PostBattle:
	.string "Honestly, I only signed up for the\n"
	.string "dental plan. Don't tell the ADMINS.$"

MagmaHideout_2F_1R_Text_Grunt4Intro:
	.string "An intruder! SCRAFTY, it's a real fight\n"
	.string "for once, not another drill!$"

MagmaHideout_2F_1R_Text_Grunt4Defeat:
	.string "Graaah! Even the drills didn't go\n"
	.string "like that!$"

MagmaHideout_2F_1R_Text_Grunt4PostBattle:
	.string "I've already lost. Do I have to keep\n"
	.string "running laps around this rock?$"

MagmaHideout_2F_1R_Text_Grunt5Intro:
	.string "Oh, oh! Nobody told me anyone would\n"
	.string "actually get this far!$"

MagmaHideout_2F_1R_Text_Grunt5Defeat:
	.string "Mutter… mutter…$"

MagmaHideout_2F_1R_Text_Grunt5PostBattle:
	.string "I'm not much of a battler. I'm in it\n"
	.string "for the volcano. It's really pretty.$"

