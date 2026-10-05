#!/usr/bin/env python3
"""Source control-flow extraction for encounter-level campaign manifests.

Conditions retain both sides of flags and comparisons. Native predicates are
named explicitly, never silently treated as true. This is deliberately separate
from reference_pool's positive-only upper-bound acquisition discovery.
"""
from __future__ import annotations

import re
from collections import defaultdict
from functools import lru_cache
from manifest_native_predicates import native_input_names, native_writes_result, native_preserves_persistent_state, native_output_names

INVERSE = {'eq':'ne', 'ne':'eq', 'lt':'ge', 'ge':'lt', 'le':'gt', 'gt':'le'}
EFFECTS = {'setflag', 'clearflag', 'setvar', 'copyvar', 'addvar', 'subvar',
           'settrainerflag', 'cleartrainerflag', 'giveitem', 'additem',
           'removeitem', 'givepokemon', 'givemon', 'giveegg','giveuniqueitem','giveitem_msg','finditem','addpcitem', 'setmetatile',
           'removeobject', 'addobject', 'setobjectxyperm','applymovement','setobjectmovementtype',
           'copyobjectxytoperm','removemoney','removecoins'}
BATTLE_SPECIALS = {'BattleSetup_StartLegendaryBattle', 'BattleSetup_StartGroudonKyogreBattle',
                  'BattleSetup_StartRayquazaBattle', 'StartWallyTutorialBattle',
                  'DoEmeraldChampionsBirchRescueBattle', 'StartEmeraldChampionsBirchRescue', 'StartRegiBattle', 'BattleSetup_StartLatiBattle'}


def command(line):
    parts=line.split(None,1)
    op=parts[0]; rest=parts[1] if len(parts)>1 else ''
    args=tuple(a.strip() for a in rest.split(',')) if rest else ()
    if ',' not in rest and op.endswith(('if_defeated','if_not_defeated','if_set','if_unset')):
        args=tuple(rest.split())
    return op,args


def is_battle(op, args):
    return op.startswith('trainerbattle') or op in ('multi_2_vs_2', 'dowildbattle') or (op == 'special' and args and args[0] in BATTLE_SPECIALS)


def _cite(scripts, label, line):
    path = scripts.labels[label]['file']
    try:
        import reference_pool as rp
        path = path.relative_to(rp.ROOT)
    except (ValueError, ImportError):
        pass
    return f'{path}:{line}'


def condition_dict(condition):
    key, op, value = condition
    kind = 'trainer' if key.startswith('DEFEATED:') else 'native' if key.startswith('SPECIAL:') else 'flag' if key.startswith('FLAG_') else 'var' if key.startswith('VAR_') else 'choice' if key == 'PLAYER_GENDER' else 'predicate'
    return {'kind':kind, 'key':key, 'op':op, 'value':value}


def _contradiction(conditions, condition):
    key, op, value = condition
    if (key, INVERSE.get(op), value) in conditions:return True
    for old_key,old_op,old_value in conditions:
        if old_key!=key:continue
        if old_op=='eq':
            result=_known_result((old_value,op,value))
            if result is False:return True
        if op=='eq':
            result=_known_result((value,old_op,old_value))
            if result is False:return True
        if key.startswith(('FLAG_','DEFEATED:','COMMAND:msgbox@','COMMAND:yesnobox@')):
            if not any(_known_result((str(candidate),old_op,old_value)) is not False
                       and _known_result((str(candidate),op,value)) is not False for candidate in (0,1)):
                return True
    return False


def _literal(env, value):
    return env.get(value, 'UNRESOLVED:'+value if value=='VAR_RESULT' else value)


def _condition(env, key, op, value):
    return (_literal(env, key), op, _literal(env, value))


@lru_cache(maxsize=None)
def _static_number(token):
    if token=='PLAYER_GENDER' or re.search(r'\b(?:VAR_|FLAG_|DEFEATED:|SPECIAL:|COMMAND:|CHECK)',str(token)):return None
    from manifest_world import constants, value
    return value(token,{'flags':set(),'vars':{},'defeated':set(),'gender':0},_script_constants())


@lru_cache(maxsize=1)
def _script_constants():
    from manifest_world import constants
    return constants()


def _known_result(condition):
    a, op, b = condition
    aliases = {'TRUE':'1', 'FALSE':'0', 'MALE':'0', 'FEMALE':'1'}
    av=_static_number(aliases.get(a,a));bv=_static_number(aliases.get(b,b))
    if av is None or bv is None:return None
    return {'eq':av == bv, 'ne':av != bv, 'lt':av < bv,
            'le':av <= bv, 'gt':av > bv, 'ge':av >= bv}[op]


def augment_script_index(scripts):
    """Common native event scripts live in data/event_scripts.s, outside Scripts' scan."""
    if getattr(scripts, '_manifest_common_loaded', False): return
    import reference_pool as rp
    path=rp.ROOT / 'data/event_scripts.s'
    label=None; added=[]
    for n,raw in enumerate(path.read_text().splitlines(),1):
        text=raw.split('@')[0].strip()
        m=re.match(r'^([A-Za-z_]\w*)::?(?:\s|$)',text)
        if m:
            label=m[1]
            if label not in scripts.labels:
                scripts.labels[label]={'file':path,'line':n,'body':[],'map':None}; added.append(label)
            continue
        if label and text and not text.startswith(('.', '#')):
            scripts.labels[label]['body'].append((n,text))
    for a,b in zip(added,added[1:]):
        scripts.next_label[a]=b
    for label in added:
        for _n,text in scripts.labels[label]['body']:
            op,args=command(text)
            if op in ('goto','call') or op.startswith(('goto_if','call_if')) or op=='case':
                dest=args[-1]
                if dest in scripts.labels: scripts.callers[dest].add(label)
    for label,info in scripts.labels.items():
        # Consecutive assembly labels share one address. The base index used
        # to omit their fall-through caller edge when the first body was empty,
        # hiding every Center clerk behind the General_Mart_Script alias.
        if not info['body']:
            following=scripts.next_label.get(label)
            if following in scripts.labels:scripts.callers[following].add(label)
        for _n,text in info['body']:
            op,args=command(text)
            if op=='map_script_2' and len(args)>=3 and args[-1] in scripts.labels:
                scripts.callers[args[-1]].add(label)
    scripts._manifest_common_loaded=True


