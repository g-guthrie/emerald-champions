@ Note: data/scripts/berry_blender.inc is for the same map, so shares local IDs

@ Emerald Champions: Pokemon Contests are retired. src/contest.c, the Contest
@ Halls and the link-contest code are all gone from this tree, so everything in
@ this room that actually ran a Contest -- the receptionists' entry flow, the
@ five Contest Hall warps, the link-contest handshake, the artist who paints the
@ winner, and the painting viewer on the walls -- has been removed rather than
@ left calling missing code.
@ The room itself stays open and reachable: the Berry Blenders (still driven by
@ data/scripts/berry_blender.inc), every flavour NPC and the Pokeblock Case girl
@ all behave exactly as before, and the two receptionists explain that Contests
@ are not being held.

LilycoveCity_ContestLobby_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, LilycoveCity_ContestLobby_OnTransition
	.byte 0

@ The contest reporter and the Blend Master were both shown by PokeNews/TV, which
@ is also gone, so they stay hidden and the Blend Master's replacement NPC (an
@ ordinary Berry Blender partner) is shown in his place. This is the same state
@ the original scripts produced whenever PokeNews had not featured him.
LilycoveCity_ContestLobby_OnTransition:
	setflag FLAG_HIDE_LILYCOVE_CONTEST_HALL_REPORTER
	setflag FLAG_HIDE_LILYCOVE_CONTEST_HALL_BLEND_MASTER
	clearflag FLAG_HIDE_LILYCOVE_CONTEST_HALL_BLEND_MASTER_REPLACEMENT
	end

@ Kept so the map's (permanently hidden) reporter object has a valid script.
LilycoveCity_ContestLobby_EventScript_Reporter::
	msgbox LilycoveCity_ContestLobby_Text_NoContestsToReport, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_ContestReceptionist::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_ContestsNotBeingHeld, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_LinkContestReceptionist::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_LinkContestsNotBeingHeld, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_BlackBelt::
	msgbox LilycoveCity_ContestLobby_Text_MasterRankHereICome, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_Girl::
	msgbox LilycoveCity_ContestLobby_Text_WholeVarietyOfPokemonHere, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_Artist::
	msgbox LilycoveCity_ContestLobby_Text_ContestFeastForEyes, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_FatMan::
	msgbox LilycoveCity_ContestLobby_Text_ToughContestIsExtreme, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_Fisherman::
	msgbox LilycoveCity_ContestLobby_Text_LavishedCareOnMon, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_NinjaBoy::
	msgbox LilycoveCity_ContestLobby_Text_MadePokeblocksWithFamily, MSGBOX_NPC
	end

@ The six wall frames used to open the contest-painting viewer.
LilycoveCity_ContestLobby_EventScript_ContestWinner1::
LilycoveCity_ContestLobby_EventScript_ContestWinner2::
LilycoveCity_ContestLobby_EventScript_ContestWinner3::
LilycoveCity_ContestLobby_EventScript_ContestWinner4::
LilycoveCity_ContestLobby_EventScript_ContestWinner5::
LilycoveCity_ContestLobby_EventScript_ContestWinner6::
	lockall
	msgbox LilycoveCity_ContestLobby_Text_PaintingNotOnDisplay, MSGBOX_SIGN
	releaseall
	end

LilycoveCity_ContestLobby_EventScript_Blender3Boy::
	msgbox BerryBlender_Text_LetsGetBlendingAlready, MSGBOX_NPC
	end

LilycoveCity_ContestLobby_EventScript_Blender3Girl::
	msgbox BerryBlender_Text_WhatKindOfPokeblockWillIGet, MSGBOX_NPC
	end

@ The Berry Blender speed records were kept by the contest code.
LilycoveCity_ContestLobby_EventScript_BerryBlenderSpeedRecords::
	lockall
	msgbox LilycoveCity_ContestLobby_Text_NoBlenderRecords, MSGBOX_SIGN
	releaseall
	end

LilycoveCity_ContestLobby_EventScript_LinkContestResults::
	lockall
	msgbox LilycoveCity_ContestLobby_Text_NoLinkContestResults, MSGBOX_SIGN
	releaseall
	end

