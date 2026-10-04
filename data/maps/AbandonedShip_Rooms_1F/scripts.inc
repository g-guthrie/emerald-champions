AbandonedShip_Rooms_1F_MapScripts:: @ 8237A92
	.byte 0

AbandonedShip_Rooms_1F_EventScript_Gentleman:: @ 8237A93
	msgbox AbandonedShip_Rooms_1F_Text_TakingALookAround, MSGBOX_NPC
	end

AbandonedShip_Rooms_1F_Text_TakingALookAround: @ 8237B15
	.string "Ships of this sort are rare, so I'm\n"
	.string "taking a look around.\p"
	.string "Hmhm…\n"
	.string "There appear to be other cabins…$"

@ A Pikachu doll left in an empty cabin is really Mimikyu. Running away leaves
@ it here for another try; a capture or a knockout removes it for good
@ (removeobject sets FLAG_HIDE_ABANDONED_SHIP_MIMIKYU).
AbandonedShip_Rooms_1F_EventScript_MimikyuDoll::
	lock
	msgbox AbandonedShip_Rooms_1F_Text_RaggedDoll, MSGBOX_DEFAULT
	closemessage
	special GetLevelCapForScriptedGift
	setwildbattle SPECIES_MIMIKYU_DISGUISED, VAR_0x800A, ITEM_NONE
	waitse
	playmoncry SPECIES_MIMIKYU_DISGUISED, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	dowildbattle
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, AbandonedShip_Rooms_1F_EventScript_MimikyuGone
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, AbandonedShip_Rooms_1F_EventScript_MimikyuGone
	release
	end

AbandonedShip_Rooms_1F_EventScript_MimikyuGone::
	fadescreenswapbuffers FADE_TO_BLACK
	removeobject VAR_LAST_TALKED
	fadescreenswapbuffers FADE_FROM_BLACK
	release
	end

AbandonedShip_Rooms_1F_Text_RaggedDoll:
	.string "A ragged Pikachu doll, all alone\n"
	.string "in the dark…\p"
	.string "…It twitched!$"
