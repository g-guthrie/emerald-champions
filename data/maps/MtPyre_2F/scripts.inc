MtPyre_2F_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CaveHole_CheckFallDownHole
	map_script MAP_SCRIPT_ON_TRANSITION, CaveHole_FixCrackedGround
	map_script MAP_SCRIPT_ON_RESUME, MtPyre_2F_SetHoleWarp
	.byte 0

MtPyre_2F_SetHoleWarp:
	setstepcallback STEP_CB_CRACKED_FLOOR
	setholewarp MAP_MT_PYRE_1F
	end

MtPyre_2F_EventScript_Woman::
	msgbox MtPyre_2F_Text_MemoriesOfSkitty, MSGBOX_NPC
	end

MtPyre_2F_EventScript_PokefanM::
	msgbox MtPyre_2F_Text_TumbledFromFloorAbove, MSGBOX_NPC
	end

MtPyre_2F_EventScript_Mark::
	msgbox MtPyre_2F_Text_MarkPostBattle, MSGBOX_NPC
	end

MtPyre_2F_EventScript_Luke::
	msgbox MtPyre_2F_Text_LukePostBattle, MSGBOX_NPC
	end

MtPyre_2F_EventScript_Dez::
	msgbox MtPyre_2F_Text_DezPostBattle, MSGBOX_NPC
	end

MtPyre_2F_EventScript_Leah::
	msgbox MtPyre_2F_Text_LeahPostBattle, MSGBOX_NPC
	end

MtPyre_2F_EventScript_Zander::
	msgbox MtPyre_2F_Text_ZanderPostBattle, MSGBOX_NPC
	end

MtPyre_2F_Text_MemoriesOfSkitty:
	.string "Memories of my darling SKITTY…\n"
	.string "My eyes overflow thinking about it.$"

MtPyre_2F_Text_TumbledFromFloorAbove:
	.string "Ooch, ouch… There are holes in the\n"
	.string "ground here and there.\p"
	.string "I didn't notice and took a tumble from\n"
	.string "the floor above.$"

MtPyre_2F_Text_MarkPostBattle:
	.string "People don't come here often, so\n"
	.string "I thought there'd be rare POKéMON.$"

MtPyre_2F_Text_LukePostBattle::
	.string "LUKE: I am right here by your side.\n"
	.string "We will make it through this dare!$"

MtPyre_2F_Text_DezPostBattle::
	.string "DEZ: We are lovey-dovey!\n"
	.string "Even this spooky place cannot stop us.$"

MtPyre_2F_Text_LeahPostBattle:
	.string "Our family has been TRAINERS here\n"
	.string "since my great-grandmother's time…\p"
	.string "It is my duty to protect this\n"
	.string "mountain…$"

MtPyre_2F_Text_ZanderPostBattle:
	.string "I get freaked out every time I see\n"
	.string "anything move…\p"
	.string "I shouldn't have come here to train…$"