class ProgressionParser:
    """Walk actual event roots with a return stack and symbolic variable values.

    `paths_to` is demand driven so unrelated map callbacks cannot bloat a battle
    query. Each path carries all script effects before its encounter; callers
    decide which are free events and which belong to prior victorious battles.
    """
    def __init__(self, scripts, max_steps=2000, max_paths=512):
        augment_script_index(scripts)
        self.scripts = scripts
        self.max_steps = max_steps
        self.max_paths = max_paths
        self.diagnostics = []
        self._target_reach = {}
        self._return_reach = None
        self._reverse_cfg = None
        self._cosmetic_summaries = self._verified_cosmetic_summaries()
        self._optional_deliveries = self._verified_optional_deliveries()
        self._map_local_aliases = {}
        self._entry_reach_cache = {}

    def reach_entries(self,label):
        """Complete reverse reachability, stopping when the frontier is empty."""
        if label not in self._entry_reach_cache:
            seen={label};queue=[label];events=[]
            while queue:
                current=queue.pop();events.extend(self.scripts.entries.get(current,[]))
                for caller in self.scripts.callers.get(current,()):
                    if caller not in seen:seen.add(caller);queue.append(caller)
            self._entry_reach_cache[label]=(events,seen)
        return self._entry_reach_cache[label]

    def object_hide_flag(self,label,args,event=None,env=None):
        """Native removeobject sets the existing template's flag id."""
        if not args:return None
        env=env or {}
        map_name=event[0] if event else self.scripts.labels[label].get('map')
        if map_name not in self.scripts.geo.maps:return None
        import reference_pool as rp
        if map_name not in self._map_local_aliases:
            path=rp.ROOT/f'data/maps/{map_name}/scripts.inc'
            text=path.read_text() if path.exists() else ''
            self._map_local_aliases[map_name]={name:value.strip() for name,value in re.findall(r'^\s*\.set\s+(\w+),\s*([^\n@]+)',text,re.M)}
        aliases=self._map_local_aliases[map_name]
        def number(token):
            for _ in range(20):
                value=env.get(token,aliases.get(token,token))
                if value==token:break
                token=value
            return _static_number(token)
        local=_literal(env,args[0])
        objects=self.scripts.geo.maps[map_name].get('object_events',[]) or []
        for index,obj in enumerate(objects,1):
            local_id=obj.get('local_id',index)
            last_talked=local=='VAR_LAST_TALKED' and event and event[3]=='object' and (obj.get('x'),obj.get('y'))==tuple(event[1:3])
            if not (str(local)==str(local_id) or last_talked or (number(local) is not None and number(local)==number(str(local_id)))):continue
            flag=obj.get('flag')
            if flag and flag not in ('0','FLAG_NONE',0):return str(flag)
        return None

    def project_transition_calls(self,root,event,conditions):
        """Factor source-verified map setup calls, retaining guard and order.

        These map callbacks return without battles or player choices. Applying
        their guarded subcalls in source order reaches the same fixed point as
        enumerating every unrelated flag combination in one giant path.
        """
        if root not in ('LittlerootTown_OnTransition','Route111_OnTransition'):return None
        body=self.scripts.labels[root]['body'];env={};cmp=None;paths=[];direct=[]
        for line,text in body:
            op,args=command(text)
            if op=='compare':cmp=_condition(env,args[0],'eq',args[1]);continue
            if op in ('setflag','clearflag','setvar'):
                direct.append({'op':op,'args':list(args),'source':_cite(self.scripts,root,line)})
                env[args[0]]=args[1] if op=='setvar' else 'TRUE' if op=='setflag' else 'FALSE'
                continue
            guard=None;helper=None
            if op=='call':helper=args[-1]
            elif op in ('call_if_set','call_if_unset'):
                helper=args[-1];guard=_condition(env,args[0],'eq','TRUE' if op.endswith('_set') else 'FALSE')
            elif op.startswith('call_if_') and cmp:
                helper=args[-1];guard=(cmp[0],op.removeprefix('call_if_'),cmp[2])
            elif op=='goto_if_not_defeated':
                helper=args[-1];guard=('DEFEATED:'+args[0],'eq','FALSE')
            elif op=='special':
                direct.append({'op':'native_call','args':list(args),'inputs':{},'source':_cite(self.scripts,root,line)})
                continue
            elif op=='end':continue
            else:return None
            if helper:
                subconditions=conditions+((guard,) if guard else ())
                for path in self.walk(helper,event=event,conditions=subconditions,stop_at_battle=True):
                    if not path.get('completed'):return None
                    path['factored_call']=helper;path['source_order']=line;paths.append(path)
        if direct:
            paths.insert(0,{'conditions':[list(c) for c in conditions],'effects':direct,'prior_battles':[],
                'assignments':env,'completed':True,'source_order':self.scripts.labels[root]['line']})
        return paths

    def project_ferry_destinations(self,root,event,keys):
        if root!='LilycoveCity_Harbor_EventScript_FerryAttendant':return None
        # Ticket-showing and boarding are independent optional transactions.
        # Stop at each actual persistent commit, then the player may cancel
        # the returning destination menu; no battle or further payment is
        # required to retain that commit and return control.
        paths=[]
        for label,info in self.scripts.labels.items():
            if info.get('map')!='LilycoveCity_Harbor':continue
            for line,text in info['body']:
                op,args=command(text)
                if op not in ('setflag','setvar') or not args or args[0] not in keys:continue
                if args[0]!='VAR_SS_TIDAL_STATE' and not args[0].startswith('FLAG_SHOWN_'):continue
                for path in self.paths_to(label,line):
                    if path.get('entry')!=root or tuple(path.get('event',()))!=tuple(event):continue
                    path=dict(path);path['effects']=list(path['effects'])+[{'op':op,'args':list(args),'source':_cite(self.scripts,label,line)}]
                    path['conditions']=[[c['key'],c['op'],c['value']] for c in path['conditions']]
                    assignments=dict(path.get('assignments',{}));assignments[args[0]]=args[1] if op=='setvar' else 'TRUE'
                    path.update(assignments=assignments,completed=True,transaction_commit=_cite(self.scripts,label,line),
                        completion_condition='continue boarding or cancel returning destination menu; committed ticket display remains saved')
                    paths.append(path)
        return paths

    def _verified_optional_deliveries(self):
        """Factor independent, retryable handoffs that always return.

        Other papers/tools are optional earlier calls, not prerequisites for
        the queried reward. Their possible writes remain in summary evidence;
        no item or receipt is granted by applying a summary. The queried call
        itself always keeps its full checks and exact delivery branch.
        """
        candidates=['EC_DeliverTravelDocuments','EventScript_PkmnCenterNurse_Itemfinder',
            'EventScript_PkmnCenterNurse_WoodsGift']
        candidates += [l for l in self.scripts.labels if l.startswith('EC_Deliver_')]
        out={}
        for start in candidates:
            if start not in self.scripts.labels:continue
            seen=set();queue=[start];effects=[];writes=set();valid=True
            while queue and valid:
                label=queue.pop()
                if label in seen:continue
                if label not in self.scripts.labels:valid=False;break
                seen.add(label);body=self.scripts.labels[label]['body']
                for line,text in body:
                    op,args=command(text)
                    if is_battle(op,args) or op in ('end','release_end','releaseall_end','waitstate_end'):
                        valid=False;break
                    if op in ('special','specialvar') and args[-1]!='CheckEmeraldChampionsHandoffItem':valid=False;break
                    if op in EFFECTS:
                        effects.append({'op':op,'args':list(args),'source':_cite(self.scripts,label,line)})
                        if op in ('setvar','copyvar','addvar','subvar'):writes.add(args[0])
                    if op in ('call','goto') or op.startswith(('call_if','goto_if')):
                        queue.append(args[-1])
                if body and command(body[-1][1])[0] not in ('return','goto'):
                    following=self.scripts.next_label.get(label)
                    if following:queue.append(following)
            if valid:
                out[start]={'labels':seen,'possible_effects':effects,'scratch_writes':sorted(writes|{'VAR_RESULT','VAR_0x8004'}),
                    'source':_cite(self.scripts,start,self.scripts.labels[start]['line'])}
        return out

    def _verified_cosmetic_summaries(self):
        """Source-verified contest heart permutation has deterministic cleanup.

        Its only lasting script writes are the nine local animation counters,
        reset to zero before returning. Actual contest winner/rank/ribbon checks
        are outside this routine and remain in the acquisition path.
        """
        start='ContestHall_EventScript_AudienceHeartEmotes'
        if start not in self.scripts.labels:return {}
        seen=set();queue=[start];valid=True
        while queue:
            label=queue.pop()
            if label in seen:continue
            if label not in self.scripts.labels:valid=False;break
            seen.add(label)
            for _n,text in self.scripts.labels[label]['body']:
                op,args=command(text)
                if op in ('setvar','addvar','subvar','copyvar'):
                    if not args or (not re.match(r'VAR_TEMP_[0-8]$',args[0]) and args[0]!='VAR_RESULT'):valid=False
                elif op == 'applymovement':
                    # The general effect inventory includes movement, but this
                    # exact emote does not move an object or unlock a path.
                    movement = self.scripts.labels.get('ContestHall_Movement_Heart', {}).get('body', [])
                    if (len(args) != 2 or args[1] != 'ContestHall_Movement_Heart'
                            or [text for _n, text in movement] != ['emote_heart', 'step_end']):
                        valid = False
                elif op in EFFECTS or is_battle(op,args):valid=False
                if op=='special' and args[0] not in ('GetContestMonCondition','GenerateContestRand'):valid=False
                if op in ('call','goto') or op.startswith(('call_if','goto_if')):
                    if args:queue.append(args[-1])
        cleanup=[a for _n,t in self.scripts.labels[start]['body'] for op,a in [command(t)] if op=='setvar' and a and a[0].startswith('VAR_TEMP_') and a[-1]=='0']
        if not valid or len(cleanup)!=8:return {}
        summaries={start:dict(labels=seen,writes={f'VAR_TEMP_{i}':'0' for i in range(9)},
                           source=_cite(self.scripts,start,self.scripts.labels[start]['line']))}
        # Showing the four entrants only animates their pictures/audience. The
        # actual appeals, winner checks and prize branch are separate calls.
        start='ContestHall_EventScript_ShowContestMons'
        if start not in self.scripts.labels:return summaries
        seen=set();queue=[start];valid=True
        allowed_specials={'GetContestMonCondition','GenerateContestRand','IsWirelessContest',
            'LinkContestWaitForConnection','HideContestEntryMonPic','BufferContestTrainerAndMonNames',
            'ShowContestEntryMonPic','IsContestWithRSPlayer'}
        while queue and valid:
            label=queue.pop()
            if label in seen:continue
            if label not in self.scripts.labels:valid=False;break
            seen.add(label);body=self.scripts.labels[label]['body']
            for _n,text in body:
                op,args=command(text)
                if op in ('setvar','addvar','subvar','copyvar'):
                    if not re.match(r'(?:VAR_TEMP_[0-8]|VAR_0x800[0-9A-F]|VAR_RESULT)$',args[0]):valid=False
                elif op in ('addobject','removeobject'):
                    if args!=('LOCALID_POKEBALL',):valid=False
                elif op in EFFECTS or is_battle(op,args) or op in ('end','waitstate_end'):valid=False
                if op in ('special','specialvar') and args[-1] not in allowed_specials:valid=False
                if op in ('call','goto') or op.startswith(('call_if','goto_if')):queue.append(args[-1])
            if body and command(body[-1][1])[0] not in ('return','goto','end'):
                following=self.scripts.next_label.get(label)
                if following:queue.append(following)
        body=[text for _,text in self.scripts.labels[start]['body']]
        if valid and 'compare VAR_0x8006, CONTESTANT_COUNT' in body and 'setvar VAR_TEMP_1, 6' in body:
            summaries[start]=dict(labels=seen,writes={**{f'VAR_TEMP_{i}':'0' for i in range(9)},
                'VAR_TEMP_1':'6','VAR_0x8006':str(_static_number('CONTESTANT_COUNT')),
                'VAR_RESULT':'COSMETIC:contest entrant presentation'},
                source=_cite(self.scripts,start,self.scripts.labels[start]['line']))
        return summaries

    def _is_service_menu(self,label):
        return any(op in ('multichoice','multichoicedefault','multichoicegrid','dynmultistack')
                   or (op=='msgbox' and args and args[-1]=='MSGBOX_YESNO')
                   or (op=='special' and args and args[0] in ('ShowScrollableMultichoice','ShowEasyChatScreen'))
                   for op,args in (command(t) for _n,t in self.scripts.labels[label]['body']))

    @staticmethod
    def _persistent_fingerprint(env,effects,battles):
        # Scratch results/menu choices do not persist. Persistent vars, actual
        # inventory/party deltas and potentially mutating native calls do.
        saved=tuple(sorted((k,v) for k,v in env.items() if k.startswith(('FLAG_','DEFEATED:','VAR_'))
                           and not k.startswith('VAR_0x800') and k not in ('VAR_RESULT','VAR_FACING','VAR_LAST_TALKED')))
        deltas=defaultdict(int);tiles={}
        for effect in effects:
            op=effect['op'];args=tuple(effect['args'])
            if op in ('giveitem','giveuniqueitem','giveitem_msg','finditem','additem','addpcitem','removeitem','giveegg','givemon','givepokemon'):
                deltas[(op,args)]+=1
            elif op=='native_call' and not native_preserves_persistent_state(args[0]):
                deltas[(op,args,tuple(sorted(effect.get('inputs',{}).items())))]+=1
            elif op in ('setmetatile','setobjectxyperm','removeobject','addobject'):
                tiles[(op,args[:2])]=args[2:]
        return saved,tuple(sorted(deltas.items())),tuple(sorted(tiles.items())),battles

    def _cfg_reach(self, target):
        """Reverse executable CFG, with conservative call continuation edges.

        A subroutine may still lead to the target through its return stack;
        `_return_reach` ensures those helpers are retained. Terminal irrelevant
        menu branches cannot return and are discarded before symbolic walking.
        """
        if self._reverse_cfg is None:
            reverse=defaultdict(set); returns=[]
            for label,info in self.scripts.labels.items():
                body=info['body']
                next_label=self.scripts.next_label.get(label)
                if next_label in self.scripts.labels:reverse[(next_label,0)].add((label,len(body)))
                for index,(_line,text) in enumerate(body):
                    op,args=command(text); here=(label,index)
                    nxt=(label,index+1) if index+1<len(body) else (self.scripts.next_label.get(label),0)
                    edges=[]
                    if op=='return':returns.append(here)
                    elif op in ('end','releaseall_end','release_end','waitstate_end') or (op=='.2byte' and args==('0',)):pass
                    elif op=='goto' and args:edges=[(args[0],0)]
                    elif op=='call' and args:edges=[(args[0],0),nxt]
                    elif op.startswith(('goto_if','call_if')) or op in ('case','map_script_2'):
                        if args:edges=[(args[-1],0),nxt]
                    else:
                        edges=[nxt]
                        if op=='trainerbattle_double' and len(args)>4:edges.append((args[4],0))
                        elif op=='trainerbattle_single' and len(args)>3:edges.append((args[3],0))
                        elif op=='trainerbattle_lavaridge_double' and len(args)>5:edges.append((args[5],0))
                    for edge in edges:
                        if edge[0] in self.scripts.labels:reverse[edge].add(here)
            self._reverse_cfg=reverse
            self._return_reach=self._reverse_closure(returns)
        target_node=next(((target[0],i) for i,(n,_t) in enumerate(self.scripts.labels[target[0]]['body']) if n==target[1]),None)
        if target_node not in self._target_reach:
            self._target_reach[target_node]=self._reverse_closure([target_node] if target_node else [])
        return self._target_reach[target_node]

    def _reverse_closure(self,seeds):
        seen=set(seeds);queue=list(seeds)
        while queue:
            for previous in self._reverse_cfg.get(queue.pop(),()):
                if previous not in seen:seen.add(previous);queue.append(previous)
        return seen

    def paths_to(self, target, target_line):
        entries, ancestors = self.reach_entries(target)
        roots = [(label, event) for label in ancestors for event in self.scripts.entries.get(label, [])]
        out = []
        for root, event in roots:
            conditions = ()
            if event[3] == 'object' and event[4] not in (None, '0', 'FLAG_NONE'):
                conditions = ((event[4], 'eq', 'FALSE'),)
            if event[3] == 'coord':
                for coord in self.scripts.geo.maps[event[0]].get('coord_events', []) or []:
                    if coord.get('script') == root and coord.get('x') == event[1] and coord.get('y') == event[2]:
                        if coord.get('var') not in (None, '0'):
                            conditions += ((str(coord['var']), 'eq', str(coord.get('var_value', 0))),)
            for raw in self.scripts.labels[root]['file'].read_text().splitlines():
                m = re.search(r'map_script_2\s+(\w+),\s*(\w+),\s*'+re.escape(root)+r'\b', raw)
                if m and event[3] == 'map': conditions += ((m[1], 'eq', m[2]),)
            out.extend(self.walk(root, event, target=(target, target_line), conditions=conditions))
        unique = {}
        for p in out:
            key = (p['entry'], tuple(p['event']), tuple(tuple(c) for c in p['conditions']), tuple(p['prior_battles']))
            unique.setdefault(key, p)
        result=list(unique.values())
        for row in result:
            row['condition_tokens']=row['conditions']
            row['conditions']=[condition_dict(c) for c in row['conditions']]
        return result

    def walk(self, root, event=None, target=None, conditions=(), start_index=0, stop_at_battle=False):
        # label, index, return addresses, conditions, assignments, effects, prior battles,
        # previous comparison, switch operand, configured wild species, steps.
        relevant=self._cfg_reach(target) if target else None
        queue = [(root, start_index, (), conditions, {}, (), (), None, None, None, 0, frozenset())]
        seen = set(); output = []
        while queue:
            label, index, stack, conds, env, effects, battles, cmp, switch, wild, steps, service_trail = queue.pop()
            if relevant is not None and (label,index) not in relevant:
                if (label,index) not in self._return_reach or not any(frame in relevant for frame in stack):
                    continue
            if len(output) >= self.max_paths:
                self.diagnostics.append({'entry':root, 'target':target, 'reason':'path enumeration bound exceeded'})
                break
            if steps > self.max_steps:
                self.diagnostics.append({'entry':root, 'target':target, 'reason':'control-flow bound exceeded'})
                continue
            if label not in self.scripts.labels:
                self.diagnostics.append({'entry':root, 'label':label, 'reason':'unresolved script label'})
                continue
            signature = (label, index, stack, conds, tuple(sorted(env.items())), battles)
            if signature in seen:
                continue
            seen.add(signature)
            body = self.scripts.labels[label]['body']
            summary=self._cosmetic_summaries.get(label) if index==0 else None
            if summary and stack and (target is None or target[0] not in summary['labels']):
                # Do not enumerate cosmetic audience permutations. All actual
                # completion writes are retained; no reward/winner is granted.
                env=dict(env);env.update(summary['writes'])
                effects=effects+({'op':'cosmetic_summary','args':[label],'source':summary['source'],'writes':summary['writes']},)
                dest,ix=stack[-1]
                queue.append((dest,ix,stack[:-1],conds,env,effects,battles,cmp,switch,wild,steps+1,service_trail))
                continue
            if index==0 and self._is_service_menu(label):
                fingerprint=self._persistent_fingerprint(env,effects,battles)
                cycle=(label,stack,fingerprint)
                if cycle in service_trail:continue
                service_trail=service_trail|{cycle}
            if index >= len(body):
                nxt = self.scripts.next_label.get(label)
                if nxt:
                    queue.append((nxt, 0, stack, conds, env, effects, battles, cmp, switch, wild, steps+1, service_trail))
                continue
            line, text = body[index]; op, args = command(text)
            def push(dest=label, ix=index+1, cs=conds, es=env, fs=effects, bs=battles, st=stack, cp=cmp, sw=switch, wi=wild):
                queue.append((dest, ix, st, cs, es, fs, bs, cp, sw, wi, steps+1, service_trail))
            if target == (label, line):
                output.append({'entry':root, 'event':event, 'conditions':[list(c) for c in conds],
                               'effects':list(effects), 'prior_battles':list(battles),
                               'assignments':env, 'wild_species':wild})
                continue
            optional=self._optional_deliveries.get(args[-1]) if args and (op=='call' or op.startswith('call_if')) else None
            if optional and target and target[0] not in optional['labels']:
                env=dict(env)
                for key in optional['scratch_writes']:env[key]=f'OPTIONAL_CALL:{args[-1]}:{key}'
                effect={'op':'optional_delivery_summary','args':[args[-1]],'source':optional['source'],
                    'activation_command':text,'possible_effects':optional['possible_effects']}
                push(es=env,fs=effects+(effect,));continue
            if op in ('end', 'releaseall_end', 'release_end', 'waitstate_end') or (op=='.2byte' and args==('0',)):
                if target is None:
                    output.append({'conditions':[list(c) for c in conds], 'effects':list(effects), 'prior_battles':list(battles), 'completed':True, 'assignments':dict(env)})
                continue
            if op == 'return':
                if stack:
                    dest, ix = stack[-1]; push(dest, ix, st=stack[:-1])
                elif target is None:
                    output.append({'conditions':[list(c) for c in conds], 'effects':list(effects), 'prior_battles':list(battles), 'completed':True, 'assignments':dict(env)})
                continue
            if op in ('goto', 'call') and args:
                push(args[0], 0, st=stack+((label,index+1),) if op == 'call' else stack); continue
            if op == 'compare':
                push(cp=_condition(env, args[0], 'eq', args[1])); continue
            if op == 'switch':
                push(sw=_literal(env,args[0])); continue
            match = re.match(r'(goto|call)_if_(set|unset|defeated|not_defeated|eq|ne|lt|le|gt|ge)$',op)
            if match or op in ('case','map_script_2'):
                if op=='map_script_2':
                    mode='goto'; test=_condition(env,args[0],'eq',args[1]); dest=args[-1]
                elif op == 'case':
                    mode='goto'; test=(switch or 'UNRESOLVED_SWITCH', 'eq', args[0]); dest=args[-1]
                else:
                    mode, kind=match.groups(); dest=args[-1]
                    if kind in ('set','unset','defeated','not_defeated'):
                        key=('DEFEATED:'+args[0]) if 'defeated' in kind else args[0]
                        test=_condition(env,key,'eq','TRUE' if kind in ('set','defeated') else 'FALSE')
                    elif len(args) >= 3:
                        test=_condition(env,args[0],kind,args[1])
                    else:
                        test=(cmp[0],kind,cmp[2]) if cmp else ('UNRESOLVED_COMPARISON',kind,'0')
                reverse=(test[0],INVERSE[test[1]],test[2]); known=_known_result(test)
                if known is not False and not _contradiction(conds,test):
                    push(dest,0,cs=conds if known else conds if test in conds else conds+(test,),st=stack+((label,index+1),) if mode=='call' else stack)
                if known is not True and not _contradiction(conds,reverse):
                    push(cs=conds if known is False else conds if reverse in conds else conds+(reverse,))
                continue
            if op=='random' and args:
                env=dict(env); env['VAR_RESULT']='RANDOM:'+_literal(env,args[0])+'@'+_cite(self.scripts,label,line)
            if op=='dynmultipush' and len(args)>1:
                env=dict(env); env['__menu_values']=env.get('__menu_values','')+'|'+_literal(env,args[1])
            if op=='dynmultistack':
                env=dict(env); values=env.pop('__menu_values','')
                env['VAR_RESULT']='DYN_MULT:'+values.lstrip('|')+'@'+_cite(self.scripts,label,line)
            if op in ('giveuniqueitem','giveitem_msg','finditem'):
                env=dict(env); env['VAR_RESULT']='COMMAND:'+op+'@'+_cite(self.scripts,label,line)
            if op in ('multichoice','multichoicedefault','multichoicegrid','yesnobox','dynmultichoice','giveitem','additem','givemon','givepokemon','giveegg','callstd') or (op=='msgbox' and args and args[-1]=='MSGBOX_YESNO'):
                env=dict(env); env['VAR_RESULT']='COMMAND:'+op+'@'+_cite(self.scripts,label,line)
            if op=='checkcoins' and args:
                env=dict(env);env[args[0]]='CHECKCOINS:'+args[0]
            if op in ('checkflag','checktrainerflag','checkplayergender','checkitem','checkitemspace','checkmoney'):
                env=dict(env)
                if op == 'checkflag': cmp=_condition(env,args[0],'eq','TRUE')
                elif op == 'checktrainerflag': cmp=_condition(env,'DEFEATED:'+args[0],'eq','TRUE')
                elif op == 'checkplayergender': env['VAR_RESULT']='PLAYER_GENDER'
                else: env['VAR_RESULT']=op.upper()+':'+','.join(_literal(env,a) for a in args)
            if op in ('specialvar','special','callnative'):
                env=dict(env); name=args[-1] if op == 'specialvar' else args[0]
                result=args[0] if op == 'specialvar' else 'VAR_RESULT'
                input_names=native_input_names(name)
                native_inputs={k:v for k,v in env.items() if k in input_names} if input_names is not None else {k:v for k,v in env.items() if k.startswith('VAR_0x800')}
                effects=effects+({'op':'native_call','args':[name], 'inputs':native_inputs, 'source':_cite(self.scripts,label,line)},)
                # Native UI callbacks and gift helpers also write scratch
                # outputs. Keeping their old input literals can falsely prune
                # legal branches (Easy Chat type 13 becomes a phrase id 0..5).
                for scratch_output in native_output_names(name) or ():
                    if scratch_output=='VAR_RESULT':continue
                    arguments=','.join(f'{k}={v}' for k,v in sorted(native_inputs.items()))
                    env[scratch_output]='SPECIAL:NativeScratchOutput(function='+name+',variable='+scratch_output+(','+arguments if arguments else '')+')'
                if name=='BuildEmeraldChampionsHarvestChoices':
                    env['__menu_values']=env.get('__menu_values','')+'|HARVEST_BERRIES'
                if op=='specialvar' or native_writes_result(name) is not False:
                    env[result]='SPECIAL:'+name+'('+','.join(f'{k}={v}' for k,v in sorted(native_inputs.items()))+')'
                if name=='BufferQuizPrizeItem':env['VAR_0x8005']='SPECIAL:QuizPrizeItem()'
                elif name=='GetContestWinnerId':env['VAR_0x8005']='SPECIAL:ContestWinnerId()'
                elif name=='GetContestPlayerId':env['VAR_0x8004']='SPECIAL:ContestPlayerId()'
                elif name.startswith('FoundAbandonedShipRoom'):
                    room=re.search(r'Room([1246])Key$',name)
                    if room:env['VAR_0x8004']='FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_'+room[1]+'_KEY'
            if op in EFFECTS:
                effect={'op':op, 'args':list(args), 'source':_cite(self.scripts,label,line)}
                effects=effects+(effect,)
                env=dict(env)
                if op in ('setvar','copyvar'): env[args[0]]=_literal(env,args[1])
                elif op in ('setflag','clearflag'): env[args[0]]='TRUE' if op=='setflag' else 'FALSE'
                elif op in ('settrainerflag','cleartrainerflag'): env['DEFEATED:'+args[0]]='TRUE' if op=='settrainerflag' else 'FALSE'
                elif op in ('addvar','subvar'):
                    previous=env.get(args[0],args[0])
                    try: env[args[0]]=str(int(previous,0)+(1 if op=='addvar' else -1)*int(args[1],0))
                    except ValueError: env[args[0]]=f'({previous}{"+" if op=="addvar" else "-"}{args[1]})'
            if op=='removeobject':
                flag=self.object_hide_flag(label,args,event,env)
                if flag:
                    effects=effects+({'op':'setflag','args':[flag], 'source':_cite(self.scripts,label,line),
                        'implicit_from':'removeobject','native_source':'src/event_object_movement.c:1652'},)
                    env=dict(env);env[flag]='TRUE'
            if op == 'setwildbattle': wild=_literal(env,args[0])
            if is_battle(op,args):
                if stop_at_battle:
                    output.append({'conditions':[list(c) for c in conds], 'effects':list(effects), 'next_battle_source':_cite(self.scripts,label,line), 'prior_battles':list(battles), 'completed':False})
                    continue
                trainers=[a for a in args if a.startswith('TRAINER_') and a!='TRAINER_NONE']
                battle_id='|'.join(trainers) or f'{label}:{line}'
                # Encounter prerequisites are idempotent. Failed scene retries do
                # not create a new campaign victory or acquisition dependency.
                battles=battles if battle_id in battles else battles+(battle_id,)
                env=dict(env)
                for trainer in trainers: env['DEFEATED:'+trainer]='TRUE'
                callbacks=[]
                if op=='trainerbattle_double' and len(args)>4: callbacks=[args[4]]
                elif op=='trainerbattle_single' and len(args)>3: callbacks=[args[3]]
                elif op=='trainerbattle_lavaridge_double' and len(args)>5: callbacks=[args[5]]
                elif op=='trainerbattle_lavaridge' and len(args)>4: callbacks=[args[4]]
                for callback in callbacks:
                    if callback in self.scripts.labels: push(callback,0,es=env,fs=effects,bs=battles)
                if callbacks: continue
            push(es=env,fs=effects,bs=battles,wi=wild,cp=cmp)
        return output


