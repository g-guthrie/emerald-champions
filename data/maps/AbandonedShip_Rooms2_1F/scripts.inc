AbandonedShip_Rooms2_1F_MapScripts::
	.byte 0

AbandonedShip_Rooms2_1F_EventScript_Dan::
	msgbox AbandonedShip_Rooms2_1F_Text_DanPostBattle, MSGBOX_NPC
	end

AbandonedShip_Rooms2_1F_EventScript_Kira::
	msgbox AbandonedShip_Rooms2_1F_Text_KiraPostBattle, MSGBOX_NPC
	end

AbandonedShip_Rooms2_1F_EventScript_Jani::
	trainerbattle_double TRAINER_JANI, AbandonedShip_Rooms2_1F_Text_JaniIntro, AbandonedShip_Rooms2_1F_Text_JaniDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox AbandonedShip_Rooms2_1F_Text_JaniPostBattle, MSGBOX_AUTOCLOSE
	end

AbandonedShip_Rooms2_1F_EventScript_Garrison::
	trainerbattle_double TRAINER_GARRISON, AbandonedShip_Rooms2_1F_Text_GarrisonIntro, AbandonedShip_Rooms2_1F_Text_GarrisonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox AbandonedShip_Rooms2_1F_Text_GarrisonPostBattle, MSGBOX_AUTOCLOSE
	end

AbandonedShip_Rooms2_1F_Text_DanPostBattle:
	.string "DAN: We can't find any treasures…\n"
	.string "I wonder if someone got them already?$"

AbandonedShip_Rooms2_1F_Text_KiraPostBattle:
	.string "KIRA: Where could the treasures be?\p"
	.string "I've already decided what I'm buying\n"
	.string "when we find the treasures!$"

AbandonedShip_Rooms2_1F_Text_JaniIntro:
	.string "I'm not good at swimming,\n"
	.string "but I am good at battles!$"

AbandonedShip_Rooms2_1F_Text_JaniDefeat:
	.string "Oops.\n"
	.string "That didn't go very well.$"

AbandonedShip_Rooms2_1F_Text_JaniPostBattle:
	.string "Walking around barefoot in this ship\n"
	.string "is kind of gross.$"

AbandonedShip_Rooms2_1F_Text_GarrisonIntro:
	.string "Strength and compassion…\n"
	.string "Those are a TRAINER's treasures!$"

AbandonedShip_Rooms2_1F_Text_GarrisonDefeat:
	.string "Ah, there is something about you\n"
	.string "that sparkles.$"

AbandonedShip_Rooms2_1F_Text_GarrisonPostBattle:
	.string "In a cabin somewhere on board,\n"
	.string "I saw something sparkle.$"
