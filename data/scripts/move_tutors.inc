SlateportCity_PokemonFanClub_EventScript_SwaggerTutor::
	lock
	faceplayer
	move_tutor MOVE_SWAGGER, MoveTutor_Text_SwaggerTeach, MoveTutor_Text_SwaggerWhichMon, MoveTutor_EventScript_SwaggerDeclined, MoveTutor_EventScript_SwaggerTaught, FLAG_MOVE_TUTOR_TAUGHT_SWAGGER
	release
	end

MoveTutor_EventScript_SwaggerDeclined::
	msgbox MoveTutor_Text_SwaggerDeclined, MSGBOX_DEFAULT
	release
	end

MoveTutor_EventScript_SwaggerTaught::
	msgbox MoveTutor_Text_SwaggerTaught, MSGBOX_DEFAULT
	release
	end

MoveTutor_EventScript_OpenBox::
	chooseboxmon SELECT_PC_MON_MOVE_TUTOR
	waitstate
	lock
	faceplayer
	return

MoveTutor_EventScript_CanOnlyBeLearnedOnce::
	msgbox MoveTutor_Text_ThisMoveCanOnlyBeLearnedOnce, MSGBOX_YESNO
	return

MoveTutor_AfterChooseBoxMon::
	waitstate
	special CanTeachMoveBoxMon
	waitstate
	return
