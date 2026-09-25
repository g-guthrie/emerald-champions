SSTidalRooms_MapScripts:: @ 823C9F1
	.byte 0

SSTidalRooms_EventScript_SnatchGiver:: @ 823C9F2
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_TM49, SSTidalRooms_EventScript_ExplainSnatch
	msgbox SSTidalRooms_Text_NotSuspiciousTakeThis, MSGBOX_DEFAULT
	giveitem ITEM_BIG_PEARL
	compare VAR_RESULT, FALSE
	goto_if_eq Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_TM49
	msgbox SSTidalRooms_Text_ExplainSnatch, MSGBOX_DEFAULT
	release
	end

SSTidalRooms_EventScript_ExplainSnatch:: @ 823CA29
	msgbox SSTidalRooms_Text_ExplainSnatch, MSGBOX_DEFAULT
	release
	end

SSTidalRooms_EventScript_Bed:: @ 823CA33
	lockall
	msgbox SSTidalRooms_Text_TakeRestOnBed, MSGBOX_DEFAULT
	closemessage
	call Common_EventScript_OutOfCenterPartyHeal
	call SSTidalRooms_EventScript_ProgessCruiseAfterBed
	releaseall
	end

@ Before the voyage stage the cabin teams only point the way; face the player first.
SSTidalRooms_EventScript_CabinWait:
	lock
	faceplayer
	goto Common_EventScript_FinaleWait

SSTidalRooms_EventScript_Colton:: @ 823CA49
	specialvar VAR_RESULT, GetEmeraldChampionsFinaleStage
	goto_if_lt VAR_RESULT, EC_FINALE_VOYAGE, SSTidalRooms_EventScript_CabinWait
	trainerbattle_double TRAINER_COLTON, SSTidalRooms_Text_ColtonIntro, SSTidalRooms_Text_ColtonDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SSTidalRooms_Text_ColtonPostBattle, MSGBOX_AUTOCLOSE
	end

SSTidalRooms_EventScript_Micah:: @ 823CA60
	specialvar VAR_RESULT, GetEmeraldChampionsFinaleStage
	goto_if_lt VAR_RESULT, EC_FINALE_VOYAGE, SSTidalRooms_EventScript_CabinWait
	trainerbattle_double TRAINER_MICAH, SSTidalRooms_Text_MicahIntro, SSTidalRooms_Text_MicahDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SSTidalRooms_Text_MicahPostBattle, MSGBOX_AUTOCLOSE
	end

SSTidalRooms_EventScript_Thomas:: @ 823CA77
	specialvar VAR_RESULT, GetEmeraldChampionsFinaleStage
	goto_if_lt VAR_RESULT, EC_FINALE_VOYAGE, SSTidalRooms_EventScript_CabinWait
	trainerbattle_double TRAINER_THOMAS, SSTidalRooms_Text_ThomasIntro, SSTidalRooms_Text_ThomasDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SSTidalRooms_Text_ThomasPostBattle, MSGBOX_AUTOCLOSE
	end

SSTidalRooms_EventScript_Jed:: @ 823CA8E
	specialvar VAR_RESULT, GetEmeraldChampionsFinaleStage
	goto_if_lt VAR_RESULT, EC_FINALE_VOYAGE, SSTidalRooms_EventScript_CabinWait
	trainerbattle_double TRAINER_LEA_AND_JED, SSTidalRooms_Text_JedIntro, SSTidalRooms_Text_JedDefeat, SSTidalRooms_Text_JedNotEnoughMons
	msgbox SSTidalRooms_Text_JedPostBattle, MSGBOX_AUTOCLOSE
	end

SSTidalRooms_EventScript_Lea:: @ 823CAA9
	specialvar VAR_RESULT, GetEmeraldChampionsFinaleStage
	goto_if_lt VAR_RESULT, EC_FINALE_VOYAGE, SSTidalRooms_EventScript_CabinWait
	trainerbattle_double TRAINER_LEA_AND_JED, SSTidalRooms_Text_LeaIntro, SSTidalRooms_Text_LeaDefeat, SSTidalRooms_Text_LeaNotEnoughMons
	msgbox SSTidalRooms_Text_LeaPostBattle, MSGBOX_AUTOCLOSE
	end

SSTidalRooms_EventScript_Naomi:: @ 823CADB
	specialvar VAR_RESULT, GetEmeraldChampionsFinaleStage
	goto_if_lt VAR_RESULT, EC_FINALE_VOYAGE, SSTidalRooms_EventScript_CabinWait
	trainerbattle_double TRAINER_NAOMI, SSTidalRooms_Text_NaomiIntro, SSTidalRooms_Text_NaomiDefeat, EmeraldChampions_Text_NeedTwoPokemon
	msgbox SSTidalRooms_Text_NaomiPostBattle, MSGBOX_AUTOCLOSE
	end

