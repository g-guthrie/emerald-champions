CaveOfOrigin_B1F_MapScripts::
	.byte 0

CaveOfOrigin_B1F_EventScript_Wallace::
	lock
	faceplayer
	msgbox CaveOfOrigin_B1F_Text_WallaceStory, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_CAVE_OF_ORIGIN_WALLACE, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	delay 60
	playse SE_PIN
	applymovement LOCALID_CAVE_OF_ORIGIN_WALLACE, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_CAVE_OF_ORIGIN_WALLACE, Common_Movement_Delay48
	waitmovement 0
	delay 30
	applymovement LOCALID_CAVE_OF_ORIGIN_WALLACE, Common_Movement_FacePlayer
	waitmovement 0
	message CaveOfOrigin_B1F_Text_WhereIsRayquaza
	waitmessage
	goto CaveOfOrigin_B1F_EventScript_WheresRayquaza

CaveOfOrigin_B1F_EventScript_WheresRayquaza::
	multichoice 0, 0, MULTI_WHERES_RAYQUAZA, FALSE
	switch VAR_RESULT
	case 0, CaveOfOrigin_B1F_EventScript_AtCaveOfOrigin
	case 1, CaveOfOrigin_B1F_EventScript_AtMtPyre
	case 2, CaveOfOrigin_B1F_EventScript_AtSkyPillar
	case 3, CaveOfOrigin_B1F_EventScript_DontRemember
	goto CaveOfOrigin_B1F_EventScript_DontRemember
	end

CaveOfOrigin_B1F_EventScript_AtCaveOfOrigin::
	message CaveOfOrigin_B1F_Text_ButWereInCaveOfOrigin
	waitmessage
	goto CaveOfOrigin_B1F_EventScript_WheresRayquaza

CaveOfOrigin_B1F_EventScript_AtMtPyre::
	message CaveOfOrigin_B1F_Text_OldLadyDidntMentionThat
	waitmessage
	goto CaveOfOrigin_B1F_EventScript_WheresRayquaza

CaveOfOrigin_B1F_EventScript_DontRemember::
	message CaveOfOrigin_B1F_Text_CantYouRememberSomehow
	waitmessage
	goto CaveOfOrigin_B1F_EventScript_WheresRayquaza

CaveOfOrigin_B1F_EventScript_AtSkyPillar::
	msgbox CaveOfOrigin_B1F_Text_WellHeadToSkyPillar, MSGBOX_DEFAULT
	closemessage
	playse SE_EXIT
	fadescreenspeed FADE_TO_BLACK, 4
	setflag FLAG_WALLACE_GOES_TO_SKY_PILLAR
	setvar VAR_SOOTOPOLIS_CITY_STATE, 3
	removeobject LOCALID_CAVE_OF_ORIGIN_WALLACE
	clearflag FLAG_HIDE_SKY_PILLAR_WALLACE
	fadescreen FADE_FROM_BLACK
	release
	end

CaveOfOrigin_B1F_Text_WallaceStory:
	.string "Ah, {PLAYER}{KUN}. STEVEN told me\n"
	.string "how far you have traveled.\p"
	.string "I am WALLACE. I once led the\n"
	.string "SOOTOPOLIS GYM; JUAN leads it\l"
	.string "now.\p"
	.string "GROUDON and KYOGRE's powers are\n"
	.string "overwhelming HOENN. Their\l"
	.string "struggle will consume land and\l"
	.string "sea alike.\p"
	.string "The oldest tale tells of a third\n"
	.string "power that stopped their clash:\l"
	.string "RAYQUAZA.\p"
	.string "We need its help again. But I do\n"
	.string "not know where it sleeps...$"

CaveOfOrigin_B1F_Text_WhereIsRayquaza:
	.string "WALLACE: Where might a POKéMON of\n"
	.string "the sky rest?\p"
	.string "Have you heard of any place on\n"
	.string "your travels that could shelter\l"
	.string "RAYQUAZA?$"

CaveOfOrigin_B1F_Text_ButWereInCaveOfOrigin:
	.string "WALLACE: We've searched the CAVE\n"
	.string "OF ORIGIN. There is no sign of\l"
	.string "RAYQUAZA here.\p"
	.string "Perhaps somewhere higher, closer\n"
	.string "to the sky?$"

CaveOfOrigin_B1F_Text_OldLadyDidntMentionThat:
	.string "WALLACE: The elders at MT. PYRE\n"
	.string "told me RAYQUAZA's story, but it\l"
	.string "doesn't rest there.\p"
	.string "Perhaps another place high above\n"
	.string "the sea?$"

CaveOfOrigin_B1F_Text_CantYouRememberSomehow:
	.string "WALLACE: There is an old tower\n"
	.string "called SKY PILLAR, near ROUTE\l"
	.string "131. It reaches high above the\l"
	.string "sea.\p"
	.string "Could that be the place we're\n"
	.string "looking for?$"

CaveOfOrigin_B1F_Text_WellHeadToSkyPillar:
	.string "WALLACE: The SKY PILLAR! Of\n"
	.string "course. A place between the sea\l"
	.string "and sky.\p"
	.string "From ROUTE 128, SURF south, then\n"
	.string "west through ROUTES 129 and 130\l"
	.string "to ROUTE 131.\p"
	.string "SKY PILLAR is on its north side,\n"
	.string "east of PACIFIDLOG. I will open\l"
	.string "the way!$"

