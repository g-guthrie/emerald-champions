from pathlib import Path
import re,json
out=Path('work/dialogue-badge1-20260910'); edits=[]
def save(p,s):
 p=Path(p); b=out/'before'/p;b.parent.mkdir(parents=True,exist_ok=True)
 if not b.exists():b.write_bytes(p.read_bytes())
 p.write_text(s)
def text(p,label,value):
 p=Path(p);s=p.read_text();pat=r'(^'+re.escape(label)+r'::?\s*\n)(?:\s*\.string[^\n]*\n)+'
 body=''.join('\t.string "'+part+'"\n' for part in re.findall(r'.*?(?:\\[npl]|\$)',value))
 s,n=re.subn(pat,lambda m:m[1]+body,s,flags=re.M);assert n==1,label;save(p,s);edits.append([str(p),label])
p='data/maps/PetalburgCity/scripts.inc'
text(p,'PetalburgCity_Text_AreYouATrainer',r'Excuse me! You travel with POKéMON,\ndon\'t you?\pI\'m SCOTT. I scout for TRAINERS\nwith a talent for battling.$'.replace("\\'","'"))
text(p,'PetalburgCity_Text_WellMaybeNot',r'No BADGES yet? Then your journey\nis just getting started.\pA strong team takes more than\npower. I watch how partners work\ltogether when a battle gets tough.$')
text(p,'PetalburgCity_Text_ImLookingForTalentedTrainers',r'Heading to RUSTBORO? ROXANNE\nwill give you plenty to think about.\pI\'ll be visiting the TRAINER\'S\nSCHOOL there. Tell me how it goes!$'.replace("\\'","'"))
text(p,'PetalburgCity_Text_FullPartyExplanation',r'Already traveling with six POKéMON?\nYou can still catch a new partner.\pChoose whether to add it to your\nparty and send someone to a BOX,\lor send the new catch to storage.$')
text(p,'PetalburgCity_Text_WaterReflection',r'My face is reflected in the water.\pA hopeful grin one day...\nA worried frown the next.\pWhat do you see in your reflection?$')
p='data/maps/PetalburgCity_WallysHouse/scripts.inc';s=Path(p).read_text()
for who in ['Dad','Mom']:
 label='PetalburgCity_WallysHouse_EventScript_Wallys'+who+'::\n\tlock\n\tfaceplayer\n';assert label in s
 s=s.replace(label,label+'\tgoto_if_lt VAR_PETALBURG_GYM_STATE, 2, PetalburgCity_WallysHouse_EventScript_BeforeWallyCatch\n',1)
s+='\nPetalburgCity_WallysHouse_EventScript_BeforeWallyCatch::\n\tmsgbox PetalburgCity_WallysHouse_Text_BeforeWallyCatch, MSGBOX_DEFAULT\n\trelease\n\tend\n\nPetalburgCity_WallysHouse_Text_BeforeWallyCatch:\n\t.string "Our WALLY is going to stay with\\n"\n\t.string "relatives in VERDANTURF TOWN.\\p"\n\t.string "He hopes to take a POKéMON along.\\n"\n\t.string "He went to ask NORMAN for help.$"\n';save(p,s)
text(p,'PetalburgCity_WallysHouse_Text_ThanksForPlayingWithWally',r'You must be {PLAYER}{KUN}!\nThank you for helping WALLY catch\lhis first POKéMON.\pHe\'s often been too ill to go out.\nSeeing him so excited means a lot.\pHe\'s left to stay with relatives\nin VERDANTURF TOWN. We hope the\lclean air will do him good.$'.replace("\\'","'"))
p='data/maps/PetalburgCity_Gym/scripts.inc'
text(p,'PetalburgCity_Gym_Text_DadYoureHereWithYourPokemon',r'DAD: {PLAYER}! You made it!\nHow was the trip from LITTLEROOT?\pAnd these are your POKéMON...\nSo your own journey has begun.\pI\'m proud of you. Take your time,\nget to know your partners, and\lfind your own way as a TRAINER.$'.replace("\\'","'"))
text(p,'PetalburgCity_Gym_Text_WallyIveNeverCaughtAPokemon',r'WALLY: I\'m going to stay with my\nrelatives in VERDANTURF TOWN.\pI thought a POKéMON could keep me\ncompany... But I\'ve never caught\lone before. I don\'t know how.$'.replace("\\'","'"))
p='data/maps/Route102/scripts.inc'
text(p,'Route102_Text_WatchMeCatchPokemon',r'WALLY: {PLAYER}... POKéMON live in\ntall grass like this, don\'t they?\pI\'ve never thrown a POKé BALL.\nWill you watch and tell me if\lI do it right?\p...Whoa!$'.replace("\\'","'"))
s=Path(p).read_text();s=s.replace('Route102_EventScript_Boy::\n','Route102_EventScript_Boy::\n\tgoto_if_set FLAG_EC_CAUGHT_SHAYMIN, Route102_EventScript_ShayminCaught\n',1)
s+='\nRoute102_EventScript_ShayminCaught::\n\tmsgbox Route102_Text_ShayminCaught, MSGBOX_NPC\n\tend\n\nRoute102_Text_ShayminCaught:\n\t.string "You caught SHAYMIN! I knew that\\n"\n\t.string "moving flower was a POKéMON.\\p"\n\t.string "Look after our little neighbor!$"\n';save(p,s)
p='data/maps/RustboroCity_PokemonSchool/scripts.inc'
text(p,'RustboroCity_PokemonSchool_Text_ScottMetAlreadyStoneBadge',r'SCOTT: There you are!\nHow did your first GYM battle go?\pA STONE BADGE! Well done.\pI wish I\'d seen the battle myself.\nA BADGE shows that you won; I want\lto see how you and your team did it.$'.replace("\\'","'"))
p='data/maps/PetalburgWoods/scripts.inc'
text(p,'PetalburgWoods_Text_HaveYouSeenShroomish',r'Oh, hello! Seen any SHROOMISH?\nI\'ve always loved them.\pI\'m taking research papers back\nto DEVON in RUSTBORO. I thought\lI could spare a little time...$'.replace("\\'","'"))
text(p,'PetalburgWoods_Text_TrainerTipsExperience',r'TRAINER TIPS\pUse the LEVELER to bring partners\nto your current training limit.\pThen prepare their moves, Ability,\nNature, and held item for battle.$')
p='data/maps/RustboroCity_PokemonCenter_1F/scripts.inc'
text(p,'RustboroCity_PokemonCenter_1F_Text_MaleAndFemalePokemon',r'Some POKéMON are male or female.\nOthers have no gender.\pSometimes it even changes how\nthey evolve! Female BURMY become\lWORMADAM; males become MOTHIM.$')
p='data/text/berries.inc'
text(p,'Route104_PrettyPetalFlowerShop_Text_LearnAboutBerries',r'{PLAYER}{KUN}, would you like to\nlearn about BERRIES?$')
text(p,'Route104_PrettyPetalFlowerShop_Text_IntroLearnAboutBerries',r'Your name is {PLAYER}{KUN}?\nThat\'s a nice name.\pWould you like to learn about\nBERRIES?$'.replace("\\'","'"))
(out/'edited-texts.json').write_text(json.dumps(edits,indent=2))
