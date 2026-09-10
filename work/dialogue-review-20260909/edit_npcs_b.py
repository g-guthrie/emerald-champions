import json
import re
from pathlib import Path

root = Path(__file__).resolve().parents[2]
rows = json.loads((root / 'work/dialogue-review-20260909/npcs_b-dialogue.json').read_text())
by_label = {r['label']: r for r in rows}
edits = {}

def edit(label, paragraphs, reason):
    row = by_label[label]
    path = root / row['file']
    source = path.read_text()
    pattern = re.compile(r'(^' + re.escape(label) + r'::?\n)((?:\t\.string .*\n)+)', re.M)
    match = pattern.search(source)
    assert match, label
    lines = []
    for i, paragraph in enumerate(paragraphs):
        parts = paragraph.split('|')
        for j, part in enumerate(parts):
            assert '"' not in part
            end = ('\\n' if j == 0 else '\\l') if j < len(parts)-1 else ('\\p' if i < len(paragraphs)-1 else '$')
            lines.append('\t.string "' + part + end + '"\n')
    source = source[:match.start(2)] + ''.join(lines) + source[match.end(2):]
    path.write_text(source)
    edits[label] = reason

edit('Route102_Text_CatchWholeBunchOfPokemon', [
    'I thought I saw a flower move...|Then a little green POKéMON ran|through the grass!',
    'SHAYMIN! Right here, so close to home!|You could meet it on your first trip.',
    'It has a 1% encounter chance.|SWEET SCENT raises that to 25%.'
], 'Keep the early Shaymin clue and rates while giving the witness an excited, local voice.')
edit('Route112_Text_LeaderGoingToAwakenThing', ['Hey, man, is our leader really going|to stir up the whole volcano?'], 'Meteorite conversation concerns the Mt. Chimney machine, not an Orb substitute for awakening Groudon.')
edit('Route112_Text_YeahWeNeedMeteorite', ["Sounds like it. I heard the machine|needs a METEORITE to get it going."], 'Separate meteorite machinery from later Orb awakening.')
edit('Route112_Text_MtChimneySign', [
    'MT. CHIMNEY',
    'For LAVARIDGE TOWN or the summit,|please take the CABLE CAR.',
    'FIELD NOTE: ENTEI has been sighted|in FIERY PATH. Search after earning|the DYNAMO BADGE.'
], 'Replace old guardian-answer wording with the verified three-badge Fiery Path wild sighting.')
edit('Route114_Text_GoodRodOffer', ['Different fish gather in these pools|at different depths. Want my GOOD ROD?'], 'Rod variety need not imply all Good Rod residents are stronger than Old Rod residents.')
edit('Route114_Text_GoodRodAdvice', ['Try your GOOD ROD along every shore.|You may find POKéMON your OLD ROD|never brought to the surface.'], 'Replace encounter-pool implementation jargon with fisherman language.')
edit('FallarborTown_MoveRelearnersHouse_Text_IvyEvieIntro3', [
    'EVIE: IVY adjusts EVs for free.',
    'I collect HEART SCALES and trade|a PP UP for each one.',
    'Come back as often as you like!'
], 'The PP Up exchange costs a Heart Scale, so the whole house is not all free.')
edit('SlateportCity_House_Text_NatureToDoWithStatGains', [
    'My POKéMON has a HASTY nature.|That raises its SPEED, but lowers|its DEFENSE.',
    'The CENTER specialist can adjust|Natures. Mine likes life in the fast|lane, so HASTY suits us!'
], 'Replace vague comparison across different species with correct Nature tradeoff and free service awareness.')
edit('LilycoveCity_DepartmentStore_4F_Text_AttackOrDefense', [
    'Hmm... An attack for a quick finish,|or a move to keep my partner safe?',
    'Only four moves, and I want five.|Shopping was supposed to be easier!'
], 'Give the shopper a concrete dilemma and personality instead of generic battle-set jargon.')
edit('MossdeepCity_House1_Text_CynthiaDefeat', [
    'What a battle! You and your POKéMON|have given me something new to study.'
], 'Do not claim the player denied sleep or followed one specific tactical solution.')
edit('MossdeepCity_House1_Text_CynthiaAfter', [
    'The SHINY CHARM is yours.|May your next journey reveal|something extraordinary.'
], 'This shared reward/repeat branch must not repeatedly ask the player to take an already-owned charm.')
edit('AquaHideout_B1F_Text_Grunt2PostBattle', [
    'The secret? Our submarine dock is at|the far end. All these warp panels|should keep you guessing. Kekeke.'
], 'The grunt can be defeated before or after departure; describe the permanent dock and warp panels rather than assert the submarine left.')
edit('AquaHideout_B1F_Text_Grunt3PostBattle', [
    'Humph! My real job was to slow you|down. Every minute you spend here|is another minute for the BOSS.'
], 'Do not assert the submarine has left before the departure scene.')
edit('SeafloorCavern_Room1_Text_Grunt1Intro', [
    'These currents nearly spun our sub|around. You should turn back while|you still know the way!'
], 'Replace impossible shifting cavern geography with a boast about its real currents.')
edit('SeafloorCavern_Room1_Text_Grunt1Defeat', ['You kept your footing better than me!'], 'Avoid claiming the floor physically changed during battle.')
edit('SeafloorCavern_Room1_Text_Grunt1PostBattle', [
    'ARCHIE went deeper into the cavern.|Those twisting passages ought to|keep you busy for a while.'
], 'Ancient natural rooms were not built by Aqua to misdirect pursuit.')
edit('SeafloorCavern_Room1_Text_Grunt2Intro', [
    "That narrow passage was just wide|enough for our sub. But this passage|isn't wide enough for both of us!"
], 'Give the guard a physical, local threat rather than formation jargon.')
edit('SeafloorCavern_Room1_Text_Grunt2PostBattle', [
    'We left the submarine outside.|ARCHIE took the crew farther in.|I was supposed to guard the way...'
], 'The stolen submarine remains visible underwater outside the cavern.')
edit('SSTidalCorridor_Text_ScottBattleFrontierInvite', [
    'SCOTT: {PLAYER}{KUN}! Just the TRAINER|I hoped to run into!',
    "I've watched you come a long way.|Now the sea has opened up a whole|new set of challenges for you.",
    'The BATTLE FRONTIER! Bring six|partners and see how far you can|go in the CHAMPIONS CIRCUIT.',
    "The ferry attendants can take you|there. I'll be waiting for you!"
], 'Ferries and Circuit open after six badges; player may not be Champion or need a new ferry unlock.')
edit('BattleFrontier_OutsideWest_Text_ThisIsBattleTower', [
    'This is the BATTLE TOWER.|The CHAMPIONS CIRCUIT holds its|doubles matches in the central arena.'
], 'Replace old elimination-style room description with current Circuit arena identity.')
edit('BattleFrontier_OutsideWest_Text_YoureOffToChallengeDome', [
    "Registering at the DOME? Good luck!|We'll both end up at the TOWER arena.|Maybe I'll see you there."
], 'All desks enter the same Circuit; no separate Dome tournament.')
edit('BattleFrontier_OutsideWest_Text_DomeAceLookedBecauseOfMyCheering', [
    'Whaaaaaat?! Pfft!',
    'The DOME ACE only looked our way|because my cheering was so loud!|That old tournament was legendary!'
], 'Frame retired Dome event fandom as a remembered tournament.')
edit('BattleFrontier_OutsideWest_Text_FansOverThereUsedToBeTrainers', [
    'Those people squabbling over there|used to be famous TRAINERS.',
    'Then one tournament opponent beat|all of them. They became fans on|the spot!',
    'The old DOME tournaments ended,|but they still meet here every day|to argue over their idol.'
], 'No daily active Dome tournament under current Circuit.')
edit('BattleFrontier_OutsideEast_Text_EnoughBattlePointsForDoll', [
    "Yes! Enough Battle Points at last!|I'm getting a giant plush DOLL.|It has a place waiting on my bed."
], 'Give the reward shopper a personal goal while preserving the exchange.')
edit('BattleFrontier_ExchangeServiceCorner_Text_MoreBattlePointsForRecord', [
    'Every CIRCUIT victory earns Battle|Points. Longer streaks earn more.',
    "I keep promising myself I'll save|them. Then I see another plush DOLL!"
], 'Replace separate facility record rewards with Circuit progression and personal flavor.')
edit('BattleFrontier_ExchangeServiceCorner_Text_DreamBallDesc', ['A BALL that excels on sleeping POKéMON.'], 'Dream Ball catch bonus concerns sleep, not vaguely dreamlike species.')
edit('BattleFrontier_Lounge5_Text_NatureGirlGreeting', [
    'Ehehe! I make up little stories|about what POKéMON are thinking!',
    'They still listen to you in battle.|Please! Can I see your POKéMON?'
], 'Preserve the child guessing personalities without presenting retired Palace autonomous move rules as current mechanics.')
edit('BattleFrontier_Lounge6_Text_WouldYouLikeToTrade', [
    'My POKéMON is a {STR_VAR_2}.|It was built to become anything.',
    'I think it needs a TRAINER who will|help it discover what it wants to be.',
    'Would you trade me a {STR_VAR_1}|for my {STR_VAR_2}?'
], 'No Champion requirement for the six-badge Frontier trade; make the trader value its future.')
edit('Route110_TrickHousePuzzle1_Text_SallyIntro', [
    "I've cut a path through every shrub!|Now let's see if I can cut through|your defenses!"
], 'Field Cut registration does not imply the battle team knows Cut.')
edit('Route110_TrickHousePuzzle5_Text_Mechadoll3Intro', [
    'CLICKETY-CLACK... MECHADOLL 3 AM I!',
    'SUPPORT, STATUS, AND HELD ITEMS!|SMALL DETAILS, LARGE CONSEQUENCES!'
], 'Its current quiz asks about burn, Helping Hand, and Eviolite, not money.')
edit('AshenWoods_Text_MartinAfter', [
    'MOLTRES roosts in EMBER PATH.|Follow the trail and keep your eyes|up. Those feathers catch the light!'
], 'Moltres actor is in Ember Path, not beyond it; remove nonexistent implied strength condition.')
edit('AshenWoods_Text_RomanIntro', [
    'The rocks hiss when rain hits them.|A little water, all that heat...|Now watch what my POKéMON can do!'
], 'Ground the trainer challenge in the local environment instead of engine jargon.')
edit('AshenWoods_Text_RomanDefeat', ["Whew! You've left us steaming!"], 'Do not presume the player stopped a combo before it activated.')
edit('AshenWoods_Text_RomanAfter', [
    'I could listen to the forest hiss|after rain all day. Even the mountain|needs to cool off sometimes.'
], 'Give the trainer local interests beyond doubles instructions.')
edit('AshenWoods_Text_ElmerIntro', [
    'Look closely at the ash. Tiny tracks|everywhere! My bugs are tougher|than they look. Care to see?'
], 'Remove arbitrary demand that the player show four answers.')
edit('AshenWoods_Text_ElmerAfter', [
    'Some insects burrow when the ash|falls. Others just shake it off.|I always find something new here.'
], 'Replace generic Champion toolbox lecture with a bug enthusiast.')
edit('AshenWoods_Text_Caretaker', [
    'Heat scorched these woods, but the|roots survived. See the new leaves?',
    'With the weather changing so much,|POKéMON we seldom saw are appearing|among the ash.',
    'The old wardens say RAYQUAZA keeps|land and sea in balance. I hope|they are right.'
], 'This area is accessible before Groudon awakens; foreshadow weather balance without claiming a later event occurred.')
edit('EmberPath_Text_Warden', [
    'The mountain has been restless.|TEAM MAGMA tampering with it|certainly did not help.',
    'MOLTRES roosts along this path.|It seems quite at home with the heat.'
], 'Remove an unsupported eastward heavier-creature clue and blanket claim about every fault.')
edit('SandstrewnRuins_Text_Archaeologist', [
    'Every fossil here tells us how a|POKéMON lived. Imagine this place|before the desert!',
    'Those green lights below us may be|ZYGARDE. Search B1F after earning|the BALANCE BADGE.'
], 'Zygarde is a five-badge wild discovery, without a Landorus prerequisite.')
edit('ScorchedSlab_B2F_Text_Warden', [
    'A white dragon has been glimpsed|among these hot stones. RESHIRAM!',
    'With the FEATHER BADGE, you can|search for it here. HEATRAN dwells|farther below.'
], 'Reshiram is a six-badge local wild discovery, without a Dialga prerequisite.')

