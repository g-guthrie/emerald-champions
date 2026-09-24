.set LOCALID_LEAF, 11

AlteringCave_B1F_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Common_EventScript_RemoveOverworldAfterBattle
	.byte 0

AlteringCave_B1F_EventScript_Mewtwo::
	lock
	faceplayer
	goto_if_unset FLAG_BADGE07_GET, Common_EventScript_LegendaryNotReady
	waitse
	playmoncry SPECIES_MEWTWO, 2
	delay 40
	waitmoncry
	special GetLevelCapForScriptedGift
	setwildbattle SPECIES_MEWTWO, VAR_0x800A, ITEM_PSYCHIC_GEM
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
AlteringCave_B1F_EventScript_CheckMewtwoResult::
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_0x8004, SPECIES_MEWTWO
	specialvar VAR_RESULT, GetBattleOutcome
	@ A knockout loses it for good; capture resolves it; anything else retreats.
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, Common_EventScript_LegendaryGone
	goto Common_EventScript_EndVisibleLegendaryEncounter

AlteringCave_B1F_EventScript_Leaf::
	lockall
	goto_if_defeated TRAINER_LEAF_ALTERING_CAVE, AlteringCave_B1F_EventScript_LeafReward
	playse SE_PIN
	applymovement LOCALID_LEAF, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_LEAF, Common_Movement_WalkInPlaceFastestDown
	waitmovement 0
	delay 40
	msgbox AlteringCave_Text_LeafPreBattle, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_LEAF, AlteringCave_Movement_Leaf
	applymovement OBJ_EVENT_ID_PLAYER, AlteringCave_Movement_Player
	waitmovement 0
	trainerbattle_no_intro_double TRAINER_LEAF_ALTERING_CAVE, AlteringCave_Text_LeafDefeated
AlteringCave_B1F_EventScript_LeafReward::
	applymovement LOCALID_LEAF, Common_Movement_FacePlayer
	waitmovement 0
	msgbox AlteringCave_Text_LeafGift, MSGBOX_DEFAULT
	giveitem ITEM_BOTTLE_CAP, 30
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	setvar VAR_LEAF_STATE, 1
	msgbox AlteringCave_Text_LeafExit, MSGBOX_DEFAULT
	closemessage
	applymovement  OBJ_EVENT_ID_PLAYER, AlteringCave_Movement_GiveWay
	applymovement  LOCALID_LEAF, AlteringCave_Movement_LeafLeaves
	waitmovement 0
	removeobject LOCALID_LEAF
	releaseall
	end

AlteringCave_Text_LeafPreBattle:
	.string "???: Are you here looking for\n"
	.string "something?\p"
	.string "Maybe a certain Legendary Pokémon?\p"
	.string "Well, you're in the right place.\n"
	.string "Mewtwo's just ahead!\p"
	.string "Oh, I'm Green, by the way!\n"
	.string "Nice to meet you!\p"
	.string "But if you want to meet Mewtwo,\n"
	.string "you'll have to get past me first!$"

AlteringCave_Text_LeafDefeated:
	.string "Wow, you're strong!$"

AlteringCave_Text_LeafGift:
	.string "Green: Well, I guess that's that!\p"
	.string "Here, take these.$"

AlteringCave_Text_LeafExit:
	.string "This is goodbye for now, {PLAYER}.\n"
	.string "But I'm sure we'll meet again!$"

AlteringCave_Movement_Leaf:
	walk_down
	step_end

AlteringCave_Movement_Player:
	walk_up
	step_end

AlteringCave_Movement_GiveWay:
	walk_left
	walk_in_place_fast_right
	step_end

AlteringCave_Movement_LeafLeaves:
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end
