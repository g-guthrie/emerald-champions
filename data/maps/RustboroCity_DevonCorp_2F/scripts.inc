RustboroCity_DevonCorp_2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RustboroCity_DevonCorp_2F_OnTransition
	.byte 0

RustboroCity_DevonCorp_2F_OnTransition:
	call_if_eq VAR_FOSSIL_RESURRECTION_STATE, 1, RustboroCity_DevonCorp_2F_EventScript_SetFossilReady
	end

RustboroCity_DevonCorp_2F_EventScript_SetFossilReady::
	setvar VAR_FOSSIL_RESURRECTION_STATE, 2
	return

RustboroCity_DevonCorp_2F_EventScript_TalkToPokemonScientist::
	lock
	faceplayer
	call_if_eq VAR_FOSSIL_RESURRECTION_STATE, 1, RustboroCity_DevonCorp_2F_EventScript_SetFossilReady
	msgbox RustboroCity_DevonCorp_2F_Text_DeviceForTalkingToPokemon, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_BallScientist::
	lock
	faceplayer
	call_if_eq VAR_FOSSIL_RESURRECTION_STATE, 1, RustboroCity_DevonCorp_2F_EventScript_SetFossilReady
	goto_if_set FLAG_MET_DEVON_EMPLOYEE, RustboroCity_DevonCorp_2F_EventScript_DevelopedBalls
	msgbox RustboroCity_DevonCorp_2F_Text_DevelopingNewBalls, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_DevelopedBalls::
	msgbox RustboroCity_DevonCorp_2F_Text_WeFinallyMadeNewBalls, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_PokenavScientist::
	lock
	faceplayer
	call_if_eq VAR_FOSSIL_RESURRECTION_STATE, 1, RustboroCity_DevonCorp_2F_EventScript_SetFossilReady
	goto_if_set FLAG_RECEIVED_POKENAV, RustboroCity_DevonCorp_2F_EventScript_HasPokenav
	msgbox RustboroCity_DevonCorp_2F_Text_IMadePokenav, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_HasPokenav::
	msgbox RustboroCity_DevonCorp_2F_Text_WowThatsAPokenav, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_PokemonDreamsScientist::
	lock
	faceplayer
	call_if_eq VAR_FOSSIL_RESURRECTION_STATE, 1, RustboroCity_DevonCorp_2F_EventScript_SetFossilReady
	msgbox RustboroCity_DevonCorp_2F_Text_LegendarySignResearcherIntro, MSGBOX_DEFAULT
RustboroCity_DevonCorp_2F_EventScript_CheckLegendarySign::
	special TryDiscoverEligibleLegendarySign
	goto_if_eq VAR_RESULT, 0, RustboroCity_DevonCorp_2F_EventScript_NoLegendarySignYet
	goto_if_eq VAR_RESULT, 1, RustboroCity_DevonCorp_2F_EventScript_LegendarySignClue
	goto_if_eq VAR_RESULT, 2, RustboroCity_DevonCorp_2F_EventScript_LegendarySignAwakened
	special TryGiveArceusLegendarySignMasteryReward
	goto_if_eq VAR_RESULT, 1, RustboroCity_DevonCorp_2F_EventScript_ArceusRewardParty
	goto_if_eq VAR_RESULT, 2, RustboroCity_DevonCorp_2F_EventScript_ArceusRewardPC
	goto_if_eq VAR_RESULT, 3, RustboroCity_DevonCorp_2F_EventScript_ArceusRewardNoRoom
	goto_if_eq VAR_RESULT, 4, RustboroCity_DevonCorp_2F_EventScript_ArceusRewardComplete
	msgbox RustboroCity_DevonCorp_2F_Text_AllLegendarySignsRecorded, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_ArceusRewardParty::
	msgbox RustboroCity_DevonCorp_2F_Text_ArceusRewardParty, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_ArceusRewardPC::
	msgbox RustboroCity_DevonCorp_2F_Text_ArceusRewardPC, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_ArceusRewardNoRoom::
	msgbox RustboroCity_DevonCorp_2F_Text_ArceusRewardNoRoom, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_ArceusRewardComplete::
	msgbox RustboroCity_DevonCorp_2F_Text_ArceusRewardComplete, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_LegendarySignAwakened::
	msgbox RustboroCity_DevonCorp_2F_Text_LegendarySignAwakened, MSGBOX_DEFAULT
	goto RustboroCity_DevonCorp_2F_EventScript_CheckLegendarySign

