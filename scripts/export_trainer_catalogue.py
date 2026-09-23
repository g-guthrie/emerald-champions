#!/usr/bin/env python3
"""Export native trainer parties plus authoring comparison and runtime alternatives.

Read-only with respect to design/game sources. Fails on missing required fields or
authoring/native party mismatch; never fabricates a fixed party for procedural battles.
"""
from __future__ import annotations
import argparse
from collections import Counter, defaultdict
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import re
import subprocess
import textwrap

import emerald_champions_teams as teams

ROOT = Path(__file__).resolve().parents[1]
STATS = ('HP', 'Atk', 'Def', 'SpA', 'SpD', 'Spe')
PARTY = 'src/data/trainers.party'


def read(path):
    return (ROOT / path).read_text()


def clean(text):
    # Keep newlines, so locations continue to refer to original source lines.
    text = re.sub(r'/\*.*?\*/', lambda m: '\n' * m[0].count('\n'), text, flags=re.S)
    return re.sub(r'//[^\n]*', '', text)


def pretty(token):
    token = re.sub(r'^(SPECIES|MOVE|ITEM|ABILITY|NATURE|TYPE|SHOWDOWN_ROLE)_', '', token)
    return token.replace('_', ' ').title()


def fields(text):
    return dict(re.findall(r'^([^\n:]+):[ \t]*([^\n]*)$', text, re.M))


def stat_values(value):
    result = {name: int(number) for number, name in re.findall(r'(\d+)\s+(HP|Atk|Def|SpA|SpD|Spe)\b', value)}
    assert set(result) == set(STATS), value
    return [result[name] for name in STATS]


def parse_parties():
    source = read(PARTY)
    text = clean(source)
    marks = list(re.finditer(r'^=== (TRAINER_\w+) ===$', text, re.M))
    result = {}
    for i, mark in enumerate(marks):
        body = text[mark.end():marks[i+1].start() if i+1 < len(marks) else len(text)]
        starts = list(re.finditer(r'^(SPECIES_\w+)(?: @ (ITEM_\w+))?$', body, re.M))
        assert starts or mark[1] == 'TRAINER_NONE' or re.search(r'^Party Size: 0$', body, re.M), mark[1]
        header = fields(body[:starts[0].start()] if starts else body)
        mons = []
        for n, start in enumerate(starts):
            block = body[start.end():starts[n+1].start() if n+1 < len(starts) else len(body)]
            f = fields(block)
            for key in ('Level', 'Level Offset', 'Ability', 'IVs', 'Happiness', 'EVs', 'Nature'):
                assert key in f, (mark[1], n, key)
            moves = re.findall(r'^- (MOVE_\w+)$', block, re.M)
            assert 1 <= len(moves) <= 4, (mark[1], n)
            mons.append(dict(species=start[1], item=start[2] or 'ITEM_NONE', moves=moves,
                ability=f['Ability'], nature=f['Nature'], level=int(f['Level']),
                offset=int(f['Level Offset']), evs=stat_values(f['EVs']),
                ivs=stat_values(f['IVs']), friendship=int(f['Happiness'])))
        assert mark[1] not in result
        result[mark[1]] = dict(header=header, mons=mons,
            line=source[:source.index('=== '+mark[1]+' ===')].count('\n')+1)
    assert len(result) == len(marks)
    return result


def source_references(ids):
    refs = defaultdict(list)
    partners = []
    for path in sorted([*(ROOT/'data/maps').rglob('scripts.inc'), *(ROOT/'data/scripts').rglob('*.inc')]):
        rel = str(path.relative_to(ROOT))
        if 'frlg' in rel.lower():
            continue
        label = ''
        for number, raw in enumerate(clean(path.read_text()).splitlines(), 1):
            line = raw.strip()
            match = re.match(r'(\w+)::?$', line)
            if match:
                label = match[1]
            if not line or line.startswith(('@', '#')):
                continue
            command = line.split()[0]
            for trainer in set(re.findall(r'\bTRAINER_\w+\b', line)) & ids:
                battle = command.startswith('trainerbattle') or command.startswith('multi_')
                refs[trainer].append(dict(path=rel,line=number,label=label,command=command,
                    map=path.parent.name if 'data/maps/' in rel else None,
                    battle=battle, text=line))
            if 'PARTNER_STEVEN' in line:
                partners.append(f'{rel}:{number}: {line}')
    rematches = {}
    for m in re.finditer(r'REMATCH\(([^\n]*)\)', read('src/battle_setup.c')):
        bits = re.findall(r'\bTRAINER_\w+\b|\bMAP_\w+\b', m[1])
        if bits:
            for trainer in bits[:-1]:
                if trainer in ids:
                    rematches[trainer] = bits[-1]
    return refs, rematches, partners