edit('RusturfTunnel_Text_GruntIntro', [
    'Grah, keelhaul it all!',
    'A hostage bird, a stolen package,|and a tunnel to nowhere! This job|was supposed to be easy!',
    'You want the package? MIGHTYENA,|show this meddler the way out!'
], 'The current Rusturf grunt has Mightyena, Qwilfish, Skrelp, and Zubat, not Dondozo.')
edit('SeafloorCavern_Entrance_Text_HearMagmaNearMossdeep', [
    'Hey! I remember your face!|You just keep turning up!',
    "You followed our sub down here?|Don't tell me you swam!",
    'ARCHIE says the ocean needs room|to breathe. We came all this way|to give it that chance.',
    "You won't stop us now. There are|plenty of us waiting inside!"
], 'Replace stale Mossdeep detour with Aqua expedition motive and recognition of the player arriving by Dive.')
edit('SeafloorCavern_Entrance_Text_HearMagmaNearMossdeepShort', [
    'ARCHIE says the ocean needs room|to breathe. We came all this way|to give it that chance.',
    'Go on, then. The others inside|will send you back to the surface!'
], 'Keep repeat dialogue coherent with the new expedition speech and actual route forward.')

(root / 'work/dialogue-review-20260909/npcs_b-edits.json').write_text(json.dumps(edits, indent=2) + '\n')
print(f'Edited {len(edits)} dialogue blocks in {len({by_label[k]["file"] for k in edits})} files.')