RustboroCity_DevonCorp_2F_EventScript_LegendarySignClue::
	msgbox RustboroCity_DevonCorp_2F_Text_LegendarySignClue, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_NoLegendarySignYet::
	msgbox RustboroCity_DevonCorp_2F_Text_NoLegendarySignYet, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_Text_LegendarySignResearcherIntro:
	.string "My dream visualizer detected marks\n"
	.string "I call LEGENDARY SIGNS.\p"
	.string "They wake when the right Pokémon\n"
	.string "returns to the marked place.\p"
	.string "I can also tune an eligible Sign\n"
	.string "remotely if you bring its partner here.$"

RustboroCity_DevonCorp_2F_Text_LegendarySignAwakened:
	.string "{STR_VAR_1} resonates with my data!\p"
	.string "A Legendary Sign has awakened.\n"
	.string "{STR_VAR_2} may now appear at\n"
	.string "{STR_VAR_3}.$"

RustboroCity_DevonCorp_2F_Text_LegendarySignClue:
	.string "My next readable Sign points toward\n"
	.string "{STR_VAR_2} at {STR_VAR_3}.\p"
	.string "Take {STR_VAR_1} there to wake it,\n"
	.string "or bring that partner to me.$"

RustboroCity_DevonCorp_2F_Text_NoLegendarySignYet:
	.string "The next Sign is still beyond the\n"
	.string "progress of your Gym challenge.$"

RustboroCity_DevonCorp_2F_Text_AllLegendarySignsRecorded:
	.string "Every conditional wild SIGN I can tune\n"
	.string "remotely is awake.\p"
	.string "The network also spans visible shrines,\n"
	.string "rare wild finds, breeding, the GAME\l"
	.string "CORNER, CIRCUIT rewards, and mastery.\p"
	.string "Complete every source--including MT.\n"
	.string "PYRE's three--to call ARCEUS.$"

RustboroCity_DevonCorp_2F_Text_ArceusRewardParty:
	.string "Every Legendary Sign is complete!\p"
	.string "Their light calls ARCEUS, who has\n"
	.string "joined your party.$"

RustboroCity_DevonCorp_2F_Text_ArceusRewardPC:
	.string "Every Legendary Sign is complete!\p"
	.string "Their light calls ARCEUS, who was\n"
	.string "sent to your PC.$"

RustboroCity_DevonCorp_2F_Text_ArceusRewardNoRoom:
	.string "Every Sign is complete, but ARCEUS\n"
	.string "has nowhere to go. Make PC space.$"

RustboroCity_DevonCorp_2F_Text_ArceusRewardComplete:
	.string "You completed every Legendary Sign.\n"
	.string "Even ARCEUS answered your call.$"

RustboroCity_DevonCorp_2F_EventScript_FossilScientist::
	lock
	faceplayer
	goto_if_eq VAR_FOSSIL_RESURRECTION_STATE, 2, RustboroCity_DevonCorp_2F_EventScript_FossilMonReady
	goto_if_eq VAR_FOSSIL_RESURRECTION_STATE, 1, RustboroCity_DevonCorp_2F_EventScript_StillRegenerating
	msgbox RustboroCity_DevonCorp_2F_Text_DevelopDeviceToResurrectFossils, MSGBOX_DEFAULT
	checkitem ITEM_HELIX_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_DOME_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_OLD_AMBER
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_ROOT_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_CLAW_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_ARMOR_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_SKULL_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_COVER_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_PLUME_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_JAW_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	checkitem ITEM_SAIL_FOSSIL
	goto_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_NoticeFossil
	release
	end

