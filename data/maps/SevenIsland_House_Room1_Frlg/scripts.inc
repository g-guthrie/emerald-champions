@ Emerald Champions: the e-Reader visiting-trainer battle is deleted; the old woman
@ keeps her reminiscing dialogue and the hidden room stays closed.
SevenIsland_House_Room1_Frlg_MapScripts::
	.byte 0

SevenIsland_House_Room1_EventScript_OldWoman::
	lock
	faceplayer
	msgbox SevenIsland_House_Room1_Text_OnlyEnjoymentWatchingBattles
	release
	end

SevenIsland_House_Room1_EventScript_Box::
	msgbox SevenIsland_House_Room1_Text_SlightBreezeAroundBox, MSGBOX_SIGN
	end

SevenIsland_House_Room1_Text_OnlyEnjoymentWatchingBattles::
	.string "I may not look like much now,\n"
	.string "but when I was younger…\p"
	.string "I flew everywhere, challenging\n"
	.string "TRAINERS wherever I found them.\p"
	.string "Now, my only source of enjoyment\n"
	.string "is reminiscing about the past while\l"
	.string "watching battles.$"

SevenIsland_House_Room1_Text_SlightBreezeAroundBox::
	.string "There is a slight breeze blowing\n"
	.string "around the box.$"

