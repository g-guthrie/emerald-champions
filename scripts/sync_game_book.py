#!/usr/bin/env python3
"""Refresh the Game Book's marked reference appendix from current source.

The guide above BEGIN is hand-authored design. Everything below is disposable
reference data. --check compares bytes, so CI catches source-to-book drift.
No game state, map coordinates, rewards or trainer authoring are changed here.
"""
from __future__ import annotations
import argparse
from collections import defaultdict
from bisect import bisect_right
import hashlib
import json
from pathlib import Path
import re
import subprocess
import time
import tempfile

import emerald_champions_teams as teams
import export_trainer_catalogue as trainers
from verify_trainer_ability_legality import preprocess_species_info, species_aliases, resolve_species
from audit.map_dynamic_inventory import strip_comments
from generate_emerald_champions_mega_archive import stones
from economy_reference import generate as generate_economy

ROOT=Path(__file__).resolve().parents[1]
BOOK=ROOT/'Game Blueprint/Emerald_Champions_Game_Book.txt'
BEGIN='=== BEGIN GENERATED GAME REFERENCE ==='
END='=== END GENERATED GAME REFERENCE ==='


def compact(s):return re.sub(r'\s+',' ',s).strip()
def pretty(s):return re.sub(r'^(?:ITEM|SPECIES|ABILITY|MOVE|NATURE)_','',s).replace('_',' ').title()
def tokens(path):return re.findall(r'\bITEM_\w+', (ROOT/path).read_text())


def split_guide(text):
    if BEGIN not in text:
        if END in text:raise ValueError('orphan generated end marker')
        return text.rstrip()+'\n\n'
    if text.count(BEGIN)!=1 or text.count(END)!=1 or not text.rstrip().endswith(END):
        raise ValueError('expected one complete generated appendix at end of book')
    return text.split(BEGIN)[0].rstrip()+'\n\n'


def source_inventory():
    maps=[];pickup=[];gifts=[];shops=[];pokemon=[]
    for path in sorted((ROOT/'data/maps').glob('*/map.json')):
        m=json.loads(path.read_text())
        if m.get('region')!='REGION_HOENN':continue
        name=path.parent.name;maps.append((path,m))
        for kind in ['object_events','bg_events']:
            for i,row in enumerate(m.get(kind) or [],1):
                item=row.get('item') if kind=='bg_events' else row.get('trainer_sight_or_berry_tree_id')
                if not isinstance(item,str) or not item.startswith('ITEM_'):continue
                if kind=='object_events' and row.get('script')!='Common_EventScript_FindItem':continue
                pickup.append(dict(map=name,kind='hidden' if kind=='bg_events' else 'visible',index=i,x=row['x'],y=row['y'],item=item,flag=row.get('flag'),source=str(path.relative_to(ROOT))))
    paths=[p.with_name('scripts.inc') for p,m in maps if p.with_name('scripts.inc').exists()]+sorted((ROOT/'data/scripts').rglob('*.inc'))
    for path in paths:
        text=strip_comments(path.read_text());label='';source=str(path.relative_to(ROOT))
        for line_no,line in enumerate(text.splitlines(),1):
            if m:=re.match(r'^(\w+)::?$',line):label=m[1]
            if m:=re.match(r'\s*(giveitem|giveuniqueitem|finditem|additem|addpcitem)\s+(ITEM_\w+)(?:\s*,\s*([^\s@]+))?',line):
                gifts.append(dict(source=source,line=line_no,label=label,command=m[1],item=m[2],quantity=m[3] or '1'))
            if m:=re.match(r'\s*(givemon|giveegg|setwildbattle)\s+(SPECIES_\w+)',line):
                pokemon.append(dict(source=source,line=line_no,label=label,command=m[1],species=m[2]))
        # Local stock labels are recorded only when passed to a shop command.
        for command,label in re.findall(r'(?m)^\s*(pokemart\w*)\s+(\w+)',text):
            m=re.search(r'(?ms)^'+re.escape(label)+r'::?\s*\n(.*?)(?=^\w+::?|\Z)',text)
            if m:
                stock=re.findall(r'\.2byte\s+(ITEM_\w+)',m[1])
                if stock:shops.append(dict(source=source,label=label,command=command,items=stock))
    return maps,pickup,gifts,shops,pokemon


