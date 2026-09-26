"""Source-backed economy atlas: location roots, transfers, shops and native providers.

This maps source ownership, not a simulated save. Conditional graph reachability
is deliberately reported as potential; literals and native formulas are kept
separate from unresolved dynamic values. No authored reward data lives here.
"""
from collections import Counter, defaultdict
from pathlib import Path
import hashlib
import json
import re
import subprocess
import tempfile

from audit.map_dynamic_inventory import (arguments, balanced, calls, derive_script_contexts,
    functions, strip_comments, _code_only)
from item_catalog import battle_item_categories
from verify_trainer_ability_legality import preprocess_species_info, species_aliases, resolve_species

ROOT=Path(__file__).resolve().parents[1]
TRANSFER={'giveitem','giveuniqueitem','finditem','additem','addpcitem','givedecoration','adddecoration',
          'givemon','givemonmodern','giveegg','ingame_trade','setwildbattle','giverandomberry','addmoney','givemoney','addcoins','givecoins','giveitem_msg'}
COST={'checkpcitem','checkitemtype','removemoney','removecoins','removeitem','checkmoney','checkcoins','checkitem','checkitemspace','setitemandprice'}
NATIVE_SINKS={'AddBagItem','AddPCItem','GiveMonToPlayer','GiveScriptedMonToPlayer','AddMoney','AddCoins',
    'GiveBattlePoints','GiveFrontierBattlePoints','AwardCircuitBattlePoints','AddBattlePoints','GiveBerryPowder','AddHarvestedBerries',
    'CreatePokemartMenu','CreateFreePokemartMenu','CreateDecorationShop1Menu','CreateDecorationShop2Menu',
    'DecorationAdd','UnlockLegendarySign','GiveEggFromDaycare','TryGiveEmeraldChampionsPreparedPokemon'}
BOOKKEEPING={'TakeFrontierBattlePoints','TakeBattlePoints','SetMoney','SetCoins','RemoveBagItem','RemovePCItem','RemoveMoney','RemoveCoins','TakeBerryPowder','SetMonData','SetBoxMonData'}
SHOPS={'pokemart','pokemartbuy','pokemartdecoration','pokemartdecoration2'}
NULL={None,0,'0','0x0','NULL','0x00000000'}
TOKENS=re.compile(r'\b(?:ITEM|DECOR|SPECIES)_[A-Z0-9_]+\b')

def compact(s):
    parts=re.split(r'("(?:\\.|[^"\\])*")',s)
    return ''.join(part if i%2 else re.sub(r'\s+',' ',part) for i,part in enumerate(parts)).strip()
def ref(path,line=1):return f'{path}:{line}'
def display(token):return re.sub(r'^(ITEM|DECOR|SPECIES)_','',token).replace('_',' ').title()


def cpp(root,headers):
    probe='\n'.join(f'#include "{h}"' for h in headers)+'\n'
    result=subprocess.run(['cc','-E','-P','-x','c','-DTRUE=1','-DFALSE=0','-Iinclude','-Isrc','-I.','-'],
        cwd=root,input=probe,capture_output=True,text=True,check=True)
    return result.stdout


def item_prices(root):
    text=cpp(root,['config/general.h','constants/global.h','config/item.h','constants/items.h','constants/pokemon.h','item.h','data/items.h'])
    marks=list(re.finditer(r'\[(ITEM_\w+)\]\s*=\s*\{',text));items={};expressions={}
    raw=(root/'src/data/items.h').read_text()
    for i,m in enumerate(marks):
        body=text[m.end():marks[i+1].start() if i+1<len(marks) else len(text)]
        def field(key,default='0'):
            x=re.search(r'\.'+key+r'\s*=\s*([^,\n]+)',body);return compact(x[1]) if x else default
        price=field('price');pocket=field('pocket','unspecified')
        source_match=re.search(r'\['+m[1]+r'\]',raw)
        items[m[1]]=dict(price_expression=price,pocket=pocket,sort=field('sortType'),
            importance=field('importance'),not_consumed=field('notConsumed'),field_use=field('fieldUseFunc','none'),
            source=ref('src/data/items.h',raw.count('\n',0,source_match.start())+1 if source_match else 1))
        if not re.fullmatch(r'[\d\s()+*/%<>=!&|?:.\-]+',price):raise ValueError('unresolved configured price '+m[1]+': '+price)
        expressions.setdefault(price,len(expressions))
    # Evaluate the actual configured C expressions; no hand-maintained gen-price table.
    with tempfile.TemporaryDirectory() as d:
        out=Path(d)/'prices'
        program='#include <stdio.h>\nint main(void){\n'+''.join(f'printf("{i} %u\\n",(unsigned)({e}));\n' for e,i in expressions.items())+'return 0;}\n'
        subprocess.run(['cc','-x','c','-o',str(out),'-'],input=program,capture_output=True,text=True,check=True)
        values=dict(map(int,line.split()) for line in subprocess.check_output([str(out)],text=True).splitlines())
    for item in items.values():item['base_price']=values[expressions[item['price_expression']]]
    decor=(root/'src/data/decoration/header.h').read_text();marks=list(re.finditer(r'\[(DECOR_\w+)\]\s*=\s*\{',decor))
    for i,m in enumerate(marks):
        body=decor[m.end():marks[i+1].start() if i+1<len(marks) else len(decor)];price=re.search(r'\.price\s*=\s*(\d+)',body)
        if price:items[m[1]]=dict(base_price=int(price[1]),price_expression=price[1],pocket='DECORATION',sort='cosmetic',importance='0',not_consumed='1',field_use='decoration',source=ref('src/data/decoration/header.h',decor.count('\n',0,m.start())+1))
    return items


