LilycoveCity_LilycoveMuseum_2F_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, LilycoveCity_LilycoveMuseum_2F_OnFrame
	.byte 0

LilycoveCity_LilycoveMuseum_2F_OnFrame:
	map_script_2 VAR_LILYCOVE_MUSEUM_2F_STATE, 0, LilycoveCity_LilycoveMuseum_2F_EventScript_ShowExhibitHall
	.2byte 0

LilycoveCity_LilycoveMuseum_2F_EventScript_ShowExhibitHall::
	lockall
	applymovement LOCALID_MUSEUM_2F_CURATOR, Common_Movement_FacePlayer
	applymovement LOCALID_PLAYER, LilycoveCity_LilycoveMuseum_2F_Movement_PlayerWalkInPlaceLeft
	waitmovement 0
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_ThisIsExhibitHall, MSGBOX_SIGN
	applymovement LOCALID_MUSEUM_2F_CURATOR, LilycoveCity_LilycoveMuseum_2F_Movement_FaceExhibitHall
	waitmovement 0
	applymovement LOCALID_PLAYER, LilycoveCity_LilycoveMuseum_2F_Movement_FaceExhibitHall
	waitmovement 0
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_ExplainExhibitHall, MSGBOX_SIGN
	applymovement LOCALID_MUSEUM_2F_CURATOR, Common_Movement_FacePlayer
	waitmovement 0
	applymovement LOCALID_PLAYER, LilycoveCity_LilycoveMuseum_2F_Movement_PlayerWalkInPlaceLeft
	waitmovement 0
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_PleaseObtainPaintingsForExhibit, MSGBOX_SIGN
#ifdef UBFIX
	setvar VAR_LILYCOVE_MUSEUM_2F_STATE, 1
#else
	copyvar VAR_LILYCOVE_MUSEUM_2F_STATE, 1, warn=FALSE
#endif
	releaseall
	end

LilycoveCity_LilycoveMuseum_2F_Movement_PlayerWalkInPlaceLeft:
	walk_in_place_faster_left
	step_end

LilycoveCity_LilycoveMuseum_2F_Movement_FaceExhibitHall:
	face_up
	delay_16
	step_end

LilycoveCity_LilycoveMuseum_2F_EventScript_Curator::
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_WishToFillExhibit, MSGBOX_NPC
	end

LilycoveCity_LilycoveMuseum_2F_EventScript_CutePainting::
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_ItsPinkPictureFrame, MSGBOX_SIGN
	end

LilycoveCity_LilycoveMuseum_2F_EventScript_ToughPainting::
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_ItsYellowPictureFrame, MSGBOX_SIGN
	end

LilycoveCity_LilycoveMuseum_2F_EventScript_CoolPainting::
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_ItsRedPictureFrame, MSGBOX_SIGN
	end

LilycoveCity_LilycoveMuseum_2F_EventScript_BeautyPainting::
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_ItsBluePictureFrame, MSGBOX_SIGN
	end

LilycoveCity_LilycoveMuseum_2F_EventScript_SmartPainting::
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_ItsGreenPictureFrame, MSGBOX_SIGN
	end

LilycoveCity_LilycoveMuseum_2F_EventScript_Girl::
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_NewPaintingsSurprisedMe, MSGBOX_NPC
	end

LilycoveCity_LilycoveMuseum_2F_EventScript_ExpertM::
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_NewPaintingsRatherAmusing, MSGBOX_NPC
	end

LilycoveCity_LilycoveMuseum_2F_EventScript_RichBoy::
	msgbox LilycoveCity_LilycoveMuseum_2F_Text_ThesePaintingsOfYourPokemon, MSGBOX_NPC
	end

LilycoveCity_LilycoveMuseum_2F_Text_ThisIsExhibitHall:
	.string "This is our special exhibit hall.$"

LilycoveCity_LilycoveMuseum_2F_Text_ExplainExhibitHall:
	.string "As you can plainly see, there is not\n"
	.string "a single painting on exhibit.\p"
	.string "Here, I don't wish to exhibit works of\n"
	.string "so-called classical art.\p"
	.string "Such classical works you can see on\n"
	.string "our ground floor.\p"
	.string "I wish to exhibit work that is far\n"
	.string "different from the classics.\p"
	.string "Art, after all, is not restricted to old,\n"
	.string "famous works!\p"
	.string "This exhibit hall, I wish to fill it with…\p"
	.string "Modern and vibrant artworks of\n"
	.string "POKéMON seemingly ready to spring\l"
	.string "forth into glorious life!$"

LilycoveCity_LilycoveMuseum_2F_Text_PleaseObtainPaintingsForExhibit:
	.string "I beg your pardon. I didn't intend\n"
	.string "to monopolize the conversation.\p"
	.string "Now, as you are young, and yet\n"
	.string "obviously well traveled…\p"
	.string "You must encounter lively POKéMON\n"
	.string "and works of art depicting them.\p"
	.string "If you were to come across such\n"
	.string "a painting, may we ask you to obtain\l"
	.string "the artist's permission to exhibit\l"
	.string "it here?$"

LilycoveCity_LilycoveMuseum_2F_Text_WishToFillExhibit:
	.string "I wish to fill this exhibit hall with\n"
	.string "modern and vibrant paintings of\l"
	.string "POKéMON.$"

LilycoveCity_LilycoveMuseum_2F_Text_ItsPinkPictureFrame:
	.string "It's a picture frame with pink-colored\n"
	.string "adornments.$"

LilycoveCity_LilycoveMuseum_2F_Text_ItsYellowPictureFrame:
	.string "It's a picture frame with\n"
	.string "yellow-colored adornments.$"

LilycoveCity_LilycoveMuseum_2F_Text_ItsBluePictureFrame:
	.string "It's a picture frame with\n"
	.string "blue-colored adornments.$"

LilycoveCity_LilycoveMuseum_2F_Text_ItsRedPictureFrame:
	.string "It's a picture frame with\n"
	.string "red-colored adornments.$"

LilycoveCity_LilycoveMuseum_2F_Text_ItsGreenPictureFrame:
	.string "It's a picture frame with\n"
	.string "green-colored adornments.$"

LilycoveCity_LilycoveMuseum_2F_Text_NewPaintingsSurprisedMe:
	.string "I've been away awhile, so the new\n"
	.string "paintings up here surprised me.\p"
	.string "I wish someone would paint my POKéMON\n"
	.string "this pretty.$"

LilycoveCity_LilycoveMuseum_2F_Text_NewPaintingsRatherAmusing:
	.string "Well… So this is where they show\n"
	.string "the topical POKéMON paintings.\p"
	.string "I see, these new paintings are indeed\n"
	.string "rather amusing.$"

LilycoveCity_LilycoveMuseum_2F_Text_ThesePaintingsOfYourPokemon:
	.string "Wow, is that right?\n"
	.string "These paintings are of your POKéMON.\p"
	.string "Isn't that just the ultimate joy for\n"
	.string "a TRAINER?$"