def regional_preset_reference():
    """Export actual preset0 inputs; runtime overrides are printed from native C."""
    preset_source=(ROOT/'src/data/pokemon/emerald_champions_battle_sets.h').read_text()
    presets=[trainers.c_fields(x) for x in re.findall(r'\.preset = \{(.*?)\}\}',preset_source,re.S)]
    offsets={sp:int(offset) for sp,offset in re.findall(r'\[EC_BATTLE_FORMAT_DOUBLES\]\[(SPECIES_\w+)\] = \{\.offset = (\d+)',preset_source)}
    starter=(ROOT/'src/starter_choose.c').read_text()
    bases=re.findall(r'SPECIES_\w+',trainers.c_array(starter,'sStarterMons'))
    mids=dict(re.findall(r'case (SPECIES_\w+):\s+return (SPECIES_\w+);',starter.split('enum Species GetMiddleEvolutionForStarter')[1].split('enum Species GetFinalEvolutionForStarter')[0]))
    finals={}
    for a,b,result in re.findall(r'case (SPECIES_\w+):\s+case (SPECIES_\w+):\s+return (SPECIES_\w+);',starter.split('enum Species GetFinalEvolutionForStarter')[1].split('u16 GetStarterPokemon')[0]):finals[a]=finals[b]=result
    rows=['DEFAULT REGIONAL PRESET INPUTS (before the explicit runtime overrides above; these are not final enemy parties)']
    for base in bases:
        for sp in [base,mids[base],finals[base]]:
            p=presets[offsets[sp]]
            rows.append(f"{sp}: item {p['item']}; ability {p['ability']}; nature {p['nature']}; EVs {p['evs']}; moves {p['moves']}")
    rows+=['STEVEN ALLIED PARTY — native authoring', (ROOT/'src/data/battle_partners.party').read_text().strip()]
    return rows


def configured_species():
    text=preprocess_species_info();text=text[text.index('const struct SpeciesInfo gSpeciesInfo[]'):]
    marks=list(re.finditer(r'\[(SPECIES_\w+)\]\s*=\s*\{',text));stats={};evolutions=[]
    for i,m in enumerate(marks):
        body=text[m.end():marks[i+1].start() if i+1<len(marks) else len(text)]
        values=[]
        for field in ['baseHP','baseAttack','baseDefense','baseSpAttack','baseSpDefense','baseSpeed']:
            x=re.search(r'\.'+field+r'\s*=\s*([^,\n]+)',body);values.append(compact(x[1]) if x else '?')
        stats[m[1]]='/'.join(values)
        evo=re.search(r'\.evolutions\s*=\s*(.*?)(?:\n\s*\.\w+\s*=|\n\s*\},?\s*\Z)',body,re.S)
        if evo:
            # Extract the actual configured expression, retaining conditions; no
            # guessing that a raw species relationship is usable in this region.
            value=compact(evo[1]).rstrip(',')
            value=re.sub(r'^\(const struct Evolution\[\]\)\s*','',value)
            value=re.sub(r'\s*([{},()])\s*',r'\1',value)
            evolutions.append((m[1],value))
    aliases=species_aliases()
    for alias in aliases:
        target=resolve_species(alias,aliases)
        if target in stats:stats[alias]=stats[target]
    return stats,evolutions