RustboroCity_DevonCorp_2F_EventScript_NoticeFossil::
	closemessage
	playse SE_PIN
	applymovement LOCALID_FOSSIL_SCIENTIST, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_FOSSIL_SCIENTIST, Common_Movement_Delay48
	waitmovement 0
	msgbox RustboroCity_DevonCorp_2F_Text_WantToBringFossilBackToLife, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, RustboroCity_DevonCorp_2F_EventScript_DeclineGiveFossil
	closemessage
	message RustboroCity_DevonCorp_2F_Text_ChooseFossil
	waitmessage
	checkitem ITEM_HELIX_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushHelixFossil
	checkitem ITEM_DOME_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushDomeFossil
	checkitem ITEM_OLD_AMBER
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushOldAmber
	checkitem ITEM_ROOT_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushRootFossil
	checkitem ITEM_CLAW_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushClawFossil
	checkitem ITEM_ARMOR_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushArmorFossil
	checkitem ITEM_SKULL_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushSkullFossil
	checkitem ITEM_COVER_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushCoverFossil
	checkitem ITEM_PLUME_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushPlumeFossil
	checkitem ITEM_JAW_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushJawFossil
	checkitem ITEM_SAIL_FOSSIL
	call_if_eq VAR_RESULT, TRUE, RustboroCity_DevonCorp_2F_EventScript_PushSailFossil
	dynmultistack 0, 1, FALSE, 5, FALSE, 0, DYN_MULTICHOICE_CB_SHOW_ITEM
	closemessage
	switch VAR_RESULT
	case ITEM_HELIX_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChooseHelixFossil
	case ITEM_DOME_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChooseDomeFossil
	case ITEM_OLD_AMBER, RustboroCity_DevonCorp_2F_EventScript_ChooseOldAmber
	case ITEM_ROOT_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChooseRootFossil
	case ITEM_CLAW_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChooseClawFossil
	case ITEM_ARMOR_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChooseArmorFossil
	case ITEM_SKULL_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChooseSkullFossil
	case ITEM_COVER_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChooseCoverFossil
	case ITEM_PLUME_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChoosePlumeFossil
	case ITEM_JAW_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChooseJawFossil
	case ITEM_SAIL_FOSSIL, RustboroCity_DevonCorp_2F_EventScript_ChooseSailFossil
	goto RustboroCity_DevonCorp_2F_EventScript_CancelFossilSelect
	end

RustboroCity_DevonCorp_2F_EventScript_PushHelixFossil::
	bufferitemname STR_VAR_1, ITEM_HELIX_FOSSIL
	dynmultipush gStringVar1, ITEM_HELIX_FOSSIL
	return

RustboroCity_DevonCorp_2F_EventScript_PushDomeFossil::
	bufferitemname STR_VAR_1, ITEM_DOME_FOSSIL
	dynmultipush gStringVar1, ITEM_DOME_FOSSIL
	return

RustboroCity_DevonCorp_2F_EventScript_PushOldAmber::
	bufferitemname STR_VAR_1, ITEM_OLD_AMBER
	dynmultipush gStringVar1, ITEM_OLD_AMBER
	return

RustboroCity_DevonCorp_2F_EventScript_PushRootFossil::
	bufferitemname STR_VAR_1, ITEM_ROOT_FOSSIL
	dynmultipush gStringVar1, ITEM_ROOT_FOSSIL
	return

RustboroCity_DevonCorp_2F_EventScript_PushClawFossil::
	bufferitemname STR_VAR_1, ITEM_CLAW_FOSSIL
	dynmultipush gStringVar1, ITEM_CLAW_FOSSIL
	return

RustboroCity_DevonCorp_2F_EventScript_PushArmorFossil::
	bufferitemname STR_VAR_1, ITEM_ARMOR_FOSSIL
	dynmultipush gStringVar1, ITEM_ARMOR_FOSSIL
	return

RustboroCity_DevonCorp_2F_EventScript_PushSkullFossil::
	bufferitemname STR_VAR_1, ITEM_SKULL_FOSSIL
	dynmultipush gStringVar1, ITEM_SKULL_FOSSIL
	return