def array_records(root,paths):
    arrays={};sources=set()
    pattern=r'(?m)^\s*(?:(?:static|const|COMMON_DATA|EWRAM_DATA)\s+)*(?:struct\s+\w+|struct\s*\{[^}]*\}|enum\s+\w+|u8|u16|u32|int)\s+(\w+)\s*(?:\[[^\]]*\])+\s*=\s*\{'
    for path in paths:
        text=strip_comments(path.read_text());code=_code_only(text)
        for m in re.finditer(pattern,code):
            start=m.end()-1;end=balanced(text,start,'{','}',code=code);body=text[start+1:end]
            for inc in re.findall(r'#include\s+"([^"]+)"',body):
                options=[path.parent/inc,root/'src'/inc,root/inc];target=next((p for p in options if p.is_file()),None)
                if target:body=body.replace('#include "'+inc+'"',target.read_text());sources.add(target)
            ident=str(path.relative_to(root))+':'+m[1]
            arrays[ident]=dict(name=m[1],source=ref(str(path.relative_to(root)),text.count('\n',0,m.start())+1),body=compact(body),tokens=sorted(set(TOKENS.findall(_code_only(body)))))
    return arrays,sources


def is_reward_table(array):
    # These are supply/prize/access selectors, not opponent loadouts, species
    # definitions or the entire item/move database merely referenced by a helper.
    return any(word in array['name'].lower() for word in [
        'pickup','ingametrade','prize','fossil','soot','gamecorner','evolutionitems',
        'milestone','frontierexchangecorner','formgifts','legendarysign','relicitems',
        'newgamepcitems','berrystonetrades','starterstones','startermons','wildmonhelditems','battleitem','berryitems',
    ]) and 'description' not in array['name'].lower()


def native_index(root):
    paths=[p for p in sorted((root/'src').glob('*.c')) if p.name not in {'debug.c','emerald_champions_headless.c'}]
    nodes={};names=defaultdict(list);raw_sinks=[]
    for path in paths:
        text=strip_comments(path.read_text());source=str(path.relative_to(root));spans=functions(text)
        for name,(a,b) in spans.items():
            key=source+':'+name;body=text[a:b]
            node=dict(name=name,path=source,source=ref(source,text.count('\n',0,a)+1),body=body,callees=[],tokens=sorted(set(TOKENS.findall(body))),sinks=[])
            nodes[key]=node;names[name].append(key)
        for name,offset,args in calls(text,NATIVE_SINKS|BOOKKEEPING):
            owner=next((source+':'+n for n,(a,b) in spans.items() if a<=offset<b),None)
            if owner is None:continue  # definitions/prototypes are not calls
            row=dict(source=ref(source,text.count('\n',0,offset)+1),function=owner,operation=name,arguments=args,kind='transfer_or_shop' if name in NATIVE_SINKS else 'cost_or_state_write')
            if name in BOOKKEEPING and name not in {'SetMonData','SetBoxMonData'}:raw_sinks.append(row)
            elif name in NATIVE_SINKS:raw_sinks.append(row)
            elif name in {'SetMonData','SetBoxMonData'} and len(args)>1 and args[1] in {'MON_DATA_HELD_ITEM','MON_DATA_SPECIES'}:raw_sinks.append(row)
            if owner and row in raw_sinks:nodes[owner]['sinks'].append(row)
    for key,node in nodes.items():
        # Direct calls and named callbacks both matter for native shop/task paths.
        identifiers=set(re.findall(r'\b[A-Za-z_]\w*\b',_code_only(node['body'])))
        for name in identifiers & names.keys():
            targets=names[name];local=[t for t in targets if nodes[t]['path']==node['path']]
            node['callees'].extend(local or targets)
        node['callees']=sorted(set(node['callees'])-{key})
    economic={key for key,node in nodes.items() if node['name'] in NATIVE_SINKS|(BOOKKEEPING-{'SetMonData','SetBoxMonData','SetMoney','SetCoins'}) or any(s['kind']=='transfer_or_shop' for s in node['sinks']) or (any(s['operation'] in {'SetMonData','SetBoxMonData'} and 'MON_DATA_HELD_ITEM' in s['arguments'] for s in node['sinks']) and re.search(r'Apply.*(?:Set|Preset)|pickup|SetWild',node['name'],re.I))}
    # Only use this as a source-call association, never as runtime reachability.
    changed=True
    while changed:
        old=len(economic)
        economic.update(key for key,node in nodes.items() if any(c in economic for c in node['callees']))
        changed=len(economic)!=old
    return nodes,names,economic,raw_sinks,paths


def game_corner_offers(root,blocks,arrays):
    """Follow literal species/coin assignments through the active prize script.

    Prices can precede the species menu or follow a shared selection label.
    Only paths that reach givemon and a matching coin debit count as offers.
    """
    source='data/maps/MauvilleCity_GameCorner/scripts.inc'
    text=(root/source).read_text()
    constants={m[1]:int(m[2],0) for m in re.finditer(r'(?m)^\s*\.set\s+(\w+),\s*(0x[0-9A-Fa-f]+|\d+)\s*$',text)}
    labels=re.findall(r'(?m)^(\w+)::?[^\n]*$',text)
    following=dict(zip(labels,labels[1:]))
    bodies={label:(line,body) for label,defs in blocks.items() for path,line,body in defs if path==source}
    if not re.search(r'(?m)^\s*removecoins\s+VAR_0x8006\s*$',text):
        raise ValueError('Game Corner coin debit binding changed')
    pending=[(label,None,None,None) for label in bodies];seen=set();offers={}
    while pending:
        label,mon,cost,origin=pending.pop()
        state=(label,mon,cost,origin)
        if state in seen or label not in bodies:continue
        seen.add(state);line,body=bodies[label];stopped=False
        for raw in body.splitlines():
            fields=raw.split('@',1)[0].strip().split(None,1)
            if not fields:continue
            command=fields[0];args=arguments(fields[1]) if len(fields)>1 else []
            if command=='setvar' and len(args)==2:
                if args[0]=='VAR_TEMP_1' and args[1].startswith('SPECIES_'):
                    mon=args[1];origin=ref(source,line)
                if args[0]=='VAR_0x8006':
                    cost=constants.get(args[1],int(args[1]) if args[1].isdigit() else None)
                    if cost is None:raise ValueError('Unresolved Pokemon price: '+args[1])
            if command=='givemon' and args and args[0]=='VAR_TEMP_1' and mon:
                if cost is None:continue
                previous=offers.get(mon)
                if previous and previous['coins']!=cost:raise ValueError('Ambiguous Pokemon price: '+mon)
                offers[mon]=dict(species=mon,coins=cost,source=origin)
            if command=='goto':
                pending.append((args[-1],mon,cost,origin));stopped=True;break
            if command.startswith('goto_if') or command=='case':
                pending.append((args[-1],mon,cost,origin))
            if command in {'end','return'}:stopped=True;break
        if not stopped and label in following:pending.append((following[label],mon,cost,origin))
    declared=set(re.findall(r'setvar\s+VAR_TEMP_1,\s*(SPECIES_\w+)',text))
    if set(offers)!=declared:raise ValueError('Unresolved Game Corner offers: '+', '.join(sorted(declared-set(offers))))
    return sorted(offers.values(),key=lambda r:(r['coins'],r['species'])),[]