def dialogue_reference(maps):
    """Keep each literal once, and index every map actor/trigger to its script.

    This is a source transcript atlas. It does not flatten conditional returns,
    standard services or different story states into an invented linear speech.
    GAS and CPP conditions are retained explicitly, including alternate builds.
    """
    lines=['\nF. ALL MAP INTERACTIONS AND SCRIPT DIALOGUE',
        'Map-local IDs and script labels are existing code identities. No new NPC numbering is written to the ROM.',
        'Dialogue variants are source branches, not simultaneous speech. Follow referenced labels; unresolved native/standard services are explicit.',
        'Conditional-build branches remain labeled where present. Shared text is printed once by label, not copied into every NPC.']
    paths=set(); bindings=0
    for path,m in maps:
        name=path.parent.name;lines.append('\nMAP '+name)
        lines.append('  Entry scenes: '+m.get('shared_scripts_map',name)+'_MapScripts')
        for kind in ['object_events','coord_events','bg_events']:
            for i,row in enumerate(m.get(kind) or [],1):
                script=row.get('script')
                if script in [None,0,'0','0x0','NULL']:continue
                bindings+=1
                trigger='; '+', '.join(f'{k}={row[k]}' for k in ['var','var_value','flag'] if k in row)
                lines.append(f"  {kind} {row.get('local_id',i)} ({row.get('x')},{row.get('y')}): {script}{trigger}")
        q=path.with_name('scripts.inc')
        if q.exists():paths.add(q)
    paths.update((ROOT/'data/text').rglob('*.inc'));paths.update((ROOT/'data/scripts').rglob('*.inc'));paths.update((ROOT/'data').glob('*.s'))
    text_count=0;unresolved=set();label_defs=defaultdict(list);native=[];routing={}
    for path in sorted(paths):
        if 'frlg' in str(path).lower():continue
        text=strip_comments(path.read_text());label='';buffer=[];flow=[];conditions=[]
        lines.append('\nSOURCE '+str(path.relative_to(ROOT)))
        def flush():
            nonlocal text_count,buffer,flow
            if buffer:
                value=''.join(buffer).rstrip('$').replace(r'\p',' / ').replace(r'\n',' ').replace(r'\l',' ')
                lines.append('  '+label+': '+value);text_count+=1
            # Routing is indexed separately; the book contains speech, not a
            # second copy of every script instruction.
            if flow:routing[str(path.relative_to(ROOT))+':'+label]=flow.copy()
            buffer=[];flow=[]
        for no,raw in enumerate(text.splitlines(),1):
            line=raw.strip()
            match=re.match(r'^(\w+)::?$',line)
            if match:
                flush();label=match[1];label_defs[label].append((str(path.relative_to(ROOT)),no));continue
            if re.match(r'^[.#](?:if|ifdef|ifndef|elif|else|endif)\b',line):
                flush();lines.append('  [BUILD CONDITION '+line+']');continue
            literals=re.findall(r'\.string\s+"((?:\\.|[^"\\])*)"',line)
            if literals:buffer+=literals;continue
            if not line or line.startswith(('@','#','.')):continue
            command=line.split()[0]
            if command.startswith(('goto','call','vcall','vgoto','trainerbattle','multi_')) or command in {'msgbox','message','case','switch','map_script','map_script_2','dynmultipush','multichoice','multichoicedefault','bufferstring','setwildbattle','givemon','giveegg'}:
                flow.append(compact(line))
            elif command in {'special','specialvar'}:
                flow.append('[NATIVE '+compact(line)+']');native.append((str(path.relative_to(ROOT)),no,compact(line)))
        flush()
    duplicates={label:rows for label,rows in label_defs.items() if len(rows)>1}
    lines+=['\nDIALOGUE COVERAGE',f'{bindings} scripted map bindings; {text_count} script literal blocks; {len(native)} native service callsites.',
        'Native service UI below is a literal-source supplement. Variable strings, localized formatting, player names and choices are retained as placeholders; this atlas is not a runtime transcript.',
        f'Labels with multiple source definitions (often build alternatives): {len(duplicates)}. Conditions above must be honored.']
    # Include the live text providers most likely to speak through shared NPC
    # services. Do not infer actor attribution from a string alone.
    native_paths=sorted(p for p in (ROOT/'src').rglob('*') if p.suffix in {'.c','.h'} and 'frlg' not in str(p).lower())
    # Generated ignored headers must not make a built working tree disagree
    # with CI's clean checkout. New, nonignored source files are still included.
    probe=subprocess.run(['git','check-ignore','--stdin','-z'],cwd=ROOT,
        input='\0'.join(str(p.relative_to(ROOT)) for p in native_paths)+'\0',capture_output=True,text=True)
    if probe.returncode not in (0,1) and 'not a git repository' not in probe.stderr:
        raise ValueError(probe.stderr)
    ignored=set(probe.stdout.split('\0')) if probe.returncode==0 else set()
    native_paths=[p for p in native_paths if str(p.relative_to(ROOT)) not in ignored]
    native_count=0
    for path in native_paths:
        if not path.exists():continue
        paths.add(path);text=strip_comments(path.read_text());native_rows=[]
        pattern=r'(?:COMPOUND_STRING|_)\s*\(((?:\s*"(?:\\.|[^"\\])*")+?)\s*\)|(?:const\s+u8|static\s+const\s+u8)\s+(\w+)\[\]\s*=\s*_?\(?((?:\s*"(?:\\.|[^"\\])*")+)' 
        for match in re.finditer(pattern,text):
            literal=match[1] or match[3];parts=re.findall(r'"((?:\\.|[^"\\])*)"',literal)
            value=''.join(parts).rstrip('$').replace(r'\p',' / ').replace(r'\n',' ').replace(r'\l',' ')
            native_rows.append(f"  {match[2] or 'line '+str(text.count(chr(10),0,match.start())+1)}: {value}");native_count+=1
        # Preserve otherwise-unclassified literal text as candidates, including
        # pointer arrays and formatting fragments. Includes are not speech.
        covered=[m.span() for m in re.finditer(pattern,text)]
        covered_starts=[a for a,b in covered]
        for m in re.finditer(r'"(?:\\.|[^"\\])*"',text):
            index=bisect_right(covered_starts,m.start())-1
            if index>=0 and m.start()<covered[index][1]:continue
            line_start=text.rfind('\n',0,m.start())+1
            if text[line_start:m.start()].lstrip().startswith('#include'):continue
            value=m[0][1:-1]
            if not value:continue
            native_rows.append(f'  literal candidate line {text.count(chr(10),0,m.start())+1}: {value}');native_count+=1
        if native_rows:lines+=['\nNATIVE TEXT SOURCE '+str(path.relative_to(ROOT)),*native_rows]
    lines.append(f'Native literal supplement:{native_count}. Includes UI/battle/item/dex text in native C/header files, which is broader than NPC speech. Literal candidates may be code/UI labels rather than speech; dynamic runtime values and encoded assets remain templates/unresolved, not invented dialogue. Missing coverage must remain visible during an area audit.')
    lines+=['\nDIALOGUE BRANCH/CALL INDEX',
        'This index binds NPC entry scripts to text, shared calls, choices and conditions. It is lookup data, not additional spoken dialogue.']
    for label,actions in routing.items():lines.append(label+' => '+'; '.join(actions))
    return lines,paths,dict(script_bindings=bindings,script_literal_blocks=text_count,native_literal_blocks=native_count,duplicate_labels=len(duplicates)),routing


