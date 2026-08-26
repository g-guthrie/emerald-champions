Route109_SeashoreHouse_MapScripts:: @ 82693F4
	map_script MAP_SCRIPT_ON_TRANSITION, Route109_SeashoreHouse_OnTransition
	.byte 0

Route109_SeashoreHouse_OnTransition: @ 82693FA
	setflag FLAG_LANDMARK_SEASHORE_HOUSE
	end

Route109_SeashoreHouse_EventScript_Owner:: @ 82693FE
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_6_SODA_POP, Route109_SeashoreHouse_EventScript_AlreadyReceivedSodaPop
	goto_if_set FLAG_DEFEATED_SEASHORE_HOUSE, Route109_SeashoreHouse_EventScript_DefeatedTrainers
	goto_if_set FLAG_TEMP_2, Route109_SeashoreHouse_EventScript_AlreadyGaveIntroduction
	msgbox Route109_SeashoreHouse_Text_SeashoreHouseIntro, MSGBOX_DEFAULT
	setflag FLAG_TEMP_2
	release
	end

Route109_SeashoreHouse_EventScript_AlreadyGaveIntroduction:: @ 8269428
	msgbox Route109_SeashoreHouse_Text_ShowMeSomeHotMatches, MSGBOX_DEFAULT
	release
	end

Route109_SeashoreHouse_EventScript_DefeatedTrainers:: @ 8269432
	msgbox Route109_SeashoreHouse_Text_TakeTheseSodaPopBottles, MSGBOX_DEFAULT
	giveitem ITEM_SODA_POP, 6
	compare VAR_RESULT, FALSE
	goto_if_eq Route109_SeashoreHouse_EventScript_BagFull
	setflag FLAG_RECEIVED_6_SODA_POP
	release
	end

Route109_SeashoreHouse_EventScript_BagFull:: @ 8269456
	msgbox Route109_SeashoreHouse_Text_BagFull, MSGBOX_DEFAULT
	release
	end

Route109_SeashoreHouse_EventScript_AlreadyReceivedSodaPop:: @ 8269460
	showmoneybox 0, 0, 0
	msgbox Route109_SeashoreHouse_Text_WantToBuySodaPop, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Route109_SeashoreHouse_EventScript_BuySodaPop
	msgbox Route109_SeashoreHouse_Text_ThatsTooBad, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

Route109_SeashoreHouse_EventScript_BuySodaPop:: @ 8269484
	checkmoney 300, 0
	compare VAR_RESULT, FALSE
	goto_if_eq Route109_SeashoreHouse_EventScript_NotEnoughMoney
	checkitemspace ITEM_SODA_POP, 1
	compare VAR_RESULT, FALSE
	goto_if_eq Route109_SeashoreHouse_EventScript_NotEnoughSpace
	msgbox Route109_SeashoreHouse_Text_HereYouGo, MSGBOX_DEFAULT
	removemoney 300, 0
	updatemoneybox 0, 0
	giveitem ITEM_SODA_POP
	hidemoneybox
	release
	end

Route109_SeashoreHouse_EventScript_NotEnoughMoney:: @ 82694C8
	msgbox Route109_SeashoreHouse_Text_NotEnoughMoney, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

Route109_SeashoreHouse_EventScript_NotEnoughSpace:: @ 82694D5
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

Route109_SeashoreHouse_EventScript_Dwayne:: @ 82694E2
	trainerbattle_single TRAINER_DWAYNE, Route109_SeashoreHouse_Text_DwayneIntro, Route109_SeashoreHouse_Text_DwayneDefeated, Route109_SeashoreHouse_EventScript_CheckTrainersCompletion
	msgbox Route109_SeashoreHouse_Text_DwaynePostBattle, MSGBOX_AUTOCLOSE
	end

Route109_SeashoreHouse_EventScript_Johanna:: @ 82694FD
	trainerbattle_double TRAINER_JOHANNA, Route109_SeashoreHouse_Text_JohannaIntro, Route109_SeashoreHouse_Text_JohannaDefeated, Route109_SeashoreHouse_Text_JohannaNotEnoughPokemon, Route109_SeashoreHouse_EventScript_CheckTrainersCompletion
	msgbox Route109_SeashoreHouse_Text_JohannaPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_SeashoreHouse_EventScript_Simon:: @ 8269518
	trainerbattle_double TRAINER_SIMON, Route109_SeashoreHouse_Text_SimonIntro, Route109_SeashoreHouse_Text_SimonDefeated, Route109_SeashoreHouse_Text_SimonNotEnoughPokemon, Route109_SeashoreHouse_EventScript_CheckTrainersCompletion
	msgbox Route109_SeashoreHouse_Text_SimonPostBattle, MSGBOX_AUTOCLOSE
	end