def extract_battle_nodes(scripts, trainer_ids=None):
    parser=ProgressionParser(scripts)
    nodes=[]
    for label, info in scripts.labels.items():
        for index,(line,text) in enumerate(info['body']):
            op,args=command(text)
            if not is_battle(op,args): continue
            trainers=[a for a in args if a.startswith('TRAINER_') and a!='TRAINER_NONE']
            if trainer_ids is not None and trainers and not any(t in trainer_ids for t in trainers): continue
            paths=parser.paths_to(label,line)
            continuations=[(label,index+1)]
            if op=='trainerbattle_double' and len(args)>4: continuations=[(args[4],0)]
            elif op=='trainerbattle_single' and len(args)>3: continuations=[(args[3],0)]
            elif op=='trainerbattle_lavaridge_double' and len(args)>5: continuations=[(args[5],0)]
            aftermath=[]
            for dest,ix in continuations:
                if dest in scripts.labels:
                    aftermath.extend(parser.walk(dest,start_index=ix,stop_at_battle=True))
            for row in aftermath:
                row['condition_tokens']=row['conditions']
                row['conditions']=[condition_dict(c) for c in row['conditions']]
            locations=[{'map':p['event'][0], 'x':p['event'][1], 'y':p['event'][2], 'trigger':p['event'][3], 'entry':p['entry']} for p in paths]
            nodes.append({'locations':locations, 'forced_trigger':any(p['event'][3] in ('coord','map') for p in paths), 'id':'|'.join(trainers) or f'{label}:{line}', 'trainers':trainers,
                          'kind':'multi' if op=='multi_2_vs_2' else 'trainer' if trainers else 'scripted_wild',
                          'site_id':f'{label}:{line}', 'partner':args[-1] if op=='multi_2_vs_2' else None,
                          'wild_species':sorted({p['wild_species'] or p['assignments'].get('VAR_0x8004','') for p in paths} - {''}) if not trainers else [],
                          'command':op,'args':list(args),'label':label,'source':_cite(scripts,label,line),
                          'paths':paths,'victory_paths':aftermath,
                          'unresolved':not bool(paths)})
    diagnostics=list({str(d):d for d in parser.diagnostics}.values())
    return {'battles':nodes,'diagnostics':diagnostics, 'native_predicates':NATIVE_PREDICATES}