def render_reference():
    branches=teams.read_teams();parties=trainers.parse_parties();slots=trainers.native_vs_authoring(parties,branches)
    meta={n:trainers.fields(block[:teams.BRANCH_RE.search(block).start()] if teams.BRANCH_RE.search(block) else block) for n,block in teams.split_encounters(teams.MASTER.read_text())[1]}
    maps,pickups,gifts,shops,pokemon=source_inventory();base,evolutions=configured_species()
    lines=[BEGIN,'Generated by python3 scripts/sync_game_book.py --write. Do not hand-edit this appendix.',
        'This is current source reference, not proof of reachability, narrative agreement or native playtesting.',
        'The guide above owns intended experience. Source files own exact records; --check rejects stale generated text.\n',
        'A. EXACT TRAINER TEAMS AND PLANS',
        f'{len(branches)} authored variants; {slots} Pokemon slots. HP/Atk/Def/SpA/SpD/Spe is the order for EVs, IVs and base stats.',
        'Offsets apply to the live cap; Easy −2, Medium0, Hard+2. First two slots lead in single-owner doubles; multi battles use one per owner.',
        'Defaults: IVs31/31/31/31/31/31 and friendship255 unless printed. Base stats are species values, not level-adjusted or item/ability-modified battle stats.',
        'Hoenn rival entries are seed parties; non-Hoenn substitutions use the regional resolver described in section B. Procedural Circuit/Tent teams are outside this reference by user request.\n']
    plans={};seen_builds={};builds=[]
    for b in branches:
        ids=[]
        for mon in b.mons:
            key=(mon.species,mon.item,mon.ability,mon.nature,mon.evs,tuple(mon.moves),mon.ivs,mon.friendship)
            if key not in seen_builds:
                ident=f'B{len(builds)+1:04}';seen_builds[key]=ident;builds.append((ident,mon))
            ids.append(f'{seen_builds[key]}({mon.offset:+})')
        loc=meta[b.encounter].get('location','unknown')
        lines.append(f'E{b.encounter:04} {b.trainer} | {loc} | {b.cls} | '+','.join(ids))
        lines.append('  Strategy '+(','.join(b.strategy) or 'none')+'; traits '+(','.join(b.ai) or 'none')+'; Mega slots '+(','.join(str(i+1) for i in range(len(b.mons)) if (b.mega_slots or 0)&(1<<i)) or 'none'))
        if b.tactics:lines.append('  Tactics: '+'; '.join('/'.join(t) for t in b.tactics))
        if b.plan in plans:lines.append('  Plan: same as '+plans[b.plan])
        else:lines.append('  Plan: '+b.plan);plans[b.plan]=b.trainer
        if b.crack not in b.plan:lines.append('  Counterplay: '+b.crack)
    lines+=['\nBUILD TABLE (generated keys are references, not persistent gameplay IDs)']
    for ident,m in builds:
        extra=('; IV '+m.ivs if m.ivs!='31/31/31/31/31/31' else '')+('; friendship '+str(m.friendship) if m.friendship!=255 else '')
        lines.append(f'{ident} {pretty(m.species)} @{pretty(m.item)} | {pretty(m.ability)} | {pretty(m.nature)} | EV {m.evs}{extra} | '+','.join(pretty(x) for x in m.moves))
    lines+=['\nCONFIGURED BASE STATS (HP/Atk/Def/SpA/SpD/Spe)']
    for sp in sorted({m.species for b in branches for m in b.mons}):
        if 'SPECIES_'+sp not in base:raise ValueError('missing configured species stats '+sp)
        lines.append(pretty(sp)+' '+base['SPECIES_'+sp])
    lines+=['\nB. REGIONAL STARTERS — NATIVE RESOLVER',
        'The unchosen third regional starter replaces the corresponding Hoenn seed slot. Exact runtime rules below are copied from their owner rather than hand-reimplemented in this generator.',
        'src/emerald_champions_opening.c (includes first-rescue setup and starter/rival overrides):',
        (ROOT/'src/emerald_champions_opening.c').read_text().strip(),
        'The full shared preset corpus is src/data/pokemon/emerald_champions_battle_sets.h; do not treat all presets as simultaneous rival alternatives.']
    lines+=regional_preset_reference()
    paid=tokens('src/data/emerald_champions_paid_evolution_items.h');forms=tokens('src/data/emerald_champions_form_items.h');mega=stones()
    economy_lines,economy_catalog,economy_paths=generate_economy(ROOT)
    lines+=economy_lines
    lines+=['\nD. ORDINARY WILD TABLES — configuration, not first-access proof',
        'Printed levels are encounter-table seeds before runtime cap normalization; step-rate parameters are not slot percentages. Fishing probabilities are separate per rod.']
    wild=json.loads((ROOT/'src/data/wild_encounters.json').read_text());mapids={m['id'] for p,m in maps};table_count=0
    for group in wild['wild_encounter_groups']:
        if not group.get('for_maps'):continue
        fields={f['type']:f for f in group['fields']}
        for entry in group['encounters']:
            if entry.get('map') not in mapids:continue
            for kind,field in fields.items():
                if kind not in entry:continue
                data=entry[kind];mons=data['mons'];rates=field['encounter_rates'];assert len(mons)==len(rates)
                groups=field.get('groups', {kind:list(range(len(mons)))})
                for method,indexes in groups.items():
                    parts=[f"{pretty(mons[i]['species'])} L{mons[i]['min_level']}–{mons[i]['max_level']} {rates[i]}%" for i in indexes]
                    lines.append(entry['map']+' '+method+' step-rate '+str(data['encounter_rate'])+': '+', '.join(parts));table_count+=1
    lines+=['\nLEGENDARY PROVIDER DEFINITIONS — source flags/conditions still apply']
    lines += [line for line in (ROOT/'src/data/pokemon/legendary_signs.h').read_text().splitlines() if re.match(r'^\w+_SIGN\(',line)]
    lines+=['\nE. CONFIGURED EVOLUTION RELATIONSHIPS',
        'Expressions retain actual conditions. A configured relationship does not establish that its region, item or destination is reachable now.']
    for species,value in evolutions:lines.append(pretty(species)+': '+value)
    dialogue_lines,dialogue_paths,dialogue_coverage,routing=dialogue_reference(maps)
    lines+=dialogue_lines
    source_paths={Path(__file__),ROOT/'scripts/emerald_champions_teams.py',ROOT/'scripts/export_trainer_catalogue.py',teams.TEAMS,teams.MASTER,ROOT/'src/data/trainers.party',ROOT/'src/emerald_champions_opening.c',ROOT/'src/mega_stone_rewards.c',ROOT/'src/caps.c',ROOT/'src/field_specials.c',ROOT/'src/item.c',ROOT/'src/data/items.h',ROOT/'src/data/wild_encounters.json',ROOT/'src/data/pokemon/legendary_signs.h',ROOT/'src/data/pokemon/form_change_tables.h'}
    source_paths.update(p for p,m in maps);source_paths.update(p.with_name('scripts.inc') for p,m in maps if p.with_name('scripts.inc').exists());source_paths.update((ROOT/'data/scripts').rglob('*.inc'));source_paths.update((ROOT/'src/data/pokemon/species_info').glob('*.h'));source_paths.update((ROOT/'include/config').glob('*.h'));source_paths.update(ROOT/p for p in ['src/data/emerald_champions_paid_evolution_items.h','src/data/emerald_champions_form_items.h','src/data/emerald_champions_mega_stones.h'])
    source_paths.update(dialogue_paths)
    source_paths.update(economy_paths)
    source_paths.add(ROOT/'.gitignore')
    source_paths.add(ROOT/'Makefile')
    source_paths.update(ROOT/p for p in ['src/data/pokemon/emerald_champions_battle_sets.h','src/starter_choose.c','src/data/battle_partners.party','include/constants/species.h','scripts/verify_trainer_ability_legality.py','scripts/audit/map_dynamic_inventory.py','scripts/generate_emerald_champions_mega_archive.py'])
    sha=hashlib.sha256()
    for path in sorted(source_paths):sha.update(str(path.relative_to(ROOT)).encode()+b'\0'+path.read_bytes()+b'\0')
    lines+=['\nREFERENCE COVERAGE',f'Authoring: {len(branches)} trainer variants/{slots} slots; {len(builds)} unique builds. Map definitions:{len(maps)}. Complete economy census and unresolved conditions are in C6; wild method tables:{table_count}; configured evolution rows:{len(evolutions)}.',
        'Known limits: this is not a full transcript, runtime access/necessity proof, stock-condition simulator or battle acceptance. Native providers and dynamic regional choices must be followed through their source owner. No claim that prose/code drift is impossible.',
        'Source SHA256: '+sha.hexdigest(),END]
    economy_catalog={k:v for k,v in economy_catalog.items() if k!='source_paths'}
    inventory=dict(economy=economy_catalog,dialogue_coverage=dialogue_coverage,dialogue_routing=routing,paid_evolution_items=paid,form_items=forms,mega_stones=mega,pickups=pickups,literal_deliveries=gifts,local_shops=shops,literal_pokemon_providers=pokemon)
    return '\n'.join(x.rstrip() for x in lines)+'\n',inventory