@ The Blend Master and his onlookers are hidden by OnTransition above, so these
@ are unreachable in play. Their lines were previously pulled from the donor
@ tree's berry_blender.inc, which this engine's copy does not carry, so the text
@ now lives at the bottom of this file.
LilycoveCity_ContestLobby_EventScript_BlendMaster::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_BlendWithTheBlendMaster, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker1::
	lock
	msgbox LilycoveCity_ContestLobby_Text_WhoaAwesome, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker2::
	lock
	msgbox LilycoveCity_ContestLobby_Text_WickedlyFast, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker3::
	lock
	msgbox LilycoveCity_ContestLobby_Text_WhatAnExpert, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker4::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_MadeAmazingPokeblocksWithMaster, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker5::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_QualitiesOfBlendMaster, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection
	end

LilycoveCity_ContestLobby_EventScript_BlendMasterOnlooker6::
	lock
	faceplayer
	msgbox LilycoveCity_ContestLobby_Text_MasterWorksOnSkillsInMountains, MSGBOX_DEFAULT
	goto LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection
	end

LilycoveCity_ContestLobby_EventScript_FaceOriginalDirection::
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

LilycoveCity_ContestLobby_EventScript_LittleGirl:: @ 821ACF1
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_POKEBLOCK_CASE, LilycoveCity_ContestLobby_EventScript_LittleGirlHaveCase
	msgbox LilycoveCity_ContestLobby_Text_LadyGaveMePokeblockCase, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_EventScript_LittleGirlHaveCase:: @ 821AD06
	msgbox LilycoveCity_ContestLobby_Text_MakePokeblocksDifferentBerries, MSGBOX_DEFAULT
	release
	end

LilycoveCity_ContestLobby_Text_ContestsNotBeingHeld:
	.string "Welcome to the CONTEST HALL!\p"
	.string "I'm afraid we aren't holding any\n"
	.string "Contests just now.\p"
	.string "The halls are dark and the judges\n"
	.string "have all gone home.\p"
	.string "Do enjoy the BERRY BLENDERS while\n"
	.string "you're here!$"

LilycoveCity_ContestLobby_Text_LinkContestsNotBeingHeld:
	.string "This counter is for Contests held\n"
	.string "with other TRAINERS.\p"
	.string "With no Contests being held, I'm\n"
	.string "afraid there's nothing to link up.\p"
	.string "Sorry to disappoint you!$"

LilycoveCity_ContestLobby_Text_NoContestsToReport:
	.string "There's no Contest to report on,\n"
	.string "so there's no story for me here.$"

LilycoveCity_ContestLobby_Text_PaintingNotOnDisplay:
	.string "The frame is empty.\p"
	.string "The painting that belongs here is\n"
	.string "not on display.$"

LilycoveCity_ContestLobby_Text_NoBlenderRecords:
	.string "It's the BERRY BLENDER record board.\p"
	.string "No records have been posted on it.$"

LilycoveCity_ContestLobby_Text_NoLinkContestResults:
	.string "It's the Contest results board.\p"
	.string "There are no results posted on it.$"

@ Used by the link Berry Blender in data/scripts/berry_blender.inc, which is still
@ live. It used to be defined in the contest-hall scripts, which are gone, so it
@ lives here with the rest of this map's text.
LilycoveCity_ContestLobby_Text_PleaseDecideLinkLeader:
	.string "Please decide which of you will\n"
	.string "become the GROUP Leader.\p"
	.string "The other players must then choose\n"
	.string "“JOIN GROUP.”$"

LilycoveCity_ContestLobby_Text_BlendWithTheBlendMaster:
	.string "Hello, hello! Would you care to\n"
	.string "blend BERRIES with me?$"

LilycoveCity_ContestLobby_Text_WhoaAwesome:
	.string "Whoa! Awesome!$"

LilycoveCity_ContestLobby_Text_WickedlyFast:
	.string "That's wickedly fast!$"

LilycoveCity_ContestLobby_Text_WhatAnExpert:
	.string "What an expert!$"

LilycoveCity_ContestLobby_Text_MadeAmazingPokeblocksWithMaster:
	.string "I made some amazing POKéBLOCKS\n"
	.string "with the BLEND MASTER!$"

LilycoveCity_ContestLobby_Text_QualitiesOfBlendMaster:
	.string "Speed, timing, and a feel for the\n"
	.string "BERRIES.\p"
	.string "Those are the qualities that make\n"
	.string "a BLEND MASTER.$"

LilycoveCity_ContestLobby_Text_MasterWorksOnSkillsInMountains:
	.string "They say the MASTER works on his\n"
	.string "skills deep in the mountains.$"

LilycoveCity_ContestLobby_Text_LadyGaveMePokeblockCase: @ 821AD10
	.string "Yippee!\p"
	.string "The lady at the reception counter\n"
	.string "gave me a case for {POKEBLOCK}s!$"