def native_vs_authoring(parties, branches):
    checked = 0
    for b in branches:
        assert b.trainer in parties, b.trainer
        native = parties[b.trainer]['mons']
        assert len(native) == len(b.mons), b.trainer
        for i, (n, m) in enumerate(zip(native, b.mons), 1):
            expected = dict(species='SPECIES_'+m.species, item='ITEM_'+m.item,
                ability='ABILITY_'+m.ability, nature='NATURE_'+m.nature,
                moves=['MOVE_'+x for x in m.moves], offset=m.offset,
                evs=list(map(int, m.evs.split('/'))), ivs=list(map(int,m.ivs.split('/'))),
                friendship=m.friendship)
            for key, val in expected.items():
                assert n[key] == val, (b.trainer, i, key, n[key], val)
            checked += 1
    return checked


def inferred_plan(mons):
    moves = {x for m in mons for x in m['moves']}
    abilities = {m['ability'] for m in mons}
    motifs = []
    engines = [('MOVE_TRICK_ROOM','Trick Room speed reversal'),('MOVE_TAILWIND','Tailwind speed control'),
        ('MOVE_PERISH_SONG','Perish Song countdown'),('MOVE_SHELL_SMASH','Shell Smash setup'),
        ('MOVE_BELLY_DRUM','Belly Drum setup'),('MOVE_FOLLOW_ME','Follow Me redirection'),
        ('MOVE_RAGE_POWDER','Rage Powder redirection'),('MOVE_BEAT_UP','Beat Up multi-hit pressure; partner activation requires a compatible ally'),
        ('MOVE_FAKE_OUT','Fake Out disruption'),('MOVE_HELPING_HAND','Helping Hand damage support'),
        ('MOVE_WILL_O_WISP','burn-based physical disruption'),('MOVE_SPORE','Spore sleep pressure'),
        ('MOVE_ELECTROWEB','Electroweb speed drops'),('MOVE_ICY_WIND','Icy Wind speed drops')]
    for move, description in engines:
        if move in moves:
            motifs.append(description)
    for ability, description in [('ABILITY_DRIZZLE','automatic rain'),('ABILITY_DROUGHT','automatic sun'),
        ('ABILITY_SAND_STREAM','automatic sand'),('ABILITY_SNOW_WARNING','automatic snow'),
        ('ABILITY_INTIMIDATE','Intimidate attack control'),('ABILITY_SHADOW_TAG','Shadow Tag trapping')]:
        if ability in abilities:
            motifs.append(description)
    if not motifs:
        motifs.append('direct attack pressure using the listed coverage and held items')
    return 'Observed tools: ' + '; '.join(motifs) + '. This is an inference from the loadouts, not a book-authored tactical plan or a verified AI execution claim.'


def c_array(text, symbol):
    start = text.index('{', text.index(symbol))
    depth = 0
    for end in range(start, len(text)):
        depth += (text[end] == '{') - (text[end] == '}')
        if depth == 0:
            return text[start+1:end]
    raise ValueError(symbol)


def c_records(text):
    return re.findall(r'^    \{\n(.*?)^    \},?$', text, re.M | re.S)


def c_fields(text):
    return dict(re.findall(r'\.(\w+)\s*=\s*(\{[^}]*\}|[^,\n]+)', text))


def enum_list(value):
    return re.findall(r'\b[A-Z][A-Z0-9_]+\b', value)