RustboroCity_DevonCorp_2F_EventScript_PushCoverFossil::
	bufferitemname STR_VAR_1, ITEM_COVER_FOSSIL
	dynmultipush gStringVar1, ITEM_COVER_FOSSIL
	return

RustboroCity_DevonCorp_2F_EventScript_PushPlumeFossil::
	bufferitemname STR_VAR_1, ITEM_PLUME_FOSSIL
	dynmultipush gStringVar1, ITEM_PLUME_FOSSIL
	return

RustboroCity_DevonCorp_2F_EventScript_PushJawFossil::
	bufferitemname STR_VAR_1, ITEM_JAW_FOSSIL
	dynmultipush gStringVar1, ITEM_JAW_FOSSIL
	return

RustboroCity_DevonCorp_2F_EventScript_PushSailFossil::
	bufferitemname STR_VAR_1, ITEM_SAIL_FOSSIL
	dynmultipush gStringVar1, ITEM_SAIL_FOSSIL
	return

@ Values 1 and 2 retain compatibility with saves made during the original
@ Root/Claw-only revival flow.
RustboroCity_DevonCorp_2F_EventScript_ChooseRootFossil::
	setvar VAR_0x8004, ITEM_ROOT_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 1
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChooseClawFossil::
	setvar VAR_0x8004, ITEM_CLAW_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 2
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChooseHelixFossil::
	setvar VAR_0x8004, ITEM_HELIX_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 3
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChooseDomeFossil::
	setvar VAR_0x8004, ITEM_DOME_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 4
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChooseOldAmber::
	setvar VAR_0x8004, ITEM_OLD_AMBER
	setvar VAR_WHICH_FOSSIL_REVIVED, 5
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChooseArmorFossil::
	setvar VAR_0x8004, ITEM_ARMOR_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 6
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChooseSkullFossil::
	setvar VAR_0x8004, ITEM_SKULL_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 7
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChooseCoverFossil::
	setvar VAR_0x8004, ITEM_COVER_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 8
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChoosePlumeFossil::
	setvar VAR_0x8004, ITEM_PLUME_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 9
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChooseJawFossil::
	setvar VAR_0x8004, ITEM_JAW_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 10
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_ChooseSailFossil::
	setvar VAR_0x8004, ITEM_SAIL_FOSSIL
	setvar VAR_WHICH_FOSSIL_REVIVED, 11
	goto RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival

RustboroCity_DevonCorp_2F_EventScript_BeginFossilRevival::
	removeitem VAR_0x8004
	goto_if_eq VAR_RESULT, FALSE, RustboroCity_DevonCorp_2F_EventScript_FossilNoLongerAvailable
	bufferitemname STR_VAR_1, VAR_0x8004
	msgbox RustboroCity_DevonCorp_2F_Text_HandedFossilToResearcher, MSGBOX_DEFAULT
	setvar VAR_FOSSIL_RESURRECTION_STATE, 1
	release
	end

RustboroCity_DevonCorp_2F_EventScript_FossilNoLongerAvailable::
	setvar VAR_WHICH_FOSSIL_REVIVED, 0
	msgbox RustboroCity_DevonCorp_2F_Text_FossilNoLongerAvailable, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_CancelFossilSelect::
	release
	end

RustboroCity_DevonCorp_2F_EventScript_DeclineGiveFossil::
	msgbox RustboroCity_DevonCorp_2F_Text_OhIsThatSo, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_StillRegenerating::
	msgbox RustboroCity_DevonCorp_2F_Text_FossilRegeneratorTakesTime, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_FossilMonReady::
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 1, RustboroCity_DevonCorp_2F_EventScript_SetLileepReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 2, RustboroCity_DevonCorp_2F_EventScript_SetAnorithReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 3, RustboroCity_DevonCorp_2F_EventScript_SetOmanyteReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 4, RustboroCity_DevonCorp_2F_EventScript_SetKabutoReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 5, RustboroCity_DevonCorp_2F_EventScript_SetAerodactylReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 6, RustboroCity_DevonCorp_2F_EventScript_SetShieldonReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 7, RustboroCity_DevonCorp_2F_EventScript_SetCranidosReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 8, RustboroCity_DevonCorp_2F_EventScript_SetTirtougaReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 9, RustboroCity_DevonCorp_2F_EventScript_SetArchenReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 10, RustboroCity_DevonCorp_2F_EventScript_SetTyruntReady
	goto_if_eq VAR_WHICH_FOSSIL_REVIVED, 11, RustboroCity_DevonCorp_2F_EventScript_SetAmauraReady
	msgbox RustboroCity_DevonCorp_2F_Text_FossilDataLost, MSGBOX_DEFAULT
	setvar VAR_FOSSIL_RESURRECTION_STATE, 0
	setvar VAR_WHICH_FOSSIL_REVIVED, 0
	release
	end