SSTidalRooms_Text_TakeRestOnBed: @ 823CAF2
	.string "There's a bed…\n"
	.string "Let's take a rest.$"

SSTidalRooms_Text_ColtonIntro:
	.string "The new Champion, on my voyage!\p"
	.string "A fine team deserves a fine stage.\n"
	.string "Let us put on a doubles exhibition!$"

SSTidalRooms_Text_ColtonDefeat: @ 823CB58
	.string "That was an enjoyable match!$"

SSTidalRooms_Text_ColtonPostBattle: @ 823CB75
	.string "What a voyage this has been!\n"
	.string "I'll tell every port in Hoenn about\p"
	.string "the new Champion's splendid team!$"

SSTidalRooms_Text_MicahIntro:
	.string "Champion, are your friends ready?\p"
	.string "My six partners have been waiting\n"
	.string "for this challenge!$"

SSTidalRooms_Text_MicahDefeat: @ 823CC04
	.string "Your friends are, indeed, strong.$"

SSTidalRooms_Text_MicahPostBattle: @ 823CC26
	.string "Friends need not be human.\n"
	.string "For me, Pokémon are treasured friends!$"

SSTidalRooms_Text_ThomasIntro:
	.string "Champion…\n"
	.string "Did you knock on the door?\p"
	.string "Come in. Let us see how calmly\n"
	.string "you handle a serious doubles match.$"

SSTidalRooms_Text_ThomasDefeat: @ 823CC8A
	.string "A loss is to be accepted without haste\n"
	.string "or panic.$"

SSTidalRooms_Text_ThomasPostBattle: @ 823CCBB
	.string "To be never ruffled in any situation is\n"
	.string "the Gentleman's code of conduct.$"

SSTidalRooms_Text_JedIntro:
	.string "Jed: The Champion is here!\p"
	.string "Our partners move as one.\n"
	.string "Show us how your team works together!$"

SSTidalRooms_Text_JedDefeat: @ 823CD54
	.string "Jed: Sigh…$"

SSTidalRooms_Text_JedPostBattle: @ 823CD5F
	.string "Jed: It's the first time that our lovey-\n"
	.string "dovey power couldn't prevail!\l"
	.string "You must be an awesome Trainer!$"

SSTidalRooms_Text_JedNotEnoughMons: @ 823CDC6
	.string "Jed: You only have one Pokémon?\n"
	.string "Isn't that just too lonesome?$"

SSTidalRooms_Text_LeaIntro:
	.string "Lea: A challenge from the Champion!\p"
	.string "Jed and I battle as a pair.\n"
	.string "Our six partners are ready for you!$"

SSTidalRooms_Text_LeaDefeat: @ 823CE4B
	.string "Lea: Oh, boo!$"

SSTidalRooms_Text_LeaPostBattle: @ 823CE59
	.string "Lea: I can't believe it!\n"
	.string "Our lovey-dovey power failed…\l"
	.string "You must be an awesome Trainer!$"

SSTidalRooms_Text_LeaNotEnoughMons: @ 823CEB0
	.string "Lea: I wanted to battle…\n"
	.string "But you don't even have two Pokémon…$"

SSTidalRooms_Text_NaomiIntro:
	.string "Oh, the Champion! Welcome aboard.\p"
	.string "Tea can wait until after our battle.\n"
	.string "I have six partners to introduce!$"

SSTidalRooms_Text_NaomiDefeat: @ 823CFF4
	.string "I see.\n"
	.string "You're the active sort.$"

SSTidalRooms_Text_NaomiPostBattle: @ 823D013
	.string "A world cruise on a luxury liner has its\n"
	.string "charms, I must say…\p"
	.string "But, I will admit there is an appealing\n"
	.string "side to touring Hoenn by ferry.$"

SSTidalRooms_Text_NotSuspiciousTakeThis: @ 823D098
	.string "Uh… Hi! I'm not acting suspicious!\n"
	.string "You can have this Big Pearl!\p"
	.string "Honestly, I didn't snatch it\n"
	.string "from someone! I'd never do that!$"

SSTidalRooms_Text_ExplainSnatch: @ 823D145
	.string "Sell that Big Pearl at a shop.\n"
	.string "Put it toward your battle supplies.\p"
	.string "If you want to learn Snatch,\n"
	.string "ask the Move Tutor at a Center.\p"
	.string "Just don't try it on me!$"

