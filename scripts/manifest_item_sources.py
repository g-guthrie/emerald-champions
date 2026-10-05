"""Acquisition-site inventory for the campaign manifest, from executable source.

This deliberately preserves locations and conditions instead of cap windows.
A source is an offer, not proof that its map is reachable or its price affordable.
Wild-held items and Pickup-generated rewards are excluded by the requested scope.
"""
from pathlib import Path
import re
import economy_reference as er
import reference_pool as rp

ROOT = Path(__file__).resolve().parents[1]
EXCLUDED_KINDS = {'wild_held', 'wild held item', 'pickup'}


def build_optional_activities(builder, sources):
    """Live optional fight/resource branches, without awarding their outputs."""
    activities=[]
    for town,facility,party in [('Slateport','FACTORY','three temporary rentals; owned party saved and restored'),
                                ('Verdanturf','PALACE','three selected owned Pokemon; moves chosen by Palace nature rules')]:
        prefix=town+('City' if town=='Slateport' else 'Town')+'_BattleTent'
        lobby=prefix+'Lobby';room=prefix+'BattleRoom'
        obj=next(o for o in builder.geo.maps[lobby]['object_events'] if o.get('script')==lobby+'_EventScript_Attendant')
        rewards=[r['key'] for r in sources if r['kind']=='tent_prize' and r['where']==lobby]
        battle_source=_ref(f'data/maps/{room}/scripts.inc','dofacilitytrainerbattle') if town=='Slateport' else _ref('data/maps/BattleFrontier_BattlePalaceBattleRoom/scripts.inc','dofacilitytrainerbattle')
        activities.append(dict(id='activity-tent-'+town.lower(),kind='three_battle_challenge',where=(lobby,obj['x'],obj['y']),
            repeatable=True,requires=[[]],party=party,party_size=3,battle_mode='doubles',facility='FRONTIER_FACILITY_'+facility,
            ownership='rented opponents/Slateport rentals never become owned Pokemon or held-item acquisitions',
            opponent_pool={'trainers':'g'+town+'BattleTentTrainers','sets':'g'+town+'BattleTentMons','source':'src/data/battle_frontier/battle_tent.h'},
            level_rule='Slateport rentals at TENT_MIN_LEVEL (30)' if town=='Slateport' else 'opponent level max(30, highest player party level); owned entrant levels preserved',
            fights=[dict(index=i,kind='facility_battle',source=battle_source,requires_previous_wins=i-1,
                story_effects=[],reward_after_win=(dict(kind='pending_random_item',alternatives=rewards,quantity=1) if i==3 else None)) for i in (1,2,3)],
            reward_condition='three consecutive wins, then claim one uniformly selected prize; pending prize survives full bag',
            prize_source='src/battle_tent.c:'+str((ROOT/'src/battle_tent.c').read_text().count(chr(10),0,(ROOT/'src/battle_tent.c').read_text().find('static void SetRandom'+town+'TentPrize(void)\n{'))+1),
            conditions=['actual frontier eligibility and valid selected party checks remain required','battle mode is doubles although the authored legacy selection count is three'],
            citations=[_ref(f'data/maps/{lobby}/scripts.inc',lobby+'_EventScript_Attendant::'),_ref(f'data/maps/{room}/scripts.inc','case 3,'),
                'src/battle_tower.c:SetTentPtrsGetLevel','include/constants/battle_tent.h:TENT_MIN_LEVEL']))
    activities.append(dict(id='activity-fallarbor-closed',kind='inactive_challenge',where='FallarborTown_BattleTentLobby',
        campaign_scope='closed challenge raw appendix',reason='live attendant offers only pending-prize claim or Closed dialogue; no fresh challenge entry',
        source='data/maps/FallarborTown_BattleTentLobby/scripts.inc:105'))
    activities.append(dict(id='activity-champions-tents-unhooked',kind='inactive_challenge',campaign_scope='unhooked script raw appendix',
        reason='ChampionsTent_EventScript_Attendant has no live map event or script caller; its six-owned-Pokemon current-cap Showdown mode cannot be reached through the actual legacy attendants',
        source='data/scripts/champions_tent.inc:1; src/champions_circuit.c:1560'))
    glass=(ROOT/'data/maps/Route113_GlassWorkshop/scripts.inc').read_text()
    prices={name:int(value) for name,value in re.findall(r'\.set\s+(\w+),\s*(\d+)',glass)}
    glass_purchases=[dict(item=item,costs={'ash':prices[price]},where='Route113_GlassWorkshop',
        source='data/maps/Route113_GlassWorkshop/scripts.inc:'+str(glass.count(chr(10),0,match.start())+1))
        for match in re.finditer(r'setvar VAR_0x8008, ((?:ITEM|DECOR)_\w+)\s+setvar VAR_0x800A, (\w+)',glass)
        for item,price in [match.groups()] if price in prices]
    activities.append(dict(id='activity-soot-gathering',kind='resource_gathering',where='Route113',needs_items=['ITEM_SOOT_SACK'],
        resource='VAR_ASH_GATHER_COUNT',yield_per_ash_tile=1,maximum_balance=9999,battle_requirement=False,
        conditions=['step onto reachable ash-covered grass; the tile loses its ash after collection',
                    'repeat visits/gathering must supply the quoted debit; spending ash does not reduce cumulative VAR_EC_SOOT_PROGRESS'],
        purchases=glass_purchases+[dict(item=r['key'],costs=r.get('costs'),transaction_conditions=r.get('transaction_conditions'),source=r['cite'])
                   for r in sources if r.get('kind')=='soot_exchange' and not r.get('campaign_scope')],
        citations=['src/field_tasks.c:CollectAsh','src/field_tasks.c:AshGrassPerStepCallback','src/inclement_stat_services.c:ExchangeSootForCaps']))
    return activities


