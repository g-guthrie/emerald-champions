MtPyre_6F_MapScripts:: @ 8231D3A
	.byte 0

MtPyre_6F_EventScript_Valerie:: @ 8231D3B
	@ This retired encounter has no campaign battle.
	specialvar VAR_RESULT, ShouldTryRematchBattle
	compare VAR_RESULT, TRUE
	goto_if_eq MtPyre_6F_EventScript_RematchValerie
	msgbox MtPyre_6F_Text_ValeriePostBattle, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_RegisterValerie:: @ 8231D67
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox MtPyre_6F_Text_ValerieRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_VALERIE_1
	release
	end

MtPyre_6F_EventScript_RematchValerie:: @ 8231D86
	@ This retired encounter has no campaign battle.
	msgbox MtPyre_6F_Text_ValeriePostRematch, MSGBOX_AUTOCLOSE
	end

MtPyre_6F_EventScript_Cedric:: @ 8231D9D
	@ This retired encounter has no campaign battle.
	msgbox MtPyre_6F_Text_CedricPostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_6F_Text_ValerieIntro: @ 8231DB4
	.string "When I'm here…\n"
	.string "A curious power flows into me…$"

MtPyre_6F_Text_ValerieDefeat: @ 8231DE2
	.string "The power is ebbing away…$"

MtPyre_6F_Text_ValeriePostBattle: @ 8231DFC
	.string "Perhaps the power is from the spirits\n"
	.string "of Pokémon in fitful sleep here…$"

MtPyre_6F_Text_ValerieRegister: @ 8231E43
	.string "Fufufu… I lost the match, but…\n"
	.string "I have this little ability…\p"
	.string "Without ever laying my hands on\n"
	.string "your PokéNav… Hiyah!$"

MtPyre_6F_Text_ValerieRematchIntro: @ 8231EB3
	.string "Behind you…\n"
	.string "What is it…$"

MtPyre_6F_Text_ValerieRematchDefeat: @ 8231ECB
	.string "Something faded away…$"

MtPyre_6F_Text_ValeriePostRematch: @ 8231EE1
	.string "The Pokémon at rest here…\n"
	.string "Sometimes, they play…$"

MtPyre_6F_Text_CedricIntro: @ 8231F11
	.string "Have you lost your bearings?\n"
	.string "Have no fear for I am here!$"

MtPyre_6F_Text_CedricDefeat: @ 8231F4A
	.string "Weren't you lost?$"

MtPyre_6F_Text_CedricPostBattle: @ 8231F5C
	.string "I had this feeling that a lost Trainer\n"
	.string "would be panicked and easy to beat.\p"
	.string "It's dirty and I won't try it again…$"


MtPyre_6F_EventScript_Pecharunt::
	lockall
	setvar VAR_0x8004, LEGENDARY_SIGN_PECHARUNT
	special GetSelectedLegendarySignState
	goto_if_eq VAR_RESULT, 3, Common_EventScript_LegendaryRestingAtShrine
	goto_if_eq VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntQuiet
	setvar VAR_0x8004, LEGENDARY_SIGN_OKIDOGI
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, LEGENDARY_SIGN_MUNKIDORI
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, LEGENDARY_SIGN_FEZANDIPITI
	special GetSelectedLegendarySignState
	goto_if_ne VAR_RESULT, 2, MtPyre_6F_EventScript_PecharuntDormant
	setvar VAR_0x8004, LEGENDARY_SIGN_PECHARUNT
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 5, Common_EventScript_LegendaryRestingAtShrine
	goto_if_eq VAR_RESULT, 0, MtPyre_6F_EventScript_PecharuntDormant
	goto_if_eq VAR_RESULT, 1, MtPyre_6F_EventScript_PecharuntDormant
	special CreateSelectedLegendarySignEncounter
	goto_if_eq VAR_RESULT, FALSE, MtPyre_6F_EventScript_PecharuntDormant
	msgbox MtPyre_6F_Text_PecharuntAwakens, MSGBOX_DEFAULT
	playmoncry SPECIES_PECHARUNT, CRY_MODE_ENCOUNTER
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special FinishLegendaryLandmarkEncounter
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_LegendaryRestingAtShrine
	releaseall
	end

MtPyre_6F_EventScript_PecharuntDormant::
	msgbox MtPyre_6F_Text_PecharuntDormant, MSGBOX_DEFAULT
	releaseall
	end

MtPyre_6F_EventScript_PecharuntQuiet::
	msgbox MtPyre_6F_Text_PecharuntQuiet, MSGBOX_DEFAULT
	releaseall
	end

MtPyre_6F_Text_PecharuntDormant:
	.string "Three masks adorn the tombstone.\p"
	.string "Earn the Mind Badge and catch\n"
	.string "Okidogi, Munkidori and Fezandipiti\l"
	.string "to awaken Pecharunt here.$"

MtPyre_6F_Text_PecharuntAwakens:
	.string "The three masks flare!\n"
	.string "Pecharunt emerges from the mist!$"

MtPyre_6F_Text_PecharuntQuiet:
	.string "The tombstone is still.\n"
	.string "The three masks have faded.$"