Route109_SeashoreHouse_EventScript_CheckTrainersCompletion:: @ 8269533
	goto_if_not_defeated TRAINER_DWAYNE, Route109_SeashoreHouse_EventScript_TrainersNotCompleted
	goto_if_not_defeated TRAINER_JOHANNA, Route109_SeashoreHouse_EventScript_TrainersNotCompleted
	goto_if_not_defeated TRAINER_SIMON, Route109_SeashoreHouse_EventScript_TrainersNotCompleted
	setflag FLAG_DEFEATED_SEASHORE_HOUSE
	release
	end

Route109_SeashoreHouse_EventScript_TrainersNotCompleted:: @ 8269553
	release
	end

Route109_SeashoreHouse_Text_SeashoreHouseIntro: @ 8269555
	.string "I'm the owner of the Seashore House.\n"
	.string "But you can call me Mr. Sea!\p"
	.string "What I love above all is to see hot\n"
	.string "Pokémon battles.\p"
	.string "Let me see that your heart burns hot!\p"
	.string "If you can defeat all the Trainers\n"
	.string "here, I'll reward your efforts.$"

Route109_SeashoreHouse_Text_ShowMeSomeHotMatches: @ 8269635
	.string "Show me some hot matches!\p"
	.string "I run this Seashore House just for\n"
	.string "that reason alone!$"

Route109_SeashoreHouse_Text_TakeTheseSodaPopBottles: @ 8269685
	.string "You're scorching hot!\n"
	.string "Those battles blazed!\l"
	.string "I'm more than just satisfied!\p"
	.string "As thanks for showing me your hot\n"
	.string "streak, I want you to take these.\p"
	.string "It's half a dozen bottles of Soda Pop!$"

Route109_SeashoreHouse_Text_BagFull: @ 826973A
	.string "Oh, but hey, your Bag's jammed full.\n"
	.string "I'll hang on to these for you.$"

Route109_SeashoreHouse_Text_WantToBuySodaPop: @ 826977E
	.string "Want to buy some Soda Pop?\n"
	.string "Pokémon love it!\p"
	.string "Just ¥300 a bottle!\n"
	.string "Buy some!$"

Route109_SeashoreHouse_Text_HereYouGo: @ 82697C8
	.string "Here you go!$"

Route109_SeashoreHouse_Text_NotEnoughMoney: @ 82697D5
	.string "You don't have the money.$"

Route109_SeashoreHouse_Text_ThatsTooBad: @ 82697EF
	.string "No?\n"
	.string "That's too bad.$"

Route109_SeashoreHouse_Text_DwayneIntro: @ 8269803
	.string "My Jynx freezes, Magmar burns,\n"
	.string "and Volcanion brings the steam!\p"
	.string "Let's have a hot battle, matey!$"

Route109_SeashoreHouse_Text_DwayneDefeated: @ 8269867
	.string "That thermal shock beat me!\n"
	.string "I can take the heat, matey!$"

Route109_SeashoreHouse_Text_DwaynePostBattle: @ 826989D
	.string "Jynx chills the Soda Pop,\n"
	.string "Magmar heats the room, and\l"
	.string "Volcanion makes the steam!$"

Route109_SeashoreHouse_Text_JohannaIntro: @ 82698E3
	.string "Hot battles make Pokémon tougher!\p"
	.string "Feebas starts my pressure test.\n"
	.string "Can you stop the engine?$"

Route109_SeashoreHouse_Text_JohannaDefeated: @ 8269947
	.string "You broke through the pressure!$"

Route109_SeashoreHouse_Text_JohannaPostBattle: @ 8269953
	.string "Surf wakes Carkol's Steam Engine\n"
	.string "or hardens Sandygast.\p"
	.string "That's how hot battles toughen us!$"

Route109_SeashoreHouse_Text_JohannaNotEnoughPokemon:
	.string "A pressure test needs two healthy\n"
	.string "Pokémon. Come back prepared!$"

Route109_SeashoreHouse_Text_SimonIntro: @ 8269986
	.string "Tropius and Mudsdale carry me,\n"
	.string "Lapras and Keldeo cross the sea!\p"
	.string "Don't cry when my great team wins!$"

Route109_SeashoreHouse_Text_SimonDefeated: @ 82699C5
	.string "They carried me into a loss…\n"
	.string "But I won't cry!$"

Route109_SeashoreHouse_Text_SimonPostBattle: @ 82699DF
	.string "Tropius starts Tailwind while\n"
	.string "Keldeo races across the field.\p"
	.string "That's a team worth riding with!$"

Route109_SeashoreHouse_Text_SimonNotEnoughPokemon:
	.string "My four riders need two healthy\n"
	.string "Pokémon to race. Come back ready!$"
