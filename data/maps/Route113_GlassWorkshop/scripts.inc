Route113_GlassWorkshop_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route113_GlassWorkshop_OnTransition
	.byte 0

Route113_GlassWorkshop_OnTransition:
	setflag FLAG_LANDMARK_GLASS_WORKSHOP
	end

Route113_GlassWorkshop_EventScript_GlassWorker::
	lock
	faceplayer
	checkitem ITEM_SOOT_SACK
	goto_if_eq VAR_RESULT, TRUE, Route113_GlassWorkshop_EventScript_ClaimMilestones
	msgbox Route113_GlassWorkshop_Text_GoCollectAshWithThis, MSGBOX_DEFAULT
	giveitem ITEM_SOOT_SACK
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	msgbox Route113_GlassWorkshop_Text_ExplainSootSack, MSGBOX_DEFAULT
	goto Route113_GlassWorkshop_EventScript_ClaimMilestones

Route113_GlassWorkshop_EventScript_ClaimMilestones:
	checkitem ITEM_MEGA_RING, 1
	copyvar VAR_0x8004, VAR_RESULT
	special ClaimEmeraldChampionsSootMilestone
	goto_if_eq VAR_RESULT, 1, Route113_GlassWorkshop_EventScript_MilestoneBag
	goto_if_eq VAR_RESULT, 2, Route113_GlassWorkshop_EventScript_MilestonePC
	goto_if_eq VAR_RESULT, 3, Route113_GlassWorkshop_EventScript_Marshadow
	goto_if_eq VAR_RESULT, 4, Route113_GlassWorkshop_EventScript_MilestoneNoRoom
	goto_if_eq VAR_RESULT, 5, Route113_GlassWorkshop_EventScript_MilestoneOwned
	goto_if_eq VAR_RESULT, 6, Route113_GlassWorkshop_EventScript_MilestoneNeedRing
	special BufferEmeraldChampionsSootProgress
	msgbox Route113_GlassWorkshop_Text_LetsSeeCollectedAshes, MSGBOX_DEFAULT
	switch VAR_RESULT
	case 0, Route113_GlassWorkshop_EventScript_NextCord
	case 1, Route113_GlassWorkshop_EventScript_NextMarshadow
	case 2, Route113_GlassWorkshop_EventScript_NextMega
	msgbox Route113_GlassWorkshop_Text_AllMilestones, MSGBOX_DEFAULT
Route113_GlassWorkshop_EventScript_BrowseGlass:
	message Route113_GlassWorkshop_Text_WhichGlassItemWoudYouLike
	waitmessage
	pokemartdecoration2 Route113_GlassWorkshop_Decorations
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
Route113_GlassWorkshop_Decorations:
	.2byte DECOR_PRETTY_CHAIR
	.2byte DECOR_PRETTY_DESK
	pokemartlistend

Route113_GlassWorkshop_Text_GoCollectAshWithThis:
	.string "This area is covered in volcanic ash,\n"
	.string "huff-puff!\p"
	.string "I'm specially gifted, huff-puff.\p"
	.string "I make glass out of volcanic ash\n"
	.string "and make items, huff-puff.\p"
	.string "Go collect ashes with this, huff-puff.$"

Route113_GlassWorkshop_Text_ExplainSootSack:
	.string "Just take that SOOT SACK and walk\n"
	.string "through piles of ash, huff-puff.\p"
	.string "And it will fill up with the volcanic ash,\n"
	.string "huff-puff.\p"
	.string "Once you think you've collected a good\n"
	.string "amount, come see me, huff-puff.$"

Route113_GlassWorkshop_Text_LetsSeeCollectedAshes:
	.string "Lifetime soot collected: {STR_VAR_2}.\n"
	.string "Collection progress is never spent!$"

Route113_GlassWorkshop_Text_WhichGlassItemWoudYouLike:
	.string "Would you like some glass furniture?\n"
	.string "The shop lists each price in cash.$"

Route113_GlassWorkshop_Text_FunToBlowGlassFlute:
	.string "Boss only blows glass furniture now.\n"
	.string "The ash keeps the kiln hot, huff-puff.$"


Route113_GlassWorkshop_EventScript_MilestoneBag:
	playfanfare MUS_OBTAIN_ITEM
	msgbox Route113_GlassWorkshop_Text_MilestoneBag, MSGBOX_DEFAULT
	waitfanfare
	goto Route113_GlassWorkshop_EventScript_ClaimMilestones