def atomic_book_write(text):
    # Build hooks and the live watcher can refresh together; each write has its
    # own temporary file and readers only see a complete old or new reference.
    with tempfile.NamedTemporaryFile(mode='w',encoding='utf-8',dir=BOOK.parent,prefix='.book-',suffix='.tmp',delete=False) as handle:
        temporary=Path(handle.name)
        try:handle.write(text)
        except BaseException:
            temporary.unlink(missing_ok=True)
            raise
    temporary.replace(BOOK)


def watch_snapshot():
    paths=subprocess.check_output(['git','ls-files','-z','--cached','--others','--exclude-standard','--',
        'src','data','include','scripts','.gitignore','Makefile'],cwd=ROOT,text=True).split('\0')
    rows={}
    for name in set(paths)-{''}:
        path=ROOT/name
        if path.is_file():
            st=path.stat();rows[name]=(st.st_mtime_ns,st.st_size)
    # The generated appendix must not retrigger its own watcher.
    rows['@guide']=hashlib.sha256(split_guide(BOOK.read_text()).encode()).hexdigest()
    return rows


def watch(inventory_path=None,interval=2):
    previous=None;last_error=None
    print('Watching game source; generated book refreshes automatically. Ctrl-C stops.',flush=True)
    while True:
        try:
            current=watch_snapshot()
            if current!=previous:
                # Let an editor finish an atomic save/burst before exporting.
                time.sleep(0.4)
                if watch_snapshot()!=current:continue
                team_path=str(teams.TEAMS.relative_to(ROOT))
                if previous is not None and current.get(team_path)!=previous.get(team_path):
                    # Exact teams are authored once; these files are projections.
                    projections=['src/data/trainers.party','src/data/emerald_champions_battle_plans.h']
                    changed_together=any(current.get(p)!=previous.get(p) for p in projections)
                    # A normal materialization edits both sides. Verify that
                    # completed work instead of blocking it indefinitely.
                    mode='--check' if changed_together else '--write'
                    subprocess.run(['python3',str(ROOT/'scripts/emerald_champions_teams.py'),mode],cwd=ROOT,check=True,stdout=subprocess.DEVNULL)
                before_refresh=watch_snapshot()
                command=['python3',str(Path(__file__).resolve()),'--write']
                if inventory_path:command+=['--inventory',str(inventory_path)]
                result=subprocess.run(command,cwd=ROOT,check=True,capture_output=True,text=True)
                previous=before_refresh;last_error=None
                print('Book refreshed: '+result.stdout.strip(),flush=True)
        except (OSError,ValueError,AssertionError,subprocess.CalledProcessError,SystemExit) as error:
            message=(error.stderr or error.stdout or str(error)).strip() if isinstance(error,subprocess.CalledProcessError) else str(error)
            if message!=last_error:print('Book refresh blocked: '+message,flush=True);last_error=message
        time.sleep(interval)


