Route102_MapScripts::
	.byte 0

Route102_EventScript_LittleBoy::
	msgbox Route102_Text_ImNotVeryTall, MSGBOX_NPC
	end

Route102_EventScript_RouteSignOldale::
	msgbox Route102_Text_RouteSignOldale, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route102_EventScript_RouteSignPetalburg::
	msgbox Route102_Text_RouteSignPetalburg, MSGBOX_SIGN
	goto Common_EventScript_ShowRouteSpecies

Route102_EventScript_Boy::
	goto_if_set FLAG_EC_CAUGHT_SHAYMIN, Route102_EventScript_ShayminCaught
	msgbox Route102_Text_CatchWholeBunchOfPokemon, MSGBOX_NPC
	releaseall
	end

Route102_EventScript_Calvin::
	trainerbattle_double TRAINER_CALVIN_1, Route102_Text_CalvinIntro, Route102_Text_CalvinDefeated, EmeraldChampions_Text_NeedTwoPokemon, Route102_EventScript_CalvinRegisterMatchCallAfterBattle
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, Route102_EventScript_CalvinRematch
	msgbox Route102_Text_CalvinPostBattle, MSGBOX_DEFAULT
	release
	end

Route102_EventScript_CalvinRegisterMatchCallAfterBattle::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	release
	end

Route102_EventScript_CalvinRematch::
	trainerbattle_rematch_double TRAINER_CALVIN_1, Route102_Text_CalvinRematchIntro, Route102_Text_CalvinRematchDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route102_Text_CalvinRematchPostBattle, MSGBOX_AUTOCLOSE
	end

Route102_EventScript_Rick::
	trainerbattle_double TRAINER_RICK, Route102_Text_RickIntro, Route102_Text_RickDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route102_Text_RickPostBattle, MSGBOX_AUTOCLOSE
	end

Route102_EventScript_Tiana::
	trainerbattle_double TRAINER_TIANA, Route102_Text_TianaIntro, Route102_Text_TianaDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route102_Text_TianaPostBattle, MSGBOX_AUTOCLOSE
	end

Route102_EventScript_Allen::
	trainerbattle_double TRAINER_ALLEN, Route102_Text_AllenIntro, Route102_Text_AllenDefeated, EmeraldChampions_Text_NeedTwoPokemon
	msgbox Route102_Text_AllenPostBattle, MSGBOX_AUTOCLOSE
	end

Route102_Text_WatchMeCatchPokemon:
	.string "WALLY: {PLAYER}... POKéMON live in\n"
	.string "tall grass like this, don't they?\p"
	.string "I've never thrown a POKé BALL.\n"
	.string "Will you watch and tell me if\l"
	.string "I do it right?\p"
	.string "...Whoa!$"

Route102_Text_WallyIDidIt:
	.string "WALLY: I did it… It chose to stay.\n"
	.string "It's my POKéMON!$"

Route102_Text_LetsGoBack:
	.string "{PLAYER}, thank you! Let's go back to\n"
	.string "the GYM. I want your dad to see!$"

Route102_Text_ImNotVeryTall:
	.string "I'm…not very tall, so I sink right\n"
	.string "into tall grass.\p"
	.string "The grass goes up my nose and…\n"
	.string "Fwafwafwafwafwa…\p"
	.string "Fwatchoo!$"

Route102_Text_CatchWholeBunchOfPokemon:
	.string "I thought I saw a flower move...\n"
	.string "Then a little green POKéMON ran\l"
	.string "through the grass!\p"
	.string "SHAYMIN! Right here, so close\n"
	.string "to home! It has a 3% chance of\l"
	.string "appearing in the grass.$"

Route102_Text_RouteSignOldale:
	.string "ROUTE 102\n"
	.string "{RIGHT_ARROW} OLDALE TOWN$"

Route102_Text_RouteSignPetalburg:
	.string "ROUTE 102\n"
	.string "{LEFT_ARROW} PETALBURG CITY$"

Route102_EventScript_ShayminCaught::
	msgbox Route102_Text_ShayminCaught, MSGBOX_NPC
	end

Route102_Text_ShayminCaught:
	.string "You caught SHAYMIN! I knew that\n"
	.string "moving flower was a POKéMON.\p"
	.string "Look after our little neighbor!$"