LilycoveCity_ContestLobby_Text_MakePokeblocksDifferentBerries: @ 821AD55
	.string "Make {POKEBLOCK}S and put them in there.\p"
	.string "When you make a {POKEBLOCK}, everyone\n"
	.string "has to put in a different Berry.$"

LilycoveCity_ContestLobby_Text_YourPokemonSpurredMeToPaint: @ 821ADB9
	.string "Congratulations!\p"
	.string "I did a painting of your Pokémon to\n"
	.string "commemorate its victory…\p"
	.string "Well, your Pokémon's appeals were\n"
	.string "so fantastic, it spurred me into\l"
	.string "painting better than I usually do.\l"
	.string "Look, see?$"

LilycoveCity_ContestLobby_Text_ShouldITakePaintingToMuseum: @ 821AE78
	.string "What do you think? I'm confident in\n"
	.string "what I've done, but do you like it?\p"
	.string "A work of this caliber, it wouldn't look\n"
	.string "out of place in an art museum.\p"
	.string "Huh? An art museum really is looking\n"
	.string "for paintings?\p"
	.string "Do you think I should take this there?$"

LilycoveCity_ContestLobby_Text_IllTakePaintingToMuseum: @ 821AF63
	.string "What, really? Then, sure, I will take\n"
	.string "this painting there right now.\p"
	.string "I'll give it a proper title, too.\p"
	.string "I hope they'll like it and consider\n"
	.string "exhibiting this.\p"
	.string "Please check if they did accept this.\n"
	.string "Thank you!$"

LilycoveCity_ContestLobby_Text_TakeMementoOfPainting: @ 821B030
	.string "Oh, that's right!\p"
	.string "As a memento of me painting your\n"
	.string "Pokémon, please take this.$"

LilycoveCity_ContestLobby_Text_ReceivedARibbon: @ 821B07E
	.string "{PLAYER} received a Ribbon.$"

LilycoveCity_ContestLobby_Text_PutTheRibbonOnMon: @ 821B094
	.string "{PLAYER} put the Ribbon on\n"
	.string "{STR_VAR_1}.$"

LilycoveCity_ContestLobby_Text_OkaySeeYou: @ 821B0AD
	.string "Okay, see you!$"

LilycoveCity_ContestLobby_Text_TakeHomeButIdLikeToTakeToMuseum: @ 821B0BC
	.string "Oh… Then, I guess I'll just take\n"
	.string "this home with me…\p"
	.string "But, you know, I would like to take\n"
	.string "this to the art museum… Okay?$"

LilycoveCity_ContestLobby_Text_FineThatsTheWayItIs: @ 821B132
	.string "Oh, fine, that's the way it is.\n"
	.string "I will hang this in my own house.\p"
	.string "I'll just have to try harder next time.\n"
	.string "Well, be seeing you.$"

LilycoveCity_ContestLobby_Text_MasterRankHereICome: @ 821B1B1
	.string "Hoo, boy… Master Rank Contests,\n"
	.string "here I come.\p"
	.string "The world will know that my dearest\n"
	.string "Pokémon is the cutest being in all\l"
	.string "existence. The time has come!\l"
	.string "Uheheheh.$"

LilycoveCity_ContestLobby_Text_WholeVarietyOfPokemonHere: @ 821B24D
	.string "You can see a whole variety of\n"
	.string "Pokémon here.\p"
	.string "That's why I make this place a regular\n"
	.string "part of my daily stroll.$"

LilycoveCity_ContestLobby_Text_ContestFeastForEyes: @ 821B2BA
	.string "Wow, coming out to a Contest is\n"
	.string "a feast for these eyes!\p"
	.string "Would you look at all the Pokémon\n"
	.string "that just scream to be painted?$"

LilycoveCity_ContestLobby_Text_ToughContestIsExtreme: @ 821B334
	.string "The Toughness Contest is like\n"
	.string "extreme, man!\p"
	.string "Those muscular appeals…\n"
	.string "Cascading sweat… I swoon!$"

LilycoveCity_ContestLobby_Text_LavishedCareOnMon: @ 821B392
	.string "Day in and day out, I lavished my care\n"
	.string "on this Pokémon.\p"
	.string "Its condition is peaking.\n"
	.string "Today, victory is mine!$"

LilycoveCity_ContestLobby_Text_MadePokeblocksWithFamily: @ 821B3FC
	.string "I made {POKEBLOCK}S with Mom, Dad, and\n"
	.string "Big Sister. They turned out great!\p"
	.string "I bet you can make smoother, better\n"
	.string "{POKEBLOCK}s if you have more people.$"