def main():
    p=argparse.ArgumentParser(description=__doc__);g=p.add_mutually_exclusive_group(required=True);g.add_argument('--write',action='store_true');g.add_argument('--check',action='store_true');g.add_argument('--watch',action='store_true');p.add_argument('--inventory',type=Path)
    args=p.parse_args()
    if args.watch:
        watch(args.inventory)
        return
    before=watch_snapshot();before.pop('@guide',None)
    old=BOOK.read_text();guide=split_guide(old);reference,inventory=render_reference()
    after=watch_snapshot();after.pop('@guide',None)
    if before!=after:raise SystemExit('Source changed during reference extraction; retry after edits finish.')
    # Preserve a guide edit made while the slow source extraction was running.
    if args.write:guide=split_guide(BOOK.read_text())
    new=guide+reference
    reading='GENERATED READING VIEW — edit the guide in Emerald_Champions_Game_Book.txt, then run sync_game_book.py --write.\n\n'+guide
    if args.write:
        atomic_book_write(new)
        (BOOK.parent/'Game_Guide_Reading_Copy.txt').write_text(reading)
    elif old!=new:raise SystemExit('Game Book reference is stale: run python3 scripts/sync_game_book.py --write')
    elif not (BOOK.parent/'Game_Guide_Reading_Copy.txt').exists() or (BOOK.parent/'Game_Guide_Reading_Copy.txt').read_text()!=reading:
        raise SystemExit('Game Guide reading view is stale: run python3 scripts/sync_game_book.py --write')
    if args.inventory:args.inventory.parent.mkdir(parents=True,exist_ok=True);args.inventory.write_text(json.dumps(inventory,indent=2)+'\n')
    print(f'PASS: {len(guide.split()):,} guide words; {len(new.encode()):,} total bytes; generated source reference '+('updated' if args.write else 'current'))

if __name__=='__main__':main()
