SootopolisCity_House2_MapScripts::
	.byte 0

SootopolisCity_House2_EventScript_MoveSpecialist::
	msgbox SootopolisCity_House2_Text_MoveSpecialist, MSGBOX_NPC
	end

SootopolisCity_House2_Text_MoveSpecialist:
	.string "I once taught DRACO METEOR only to\n"
	.string "DRAGONS that trusted their TRAINER.\p"
	.string "The CENTER specialist now handles\n"
	.string "every legal competitive move.$"

SootopolisCity_House2_EventScript_ExpertF::
	lock
	faceplayer
	msgbox SootopolisCity_House2_Text_DidYouKnowAboutMtPyreOrbs, MSGBOX_YESNO
	call_if_eq VAR_RESULT, YES, SootopolisCity_House2_EventScript_KnowAboutOrbs
	call_if_eq VAR_RESULT, NO, SootopolisCity_House2_EventScript_DontKnowAboutOrbs
	release
	end

SootopolisCity_House2_EventScript_KnowAboutOrbs::
	msgbox SootopolisCity_House2_Text_YesTwoOrbsSideBySide, MSGBOX_DEFAULT
	return

SootopolisCity_House2_EventScript_DontKnowAboutOrbs::
	msgbox SootopolisCity_House2_Text_OughtToVisitAndSee, MSGBOX_DEFAULT
	return

SootopolisCity_House2_Text_DidYouKnowAboutMtPyreOrbs:
	.string "MT. PYRE… I once visited its\n"
	.string "summit and saw the two ORBS in\l"
	.string "the elders' care.\p"
	.string "Have you heard their story?$"

SootopolisCity_House2_Text_YesTwoOrbsSideBySide:
	.string "The elders said those ORBS were\n"
	.string "passed down for generations.\p"
	.string "I still remember how carefully\n"
	.string "they watched over them.$"

SootopolisCity_House2_Text_OughtToVisitAndSee:
	.string "The elders at MT. PYRE can tell\n"
	.string "you. They've kept HOENN's old\l"
	.string "stories alive for so long.$"