RustboroCity_DevonCorp_2F_EventScript_SetLileepReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_LILEEP
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetAnorithReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_ANORITH
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetOmanyteReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_OMANYTE
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetKabutoReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_KABUTO
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetAerodactylReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_AERODACTYL
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetShieldonReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_SHIELDON
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetCranidosReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_CRANIDOS
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetTirtougaReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_TIRTOUGA
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetArchenReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_ARCHEN
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetTyruntReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_TYRUNT
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_SetAmauraReady::
	setvar VAR_TEMP_TRANSFERRED_SPECIES, SPECIES_AMAURA
	goto RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon

RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMon::
	bufferspeciesname STR_VAR_2, VAR_TEMP_TRANSFERRED_SPECIES
	msgbox RustboroCity_DevonCorp_2F_Text_FossilizedMonBroughtBackToLife, MSGBOX_DEFAULT
	copyvar VAR_0x8004, VAR_TEMP_TRANSFERRED_SPECIES
	setvar VAR_0x8005, 20
	special GiveEmeraldChampionsPreparedPokemon
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY, RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMonParty
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMonPC
	goto Common_EventScript_NoMoreRoomForPokemon
	end

RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMonParty::
	call RustboroCity_DevonCorp_2F_EventScript_ReceivedFossilMonFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, RustboroCity_DevonCorp_2F_EventScript_FinishReceivingFossilMon
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto RustboroCity_DevonCorp_2F_EventScript_FinishReceivingFossilMon
	end

RustboroCity_DevonCorp_2F_EventScript_ReceiveFossilMonPC::
	call RustboroCity_DevonCorp_2F_EventScript_ReceivedFossilMonFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, RustboroCity_DevonCorp_2F_EventScript_TransferFossilMonToPC
	call Common_EventScript_NameReceivedBoxMon
	goto RustboroCity_DevonCorp_2F_EventScript_TransferFossilMonToPC
	end

RustboroCity_DevonCorp_2F_EventScript_TransferFossilMonToPC::
	call Common_EventScript_TransferredToPC
	goto RustboroCity_DevonCorp_2F_EventScript_FinishReceivingFossilMon
	end

RustboroCity_DevonCorp_2F_EventScript_ReceivedFossilMonFanfare::
	bufferspeciesname STR_VAR_2, VAR_TEMP_TRANSFERRED_SPECIES
	playfanfare MUS_OBTAIN_ITEM
	message RustboroCity_DevonCorp_2F_Text_ReceivedMonFromResearcher
	waitmessage
	waitfanfare
	bufferspeciesname STR_VAR_1, VAR_TEMP_TRANSFERRED_SPECIES
	return

RustboroCity_DevonCorp_2F_EventScript_FinishReceivingFossilMon::
	setvar VAR_FOSSIL_RESURRECTION_STATE, 0
	setvar VAR_WHICH_FOSSIL_REVIVED, 0
	setflag FLAG_RECEIVED_REVIVED_FOSSIL_MON
	release
	end