Route113_GlassWorkshop_EventScript_MilestonePC:
	msgbox Route113_GlassWorkshop_Text_MilestonePC, MSGBOX_DEFAULT
	goto Route113_GlassWorkshop_EventScript_ClaimMilestones
Route113_GlassWorkshop_EventScript_MilestoneOwned:
	msgbox Route113_GlassWorkshop_Text_MilestoneOwned, MSGBOX_DEFAULT
	goto Route113_GlassWorkshop_EventScript_ClaimMilestones
Route113_GlassWorkshop_EventScript_MilestoneNoRoom:
	msgbox Route113_GlassWorkshop_Text_MilestoneNoRoom, MSGBOX_DEFAULT
	release
	end
Route113_GlassWorkshop_EventScript_MilestoneNeedRing:
	msgbox Route113_GlassWorkshop_Text_MilestoneNeedRing, MSGBOX_DEFAULT
	goto Route113_GlassWorkshop_EventScript_BrowseGlass
Route113_GlassWorkshop_EventScript_Marshadow:
	msgbox Route113_GlassWorkshop_Text_Marshadow, MSGBOX_DEFAULT
	goto Route113_GlassWorkshop_EventScript_ClaimMilestones
Route113_GlassWorkshop_EventScript_NextCord:
	msgbox Route113_GlassWorkshop_Text_NextCord, MSGBOX_DEFAULT
	goto Route113_GlassWorkshop_EventScript_BrowseGlass
Route113_GlassWorkshop_EventScript_NextMarshadow:
	msgbox Route113_GlassWorkshop_Text_NextMarshadow, MSGBOX_DEFAULT
	goto Route113_GlassWorkshop_EventScript_BrowseGlass
Route113_GlassWorkshop_EventScript_NextMega:
	msgbox Route113_GlassWorkshop_Text_NextMega, MSGBOX_DEFAULT
	goto Route113_GlassWorkshop_EventScript_BrowseGlass
Route113_GlassWorkshop_Text_NextCord:
	.string "At 100 lifetime soot, I'll give you\n"
	.string "a reusable LINKING CORD!$"
Route113_GlassWorkshop_Text_NextMarshadow:
	.string "At 250 lifetime soot, I can show you\n"
	.string "a secret in the ash grass!$"
Route113_GlassWorkshop_Text_NextMega:
	.string "At 500 lifetime soot, a HOUNDOOMINITE\n"
	.string "is yours. Keep exploring, huff-puff!$"
Route113_GlassWorkshop_Text_AllMilestones:
	.string "You have earned every collection gift!\n"
	.string "My glass furniture is sold for cash.$"

Route113_GlassWorkshop_Text_MilestoneBag:
	.string "You've earned a {STR_VAR_1}!\n"
	.string "It's in your BAG, huff-puff.$"
Route113_GlassWorkshop_Text_MilestonePC:
	.string "You've earned a {STR_VAR_1}!\n"
	.string "Your BAG is full, so it's in your PC.$"
Route113_GlassWorkshop_Text_MilestoneOwned:
	.string "You already own a {STR_VAR_1}.\n"
	.string "Your one-time bonus is ¥{STR_VAR_2}!$"

Route113_GlassWorkshop_Text_MilestoneNeedRing:
	.string "The ash hides a Mega Stone, huff-puff.\n"
	.string "Come back wearing a MEGA RING.$"

Route113_GlassWorkshop_Text_MilestoneNoRoom:
	.string "Your collection gift is safe with me.\n"
	.string "Make room in your BAG or item PC,\l"
	.string "then come back, huff-puff!$"
Route113_GlassWorkshop_Text_Marshadow:
	.string "250 soot! You've stirred up a shadow\n"
	.string "that follows the drifting ash.\p"
	.string "That's MARSHADOW! It can now appear\n"
	.string "in ROUTE 113's wild grass.\p"
	.string "The route sign records this discovery.\n"
	.string "Once caught, it stops appearing here.$"
Route113_GlassWorkshop_EventScript_NinjaBoy::
	msgbox Route113_GlassWorkshop_Text_FunToBlowGlassFlute, MSGBOX_NPC
	end