def regional_sets():
    src = read('src/data/pokemon/emerald_champions_battle_sets.h')
    presets = [c_fields(x) for x in re.findall(r'\.preset = \{(.*?)\}\}', src, re.S)]
    offsets = {sp:int(offset) for sp, offset in re.findall(
        r'\[EC_BATTLE_FORMAT_DOUBLES\]\[(SPECIES_\w+)\] = \{\.offset = (\d+)', src)}
    starters = read('src/starter_choose.c')
    generations = {int(gen):enum_list(val) for gen, val in re.findall(r'\[(\d+)\] = \{([^}]+)\}', c_array(starters,'sStarterMons'))}
    mids = dict(re.findall(r'case (SPECIES_\w+):\s+return (SPECIES_\w+);', starters.split('enum Species GetMiddleEvolutionForStarter')[1].split('enum Species GetFinalEvolutionForStarter')[0]))
    finals = {}
    finaltext = starters.split('enum Species GetFinalEvolutionForStarter')[1].split('u16 GetStarterPokemon')[0]
    for a,b,result in re.findall(r'case (SPECIES_\w+):\s+case (SPECIES_\w+):\s+return (SPECIES_\w+);', finaltext):
        finals[a] = finals[b] = result
    out = []
    for generation in range(1,10):
        for index, base in enumerate(generations[generation]):
            for stage, species in enumerate((base,mids[base],finals[base])):
                p = presets[offsets[species]]
                m = dict(species=species,item=p['item'],nature=p['nature'],ability=p['ability'],
                    moves=enum_list(p['moves']),evs=list(map(int,re.findall(r'\d+',p['evs']))),
                    ivs=[31]*6, friendship=255)
                # Exact explicit overrides in emerald_champions_story.c.
                if stage == 0:
                    if species == 'SPECIES_CHIKORITA': m['moves'][0] = 'MOVE_GIGA_DRAIN'
                    if species == 'SPECIES_TORCHIC':
                        m.update(moves=['MOVE_FLAMETHROWER','MOVE_HELPING_HAND','MOVE_WILL_O_WISP','MOVE_PROTECT'],
                            item='ITEM_EVIOLITE',nature='NATURE_TIMID',ability='ABILITY_SPEED_BOOST',evs=[4,0,0,252,0,252])
                    if species == 'SPECIES_BULBASAUR':
                        m['ability']='ABILITY_OVERGROW';m['moves'][0]='MOVE_PROTECT'
                    if species == 'SPECIES_CHARMANDER':
                        m['ability']='ABILITY_BLAZE';m['moves'][1]='MOVE_FLAMETHROWER'
                    if species == 'SPECIES_ROWLET': m['moves'][3]='MOVE_PROTECT'
                    if species == 'SPECIES_SOBBLE': m['moves'][2]='MOVE_MUD_SHOT'
                    if m['item']=='ITEM_EVIOLITE': m['item']='ITEM_SITRUS_BERRY'
                else:
                    if species == 'SPECIES_IVYSAUR':
                        m['ability']='ABILITY_OVERGROW'
                        for i,move in ((0,'SLEEP_POWDER'),(1,'GIGA_DRAIN'),(3,'PROTECT')):m['moves'][i]='MOVE_'+move
                    if species == 'SPECIES_CHARMELEON':
                        m['ability']='ABILITY_BLAZE';m['moves'][1]='MOVE_DRAGON_PULSE'
                    if species=='SPECIES_BAYLEEF':m['moves'][0]='MOVE_GIGA_DRAIN'
                    if species=='SPECIES_MONFERNO':m['moves'][3]='MOVE_FIRE_PUNCH'
                    if species=='SPECIES_PRINPLUP':m['moves'][0]='MOVE_HYDRO_PUMP'
                    if species=='SPECIES_DRIZZILE':m['moves'][2]='MOVE_MUD_SHOT'
                out.append((generation,index,stage,m))
    assert len(out)==81
    return out


