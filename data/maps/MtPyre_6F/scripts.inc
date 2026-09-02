MtPyre_6F_MapScripts::
	.byte 0

MtPyre_6F_EventScript_Valerie::
	trainerbattle_double TRAINER_VALERIE_1, MtPyre_6F_Text_ValerieIntro, MtPyre_6F_Text_ValerieDefeat, EmeraldChampions_Text_NeedTwoPokemon, MtPyre_6F_EventScript_RegisterValerie
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, MtPyre_6F_EventScript_RematchValerie
	msgbox MtPyre_6F_Text_ValeriePostBattle, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_RegisterValerie::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox MtPyre_6F_Text_ValerieRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_VALERIE_1
	release
	end

MtPyre_6F_EventScript_RematchValerie::
	trainerbattle_rematch_double TRAINER_VALERIE_1, MtPyre_6F_Text_ValerieRematchIntro, MtPyre_6F_Text_ValerieRematchDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MtPyre_6F_Text_ValeriePostRematch, MSGBOX_AUTOCLOSE
	end

MtPyre_6F_EventScript_Cedric::
	trainerbattle_double TRAINER_CEDRIC, MtPyre_6F_Text_CedricIntro, MtPyre_6F_Text_CedricDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox MtPyre_6F_Text_CedricPostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_6F_Text_ValerieIntro:
	.string "When I'm here…\n"
	.string "A curious power flows into me…$"

MtPyre_6F_Text_ValerieDefeat:
	.string "The power is ebbing away…$"

MtPyre_6F_Text_ValeriePostBattle:
	.string "Perhaps the power is from the spirits\n"
	.string "of POKéMON in fitful sleep here…$"

MtPyre_6F_Text_ValerieRegister:
	.string "Fufufu… I lost the match, but…\n"
	.string "I have this little ability…\p"
	.string "Without ever laying my hands on\n"
	.string "your POKéNAV… Hiyah!$"

MtPyre_6F_Text_ValerieRematchIntro:
	.string "Behind you…\n"
	.string "What is it…$"

MtPyre_6F_Text_ValerieRematchDefeat:
	.string "Something faded away…$"

MtPyre_6F_Text_ValeriePostRematch:
	.string "The POKéMON at rest here…\n"
	.string "Sometimes, they play…$"

MtPyre_6F_Text_CedricIntro:
	.string "Have you lost your bearings?\n"
	.string "Have no fear for I am here!$"

MtPyre_6F_Text_CedricDefeat:
	.string "Weren't you lost?$"

MtPyre_6F_Text_CedricPostBattle:
	.string "I had this feeling that a lost TRAINER\n"
	.string "would be panicked and easy to beat.\p"
	.string "It's dirty and I won't try it again…$"

@ BEGIN EMERALD CHAMPIONS LEGENDARY ROOTS
.set EC_SIGN_PECHARUNT_ID, 65

MtPyre_6F_EventScript_Pecharunt::
	lock
	setvar VAR_0x8004, EC_SIGN_PECHARUNT_ID
	special GetSelectedLegendarySignState
	goto_if_eq VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntQuiet
	setvar VAR_0x8004, 64
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, 63
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, 56
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, EC_SIGN_PECHARUNT_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, MtPyre_6F_EventScript_PecharuntDormant
	goto_if_eq VAR_RESULT, 1, MtPyre_6F_EventScript_PecharuntNeedsPartner
	msgbox MtPyre_6F_Text_PecharuntAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_PECHARUNT, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_PECHARUNT_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MtPyre_6F_EventScript_PecharuntCaught
	msgbox MtPyre_6F_Text_PecharuntRemains, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_PecharuntDormant::
	msgbox MtPyre_6F_Text_PecharuntDormant, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_PecharuntNeedsPartner::
	msgbox MtPyre_6F_Text_PecharuntNeedsPartner, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_PecharuntQuiet::
	msgbox MtPyre_6F_Text_PecharuntQuiet, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_PecharuntCaught::
	setflag FLAG_EC_CAUGHT_PECHARUNT
	release
	end

MtPyre_6F_Text_PecharuntDormant:
	.string "A violet SIGN shows three masks: a dog,\n"
	.string "a monkey, and a brilliant bird.\p"
	.string "Catch all three challengers before\n"
	.string "returning to this poisoned shrine.$"

MtPyre_6F_Text_PecharuntNeedsPartner:
	.string "All three masks are lit, but the dog\n"
	.string "mark still reaches toward OKIDOGI.\p"
	.string "Bring its family here to finish the\n"
	.string "poisoned shrine's ritual.$"

MtPyre_6F_Text_PecharuntAwakens:
	.string "The three masks flare! A poisoned mist\n"
	.string "spills from the tombstone--PECHARUNT!$"

MtPyre_6F_Text_PecharuntQuiet:
	.string "The tombstone is still. Three faded\n"
	.string "masks record the trial you completed.$"

MtPyre_6F_Text_PecharuntRemains:
	.string "The mist retreats into the tombstone.\n"
	.string "The three masks remain lit.$"


@ END EMERALD CHAMPIONS LEGENDARY ROOTS