RustboroCity_DevonCorp_2F_EventScript_MatchCallScientist::
	lock
	faceplayer
	call_if_eq VAR_FOSSIL_RESURRECTION_STATE, 1, RustboroCity_DevonCorp_2F_EventScript_SetFossilReady
	goto_if_ge VAR_RUSTBORO_CITY_STATE, 6, RustboroCity_DevonCorp_2F_EventScript_WorkOnNext
	msgbox RustboroCity_DevonCorp_2F_Text_DevelopNewPokenavFeature, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_WorkOnNext::
	msgbox RustboroCity_DevonCorp_2F_Text_WhatToWorkOnNext, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_Text_DeviceForTalkingToPokemon:
	.string "We're developing a device for talking\n"
	.string "with POKéMON.\p"
	.string "But we haven't had much success…$"

RustboroCity_DevonCorp_2F_Text_DevelopingNewBalls:
	.string "I'm developing new kinds of\n"
	.string "POKé BALLS…\p"
	.string "But I haven't made much headway…$"

RustboroCity_DevonCorp_2F_Text_WeFinallyMadeNewBalls:
	.string "We finally made new kinds of\n"
	.string "POKé BALLS!\p"
	.string "The REPEAT BALL makes it easier to\n"
	.string "catch POKéMON you've caught before.\p"
	.string "The TIMER BALL gets better at catching\n"
	.string "POKéMON the longer a battle runs.\p"
	.string "Both are proudly developed by\n"
	.string "the DEVON CORPORATION.\p"
	.string "Please give them a try!$"

RustboroCity_DevonCorp_2F_Text_IMadePokenav:
	.string "I made the POKéNAV!\p"
	.string "As an engineer, I feel blessed to have\n"
	.string "made something so great!$"

RustboroCity_DevonCorp_2F_Text_WowThatsAPokenav:
	.string "Oh, wow!\n"
	.string "That's a POKéNAV!\p"
	.string "It came about as a result of our\n"
	.string "PRESIDENT's desire to learn about\l"
	.string "the feelings of POKéMON.\p"
	.string "Would you like me to describe its\n"
	.string "features in detail?\p"
	.string "No, no. I think you'll find out just by\n"
	.string "trying the POKéNAV out.$"

RustboroCity_DevonCorp_2F_Text_DeviceToVisualizePokemonDreams:
	.string "I'm trying to develop a device that\n"
	.string "visually reproduces the dreams of\l"
	.string "POKéMON…\p"
	.string "But it's not going well.$"

RustboroCity_DevonCorp_2F_Text_DevelopDeviceToResurrectFossils:
	.string "My FOSSIL REGENERATOR can restore\n"
	.string "ancient POKéMON from every era.\p"
	.string "Bring me any complete fossil you find.$"

RustboroCity_DevonCorp_2F_Text_WantToBringFossilBackToLife:
	.string "Wait! You have a POKéMON fossil!\p"
	.string "Shall I restore its ancient POKéMON\n"
	.string "with the FOSSIL REGENERATOR?$"

RustboroCity_DevonCorp_2F_Text_OhIsThatSo:
	.string "Oh, is that so?\p"
	.string "DEVON's technological expertise\n"
	.string "is outstanding, I tell you.$"

RustboroCity_DevonCorp_2F_Text_ChooseFossil:
	.string "The machine restores one fossil at\n"
	.string "a time. Which one shall I revive?$"

RustboroCity_DevonCorp_2F_Text_HandedFossilToResearcher:
	.string "Excellent!\n"
	.string "Let's do this right away.\p"
	.string "{PLAYER} handed the {STR_VAR_1} to\n"
	.string "the DEVON RESEARCHER.$"

RustboroCity_DevonCorp_2F_Text_FossilNoLongerAvailable:
	.string "Oh! That fossil is no longer in your\n"
	.string "Bag. Bring it back and we'll try again.$"

RustboroCity_DevonCorp_2F_Text_FossilDataLost:
	.string "The regenerator's record is unreadable.\n"
	.string "I've reset it so we can try again.$"

RustboroCity_DevonCorp_2F_Text_FossilRegeneratorTakesTime:
	.string "The FOSSIL REGENERATOR, which I made,\n"
	.string "is incredible.\p"
	.string "But it has one drawback--it takes\n"
	.string "a long time to work.\p"
	.string "So, uh… How about you go for a stroll\n"
	.string "and look around for a while?$"