def build_catalog(root=ROOT):
    root=Path(root);maps={};map_paths=[];records={};by_map=defaultdict(list);reverse=defaultdict(set);diagnostics={};flow={}
    for p in sorted((root/'data/maps').glob('*/map.json')):
        m=json.loads(p.read_text())
        if m.get('region','REGION_HOENN')=='REGION_HOENN':maps[p.parent.name]=m;map_paths.append(p)
    contexts=derive_script_contexts(root,maps,diagnostics=diagnostics,flow=flow)
    blocks=flow['blocks'];graph=flow['graph'];predecessors=defaultdict(set)
    for parent,children in graph.items():
        for child in children:predecessors[child].add(parent)
    native,names,economic,native_sinks,native_paths=native_index(root)
    data_paths=sorted((root/'src/data').rglob('*.h'))
    # Ignore generated headers so fresh-checkout and built-tree books agree.
    candidates=native_paths+data_paths
    check=subprocess.run(['git','check-ignore','--stdin','-z'],cwd=root,input='\0'.join(str(p.relative_to(root)) for p in candidates)+'\0',capture_output=True,text=True)
    ignored=set(check.stdout.split('\0')) if check.returncode==0 else set()
    arrays,array_sources=array_records(root,[p for p in candidates if str(p.relative_to(root)) not in ignored])
    array_names=defaultdict(list)
    for key,v in arrays.items():array_names[v['name']].append(key)
    prices=item_prices(root)
    free=battle_item_categories(root)
    paid_items=re.findall(r'ITEM_\w+',(root/'src/data/emerald_champions_paid_evolution_items.h').read_text())
    form_items=re.findall(r'ITEM_\w+',(root/'src/data/emerald_champions_form_items.h').read_text())
    corner_offers,bp_pokemon_offers=game_corner_offers(root,blocks,arrays)
    trade_text=(root/'src/data/trade.h').read_text()
    trade_defs={}
    for m in re.finditer(r'\[(INGAME_TRADE_\w+)\]\s*=\s*\{(.*?)\n\s*\}',trade_text,re.S):
        values=dict(re.findall(r'\.(species|requestedSpecies|heldItem)\s*=\s*((?:SPECIES|ITEM)_\w+)',m[2]))
        if 'species' in values:trade_defs[m[1]]=dict(values,source=ref('src/data/trade.h',trade_text.count('\n',0,m.start())+1))
    berry_names=re.findall(r'F\((\w+)\)',(root/'include/constants/berries.h').read_text())
    unclassified_commands=[]
    variables=defaultdict(list);operations=defaultdict(list);native_roots={};shop_stock={};all_transfers=0
    for label,defs in blocks.items():
        for source,line_start,body in defs:
            conditions=[compact(l) for l in body.splitlines() if re.match(r'\s*(?:goto_if|call_if|case|check|switch|setflag|clearflag|remove|addmoney|addcoins)',l)]
            local_inputs={}
            for delta,raw in enumerate(body.splitlines()):
                line=raw.split('@',1)[0].strip()
                if not line or line.startswith(('#','.')):continue
                fields=line.split(None,1);command=fields[0];args=arguments(fields[1]) if len(fields)>1 else [];where=ref(source,line_start+delta)
                if command in {'setvar','setorcopyvar','copyvar'} and len(args)>=2:
                    variables[args[0]].append(dict(value=args[1],label=label,source=where,maps=contexts.get(label,[])))
                    local_inputs[args[0]]=args[1]
                if re.fullmatch(r'(give|add)[a-z_]*(item|money|coins|berry|egg|pokemon|mon|decoration|points)[a-z_]*',command) and command not in TRANSFER:
                    unclassified_commands.append(dict(source=where,command=command,arguments=args))
                relevant=command in TRANSFER or command in COST or command in SHOPS or command in {'special','specialvar','callnative','gotonative'}
                if not relevant:continue
                if command in {'special','specialvar','callnative','gotonative'}:
                    target=(args[1] if command=='specialvar' and len(args)>1 else args[0] if args else '');keys=[k for k in names.get(target,[]) if k in economic]
                    # Include native menu/read helpers only through their effectful
                    # implementation; read-only buffer functions aren't gift claims.
                    if not keys:continue
                    for key in keys:native_roots[key]=native[key]
                    rid='native-call '+where
                    record=dict(kind='native cost/state service' if target in BOOKKEEPING else 'native provider',source=where,label=label,operation=command,arguments=args,outputs=[],conditions=conditions,possible_maps=contexts.get(label,[]),native=keys)
                    record['local_inputs']=local_inputs.copy()
                    if target=='OpenEmeraldChampionsEvolutionSpecialist':
                        mode=local_inputs.get('VAR_0x8004')
                        if mode not in {'0','1'}:raise ValueError('Unresolved evolution counter mode at '+where)
                        record['kind']='native shop'
                        record['outputs']=[item for item in paid_items if (prices[item]['sort']=='ITEM_TYPE_EVOLUTION_STONE')==(mode=='0')]
                        record['cost_rule']='base cash price; native shop discounts and ownership checks apply'
                    elif target=='OpenEmeraldChampionsEvolutionItemArchive':
                        record['kind']='native free shop';record['outputs']=form_items;record['cost_rule']='free; form-use prerequisites remain separate'
                    elif target=='OpenEmeraldChampionsBattleItemMart':
                        mode=local_inputs.get('VAR_0x8004')
                        record['kind']='native free shop';record['outputs']=list(free.get(mode,dict.fromkeys(x for row in free.values() for x in row)))
                        record['cost_rule']='free category selector; union of selectable categories when selection is dynamic'

                else:
                    rid='script '+where
                    record=dict(kind='shop' if command in SHOPS else 'scripted encounter' if command=='setwildbattle' else 'transfer' if command in TRANSFER else 'cost/offer/access check',source=where,label=label,operation=command,arguments=args,outputs=[],conditions=conditions,possible_maps=contexts.get(label,[]))
                    if command in TRANSFER:
                        all_transfers+=1
                        record['outputs']=TOKENS.findall(args[0]) if args else []
                    if command=='giverandomberry':
                        low=berry_names.index(args[0].removeprefix('BERRY_ID_'));high=berry_names.index(args[1].removeprefix('BERRY_ID_'))
                        record['outputs']=['ITEM_'+name+'_BERRY' for name in berry_names[low:high+1]]
                        record['quantity']='one randomly chosen berry; not the entire listed range'
                        record['selection_source']='asm/macros/event.inc:giverandomberry -> src/script_pokemon_util.c:Script_GiveRandomBerry'
                    if command=='ingame_trade':
                        trade=trade_defs.get(args[0])
                        if not trade:raise ValueError('missing native trade definition '+args[0])
                        record['outputs']=[trade['species']]+([trade['heldItem']] if trade.get('heldItem','ITEM_NONE')!='ITEM_NONE' else [])
                        record['trade']=trade;record['required_pokemon']=trade.get('requestedSpecies')
                    if command=='setitemandprice':record['outputs']=TOKENS.findall(args[0]);record['quoted_cost']=args[1:];record['currency']='BP' if 'ExchangeServiceCorner' in source else 'native debit rule'
                    if command in SHOPS:
                        table=args[0] if args else '';stock=[]
                        for table_source,first,table_body in blocks.get(table,[]):
                            for match in re.finditer(r'(?m)^\s*\.2byte\s+((?:ITEM|DECOR)_\w+)',table_body):
                                item=match[1];stock.append(item)
                        stock=[x for x in dict.fromkeys(stock) if x!='ITEM_NONE'];record['outputs']=stock;record['table']=table
                        if not stock:record['unresolved']='stock is dynamic/missing; inspect table '+table
                record['entry_branches']=[dict(label=parent,source=ref(source_line,first),controls=[compact(x) for x in parent_body.splitlines() if re.match(r'\s*(?:goto_if|call_if|case|switch|check|setvar)',x)]) for parent in sorted(predecessors[label]) for source_line,first,parent_body in blocks[parent]]
                records[rid]=record;operations[label].append(rid)
    def reachable(start):
        todo=[start];seen=set()
        while todo:
            label=todo.pop()
            if label in seen:continue
            seen.add(label);todo.extend(graph.get(label,set())-seen)
        return seen
    seeds={m[1]:(m[2],m[3]) for m in re.finditer(r'(?m)^\s*setberrytree\s+(\w+),\s*(\w+),\s*(\w+)',(root/'data/scripts/new_game.inc').read_text())}
    covered_operations=set();actors=0
    for map_name,m in maps.items():
        source='data/maps/'+map_name+'/map.json';raw=(root/source).read_text()
        for kind in ['object_events','bg_events','coord_events']:
            for i,row in enumerate(m.get(kind) or [],1):
                script=row.get('script');item=row.get('item') if kind=='bg_events' else row.get('trainer_sight_or_berry_tree_id')
                location=dict(map=map_name,event_kind=kind,event_id=row.get('local_id',i),x=row.get('x'),y=row.get('y'),script=script,flag=row.get('flag'),source=source)
                rid=f'{map_name}:{kind}:{i}'
                if kind=='bg_events' and row.get('type')=='hidden_item':
                    record=dict(location,kind='hidden pickup',outputs=[item],quantity=row.get('quantity',1),conditions={'flag':row.get('flag'),'elevation':row.get('elevation')})
                    records[rid]=record;by_map[map_name].append(rid)
                elif kind=='object_events' and script=='Common_EventScript_FindItem':
                    quantity=row.get('movement_range_x',0) or 1
                    record=dict(location,kind='visible pickup',outputs=[item],quantity=quantity,conditions={'hide/receipt':row.get('flag'),'graphics':row.get('graphics_id'),'elevation':row.get('elevation'),'global_spawn_filter':'src/event_object_movement.c:TrySpawnObjectEventTemplate','quantity_and_unique_reward':'GetItemBallIdAndAmountFromTemplate + Common_EventScript_FindItem'})
                    records[rid]=record;by_map[map_name].append(rid)
                elif kind=='object_events' and script=='BerryTreeScript':
                    berry_id=str(item);initial=seeds.get(berry_id);out=['ITEM_'+initial[0].removeprefix('BERRY_ID_')+'_BERRY'] if initial else []
                    records[rid]=dict(location,kind='berry harvest',outputs=out,tree=berry_id,initial=initial or ['EMPTY_SOIL','plant a seed first'],quantity='native yield; planting may change the type',conditions='Repeatable growth/harvest; only successful harvest mints per-type credits; see native berry provider')
                    by_map[map_name].append(rid)
                if script in NULL:continue
                rs=reachable(script);refs=sorted({op for label in rs for op in operations[label]})
                if refs:
                    key=rid+':entry';records[key]=dict(location,kind='interaction entry',outputs=[],references=refs,
                        conditions={k:row[k] for k in ['var','var_value','flag','trainer_type','trainer_sight_or_berry_tree_id'] if k in row},
                        access='Potential source paths. Story predicates, geometry, stock selection and repeat receipts are not simulated.')
                    by_map[map_name].append(key);covered_operations.update(refs);actors+=1
        entry=m.get('shared_scripts_map',map_name)+'_MapScripts'
        rs=reachable(entry);refs=sorted({op for label in rs for op in operations[label]})
        if refs:
            key=map_name+':map-scene';records[key]=dict(kind='map scene',map=map_name,source=source,script=entry,references=refs,outputs=[],access='Conditional transition/load/frame script paths; not first-arrival proof');by_map[map_name].append(key);covered_operations.update(refs)
    # Show variable inputs only from source paths associated with the same map.
    # Alternatives remain alternatives; never cross-product item and price values
    # into fabricated offers.
    for key,record in records.items():
        vars_used=[a for a in record.get('arguments',[]) if re.fullmatch(r'(?:VAR_|gSpecialVar_)\w+',a)]
        candidates={}
        for var in vars_used:
            rows=[r for r in variables[var] if set(r['maps']) & set(record.get('possible_maps',[]))]
            if rows:candidates[var]=rows
        if candidates:
            record['variable_inputs']=candidates
            if record.get('operation') in TRANSFER:
                for rows in candidates.values():
                    for candidate in rows:
                        for token in TOKENS.findall(candidate['value']):reverse[token].add('possible-script '+key)
        for item in record.get('outputs',[]):
            if item not in {'ITEM_NONE','SPECIES_NONE'}:reverse[item].add(key)
    # Native providers retain their actual function bodies and referenced tables.
    providers={};native_covered=set()
    for key,node in native_roots.items():
        todo=[key];seen=set();related=[];table_keys=set()
        while todo:
            current=todo.pop()
            if current in seen:continue
            seen.add(current);child=native[current]
            if child['sinks']:native_covered.update(r['source'] for r in child['sinks'])
            for name in set(re.findall(r'\b\w+\b',child['body'])) & array_names.keys():
                for a in array_names[name]:
                    if arrays[a]['tokens'] and is_reward_table(arrays[a]):table_keys.add(a)
            # Keep effectful local helpers and data selectors, not every UI task.
            if current==key or child['tokens'] or child['sinks']:related.append(current)
            todo.extend(c for c in child['callees'] if c not in seen)
        providers[key]=dict(source=node['source'],functions=sorted(set(related)),tables=sorted(table_keys),
            scope='Native call/data dependencies are potential paths, not proof every table entry is offered at every caller.')
    used_functions={k for p in providers.values() for k in p['functions']};used_tables={k for p in providers.values() for k in p['tables']}
    # Economic data tables and global mechanics also exist outside NPC calls.
    extra_tables={k for k,a in arrays.items() if is_reward_table(a) and (a['tokens'] or 'milestone' in a['name'].lower())}
    used_tables|=extra_tables
    pokemon_source=(root/'src/pokemon.c').read_text()
    low=re.search(r'chanceNoItem\s*=\s*itemHeldBoost\s*\?\s*(\d+)\s*:\s*(\d+)',pokemon_source)
    high=re.search(r'chanceNotRare\s*=\s*itemHeldBoost\s*\?\s*(\d+)\s*:\s*(\d+)',pokemon_source)
    if not low or not high:raise ValueError('Wild held-item threshold reader needs updating for current native source')
    chances={'common':int(high[2])-int(low[2]),'rare':100-int(high[2]),'boost_common':int(high[1])-int(low[1]),'boost_rare':100-int(high[1])}
    altering_table=next(a['body'] for a in arrays.values() if a['name']=='sAlteringCaveWildMonHeldItems')
    altering=dict(re.findall(r'\{\s*(SPECIES_\w+)\s*,\s*(ITEM_\w+)',altering_table))
    held={};species_text=preprocess_species_info();marks=list(re.finditer(r'\[(SPECIES_\w+)\]\s*=\s*\{',species_text))
    for i,m in enumerate(marks):
        body=species_text[m.end():marks[i+1].start() if i+1<len(marks) else len(species_text)]
        common=re.search(r'\.itemCommon\s*=\s*(ITEM_\w+)',body);rare=re.search(r'\.itemRare\s*=\s*(ITEM_\w+)',body)
        if common or rare:held[m[1]]=(common[1] if common else 'ITEM_NONE',rare[1] if rare else 'ITEM_NONE')
    aliases=species_aliases()
    for alias in aliases:
        target=resolve_species(alias,aliases)
        if target in held:held[alias]=held[target]
    wild=json.loads((root/'src/data/wild_encounters.json').read_text());wild_items=[]
    active_ids={m['id']:name for name,m in maps.items()}
    for group in wild['wild_encounter_groups']:
        if not group.get('for_maps'):continue
        for entry in group['encounters']:
            map_name=active_ids.get(entry.get('map'))
            if not map_name:continue
            for field in group['fields']:
                method=field['type']
                if method not in entry:continue
                for mon_index,mon in enumerate(entry[method]['mons']):
                    submethod=next((rod for rod,indexes in field.get('groups',{}).items() if mon_index in indexes),method)
                    sp=mon['species']
                    reverse[sp].add('wild-table '+map_name+'/'+submethod+' (see ordinary wild tables)')
                    pair=held.get(sp,('ITEM_NONE','ITEM_NONE'))
                    if maps[map_name]['layout']=='LAYOUT_ALTERING_CAVE' and sp in altering:pair=('ITEM_NONE',altering[sp])
                    if pair==('ITEM_NONE','ITEM_NONE'):continue
                    row=(map_name,submethod,sp,*pair)
                    if row not in wild_items:wild_items.append(row)
    for map_name,method,sp,common,rare in wild_items:
        key=f'wild-held {map_name}/{method}/{sp}'
        records[key]=dict(kind='wild held item',map=map_name,source='src/data/wild_encounters.json + src/data/pokemon/species_info/* + src/pokemon.c:SetWildMonHeldItem',species=sp,method=method,common=common,rare=rare,outputs=[x for x in dict.fromkeys([common,rare]) if x!='ITEM_NONE'],quantity=1,
            conditions=f"Native thresholds: common{chances['common']}%/rare{chances['rare']}%; item-rarity boost common{chances['boost_common']}%/rare{chances['boost_rare']}%. Identical non-NONE slots, Altering Cave and preassigned items have explicit native branches; see SetWildMonHeldItem. Capturing/eligible theft is required.")
        by_map[map_name].append(key)
        for item in records[key]['outputs']:reverse[item].add(key)
    for key in used_tables:
        for item in arrays[key]['tokens']:
            if item not in {'ITEM_NONE','SPECIES_NONE'}:reverse[item].add('native-table '+key)
    free=battle_item_categories(root)
    for category,items in free.items():
        for item in items:reverse[item].add('free-category '+category)
    global_functions={r['function'] for r in native_sinks if r['function'] and r['source'] not in native_covered}
    used_functions.update(global_functions)
    core_names={'SetWildMonHeldItem','CanFirstMonBoostHeldItemRarity','Cmd_pickup','Cmd_getmoneyreward','Cmd_givepaydaymoney','NewGameInitPCItems','NewGameInitData','GetItemPrice','GetItemSellPrice','IsItemProtectedFromLoss','IsEmeraldChampionsFreeCatalogueItem','IsEmeraldChampionsFreePresetItem','GetShopItemPrice','CalcBerryYield','CalcBerryYieldInternal','GetBerryTreeAge','PickBerryTree','BerryTreeGrow','GiveEggFromDaycare','_GiveEggFromDaycare','TrySpawnObjectEventTemplate','GetItemBallIdAndAmountFromTemplate'}
    core_functions={key for key,node in native.items() if node['name'] in core_names}
    used_functions.update(core_functions)
    if unclassified_commands:
        raise ValueError('Unclassified economic commands must be mapped: '+json.dumps(unclassified_commands))
    source_paths=set(map_paths)|{root/'src/data/items.h',root/'src/data/decoration/header.h',root/'data/scripts/new_game.inc',root/'src/data/wild_encounters.json',Path(__file__),root/'scripts/audit/map_dynamic_inventory.py',root/'scripts/item_catalog.py'}|set(native_paths)|set(array_sources)
    source_paths.update(root/source for defs in blocks.values() for source,_,_ in defs)
    source_paths.update(root/arrays[k]['source'].rsplit(':',1)[0] for k in used_tables)
    source_paths.update((root/'src/data/pokemon/species_info').glob('*.h'));source_paths.update((root/'include/config').glob('*.h'))
    source_paths.update(root/p for p in ['include/constants/berries.h','include/constants/item.h','include/constants/items.h','include/constants/species.h','asm/macros/event.inc','src/data/emerald_champions_paid_evolution_items.h','src/data/emerald_champions_form_items.h'])
    economic_constants=[]
    constant_paths=set(native_paths)|set((root/'include/config').glob('*.h'))|set((root/'include/constants').glob('*.h'))
    for path in sorted(constant_paths):
        text=strip_comments(path.read_text())
        for m in re.finditer(r'(?m)^#define[ \t]+(\w+)[ \t]+([^\n]+)',text):
            if re.search(r'SOOT|HARVEST|BERRY_(?:GROWTH|YIELD|MOISTURE|DRAIN)|CIRCUIT_BP|BATTLE_FRONTIER_POINTS|SELL_FACTOR|BERRY_POWDER|MAX_MONEY|MAX_COINS',m[1]):
                economic_constants.append(dict(name=m[1],expression=m[2],source=ref(str(path.relative_to(root)),text.count('\n',0,m.start())+1)))
    source_paths.update(constant_paths)
    berry_source=(root/'src/berry.c').read_text();berry_profiles={}
    marks=list(re.finditer(r'\[(BERRY_ID_\w+)\]\s*=\s*\{',berry_source))
    for i,m in enumerate(marks):
        body=berry_source[m.end():marks[i+1].start() if i+1<len(marks) else len(berry_source)]
        values={name:compact(value) for name,value in re.findall(r'\.(minYield|maxYield|growthDuration|stageDuration|hoursPerStage)\s*=\s*([^\n]+)',body)}
        if values:berry_profiles[m[1]]=dict(values,source=ref('src/berry.c',berry_source.count('\n',0,m.start())+1))
    return dict(game_corner_offers=corner_offers,circuit_bp_pokemon_offers=bp_pokemon_offers,economic_constants=economic_constants,berry_profiles=berry_profiles,core_functions=sorted(core_functions),maps=by_map,records=records,prices=prices,free_categories=free,providers=providers,
        native_functions={k:native[k] for k in sorted(used_functions)},native_tables={k:arrays[k] for k in sorted(used_tables)},
        native_effect_calls=native_sinks,reward_sources={k:sorted(v) for k,v in sorted(reverse.items())},
        coverage=dict(maps=len(maps),interaction_entries=actors,script_transfer_sites=all_transfers,
            script_operations=len([r for r in records if r.startswith(('script ','native-call '))]),
            unclassified_economy_commands=unclassified_commands,operations_with_entry_paths=len(covered_operations),unbound_operations=sorted(set(r for r in records if r.startswith(('script ','native-call ')))-covered_operations),
            native_effect_calls=len(native_sinks),native_effects_associated_with_specials=len(native_covered),
            native_unassociated=[r for r in native_sinks if r['source'] not in native_covered],
            configured_entries_without_fixed_source=sorted(set(prices)-set(reverse)),record_kinds=dict(Counter(r['kind'] for r in records.values())),script_diagnostics=diagnostics),
        source_paths=source_paths)


