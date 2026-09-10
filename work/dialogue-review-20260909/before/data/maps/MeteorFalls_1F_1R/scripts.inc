MeteorFalls_1F_1R_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, MeteorFalls_1F_1R_OnLoad
	.byte 0

MeteorFalls_1F_1R_EventScript_RivalTalkAfterBattle::
	msgbox MeteorFalls_1F_1R_Text_RivalAfterBattleRestored, MSGBOX_NPC
	end

MeteorFalls_1F_1R_Text_RivalAfterBattleRestored:
	.string "MAGMA took the METEORITE toward\n"
	.string "MT. CHIMNEY. We have to follow.\p"
	.string "Take ROUTE 112's cable car\n"
	.string "up to the summit.$"

MeteorFalls_1F_1R_OnLoad:
	call_if_set FLAG_SYS_GAME_CLEAR, MeteorFalls_1F_1R_EventScript_OpenStevensCave
	@ The restored rival object uses OBJ_EVENT_GFX_VAR_0, so resolve it here
	@ exactly as Inclement does instead of inheriting the previous map's value.
	call Common_EventScript_SetupRivalGfxId
	end

MeteorFalls_1F_1R_EventScript_OpenStevensCave::
	setmetatile 4, 1, METATILE_MeteorFalls_CaveEntrance_Top, TRUE
	setmetatile 3, 2, METATILE_MeteorFalls_CaveEntrance_Left, TRUE
	setmetatile 4, 2, METATILE_MeteorFalls_CaveEntrance_Bottom, FALSE
	setmetatile 5, 2, METATILE_MeteorFalls_CaveEntrance_Right, TRUE
	return

MeteorFalls_1F_1R_EventScript_MagmaStealsMeteoriteScene::
	lockall
	call EventScript_CheckStoryDoubleBattleParty
	playbgm MUS_ENCOUNTER_MAGMA, FALSE
	applymovement LOCALID_PLAYER, Common_Movement_FaceDown
	waitmovement 0
	delay 30
	@ Inclement's restored placement puts Magma on the lower floor. Bring the
	@ player down the ramp so every later path stays on passable tiles.
	applymovement LOCALID_PLAYER, MeteorFalls_1F_1R_Movement_PlayerWalkDown2
	waitmovement 0
	applymovement LOCALID_METEOR_FALLS_RIVAL, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_1, Common_Movement_WalkInPlaceFasterRight
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_2, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_WithThisMeteorite, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_1, MeteorFalls_1F_1R_Movement_MagmaGruntStepForward
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	playse SE_PIN
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_1, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_1, Common_Movement_Delay48
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_DontExpectMercyFromMagma, MSGBOX_DEFAULT
	closemessage
	trainerbattle_double_two_trainers TRAINER_COURTNEY_METEOR_FALLS, MeteorFalls_1F_1R_Text_CourtneyDefeat, TRAINER_GRUNT_METEOR_FALLS, MeteorFalls_1F_1R_Text_GruntDefeat, EmeraldChampions_Text_NeedTwoPokemon
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_1, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_HoldItRightThereMagma, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_METEOR_FALLS_RIVAL, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_1, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_2, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	addobject LOCALID_METEOR_FALLS_ARCHIE
	addobject LOCALID_METEOR_FALLS_AQUA_GRUNT_1
	addobject LOCALID_METEOR_FALLS_AQUA_GRUNT_2
	playbgm MUS_ENCOUNTER_AQUA, FALSE
	applymovement LOCALID_METEOR_FALLS_ARCHIE, MeteorFalls_1F_1R_Movement_ArchieArrive
	applymovement LOCALID_METEOR_FALLS_AQUA_GRUNT_1, MeteorFalls_1F_1R_Movement_AquaGrunt1Arrive
	applymovement LOCALID_METEOR_FALLS_AQUA_GRUNT_2, MeteorFalls_1F_1R_Movement_AquaGrunt2Arrive
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_BeSeeingYouTeamAqua, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, MeteorFalls_1F_1R_Movement_PushPlayerOutOfWay
	applymovement LOCALID_METEOR_FALLS_RIVAL, MeteorFalls_1F_1R_Movement_PushPlayerOutOfWay
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_1, MeteorFalls_1F_1R_Movement_MagmaGrunt1Exit
	applymovement LOCALID_METEOR_FALLS_MAGMA_GRUNT_2, MeteorFalls_1F_1R_Movement_MagmaGrunt2Exit
	waitmovement 0
	removeobject LOCALID_METEOR_FALLS_MAGMA_GRUNT_1
	removeobject LOCALID_METEOR_FALLS_MAGMA_GRUNT_2
	applymovement LOCALID_METEOR_FALLS_ARCHIE, MeteorFalls_1F_1R_Movement_ArchieApproachPlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
	applymovement LOCALID_METEOR_FALLS_RIVAL, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_ArchieSeenYouBefore, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_METEOR_FALLS_AQUA_GRUNT_1, MeteorFalls_1F_1R_Movement_AquaGrunt1ApproachArchie
	applymovement LOCALID_METEOR_FALLS_AQUA_GRUNT_2, MeteorFalls_1F_1R_Movement_AquaGrunt2ApproachArchie
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_BossWeShouldChaseMagma, MSGBOX_DEFAULT
	applymovement LOCALID_METEOR_FALLS_ARCHIE, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_ArchieYesNoTellingWhatMagmaWillDo, MSGBOX_DEFAULT
	applymovement LOCALID_METEOR_FALLS_ARCHIE, Common_Movement_WalkInPlaceFasterRight
	waitmovement 0
	msgbox MeteorFalls_1F_1R_Text_ArchieFarewell, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_METEOR_FALLS_ARCHIE, MeteorFalls_1F_1R_Movement_ArchieExit
	applymovement LOCALID_METEOR_FALLS_AQUA_GRUNT_1, MeteorFalls_1F_1R_Movement_AquaGrunt1Exit
	applymovement LOCALID_METEOR_FALLS_AQUA_GRUNT_2, MeteorFalls_1F_1R_Movement_AquaGrunt2Exit
	applymovement LOCALID_PLAYER, MeteorFalls_1F_1R_Movement_WatchAquaLeave
	applymovement LOCALID_METEOR_FALLS_RIVAL, MeteorFalls_1F_1R_Movement_WatchAquaLeave
	waitmovement 0
	fadedefaultbgm
	removeobject LOCALID_METEOR_FALLS_ARCHIE
	removeobject LOCALID_METEOR_FALLS_AQUA_GRUNT_1
	removeobject LOCALID_METEOR_FALLS_AQUA_GRUNT_2
	setflag FLAG_HIDE_ROUTE_112_TEAM_MAGMA
	setflag FLAG_MET_ARCHIE_METEOR_FALLS
	setflag FLAG_HIDE_FALLARBOR_TOWN_BATTLE_TENT_SCOTT
	setvar VAR_METEOR_FALLS_STATE, 1
	releaseall
	end

