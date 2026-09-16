@ The Berry Blender is retired. The machines and the people around them stayed;
@ they talk about growing and picking BERRIES now.

BerryBlender_EventScript_BerryBlender1::
	lockall
	msgbox BerryBlender_Text_MachineIdle, MSGBOX_SIGN
	releaseall
	end

BerryBlender_EventScript_BerryBlender2::
	lockall
	msgbox BerryBlender_Text_MachineIdle, MSGBOX_SIGN
	releaseall
	end

BerryBlender_EventScript_BerryBlender3::
	lockall
	msgbox BerryBlender_Text_MachineIdle, MSGBOX_SIGN
	releaseall
	end

BerryBlender_EventScript_BerryBlenderLink::
	lockall
	msgbox BerryBlender_Text_MachineIdle, MSGBOX_SIGN
	releaseall
	end

BerryBlender_EventScript_Blender2Man::
	msgbox BerryBlender_Text_Blender2Man, MSGBOX_NPC
	end

BerryBlender_EventScript_Blender3PokefanF::
	msgbox BerryBlender_Text_Blender3PokefanF, MSGBOX_NPC
	end

BerryBlender_EventScript_Blender2Twin::
	msgbox BerryBlender_Text_Blender2Twin, MSGBOX_NPC
	end

BerryBlender_EventScript_Blender1ExpertM::
	lock
	faceplayer
	msgbox BerryBlender_Text_Blender1ExpertM, MSGBOX_DEFAULT
	release
	end

BerryBlender_Text_MachineIdle:
	.string "An old machine with a crank.\n"
	.string "Nobody has turned it in years.$"

BerryBlender_Text_Blender2Man:
	.string "I walk the ROUTE 123 row every\n"
	.string "morning before the heat.$"

BerryBlender_Text_Blender3PokefanF:
	.string "Pick a tree and it fruits again.\n"
	.string "Leave it alone and it sulks!$"

BerryBlender_Text_Blender2Twin:
	.string "I want a PECHA tree in our yard!$"

BerryBlender_Text_Blender1ExpertM:
	.string "I have grown BERRIES for forty\n"
	.string "years, and I still learn a thing\l"
	.string "from every tree.\p"
	.string "The BERRY MASTER on ROUTE 123\n"
	.string "trades real prizes for a full\l"
	.string "harvest pouch. Go and see him.$"