# These are translations of current native functions, not storyline estimates.
# Unknown SPECIAL expressions remain unsupported predicates in exported paths.
NATIVE_PREDICATES = {
    'CheckEmeraldChampionsHandoffItem': {
        'meaning':'Bag or PC contains at least one VAR_0x8004 item',
        'source':'src/field_specials.c:663', 'inputs':['VAR_0x8004'],
        'kind':'inventory_any_store'},
    'IsPlayerPartyBelowLevelCap': {
        'meaning':'Any occupied non-Egg party member is below its current species level cap',
        'source':'src/field_specials.c:5604', 'kind':'party_below_cap'},
    'BufferEmeraldChampionsRivalBranch': {
        'meaning':'(GetEmeraldChampionsRivalStarterIndex() + 2) % 3; paired save rival is 3-first-second',
        'source':'src/emerald_champions_story.c:71',
        'inputs':['VAR_STARTER_MON','VAR_EC_SECOND_STARTER','VAR_EC_OPENING_STATE'], 'kind':'rival_starter'},
    'GetBattleOutcome': {
        'meaning':'Native battle outcome: won, lost, caught, ran or other outcomes remain separate branches',
        'source':'src/field_specials.c:1553', 'kind':'battle_outcome'},
}


def extract_story_events(scripts, keys=None):
    """Individual source mutations with path prerequisites and preceding fights.

    This export does not execute events or grant their effects. A consumer must
    establish event root reachability and satisfy its conditions first. `keys`
    allows a focused export of geometry/story tokens rather than every local var.
    """
    parser=ProgressionParser(scripts)
    events=[]
    for label,info in scripts.labels.items():
        if info['file'].name in ('new_game.inc','debug.inc'): continue
        for line,text in info['body']:
            op,args=command(text)
            if op not in EFFECTS or not args: continue
            if keys is not None and args[0] not in keys: continue
            paths=parser.paths_to(label,line)
            if not paths: continue
            events.append({'id':f'{label}:{line}', 'op':op, 'args':list(args),
                           'source':_cite(scripts,label,line), 'paths':paths,
                           'battle_dependent':any(p['prior_battles'] for p in paths)})
    return {'events':events, 'diagnostics':list({str(d):d for d in parser.diagnostics}.values()), 'native_predicates':NATIVE_PREDICATES}