@ Every path below was walked against the Inclement Emerald v1.13 layout that
@ this map now uses: Magma stands at (11,20)/(12,21), Aqua enters from (4,20)
@ and (4,21), and every exit climbs the (14,19) ramp before the row-18 corridor.
MeteorFalls_1F_1R_Movement_PlayerWalkDown2:
	walk_down
	walk_down
	step_end

MeteorFalls_1F_1R_Movement_MagmaGruntStepForward:
	walk_right
	walk_in_place_faster_right
	step_end

MeteorFalls_1F_1R_Movement_MagmaGrunt1Exit:
	delay_16
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

MeteorFalls_1F_1R_Movement_MagmaGrunt2Exit:
	walk_fast_right
	walk_fast_up
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

MeteorFalls_1F_1R_Movement_ArchieArrive:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_AquaGrunt1Arrive:
	delay_16
	delay_16
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_AquaGrunt2Arrive:
	delay_16
	delay_16
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_ArchieApproachPlayer:
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_AquaGrunt1ApproachArchie:
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_AquaGrunt2ApproachArchie:
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_ArchieExit:
	walk_up
	walk_right
	walk_right
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_AquaGrunt1Exit:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_AquaGrunt2Exit:
	walk_right
	walk_right
	walk_up
	walk_right
	walk_right
	walk_up
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

MeteorFalls_1F_1R_Movement_PushPlayerOutOfWay:
	walk_in_place_faster_left
	delay_4
	walk_in_place_faster_up
	lock_facing_direction
	walk_down
	unlock_facing_direction
	face_right
	step_end

