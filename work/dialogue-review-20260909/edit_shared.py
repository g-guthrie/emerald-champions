from pathlib import Path
import re,json,textwrap
root=Path('.')
blocks=json.loads(Path('work/dialogue-review-20260909/shared-dialogue.json').read_text())
bylabel={b['label']:b for b in blocks}
changes={}
def settext(label,*pages):
    changes[label]=pages
settext('gText_Birch_MainSpeech',
'This world is full of POKéMON. They share our forests, our seas, and our everyday lives.',
'I study them here in HOENN. Lately, the weather has been changing, and so have their habitats.',
'Every journey brings a new discovery. Even familiar paths can surprise you!',
'Your journey will begin with two partners. Most TRAINERS here battle in pairs, so teamwork matters from the start.',
'Our POKéMON CENTERS will help you prepare. Their TUTORS teach moves, change NATURES, and adjust EVs for free.',
'They can also suggest complete BATTLE SETS. Try your own ideas, too!',
'A free LEVELER brings your party to the current level cap. Each BADGE lets your team reach higher.',
'You can spend your time exploring and learning how your partners work together.',
'HOENN\'s TRAINERS will give you a real challenge. A loss can teach you something a victory cannot.',
'But there is more to this region than its GYMS. Listen to its people. Watch its POKéMON.',
'I wonder what you will discover together!')
settext('gText_ApprenticePleaseTeach13',
'Oh, hi! You there! Can I get you to massage my shoulder?',
'…Yes, there! Ouch, ouch! Oooh, that feels great!',
'My name\'s {STR_VAR_1}. I take karate training, but my body\'s not built to take the abuse…',
'I decided I\'ll battle POKéMON and toughen myself up.',
'You\'re {PLAYER}{KUN}, aren\'t you? I\'ve heard you\'re a tough TRAINER!',
'Listen, can I get you to give me advice?')
practice=[
['Oh, really? You will? Awesome! Wicked! Awoooh!', 'Oh… I\'m sorry… I\'m so happy, I\'m crying…', 'I\'m planning my own practice team. Should I train for Level 50 or Open Level?'],
['Yay! Great!', 'I want to set a goal for my practice team! Level 50 or Open Level?', 'Which would be perfect for me?'],
['Thank you! Here\'s my first question!', 'For my own practice team, should I aim for Level 50 or Open Level?', 'Which is more suitable for me?'],
['Really? I can\'t believe you\'ll advise me! I… I\'m so happy…', 'I need a goal for my practice team, but I can\'t decide…', 'Level 50 or Open Level? Which could someone like me manage?'],
['Okay, I need you to decide something for me.', 'I\'m planning my own practice team. Should I aim for Level 50 or Open Level?', 'Which do you think would suit me, {PLAYER}{KUN}?'],
['Thank you! That\'s more like it! Let\'s start with an easy one!', 'I need a training goal for my team. Level 50 or Open Level?', 'Me being a busy guy, which should I gear up for?'],
['Yay! I knew you\'d have a big heart, {PLAYER}!', 'Drum roll, please! My first question is about my practice team!', 'Level 50 or Open Level? Which suits me more?'],
['Oh… I\'m delighted!', 'Please help me set a goal for my own practice team.', 'Which would be most suitable for me? Level 50 or Open Level?'],
['Hieeeeh! {PLAYER} said yes! I won\'t be able to sleep tonight…', 'Then, here\'s my question! For my practice team, what is right for me?', 'Level 50 or Open Level?'],
['Oh, yeah! That\'s a solid reply! Excellent, I might add!', 'Now for my own practice team… Should I train for Level 50 or Open Level?'],
['Yippee! I\'ll buy you a boat for that!', 'Of course I\'m lying again! But… Thank you, master!', 'Should I train my own team for Level 50 or Open Level? Decide for me, master!'],
['That\'s it! {PLAYER}, you\'ve got the spirit!', 'My practice team needs a destination! I leave it to your inspiration!', 'Level 50 or Open Level there be, which is the one that\'s good for me?'],
['Lucky, yeah, woohoo! Should I pop a question to you?', 'My practice team needs a goal to pursue. Level 50 or Open Level? It\'s up to you!'],
['Thank you. Mighty good of you! …Cough! Cough!', 'Oogh, I have to toughen up quick… My POKéMON need a training goal, too.', 'Level 50 or Open Level? Which should I aim for?'],
['Th-thank you… Please don\'t stare. I\'m getting all flustered.', 'I need a goal for my practice team. Level 50 or Open Level?', 'Which should I attempt?'],
['You really are accepting me? I don\'t wish to celebrate prematurely.', 'If it is true, I apologize. Now, I need your advice.', 'For my own practice team, what would be worthy of me? Level 50 or Open Level?']]
for i,p in enumerate(practice):settext('gText_ApprenticeWhichLevelMode'+str(i),*p)
settext('MatchCall_Text_Scott6',
'SCOTT: Hi, hi, {PLAYER}{KUN}! All eight BADGES! The LEAGUE is waiting for you.',
'If you want another challenge, visit the CHAMPIONS CIRCUIT at the BATTLE FRONTIER.',
'The LILYCOVE ferry opens that route with six BADGES. You\'ve already earned your passage!',
'I hope to see you over there!')
settext('MatchCall_Text_Drake',
'DRAKE: That voice… {PLAYER}, is it? You sound well…',
'The CHAMPIONS CIRCUIT tests how long a crew can keep winning.',
'Here at the LEAGUE, you face us in succession. Two worthy challenges for a seasoned TRAINER.',
'When the sea brings you back, we\'ll be ready.')
settext('MatchCall_Text_May8','MAY: {PLAYER}{KUN}, hi.', 'MT. PYRE is a memorial to POKéMON whose lives have ended.', 'GHOST POKéMON gather there. I wonder if they feel at home among the memories.')
settext('MatchCall_Text_Brendan8','BRENDAN: {PLAYER}!', 'MT. PYRE is a memorial to POKéMON whose lives have ended.', 'There are GHOST POKéMON everywhere. I\'m keeping my voice down while I explore.')
settext('MatchCall_NegativeBattleText3','Hey there, {PLAYER}! It\'s me, {STR_VAR_1}!', 'I just got cleaned in a battle. I need to rethink how my partners work together!')
settext('MatchCall_NegativeBattleText4','Hey, {PLAYER}. {STR_VAR_1} here.', 'I tried another battle yesterday, but I couldn\'t pull out the win.', 'I\'m trying a different lead pair. Okay, catch you later.')
settext('MatchCall_NegativeBattleText10','Oh, {PLAYER}{KUN}, hello… This is {STR_VAR_1}.', 'I lost a battle earlier. I\'m going to review our moves with a TUTOR.', 'See you around.')
settext('gBirchDexRatingText_Cancel','Another time, then! I always enjoy hearing about your discoveries.')
settext('gBirchDexRatingText_LessThan110','Your field notes are really taking shape. This is an impressive POKéDEX!')
settext('gBirchDexRatingText_LessThan130','Try returning to familiar places with a new ROD or a field skill. You may meet different POKéMON!')
settext('gBirchDexRatingText_DexCompleted','Congratulations! Your HOENN POKéDEX is complete!')
settext('MoveTutor_Text_ThisMoveCanOnlyBeLearnedOnce','I can give this lesson only once. Shall we begin?')
settext('EventTicket_Text_SouthernIslandSailBack','Aye, mate, this island has a quiet all its own.', 'Ready to sail back to LILYCOVE?')
settext('gText_FollowerDefault','{STR_VAR_1} pauses to take in the surroundings.')
settext('Text_NothingButPokemonBooks','Every book on the shelf is about POKéMON.')
settext('EmeraldChampions_Text_ExplainBattleSets','A Battle Set prepares moves, Nature, Ability, EVs, and a held item.', 'Transformation items, such as Mega Stones, must be found separately.')
settext('EmeraldChampions_Text_FormGiftFull','There\'s no room in your party or PC. I\'ll keep your partner here until a space opens up.')
settext('Route111_Text_ExplainSecretPower','Any compatible partner can use SECRET POWER in the field.', 'Look for marked trees, thick bushes, and small indents in rock walls.')
settext('Route120_Text_LeonelPostBattle','Travelers with five BADGES have spotted OGERPON in this route\'s grass.', 'It\'s a rare sight. SWEET SCENT can help draw out the local discoveries.')
settext('Route111_Text_BrookePostRematch','I should ask a CENTER TUTOR about moves that give my partners more ways to help each other.')
settext('Route111_Text_WiltonDefeat','My defenses weren\'t enough…')
settext('Route111_Text_BrookeIntro','Every partner has a job. Some make an opening; others charge through it.', 'Which one will you answer first?')
settext('Route115_Text_TimothyDefeat','You set the pace of that match!')
settext('Route119_Text_DonaldIntro','A swarm doesn\'t have to stand still! Mine can slow you down while keeping the pressure on.', 'Try to keep up!')
settext('Route119_Text_DonaldDefeat','My swarm couldn\'t hold you back!')
settext('Route119_Text_DonaldPostBattle','ELECTROWEB slows the opposing pair. I need to make better use of the time that buys.')
settext('Route119_Text_GregIntro','We\'re the MIMIC CIRCLE! I\'ve copied a few tricks from the wind and the light.', 'Let\'s see if they hold up in battle!')
settext('Route119_Text_GregDefeat','My reflections weren\'t enough…')
settext('Route119_Text_GregPostBattle','REFLECT and LIGHT SCREEN soften attacks. TAILWIND helps us act sooner.', 'Putting those tricks together takes practice!')
settext('Route119_Text_KentIntro','See that darting shape between the trees? That\'s my partner warming up!', 'The forest never stands still. Neither do we!')
settext('Route119_Text_KentDefeat','You caught up with us!')
settext('Route119_Text_KentPostBattle','NINJASK gathers Speed, then BATON PASS can hand it to a teammate.', 'The tricky part is choosing when to make the handoff.')
settext('Route119_Text_JacksonDefeat','You handled our charged-up team!')
settext('Route119_Text_JacksonPostBattle','ELECTRIC TERRAIN changes the ground under our feet. The rain changes the weather above it.', 'A RANGER watches both.')
settext('Route119_Text_CatherinePostBattle','Being prepared means having more than one plan. The weather can change before you finish packing!')
settext('Route119_Text_YasuIntro','A ninja watches for an opening. My partners can make one with speed, strength, or a well-timed trick.', 'Will you leave us a gap?')
settext('Route119_Text_YasuDefeat','My techniques fell short…')
settext('Route119_Text_YasuPostBattle','A ninja must learn when to strike and when to retreat. My partners and I still have work to do.')
settext('Route119_Text_TakashiIntro','I trained in this rain until you couldn\'t hear my footsteps.', 'Now let\'s see how quietly my partners can take you by surprise!')
settext('Route119_Text_TakashiDefeat','So much for my surprise attack!')
settext('Route119_Text_TakashiPostBattle','Concealment only lasts until the battle starts. After that, I have to rely on my partners.')
settext('Route119_Text_FabianIntro','My rhythm is thunder, voice, and wind. Every partner adds something to the band.', 'Try to break the chorus!')
settext('Route119_Text_FabianDefeat','That brought the house down!')
settext('Route119_Text_RachelIntro','My parasol is for more than show. My partners know how to make the most of a rainy day.', 'Shall we?')
settext('Route119_Text_RachelDefeat','Oh! My forecast was wrong…')
settext('Route119_Text_RachelPostBattle','A good rain team needs a plan for clear skies, too. I\'ll remember that on my next walk.')
settext('Route120_Text_JenniferIntro','An ABILITY can change a whole battle. Mine have a few surprises for you.', 'Ready to discover them?')
settext('Route120_Text_JenniferDefeat','Your team surprised me!')
settext('Route120_Text_JenniferPostBattle','PRANKSTER gives many status moves priority. Think about what a partner can do with that opening.')
settext('Route120_Text_ClarissaDefeat','You weathered that battle well!')
settext('Route120_Text_ClarissaPostBattle','I love the rain, but I can\'t count on it forever. Even a parasol needs folding sometimes.')
settext('Route123_Text_WendyPostBattle','A good test gives both TRAINERS something to think about. You\'ve certainly given me plenty!')
settext('Route123_Text_BraxtonPostBattle','You did those BADGES proud. Wherever you go next, keep that determination.')
settext('Route127_Text_AthenaDefeat','You got the better of me!')
settext('Route133_Text_WarrenIntro','I like finding my own way to win. MUSHARNA and TORTERRA help me do just that!')
settext('Route134_Text_MarleyIntro','Fast or slow, every partner needs its moment. Mine won\'t let an opening go to waste!')
settext('Route134_Text_MarleyDefeat','Oh! Thoroughly beaten!')
settext('Route134_Text_MarleyPostBattle','A slow partner can still hit hard. The trick is getting it safely into position.')
settext('Route118_Text_DeandreIntro','Go, go, go! Every partner, ready for action!')
settext('Route118_Text_DeandreDefeat','Come in, POKéMON! Is everybody okay?!')
settext('gTVThePokemonBattleSeminarText01','And it used the move {STR_VAR_3} on the {STR_VAR_2}…', 'Let\'s look at the damage options, {STR_VAR_1}!')
settext('gTVThePokemonBattleSeminarText02','The TRAINER\'s {STR_VAR_1} also knew')
settext('gTVThePokemonBattleSeminarText06','Our damage comparison suggests {STR_VAR_1} could hit harder than {STR_VAR_2}.', 'More damage is not always the goal! Accuracy, side effects, and catching a wild POKéMON can change the best choice.', 'Consider the whole field before you choose. Until next time!')
settext('ContestLiveUpdates_Text_RepeatedAppeals','Repeating the same appeals cost it the JUDGE\'s attention.', '{STR_VAR_1} could try varying the routine next time to keep the audience guessing.')
settext('ContestLiveUpdates_Text_ValiantEffortButLost','{STR_VAR_1} finished last, but kept giving the performance everything.', 'There is plenty to learn from that effort. We hope to see a comeback!')
settext('ContestLiveUpdates_Text_LastInBothRounds','It finished last in both rounds of judging.', '{STR_VAR_1} and the {STR_VAR_2} have a fresh challenge: a routine that lets their best qualities shine.')
settext('gText_EmeraldChampionsExplainTools',
'The Poké Vial fully restores your party outside battle.',
'It holds one dose. Every Pokémon Center refills it for free.',
'The Leveler raises your whole party to the current level cap. It never runs out.',
'The Repel Spray keeps wild Pokémon away for 500 steps per use. Fishing, Rock Smash and Sweet Scent still work.',
'The Tutor by the PC helps with moves, Nature, EVs and Battle Sets.',
'The Vendor supplies battle held items for free. Every Center has both services.',
'The Flight Beacon calls a flier once you have the badge for Fly.')
settext('SafariZone_Northeast_Text_Woman','I heard you can see PIKACHU in the southern part of the SAFARI ZONE. I may have wandered too far!')
settext('Route128_Text_WayneIntro','I want to visit EVER GRANDE, so I brought a partner that can use WATERFALL to crest the falls.')
settext('Route128_Text_WaynePostBattle','Awww, phooey! My partner can use WATERFALL, but I still need the SOOTOPOLIS GYM BADGE!')
settext('Route111_Text_MakingRoomUseTMToMakeYourOwn', "I'm making my own room here with SECRET POWER. A partner that can learn it can use it in the field.", 'Want an ULTRA BALL for the road?')
settext('gTVSecretBaseVisitText09',
"Sigh… I've got to hand it to {STR_VAR_1}. Those POKéMON were a formidable team!",
"That the {STR_VAR_2} knew the move {STR_VAR_3}… It shows you what sort of a TRAINER {STR_VAR_1} is.",
"Certainly, it was quite a lesson!")
# Rewrite only each owned label's contiguous string directives.
written=[]
for label,pages in changes.items():
    b=bylabel[label];p=root/b['file'];s=p.read_text()
    pattern=re.compile(r'(?m)^('+re.escape(label)+r'::?[^\n]*\n)((?:[ \t]*\.string[^\n]*\n)+)')
    m=pattern.search(s)
    if not m: raise RuntimeError(label)
    chunks=[]
    for pn,page in enumerate(pages):
        # Keep tokens together; short lines leave room for proportional text and variables.
        lines=textwrap.wrap(page, width=34,break_long_words=False,break_on_hyphens=False)
        for ln,line in enumerate(lines):
            tail='\\n' if ln==0 and len(lines)>1 else ('\\l' if ln<len(lines)-1 else ('\\p' if pn<len(pages)-1 else '$'))
            chunks.append('\t.string "'+line.replace('"','\\"')+tail+'"\n')
    s=s[:m.start(2)]+''.join(chunks)+s[m.end(2):]
    p.write_text(s)
    written.append({'file':b['file'],'label':label,'original_line':b['line'],'new_text':' / '.join(pages)})
p=Path('data/text/birch_speech.inc');s=p.read_text().replace('@ Emerald Champions: Birch\'s monologue is the one moment every new save\n@ passes through before any mechanic bites, so it carries the explainer.\n@ One page of lore, then what is different and where to do it.','@ Introduce HOENN and its doubles journey; CENTER staff explain the tools.')
p.write_text(s)
Path('work/dialogue-review-20260909/shared-changes.json').write_text(json.dumps(written,indent=2,ensure_ascii=False)+'\n')
print('Changed',len(written),'blocks in',len(set(b['file'] for b in written)),'files')