def extract_completed_event_transitions(scripts, keys=None, maps=None, progress=None):
    """Return whole field events that return control without crossing a battle.

    Partial paths arriving at a battle are separately exported as blocked paths.
    This prevents a temporary setvar before an encounter from granting passage.
    Only completed paths' final assignments are valid field-state updates.
    """
    parser=ProgressionParser(scripts)
    roots=set()
    for label,info in scripts.labels.items():
        if info['file'].name in ('new_game.inc','debug.inc'): continue
        writes=[args[0] for op,args in (command(t) for _,t in info['body']) if op in EFFECTS and args and (keys is None or args[0] in keys)]
        scratch_only=bool(writes) and all(key.startswith(('VAR_TEMP_','VAR_0x8')) or key in ('VAR_RESULT','VAR_FACING','VAR_LAST_TALKED') for key in writes)
        relevant=keys is None or bool(writes)
        if not relevant and keys is not None:
            for _line,text in info['body']:
                op,args=command(text)
                if op!='removeobject':continue
                if info.get('map') and not args[0].startswith('VAR_'):
                    event=(info['map'],None,None,'map',None)
                    if parser.object_hide_flag(label,args,event) not in keys:continue
                events,_=parser.reach_entries(label)
                if any(parser.object_hide_flag(label,args,event) in keys for event in events):
                    relevant=True;break
        if relevant:
            _events,ancestors=parser.reach_entries(label)
            roots.update(a for a in ancestors if a in scripts.entries and
                (not scratch_only or any(event[3]=='map' for event in scripts.entries[a])))
    transitions=[]; blocked=[];factored_activities=[]
    for root in sorted(roots):
        for event in scripts.entries[root]:
            if maps is not None and event[0] not in maps:continue
            if progress:progress(root,event,'start')
            conditions=()
            if event[3]=='object' and event[4] not in (None,'0','FLAG_NONE'):
                conditions=((event[4],'eq','FALSE'),)
            if event[3]=='coord':
                for coord in scripts.geo.maps[event[0]].get('coord_events',[]) or []:
                    if coord.get('script')==root and coord.get('x')==event[1] and coord.get('y')==event[2]:
                        if coord.get('var') not in (None,'0'):
                            conditions+=((str(coord['var']),'eq',str(coord.get('var_value',0))),)
            if event[3]=='map':
                for raw in scripts.labels[root]['file'].read_text().splitlines():
                    m=re.search(r'map_script_2\s+(\w+),\s*(\w+),\s*'+re.escape(root)+r'\b',raw)
                    if m:conditions+=((m[1],'eq',m[2]),)
            body=[command(text) for _n,text in scripts.labels[root]['body']]
            nurse=any(op=='call' and args==('Common_EventScript_PkmnCenterNurse',) for op,args in body)
            transparent_wrapper=all(op in ('setvar','call','waitmessage','waitbuttonpress','release','end')
                and (op!='call' or args==('Common_EventScript_PkmnCenterNurse',))
                and (op!='setvar' or args[0].startswith('VAR_0x800')) for op,args in body)
            projected_ferry=parser.project_ferry_destinations(root,event,keys) if keys is not None else None
            projected_map=parser.project_transition_calls(root,event,conditions) if keys is not None else None
            if projected_ferry is not None:
                paths=projected_ferry
            elif projected_map is not None:
                paths=projected_map
            elif nurse and transparent_wrapper and keys is not None:
                # Independent, always-returning travel-document calls do not
                # require a Cartesian product of the other papers, tools and
                # healing choices. Each projection retains its actual source
                # entitlement and the helper's success/failure receipt paths.
                paths=[]
                for _line,text in scripts.labels['EC_DeliverTravelDocuments']['body']:
                    op,args=command(text)
                    if op!='call_if_set' or args[-1] not in parser._optional_deliveries:continue
                    helper=args[-1];summary=parser._optional_deliveries[helper]
                    if not any(e['op'] in ('setflag','clearflag') and e['args'][0] in keys for e in summary['possible_effects']):continue
                    helper_conditions=conditions+((args[0],'eq','TRUE'),)
                    for path in parser.walk(helper,event=event,conditions=helper_conditions,stop_at_battle=True):
                        path['factored_call']=helper;paths.append(path)
                factored_activities.append({'entry':root,'event':event,'source':_cite(scripts,root,scripts.labels[root]['line']),
                    'scope':'Nurse tool/heal/relic services retained in acquisition/activity sources; story receipt helpers projected independently'})
            else:
                paths=parser.walk(root,event=event,conditions=conditions,stop_at_battle=True)
                if any(not path.get('completed') for path in paths):
                    # An encounter's callback is not necessarily the caller's
                    # end. Continue the original return stack through all
                    # battles to retain later caller writes/removals. Every
                    # such completed path keeps exact required battle ids;
                    # consumers must prove them before applying any effects.
                    paths.extend(path for path in parser.walk(root,event=event,conditions=conditions,stop_at_battle=False)
                                 if path.get('completed') and path.get('prior_battles'))
            if progress:progress(root,event,len(paths))
            for i,path in enumerate(paths):
                path['id']=f'{root}:{event[0]}:{event[1]}:{event[2]}:{i}'
                path['entry']=root; path['event']=event
                path['source']=_cite(scripts,root,scripts.labels[root]['line'])
                path['condition_tokens']=path['conditions']
                path['conditions']=[condition_dict(c) for c in path['conditions']]
                final={}
                for effect in path['effects']:
                    op=effect['op'];args=effect['args']
                    if op in ('setflag','clearflag'):final[args[0]]=op=='setflag'
                    elif op in ('setvar','copyvar','addvar','subvar'):final[args[0]]=path.get('assignments',{}).get(args[0],args[1])
                path['final_writes']=final
                if path.get('completed'):
                    if keys is None or any(k in keys for k in final):transitions.append(path)
                else:blocked.append(path)
    return {'transitions':transitions, 'blocked_at_battle':blocked,'factored_activities':factored_activities,
            'diagnostics':list({str(d):d for d in parser.diagnostics}.values()),
            'native_predicates':NATIVE_PREDICATES}