MeteorFalls_1F_1R_Movement_WatchAquaLeave:
	delay_16
	delay_16
	walk_in_place_faster_up
	delay_16
	delay_16
	walk_in_place_faster_right
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	step_end

MeteorFalls_1F_1R_EventScript_ProfCozmo::
	lock
	faceplayer
	goto_if_set FLAG_MET_PROF_COZMO, MeteorFalls_1F_1R_EventScript_MetCozmo
	setflag FLAG_MET_PROF_COZMO
	msgbox MeteorFalls_1F_1R_Text_MeetProfCozmo, MSGBOX_DEFAULT
	release
	end

MeteorFalls_1F_1R_EventScript_MetCozmo::
	msgbox MeteorFalls_1F_1R_Text_WhatsTeamMagmaDoingAtMtChimney, MSGBOX_DEFAULT
	release
	end

MeteorFalls_1F_1R_Text_WithThisMeteorite:
	.string "The METEORITE is secured. Its\n"
	.string "energy will feed MAXIE's machine\l"
	.string "at MT. CHIMNEY!$"

MeteorFalls_1F_1R_Text_DontExpectMercyFromMagma:
	.string "COURTNEY: You crossed our operation\n"
	.string "twice. Coincidence is no longer likely.\p"
	.string "This formation will remove you from\n"
	.string "MAXIE's equation.$"

MeteorFalls_1F_1R_Text_CourtneyDefeat:
	.string "You broke our celestial formation…$"

MeteorFalls_1F_1R_Text_GruntDefeat:
	.string "Even the meteor team failed?!$"

MeteorFalls_1F_1R_Text_HoldItRightThereMagma:
	.string "Hold it, TEAM MAGMA! If you set\n"
	.string "off that volcano, the whole coast\l"
	.string "will suffer!$"

MeteorFalls_1F_1R_Text_BeSeeingYouTeamAqua:
	.string "COURTNEY: AQUA arrived sooner\n"
	.string "than projected. The result is\l"
	.string "unchanged.\p"
	.string "We have the METEORITE. MAXIE is\n"
	.string "already preparing the machine at\l"
	.string "MT. CHIMNEY.\p"
	.string "Follow us if you want to see the\n"
	.string "land rise.$"

MeteorFalls_1F_1R_Text_ArchieSeenYouBefore:
	.string "ARCHIE: We met at SLATEPORT's MUSEUM.\n"
	.string "{PLAYER}, wasn't it?\p"
	.string "I mistook you for one of MAGMA's crew.\n"
	.string "You proved you choose your own current.\p"
	.string "MAXIE thinks every living system can be\n"
	.string "measured, fixed, and made obedient.\p"
	.string "Now he means to force that certainty\n"
	.string "through MT. CHIMNEY.\p"
	.string "AQUA will stop him. If you value a world\n"
	.string "that can still change, follow us.$"

MeteorFalls_1F_1R_Text_BossWeShouldChaseMagma:
	.string "BOSS, we should give chase to\n"
	.string "TEAM MAGMA…$"

MeteorFalls_1F_1R_Text_ArchieYesNoTellingWhatMagmaWillDo:
	.string "ARCHIE: We move now.\p"
	.string "If MAGMA drives that METEORITE into the\n"
	.string "fault line, the mountain answers first.$"

MeteorFalls_1F_1R_Text_ArchieFarewell:
	.string "ARCHIE: {PLAYER}, come to MT. CHIMNEY.\n"
	.string "Take the cable car on ROUTE 112.\p"
	.string "You have already changed this race.$"

MeteorFalls_1F_1R_Text_MeetProfCozmo:
	.string "I'm PROF. COZMO. MAGMA asked me\n"
	.string "to study the METEORITE's energy.\p"
	.string "I thought it was geological\n"
	.string "research. They took the sample\l"
	.string "and my readings.\p"
	.string "That energy could trigger an\n"
	.string "eruption at MT. CHIMNEY. People\l"
	.string "and POKéMON would lose their\l"
	.string "homes!$"

MeteorFalls_1F_1R_Text_WhatsTeamMagmaDoingAtMtChimney:
	.string "PROF. COZMO: MAGMA plans to use\n"
	.string "the METEORITE to power a machine\l"
	.string "at the volcano.\p"
	.string "Please stop them before it\n"
	.string "erupts! ROUTE 112's cable car\l"
	.string "goes to MT. CHIMNEY's summit.$"
