from pathlib import Path
import re,shutil,json
root=Path(__file__).resolve().parents[2]
changes=[]
def load(f):
 p=root/f; back=root/'work/dialogue-opening-20260910/before'/f
 if not back.exists():back.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,back)
 return p,p.read_text()
def texts(f,updates):
 p,s=load(f)
 for label,text in updates.items():
  pat=r'(^'+re.escape(label)+r':{1,2}\n)(?:\t\.string[^\n]*\n)+'
  assert re.search(pat,s,re.M),label
  chunks=re.split(r'(?<=\\n)|(?<=\\l)|(?<=\\p)',text+'$')
  body=''.join('\t.string "'+t+'"\n' for t in chunks if t)
  s=re.sub(pat,lambda m:m[1]+body,s,count=1,flags=re.M);changes.append((f,label))
 p.write_text(s)
texts('data/maps/Route101/scripts.inc',{'Route101_Text_YouSavedMe':r"PROF. BIRCH: Whew! Thank you!\pI was studying those wild POKéMON\nwhen they suddenly chased me.\pYou're {PLAYER}{KUN}, our new neighbor!\nWhat a way to meet!\pLet's get back to my LAB. I want\nto thank you and your partners.",'Route101_Text_WildPokemonInTallGrass':r"Wild POKéMON live in tall grass.\pWalk through it to find a partner,\nthen use a POKé BALL to catch it."})
texts('data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc',{
'LittlerootTown_ProfessorBirchsLab_Text_BirchAwayOnFieldwork':r"Looking for PROF. BIRCH?\pHe's on ROUTE 101, north of town,\nstudying POKéMON in the wild.\pHe calls it fieldwork. I call it\na good excuse to leave the desk!",
'LittlerootTown_ProfessorBirchsLab_Text_BirchIsntOneForDeskWork':r"The PROF. prefers fieldwork to\npaperwork. Try ROUTE 101, just\lnorth of town.",
'LittlerootTown_ProfessorBirchsLab_Text_YourTwoPartners':r"PROF. BIRCH: You and both of these\nPOKéMON were brave out there.\pI'd like {STR_VAR_1} and\n{STR_VAR_2} to stay with you.\pI think you're going to make\na wonderful team!",
'LittlerootTown_ProfessorBirchsLab_Text_OpeningDex':r"Take this POKéDEX, too. It records\nthe POKéMON you see and catch.\pMy research takes me all over\nHOENN, but I can't be everywhere.\lLet me know what you discover!",
'LittlerootTown_ProfessorBirchsLab_Text_OpeningReady':r"{RIVAL} is studying POKéMON on\nROUTE 103, north of OLDALE TOWN.\pStop at OLDALE's POKéMON CENTER\non the way. Its staff will help\lyou prepare your team for free.\pTry catching a few more partners,\nthen see how your team does\lin a battle with {RIVAL}!",
'LittlerootTown_ProfessorBirchsLab_Text_CountlessPokemonAwait':r"Take ROUTE 102 west from OLDALE\nto PETALBURG CITY. Your dad will\lbe happy to see you at his GYM.\pBring back a few discoveries\nof your own, too!",
})
p,s=load('data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc')
s=s.replace('@ 5: Received pokedex','@ 5: Completed the post-rival send-off\n\t@ 6: Post-rival Great Balls waiting for storage space')
s=s.replace('\tgoto_if_eq VAR_BIRCH_LAB_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex','\tgoto_if_eq VAR_BIRCH_LAB_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex\n\tgoto_if_eq VAR_BIRCH_LAB_STATE, 6, LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedex')
needle='\tgoto_if_eq VAR_EC_OPENING_STATE, EC_OPENING_RESCUE_WON, LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSupplies\n'
s=s.replace(needle,needle+'\tgoto_if_eq VAR_BIRCH_LAB_STATE, 3, LittlerootTown_ProfessorBirchsLab_EventScript_OpeningReminder\n')
s=s.replace('LittlerootTown_ProfessorBirchsLab_EventScript_OpeningSuppliesFull::','LittlerootTown_ProfessorBirchsLab_EventScript_OpeningReminder::\n\tmsgbox LittlerootTown_ProfessorBirchsLab_Text_OpeningReady, MSGBOX_DEFAULT\n\treleaseall\n\tend\n\nLittlerootTown_ProfessorBirchsLab_EventScript_OpeningSuppliesFull::')
s=s.replace('LittlerootTown_ProfessorBirchsLab_EventScript_DebriefBagFull::\n','LittlerootTown_ProfessorBirchsLab_EventScript_DebriefBagFull::\n\t@ Do not retrigger the automatic walk while the player makes room.\n\tsetvar VAR_BIRCH_LAB_STATE, 6\n')
p.write_text(s)
texts('data/maps/OldaleTown/scripts.inc',{'OldaleTown_Text_ThisIsAPokemonMart':r"This is a POKéMON MART.\nJust look for the blue roof!\pThe clerk stocks POTIONS and\nother supplies for your travels.\pHere, take a POTION to try.",'OldaleTown_Text_SavingMyProgress':r"I'm saving my progress before\nI take a rest. Don't forget yours!",'OldaleTown_Text_FinishedSketchingFootprints':r"I finished sketching those rare\nPOKéMON footprints!\pBut it turns out they were only\nmy own footprints…"})
texts('data/maps/OldaleTown_Mart/scripts.inc',{'OldaleTown_Mart_Text_PokeBallsAreSoldOut':r"The clerk is waiting for more\nPOKé BALLS to arrive.\pPROF. BIRCH and the MART guide\noutside have field supplies for\lnew Trainers, though!",'OldaleTown_Mart_Text_ImGoingToBuyPokeBalls':r"POKé BALLS are back in stock!\nI'm going to catch some new\lpartners for my team.",'OldaleTown_Mart_Text_RestoreHPWithPotion':r"A POTION restores a POKéMON's HP,\nbut it won't wake a fainted one.\pTake fainted partners to the\nPOKéMON CENTER for a rest."})
texts('data/maps/LittlerootTown_MaysHouse_1F/scripts.inc',{'RivalsHouse_1F_Text_TooBusyToNoticeVisit':r"{RIVAL} is already packing for\nanother trip into the field.\pI'm glad you two got to meet,\n{PLAYER}{KUN}. It's nice to have\la friend next door."})
texts('data/maps/LittlerootTown/scripts.inc',{'LittlerootTown_Text_OurNewHomeLetsGoInside':r"MOM: {PLAYER}, we're here, honey!\pThat must have been a tiring ride\nin the moving truck.\pWelcome to LITTLEROOT TOWN!\nThis is our new home.\pIt's quiet, but I think we'll\nlike it here. And you have\lyour very own room, {PLAYER}!\pCome on, let's go inside.",'LittlerootTown_Text_ExplainRunningShoes':r"MOM: The shoes came with a note:\p“Hold the B Button to run.\nNow get out there and explore!”",'LittlerootTown_Text_BirchSpendsDaysInLab':r"PROF. BIRCH spends days studying\nin his LAB, then rushes outside\lto do even more research.\pWhen does he find time to go home?"})
p,s=load('data/maps/LittlerootTown/scripts.inc');s=s.replace('Oh! Look at your two new friends!', 'Oh! Look at your new friends!').replace('If anything happens, you can come home.', 'You can always come home to rest.');p.write_text(s)
texts('data/maps/InsideOfTruck/scripts.inc',{'InsideOfTruck_Text_BoxPrintedWithMonLogo':r"The box has a POKéMON logo on it.\pIt's from a moving and delivery\nservice that works with POKéMON."})
texts('data/maps/LittlerootTown_BrendansHouse_1F/scripts.inc',{'PlayersHouse_1F_Text_ReportFromPetalburgGym':r"INTERVIEWER: …And that was our\nreport from PETALBURG GYM."})
texts('data/text/birch_speech.inc',{'gText_Birch_MainSpeech':r"This world is full of POKéMON.\nThey share our forests, our seas,\land our everyday lives.\pI study them here in HOENN.\nLately, the weather has changed,\land so have their habitats.\pEven a familiar path can lead\nto a new discovery!\pYour journey will begin with two\npartners. TRAINERS here battle\lin pairs, so teamwork matters.\pOur POKéMON CENTERS will help you\nprepare your team for free.\pHOENN's TRAINERS are a challenge.\nExplore, try new partners, and\llearn from the battles you lose.\pThere is more to HOENN than its\nGYMS. Listen to its people.\lWatch its POKéMON.\pI wonder what you will discover\ntogether!",'gText_Birch_AreYouReady':r"All right, are you ready?\pYour own adventure is about\nto begin. New friends and\ldiscoveries are waiting!\pCome find me in LITTLEROOT.\nI'd love to show you around."})
p,s=load('data/text/pkmn_center_nurse.inc');s=s.replace('\t.string "Scent still work.\\p"','\t.string "Scent still work.\\p"\n\t.string "Sweet Scent makes rarer local\\n"\n\t.string "species easier to find. Route signs\\l"\n\t.string "explain its legendary chance.\\p"');p.write_text(s)
p,s=load('src/legendary_signs.c');needle='        StringCopy(gStringVar4, sCenterLegendaryLeads[i].lead);';s=s.replace(needle,needle+'\n        if (id == LEGENDARY_SIGN_SHAYMIN && !FlagGet(FLAG_ADVENTURE_STARTED))\n            StringCopy(gStringVar4, COMPOUND_STRING("After you battle the PROF.\'s kid,\\nreturn to BIRCH for your send-off.\\pThen head west to ROUTE 102.\\nSHAYMIN lives in its grass."));');p.write_text(s)
(root/'work/dialogue-opening-20260910/edited-texts.json').write_text(json.dumps(changes,indent=2)+'\n')