def _ref(path, needle):
    text = (ROOT / path).read_text()
    pos = text.find(needle)
    return f'{path}:{text.count(chr(10), 0, pos) + 1}' if pos >= 0 else path


def _controls(builder, label):
    entries, labels = builder.scripts.reach_entries(label)
    return [dict(label=lab, source=f"{rp.rel(builder.scripts.labels[lab]['file'])}:{n}", condition=line)
            for lab in sorted(labels) for n, line in builder.scripts.labels[lab]['body']
            if re.match(r'(?:goto_if|call_if|case|check|switch|remove|setflag|clearflag)', line)]


def build_item_sources(builder, economy=None, parser=None, enrich=True):
    """Return location-based sources, services and unresolved native providers.

    Source rows use the Builder Source contract (key, where, requires DNF,
    needs_species/items). Conditions retained alongside DNF must be respected by
    consumers. Conditional native stock is never mistaken for an unconditional
    item source. Catalog may be supplied to avoid duplicate source indexing.
    """
    catalog = economy or er.build_catalog(ROOT)
    from manifest_battle_progression import ProgressionParser
    parser = parser or getattr(builder.scripts, '_manifest_progression_parser', None)
    if parser is None:
        parser=ProgressionParser(builder.scripts)
        builder.scripts._manifest_progression_parser=parser
    cache=getattr(parser,'_manifest_item_paths',None)
    if cache is None:
        cache={};parser._manifest_item_paths=cache
    rows = []; services = []; unresolved = []
    prices = catalog['prices']
    def add(item, kind, where, cite, requires=None, detail='', quantity=1,
            needs_species=(), needs_items=(), **extra):
        if item in {'ITEM_NONE', 'DECOR_NONE'}: return
        row = dict(key=item, kind=kind, where=where, requires=requires or [[]],
                   needs_species=[builder.species.resolve(sp) for sp in needs_species], needs_items=list(needs_items),
                   detail=detail, cite=cite, quantity=quantity, **extra)
        if kind in {'mart', 'vendor_form_items', 'vendor_badge_stock', 'vendor_species', 'vendor_restock'}:
            row['base_price'] = prices.get(item, {}).get('base_price')
            form = item in re.findall(r'ITEM_\w+', (ROOT/'src/data/emerald_champions_form_items.h').read_text())
            equipment = item in rp.EQUIPMENT_SPECIES or prices.get(item, {}).get('sort') == 'ITEM_TYPE_MEMORY'
            row['price'] = 0 if form and not equipment else row['base_price']
            row['repeatable'] = True
            row['affordability']='requires enough cash; a listed offer does not imply funds already available'
            if item in {'ITEM_NET_BALL','ITEM_DIVE_BALL'}:
                row['discount']='half base price only at Catching Guru stall while Slateport PokeNews is active (src/shop.c:671)'
            row['price_source'] = [prices.get(item, {}).get('source'), 'src/shop.c:667', 'src/field_specials.c:5369']
        rows.append(row)
        return row
    for source in builder.item_sources:
        if source.kind in EXCLUDED_KINDS or source.kind == 'harvest_trade' or 'wild_held' in source.kind: continue
        # Static marts are rebuilt below from their actual call sites; keeping
        # the older table-only rows would bypass exact branch conditions.
        if source.kind=='mart' and source.detail!='Lilycove Dept. Store 4F evolution specialist':continue
        row = add(source.key, source.kind, source.where, source.cite,
            [list(x) for x in source.requires], source.detail, needs_species=source.needs_species,
            needs_items=source.needs_items, source_extra=source.extra)
        match = re.search(r'(finditem|giveitem|giveuniqueitem|additem|addpcitem|giveitem_msg) at (\w+)', source.detail)
        if match:
            label = match[2]; info = builder.scripts.labels[label]
            line = next((line for n, line in info['body'] if re.match(match[1]+r'\s+'+source.key+r'(?:,|$)', line)), '')
            args = er.arguments(line.split(None, 1)[1]) if line else []
            row['quantity'] = args[1] if len(args)>1 else 1
            row['controls'] = _controls(builder, label)
            row['repeatability'] = 'receipt flags and daily checks retained in controls; bag-space failures leave unreceived rewards retryable where source permits'
        if "starter-pair Mega Stone" in source.detail:
            row['selection']='only selected starter-pair final-form stones; Swampertite fallback if that pair has no Mega; other Hoenn stones require showing the corresponding starter line'
            row['native_conditions']=['src/mega_stone_rewards.c:sStarterMegaStones','src/mega_stone_rewards.c:IsNormanStarterMegaStone','src/mega_stone_rewards.c:GetNormanStarterMegaStone']
        if source.kind == 'hidden':
            map_name=source.where[0] if isinstance(source.where,tuple) else source.where
            event=next((x for x in builder.geo.maps.get(map_name,{}).get('bg_events',[]) if x.get('item')==source.key and (x.get('x'),x.get('y'))==source.where[1:]),{})
            row['receipt']=event.get('flag');row['quantity']=event.get('quantity',1)
        if source.kind=='gift' and 'flower shop daily bundle' in source.detail:
            row.update(repeatable='daily',receipt='FLAG_DAILY_FLOWER_SHOP_RECEIVED_BERRY',quantity=1,
                retry='atomic bundle contains six fixed EV berries and one randomly selected common berry; failed delivery permits retry')
        if source.kind == 'vendor_kit':
            row.update(quantity=1, receipt='FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS', repeatable=False,
                retry='all six items together; bag-full failure records no receipt')
        if source.kind == 'vendor_form_items':
            species = rp.EQUIPMENT_SPECIES.get(source.key)
            if prices.get(source.key, {}).get('sort') == 'ITEM_TYPE_MEMORY': species='SPECIES_SILVALLY'
            if species:
                row['needs_species']=[builder.species.resolve(species)]; row['stock_condition']='caught equipment species or prior item acquisition'
    # Generic modern item-ball objects encode item and quantity in map.json,
    # rather than literal finditem commands. Their local flags close the source.
    for name, m in builder.geo.maps.items():
        raw=(ROOT/f'data/maps/{name}/map.json').read_text()
        for obj in m.get('object_events') or []:
            if obj.get('script') != 'Common_EventScript_FindItem': continue
            item=obj.get('trainer_sight_or_berry_tree_id')
            if not isinstance(item,str) or not item.startswith('ITEM_'): continue
            add(item,'item_ball',(name,obj['x'],obj['y']),_ref(f'data/maps/{name}/map.json', '"local_id": '+str(obj.get('local_id'))),
                detail='visible overworld pickup',quantity=obj.get('movement_range_x') or 1,
                receipt=obj.get('flag'),native_conditions=['src/event_object_movement.c:TrySpawnObjectEventTemplate',
                'src/field_specials.c:GetItemBallIdAndAmountFromTemplate', 'data/scripts/item_ball_scripts.inc:Common_EventScript_FindItem'])
    # Audit direct live transfer commands and every actual static stock call.
    # Specialty Center ball stocks share the PokeMart_ prefix with badge-tier
    # tables; discovery must not discard them merely because of that prefix.
    existing={(r['key'],r['cite'].split(';')[0]) for r in rows}
    source_coverage={'literal_transfer_sites':0,'added_literal_transfer_sites':[], 'unbound_literal_transfer_sites':[], 'static_shop_calls':0}
    for label,info in builder.scripts.labels.items():
        if info['file'].name in ('debug.inc','new_game.inc'):continue
        for line,text in info['body']:
            reference=f"{rp.rel(info['file'])}:{line}"
            transfer=re.match(r'(finditem|giveitem|giveuniqueitem|additem|addpcitem|giveitem_msg)\s+(ITEM_\w+)',text)
            if transfer and transfer[2]!='ITEM_NONE':source_coverage['literal_transfer_sites']+=1
            if transfer and transfer[2]!='ITEM_NONE' and info['file'].name=='cable_club.inc':
                source_coverage.setdefault('external_literal_transfer_sites',[]).append({'key':transfer[2],'cite':reference,'label':label})
                entries,_=builder.scripts.reach_entries(label,depth=len(builder.scripts.labels))
                for event in entries or [('External_Link_Session',None,None,'external',None)]:
                    where=(event[0],event[1],event[2]) if event[1] is not None else event[0]
                    args=er.arguments(text.split(None,1)[1])
                    add(transfer[2],'external_gift',where,reference,detail=f'{transfer[1]} at {label}',
                        quantity=args[1] if len(args)>1 else 1,controls=_controls(builder,label),
                        campaign_scope='external multiplayer raw appendix',
                        activity_requirement={'kind':'external_link_session','source':'data/scripts/cable_club.inc'},
                        eligibility_status='external multiplayer raw appendix')
                continue
            if transfer and transfer[2]!='ITEM_NONE' and (transfer[2],reference) not in existing:
                if not enrich:
                    found=builder.scripted(label,line)
                    source_coverage['added_literal_transfer_sites' if found else 'unbound_literal_transfer_sites'].append({'key':transfer[2],'cite':reference,'label':label})
                    for req,where in found:
                        args=er.arguments(text.split(None,1)[1])
                        add(transfer[2],'item_ball' if transfer[1]=='finditem' else 'gift',where,reference,[list(req)],
                            detail=f'{transfer[1]} at {label}',quantity=args[1] if len(args)>1 else 1)
                    continue
                if (label,line) not in cache:cache[(label,line)]=parser.paths_to(label,line)
                paths=cache[(label,line)]
                source_coverage['added_literal_transfer_sites' if paths else 'unbound_literal_transfer_sites'].append({'key':transfer[2],'cite':reference,'label':label})
                for path in paths:
                    event=path.get('event')
                    if not event:continue
                    where=(event[0],event[1],event[2]) if event[1] is not None else event[0]
                    args=er.arguments(text.split(None,1)[1])
                    add(transfer[2],'item_ball' if transfer[1]=='finditem' else 'gift',where,reference,
                        detail=f'{transfer[1]} at {label}',quantity=args[1] if len(args)>1 else 1,
                        controls=_controls(builder,label),eligibility_paths=[path],eligibility_status='parsed script paths')
            shop=re.match(r'(pokemart|pokemartbuy|pokemartdecoration|pokemartdecoration2)\s+(\w+)',text)
            if not shop:continue
            source_coverage['static_shop_calls']+=1
            for req,where in builder.scripted(label,line):
                for item in builder._mart_table(info['file'],shop[2]):
                    if item in ('ITEM_NONE','DECOR_NONE'):continue
                    add(item,'mart',where,reference,[list(req)],detail=shop[2],quantity='buy selected quantity',
                        controls=_controls(builder,label))
    # Tree IDs in current map source are symbolic, while older inventories
    # assumed numeric IDs. Read both forms directly and retain empty soil.
    new_game=(ROOT/'data/scripts/new_game.inc').read_text()
    seeds={tree:(berry,stage) for tree,berry,stage in re.findall(r'setberrytree\s+(BERRY_TREE_\w+),\s*BERRY_ID_(\w+),\s*(\w+)',new_game)}
    numeric={value:key for key,value in re.findall(r'#define\s+(BERRY_TREE_\w+)\s+(\d+)',(ROOT/'include/constants/berry.h').read_text())}
    for name,m in builder.geo.maps.items():
        for obj in m.get('object_events') or []:
            if obj.get('script')!='BerryTreeScript':continue
            raw=str(obj.get('trainer_sight_or_berry_tree_id'));tree=numeric.get(raw,raw)
            where=(name,obj['x'],obj['y'])
            if tree not in seeds:
                services.append(dict(kind='berry_soil',where=where,tree=tree,
                    cite=f'data/maps/{name}/map.json',condition='empty until player plants an acquired seed'))
                continue
            berry,stage=seeds[tree]
            add(f'ITEM_{berry}_BERRY','berry_tree',where,_ref('data/scripts/new_game.inc',tree),
                detail=f'initial planted berry tree {tree}',quantity='native harvest yield',initial_stage=stage,
                repeatable=True,growth_profile=catalog['berry_profiles'].get('BERRY_ID_'+berry,{}),
                retry='bag-full failure adds no berries or harvest credits; tree remains available',
                propagation='plant any acquired berry in available soil; wait for native growth, then harvest its type',
                native_conditions=['src/berry.c:PlantBerryTree','src/berry.c:BerryTreeGrow','src/berry.c:CalcBerryYield','src/berry.c:ObjectEventInteractionPickBerryTree'])
    # Native flower bundle picks one of the first eight berries plus all six
    # EV berries. The random berry is an alternative, not eight gifts at once.
    for berry in re.findall(r'F\((\w+)\)',(ROOT/'include/constants/berries.h').read_text())[:8]:
        add(f'ITEM_{berry}_BERRY','random_berry','Route104_PrettyPetalFlowerShop',
            'data/maps/Route104_PrettyPetalFlowerShop/scripts.inc:107; src/field_specials.c:516',
            detail='flower-shop daily bundle random berry',quantity=1,selection='one of the first eight berry types',
            receipt='FLAG_DAILY_FLOWER_SHOP_RECEIVED_BERRY',repeatable='daily',retry='atomic seven-item bundle; full bag permits retry')
    # Birch native gift carries one Eviolite; opening starters have none.
    add('ITEM_EVIOLITE','gift_held','LittlerootTown_ProfessorBirchsLab','src/script_pokemon_util.c:370',
        detail='first Cosmog of Birch gift pair carries Eviolite',quantity=1,needs_species=['SPECIES_COSMOG'])
    # Dynamic script transfers, random berries, trades and decoration shops.
    supported_native={'GiveEmeraldChampionsStarterBattleItems','GiveFlowerShopBerryBundle',
        'OpenEmeraldChampionsBattleItemMart','OpenEmeraldChampionsEvolutionItemArchive',
        'OpenEmeraldChampionsEvolutionSpecialist','CheckEmeraldChampionsHandoffItem',
        'TakeEmeraldChampionsHandoffItem','ConvertEmeraldChampionsFiniteReward',
        'GiveBirchCosmogPair','GiveEmeraldChampionsGameCornerPokemon','GiveEggFromDaycare',
        'TryGiveSelectedLegendarySignReward','TryGiveArceusLegendarySignMasteryReward',
        'ChampionsTentTryGivePrize','ExchangeSootForCaps','TradeEmeraldChampionsGardenBerries',
        'ObjectEventInteractionPickBerryTree','ObjectEventInteractionPickApricornTree',
        'TraderShowDecorationMenu','TraderDoDecorationTrade','GiveFrontierBattlePoints'}
    for ident, rec in catalog['records'].items():
        operation=rec.get('operation'); outputs=rec.get('outputs',[])
        if rec.get('kind')=='wild held item': continue
        if rec.get('kind','').startswith('native'):
            target=rec.get('arguments',[''])[1 if operation=='specialvar' else 0]
            if target not in supported_native:
                services.append(dict(id=ident, **rec))
                if rec.get('native') and not outputs and re.search(r'^(?:Give|TryGive|Trade|Exchange|ChampionsTentTryGive|ObjectEventInteractionPick)',target):
                    unresolved.append(dict(id=ident, operation=target, possible_maps=rec.get('possible_maps',[]),
                        cite=rec['source'], native=rec['native'], conditions=rec.get('conditions'),
                        note='inspect native provider; no unconditional item availability inferred'))
            continue
        label=rec.get('label'); line_no=int(rec['source'].rsplit(':',1)[1]) if label else None
        relevant=operation in {'giverandomberry','ingame_trade','givedecoration','adddecoration','pokemartdecoration','pokemartdecoration2'}
        dynamic=operation in er.TRANSFER and not outputs and rec.get('variable_inputs')
        if not relevant and not dynamic: continue
        paths=builder.scripted(label,line_no) if label in builder.scripts.labels else []
        if not paths: continue
        alternatives=outputs[:]
        if dynamic:
            alternatives=sorted({token for values in [rec['variable_inputs'].get(rec.get('arguments',[''])[0],[])] for value in values
                for token in er.TOKENS.findall(value['value']) if token.startswith(('ITEM_','DECOR_'))})
        for item in alternatives:
            if not item.startswith(('ITEM_','DECOR_')): continue
            for req,where in paths:
                trade=rec.get('trade',{})
                add(item,'trade_held' if operation=='ingame_trade' else 'random_berry' if operation=='giverandomberry' else 'decoration' if item.startswith('DECOR_') else 'dynamic_gift',
                    where,rec['source'],[list(req)],quantity=rec.get('quantity',rec.get('arguments',[None,1])[1] if len(rec.get('arguments',[]))>1 else 1),
                    needs_species=[rec['required_pokemon']] if rec.get('required_pokemon') else [],
                    controls=_controls(builder,label), selection='one alternative selected by source branches' if dynamic or operation=='giverandomberry' else None,
                    native_trade=trade, variable_inputs=rec.get('variable_inputs'), costs=rec.get('quoted_cost'),
                    delivery_item_argument=rec.get('arguments',[''])[0] if dynamic else None)
    # Script random/addvar berry donors use variables rather than literal
    # item tokens. Resolve ranges from current item constants in berry order.
    berry_names=re.findall(r'F\((\w+)\)',(ROOT/'include/constants/berries.h').read_text())
    constants=(ROOT/'include/constants/items.h').read_text()
    donor_defs=[('NUM_BERRY_MASTER_BERRIES','FIRST_BERRY_MASTER_BERRY','LAST_BERRY_MASTER_BERRY','Route123_BerryMastersHouse',2),
        ('NUM_BERRY_MASTER_WIFE_BERRIES','FIRST_BERRY_MASTER_WIFE_BERRY','LAST_BERRY_MASTER_WIFE_BERRY','Route123_BerryMastersHouse',1),
        ('NUM_KIRI_BERRIES','FIRST_KIRI_BERRY','LAST_KIRI_BERRY','SootopolisCity',1),
        ('NUM_ROUTE_114_MAN_BERRIES','FIRST_ROUTE_114_MAN_BERRY','LAST_ROUTE_114_MAN_BERRY','Route114',1)]
    for random_range,first,last,where,draws in donor_defs:
        bounds=[]
        for token in (first,last):
            match=re.search(r'#define\s+'+token+r'\s+ITEM_(\w+)_BERRY',constants)
            if not match:raise ValueError('Unresolved random berry boundary '+token)
            bounds.append(berry_names.index(match[1]))
        path=f'data/maps/{where}/scripts.inc'
        if 'random '+random_range not in (ROOT/path).read_text():raise ValueError('Berry donor path changed: '+random_range)
        for berry in berry_names[bounds[0]:bounds[1]+1]:
            add(f'ITEM_{berry}_BERRY','random_berry',where,_ref(path,'random '+random_range),
                detail='random daily berry gift',quantity=1,random_draws=draws,
                selection=f'{draws} random draws; each draw independently selects one berry from the source range',
                repeatable='daily',retry='only successful delivery records the daily receipt')
    lottery=(ROOT/'src/lottery_corner.c').read_text()
    prizes=re.findall(r'ITEM_\w+',re.search(r'sLotteryPrizes\[\]\s*=\s*\{(.*?)\};',lottery,re.S)[1])
    for index,item in enumerate(prizes):
        add(item,'lottery_prize','LilycoveCity_DepartmentStore_1F',_ref('src/lottery_corner.c','sLotteryPrizes'),
            detail=f'lottery prize for {index+2} matching OT-ID digits',quantity=1,repeatable='daily',
            selection='best party or PC non-egg OT-ID match; lottery result is random',
            activity_requirement={'kind':'lottery_match','digits':index+2,'source':'src/lottery_corner.c:PickLotteryCornerTicket'},
            retry='full-bag prize retained in VAR_POKELOT_PRIZE_ITEM for later collection',
            receipt='FLAG_DAILY_PICKED_LOTO_TICKET',native_conditions=['data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc:9','src/lottery_corner.c:PickLotteryCornerTicket'])
    # Native campaign-side activities mint these prizes. They are optional
    # transactions/battle detours, not unconditional map-arrival grants.
    soot_path='src/inclement_stat_services.c'
    add('ITEM_BOTTLE_CAP','soot_exchange','Route113_GlassWorkshop',_ref(soot_path,'void ExchangeSootForCaps'),
        detail='Glass Workshop soot exchange',quantity='1, 5, or all affordable caps',repeatable=True,
        needs_items=['ITEM_SOOT_SACK'],costs={'soot_per_cap':500},
        cost_rule='500 gathered ash units per cap; debit only after successful bag delivery',
        native_conditions=['include/constants/emerald_champions.h:20'])
    tent=(ROOT/'src/battle_tent.c').read_text()
    for town in ('Slateport','Verdanturf','Fallarbor'):
        table='s'+town+'TentRewards'
        body=re.search(table+r'\[\]\s*=\s*\{(.*?)\};',tent,re.S)[1]
        for item in re.findall(r'ITEM_\w+',body):
            row=add(item,'tent_prize',town+('Town' if town!='Slateport' else 'City')+'_BattleTentLobby',
                _ref('src/battle_tent.c',table),detail='random Tent completion prize after three wins',quantity=1,repeatable=True,
                selection='one randomly selected reward after completing the local tent battle run',
                activity_requirement={'kind':'tent_battle_run','town':town,'wins':3,'source':f'data/maps/{town+("Town" if town!="Slateport" else "City")}_BattleTentBattleRoom/scripts.inc'},
                native_conditions=[f'data/maps/{town+("Town" if town!="Slateport" else "City")}_BattleTentLobby/scripts.inc','src/battle_tent.c:SetRandom'+town+'TentPrize'],
                retry='pending prize remains claimable when bag delivery fails')
            if town=='Fallarbor':
                row['campaign_scope']='closed challenge raw appendix'
                row['eligibility_status']='closed challenge raw appendix'
    lady=(ROOT/'src/data/lilycove_lady.h').read_text()
    for table,kind in [('sQuizLadyQuestions','quiz_prize'),('sFavorLady','favor_prize')]:
        match=re.search(r'\b'+table+r'\[\]\s*=\s*\{(.*?)\n\};',lady,re.S)
        if not match:raise ValueError('Missing Lilycove lady reward table '+table)
        for item in sorted(set(re.findall(r'\.prize\s*=\s*(ITEM_\w+)',match[1]))):
            add(item,kind,'LilycoveCity_PokemonCenter_1F',_ref('src/data/lilycove_lady.h',table),
                detail='Lilycove lady native reward',quantity=1,
                selection='lady identity depends on player trainer ID; current quiz/favor selection is random; record-mixed custom prizes excluded',
                cost_rule='correct quiz answer' if kind=='quiz_prize' else 'complete selected favor by supplying accepted items',
                native_conditions=['src/lilycove_lady.c:InitLilycoveLady','data/scripts/lilycove_lady.inc'])
    trader=(ROOT/'src/trader.c').read_text()
    defaults=re.search(r'sDefaultTraderDecorations\[NUM_TRADER_ITEMS\]\s*=\s*\{(.*?)\};',trader,re.S)[1]
    for item in re.findall(r'DECOR_\w+',defaults):
        add(item,'decoration_trade','MauvilleCity_PokemonCenter_1F',_ref('src/trader.c','sDefaultTraderDecorations'),
            detail='Trader default decoration offer',quantity=1,
            selection='old-man identity determines whether Trader is present; one trade until reset/record mixing',
            cost_rule='exchange an owned decoration of an allowed kind',
            native_conditions=['src/trader.c:TraderDoDecorationTrade','data/scripts/mauville_man.inc:220'])
    # Native Devon legendary rewards apply a uniformly selected usable
    # non-Mega Doubles preset, including its held item. These are gifts, not
    # captured wild held-item sources. Read the live C range and preset tables.
    sets_path='src/data/pokemon/emerald_champions_battle_sets.h'
    sets_text=(ROOT/sets_path).read_text()
    preset_start=sets_text.index('gEmeraldChampionsBattleSets[]')
    presets=list(re.finditer(r'\.preset\s*=\s*\{(.*?)\}\}',sets_text[preset_start:],re.S))
    gift_calls={'SPECIES_MAGEARNA':'TryGiveSelectedLegendarySignReward',
        'SPECIES_ARCEUS':'TryGiveArceusLegendarySignMasteryReward'}
    for species,handler in gift_calls.items():
        range_match=re.search(r'\[EC_BATTLE_FORMAT_DOUBLES\]\['+species+r'\]\s*=\s*\{\.offset = (\d+), \.count = (\d+)\}',sets_text)
        if not range_match:raise ValueError('Missing native gift set range '+species)
        offset,count=map(int,range_match.groups())
        for index in range(offset,offset+count):
            preset=presets[index];fields=dict(re.findall(r'\.(item|requiredItem|requiredMove)\s*=\s*(\w+)',preset[1]))
            if fields.get('requiredItem')!='ITEM_NONE' or fields.get('requiredMove')!='MOVE_NONE':continue
            item=fields.get('item','ITEM_NONE')
            if item=='ITEM_NONE':continue
            cite=f"{sets_path}:{sets_text.count(chr(10),0,preset_start+preset.start())+1}; src/legendary_signs.c:949"
            for label,info in builder.scripts.labels.items():
                if info['map']!='RustboroCity_DevonCorp_2F':continue
                for line,text in info['body']:
                    if text!='special '+handler:continue
                    requirement,needs,note=builder.legend_requirement(species)
                    for req,where in builder.scripted(label,line):
                        add(item,'gift_held',where,cite,[list(set(req)|set(requirement))],
                            detail=f'held by Devon native {species} gift',quantity=1,needs_species=needs,
                            selection='one randomly selected non-transformation Doubles gift preset; exactly one held item',
                            native_conditions=['src/legendary_signs.c:ApplyNonMegaGiftSet','src/emerald_champions_battle_sets.c:246'],
                            eligibility_target=(label,line),gift_selection_species=species)
    # Starting PC storage is accessible before the opening rescue battle.
    path='src/player_pc.c'; body=(ROOT/path).read_text()
    table=re.search(r'sNewGamePCItems\[\]\[2\]\s*=\s*\{(.*?)\};',body,re.S)
    for item,qty in re.findall(r'\{\s*(ITEM_\w+),\s*(\d+)\s*\}',table[1]):
        add(item,'starting_pc','LittlerootTown_BrendansHouse_2F',_ref(path,'sNewGamePCItems'),quantity=int(qty),detail='initial PC inventory; withdraw at home or any PC')
    # Repeat stock unlocks only after a legitimate source has been acquired;
    # excluded wild-held/Pickup sources never seed this dependency.
    for category, items in builder.catalogue.items():
        for item in items:
            species=rp.EQUIPMENT_SPECIES.get(item)
            if prices.get(item,{}).get('sort')=='ITEM_TYPE_MEMORY':species='SPECIES_SILVALLY'
            add(item,'vendor_species' if species else 'vendor_restock','OldaleTown_PokemonCenter_1F',
                'src/field_specials.c:485; src/field_specials.c:506; src/field_specials.c:627',
                detail=f'Center catalogue category {category}',needs_species=[species] if species else [],
                needs_items=[] if species else [item],stock_condition='caught equipment species or previously acquired item' if species else 'first legitimate acquisition permanently unlocks paid duplicates')
    # Harvest-pouch Mega rewards spend recorded harvested quantities, not gifted
    # berry bag contents. Seeds may be replanted; farming is an optional detour.
    path='src/mega_stone_rewards.c';text=(ROOT/path).read_text()
    table=re.search(r'sBerryStoneTrades\[\]\s*=\s*\{(.*?)\n\};',text,re.S)[1]
    harvest_gate = re.search(r'if \(choice != 3 && !FlagGet\((FLAG_\w+)\)\)', text)[1]
    starts=list(re.finditer(r'\{(ITEM_\w+),\s*(FLAG_\w+|0),',table))
    for index,m in enumerate(starts):
        body=table[m.end():starts[index+1].start() if index+1<len(starts) else len(table)]
        recipe=[dict(item=f'ITEM_{berry}_BERRY',count=int(qty)) for berry,qty in re.findall(r'\{BERRY_ID_(\w+),\s*(\d+)\}',body)]
        if m[1]=='ITEM_NONE': continue
        add(m[1],'harvest_trade','Route123_BerryMastersHouse',_ref(path,'sBerryStoneTrades'), [[harvest_gate]],
            needs_items=[r['item'] for r in recipe],detail='trade harvested berry credits',receipt=m[2],recipe=recipe,activity_requirement={'kind':'harvest_credit_trade','recipe':recipe,'source':'src/mega_stone_rewards.c:39'},
            cost_rule='credits from successfully harvested berries; gifted/purchased berries give no credit until planted and harvested')
    # Relics are native gifts earned by obtaining their legendary species,
    # including Arceus plates omitted by older reference-pool convenience data.
    path='src/legendary_signs.c';text=(ROOT/path).read_text()
    items=re.findall(r'ITEM_\w+',re.search(r'sLegendaryRelicItems\[\]\s*=\s*\{(.*?)\};',text,re.S)[1])
    grants=re.search(r'sLegendaryRelicGrants\[\]\s*=\s*\{(.*?)\};',text,re.S)[1]
    for species,first,count,champion in re.findall(r'\{(SPECIES_\w+),\s*(\d+),\s*(\d+),\s*(TRUE|FALSE)\}',grants):
        for item in items[int(first):int(first)+int(count)]:
            add(item,'legendary_relic','OldaleTown_PokemonCenter_1F',_ref(path,'sLegendaryRelicGrants'),
                [['FLAG_IS_CHAMPION']] if champion=='TRUE' else None,needs_species=[species],
                detail='native earned relic gift; delivered on capture or next Center heal if pending',repeatable=False,
                unlock_condition='earned when species obtained; champion-only forms held until Hall of Fame; pending gifts retry when inventory has space')
    # Retain both positive and negative native/script comparisons and battles
    # along the actual call/return path to each literal delivery command.
    # A shared parser and target cache avoid repeating this for duplicate rows.
    # Equipment stock has two independent native unlock branches. Preserve
    # each as its own source so standard species/item dependency checks can
    # evaluate the OR without assuming every owned/evolved form was caught.
    for row in list(rows):
        if row['kind'] in ('vendor_species','vendor_form_items') and row['needs_species']:
            alternate=dict(row,kind='vendor_equipment_restock',needs_species=[],needs_items=[row['key']],
                stock_condition='previous legitimate acquisition unlocks equipment even without its species caught',
                native_handler='OpenEmeraldChampionsEvolutionItemArchive' if row['kind']=='vendor_form_items' else 'OpenEmeraldChampionsBattleItemMart')
            rows.append(alternate)
    sites={}
    native_sites={}
    for label,info in builder.scripts.labels.items():
        for line,command_text in info['body']:
            if re.match(r'(?:finditem|giveitem|giveuniqueitem|additem|addpcitem|giveitem_msg|givemon|giveegg|givedecoration|adddecoration|pokemart|pokemartbuy|pokemartdecoration|pokemartdecoration2|random|giverandomberry)\b',command_text):
                sites[(rp.rel(info['file']),line)]=(label,line)
            special=re.match(r'special(?:var\s+\w+,)?\s+(\w+)',command_text)
            if special:native_sites.setdefault(special[1],[]).append((label,line))
    location_alternatives=[]
    for row in rows:
        if not enrich:
            row.pop('eligibility_paths',None)
            row['eligibility_status']='raw_not_certified'
            continue
        if 'data/scripts/cable_club.inc' in row['cite'] or row.get('campaign_scope')=='external multiplayer raw appendix':
            row['campaign_scope']='external multiplayer raw appendix'
            row['eligibility_status']='external multiplayer raw appendix'
            continue
        targets=[tuple(row['eligibility_target'])] if row.get('eligibility_target') else []
        for path,line in re.findall(r'(data/(?:maps/[^ ;:]+/scripts\.inc|scripts/[^ ;:]+\.inc|event_scripts\.s)):(\d+)',row['cite']):
            target=sites.get((path,int(line)))
            if target and target not in targets:targets.append(target)
        where=row['where'];map_name=where[0] if isinstance(where,tuple) else where
        handler={'vendor_kit':'GiveEmeraldChampionsStarterBattleItems',
            'vendor_badge_stock':'OpenEmeraldChampionsBattleItemMart',
            'vendor_species':'OpenEmeraldChampionsBattleItemMart',
            'vendor_equipment_restock':'OpenEmeraldChampionsBattleItemMart',
            'vendor_restock':'OpenEmeraldChampionsBattleItemMart',
            'vendor_form_items':'OpenEmeraldChampionsEvolutionItemArchive'}.get(row['kind'])
        handler=row.get('native_handler',handler)
        if row['kind']=='mart' and row['detail']=='Lilycove Dept. Store 4F evolution specialist':handler='OpenEmeraldChampionsEvolutionSpecialist'
        if map_name=='Route104_PrettyPetalFlowerShop' and ('flower' in row['detail']):handler='GiveFlowerShopBerryBundle'
        if row['kind']=='gift_held' and 'Cosmog' in row['detail']:handler='GiveBirchCosmogPair'
        if handler:
            targets.extend(native_sites.get(handler,[]))
        if not targets:
            row['eligibility_status']='native source conditions'
            continue
        paths=[]
        if str(map_name).startswith('BattleFrontier'):
            row['campaign_scope']='post-finale raw appendix'
            row['eligibility_status']='post-finale raw appendix'
            continue
        for target in targets:
            if target not in cache:cache[target]=parser.paths_to(*target)
            for path in cache[target]:
                event=path.get('event')
                if event and event[0]!=map_name:continue
                if event and isinstance(where,tuple) and tuple(event[1:3])!=tuple(where[1:3]):continue
                item_argument=row.get('delivery_item_argument')
                bound=path.get('assignments',{}).get(item_argument) if item_argument else None
                if bound and bound.startswith(('ITEM_','DECOR_')) and bound!=row['key']:continue
                paths.append(path)
        row['eligibility_paths']=paths
        row['eligibility_targets']=[dict(label=label,line=line) for label,line in targets]
        if paths:
            row['eligibility_status']='parsed script paths'
            row['transaction_conditions']=[dict(path_index=index,
                debits=[e for e in p.get('effects',[]) if e['op'] in ('removeitem','removemoney','removecoins') or (e['op']=='subvar' and e['args'][0]=='VAR_ASH_GATHER_COUNT')]) for index,p in enumerate(paths)]
        elif row.get('delivery_item_argument'):
            # Older variable-input discovery takes a union across branches.
            # Keep the rejected alternative inspectable without treating it as
            # an actual source at the incompatible item/decor delivery site.
            row['campaign_scope']='incompatible variable branch raw appendix'
            row['eligibility_status']='inactive source appendix'
        elif all(not cache.get(target) for target in targets):
            row['campaign_scope']='inactive script source raw appendix'
            row['eligibility_status']='inactive source appendix'
        else:
            row['eligibility_status']='unresolved'
        if handler and row['kind'].startswith('vendor_'):
            by_location={}
            for target in targets:
                for path in cache.get(target,[]):
                    event=path.get('event')
                    if not event or event[0]==map_name:continue
                    location=tuple(event[:3]) if event[1] is not None else event[0]
                    by_location.setdefault(location,[]).append(path)
            for location,alternatives in by_location.items():
                alternate=dict(row,where=location,eligibility_paths=alternatives,
                    eligibility_status='parsed script paths',transaction_conditions=[])
                if str(location[0] if isinstance(location,tuple) else location).startswith('BattleFrontier'):
                    alternate['campaign_scope']='post-finale raw appendix'
                    alternate['eligibility_status']='post-finale raw appendix'
                location_alternatives.append(alternate)
    rows.extend(location_alternatives)
    eligibility_diagnostics=list({str(d):d for d in parser.diagnostics}.values())
    bounded_targets={tuple(d['target']) for d in eligibility_diagnostics if d.get('target') and 'bound exceeded' in d.get('reason','')}
    for row in rows:
        if row.get('campaign_scope'):
            if row.get('eligibility_paths'):
                row['appendix_path_count']=len(row['eligibility_paths'])
                row.pop('eligibility_paths',None)
            continue
        if any((target['label'],target['line']) in bounded_targets for target in row.get('eligibility_targets',[])):
            row['eligibility_status']='unresolved'
            row['eligibility_problem']='control-flow enumeration incomplete; retained paths are partial evidence'
        summaries={}
        for path in row.get('eligibility_paths',[]):
            for effect in path.get('effects',[]):
                if effect['op']=='optional_delivery_summary':summaries.setdefault(effect['args'][0],effect)
            # Keep the complete eligibility conjunction and battle prerequisites
            # while dropping execution-history copies unrelated to eligibility.
        if row.get('eligibility_paths'):
            row['eligibility_paths']=[{key:value for key,value in path.items()
                if key not in ('effects','assignments','wild_species','condition_tokens')} for path in row['eligibility_paths']]
        if summaries:row['factored_optional_calls']=list(summaries.values())
    # Stable identity and deduplication preserve multiple legitimate locations.
    seen=set();clean=[]
    for row in rows:
        sig=(row['key'],row['kind'],str(row['where']),str(row['requires']),row['cite'])
        if sig in seen:continue
        seen.add(sig);row['id']=f'item-source-{len(clean)+1:05d}';clean.append(row)
    source_coverage['unresolved_live_sources']=[r['id'] for r in clean if r.get('eligibility_status')=='unresolved' and not r.get('campaign_scope')]
    return dict(sources=clean,services=services,unresolved=unresolved,eligibility_diagnostics=eligibility_diagnostics,source_coverage=source_coverage,enriched=enrich,
        fully_certified=enrich and not source_coverage['unresolved_live_sources'] and not unresolved,
        activities=build_optional_activities(builder,clean),
        rules=[dict(rule='wild held items, theft and Pickup-generated items excluded',cite='user request'),
            dict(rule='paid stock unlocks from first legitimate acquisition, badges, or caught equipment species',cite='src/field_specials.c:485'),
            dict(rule='planting available berry seeds expands repeatable harvest supply; only harvested quantities fund harvest trades',cite='src/berry.c:PickBerryTree; src/mega_stone_rewards.c:39')])
