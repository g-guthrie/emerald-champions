VerdanturfMeadow_MapScripts::
	.byte 0

@ Route sign for the Meadow, Enamorus's anomaly home. Proposed: a sign
@ bg_event in map.json (Codex owns placement) pointing here; until then this
@ label is unreferenced.
VerdanturfMeadow_EventScript_MeadowSign::
	msgbox VerdanturfMeadow_Text_MeadowSign, MSGBOX_SIGN
	call Common_EventScript_ShowRouteRoster
	setvar VAR_0x8004, LEGENDARY_SIGN_ENAMORUS
	call Common_EventScript_ShowRouteLegend
	setvar VAR_0x8004, LEGENDARY_SIGN_FEZANDIPITI
	call Common_EventScript_ShowRouteLegend
	releaseall
	end

VerdanturfMeadow_Text_MeadowSign:
	.string "Verdanturf Meadow\n"
	.string "{UP_ARROW} Verdanturf Town$"

VerdanturfMeadow_EventScript_GiftAudino::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_AUDINO, VerdanturfMeadow_EventScript_TakeCareOfAudino
	msgbox VerdanturfMeadow_Text_PleaseTakePokemon, MSGBOX_DEFAULT
	setvar VAR_TEMP_1, SPECIES_AUDINO
	@ Gift Pokemon arrive at the current level cap.
	special GetLevelCapForScriptedGift
	givemon SPECIES_AUDINO, VAR_0x800A, ITEM_LUCKY_EGG
	compare VAR_RESULT, 0
	goto_if_eq VerdanturfMeadow_EventScript_ReceiveAudinoParty
	compare VAR_RESULT, 1
	goto_if_eq VerdanturfMeadow_EventScript_ReceiveAudinoPC
	goto Common_EventScript_NoMoreRoomForPokemon
	end

VerdanturfMeadow_EventScript_ReceiveAudinoParty::
	call VerdanturfMeadow_EventScript_ReceivedAudinoFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq VerdanturfMeadow_EventScript_TakeCareOfAudino
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto VerdanturfMeadow_EventScript_TakeCareOfAudino
	end

VerdanturfMeadow_EventScript_ReceiveAudinoPC::
	call VerdanturfMeadow_EventScript_ReceivedAudinoFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq VerdanturfMeadow_EventScript_SendAudinoToPC
	call Common_EventScript_NameReceivedBoxMon
	goto VerdanturfMeadow_EventScript_SendAudinoToPC
	end

VerdanturfMeadow_EventScript_SendAudinoToPC::
	call Common_EventScript_TransferredToPC
	goto VerdanturfMeadow_EventScript_TakeCareOfAudino
	end

VerdanturfMeadow_EventScript_ReceivedAudinoFanfare::
	playfanfare MUS_OBTAIN_ITEM
	message VerdanturfMeadow_Text_PlayerReceivedAudino
	waitmessage
	waitfanfare
	bufferspeciesname 0, SPECIES_AUDINO
	return

VerdanturfMeadow_EventScript_TakeCareOfAudino::
	msgbox VerdanturfMeadow_Text_TakeCareOfAudino, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_AUDINO
	release
	end

VerdanturfMeadow_Text_PleaseTakePokemon:
	.string "Do you know the Pokémon Audino?\p"
	.string "I'm giving them away to promote\n"
	.string "Audino welfare!\p"
	.string "Some Trainers use Audino like punching\n"
	.string "bags for their Pokémon, just to make\l"
	.string "their team grow quickly.\p"
	.string "This is not OK!\p"
	.string "Please take this Audino to keep it safe\n"
	.string "from EXP-mad Trainers!$"

VerdanturfMeadow_Text_PlayerReceivedAudino:
	.string "{PLAYER} received Audino!$"

VerdanturfMeadow_Text_TakeCareOfAudino:
	.string "Take good care of Audino!$"
