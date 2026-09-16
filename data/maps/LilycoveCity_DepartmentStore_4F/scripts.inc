LilycoveCity_DepartmentStore_4F_MapScripts::
	.byte 0

LilycoveCity_DepartmentStore_4F_EventScript_Gentleman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_Gentleman, MSGBOX_NPC
	end
LilycoveCity_DepartmentStore_4F_EventScript_Woman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_Woman, MSGBOX_NPC
	end
LilycoveCity_DepartmentStore_4F_EventScript_Youngster::
	msgbox LilycoveCity_DepartmentStore_4F_Text_Youngster, MSGBOX_NPC
	end
LilycoveCity_DepartmentStore_4F_EventScript_ClerkLeft::
	lock
	faceplayer
	msgbox LilycoveCity_DepartmentStore_4F_Text_Stones, MSGBOX_DEFAULT
	setvar VAR_0x8004, 0
	special OpenEmeraldChampionsEvolutionSpecialist
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end
LilycoveCity_DepartmentStore_4F_EventScript_ClerkRight::
	lock
	faceplayer
	msgbox LilycoveCity_DepartmentStore_4F_Text_Tools, MSGBOX_DEFAULT
	setvar VAR_0x8004, 1
	special OpenEmeraldChampionsEvolutionSpecialist
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end
LilycoveCity_DepartmentStore_4F_Text_Stones:
	.string "Every evolution stone, ¥500 each!\n"
	.string "Which one does your partner need?$"
LilycoveCity_DepartmentStore_4F_Text_Tools:
	.string "Special evolution tools cost ¥1000.\n"
	.string "Reusable devices cost ¥3000.\p"
	.string "A reusable device only needs to be\n"
	.string "bought once. Check its description!$"
LilycoveCity_DepartmentStore_4F_Text_Gentleman:
	.string "Everything, every floor.\p"
	.string "RUSTBORO mines six stones and\n"
	.string "SLATEPORT's ships bring ten more.\p"
	.string "Only this store has all of it, tools\n"
	.string "and the reusable devices included.$"
LilycoveCity_DepartmentStore_4F_Text_Woman:
	.string "I found an evolution item on my travels.\n"
	.string "That saved me a shopping trip!$"
LilycoveCity_DepartmentStore_4F_Text_Youngster:
	.string "Evolving opens up new moves and roles.\n"
	.string "I'll visit the free CENTER tutor next!$"
