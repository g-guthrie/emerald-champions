LilycoveCity_ContestLobby_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, LilycoveCity_ContestLobby_OnTransition
	.byte 0

@ The hobby wing. Contests, Pokeblocks and the Berry Blender are retired; the counter
@ reads harvest pouches and the hall behind it is closed. The blender objects are in
@ data/scripts/berry_blender.

LilycoveCity_ContestLobby_OnTransition:
	call LilycoveCity_ContestLobby_EventScript_TryShowBlendMaster
	end

LilycoveCity_ContestLobby_EventScript_TryShowBlendMaster::
	clearflag FLAG_HIDE_LILYCOVE_CONTEST_HALL_BLEND_MASTER_REPLACEMENT
	setflag FLAG_HIDE_LILYCOVE_CONTEST_HALL_BLEND_MASTER
	return

LilycoveCity_ContestLobby_EventScript_ContestReceptionist::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_HarvestCounter, MSGBOX_DEFAULT
	setvar VAR_0x8009, 0
	call EmeraldChampions_EventScript_HarvestMenu
	release
	end

LilycoveCity_ContestLobby_EventScript_LinkContestReceptionist::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_HallClosed, MSGBOX_DEFAULT
	release
	end

@ For the doors at the back of the lobby once the hall warps are gone.
LilycoveCity_ContestLobby_EventScript_ContestHallClosed::
	lockall
	msgbox LilycoveCity_ContestLobby_Text_HallClosed, MSGBOX_SIGN
	releaseall
	end

LilycoveCity_ContestLobby_EventScript_BlackBelt::
	msgbox LilycoveCity_ContestLobby_Text_MasterRankHereICome, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_Girl::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_WholeVarietyOfPokemonHere, MSGBOX_DEFAULT
	call Common_EventScript_EmeraldChampionsStylistServices
	release
	end

LilycoveCity_ContestLobby_EventScript_Artist::
	msgbox LilycoveCity_ContestLobby_Text_PaintingBerryTrees, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_FatMan::
	msgbox LilycoveCity_ContestLobby_Text_HaulingSoil, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_Fisherman::
	msgbox LilycoveCity_ContestLobby_Text_LavishedCareOnTree, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_NinjaBoy::
	msgbox LilycoveCity_ContestLobby_Text_PickedBerriesWithFamily, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_LittleGirl::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_LittleGirl, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_Blender3Boy::
	msgbox LilycoveCity_ContestLobby_Text_Blender3Boy, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_Blender3Girl::
	msgbox LilycoveCity_ContestLobby_Text_Blender3Girl, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_BlendMaster::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_GrowerPride, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker1::
	lock
	msgbox LilycoveCity_ContestLobby_Text_Onlooker1, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker2::
	lock
	msgbox LilycoveCity_ContestLobby_Text_Onlooker2, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker3::
	lock
	msgbox LilycoveCity_ContestLobby_Text_Onlooker3, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker4::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_Onlooker4, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker5::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_Onlooker5, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker6::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_Onlooker6, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection
	end

LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection::
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

LilycoveCity_ContestLobby_EventScript_ContestWinner1::
LilycoveCity_ContestLobby_EventScript_ContestWinner2::
LilycoveCity_ContestLobby_EventScript_ContestWinner3::
LilycoveCity_ContestLobby_EventScript_ContestWinner4::
LilycoveCity_ContestLobby_EventScript_ContestWinner5::
LilycoveCity_ContestLobby_EventScript_ContestWinner6::
	lockall
	msgbox LilycoveCity_ContestLobby_Text_EmptyFrame, MSGBOX_SIGN
	releaseall
	end

LilycoveCity_ContestLobby_EventScript_BerryBlenderSpeedRecords::
LilycoveCity_ContestLobby_EventScript_LinkContestResults::
	lockall
	msgbox LilycoveCity_ContestLobby_Text_HarvestBoard, MSGBOX_SIGN
	releaseall
	end