def render_catalog(catalog):
    records=catalog['records'];coverage=catalog['coverage'];prices=catalog['prices']
    ids={key:f'R{i:04}' for i,key in enumerate(sorted(records),1)}
    provider_ids={key:f'N{i:03}' for i,key in enumerate(sorted(catalog['providers']),1)}
    table_ids={key:f'T{i:03}' for i,key in enumerate(sorted(catalog['native_tables']),1)}
    lines=['C. COMPLETE ECONOMY SOURCE ATLAS',
        'Current implementation. Proposed shop/reward changes remain separate in the guide. No source location is added or moved by this atlas.',
        'Read by location below, or use the reverse reward index. Generated R/N/T references are lookup keys, not new gameplay IDs.',
        'LITERAL means the source states the item/quantity. POTENTIAL PATH means a map entry can call that block in the source graph; it does not prove that every condition is satisfied in one save.',
        'Receipts, daily flags, stock selection, hidden-object rules and costs are retained at their actual source owners. Variables and alternate delivery attempts are not flattened into fabricated independent rewards.',
        'First physical access remains a separate geometry/story audit. The atlas contains the conditions needed to do that audit, not an invented first-badge number.\n',
        'C1. LOCATION MAP — every reward-bearing actor, pickup, trigger and map-scene source path']
    for map_name in sorted(catalog['maps']):
        lines.append('\n'+map_name)
        for key in catalog['maps'][map_name]:
            r=records[key];pos=f" ({r['x']},{r['y']})" if 'x' in r else ''
            actor=str(r.get('event_id',r.get('script','')))
            output=', '.join(display(x) for x in r.get('outputs',[]))
            lines.append(f"  {ids[key]} {r['kind']} {actor}{pos}"+(f': {output}' if output else '')+f" | {r['source']}")
            if 'quantity' in r:lines.append('    Quantity: '+str(r['quantity']))
            if r.get('initial'):lines.append('    Initial crop: '+str(r['initial'])+'; tree '+r['tree'])
            if 'conditions' in r:lines.append('    Conditions: '+compact(json.dumps(r['conditions'],ensure_ascii=False)))
            if r.get('references'):lines.append('    Potential operations: '+', '.join(ids[x] for x in r['references']))
            if r.get('species'):lines.append(f"    Species {r['species']}; method {r['method']}; common {r['common']}; rare {r['rare']}")
    lines+=['\nC2. SCRIPT TRANSACTIONS, SHOPS AND VARIABLE OFFERS',
        'All effects are printed once; location entries link to them. A zero base price is not permission to obtain an item: the actual shop/free-service entry must exist.',
        'Cash shop prices use GetItemPrice; sale events may apply the live discount in src/shop.c:GetShopItemPrice. BP/Coins/barter values are separate explicit costs.']
    for key in sorted(records):
        if not key.startswith(('script ','native-call ')):continue
        r=records[key];lines.append(f"\n{ids[key]} {r['kind']} | {r['source']} | {r['label']}")
        lines.append('  '+r['operation']+' '+', '.join(r.get('arguments',[])))
        if r.get('possible_maps'):lines.append('  Source-associated maps: '+', '.join(r['possible_maps']))
        else:lines.append('  NO MAP ENTRY ASSOCIATION: callback, macro, alternate/dormant or unbound path; not a confirmed campaign acquisition.')
        if r.get('quantity'):lines.append('  Quantity: '+str(r['quantity']))
        if r.get('selection_source'):lines.append('  Selector: '+r['selection_source'])
        if r.get('outputs'):
            outputs=[]
            for item in r['outputs']:
                price=prices.get(item,{}).get('base_price')
                outputs.append(item+(f' [base cash {price}]' if price is not None else ''))
            lines.append('  Outputs/stock: '+', '.join(outputs))
        if 'table' in r:lines.append('  Stock table: '+r['table'])
        if 'quoted_cost' in r:lines.append('  Quoted '+r.get('currency','cost')+': '+', '.join(r['quoted_cost']))
        if 'trade' in r:lines.append('  NPC TRADE: requested '+r['required_pokemon']+'; received '+r['trade']['species']+'; held '+r['trade'].get('heldItem','ITEM_NONE')+' | '+r['trade']['source'])
        if r.get('cost_rule'):lines.append('  Cost: '+r['cost_rule'])
        if r.get('local_inputs'):lines.append('  Caller inputs: '+compact(json.dumps(r['local_inputs'])))
        if r.get('native'):lines.append('  Native provider: '+', '.join(provider_ids[x] for x in r['native']))
        if r.get('conditions'):lines.append('  Local branch/receipt/cost statements: '+'; '.join(r['conditions']))
        for entry in r.get('entry_branches',[]):
            lines.append('  Possible incoming branch '+entry['label']+' | '+entry['source']+': '+'; '.join(entry['controls']))
        for var,candidates in r.get('variable_inputs',{}).items():
            values=defaultdict(set)
            for candidate in candidates:values[candidate['value']].add(candidate['source'])
            lines.append('  '+var+' POSSIBLE INPUTS (not a resolved item/price pairing): '+'; '.join(value+' @ '+','.join(sorted(locations)) for value,locations in sorted(values.items())))
        if r.get('unresolved'):lines.append('  UNRESOLVED: '+r['unresolved'])
    lines+=['\nC3. NATIVE PROVIDERS, EXACT REWARD TABLES AND GLOBAL RULES',
        'Native code owns variable species/items, free equipment, transaction ordering, duplicate alternatives and repeat limits. Read conditions before treating a table entry as an obtainable reward.',
        'Former Frontier/Trainer Hill/Tower prize routines can remain in source without a live campaign desk. They are labeled source-only below; procedural opponent teams are not included.']
    for key in sorted(catalog['providers']):
        r=catalog['providers'][key];lines.append(f"\n{provider_ids[key]} {key} | {r['source']}")
        lines.append('  Tables: '+(', '.join(table_ids[t] for t in r['tables']) or 'none; value is literal/computed/passed by caller'))
        lines.append('  Function dependencies: '+', '.join(r['functions']))
    lines+=['\nGAME CORNER POKEMON COUNTER — source coin prices',
        'Repeatable coin purchases; cash-to-Coin rates are a separate clerk transaction.']
    for row in catalog['game_corner_offers']:lines.append(row['species']+' | '+str(row['coins'])+' Coins | '+row['source'])
    lines+=['\nNATIVE SUPPLY / PRIZE / TRADE TABLES']
    for key in sorted(catalog['native_tables']):
        r=catalog['native_tables'][key]
        lines += [f"\n{table_ids[key]} {key} | {r['source']}",r['body']]
    lines+=['\nECONOMIC CONSTANTS — source expressions; conditional configuration must be respected']
    for row in catalog['economic_constants']:lines.append(row['name']+' = '+row['expression']+' | '+row['source'])
    lines+=['\nFREE CENTER STOCK — price0 at the free category service, not every vendor']
    for category,items in catalog['free_categories'].items():lines.append(category+': '+', '.join(items))
    lines+=['\nBERRY YIELD/GROWTH INPUTS — base table expressions; native growth/yield rules below apply']
    for berry,row in catalog['berry_profiles'].items():lines.append(berry+': '+compact(json.dumps(row)))
    lines+=['\nNATIVE ECONOMY FUNCTIONS — source conditions and formulas; each body appears once',
        'These are source dependencies, not a claim that every callback is reachable or every item operation creates new ownership.']
    for key in sorted(catalog['native_functions']):
        r=catalog['native_functions'][key]
        if r['sinks'] or key in catalog['providers'] or key in catalog['core_functions'] or (r['tokens'] and r['name'].startswith('Get')) or re.search(r'Reward|Prize|Gift|Fossil|SellPrice|ItemPrice|ProtectedFromLoss|Soot|BerryYield|HeldItemRarity|SetWildMonHeldItem|InGameTrade',r['name']):
            lines += ['\n'+key+' | '+r['source'],'\n'.join(line.rstrip() for line in r['body'].strip().splitlines())]
    lines+=['\nGLOBAL NATIVE EFFECT CALL REGISTER',
        'Transfer/shop calls are source write sites, not distinct rewards. Cost/state writes include storage transfers, restoration, temporary parties and consumed-item updates. A function without an NPC association can be a battle, new-game or minigame hook, or dormant legacy code.']
    for r in catalog['native_effect_calls']:
        lines.append(r['source']+' | '+str(r['function'])+' | '+r['kind']+' | '+r['operation']+'('+', '.join(r['arguments'])+')')
    lines+=['\nC4. REVERSE REWARD INDEX — every identified literal/table/free-stock source',
        'R = location or script operation; T = native data selector; free-category = free equipment service. Native-table and possible-script references are conditional candidates, not guaranteed sources. Variable native results must also be checked in C2/C3.']
    for item,keys in catalog['reward_sources'].items():
        out=[]
        for k in keys:
            if k in ids:out.append(ids[k])
            elif k.startswith('possible-script '):out.append('possible '+ids[k.removeprefix('possible-script ')])
            elif k.startswith('native-table '):out.append(table_ids[k.removeprefix('native-table ')])
            else:out.append(k)
        lines.append(item+': '+', '.join(out))
    lines+=['\nC5. CONFIGURED ITEM AND DECORATION PRICES',
        'These are configured base cash prices, evaluated as C expressions. Sell prices, ownership refusal, dynamic event discounts and other currencies follow their native/script rules. No shop availability is inferred from an item definition.']
    for item,r in prices.items():
        lines.append(f"{item}: cash {r['base_price']}; pocket {r['pocket']}; category {r['sort']}; importance {r['importance']}; notConsumed {r['not_consumed']}; field use {r['field_use']} | {r['source']}")
    lines+=['\nC6. COVERAGE AND DISCREPANCY REGISTER',
        compact(json.dumps({k:v for k,v in coverage.items() if k not in {'unbound_operations','native_unassociated','script_diagnostics','configured_entries_without_fixed_source'}},ensure_ascii=False)),
        'The checks protect extraction/source agreement; they do not certify every runtime branch, native shop UI, geometry, affordability or intended reward placement. A green structural census is not permission to remove these limits.',
        'Operations without a map-entry association: '+', '.join(ids[k] for k in coverage['unbound_operations']),
        'Unresolved script graph edges (standard/native/callback paths need their linked owners):']
    for r in coverage['script_diagnostics']['unresolved_edges']:
        lines.append(compact(json.dumps(r,ensure_ascii=False)))
    lines.append('Unresolved build conditions / duplicate labels are alternatives, not combined offers:')
    for r in coverage['script_diagnostics']['unresolved_build_conditions']:lines.append(compact(json.dumps(r,ensure_ascii=False)))
    lines.append('Duplicate script labels: '+', '.join(coverage['script_diagnostics']['duplicate_labels']))
    lines.append('Configured item/decor entries without a mapped literal/table/free-category source (not proof of unobtainability; variable providers and dormant definitions must be distinguished): '+', '.join(coverage['configured_entries_without_fixed_source']))
    return lines


def generate(root=ROOT):
    catalog=build_catalog(root);lines=render_catalog(catalog)
    return lines,catalog,catalog['source_paths']


if __name__=='__main__':
    import argparse
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True);args=p.parse_args()
    lines,catalog,paths=generate();args.out.mkdir(parents=True,exist_ok=True)
    (args.out/'economy.txt').write_text('\n'.join(x.rstrip() for x in lines)+'\n')
    catalog['source_paths']=sorted(str(x.relative_to(ROOT)) for x in paths)
    (args.out/'economy.json').write_text(json.dumps(catalog,indent=2,ensure_ascii=False)+'\n')
    print(json.dumps({k:v for k,v in catalog['coverage'].items() if k not in {'script_diagnostics','unbound_operations','native_unassociated'}},indent=2))
