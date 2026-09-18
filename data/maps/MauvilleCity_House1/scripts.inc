MauvilleCity_House1_MapScripts:: @ 820F975
	.byte 0

MauvilleCity_House1_EventScript_RockSmashDude:: @ 820F976
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_HM06, MauvilleCity_House1_EventScript_ReceivedRockSmash
	msgbox MauvilleCity_House1_Text_ImRockSmashDudeTakeThis, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_HM06
	msgbox MauvilleCity_House1_Text_ExplainRockSmash, MSGBOX_DEFAULT
	release
	end

MauvilleCity_House1_EventScript_ReceivedRockSmash:: @ 820F9A5
	msgbox MauvilleCity_House1_Text_MonCanFlyOutOfSmashedRock, MSGBOX_DEFAULT
	release
	end

MauvilleCity_House1_Text_ImRockSmashDudeTakeThis: @ 820F9AF
	.string "Woohoo! Call me the Rock Smash\n"
	.string "Dude!\p"
	.string "Your team looks strong. I\n"
	.string "authorize you to use Rock Smash!$"

MauvilleCity_House1_Text_ExplainRockSmash: @ 820FAA9
	.string "Earn the Dynamo Badge, then\n"
	.string "approach a cracked rock with a\l"
	.string "partner that can learn Rock\l"
	.string "Smash.\p"
	.string "You can break it without\n"
	.string "teaching the move. Woohoo!$"

MauvilleCity_House1_Text_MonCanFlyOutOfSmashedRock: @ 820FB67
	.string "Oh, yes, if you smash a rock, a Pokémon\n"
	.string "could come flying out of hiding.\p"
	.string "Sometimes you'll find an item, or even\n"
	.string "a Fossil.\p"
	.string "Woohoo!$"