class Writer:
    def __init__(self): self.lines=[]
    def line(self,text=''): self.lines.append(str(text))
    def prose(self,text):
        self.lines.extend(textwrap.wrap(text,width=94,break_long_words=False,break_on_hyphens=False) or [''])
    def heading(self,text):
        self.line();self.line('='*78);self.line(text);self.line('='*78)
    def mon(self,number,m,level=True):
        self.line(f"  {number}. {pretty(m['species'])} @ {pretty(m['item'])}")
        self.line(f"     Species ID: {m['species']} | Item ID: {m['item']}")
        self.line(f"     Ability: {pretty(m['ability'])} | Nature: {pretty(m['nature'])}")
        if level:
            self.line(f"     Runtime level: live cap {m['offset']:+d}, then difficulty adjustment.")
            self.line(f"     Stored absolute level (legacy preview only): {m['level']}")
        self.line('     EVs: ' + ' / '.join(f'{s} {v}' for s,v in zip(STATS,m['evs'])))
        self.line('     IVs: ' + ' / '.join(f'{s} {v}' for s,v in zip(STATS,m['ivs'])))
        self.line(f"     Friendship: {m['friendship']}")
        for move in m['moves']:self.line(f'     - {pretty(move)} [{move}]')
        self.line()


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--out',type=Path,default=ROOT/'work/exports/Emerald_Champions_All_Trainer_Battles.txt')
    args=parser.parse_args()
    parties=parse_parties()
    branches=teams.read_teams()
    by_id={b.trainer:b for b in branches}
    assert len(by_id)==len(branches)
    checked=native_vs_authoring(parties,branches)
    assert {t for t,p in parties.items() if p['mons']} == set(by_id)
    subprocess.run(['python3','scripts/verify_campaign_trainer_roster.py'],cwd=ROOT,check=True)
    meta={n:fields(block[:teams.BRANCH_RE.search(block).start()] if teams.BRANCH_RE.search(block) else block)
        for n,block in teams.split_encounters(teams.MASTER.read_text())[1]}
    refs,rematches,partnerrefs=source_references(set(parties))
    w=Writer()
    w.heading('EMERALD CHAMPIONS — COMPLETE TRAINER SOURCE CATALOGUE')
    w.line('Snapshot: '+datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M UTC'))
    w.line('Repository: https://github.com/g-guthrie/emerald-champions')
    w.line('Current combat-party count matches the authored catalogue; retired metadata is not a battle.')
    w.line('Commit baseline: '+subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip())
    w.line('Includes the current uncommitted working-tree changes; this is not a published-release claim.')
    w.line()
    total=sum(len(x['mons']) for x in parties.values())
    w.prose(f"Scope: ALL {len(branches)} CURRENT COMBAT PARTY VARIANTS, containing {total} Pokemon slots, across {len(set(b.encounter for b in branches))} retained authored encounter groups. See docs/trainer-review-index.json for the encounter count; starter/gender alternatives and multi-battle owners are not extra clears. An E-group or catalogue position is not first-access order. Native source also preserves {len(parties)-len(branches)} empty records: {len(parties)-len(branches)-1} retired trainer metadata entries and TRAINER_NONE. Those contain NO battle parties. They are included separately for audit, not counted as playable trainers.")
    w.prose('Contents: 1) findings and reading guide; 2) campaign index; 3) every retained campaign party; 4) empty retired metadata and sentinel; 5) exact regional-rival replacement sets; 6) Steven ally and opening rescue; 7) procedural Circuit/Tent roster templates and generating rules; 8) source fingerprints and validation scope.')
    w.heading('1. FINDINGS, AUTHORING COMPARISON, AND READING GUIDE')
    findings=[
        f'LOADOUT AGREEMENT: all {len(branches)} retained authored variants and {checked} Pokemon slots match native source for species, order, items, abilities, natures, EVs, IVs, moves, friendship and level offsets. Separate generator verification checks generated encounter/AI tables. Agreement is not proof that every tactical idea works or every battle has been played.',
        'LEVELS: actual campaign level = live player cap + authored offset + difficulty adjustment, with a floor of 1 and the existing native byte representation bound of 255. Opponents can exceed 100. Hard plays the roster as authored against the cap; Medium is one level below and Easy three. The cap ladder is Inclement Emerald Strict, badge-indexed, with one extra step at the Groudon awakening: 14, 20, 30, 40, 45, 55, 60, 65, 70, 80, 100. The printed absolute Level in trainers.party is a Hard-difficulty preview rendered against the authored strict_cap, not a stored encounter level.',
        'LEVEL LIMIT REPAIRED: this audit exposed the old signed four-bit (-8..+7) offset restriction and level-100 clamp. Trainer offsets now use signed 16-bit storage and authoring accepts -254..+254. Trainer creation uses bounded EXP plus transient opponent levels; stat recalculation preserves overlevel trainer opponents, including Mega forms and either opponent owner. The native battle/controller level fields remain one byte (1..255); this is a technical representation bound, not a prescribed difficulty cap. No blanket party-level increase was applied.',
        'DWAYNE IS AN EXPERIMENT: the exported working tree currently uses Magmar, Jynx, Electabuzz and Monferno. The original Magby/Smoochum/Elekid/Monferno battle was won in four turns with zero faints. The evolved-team retest is paused mid-battle; it is not an accepted final composition or completed difficulty benchmark. The user clarified that level tuning should preserve deliberate low-stat themes.',
        'RUNTIME RIVALS: native Hoenn trainer blocks are seeds. Nonmatching regional starters replace the first Hoenn starter slot using the selected generation and unchosen starter index, preserving its level and using the matching evolution stage. Appendix 5 gives the complete alternative sets; printing only the seeds would be incomplete.',
        'PROCEDURAL OPPONENTS: the Champions Circuit, live Battle Tents and exhibition provider generate teams. There is no finite list of fixed six-Pokemon parties for them. Appendix 7 includes every native variant/template plus the exact local generator source, rather than inventing deterministic teams.',
        'STATIC COVERAGE IS NOT SHOWCASE ACCEPTANCE: the authored roster contains all 99 supported Mega Stone item types. The current legendary-family index reports 73 of 78 acquisition families, with Galarian Articuno/Zapdos/Moltres, Glastrier and Meltan absent under its explicit identity/alias rules. Meltan has a recorded intentional trainer-showcase cut; the others need curation review. Hoopa Unbound counts toward the Hoopa family. Presence alone does not prove useful Mega activation, good synergy, fair availability or strong AI execution.',
        f'ROSTER CLEANUP: {len(branches)} authored combat parties agree with nonempty native parties and Hoenn battle IDs. The {len(parties)-len(branches)-1} retired metadata records and TRAINER_NONE contain no combat loadouts. Native rematches are disabled. First-access ordering is tracked in docs/trainer-review-index.json; this export does not reuse a pre-restoration census.',
        'DIFFICULTY AND DESIGN: native play is still in chapter C15. Earlier source checks and fixed-mechanic regressions do not amount to completed whole-game playtesting. Evaluate each team concept, local theme, engine support, AI decisions and strongest stage-legal counterteam before using levels as the main tuning lever. This catalogue provides the exact inputs for that work, not a final green check.'
    ]
    for i,f in enumerate(findings,1):w.prose(f'{i}. {f}');w.line()
    w.prose('Every EV and IV line is explicitly labeled HP / Attack / Defense / Special Attack / Special Defense / Speed. EV numbers are native EV allocations, not a derived percentage or a speculative point budget. Items are held items; there is no authorization here for manual potion/X-item use. Campaign held consumables restore after battle according to the Champions settlement rules.')
    w.prose('Party slots are in native source order. Multi battles may combine two owners and take up to three Pokemon from each according to multiTeamSize/B_MULTI_HALF_TEAMS. The header names the authored group and its owners so both halves can be found. A location called “script references” means source linkage, not a proof that every conditional branch is reachable at the current story state. Non-campaign definitions with references can still be retired rematches or dormant scripts.')
    w.prose('Plans and counterplay in campaign entries are the team file’s authored intent. Strategy flags and tactics are executable configuration; prose itself is not executable AI. Extra definitions have explicitly labeled loadout inferences. Nicknames are presentation only; enum IDs are retained for exact lookup.')
    w.line()
    w.prose('Observed engine coverage in the current native campaign loadouts (party variants, not distinct played encounters):')
    for token,field in [('STEAM_ENGINE','ability'),('JUSTIFIED','ability'),('COMMANDER','ability'),('ANGER_POINT','ability'),('WEAKNESS_POLICY','item'),('PERISH_SONG','moves'),('SHELL_SMASH','moves'),('BEAT_UP','moves')]:
        prefix = {'ability':'ABILITY_','item':'ITEM_','moves':'MOVE_'}[field]
        owners = [b.trainer for b in branches if any(prefix+token in m[field] if isinstance(m[field],list) else prefix+token==m[field] for m in parties[b.trainer]['mons'])]
        w.prose(f'{pretty(token)}: {len(owners)} variants. ' + ', '.join(owners))
    w.prose('These counts locate design-review work; an ability or item alone is not proof of a supported activation engine. In particular, Justified users are not automatically Beat Up teams, and Perish Song users need their actual trap/protection plan examined.')
    w.line()
    w.prose('Focused native validation: wide trainer offsets produce levels 150 and 250, preserve levels and bounded EXP through Mega stat recalculation for both opponent owners, and leave player progression capped. Existing native Circuit controller tests also pass level-255 Seismic Toss damage and a level-150 Mega transformation. These are mechanic regressions, not difficulty simulations or new earned wins.')
    w.heading('2. RETAINED CAMPAIGN INDEX')
    for b in branches:
        h=parties[b.trainer]['header'];m=meta[b.encounter]
        w.line(f"E{b.encounter:04d} | {h['Name']} ({h['Class']}) | {m.get('location','Unspecified')} | {b.trainer}")
    w.heading('3. EVERY RETAINED CAMPAIGN PARTY — SOURCE ORDER WITHIN EACH PARTY')
    emitted=[]
    def emit(trainer,b=None):
        p=parties[trainer];h=p['header'];r=refs[trainer]
        encounter=f'E{b.encounter:04d} — ' if b else 'ADDITIONAL NATIVE DEFINITION — '
        w.heading(encounter+h['Name']+' ('+h['Class']+')')
        w.line('Trainer ID: '+trainer)
        w.line(f"Source: {PARTY}:{p['line']}")
        if b:
            m=meta[b.encounter]
            for key in ('chapter','location','requirement','trainer_ids'):
                w.prose(key.replace('_',' ').title()+': '+m.get(key,'Not specified'))
            w.line('Status: retained in authored catalogue; '+('EXPERIMENTAL CURRENT PARTY' if trainer=='TRAINER_DWAYNE' else 'source agreement confirmed'))
        else:
            w.prose('Status: EMPTY METADATA ONLY — no Pokemon, no combat party, and no live Hoenn script battle call. Retained for stable IDs/contact metadata; TRAINER_NONE is the sentinel.')
        maps=sorted({x['map'] for x in r if x['map']})
        if maps:w.prose('Map script locations: '+'; '.join(maps))
        elif trainer in rematches:w.prose('Location metadata from native rematch table: '+rematches[trainer]+' (rematches retired from current campaign).')
        elif not b:w.line('Location: no direct map/script or rematch-table location found; not assigned a guessed location.')
        calls=[x for x in r if x['battle']]
        if calls:
            w.line('Battle callsites (source references, conditional reachability separate):')
            for x in calls:w.prose(f"  {x['path']}:{x['line']} [{x['label']}] {x['text']}")
        elif r:
            w.line('Other script references (not a direct battle invocation):')
            for x in r:w.prose(f"  {x['path']}:{x['line']} [{x['label']}] {x['text']}")
        else:w.line('No direct trainer-ID occurrence in map/shared scripts was found.')
        w.line('Format: Double Battle = '+h.get('Double Battle','not specified')+'; Multi Party = '+h.get('Multi Party','default/full'))
        w.prose('AI configuration: '+h.get('AI','not explicitly specified in this source block'))
        for key,val in h.items():
            if key not in ('Name','Class','Double Battle','Multi Party','AI'):
                w.prose(key+': '+val)
        if b:
            w.prose('Authored strategy: '+b.plan)
            w.prose('Authored counterplay / audit: '+b.crack)
            w.line('Executable strategy flags: '+(', '.join(b.strategy) or 'NONE'))
            w.line('Executable partner tactics: '+('; '.join(str(t) for t in b.tactics) or 'NONE'))
            w.line('Mega slot configuration: '+str(b.mega_slots)+' (None means generator default; actual source table is authoritative)')
        else:w.line('General strategy: not applicable; obsolete combat loadout removed.')
        if re.match(r'TRAINER_(BRENDAN|MAY)_',trainer):
            w.prose('Runtime regional-rival note: if this ID is selected by IsRegionalRivalTrainer and a Hoenn starter slot differs from the unchosen regional starter, the corresponding slot is replaced. See Appendix 5 for the complete replacement mechanism and sets.')
        w.line(f"Complete source party: {len(p['mons'])} Pokemon")
        for i,m in enumerate(p['mons'],1):w.mon(i,m)
        emitted.append(trainer)
    for b in branches:emit(b.trainer,b)
    w.heading('4. EMPTY RETIRED METADATA AND SENTINEL — NOT PLAYABLE PARTIES')
    for trainer in parties:
        if trainer not in by_id:emit(trainer)
    assert len(emitted)==len(set(emitted))==len(parties)
    w.heading('5. REGIONAL RIVAL ALTERNATIVES — EXACT RUNTIME SET REPLACEMENTS')
    w.prose('Selection: the player chooses two different starters in a generation; the rival gets index 3 - first - second. Index 0/1/2 corresponds to Grass/Fire/Water. The dispatcher chooses a printed rival ID, then ApplyRegionalRivalStarter scans its party for the first Treecko/Torchic/Mudkip family member. It substitutes the selected generation’s starter at the same stage. Other party members remain as printed. If the species is already identical, it returns without replacing the authored set. Thus a printed Hoenn seed may intentionally differ from the preset below. These are replacement possibilities, not 81 extra trainer battles.')
    w.prose('Levels retain the seed slot’s live-cap offset. ApplyEmeraldChampionsScriptedSet supplies the listed moves/item/nature/ability/EVs; scripted application normalizes IVs to 31 while retaining friendship. All retained rival seed slots currently have friendship 255. Full runtime functions are reproduced below so early-return and generation dispatch behavior are reviewable.')
    replacement=regional_sets()
    for gen,index,stage,m in replacement:
        w.line(f'Generation {gen}; rival starter index {index}; stage {stage} (0 base / 1 middle / 2 final)')
        w.mon(1,m,False)
    w.line('Runtime replacement source: src/emerald_champions_story.c')
    src=read('src/emerald_champions_story.c')
    w.line(src[src.index('static void GetOpeningStarterSet'):src.index('static bool32 CreateOpeningStarter')])
    w.line(src[src.index('void ApplyEmeraldChampionsRegionalRivalSet'):src.index('\n',src.index('\n}\n',src.index('void ApplyEmeraldChampionsRegionalRivalSet'))+1)])
    src=read('src/battle_setup.c')
    start=src.index('static bool32 IsRegionalRivalTrainer(u16 trainerNum)\n{')
    w.line('Runtime dispatch source: src/battle_setup.c')
    w.line(src[start:src.index('void CreateTrainerPartyForPlayer(void)',start)])
    w.heading('6. ALLIED TRAINER AND OPENING RESCUE (SEPARATE FROM OPPONENT COUNT)')
    w.prose('Steven is an allied trainer in the Mossdeep Space Center multi battle, not another opponent to defeat. His complete native party source follows, including fixed source levels and independent AI flags. His lower-level runtime creation uses the applicable trainer/partner difficulty provider; these absolute partner levels are not campaign-offset records.')
    for ref in partnerrefs:w.prose(ref)
    w.line(read('src/data/battle_partners.party'))
    w.prose('Inferred ally plan: Metagross uses Assault Vest bulk and priority; Skarmory supplies Tailwind and Body Press pressure; Aggron attacks with Rock Head Head Smash and an Air Balloon. These are source-based observations, not verified ally behavior.')
    w.prose('The Birch rescue is a scripted wild double battle, not a trainer ID. For completeness: level-2 Poochyena and Zigzagoon, both 31 IVs, with the exact scripted sets below. It is not part of the 369 combat-party count.')
    src=read('src/emerald_champions_story.c')
    w.line(src[src.index('static const struct EmeraldChampionsBattleSet sRescueSets'):src.index('static void GetOpeningStarterSet')])
    w.heading('7. PROCEDURAL CIRCUIT / TENT / EXHIBITION OPPONENTS')
    w.prose('Locations: live Battle Frontier challenge desks feed the Champions Circuit; Battle Tent challenge desks use the same competition generator at their current Tent cap. Exhibition generation uses its supplied fixed level. Exact entrypoint script references are listed below. Each generated party has six Pokemon, max IVs, generated or explicitly authored EV/nature/item/ability/moves, family/role/dependency constraints and lead ordering. There is no fixed named party per opponent. Every available variant and template is listed below; full generating rules follow to specify fields decided at runtime.')
    for path in sorted([*(ROOT/'data/maps').rglob('scripts.inc'),*(ROOT/'data/scripts').rglob('*.inc')]):
        for num,line in enumerate(path.read_text().splitlines(),1):
            if re.search(r'\b(ChampionsCircuitGenerateOpponent|ChampionsTentGenerateOpponent|ChampionsCircuitBegin|ChampionsTentBegin)\b',line):
                w.line(f'{path.relative_to(ROOT)}:{num}: {line.strip()}')
    circuit=read('src/data/pokemon/showdown_champions_circuit.h')
    variants=[c_fields(r) for r in c_records(c_array(circuit,'gShowdownCircuitVariants'))]
    templates=[c_fields(r) for r in c_records(c_array(circuit,'gShowdownCircuitTemplates'))]
    assert variants and templates
    w.line(f'Native variant count: {len(variants)}; template count: {len(templates)}')
    w.prose('Non-authored templates list candidate move and ability pools, not a simultaneous complete moveset. The generator selects at most four moves and chooses item/nature/EVs contextually. Authored templates specify those fields directly. A variant’s required item is a separate constraint, commonly its Mega Stone. For generated template EV/item details use the exact generator rules following the template catalogue.')
    for i,v in enumerate(variants):
        w.line();w.line(f"Variant {i}: {pretty(v['partySpecies'])} -> {pretty(v['formSpecies'])}")
        w.line('  Party species: '+v['partySpecies']+'; form: '+v['formSpecies'])
        w.line('  Required item: '+pretty(v['requiredItem']))
        w.line('  Compatibility flags: '+v['compatibilityFlags'])
        start=int(v['templateOffset']);count=int(v['templateCount'])
        assert 0 <= start < len(templates) and start+count <= len(templates)
        for ti in range(start,start+count):
            t=templates[ti]
            w.line(f"  Template {ti}: {pretty(t['role'])}; authored = {t.get('authored','FALSE')}")
            for k,value in t.items():
                if k=='role':continue
                if k in ('moves','abilities'):
                    tokens=[x for x in enum_list(value) if not x.endswith('_NONE')]
                    w.prose('    '+k.title()+': '+', '.join(pretty(x) for x in tokens))
                elif k=='evs':
                    w.line('    EVs: '+value+' (Circuit struct native order: HP/Atk/Def/Spe/SpA/SpD)')
                else:w.line('    '+k+': '+value)
    used={i for v in variants for i in range(int(v['templateOffset']),int(v['templateOffset'])+int(v['templateCount']))}
    assert used==set(range(len(templates))), ('unmapped Circuit templates',set(range(len(templates)))-used)
    w.heading('7B. EXACT NATIVE PROCEDURAL TEAM GENERATION RULES')
    w.prose('The following is actual local source, included because dynamic item/nature/EV/move selection cannot honestly be flattened into one fixed party. This appendix also preserves the dependency checks, lead ordering, failure handling and level scaling used by the current build. The source is provided for audit; campaign entries above remain readable without interpreting it.')
    for path in ('include/champions_circuit.h','src/champions_circuit.c'):
        w.line('BEGIN SOURCE: '+path);w.line(read(path));w.line('END SOURCE: '+path)
    w.heading('8. PROVENANCE AND VALIDATION SCOPE')
    w.prose('The exporter independently parsed native trainerproc input, validated every required mon field, compared every retained authored slot field, and asserted complete unique coverage of all native trainer IDs and all procedural templates. It reads source files, not screenshots or memory of an older build. It does not certify every script branch reachable, every move strategically sensible, every AI tactic implemented correctly, or the full game playtested.')
    w.prose('Out-of-scope alternate games/providers: trainers_frlg.party belongs to the alternate FireRed/LeafGreen build selected by IS_FRLG; debug_trainers.party is development content. Retired original Battle Frontier facility tables, Trainer Hill, record-mixed Secret Base teams, link opponents and player-created teams do not constitute fixed current campaign encounters. The active Frontier desk boundary is the Champions Circuit. The complete active Hoenn trainers.party is included even for dormant or otherwise unclassified definitions.')
    for path in (PARTY,
        'data/emerald_champions/emerald_champions_master_battle_design.txt',
        'src/data/emerald_champions_battle_plans.h','src/battle_setup.c','src/difficulty.c','include/data.h','src/trainer_util.c','src/pokemon.c',
        'src/emerald_champions_story.c','src/data/pokemon/emerald_champions_battle_sets.h',
        'src/data/pokemon/showdown_champions_circuit.h','src/champions_circuit.c',
        'src/data/battle_partners.party','scripts/export_trainer_catalogue.py'):
        w.line(path);w.line('SHA-256: '+hashlib.sha256((ROOT/path).read_bytes()).hexdigest())
    w.line();w.line(f'END — {len(branches)} combat-party variants + {len(emitted)-len(branches)} empty metadata/sentinel records; {total} Pokemon slots; {len(replacement)} regional replacement sets; {len(variants)} procedural variants; {len(templates)} procedural templates.')
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text('\n'.join(w.lines)+'\n',encoding='utf-8')
    report=dict(path=str(args.out),native_records=len(emitted),combat_parties=len(branches),pokemon=total,authored_variants=len(branches),
        authored_pokemon_checked=checked,regional_sets=len(replacement),circuit_variants=len(variants),
        circuit_templates=len(templates),bytes=args.out.stat().st_size,
        sha256=hashlib.sha256(args.out.read_bytes()).hexdigest(),
        campaign_without_direct_battle_call=[b.trainer for b in branches if not any(x['battle'] for x in refs[b.trainer])])
    args.out.with_suffix('.validation.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))

if __name__=='__main__':main()