LilycoveCity_ContestLobby_Text_HarvestCounter:
	.string "Welcome to the hobby wing.\p"
	.string "The BERRY MASTER on ROUTE 123 pays\n"
	.string "in stones. I can show you what he\l"
	.string "still needs.$"

LilycoveCity_ContestLobby_Text_HallClosed:
	.string "The hall behind me is closed for\n"
	.string "the season.\p"
	.string "The stage has been dark a long\n"
	.string "while now.$"

LilycoveCity_ContestLobby_Text_EmptyFrame:
	.string "An empty frame.\n"
	.string "Nothing hangs in it.$"

LilycoveCity_ContestLobby_Text_HarvestBoard:
	.string "A board of BERRY trees around\n"
	.string "HOENN, with the ripe ones circled.$"

LilycoveCity_ContestLobby_Text_MasterRankHereICome:
	.string "Hoo, boy… One more season and my\n"
	.string "SITRUS tree will outgrow the roof.\p"
	.string "The whole street will know whose\n"
	.string "garden that is. Uheheheh.$"

LilycoveCity_ContestLobby_Text_WholeVarietyOfPokemonHere:
	.string "You can see a whole variety of\n"
	.string "POKéMON here.\p"
	.string "That's why I make this place a\n"
	.string "regular part of my daily stroll.\p"
	.string "I can fix up how yours looks, too,\n"
	.string "if you like.$"

LilycoveCity_ContestLobby_Text_PaintingBerryTrees:
	.string "I came to paint the BERRY trees on\n"
	.string "ROUTE 123.\p"
	.string "A row of ripe ones in the morning\n"
	.string "light. That is my subject.$"

LilycoveCity_ContestLobby_Text_HaulingSoil:
	.string "Hauling soil up to a hill garden is\n"
	.string "like extreme, man!\p"
	.string "Sun on your back, dirt to the elbow…\n"
	.string "I swoon!$"

LilycoveCity_ContestLobby_Text_LavishedCareOnTree:
	.string "Day in and day out, I watered that\n"
	.string "tree by the shore.\p"
	.string "Today it finally ripened.\n"
	.string "Worth every trip!$"

LilycoveCity_ContestLobby_Text_PickedBerriesWithFamily:
	.string "I picked BERRIES with Mom, Dad, and\n"
	.string "Big Sister!\p"
	.string "A tree gives more when more of us\n"
	.string "come back for it.$"

LilycoveCity_ContestLobby_Text_LittleGirl:
	.string "Yippee!\p"
	.string "The lady at the counter counted my\n"
	.string "BERRIES. I picked nine!$"

LilycoveCity_ContestLobby_Text_GrowerPride:
	.string "Forty trees, and I have never lost\n"
	.string "one to a dry summer.\p"
	.string "Pick a tree and it thanks you the\n"
	.string "next time you come by.$"

LilycoveCity_ContestLobby_Text_Onlooker1:
	.string "Whoa! Forty trees!$"

LilycoveCity_ContestLobby_Text_Onlooker2:
	.string "He never loses one.$"

LilycoveCity_ContestLobby_Text_Onlooker3:
	.string "What a grower!$"

LilycoveCity_ContestLobby_Text_Onlooker4:
	.string "I walked his ROUTE 123 row with\n"
	.string "him once. My pouch came home full.$"

LilycoveCity_ContestLobby_Text_Onlooker5:
	.string "A real grower waters before dawn\n"
	.string "and never argues with the weather.$"

LilycoveCity_ContestLobby_Text_Onlooker6:
	.string "He keeps a second row up in the\n"
	.string "mountains, they say.$"

LilycoveCity_ContestLobby_Text_Blender3Boy:
	.string "Let's go pick something already!$"

LilycoveCity_ContestLobby_Text_Blender3Girl:
	.string "I wonder which tree is ripe today.$"
