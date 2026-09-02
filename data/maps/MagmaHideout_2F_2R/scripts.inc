MagmaHideout_2F_2R_MapScripts::
	.byte 0

MagmaHideout_2F_2R_EventScript_Grunt15::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_15, MagmaHideout_2F_2R_Text_Grunt15Intro, MagmaHideout_2F_2R_Text_Grunt15Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_2F_2R_Text_Grunt15PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_2F_2R_EventScript_Grunt6::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_6, MagmaHideout_2F_2R_Text_Grunt6Intro, MagmaHideout_2F_2R_Text_Grunt6Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_2F_2R_Text_Grunt6PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_2F_2R_EventScript_Grunt7::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_7, MagmaHideout_2F_2R_Text_Grunt7Intro, MagmaHideout_2F_2R_Text_Grunt7Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_2F_2R_Text_Grunt7PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_2F_2R_EventScript_Grunt8::
	trainerbattle_double TRAINER_GRUNT_MAGMA_HIDEOUT_8, MagmaHideout_2F_2R_Text_Grunt8Intro, MagmaHideout_2F_2R_Text_Grunt8Defeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MagmaHideout_2F_2R_Text_Grunt8PostBattle, MSGBOX_AUTOCLOSE
	end

MagmaHideout_2F_2R_Text_Grunt15Intro:
	.string "Nothing personal. I'm following orders,\n"
	.string "and my orders are: nobody passes.$"

MagmaHideout_2F_2R_Text_Grunt15Defeat:
	.string "Orders, meet reality…$"

MagmaHideout_2F_2R_Text_Grunt15PostBattle:
	.string "We dug up something huge down here, and\n"
	.string "MAXIE has the BLUE ORB now.\p"
	.string "I don't know what happens next. I don't\n"
	.string "think he does either.$"

MagmaHideout_2F_2R_Text_Grunt6Intro:
	.string "I used to hate this heat. Then HEATRAN\n"
	.string "taught me to wear it like armor.$"

MagmaHideout_2F_2R_Text_Grunt6Defeat:
	.string "Even armor buckles under pressure…$"

MagmaHideout_2F_2R_Text_Grunt6PostBattle:
	.string "Maybe TEAM MAGMA suits me after all.\n"
	.string "I just need a stronger furnace.$"

MagmaHideout_2F_2R_Text_Grunt7Intro:
	.string "You feel those tremors? People say it's\n"
	.string "the volcano. Some of us know better.\p"
	.string "Whoops! Never mind. Fight me!$"

MagmaHideout_2F_2R_Text_Grunt7Defeat:
	.string "You hit like a volcano yourself…$"

MagmaHideout_2F_2R_Text_Grunt7PostBattle:
	.string "You won this one. Doesn't matter.\n"
	.string "MAGMA's almost there. I hope.$"

MagmaHideout_2F_2R_Text_Grunt8Intro:
	.string "Wait. One of ours dropped his MAGMA\n"
	.string "EMBLEM outside. Was it you who found it?\p"
	.string "Doesn't matter. You're still not\n"
	.string "getting past FLYGON.$"

MagmaHideout_2F_2R_Text_Grunt8Defeat:
	.string "I'm having trouble believing this…$"

MagmaHideout_2F_2R_Text_Grunt8PostBattle:
	.string "Between you and me? I've got a feeling\n"
	.string "this plan ends badly for everyone.$"
