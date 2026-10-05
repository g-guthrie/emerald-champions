#!/usr/bin/env python3
"""Source records and condition-preserving Pokemon derivations for the manifest.

This module deliberately does not assign campaign windows. Callers supply physical
reachability, current cap, acquired items and defeated/story state. Species pools
are alternatives across possible saves; they do not assert one save owns every
starter, mutually exclusive gift, or transient battle form simultaneously.
"""
from __future__ import annotations
import argparse
import json
import re
from collections import defaultdict
from functools import lru_cache
from pathlib import Path
import reference_pool as rp

ROOT = rp.ROOT


def _citation(path, needle):
    return rp.cite(ROOT / path, needle)


def _species_bodies():
    text = rp.preprocess_species_info()
    marks = list(re.finditer(r"\[(SPECIES_\w+)\]\s*=\s*\{", text))
    return {m[1]: text[m.end():marks[i+1].start() if i+1 < len(marks) else len(text)]
            for i, m in enumerate(marks)}


class PokemonSources:
    def __init__(self, builder=None):
        self.builder = builder or rp.Builder()
        self.sd = self.builder.species
        self.sources = [rp.Source(s.kind,s.key,s.where,s.requires,s.detail,s.cite,s.needs_species,s.extra,s.needs_items) for s in self.builder.species_sources]
        # Feebas fishing is possible from shore, without a Surf licence.
        self.sources = [s for s in self.sources if 'Route119 Feebas tiles' not in s.detail]
        for where in self.builder._fish_where('Route119'):
            for rod in ('HAS_OLD_ROD', 'HAS_GOOD_ROD', 'HAS_SUPER_ROD'):
                self.sources.append(rp.Source('wild', 'SPECIES_FEEBAS', where, [rod],
                    'Route119 native Feebas fishing tiles; any rod; six seeded tiles',
                    _citation('src/wild_encounter.c', 'bool8 CheckFeebasAtCoords')))
        for item, species in self.builder.fossils.items():
            self.sources.append(rp.Source('fossil', self.sd.resolve(species), 'RustboroCity_DevonCorp_2F', None,
                f'Revive {item} at Devon laboratory; fossil consumed',
                _citation('src/field_specials.c', 'sRevivableFossils'), needs_items=(item,)))
        self.sources.append(rp.Source('egg','SPECIES_TOGEPI','Route117_PokemonDayCare',None,
            'Initial Day-Care lady Togepi egg; before daily random egg pool',
            _citation('data/maps/Route117_PokemonDayCare/scripts.inc','giveegg VAR_0x8004')))
        self.bodies = _species_bodies()
        self.species_cites = {}
        for path in sorted((ROOT/'src/data/pokemon/species_info').glob('*.h')):
            for n, line in enumerate(path.read_text().splitlines(), 1):
                m = re.search(r'\[(SPECIES_\w+)\]', line)
                if m:
                    self.species_cites.setdefault(self.sd.resolve(m[1]), f'{path.relative_to(ROOT)}:{n}')
        self.rows = self._rows()
        self.evolutions = self._evolutions()
        self.forms = self._forms()
        self.breeding = self._breeding_info()
        self._moves = self._preparation_moves()
        self._move_types = self._read_move_types()
        iconic=(ROOT/'src/data/pokemon/emerald_champions_iconic_moves.h').read_text()
        self.iconic_moves=[dict(species=self.sd.resolve(sp),move=move,badges=int(badges)) for sp,move,badges in re.findall(r'\{(SPECIES_\w+),\s*(MOVE_\w+),\s*SPECIES_\w+,\s*(\d+)',iconic)]
        self.eligibility_diagnostics = []

    def _rows(self):
        out = []
        seen = set()
        regions = {self.sd.resolve(sp): i+1 for i, trio in enumerate(self.builder.starter_regions) for sp in trio}
        for s in self.sources:
            if s.kind == 'storm_visitor':
                legend_req, needs, note = self.builder.legend_requirement(s.key)
                s.requires = tuple(tuple(dict.fromkeys(list(a) + legend_req)) for a in s.requires)
                s.needs_species = tuple(dict.fromkeys(s.needs_species + needs))
            row = dict(species=s.key, name=self.sd.name(s.key), kind=s.kind,
                       where=list(s.where) if isinstance(s.where, tuple) else s.where,
                       requires=[list(a) for a in s.requires], needs_species=list(s.needs_species),
                       needs_items=list(s.needs_items), detail=s.detail,
                       citations=[s.cite], conditions=[])
            if self.sd.restricted_class(s.key):
                row['conditions'].append('Restricted spawn stops after its species/form is caught; one restricted Pokemon in party total')
                row['citations'].append(_citation('src/legendary_signs.c', 'MeetsSignDiscovery'))
            base = self.sd.base(s.key)
            if base in ('SPECIES_MELOETTA', 'SPECIES_LANDORUS', 'SPECIES_MARSHADOW'):
                quest = base.removeprefix('SPECIES_')
                for alt in row['requires']:
                    alt.append('QUEST_LEGENDARY_' + quest)
                row['conditions'].append({'MELOETTA': 'Visit DewfordMeadow with a party Pokemon knowing Sing',
                    'LANDORUS': 'Visit Route111_RuinsExterior with Castform-family in party',
                    'MARSHADOW': 'Visit Route113_GlassWorkshop after meeting cumulative soot target'}[quest])
                row['citations'].append(_citation('src/legendary_signs.c', 'void TryUnlockLocalLegendaryDiscovery'))
            gate = self.builder.gates.get(s.key, self.builder.gates.get(base, {}))
            if s.kind == 'wild' and gate.get('visitor'):
                for alt in row['requires']:
                    alt.append('FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE')
                row['conditions'].append('Storm visitor resident slot is inert until skies calm')
                row['citations'].append(_citation('src/weather_anomaly.c', 'bool32 IsWeatherAnomalyVisitorSlotInert'))
            s.requires = tuple(tuple(a) for a in row['requires'])
            if s.kind == 'starter':
                row['starter_region'] = regions.get(s.key)
                row['conditions'].append('At most two of the three starters, both from the one selected region')
            if s.kind == 'static':
                row['conditions'].append('Capture branch grants ownership; defeat/knockout branch grants no Pokemon and may permanently lose a legend')
            if s.kind == 'storm_visitor':
                row['conditions'].append('Available on an active matching storm; after skies calm use resident table; DexNav cannot summon storm visitors')
            if s.kind == 'wild':
                if 'land_mons' in s.detail or 'water_mons' in s.detail:
                    row['conditions'].append('Ordinary encounter or DexNav; Sweet Scent reweights this same legal roster and adds no new species')
                if 'honey_mons' in s.detail:
                    row['conditions'].append('Use Honey on a legal land encounter tile')
            if s.kind == 'trade':
                row['conditions'].append('Exchange requested exact species; the donor is consumed; one-time NPC trade')
            key = json.dumps(row, sort_keys=True)
            if key in seen:
                continue
            seen.add(key)
            row['id'] = f'pokemon-source-{len(out)+1:05d}'
            out.append(row)
        return out

    def enrich_eligibility(self, parser=None):
        """Attach complete control-flow paths, including negative flags and vars.

        Demand driven and cached by acquisition command; callers may reuse their
        battle parser. Native C gift specials are anchored at their real command.
        """
        from manifest_battle_progression import ProgressionParser, command, is_battle
        parser = parser or ProgressionParser(self.builder.scripts)
        import economy_reference as er
        blocks={label:[(str(info['file'].relative_to(ROOT)),info['line'], '\n'.join(text for _,text in info['body']))]
                for label,info in self.builder.scripts.labels.items() if info.get('map')=='MauvilleCity_GameCorner'}
        offers={self.sd.resolve(r['species']):r for r in er.game_corner_offers(ROOT,blocks, {})[0]}
        index = {}
        for label, info in self.builder.scripts.labels.items():
            for line, text in info['body']:
                index[(str(info['file'].relative_to(ROOT)), line)] = label
        special = {
            'SPECIES_COSMOG': ('LittlerootTown_ProfessorBirchsLab_EventScript_GiveCosmog', 'GiveBirchCosmogPair'),
            'SPECIES_MAGEARNA': ('RustboroCity_DevonCorp_2F_EventScript_PokemonDreamsScientist', 'TryGiveSelectedLegendarySignReward'),
            'SPECIES_ARCEUS_NORMAL': ('RustboroCity_DevonCorp_2F_EventScript_TryArceus', 'TryGiveArceusLegendarySignMasteryReward'),
        }
        cache = getattr(parser, '_manifest_pokemon_paths', None)
        if cache is None:
            cache = {}; parser._manifest_pokemon_paths = cache
        for row in self.rows:
            if row['kind'] not in ('gift','egg','trade','static','game_corner','fossil'): continue
            targets = []
            for citation in row['citations']:
                for path, number in re.findall(r'((?:data/)?(?:maps|scripts)/[^; :]+|src/[^; :]+):(\d+)', citation):
                    if path.startswith('maps/'): path='data/'+path
                    if path.startswith('scripts/'): path='data/'+path
                    key=(path,int(number))
                    if key in index: targets.append((index[key],int(number)))
            if row['kind']=='egg' and row['detail']=='Day-Care lady daily gift egg':
                label='Route117_PokemonDayCare_EventScript_CheckEggSpace'
                targets=[(label,n) for n,text in self.builder.scripts.labels.get(label,{}).get('body',[]) if 'special SetSpeciesAndEggMove' in text]
            if row['kind']=='game_corner' and 'starter archive' in row['detail']:
                targets=[(label,n) for label,info in self.builder.scripts.labels.items() for n,text in info['body'] if info.get('map')=='MauvilleCity_GameCorner' and 'special GiveEmeraldChampionsGameCornerPokemon' in text]
            elif row['kind']=='game_corner':
                targets=[(label,n) for label,info in self.builder.scripts.labels.items() for n,text in info['body'] if info.get('map')=='MauvilleCity_GameCorner' and (text.startswith('givemon VAR_TEMP_1') or text=='special GiveEmeraldChampionsGameCornerPokemon')]
            if row['kind']=='fossil':
                targets=[(label,n) for label,info in self.builder.scripts.labels.items() for n,text in info['body'] if info.get('map')=='RustboroCity_DevonCorp_2F' and text.startswith('givemon VAR_0x8006')]
            if row['kind']=='trade':
                where=row['where']; map_name=where[0] if isinstance(where,list) else where
                targets=[(label,n) for label,info in self.builder.scripts.labels.items() for n,text in info['body']
                    if info.get('map')==map_name and text=='special DoInGameTradeScene']
            if row['species'] in special and row['kind']=='gift':
                label, needle=special[row['species']]
                targets=[(label,n) for n,text in self.builder.scripts.labels.get(label,{}).get('body',[]) if needle in text]
            paths=[]
            for target in dict.fromkeys(targets):
                if target not in cache: cache[target]=parser.paths_to(*target)
                paths.extend(cache[target])
            where=row['where']; map_name=where[0] if isinstance(where,list) else where
            if str(map_name).startswith('BattleFrontier'):
                row['campaign_scope']='post-finale raw appendix'
            paths=[p for p in paths if not p.get('event') or p['event'][0]==map_name]
            if row['kind']=='game_corner':
                paths=[p for p in paths if self.sd.resolve(p.get('assignments',{}).get('VAR_TEMP_1',''))==row['species']]
                offer=offers.get(row['species'])
                if offer:row['cost']={'currency':'coins','amount':offer['coins'],'source':offer['source']}
                row['needs_items']=list(dict.fromkeys(row['needs_items']+['ITEM_COIN_CASE']))
                row['conditions'].append('Enough coins for the selected prize; coin purchase requires sufficient cash; native claim and inventory checks apply')
            if row['species']=='SPECIES_TOGEPI' and row['kind']=='egg':
                paths=[p for p in paths if p.get('assignments',{}).get('VAR_0x8004')=='SPECIES_TOGEPI']
            row['eligibility_targets']=[dict(label=label,line=line) for label,line in dict.fromkeys(targets)]
            if row['kind']=='static':
                # Ownership is granted by catching the encounter, never by
                # merely reaching the encounter setup command.
                row['ownership_condition']='successful_capture'
                row['capture_sites']=[]
                if map_name=='SouthernIsland_Interior':
                    row['capture_sites']=[f'{lab}:{n}' for lab,info in self.builder.scripts.labels.items() for n,text in info['body']
                        if info.get('map')==map_name and text=='special BattleSetup_StartLatiBattle']
                for target in dict.fromkeys(targets):
                    label,line=target
                    body=self.builder.scripts.labels[label]['body']
                    start=next((i for i,(n,_) in enumerate(body) if n==line),len(body))
                    queue=[(label,start)]; seen=set()
                    while queue:
                        lab,ix=queue.pop()
                        if (lab,ix) in seen or lab not in self.builder.scripts.labels:continue
                        seen.add((lab,ix)); statements=self.builder.scripts.labels[lab]['body']
                        if ix>=len(statements):
                            following=self.builder.scripts.next_label.get(lab)
                            if following:queue.append((following,0))
                            continue
                        n,text=statements[ix]; op,args=command(text)
                        if is_battle(op,args):
                            site=f'{lab}:{n}'
                            if site not in row['capture_sites']:row['capture_sites'].append(site)
                            continue
                        if op in ('end','return','releaseall_end','release_end'):continue
                        if op=='goto':queue.append((args[-1],0));continue
                        if op.startswith(('goto_if','call_if')) or op in ('call','case'):queue.append((args[-1],0))
                        queue.append((lab,ix+1))
            row['eligibility_paths']=paths
            if not paths:
                row['eligibility_status']='post-finale raw appendix' if row.get('campaign_scope') else 'unresolved'
            else: row['eligibility_status']='source_control_flow'
            # Source clone extra is consumed by the physical-state engine.
            for source in self.sources:
                if source.key==row['species'] and source.kind==row['kind'] and source.cite in row['citations']:
                    source.extra['eligibility_paths']=paths
                    source.extra['eligibility_status']=row['eligibility_status']
        self.eligibility_diagnostics=list({json.dumps(d,sort_keys=True):d for d in parser.diagnostics}.values())
        return self.rows

    def _read_move_types(self):
        text=(ROOT/'src/data/moves_info.h').read_text()
        out={}
        for block in re.split(r'\n    \[MOVE_',text)[1:]:
            m=re.search(r'\.type\s*=\s*(TYPE_\w+)',block)
            if m: out['MOVE_'+block.split(']')[0].strip()]=m[1]
        return out

    def can_learn_type(self,species,move_type):
        return any(self._move_types.get(move)==move_type for move in self.preparation_moves(species))

    def _evolutions(self):
        out = []
        for species, info in self.sd.info.items():
            for method, param, target, conditions in info['evolutions']:
                out.append(dict(species=species, target=self.sd.resolve(target), method=method,
                    parameter=param, conditions=[dict(condition=c, argument=a) for c,a in conditions],
                    citations=[self.species_cites.get(species, 'src/data/pokemon/species_info'),
                        _citation('src/pokemon.c', 'enum Species GetEvolutionTargetSpecies'),
                        _citation('src/pokemon.c', 'case IF_GENDER:')]))
        return out

    def _forms(self):
        text = rp.mr.FORM_TABLES.read_text()
        tables = {}
        for m in re.finditer(r'static const struct FormChange (\w+)\[\]\s*=\s*\{(.*?)\};', text, re.S):
            tables[m[1]] = [(method, self.sd.resolve(target), [a.strip() for a in args.split(',') if a.strip()],
                f'src/data/pokemon/form_change_tables.h:{text[:m.start()].count(chr(10))+1}')
                for method, target, args in re.findall(r'\{\s*(FORM_CHANGE_\w+)\s*,\s*(SPECIES_\w+)([^{}]*)\}', m[2])]
        out = []
        for sp, body in self.bodies.items():
            owner = re.search(r'\.formChangeTable\s*=\s*(\w+)', body)
            if not owner:
                continue
            for method, target, args, citation in tables.get(owner[1], []):
                if sp == target:
                    continue
                out.append(dict(species=self.sd.resolve(sp), target=target, method=method,
                    parameters=args, transient=method.startswith(('FORM_CHANGE_BATTLE_', 'FORM_CHANGE_BEGIN_BATTLE')),
                    citations=[citation]))
        # Fusions use a distinct native table; require BOTH Pokemon and the tool.
        for item, base, donor, target in re.findall(r'\{\d+,\s*(ITEM_\w+),\s*(SPECIES_\w+),\s*(SPECIES_\w+),\s*(SPECIES_\w+)', text):
            out.append(dict(species=self.sd.resolve(base), target=self.sd.resolve(target), method='FUSION',
                parameters=[item, self.sd.resolve(donor)], transient=False,
                citations=[_citation('src/data/pokemon/form_change_tables.h', item)]))
        # Cozmo's meteorite offers all Deoxys forms independently of relic tools.
        for target in ('SPECIES_DEOXYS', 'SPECIES_DEOXYS_ATTACK', 'SPECIES_DEOXYS_DEFENSE', 'SPECIES_DEOXYS_SPEED'):
            out.append(dict(species='SPECIES_DEOXYS', target=target, method='COZMO_METEORITE',
                parameters=['FallarborTown_CozmosHouse'], transient=False,
                citations=[_citation('data/maps/FallarborTown_CozmosHouse/scripts.inc', 'SPECIES_DEOXYS_ATTACK')]))
        return out

    def _breeding_info(self):
        out = {}
        for sp, body in self.bodies.items():
            groups = re.search(r'\.eggGroups\s*=\s*\{([^}]*)\}', body)
            gender = re.search(r'\.genderRatio\s*=\s*([^,]+)', body)
            types = re.search(r'\.types\s*=\s*\{([^}]*)\}', body)
            out[self.sd.resolve(sp)] = dict(egg_groups=re.findall(r'EGG_GROUP_\w+', groups[1]) if groups else [],
                gender=gender[1].strip() if gender else '', types=re.findall(r'TYPE_\w+', types[1]) if types else [])
        return out

    def _preparation_moves(self):
        path = ROOT/'src/data/pokemon/emerald_champions_preparation_learnsets.h'
        text = path.read_text()
        arrays = {name:set(re.findall(r'MOVE_\w+',body)) - {'MOVE_UNAVAILABLE'}
            for name,body in re.findall(r'static const u16 (\w+)\[\]\s*=\s*\{(.*?)\};',text,re.S)}
        moves = {self.sd.resolve(sp):set(arrays.get(name,()))
            for sp,name in re.findall(r'\[(SPECIES_\w+)\]\s*=\s*(\w+)',text)}
        presets = (ROOT/'src/data/pokemon/emerald_champions_battle_sets.h').read_text()
        sets = [set(re.findall(r'MOVE_\w+',b)) for b in re.findall(r'\.moves\s*=\s*\{([^}]+)\}',presets)]
        for sp,start,count in re.findall(r'\[EC_BATTLE_FORMAT_\w+\]\[(SPECIES_\w+)\]\s*=\s*\{\.offset\s*=\s*(\d+),\s*\.count\s*=\s*(\d+)',presets):
            pool = moves.setdefault(self.sd.resolve(sp),set())
            for entry in sets[int(start):int(start)+int(count)]: pool.update(entry)
        for pool in moves.values():
            if 'MOVE_HAIL' in pool: pool.add('MOVE_SNOWSCAPE')
        return moves

    def preparation_moves(self,species):
        species=self.sd.resolve(species)
        if species in self._moves:return self._moves[species]
        return self._moves.get(self.sd.base(species),set())

    def can_learn(self,species,move):
        return move in self.preparation_moves(species)

    @lru_cache(maxsize=None)
    def can_learn_field(self,species,move):
        """Native preparation/signature branch of SpeciesCanLearnFieldMove.

        Signature compatibility is unconditional here; the field licence and
        badge checks independently gate use. Center tutor offers no signatures.
        """
        species=self.sd.resolve(species)
        if self.can_learn(species,move): return True
        base=self.sd.base(species)
        return any(row['species']==base and row['move']==move for row in self.iconic_moves)

    def _mate(self, species, have):
        info = self.breeding.get(species, {})
        groups = set(info.get('egg_groups', []))
        ditto = any('EGG_GROUP_DITTO' in self.breeding.get(s, {}).get('egg_groups', []) for s in have)
        if species == 'SPECIES_MANAPHY':
            return 'SPECIES_DITTO' if ditto else None
        if not groups or 'EGG_GROUP_NO_EGGS_DISCOVERED' in groups or 'EGG_GROUP_DITTO' in groups:
            return None
        if ditto:
            return 'SPECIES_DITTO'
        gender = info.get('gender', '')
        # Mixed-gender repeatable species can supply both sexes. For single-sex
        # species an opposite-sex, overlapping-group mate must exist.
        if gender not in ('MON_GENDERLESS', 'MON_MALE', 'MON_FEMALE'):
            return species
        if gender == 'MON_GENDERLESS':
            return None
        for mate in have:
            mi = self.breeding.get(mate, {})
            mg = mi.get('gender', '')
            if mg not in ('MON_GENDERLESS', gender) and groups.intersection(mi.get('egg_groups', [])) and 'EGG_GROUP_NO_EGGS_DISCOVERED' not in mi.get('egg_groups', []):
                return mate
        return None

    def derive(self, have_species, items, cap, reachable, flags=(), context=None):
        """Conditional closure over exact state; unresolved native checks stay visible.

        Optional context callbacks: knows_move(species,move), knows_move_type,
        weather(token), condition(species,condition,arg), repeatable(species).
        No callback means such a check is unresolved, never silently true.
        """
        context = context or {}
        have = {self.sd.resolve(s) for s in have_species}
        result, unresolved = {}, {}
        flags, items = set(flags), set(items)
        def add(target, row):
            if target not in have and self.sd.exists(target):
                have.add(target)
                result[target] = row
                return True
            return False
        def check(sp, cond, arg):
            if cond in ('IF_ATK_LT_DEF','IF_ATK_EQ_DEF','IF_ATK_GT_DEF'):
                return reachable('OldaleTown_PokemonCenter_1F')
            if cond == 'IF_HOLD_ITEM': return arg in items
            if cond == 'IF_SPECIES_IN_PARTY': return self.sd.resolve(arg) in have
            if cond == 'IF_IN_MAP': return reachable(self.builder.geo.id_to_dir.get(arg, arg))
            if cond == 'IF_IN_MAPSEC': return any(m.get('region_map_section') == arg and reachable(d) for d,m in self.builder.geo.maps.items())
            if cond == 'IF_TYPE_IN_PARTY': return any(arg in self.breeding.get(s,{}).get('types',[]) for s in have)
            # Tuning assumes walking can reach evolution friendship before Bonding opens.
            if cond == 'IF_MIN_FRIENDSHIP': return reachable('OldaleTown_PokemonCenter_1F') or context.get('friendship_max',0) >= int(arg)
            if cond in ('IF_TIME','IF_NOT_TIME') or cond.startswith('IF_PID_'): return True
            if cond == 'IF_GENDER':
                gender = self.breeding.get(sp,{}).get('gender','')
                return gender == arg or gender not in ('MON_GENDERLESS','MON_MALE','MON_FEMALE')
            if cond in ('IF_NATURE','IF_AMPED_NATURE','IF_LOW_KEY_NATURE'):
                return True  # Feasible random starting nature; caller retains the condition
            if cond == 'IF_KNOWS_MOVE': return (context.get('knows_move',self.can_learn)(sp,arg)
                and context.get('evolution_move_ready',lambda s,m: True)(sp,arg))
            if cond == 'IF_KNOWS_MOVE_TYPE': return context.get('knows_move_type',self.can_learn_type)(sp,arg)
            if cond == 'IF_WEATHER':
                def native_weather(token):
                    wanted={'WEATHER_RAIN':{'WEATHER_RAIN','WEATHER_RAIN_THUNDERSTORM','WEATHER_DOWNPOUR'},'WEATHER_FOG':{'WEATHER_FOG_HORIZONTAL','WEATHER_FOG_DIAGONAL'}}.get(token,{token})
                    for d,m in self.builder.geo.maps.items():
                        if not reachable(d): continue
                        if m.get('weather') in wanted: return True
                        for event in m.get('coord_events',[]) or []:
                            if event.get('weather','').replace('COORD_EVENT_','') in wanted and reachable((d,event['x'],event['y'])): return True
                    return False
                return context.get('weather',native_weather)(arg)
            if cond == 'IF_TRADE_PARTNER_SPECIES': return False
            if cond == 'IF_REGION': return arg == 'REGION_HOENN'
            if cond == 'IF_NOT_REGION': return arg != 'REGION_HOENN'
            if cond == 'IF_BAG_ITEM_COUNT':
                parts = [s.strip() for s in arg.split(',')]
                return parts[0] in items and (len(parts)<2 or int(parts[1])<=1 or context.get('item_count',lambda i: 0)(parts[0]) >= int(parts[1]))
            return context.get('condition',lambda s,c,a: None)(sp,cond,arg)
        changed = True
        while changed:
            changed = False
            for row in self.evolutions:
                sp,target,method,param = (row[k] for k in ('species','target','method','parameter'))
                if sp not in have or target in have: continue
                if method in ('EVO_TRADE','EVO_NONE'): continue
                if method == 'EVO_LEVEL_BATTLE_ONLY':
                    if not param.isdigit() or int(param)>cap: continue
                    eligible=any(src.kind=='wild' and 'land_mons' in src.detail and self.sd.restricted_class(src.key) is None and reachable(src.where) for src in self.sources)
                    if not context.get('wild_battle_possible',eligible): continue
                if method == 'EVO_LEVEL' and (not param.isdigit() or int(param)>cap): continue
                if method == 'EVO_ITEM' and param not in items: continue
                if method == 'EVO_SPLIT_FROM_EVO' and self.sd.resolve(param) not in have: continue
                if method not in ('EVO_LEVEL','EVO_ITEM','EVO_SPLIT_FROM_EVO','EVO_LEVEL_BATTLE_ONLY'):
                    possible = context.get('condition',lambda s,c,a: None)(sp,method,param)
                    if possible is not True:
                        unresolved[target] = dict(row, reason='Native evolution trigger requires confirmation')
                        continue
                values = [check(sp,c['condition'],c['argument']) for c in row['conditions']]
                if any(v is False for v in values): continue
                if any(v is None for v in values):
                    unresolved[target] = dict(row, reason='Additional native evolution conditions unresolved')
                    continue
                provenance=dict(row, kind='evolution', detail=f'{method} {param} from {self.sd.name(sp)}')
                if method=='EVO_LEVEL_BATTLE_ONLY':
                    provenance['conditions']=provenance['conditions']+[dict(condition='ELIGIBLE_BATTLE_WIN_OR_CAPTURE',argument='Survive a normal wild win/capture; excludes Safari, tutorial, link and facility battles')]
                    provenance['citations']=provenance['citations']+[_citation('src/battle_main.c','static void TryEvolvePokemon(void)')]
                changed |= add(target,provenance)
            if reachable('Route117_PokemonDayCare'):
                for sp in list(have):
                    mate = self._mate(sp,have)
                    if not mate: continue
                    if mate == sp and not context.get('repeatable',lambda s: False)(sp):
                        # Ownership alone is not proof of a second compatible mon.
                        continue
                    egg = 'SPECIES_PHIONE' if sp == 'SPECIES_MANAPHY' else self.sd.egg_species(sp)
                    if any(tag in sp for tag in ('_ALOLA','_GALAR','_HISUI','_PALDEA')) and 'ITEM_EVERSTONE' not in items:
                        egg = self.sd.base(egg)
                    egg = {'SPECIES_SINISTEA_ANTIQUE':'SPECIES_SINISTEA_PHONY','SPECIES_POLTCHAGEIST_ARTISAN':'SPECIES_POLTCHAGEIST_COUNTERFEIT'}.get(egg,egg)
                    changed |= add(egg, dict(kind='breeding',species=sp, mate=mate, target=egg,
                        detail=f'Day-Care egg from {self.sd.name(sp)} with compatible {self.sd.name(mate)}',
                        conditions=['Compatible parent sexes and egg groups; regional parent holds Everstone to preserve foreign form'],
                        citations=[_citation('src/daycare.c','static enum Species DetermineEggSpeciesAndParentSlots'),_citation('src/daycare.c','u8 GetDaycareCompatibilityScore')]))
                    sibling = {'SPECIES_NIDORAN_F':'SPECIES_NIDORAN_M','SPECIES_NIDORAN_M':'SPECIES_NIDORAN_F','SPECIES_ILLUMISE':'SPECIES_VOLBEAT','SPECIES_VOLBEAT':'SPECIES_ILLUMISE'}.get(egg)
                    if sibling: changed |= add(sibling, dict(kind='breeding',species=sp,mate=mate,target=sibling,detail='Native mixed-species egg outcome',citations=[_citation('src/daycare.c','eggSpecies == SPECIES_NIDORAN_F')]))
            for row in self.forms:
                sp,target,method,args = (row[k] for k in ('species','target','method','parameters'))
                if method in ('FORM_CHANGE_BATTLE_GIGANTAMAX','FORM_CHANGE_BATTLE_ULTRA_BURST'): continue
                if sp not in have or target in have: continue
                if method == 'COZMO_METEORITE': ok = reachable(args[0])
                elif method == 'FUSION': ok = args[0] in items and args[1] in have
                elif method in rp.ITEM_FORM_METHODS: ok = not args or args[0] == 'ITEM_NONE' or args[0] in items
                elif method in ('FORM_CHANGE_DEPOSIT','FORM_CHANGE_WITHDRAW','FORM_CHANGE_TIME_OF_DAY','FORM_CHANGE_DAYS_PASSED'): ok = True
                elif method == 'FORM_CHANGE_MOVE': ok = context.get('knows_move',self.can_learn)(sp,args[0]) if args else None
                elif method == 'FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM': ok = 'ITEM_MEGA_RING' in items and args[0] in items
                elif method == 'FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE': ok = 'ITEM_MEGA_RING' in items and 'FLAG_IS_CHAMPION' in flags and self.can_learn(sp,args[0])
                elif method == 'FORM_CHANGE_BATTLE_PRIMAL_REVERSION': ok = args[0] in items
                else: ok = context.get('form',lambda s,m,a: None)(sp,method,args)
                if ok is None:
                    unresolved[target] = dict(row,reason='Native form trigger/parameters require confirmation')
                elif ok:
                    changed |= add(target,dict(row,kind='form',detail=f'{method} from {self.sd.name(sp)}'))
        return dict(species=result, unresolved=[v for k,v in sorted(unresolved.items()) if k not in have])

    def export(self):
        references={citation.split(';')[0].strip() for row in self.rows for citation in row['citations']}
        literal_sites=[]; missing=[]
        for label,info in self.builder.scripts.labels.items():
            if info['file'].name=='debug.inc':continue
            for line,text in info['body']:
                if not re.match(r'(?:givemon|giveegg|setwildbattle)\s+SPECIES_\w+',text):continue
                citation=f"{info['file'].relative_to(ROOT)}:{line}"
                literal_sites.append(dict(source=citation,command=text))
                if citation not in references:missing.append(dict(source=citation,command=text,label=label))
        return dict(sources=self.rows,evolutions=self.evolutions,forms=self.forms,
            breeding=self.breeding,iconic_moves=self.iconic_moves,eligibility_diagnostics=self.eligibility_diagnostics,starter_regions=self.builder.starter_regions,
            source_coverage=dict(literal_species_sites=literal_sites,unrepresented_literal_sites=missing,
                trade_sources=sum(row['kind']=='trade' for row in self.rows),
                unresolved_live_sources=[row['id'] for row in self.rows if row.get('eligibility_status')=='unresolved' and not row.get('campaign_scope')]),
            rules=rp.player_rules(),
            facilities='Battle Tent/Frontier rentals and partner Pokemon are battle loans, not owned species sources',
            scope='Source alternatives through Buffel; no cap-window availability assignment')


def export(builder=None):
    return PokemonSources(builder).export()


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--out',type=Path)
    args=parser.parse_args()
    model=PokemonSources()
    model.enrich_eligibility()
    data=model.export()
    text=json.dumps(data,ensure_ascii=False,indent=2)+'\n'
    if args.out:
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(text)
    else: print(text,end='')

if __name__ == '__main__': main()