RustboroCity_DevonCorp_2F_Text_FossilizedMonBroughtBackToLife:
	.string "Thanks for waiting!\p"
	.string "Your fossilized POKéMON has been\n"
	.string "brought back to life!\p"
	.string "It was {STR_VAR_2}!\n"
	.string "An ancient POKéMON!$"

RustboroCity_DevonCorp_2F_Text_ReceivedMonFromResearcher:
	.string "{PLAYER} received {STR_VAR_2} from\n"
	.string "the DEVON RESEARCHER.$"

@ Unused
RustboroCity_DevonCorp_2F_Text_TooManyPokemon:
	.string "Uh-oh, you've got too many POKéMON.\n"
	.string "You have no room for this one.$"

RustboroCity_DevonCorp_2F_Text_DevelopNewPokenavFeature:
	.string "I'm trying to develop a new feature\n"
	.string "for the POKéNAV…\p"
	.string "But it's not going well.$"

RustboroCity_DevonCorp_2F_Text_WhatToWorkOnNext:
	.string "Well, now what shall I work on\n"
	.string "developing next?\p"
	.string "Our company allows us to make our\n"
	.string "inspirations into reality.\p"
	.string "One couldn't ask for a better\n"
	.string "environment as an engineer.$"

@ BEGIN EMERALD CHAMPIONS LEGENDARY ROOTS
.set EC_SIGN_MAGEARNA_ID, 58

RustboroCity_DevonCorp_2F_EventScript_Magearna::
	lock
	faceplayer
	setvar VAR_0x8004, EC_SIGN_MAGEARNA_ID
	special TryUnlockSelectedLegendarySign
	goto_if_eq VAR_RESULT, 0, RustboroCity_DevonCorp_2F_EventScript_MagearnaDormant
	goto_if_eq VAR_RESULT, 1, RustboroCity_DevonCorp_2F_EventScript_MagearnaNeedsPartner
	goto_if_eq VAR_RESULT, 4, RustboroCity_DevonCorp_2F_EventScript_MagearnaCleanup
	msgbox RustboroCity_DevonCorp_2F_Text_MagearnaAwakens, MSGBOX_DEFAULT
	waitse
	playmoncry SPECIES_MAGEARNA, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_0x8004, EC_SIGN_MAGEARNA_ID
	special CreateSelectedLegendarySignEncounter
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, RustboroCity_DevonCorp_2F_EventScript_MagearnaCaught
	msgbox RustboroCity_DevonCorp_2F_Text_MagearnaRemains, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_MagearnaDormant::
	msgbox RustboroCity_DevonCorp_2F_Text_MagearnaDormant, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_MagearnaNeedsPartner::
	msgbox RustboroCity_DevonCorp_2F_Text_MagearnaNeedsPartner, MSGBOX_DEFAULT
	release
	end

RustboroCity_DevonCorp_2F_EventScript_MagearnaCaught::
	setflag FLAG_EC_CAUGHT_MAGEARNA
RustboroCity_DevonCorp_2F_EventScript_MagearnaCleanup::
	setflag FLAG_EC_CAUGHT_MAGEARNA
	removeobject LOCALID_EC_MAGEARNA
	release
	end

RustboroCity_DevonCorp_2F_Text_MagearnaDormant:
	.string "A CHAMPION'S SIGN marks a sealed\n"
	.string "DEVON prototype.\p"
	.string "Its light is dormant. HOENN's story\n"
	.string "has not yet reached this place.$"

RustboroCity_DevonCorp_2F_Text_MagearnaNeedsPartner:
	.string "The SIGN sketches DIANCIE.\p"
	.string "Bring that POKéMON's family here, and\n"
	.string "the hidden challenger may answer.$"

RustboroCity_DevonCorp_2F_Text_MagearnaAwakens:
	.string "The CHAMPION'S SIGN erupts with light!\n"
	.string "MAGEARNA answers the challenge!$"

RustboroCity_DevonCorp_2F_Text_MagearnaRemains:
	.string "The SIGN remains bright. MAGEARNA\n"
	.string "will accept another challenge.$"


@ END EMERALD CHAMPIONS LEGENDARY ROOTS
