#!/usr/bin/env python3
"""Native predicate domains for source-driven theoretical campaign preparation.

None means missing state or unsupported semantics. No unsupported predicate is
silently accepted. `preparation_possible` opts into existential legal player
choices (free storage, reorder a party, apply the owned Leveler); it never grants
an item, a species, a victory, or a legendary discovery flag.
"""
from __future__ import annotations
import re
import ast
from functools import lru_cache
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]

CITATIONS = {
 'RecordOwnedBloodmoon':('src/legendary_signs.c:RecordOwnedBloodmoon','remember raw Bloodmoon form already in the party or PC during the rule lesson'),
 'QuizPrizeItem':('src/lilycove_lady.c:466','native scratch prize item output'),
 'TryEnterContestMon':('src/contest.c:3052','Egg, fainted and selected category ribbon rank eligibility'),
 'HasMonWonThisContestBefore':('src/contest_util.c:1915','selected category ribbon strictly exceeds entered rank'),
 'ContestWinnerId':('src/contest_util.c:1973','recorded zero-standing contestant index'),
 'ContestPlayerId':('src/contest_util.c:2037','actual contest player index'),
 'Script_FavorLadyOpenBagMenu':('src/item_menu.c:2797','selected owned item is debited before callback success'),
 'Script_QuizLadyOpenBagMenu':('src/item_menu.c:2813','selected quiz prize item confirmed without debit'),
 'IsEnoughForCostInVar0x8005':('src/money.c:120','current money compared to authored scratch cost'),
 'SubtractMoneyFromVar0x8005':('src/money.c:125','debit authored scratch cost with zero floor'),
 'COMMAND:checkmoney':('src/scrcmd.c:2339','money affordability boolean'),
 'CHECKCOINS':('src/scrcmd.c:2848','current coin balance written to output variable'),
 'NativeScratchOutput':('src/easy_chat.c:2920','native output variable domain; unsupported outputs stay unresolved'),
 'GetLeadMonFriendshipScore':('src/pokemon.c:6698','lead friendship threshold score; legal walking/bonding preparation'),
 'GiveEmeraldChampionsStarterBattleItems':('src/field_specials.c:599','atomic six-item kit and saved receipt'),
 'PlayerPartyLeagueEligible':('src/pokemon.c:3144','at most one restricted-class member; the player arranges the party'),
 'IsItemFossil':('src/field_specials.c','VAR_ITEM_ID is a revivable fossil'),
 'DoesPlayerHaveFossil':('src/field_specials.c:5476','any revivable fossil in the Bag'),
 'StartInitialToolsBagTutorial':('src/item_menu.c:2595','asynchronous completed demonstration; original bag restored'),
 'ExchangeSootForCaps':('src/inclement_stat_services.c:678','500 gathered ash per cap; delivery before debit'),
 'GetTraderTradedFlag':('src/trader.c:139','saved decoration trade receipt'),
 'DoesPlayerHaveNoDecorations':('src/trader.c:146','owned category inventory query'),
 'IsDecorationCategoryFull':('src/trader.c:163','same-category trade preserves capacity'),
 'GetCurrentMauvilleOldMan':('src/mauville_old_man.c:140','trainer-id-selected native character'),
 'Script_GetCurrentMauvilleMan':('src/mauville_old_man.c:145','trainer-id-selected native character'),
 'BufferEmeraldChampionsBattleItemStock':('src/field_specials.c:627','source catalogue count after first acquisition, equipment catch or badges'),
 'ChooseStarter':('src/battle_setup.c:922','asynchronous paired grant transaction result'),
 'ShowEasyChatScreen':('src/easy_chat.c:1953','legal phrase confirmation or cancel'),
 'GabbyAndTyGetLastQuote':('src/tv.c:918','saved quote exists; consumed by query'),
 'GabbyAndTyGetLastBattleTrivia':('src/tv.c:930','ordered predicates on saved last-battle history'),
 'TryGiveArceusLegendarySignMasteryReward':('src/legendary_signs.c:995','Hall of Fame gate and acquired/not acquired sign; delivery result'),
 'GetPlayerAvatarBike':('src/field_specials.c:818','currently riding Acro 1, Mach 2, otherwise 0'),
 'GiveFlowerShopBerryBundle':('src/field_specials.c:516','one random basic Berry plus six EV Berries, atomic storage rollback'),
 'GetInGameTradeSpeciesInfo':('src/trade.c:4552','requested species from native trade table'),
 'GetTradeSpecies':('src/trade.c:4656','actual selected non-Egg PC/party species'),
 'CanReceiveInGameTradePokemon':('src/trade.c:4560','PC selected always accepted; party respects restricted slot'),
 'ReturnInGameTradeHeldItem':('src/trade.c:5145','return outgoing held item to Bag/PC, preserving Mail'),
 'IsEmeraldChampionsGameCornerPokemonClaimed':('src/field_specials.c:369','prize receipt or either original paired starter'),
 'GiveEmeraldChampionsGameCornerPokemon':('src/field_specials.c:394','one finite, eligible prize transaction'),
 'CanReceiveBerryPair':('src/field_specials.c:544','atomic two-Berry Bag bundle'),
 'CanReceiveShellBellReward':('src/field_specials.c:578','Shell Bell and any still owed Slowbronite'),
 'ShowScrollableMultichoice':('src/field_specials.c:2802','native authored row count and cancel'),
 'TryGiveSelectedLegendarySignReward':('src/legendary_signs.c:743','source gate and party/PC delivery'),
 'TryUnlockLocalLegendaryDiscovery':('src/legendary_signs.c:341','actual local map, progression and Sing/Castform/soot preparation'),
 'CalculatePlayerPartyCount':('src/pokemon.c:3293','native party occupancy'),
 'ScriptGetPartyMonSpecies':('src/field_specials.c:ScriptGetPartyMonSpecies','selected obtainable party species'),
 'GetDiancieFriendshipScore':('src/field_specials.c:GetDiancieFriendshipScore','Diancie at maximum friendship'),
 'ScriptCheckFreePokemonStorageSpace':('src/pokemon_storage_system.c:ScriptCheckFreePokemonStorageSpace','available free PC slot during preparation'),
 'CountPartyNonEggMons':('src/pokemon_storage_system.c:1367','native occupied non-Egg slots'),
 'IsChanseyVialRewardClaimed':('src/quest_states.c:37','saved vial capacity >=2'),
 'GetEmeraldChampionsFinaleStage':('src/emerald_champions_story.c:178','computed victories and completion flags'),
 'BufferEmeraldChampionsRivalBranch':('src/emerald_champions_story.c:58','actual paired-starter save selection'),
 'CheckEmeraldChampionsHandoffItem':('src/field_specials.c:663','Bag or PC quantity >=1'),
 'TakeEmeraldChampionsHandoffItem':('src/field_specials.c:669','debit one from Bag first, otherwise PC'),
 'IsPlayerPartyBelowLevelCap':('src/field_specials.c:5604','occupied non-Egg party levels'),
 'CheckPlayerCaughtSpecies':('src/field_specials.c:364','Pokédex caught flag, not availability'),
 'CheckRelicanthWailord':('src/braille_puzzles.c:336','first Wailord and last Relicanth'),
 'CheckMagikarpBattle':('src/field_specials.c:5580','all six slots Magikarp'),
 'GetBattleOutcome':('src/field_specials.c:1553','recorded native battle outcome'),
 'CanReceiveGoGogglesGift':('src/field_specials.c:590','atomic Bag bundle capacity'),
 'CanReceiveNormanMegaGift':('src/mega_stone_rewards.c:358','undelivered owed starter stones after Norman'),
 'CanReceiveFrontierReward':('src/field_specials.c:558','reward and bottle caps Bag bundle'),
 'CanReceiveLatiStones':('src/field_specials.c:566','only undelivered Lati stones require capacity'),
 'DoDeoxysRockInteraction':('src/field_specials.c:4020','legal shortest-path puzzle sequence, 11 stages'),
 'ScriptGetPokedexInfo':('src/birch_pc.c:7','National Dex flag plus seen/caught counts side effects'),
 'ShouldTryGetTrainerScript':('src/battle_setup.c:2159','remaining battle return-script count'),
 'GetSelectedLegendarySignState':('src/legendary_signs.c:700','caught/lost/acquirable/resting state'),
 'TryUnlockSelectedLegendarySign':('src/legendary_signs.c:677','progression then discovery; permanent unlock side effect'),
 'TryUnlockDarkraiLegendarySign':('src/legendary_signs.c:761','selected-sign operation with Darkrai id'),
 'CreateSelectedLegendarySignEncounter':('src/legendary_signs.c:717','requires acquirable non-resting sign'),
 'CheckEmeraldChampionsGardenCelebi':('src/mega_stone_rewards.c:146','caught/lost/invited garden state'),
 'BufferEmeraldChampionsHarvestRecipe':('src/mega_stone_rewards.c:77','reward already claimed predicate'),
 'TradeEmeraldChampionsGardenBerries':('src/mega_stone_rewards.c:106','harvest recipe counters and Bag debit transaction'),
 'GetBoxedLegendaryGiftSwapStatus':('src/pokemon.c:3236','selected boxed restricted gift and unique party restricted slot'),
 'SwapBoxedLegendaryGiftWithParty':('src/pokemon.c:3255','only swap status 1 succeeds'),
 'ShouldShowBoxWasFullMessage':('src/field_specials.c:4130','box message flag and selected storage box differ'),
 'VAR_FACING':('src/field_control_avatar.c:371','interaction-facing cardinal direction set from native player approach'),
 'COMMAND:msgbox':('src/script_menu.c:688','YES/NO are selectable 1/0'),
 'COMMAND:multichoice':('src/script_menu.c:429','authored native menu rows and optional cancel'),
 'COMMAND:giveitem':('asm/macros/event.inc:2139','AddBagItem result; capacity may fail'),
 'COMMAND:givemon':('src/pokemon.c:3130','party 0, PC 1, cannot give 2'),
}

for _name,_line in {'GetLilycoveLadyId':39,'Script_GetLilycoveLadyId':96,'SetLilycoveLadyGfx':44,
 'GetFavorLadyState':140,'HasAnotherPlayerGivenFavorLadyItem':157,'DidFavorLadyLikeItem':194,
 'Script_DoesFavorLadyLikeItem':236,'IsFavorLadyThresholdMet':241,'FavorLadyGetPrize':250,
 'GetQuizLadyState':320,'GetQuizAuthor':331,'BufferQuizAuthorNameAndCheckIfLady':436,
 'IsQuizLadyWaitingForChallenger':447,'IsQuizAnswerCorrect':458,'GetContestLadyPokeblockState':710,
 'HasPlayerGivenContestLadyPokeblock':722,'ShouldContestLadyShowGoOnAir':730}.items():
    CITATIONS[_name]=(f'src/lilycove_lady.c:{_line}','concrete saved lady state and source-authored choices')
for _room,_line in ((1,1978),(2,1989),(4,2000),(6,2011)):
    CITATIONS[f'FoundAbandonedShipRoom{_room}Key']=(f'src/field_specials.c:{_line}','saved hidden key flag and scratch flag id write')


def _number(value, constants, depth=0):
    if depth>20:return None
    if isinstance(value,(int,bool)):return int(value)
    try:return int(str(value),0)
    except ValueError:
        found=constants.get(value)
        return _number(found,constants,depth+1) if found is not None and found!=value else None


class FlagConstants(dict):
    """Constant map with mutation-safe numeric flag alias indexing."""
    def __init__(self,*args,**kwargs):
        super().__init__(*args,**kwargs);self._flag_reverse=None
    def __setitem__(self,key,value):
        super().__setitem__(key,value);self._flag_reverse=None
    def __delitem__(self,key):
        super().__delitem__(key);self._flag_reverse=None
    def update(self,*args,**kwargs):
        super().update(*args,**kwargs);self._flag_reverse=None
    def clear(self):
        super().clear();self._flag_reverse=None
    def pop(self,*args):
        result=super().pop(*args);self._flag_reverse=None;return result
    def popitem(self):
        result=super().popitem();self._flag_reverse=None;return result
    def setdefault(self,*args):
        result=super().setdefault(*args);self._flag_reverse=None;return result
    def __ior__(self,other):
        self.update(other);return self
    def flag_aliases(self,number):
        if self._flag_reverse is None:
            reverse={}
            for key in self:
                if not isinstance(key,str) or not key.startswith('FLAG_'):continue
                value=_number(key,self)
                if value is not None:reverse.setdefault(value,[]).append(key)
            self._flag_reverse=reverse
        return self._flag_reverse.get(number,())


def _flag(state,key,constants):
    flags=state.get('flags',set())
    if key in flags:return True
    value=_number(key,constants)
    if value is None:return False
    if value in flags or str(value) in flags:return True
    if isinstance(constants,FlagConstants):return any(alias in flags for alias in constants.flag_aliases(value))
    # Generic mutable dictionaries remain uncached. Synthetic geometry tokens
    # cannot be source flags and never enter numeric expression resolution.
    for flag in flags:
        if isinstance(flag,str) and flag.startswith('FLAG_'):
            raw=constants.get(flag)
            if raw==value or (not isinstance(raw,int) and _number(flag,constants)==value):return True
    return False


def _parts(text):
    depth=0;start=0;out=[]
    for i,c in enumerate(text):
        depth += (c=='(')-(c==')')
        if c==',' and depth==0:out.append(text[start:i]);start=i+1
    out.append(text[start:]);return out


def _inputs(key):
    match=re.match(r'SPECIAL:([^(@]+)\((.*)\)$',key)
    if not match:return None,{}
    return match[1],dict(part.split('=',1) for part in _parts(match[2]) if '=' in part)


def _arg(args,name,state,constants):
    value=args.get(name,state.get('vars',{}).get(name))
    if isinstance(value,str) and value.startswith('SPECIAL:'):
        values=evaluate(value,state,constants)
        return next(iter(values)) if values is not None and len(values)==1 else None
    return value


def _token(value,prefix,constants):
    if isinstance(value,str) and value.startswith(prefix):return value
    number=_number(value,constants)
    return next((k for k,v in constants.items() if k.startswith(prefix) and _number(v,{})==number),None) if number is not None else None


def _capacity(state,kind='bag'):
    exact=state.get(kind+'_has_space')
    if exact is not None:return {int(bool(exact))}
    return {0,1} if state.get('preparation_possible') else None


def _coin_funding(state):
    return bool(state.get('coin_funding_possible') or (state.get('funding_possible')
        and 'ITEM_COIN_CASE' in state.get('items',set()) and 'MauvilleCity_GameCorner' in state.get('maps',set())))


def _afford(amount,state,constants):
    domain=numeric_domain(amount,state,constants)
    if domain is None:return None
    money=state.get('money')
    if money is None and not state.get('funding_possible'):return None
    # Funding is an explicit upstream proof of renewable legal income. It is
    # never inferred from preparation_possible or mere player choice.
    return {int(state.get('funding_possible') or money>=price) for price in domain}


def _soot_exchange(args,state,constants):
    choices=numeric_domain(args.get('VAR_0x8004',state.get('vars',{}).get('VAR_0x8004')),state,constants)
    if choices is None or not choices:return None,None,None
    if len(choices)>1:
        results=set()
        for choice in choices:
            domain,_count,_remaining=_soot_exchange(dict(args,VAR_0x8004=choice),state,constants)
            if domain is None:return None,None,None
            results.update(domain)
        return results,None,None
    count=next(iter(choices))
    if count not in (1,5,9):return {0},None,None
    soot=_number(state.get('vars',{}).get('VAR_ASH_GATHER_COUNT',0 if state.get('zero_unset_vars') else None),constants)
    if state.get('preparation_possible') and state.get('collectable_ash'):soot=9999
    if soot is None:return None,None,None
    if count==9:count=soot//500
    if not count or count*500>soot:return {0},None,None
    capacity=_capacity(state)
    return ({1 if space else 2 for space in capacity} if capacity is not None else None),count,soot-count*500


def _caught(state,species):
    if species=='SPECIES_NONE':return True
    records=state.get('caught')
    if records is None and state.get('species_is_caught'):records=state.get('species',set())
    if records is None:return None
    if species in records:return True
    base=_base_species(species)
    # Source-compiled equivalence classes avoid rescanning the entire growing
    # preparation pool for each native Pokédex query. State is never cached.
    return any(member in records for member in _base_species_members().get(base,(base,)))


@lru_cache(None)
def _gates():
    import mega_register
    return mega_register.legendary_gates()


def _sign_sets(state,field,sign):
    values=state.get(field,set())
    return sign in values or sign.replace('LEGENDARY_SIGN_','SPECIES_') in values


@lru_cache(None)
def _species_data():
    import reference_pool
    return reference_pool.SpeciesData()


@lru_cache(None)
def _resolved_species(species):
    return _species_data().resolve(species)


@lru_cache(None)
def _base_species(species):
    resolved=_resolved_species(species)
    return _species_data().base_of.get(resolved,resolved)


@lru_cache(None)
def _base_species_members():
    data=_species_data();index={}
    names=set(data.info)|set(data.aliases)|set(data.aliases.values())|set(data.base_of)|set(data.base_of.values())
    for species in names:index.setdefault(_base_species(species),set()).add(species)
    return {base:frozenset(members) for base,members in index.items()}


def _egg_root(species,constants):
    data=_species_data(); current=_resolved_species(species)
    for _ in range(5):
        pre=list(data.pre_evo.get(current,()))
        if not pre:return current
        if len(pre)==1:current=pre[0];continue
        indices={p:_number(p,constants) for p in pre}
        if any(v is None for v in indices.values()):return None
        current=min(pre,key=indices.get)
    return current


def _family(state,species,constants):
    if not species or species=='SPECIES_NONE':return True
    # A supplied egg-family relation is source-derived from GetEggSpecies.
    family=state.get('species_families')
    if family is None:
        data=_species_data(); relevant=set(state.get('caught',state.get('species',set()))) | {species}
        relevant.update(p.get('species') if isinstance(p,dict) else p for p in state.get('party',[]))
        family={sp:_egg_root(sp,constants) for sp in relevant}
    root=family.get(species,species)
    if root is None:return None
    caught=state.get('caught')
    party=state.get('party')
    if party is not None and any(family.get(p.get('species') if isinstance(p,dict) else p,p.get('species') if isinstance(p,dict) else p)==root for p in party):return True
    if caught is None and state.get('species_is_caught'):caught=state.get('species',set())
    if caught is None:return None
    if any(family.get(p,p)==root for p in caught):return True
    return None if any(family.get(p,p) is None for p in caught) else False


def _legendary(name,args,state,constants):
    sign='LEGENDARY_SIGN_DARKRAI' if name=='TryUnlockDarkraiLegendarySign' else _token(_arg(args,'VAR_0x8004',state,constants),'LEGENDARY_SIGN_',constants)
    if sign is None:return None
    species=sign.replace('LEGENDARY_SIGN_','SPECIES_'); row=_gates().get(species)
    if row is None:return {0}
    caught=_sign_sets(state,'legend_caught',sign)
    if not caught:
        records=_caught(state,species)
        if records is not None:caught=records
    lost=_sign_sets(state,'legend_lost',sign); resting=_sign_sets(state,'legend_resting',sign)
    unlocked=_sign_sets(state,'legend_unlocked',sign)
    badges=sum(_flag(state,f'FLAG_BADGE{i:02d}_GET',constants) for i in range(1,9))
    progress=badges>=row['badges'] and (not row['flag'] or _flag(state,row['flag'],constants))
    if species=='SPECIES_REGIGIGAS':
        needed=[_family(state,'SPECIES_'+p,constants) for p in ('REGIROCK','REGICE','REGISTEEL')]
        discovery=False if False in needed else None if None in needed else True
    elif unlocked:discovery=True
    elif species in ('SPECIES_MELOETTA','SPECIES_LANDORUS','SPECIES_MARSHADOW'):discovery=False
    elif species=='SPECIES_KYUREM':
        needed=[_family(state,'SPECIES_RESHIRAM',constants),_family(state,'SPECIES_ZEKROM',constants)]
        discovery=True if True in needed else None if None in needed else False
    elif species=='SPECIES_PECHARUNT':
        discovery=all(_sign_sets(state,'legend_caught','LEGENDARY_SIGN_'+p) for p in ('OKIDOGI','MUNKIDORI','FEZANDIPITI'))
    else:discovery=_family(state,row.get('req_species'),constants)
    acquire=not caught and not lost and progress and discovery
    if name.startswith('TryUnlock'):
        if caught:return {4}
        if lost:return {6}
        if not progress:return {0}
        if discovery is None:return None
        if not discovery:return {1}
        if resting:return {5}
        return {3 if unlocked else 2}
    if name=='GetSelectedLegendarySignState':
        if caught:return {2}
        if lost:return {4}
        if discovery is None and progress:return None
        return {3 if resting else 1} if acquire else {0}
    if name=='CreateSelectedLegendarySignEncounter':
        if resting or caught or lost or not progress:return {0}
        return {int(discovery)} if discovery is not None else None
    return None


@lru_cache(None)
def _command_source(source):
    try:
        path,line=source.rsplit(':',1)
        raw=(ROOT/path).read_text().splitlines()[int(line)-1].split('@')[0].strip()
        parts=raw.split(None,1)
        return parts[0],[a.strip() for a in parts[1].split(',')] if len(parts)>1 else []
    except (OSError,ValueError,IndexError):return '',[]


@lru_cache(None)
def _menu_rows(menu):
    text=(ROOT/'src/data/script_menu.h').read_text()
    match=re.search(r'\['+re.escape(menu)+r'\]\s*=\s*MULTICHOICE\((\w+)\)',text)
    if not match:return None
    array=re.search(r'\b'+re.escape(match[1])+r'\[\]\s*=\s*\{(.*?)\n\};',text,re.S)
    return len(re.findall(r'\{[^{}]*\}',array[1])) if array else None


def evaluate(key,state,constants=None):
    """Return set[int] of native possible results; None is explicitly unresolved."""
    constants=constants or {}
    if key.startswith(('CHECKMONEY:','MONEY:')):
        return _afford(key.split(':',1)[1].split('@',1)[0],state,constants)
    if key=='COINS' or key.startswith(('CHECKCOINS','COINS:')):
        maximum=_number('MAX_COINS',constants)
        maximum=9999 if maximum is None else maximum
        if _coin_funding(state):return set(range(maximum+1))
        coins=state.get('coins');return {coins} if coins is not None else None
    if key.startswith('RANDOM:'):
        limit=_number(key.split(':',1)[1].split('@',1)[0],constants)
        return set(range(limit)) if limit is not None and 0<limit<=1000 else None
    if key=='VAR_FACING':
        if state.get('facing') is not None:return {int(state['facing'])}
        return set(state.get('facing_domain',(1,2,3,4))) if state.get('preparation_possible') else None
    if key.startswith(('DYN_MULT:','MENU:')):
        values=key.split(':',1)[1].split('@',1)[0].split('|')
        result=set()
        for value in values:
            if value=='HARVEST_BERRIES':
                count=_number('NUM_BERRIES',constants)
                if count is None:return None
                result.update(range(1,count))
            elif value:
                parsed=_number(value,constants)
                if parsed is None:return None
                result.add(parsed)
        return result|{127} if result else None
    if key.startswith('COMMAND:'):
        match=re.match(r'COMMAND:([^@]+)@(.+)',key)
        if not match:return None
        op,source=match.groups();_,args=_command_source(source)
        if op=='checkmoney':return _afford(args[0],state,constants) if args else None
        if op in ('addcoins','removecoins') and args:
            amounts=numeric_domain(args[0],state,constants);balances=evaluate('COINS',state,constants)
            if amounts is None or balances is None:return None
            maximum=_number('MAX_COINS',constants) or 9999
            return {int(balance>=maximum) if op=='addcoins' else int(balance<amount)
                    for balance in balances for amount in amounts}
        if op in ('msgbox','yesnobox'):return {0,1}
        # dynmultichoice left, top, ignoreBPress, maxBeforeScroll, initial,
        # callback, option... (asm/macros/event.inc): one row per option.
        if op=='dynmultichoice' and len(args)>6:
            result=set(range(len(args)-6))
            if args[2] not in ('TRUE','1'):result.add(127)
            return result
        if op.startswith('multichoice') and len(args)>2:
            count=_menu_rows(args[2])
            if count is None:return None
            result=set(range(count))
            if len(args)>3 and args[3] not in ('TRUE','1'):result.add(127)
            return result
        if op in ('giveitem','giveuniqueitem','giveitem_msg','finditem','additem'):return _capacity(state)
        if op in ('givemon','givepokemon','giveegg'):
            if state.get('mon_delivery_result') is not None:return {int(state['mon_delivery_result'])}
            return {0,1,2} if state.get('preparation_possible') else None
        return None
    if key.startswith(('CHECKITEM:','CHECKITEMSPACE:','ITEM:')):
        kind,rest=key.split(':',1);parts=rest.split(',');item=_token(parts[0],'ITEM_',constants)
        if item is None:return None
        if kind=='CHECKITEMSPACE':return _capacity(state)
        quantity=_number(parts[1],constants) if len(parts)>1 else 1
        if quantity==1:return {int(item in state.get('items',set()))}
        counts=state.get('item_quantities')
        return {int(counts.get(item,0)>=quantity)} if counts is not None and quantity is not None else None
    name,args=_inputs(key)
    if name is None:return None
    if name=='RecordOwnedBloodmoon':return {0}  # No script result is read; ownership writes are below.
    if name in ('BufferNormanMegaGiftKind','BufferNormanStarterMegaStone','BufferNormanPartnerMegaStone'):
        rows=_starter_stones()
        pair=_initial_starters(state,constants)
        selected=[r for r in rows if r['starter'] in pair]
        owed=selected or [r for r in rows if r['item']=='ITEM_SWAMPERTITE']
        pending=[r for r in owed if r['item'] not in state.get('items',()) and not _flag(state,r['flag'],constants)]
        if name=='BufferNormanMegaGiftKind':return {3 if not pending else 2 if not selected else 1 if len(pending)==1 else 0}
        if name=='BufferNormanStarterMegaStone':return {_number(pending[0]['item'] if pending else 'ITEM_NONE',constants)}
        partners=[r for r in rows if r['starter'] in ('SPECIES_TREECKO','SPECIES_TORCHIC','SPECIES_MUDKIP')
                  and r['item'] not in state.get('items',()) and not _flag(state,r['flag'],constants)
                  and any(_base_species(s) in r['family'] for s in state.get('species',()))]
        return {_number(r['item'],constants) for r in partners} or {_number('ITEM_NONE',constants)}
    if name=='ScriptGetPartyMonSpecies':
        # The player may choose any currently obtainable non-Egg party member.
        party=state.get('party')
        if party is not None:
            slot=_number(_arg(args,'VAR_0x8004',state,constants),constants)
            if slot is None or slot>=len(party):return None
            return {_number(party[slot].get('species'),constants)}
        if state.get('preparation_possible'):
            values={_number(sp,constants) for sp in state.get('species',())}
            return values-{None} if values else {0}
        return None
    if name=='GetDiancieFriendshipScore':
        if state.get('preparation_possible'):
            return {int('SPECIES_DIANCIE' in state.get('species',()))}
        party=state.get('party')
        if party is None:return None
        return {int(any(mon.get('species')=='SPECIES_DIANCIE' and mon.get('friendship',0)==255 for mon in party))}
    if name=='ScriptCheckFreePokemonStorageSpace':
        return {1} if state.get('preparation_possible') else None
    if name=='NativeScratchOutput':return _scratch_output(args,state,constants)
    # Party composition is a player choice; an eligible party always exists.
    if name=='PlayerPartyLeagueEligible':return {0,1}
    # The Bag choice it tests is a free player pick: TRUE whenever a fossil is held.
    if name=='IsItemFossil':
        text=(ROOT/'src/field_specials.c').read_text().split('sRevivableFossils[]',1)[1].split('};',1)[0]
        held=any(item in state.get('items',()) for item in re.findall(r'\{(ITEM_\w+),',text))
        return {0,1} if held else {0}
    if name=='DoesPlayerHaveFossil':
        text=(ROOT/'src/field_specials.c').read_text().split('sRevivableFossils[]',1)[1].split('};',1)[0]
        return {int(any(item in state.get('items',()) for item in re.findall(r'\{(ITEM_\w+),',text)))}
    if name=='GiveEmeraldChampionsStarterBattleItems':
        return {1} if _flag(state,'FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS',constants) else _capacity(state)
    if name=='StartInitialToolsBagTutorial':
        available=state.get('tutorial_allocation_possible')
        if available is False:return {0}
        return {0,1} if available is True or state.get('preparation_possible') else None
    if name=='GetLeadMonFriendshipScore':
        friendship=state.get('lead_friendship');party=state.get('party')
        if friendship is None and party:
            lead=next((p for p in party if isinstance(p,dict) and not p.get('is_egg')),None)
            if lead:friendship=lead.get('friendship')
        values=set()
        if friendship is not None:values.add(6 if friendship==255 else 5 if friendship>=200 else 4 if friendship>=150 else 3 if friendship>=100 else 2 if friendship>=50 else 1 if friendship else 0)
        if state.get('preparation_possible') and (state.get('species') or party):
            values.update(range(7) if 'OldaleTown_PokemonCenter_1F' in state.get('maps',()) else {6})
        return values if values else None
    if name=='ExchangeSootForCaps':return _soot_exchange(args,state,constants)[0]
    if name=='BufferEmeraldChampionsBattleItemStock':
        categories=numeric_domain(args.get('VAR_0x8004',state.get('vars',{}).get('VAR_0x8004')),state,constants)
        if categories is None:return None
        rows=_battle_stock_rows();badges=sum(_flag(state,f'FLAG_BADGE{i:02d}_GET',constants) for i in range(1,9))
        acquired=set(state.get('items',()))|set(state.get('pc_items',()))|set(state.get('unlocked_battle_items',()))
        counts=set()
        for category in categories:
            row=next((items for name,items in rows['categories'].items() if _number(name,constants)==category),())
            count=0
            for item in row:
                equipment=rows['equipment'].get(item)
                caught=_caught(state,equipment) if equipment else False
                if item in acquired or caught or (item in rows['badge_stock'] and badges>=rows['badge_stock'][item]):count+=1
            counts.add(count)
        return counts
    if name in ('GetCurrentMauvilleOldMan','Script_GetCurrentMauvilleMan'):
        id_=state.get('mauville_man_id');trainer=state.get('trainer_id')
        if id_ is None and trainer is not None:id_=(trainer%10)//2
        return {id_} if id_ in range(5) else None
    if name=='GetTraderTradedFlag':
        traded=state.get('trader',{}).get('already_traded');return {int(traded)} if traded is not None else None
    if name=='DoesPlayerHaveNoDecorations':
        decorations=state.get('decorations')
        if decorations is None and state.get('preparation_possible'):
            decorations={i for i in state.get('items',()) if i.startswith('DECOR_') and i!='DECOR_NONE'}
        return {int(not decorations)} if decorations is not None else None
    if name=='IsDecorationCategoryFull':
        full=state.get('decoration_category_full')
        return {int(full)} if full is not None else {0,1} if state.get('preparation_possible') and state.get('decoration_trade_possible') else None
    if name=='IsEnoughForCostInVar0x8005':return _afford(_arg(args,'VAR_0x8005',state,constants),state,constants)
    if name in ('ContestWinnerId','ContestPlayerId'):
        value=state.get('contest_winner' if name=='ContestWinnerId' else 'contest_player')
        return {value} if value in range(4) else None
    if name in ('Script_FavorLadyOpenBagMenu','Script_QuizLadyOpenBagMenu'):
        item=state.get('selected_item')
        if item is None:return {0} if not state.get('items') else None
        return {0,1} if item in state.get('items',set()) else {0}
    lady=_lady_predicate(name,state,constants)
    if lady is not NotImplemented:return lady
    if name in ('TryEnterContestMon','HasMonWonThisContestBefore'):
        mon=state.get('contest_mon');rank=_number(state.get('contest_rank',state.get('vars',{}).get('VAR_CONTEST_RANK')),constants)
        category=_number(state.get('contest_category',state.get('vars',{}).get('VAR_CONTEST_CATEGORY')),constants)
        if mon is None or rank is None or category is None:return None
        ribbon=mon.get('ribbons',{}).get(category)
        if name=='TryEnterContestMon':
            if mon.get('is_egg'):return {3}
            if mon.get('hp')==0:return {4}
            if mon.get('hp') is None:return None
        if ribbon is None:return None
        return {int(ribbon>rank)} if name=='HasMonWonThisContestBefore' else {2 if ribbon>rank else 1 if ribbon>=rank else 0}
    if name in ('GetSelectedLegendarySignState','TryUnlockSelectedLegendarySign','TryUnlockDarkraiLegendarySign','CreateSelectedLegendarySignEncounter'):
        return _legendary(name,args,state,constants)
    if name in ('CanReceiveBerryPair','CanReceiveShellBellReward'):return _capacity(state)
    if name and name.startswith('FoundAbandonedShipRoom'):
        room=re.search(r'Room(\d)',name)
        return {int(_flag(state,'FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_'+room[1]+'_KEY',constants))} if room else None
    if name in ('CalculatePlayerPartyCount','CountPartyNonEggMons'):
        party=state.get('party')
        if party is None:return set(range(7)) if state.get('preparation_possible') else None
        if name=='CalculatePlayerPartyCount':return {len(party)}
        return {sum((p.get('species') not in ('SPECIES_NONE','SPECIES_EGG') and not p.get('is_egg')) if isinstance(p,dict) else p not in ('SPECIES_NONE','SPECIES_EGG') for p in party)}
    if name=='IsChanseyVialRewardClaimed':
        capacity=_number(state.get('vars',{}).get('VAR_POKE_VIAL_MAX_CHARGES',0 if state.get('zero_unset_vars') else None),constants)
        return {int(capacity>=2)} if capacity is not None else None
    if name=='ShowScrollableMultichoice':
        menu=_token(_arg(args,'VAR_0x8004',state,constants),'SCROLL_MULTI_',constants)
        count=_scroll_menu_rows(menu) if menu else None
        return set(range(count))|{127} if count is not None else None
    if name in ('GetInGameTradeSpeciesInfo','CanReceiveInGameTradePokemon'):
        trade=_token(_arg(args,'VAR_0x8005',state,constants),'INGAME_TRADE_',constants)
        row=_trade_rows().get(trade)
        if row is None:return {0} if name=='CanReceiveInGameTradePokemon' else None
        if name=='GetInGameTradeSpeciesInfo':
            number=_number(row['requestedSpecies'],constants);return {number} if number is not None else None
        slot=_number(_arg(args,'VAR_0x8004',state,constants),constants)
        if slot==254:return {1}
        if slot is not None and not 0<=slot<6:return {0}
        if state.get('trade_party_receivable') is not None:return {int(state['trade_party_receivable'])}
        return {1} if state.get('preparation_possible') else None # can select outgoing PC mon
    if name=='GetTradeSpecies':
        selected=state.get('selected_trade_species')
        if selected is not None:
            number=_number(selected,constants);return {number} if number is not None else None
        if state.get('preparation_possible'):
            values={_number(species,constants) for species in state.get('species',set())};return values-{None}
        return None
    if name=='ReturnInGameTradeHeldItem':
        item=state.get('outgoing_trade_item')
        if item=='ITEM_NONE':return {1}
        if item is not None and item.endswith('_MAIL'):return {4}
        if item is not None and state.get('bag_has_space') is True:return {2}
        if item is not None and state.get('pc_has_space') is True:return {3}
        return {1,2,3,4,0} if state.get('preparation_possible') else None
    if name in ('IsEmeraldChampionsGameCornerPokemonClaimed','GiveEmeraldChampionsGameCornerPokemon'):
        species=_token(_arg(args,'VAR_0x8004',state,constants),'SPECIES_',constants)
        flag=_prize_flags().get(species)
        if not flag:return {0} if name.startswith('Is') else {3}
        initial=_initial_starters(state,constants)
        claimed=_flag(state,flag,constants) or species in initial
        if name.startswith('Is'):return {int(claimed)}
        if claimed:return {3}
        if species in _gates():
            signargs={'VAR_0x8004':species.replace('SPECIES_','LEGENDARY_SIGN_')}
            eligible=_legendary('GetSelectedLegendarySignState',signargs,state,constants)
            if eligible is None:return None
            if eligible!={1}:return {4}
        delivery=state.get('mon_delivery_result')
        return {delivery} if delivery in (0,1,2) else {0,1,2} if state.get('preparation_possible') else None
    if name=='TryGiveSelectedLegendarySignReward':
        sign=_token(_arg(args,'VAR_0x8004',state,constants),'LEGENDARY_SIGN_',constants)
        if not sign:return {0}
        if _sign_sets(state,'legend_caught',sign):return {0}
        eligible=_legendary('GetSelectedLegendarySignState',{'VAR_0x8004':sign},state,constants)
        if eligible is None:return None
        if eligible!={1}:return {0}
        delivery=state.get('mon_delivery_result')
        return {delivery+1} if delivery in (0,1,2) else {1,2,3} if state.get('preparation_possible') else None
    if name=='TryUnlockLocalLegendaryDiscovery':return _local_discovery(args,state,constants)
    if name=='GetPlayerAvatarBike':
        if state.get('bike') is not None:return {int(state['bike'])}
        if not state.get('preparation_possible'):return None
        return {0}|({1} if 'ITEM_ACRO_BIKE' in state.get('items',set()) else set())|({2} if 'ITEM_MACH_BIKE' in state.get('items',set()) else set())
    if name=='GiveFlowerShopBerryBundle':
        basic={'ITEM_'+b+'_BERRY' for b in ('CHERI','CHESTO','PECHA','RAWST','ASPEAR','LEPPA','ORAN','PERSIM')}
        berry=_token(_arg(args,'VAR_0x8004',state,constants),'ITEM_',constants)
        if berry in basic:return _capacity(state)
        domain=numeric_domain(args.get('VAR_0x8004'),state,constants);first=_number('FIRST_BERRY_INDEX',constants)
        if domain is not None and first is not None and domain and all(first<=n<first+8 for n in domain):return _capacity(state)
        number=_number(_arg(args,'VAR_0x8004',state,constants),constants);first=_number('FIRST_BERRY_INDEX',constants)
        if number is not None and first is not None:return _capacity(state) if first<=number<first+8 else {0}
        return None
    if name=='TryGiveArceusLegendarySignMasteryReward':
        sign='LEGENDARY_SIGN_ARCEUS'
        if _sign_sets(state,'legend_caught',sign):return {4}
        if not (_flag(state,'FLAG_IS_CHAMPION',constants) or _flag(state,'FLAG_SYS_GAME_CLEAR',constants)):return {0}
        if _sign_sets(state,'legend_lost',sign):return {0}
        delivery=state.get('mon_delivery_result')
        return {delivery+1} if delivery in (0,1,2) else {1,2,3} if state.get('preparation_possible') else None
    if name=='ChooseStarter':
        first=_number(state.get('first'),constants);second=_number(state.get('second'),constants);party=state.get('party')
        if party is not None and len(party)>0:return {0}
        if first is not None and second is not None and party is not None:return {int(first in range(3) and second in range(3) and first!=second)}
        return {0,1} if state.get('preparation_possible') else None
    if name=='ShowEasyChatScreen':return {0,1}
    if name=='GabbyAndTyGetLastQuote':
        quote=state.get('gabby_quote')
        return {int(bool(quote))} if quote is not None else None
    if name=='GabbyAndTyGetLastBattleTrivia':
        data=state.get('gabby_history')
        if data is None:return None
        if not data.get('battleTookMoreThanOneTurn2'):return {1}
        if data.get('playerThrewABall2'):return {2}
        if data.get('playerUsedHealingItem2'):return {3}
        if data.get('playerLostAMon2'):return {4}
        return {0}
    if name=='GetEmeraldChampionsFinaleStage':
        defeated=state.get('defeated',set())
        if not _flag(state,'FLAG_SYS_GAME_CLEAR',constants):return {0}
        if 'TRAINER_WALLY_VR_2' not in defeated:return {1}
        if not {'TRAINER_COLTON','TRAINER_MICAH','TRAINER_THOMAS','TRAINER_LEA_AND_JED','TRAINER_NAOMI'}<=defeated:return {2}
        if 'TRAINER_STEVEN' not in defeated or not _flag(state,'FLAG_RECEIVED_AURORA_TICKET',constants):return {3}
        if not any(_flag(state,f,constants) for f in ('FLAG_EC_FINALE_DEOXYS_RESOLVED','FLAG_BATTLED_DEOXYS','FLAG_DEFEATED_DEOXYS')):return {4}
        return {6 if 'TRAINER_BUFFEL' in defeated else 5}
    if name=='BufferEmeraldChampionsRivalBranch':
        variables=state.get('vars',{})
        first=_number(state.get('first',variables.get('VAR_STARTER_MON')),constants)
        second=_number(state.get('second'),constants)
        savedsecond=_number(variables.get('VAR_EC_SECOND_STARTER'),constants)
        opening=_number(variables.get('VAR_EC_OPENING_STATE'),constants)
        if first is None:return None
        first%=3
        paired=(second is not None) or (opening in (1,2) and savedsecond in (1,2,3))
        if second is None and paired:second=savedsecond-1
        rival=3-first-second if paired else (first+1)%3
        return {(rival+2)%3} if rival in (0,1,2) else None
    if name in ('CheckEmeraldChampionsHandoffItem','TakeEmeraldChampionsHandoffItem'):
        item=_token(_arg(args,'VAR_0x8004',state,constants),'ITEM_',constants)
        return {int(item in (state.get('items',set()) | state.get('pc_items',set())))} if item else None
    if name=='CheckPlayerCaughtSpecies':
        species=_token(_arg(args,'VAR_0x8004',state,constants),'SPECIES_',constants)
        result=_caught(state,species) if species else None
        return {int(result)} if result is not None else None
    if name=='GetBattleOutcome':
        outcome=state.get('battle_outcome')
        return {_number(outcome,constants)} if _number(outcome,constants) is not None else None
    if name=='CanReceiveLatiStones' and {'ITEM_LATIOSITE','ITEM_LATIASITE'} <= state.get('finite_duplicate_rewards',set()):return {1}
    if name in ('CanReceiveGoGogglesGift','CanReceiveNormanMegaGift','CanReceiveFrontierReward','CanReceiveLatiStones','CanReceiveWeatherInstituteRocks'):return _capacity(state)
    if name=='IsPlayerPartyBelowLevelCap':
        party=state.get('party')
        if party is not None and all(isinstance(p,dict) and 'level' in p for p in party):
            return {int(any(p.get('species') not in ('SPECIES_NONE','SPECIES_EGG') and p['level']<p.get('cap',state.get('cap',0)) for p in party))}
        if state.get('preparation_possible') and 'ITEM_LEVELER' in state.get('items',set()):return {0,1}
        return None
    if name in ('CheckRelicanthWailord','CheckMagikarpBattle'):
        party=state.get('party')
        if party is not None:
            species=[p.get('species') if isinstance(p,dict) else p for p in party]
            success=(len(species)>=2 and species[0]=='SPECIES_WAILORD' and species[-1]=='SPECIES_RELICANTH') if name=='CheckRelicanthWailord' else len(species)==6 and all(p=='SPECIES_MAGIKARP' for p in species)
            return {int(success)}
        required={'SPECIES_WAILORD','SPECIES_RELICANTH'} if name=='CheckRelicanthWailord' else {'SPECIES_MAGIKARP'}
        if state.get('preparation_possible'):return {0,1} if required<=state.get('species',set()) else {0}
        return None
    if name=='ScriptGetPokedexInfo':
        if not _flag(state,'FLAG_SYS_NATIONAL_DEX',constants):return {0}
        magic=state.get('national_magic'); var=_number(state.get('vars',{}).get('VAR_NATIONAL_DEX'),constants)
        # It only picks Birch's rating lines; when the save magic is unknown
        # either answer leads on to the same scene.
        if magic is None or var is None:return {0,1}
        return {int(magic==0xDA and var==0x302)}
    if name=='ShouldTryGetTrainerScript':
        count=state.get('trainer_return_script_count');return {int(count>1)} if count is not None else None
    if name=='DoDeoxysRockInteraction':
        if _flag(state,'FLAG_DEOXYS_ROCK_COMPLETE',constants):return {3}
        level=_number(state.get('vars',{}).get('VAR_DEOXYS_ROCK_LEVEL',0 if state.get('zero_unset_vars') else None),constants)
        # Each visit resets the level (BirthIsland_Exterior/scripts.inc:17-18),
        # but touching the triangle where it moves walks levels 0-10 within that
        # visit, so the solving touch is always one walk away.
        if level is not None:return {0,1,2}
        return {0,1,2} if state.get('preparation_possible') else None
    if name=='CheckEmeraldChampionsGardenCelebi':
        if _sign_sets(state,'legend_caught','LEGENDARY_SIGN_CELEBI'):return {2}
        garden=state.get('garden_celebi')
        if garden is None:return None
        return {3 if garden==2 else 1 if garden else 0}
    if name in ('BufferEmeraldChampionsHarvestRecipe','TradeEmeraldChampionsGardenBerries'):
        return _harvest(name,args,state,constants)
    if name=='GetBoxedLegendaryGiftSwapStatus':
        status=state.get('gift_swap_status');return {int(status)} if status is not None else None
    if name=='SwapBoxedLegendaryGiftWithParty':
        status=state.get('gift_swap_status');return {int(status==1)} if status is not None else None
    if name=='ShouldShowBoxWasFullMessage':
        if _flag(state,'FLAG_SHOWN_BOX_WAS_FULL_MESSAGE',constants):return {0}
        current=state.get('storage_box');target=state.get('gift_box')
        return {int(current!=target)} if current is not None and target is not None else None
    return None

# Direct rows from src/mega_stone_rewards.c:24-38; counts are harvested-pouch
# counts, not bag Berry quantities, and cannot be paid with purchased berries.
HARVEST_RECIPES = [
 ('ITEM_BAXCALIBRITE','FLAG_EC_BERRY_TRADE_BAXCALIBRITE',{'RAZZ':6,'BLUK':6,'NANAB':4,'WEPEAR':4}),
 ('ITEM_DRAGONINITE','FLAG_EC_BERRY_TRADE_DRAGONINITE',{'CHERI':6,'CHESTO':6,'ORAN':6,'PECHA':6}),
 ('ITEM_TYRANITARITE','FLAG_EC_BERRY_TRADE_TYRANITARITE',{'POMEG':4,'KELPSY':4,'QUALOT':4,'HONDEW':4,'GREPA':4,'TAMATO':4}),
 ('ITEM_NONE',None,{'PINAP':8,'SITRUS':8,'LUM':4,'LEPPA':8}),
]


def _harvest(name,args,state,constants):
    # Nested symbolic menu-loop expressions remain unresolved until root gives
    # a concrete native reward choice; this avoids inventing harvest payments.
    raw=args.get('VAR_0x8004',state.get('vars',{}).get('VAR_0x8004'))
    if isinstance(raw,str) and len(raw)>256:return None
    choice=_number(_arg(args,'VAR_0x8004',state,constants),constants)
    if choice is None:return None
    if choice not in range(len(HARVEST_RECIPES)):return {0 if name=='BufferEmeraldChampionsHarvestRecipe' else 4}
    item,flag,recipe=HARVEST_RECIPES[choice]
    if choice==3:
        garden=state.get('garden_celebi')
        caught=_sign_sets(state,'legend_caught','LEGENDARY_SIGN_CELEBI')
        if not caught and garden is None:return None
        claimed=caught or bool(garden)
    else:claimed=_flag(state,flag,constants) or item in (state.get('items',set())|state.get('pc_items',set()))
    if choice<3 and not claimed and not _flag(state,'FLAG_BADGE07_GET',constants):return {5}
    if name=='BufferEmeraldChampionsHarvestRecipe':return {int(claimed)}
    if claimed:return {1}
    pouch=state.get('harvested_berries')
    if pouch is not None:
        enough=all(pouch.get('BERRY_ID_'+berry,pouch.get('ITEM_'+berry+'_BERRY',pouch.get(berry,0)))>=count for berry,count in recipe.items())
    elif state.get('preparation_possible') and 'harvestable_berries' in state:
        renewable=state['harvestable_berries']
        enough=all('BERRY_ID_'+b in renewable or 'ITEM_'+b+'_BERRY' in renewable or b in renewable for b in recipe)
    else:return None
    if not enough:return {2}
    if choice==3:return {0}
    capacity=_capacity(state)
    return {0 if success else 3 for success in capacity} if capacity is not None else None


def native_effects(key,result,state,constants=None):
    """Native writes needed when committing a completed event's chosen result.

    Returns structural mutations; the caller commits them atomically with the
    script path and its normal writes. This function does not modify State.
    """
    constants=constants or {};name,args=_inputs(key)
    effect={'flags_set':set(),'vars':{},'items_add':{},'items_remove':{},
            'legend_unlocked':set(),'harvested_remove':{}}
    if key.startswith('COMMAND:'):
        match=re.match(r'COMMAND:([^@]+)@(.+)',key)
        if not match:return effect
        op,source=match.groups();_,rawargs=_command_source(source)
        if rawargs and op in ('addmoney','removemoney','addcoins','removecoins') and (op.endswith('money') or result==0):
            domain=numeric_domain(rawargs[0],state,constants)
            if domain is not None and len(domain)==1:
                amount=next(iter(domain));kind='money' if op.endswith('money') else 'coins'
                effect[kind+'_add' if op.startswith('add') else kind+'_remove']=amount
        return effect
    if name is None:return effect
    if name=='RecordOwnedBloodmoon' and 'SPECIES_URSALUNA_BLOODMOON' in state.get('species',()):
        effect['flags_set'].add('FLAG_EC_CAUGHT_URSALUNA_BLOODMOON')
    if name=='GiveEmeraldChampionsStarterBattleItems' and result==1 and not _flag(state,'FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS',constants):
        text=(ROOT/'src/field_specials.c').read_text().split('void GiveEmeraldChampionsStarterBattleItems(void)',1)[1].split('};',1)[0]
        effect['items_add']={item:1 for item in re.findall(r'\{(ITEM_\w+),\s*1\}',text)}
        effect['flags_set'].add('FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS')
    elif name=='ExchangeSootForCaps' and result==1:
        domain,count,remaining=_soot_exchange(args,state,constants)
        if count is not None:effect['items_add']['ITEM_BOTTLE_CAP']=count
        if remaining is not None:effect['vars']['VAR_ASH_GATHER_COUNT']=remaining
    elif name=='TraderDoDecorationTrade':
        incoming=_token(_arg(args,'VAR_0x8004',state,constants),'DECOR_',constants)
        outgoing=_token(_arg(args,'VAR_0x8006',state,constants),'DECOR_',constants)
        if incoming and outgoing:
            effect['decorations_add']={incoming};effect['decorations_remove']={outgoing};effect['trader_update']={'already_traded':True}
    elif name=='SubtractMoneyFromVar0x8005':
        domain=numeric_domain(_arg(args,'VAR_0x8005',state,constants),state,constants)
        if domain is not None and len(domain)==1:effect['money_remove']=next(iter(domain))
    elif name=='ReturnInGameTradeHeldItem' and result in (2,3):
        item=state.get('outgoing_trade_item')
        if item:
            effect.setdefault('items_add' if result==2 else 'pc_items_add',{})[item]=1
            effect['outgoing_trade_item']='ITEM_NONE'
    elif name=='Script_FavorLadyOpenBagMenu' and result==1:
        item=state.get('selected_item')
        if item:effect['items_remove'][item]=1
    elif name and name.startswith('FoundAbandonedShipRoom'):
        room=re.search(r'Room([1246])Key$',name)
        if room:effect['vars']['VAR_0x8004']='FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_'+room[1]+'_KEY'
    elif name=='BufferQuizPrizeItem':
        row=state.get('lilycove_lady',{}).get('quiz',{})
        prize=row.get('prize')
        if prize is None and row.get('question_id') in range(len(_lady_rows()[0])):prize=_lady_rows()[0][row['question_id']]['prize']
        if prize is not None:effect['vars']['VAR_0x8005']=prize
    elif name in ('SetQuizLadyState_Complete','SetQuizLadyState_GivePrize','QuizLadySetWaitingForChallenger','ClearQuizLadyPlayerAnswer'):
        field,value={'SetQuizLadyState_Complete':('state',1),'SetQuizLadyState_GivePrize':('state',2),'QuizLadySetWaitingForChallenger':('waiting',True),'ClearQuizLadyPlayerAnswer':('player_answer','EC_EMPTY_WORD')}[name]
        effect['lilycove_lady_update']={'quiz':{field:value}}
    elif name=='FavorLadyGetPrize':effect['lilycove_lady_update']={'favor':{'state':2}}
    elif name=='Script_DoesFavorLadyLikeItem' and result in (0,1):
        lady=state.get('lilycove_lady',{}).get('favor',{}); item=state.get('selected_item')
        count=lady.get('num_items_given')
        updates={'state':1,'item':item,'liked':bool(result),'has_player_name':True}
        if count is not None:updates['num_items_given']=5 if result and item==lady.get('best_item') else count+int(result)
        effect['lilycove_lady_update']={'favor':updates}
    elif name=='QuizLadyTakePrizeForCustomQuiz':
        item=state.get('selected_item')
        if item:effect['items_remove'][item]=1
    elif name=='QuizLadyRecordCustomQuizData':effect['lilycove_lady_update']={'quiz':{'prize':state.get('selected_item'),'author':0}}
    elif name=='SetContestLadyGivenPokeblock':effect['lilycove_lady_update']={'contest':{'given':True}}
    elif name in ('TryUnlockSelectedLegendarySign','TryUnlockDarkraiLegendarySign','CreateSelectedLegendarySignEncounter'):
        sign='LEGENDARY_SIGN_DARKRAI' if name=='TryUnlockDarkraiLegendarySign' else _token(_arg(args,'VAR_0x8004',state,constants),'LEGENDARY_SIGN_',constants)
        succeeds=result in (2,3) if name.startswith('TryUnlock') else result==1
        if sign and succeeds:effect['legend_unlocked'].add(sign)
    elif name=='TryUnlockLocalLegendaryDiscovery' and result==1:
        sign=_token(_arg(args,'VAR_0x8004',state,constants),'LEGENDARY_SIGN_',constants)
        if sign:effect['legend_unlocked'].add(sign)
    elif name=='GiveEmeraldChampionsGameCornerPokemon' and result in (0,1):
        species=_token(_arg(args,'VAR_0x8004',state,constants),'SPECIES_',constants)
        if species:
            effect['species_add']={species};effect['caught_add']={species}
            flag=_prize_flags().get(species)
            if flag:effect['flags_set'].add(flag)
            if species in _gates():effect['legend_caught']={species.replace('SPECIES_','LEGENDARY_SIGN_')}
    elif name=='TryGiveSelectedLegendarySignReward' and result in (1,2):
        sign=_token(_arg(args,'VAR_0x8004',state,constants),'LEGENDARY_SIGN_',constants)
        if sign:
            species=sign.replace('LEGENDARY_SIGN_','SPECIES_')
            effect['species_add']={species};effect['caught_add']={species};effect['legend_caught']={sign};effect['legend_unlocked'].add(sign)
    elif name=='DoDeoxysRockInteraction':
        level=_number(state.get('vars',{}).get('VAR_DEOXYS_ROCK_LEVEL',0 if state.get('zero_unset_vars') else None),constants)
        if result!=3:effect['vars']['VAR_DEOXYS_ROCK_STEP_COUNT']=0
        if result==0:effect['vars']['VAR_DEOXYS_ROCK_LEVEL']=0
        elif result==1 and level is not None and 0<=level<10:
            effect['vars']['VAR_DEOXYS_ROCK_LEVEL']=level+1
        elif result==2 and level==10:effect['flags_set'].add('FLAG_DEOXYS_ROCK_COMPLETE')
    elif name=='TakeEmeraldChampionsHandoffItem' and result==1:
        item=_token(_arg(args,'VAR_0x8004',state,constants),'ITEM_',constants)
        if item:effect['items_remove'][item]=1
    elif name=='TradeEmeraldChampionsGardenBerries' and result==0:
        choice=_number(_arg(args,'VAR_0x8004',state,constants),constants)
        if choice in range(len(HARVEST_RECIPES)):
            item,flag,recipe=HARVEST_RECIPES[choice]
            effect['harvested_remove']={'BERRY_ID_'+b:n for b,n in recipe.items()}
            if choice==3:effect['garden_celebi']=1
            else:effect['items_add'][item]=1;effect['flags_set'].add(flag)
    elif name=='LoseEmeraldChampionsGardenCelebi':effect['garden_celebi']=2
    elif name=='TryGiveArceusLegendarySignMasteryReward':
        if result in (1,2,3):effect['legend_unlocked'].add('LEGENDARY_SIGN_ARCEUS')
        if result in (1,2):
            effect['species_add']={'SPECIES_ARCEUS'};effect['caught_add']={'SPECIES_ARCEUS'};effect['legend_caught']={'LEGENDARY_SIGN_ARCEUS'}
    elif name=='GiveFlowerShopBerryBundle' and result==1:
        berry=_token(_arg(args,'VAR_0x8004',state,constants),'ITEM_',constants)
        effect['items_add']={i:1 for i in ['ITEM_'+b+'_BERRY' for b in ('POMEG','KELPSY','QUALOT','HONDEW','GREPA','TAMATO')]}
        if berry:effect['items_add'][berry]=1
        else:
            domain=numeric_domain(args.get('VAR_0x8004'),state,constants)
            if domain is not None:effect['items_add_possible']={_token(n,'ITEM_',constants) for n in domain}- {None}
    elif name=='ChooseStarter' and result==1:
        first=_number(state.get('first'),constants);second=_number(state.get('second'),constants)
        if first in range(3) and second in range(3) and first!=second:
            effect['vars'].update({'VAR_STARTER_MON':first,'VAR_EC_SECOND_STARTER':second+1,'VAR_EC_OPENING_STATE':1})
            effect['flags_set'].add('FLAG_SYS_POKEMON_GET')
            generation=_number(state.get('starter_generation',state.get('vars',{}).get('VAR_STARTER_GEN',0)),constants)
            starters=_starter_rows().get(generation,_starter_rows()[0])
            effect['species_add']={starters[first],starters[second]}
    elif name=='GabbyAndTyGetLastQuote' and result==1:effect['gabby_quote']=False
    elif name=='EnableNationalPokedex':
        effect['flags_set'].add('FLAG_SYS_NATIONAL_DEX');effect['vars']['VAR_NATIONAL_DEX']=0x302
        effect['national_magic']=0xDA
    elif name=='ShouldShowBoxWasFullMessage' and result==1:
        effect['flags_set'].add('FLAG_SHOWN_BOX_WAS_FULL_MESSAGE')
    return effect


@lru_cache(None)
def _starter_rows():
    text=(ROOT/'src/starter_choose.c').read_text()
    body=text.split('sStarterMons[][STARTER_MON_COUNT]',1)[1].split('};',1)[0]
    return {int(n):re.findall(r'SPECIES_\w+',values) for n,values in re.findall(r'\[(\d+)\]\s*=\s*\{([^}]+)\}',body)}


@lru_cache(None)
def _starter_stones():
    text=(ROOT/'src/mega_stone_rewards.c').read_text().split('} sStarterMegaStones[] =',1)[1].split('};',1)[0]
    from manifest_pokemon_sources import PokemonSources
    pokemon=PokemonSources()
    starters={s for row in _starter_rows().values() for s in row}
    result=[]
    for final,item,flag in re.findall(r'\{(SPECIES_\w+),\s*(ITEM_\w+),\s*(FLAG_\w+)\}',text):
        family={final};changed=True
        while changed:
            changed=False
            for edge in pokemon.evolutions:
                if edge['target'] in family and edge['species'] not in family:
                    family.add(edge['species']);changed=True
        starter=next(iter(family & starters),None)
        result.append(dict(starter=starter,item=item,flag=flag,family=family))
    return result


def initial_event_state():
    """Literal new-save defaults, before the truck's field events run.

    Native memory clears all other persistent vars/flags to zero; absent vars
    must therefore default to zero in the caller's new-save State.
    """
    text=(ROOT/'data/scripts/new_game.inc').read_text()
    text=text.split('EventScript_ResetAllMapFlags::',1)[1]
    flags=set(re.findall(r'^\s*setflag\s+(FLAG_\w+)',text,re.M))
    flags.update(('FLAG_EC_PLAYER_IVS_MAXED','FLAG_SYS_SHOAL_TIDE'))
    variables={key:int(value,0) for key,value in re.findall(r'^\s*setvar\s+(VAR_\w+),\s*(0x[0-9A-Fa-f]+|\d+)\b',text,re.M)}
    return {'flags':flags,'vars':variables,'items':set(),'pc_items':{'ITEM_POTION'},
            'item_quantities':{},'pc_item_quantities':{'ITEM_POTION':1},
            'money':6000,'coins':0,'caught':set(),'legend_caught':set(),'legend_lost':set(),
            'legend_unlocked':set(),'legend_resting':set(),'garden_celebi':0,
            'national_magic':0,'defeated':set(),'harvested_berries':{},'zero_unset_vars':True,
            'gabby_quote':False,'gabby_history':{'battleTookMoreThanOneTurn2':False},
            'sources':['src/event_data.c:51','src/new_game.c:186','src/new_game.c:190',
                       'src/new_game.c:231','data/scripts/new_game.inc:115','src/player_pc.c:221']}


def coverage_report(data,state=None,constants=None):
    """Actionable distinction between unsupported code and missing native state."""
    keys=set()
    def scan(value):
        if isinstance(value,dict):
            key=value.get('key')
            if isinstance(key,str) and key.startswith(('SPECIAL:','COMMAND:','DYN_MULT:','MENU:','ITEM:','CHECKITEM:','CHECKITEMSPACE:','CHECKMONEY:','MONEY:','CHECKCOINS','COINS')):keys.add(key)
            for child in value.values():scan(child)
        elif isinstance(value,list):
            for child in value:scan(child)
    scan(data);report=[]
    for key in sorted(keys):
        name,_args=_inputs(key)
        supported=(name in CITATIONS) if name else key.startswith(('COMMAND:','DYN_MULT:','MENU:','ITEM:','CHECKITEM:','CHECKITEMSPACE:','CHECKMONEY:','MONEY:','CHECKCOINS','COINS'))
        if name in ('DrawWholeMapView','MauvilleGymDeactivatePuzzle','PetalburgGymUnlockRoomDoors','ResetHealLocationFromDewford'):
            report.append({'key':key,'status':'reextract','reason':'Pure native redraw preserves VAR_RESULT; parser now preserves it','source':'src/field_camera.c:94'})
        elif not supported:report.append({'key':key,'status':'unsupported','reason':'Native result semantics require source inspection'})
        elif state is not None and evaluate(key,state,constants) is None:
            report.append({'key':key,'status':'state_required','source':CITATIONS.get(name,(None,))[0],
                           'reason':'Concrete native inputs, save state, party/history or storage state required'})
    return report


@lru_cache(None)
def _native_function_index():
    """Read native function bodies, discarding quoted text and comments.

    Used only for symbolic input capture: scratch arguments not read by the
    native special (or its directly called helpers) do not affect its result.
    Unknown external helpers remain an explicit unavailable analysis result.
    """
    functions={}
    scrub=re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',re.S)
    declaration=re.compile(r'^\w[^;\n{}]*?\b([A-Za-z_]\w*)\s*\([^;\n{}]*\)\s*\n\{',re.M)
    for path in sorted((ROOT/'src').glob('*.c')):
        raw=path.read_text();text=scrub.sub(lambda m:''.join('\n' if c=='\n' else ' ' for c in m[0]),raw)
        for match in declaration.finditer(text):
            start=match.end();depth=1;end=start
            while end<len(text) and depth:
                depth+=(text[end]=='{')-(text[end]=='}');end+=1
            body=text[start:end-1];reads=set()
            for var in re.finditer(r'\bgSpecialVar_(0x800[0-9A-Fa-f])\b',body):
                if not re.match(r'\s*=(?!=)',body[var.end():]):reads.add('VAR_'+var[1])
            calls=set(re.findall(r'\b([A-Za-z_]\w*)\s*\(',body))
            writes=bool(re.search(r'\b(?:FlagSet|FlagClear|VarSet|AddBagItem|AddPCItem|RemoveBagItem|RemovePCItem|GiveMonToPartyOrPC|GiveScriptedMonToPlayer|SetMonData|SetBoxMonData|SetMoney|RemoveMoney|AddMoney|SetCoins|AddCoins|RemoveCoins)\s*\(',body) or re.search(r'\b(?:gSaveBlock[123]Ptr|gParties|gPokemonStoragePtr)\b[^;\n]*?(?:=(?!=)|\+\+|--|[+|&-]=)',body))
            refs=set(re.findall(r'\b[A-Za-z_]\w*\b',body))
            outputs={'VAR_'+v for v in re.findall(r'\bgSpecialVar_(0x800[0-9A-Fa-f])\s*(?:=(?!=)|[+|&-]=|\+\+|--)',body)}
            # The Bag's chosen item (gSpecialVar_ItemId is VAR_ITEM_ID).
            if re.search(r'\bgSpecialVar_ItemId\s*=(?!=)',body):outputs.add('VAR_ITEM_ID')
            functions[match[1]]={'reads':reads,'outputs':outputs,'calls':calls,'refs':refs,'writes':writes,'result_writes':bool(re.search(r'\bgSpecialVar_Result\s*(?:=(?!=)|[+|&-]=|\+\+|--)',body)),'source':f'{path.relative_to(ROOT)}:{raw.count(chr(10),0,match.start())+1}'}
    return functions


@lru_cache(None)
def native_input_names(name):
    """Source-read scratch input names; None for a special absent from C index."""
    if name=='TryUnlockDarkraiLegendarySign':return frozenset() # writes its own selected id first
    index=_native_function_index()
    if name not in index:return None
    reads=set();seen=set();queue=[name]
    while queue:
        fn=queue.pop()
        if fn in seen or fn not in index:continue
        seen.add(fn);row=index[fn];reads.update(row['reads'])
        queue.extend(row['calls']-seen)
    return frozenset(reads)


# Exact outputs where the call-graph closure over-approximates. The party
# picker (PARTY_ACTION_CHOOSE_AND_CLOSE) returns only the chosen slot through
# BufferMonSelection (src/party_menu.c); the closure also reaches unrelated
# menu modes and would erase a trade's VAR_0x8005 index and VAR_0x8009 species.
NATIVE_OUTPUT_OVERRIDES={'ChoosePartyMon':frozenset({'VAR_0x8004'})}


@lru_cache(None)
def native_output_names(name):
    """Scratch writes from C bodies and referenced callback/helper bodies.

    This conservative may-write set invalidates stale parser inputs. A matching
    native output domain is a separate proof, never inferred from a write.
    """
    if name in NATIVE_OUTPUT_OVERRIDES:return NATIVE_OUTPUT_OVERRIDES[name]
    index=_native_function_index()
    if name not in index:return None
    outputs=set();seen=set();queue=[name]
    while queue:
        function=queue.pop()
        if function in seen:continue
        seen.add(function);row=index[function];outputs.update(row['outputs'])
        queue.extend(((row['calls']|row['refs']) & index.keys())-seen)
    return frozenset(outputs)


@lru_cache(None)
def native_preserves_persistent_state(name):
    """Conservative source proof for the service-loop guard.

    Saved fields, item/party writes, VarSet/FlagSet and referenced callbacks all
    make a helper mutating. Absent C bodies are not assumed to be harmless.
    """
    index=_native_function_index()
    if name not in index:return False
    seen=set();queue=[name]
    while queue:
        fn=queue.pop()
        if fn in seen:continue
        seen.add(fn);row=index[fn]
        if row['writes']:return False
        queue.extend(((row['calls'] | row['refs']) & index.keys())-seen)
    return True


@lru_cache(None)
def native_writes_result(name):
    """Actual gSpecialVar_Result writes, including referenced native callbacks."""
    index=_native_function_index()
    if name not in index:return None
    seen=set();queue=[name]
    while queue:
        fn=queue.pop()
        if fn in seen:continue
        seen.add(fn);row=index[fn]
        if row['result_writes']:return True
        queue.extend(((row['calls'] | row['refs']) & index.keys())-seen)
    return False


def numeric_domain(value,state,constants=None,_depth=0):
    """Small source expressions containing native/random choice domains."""
    constants=constants or {}
    if _depth>30:return None
    if isinstance(value,str):value=value.strip()
    number=_number(value,constants)
    if number is not None:return {number}
    if not isinstance(value,str):return None
    if value.startswith(('RANDOM:','SPECIAL:','COMMAND:','DYN_MULT:','CHECKCOINS','COINS')):return evaluate(value,state,constants)
    if value.startswith('(') and value.endswith(')'):
        inner=value[1:-1];depth=0;split=None
        for i,ch in enumerate(inner):
            depth+=(ch=='(')-(ch==')')
            if ch in ('+','-') and depth==0 and i>0:
                split=(i,ch)
        if split:
            i,ch=split
            left=numeric_domain(inner[:i],state,constants,_depth+1);right=numeric_domain(inner[i+1:],state,constants,_depth+1)
            if left is None or right is None:return None
            return {a+b if ch=='+' else a-b for a in left for b in right}
    variables=state.get('vars',{})
    if value in variables and variables[value]!=value:return numeric_domain(variables[value],state,constants,_depth+1)
    if value in constants and constants[value]!=value:return numeric_domain(constants[value],state,constants,_depth+1)
    try:tree=ast.parse(value,mode='eval').body
    except (SyntaxError,ValueError):return None
    def domain(node):
        if isinstance(node,ast.Constant) and isinstance(node.value,int):return {node.value}
        if isinstance(node,ast.Name):return numeric_domain(node.id,state,constants,_depth+1)
        if isinstance(node,ast.UnaryOp):
            values=domain(node.operand)
            if values is None:return None
            if isinstance(node.op,ast.USub):return {-n for n in values}
            if isinstance(node.op,ast.UAdd):return values
            if isinstance(node.op,ast.Invert):return {~n for n in values}
        if isinstance(node,ast.BinOp):
            left=domain(node.left);right=domain(node.right)
            if left is None or right is None or len(left)*len(right)>100000:return None
            operations={ast.Add:lambda a,b:a+b,ast.Sub:lambda a,b:a-b,ast.Mult:lambda a,b:a*b,
                        ast.FloorDiv:lambda a,b:a//b,ast.Div:lambda a,b:a//b,ast.Mod:lambda a,b:a%b,
                        ast.LShift:lambda a,b:a<<b,ast.RShift:lambda a,b:a>>b,
                        ast.BitAnd:lambda a,b:a&b,ast.BitOr:lambda a,b:a|b,ast.BitXor:lambda a,b:a^b}
            operation=operations.get(type(node.op))
            if operation is None:return None
            try:return {operation(a,b) for a in left for b in right}
            except (ZeroDivisionError,ValueError):return None
        return None
    return domain(tree)


@lru_cache(None)
def _prize_flags():
    text=(ROOT/'src/field_specials.c').read_text().split('sEmeraldChampionsGameCornerPokemonPrizes[]',1)[1].split('};',1)[0]
    return dict(re.findall(r'\{(SPECIES_\w+),\s*(FLAG_\w+)\}',text))


def _initial_starters(state,constants):
    variables=state.get('vars',{});generation=_number(state.get('starter_generation',variables.get('VAR_STARTER_GEN',0)),constants)
    choices=_starter_rows().get(generation,_starter_rows()[0])
    first=_number(state.get('first',variables.get('VAR_STARTER_MON')),constants)
    second=_number(state.get('second'),constants)
    opening=_number(variables.get('VAR_EC_OPENING_STATE'),constants);savedsecond=_number(variables.get('VAR_EC_SECOND_STARTER'),constants)
    if second is None and opening in (1,2) and savedsecond in (1,2,3):second=savedsecond-1
    return {choices[index] for index in (first,second) if index in range(3)}


@lru_cache(None)
def _trade_rows():
    text=(ROOT/'src/data/trade.h').read_text()
    rows={}
    for match in re.finditer(r'\[(INGAME_TRADE_\w+)\]\s*=\s*\{(.*?)\n\s*\}',text,re.S):
        rows[match[1]]=dict(re.findall(r'\.(species|requestedSpecies|heldItem)\s*=\s*(\w+)',match[2]))
    return rows


@lru_cache(None)
def _lady_rows():
    text=(ROOT/'src/data/lilycove_lady.h').read_text()
    quiz=[dict(answer=a,prize=p) for a,p in re.findall(r'\.answer\s*=\s*(EC_WORD_\w+),\s*\.prize\s*=\s*(ITEM_\w+)',text)]
    favor=[]
    for accepted,prize in re.findall(r'\.acceptedItems\s*=\s*(\w+),\s*\.prize\s*=\s*(ITEM_\w+)',text):
        body=re.search(r'\b'+accepted+r'\[\]\s*=\s*\{(.*?)\};',text,re.S)
        favor.append({'accepted':set(re.findall(r'ITEM_\w+',body[1]))-{'ITEM_NONE'},'prize':prize})
    return quiz,favor


def initial_lady_variants(trainer_id=None):
    """JSON-ready legal new-save alternatives, never merged into one save.

    Native trainer-id selection and randomized question/favor/best-item/category
    are correlated alternatives. Callers choose one branch or list conditional
    rewards; these rows do not grant items or simulate optional activity wins.
    """
    ids=[(trainer_id%6)>>1] if trainer_id is not None else range(3)
    variants=[]
    for id_ in ids:
        if id_==0:
            for qid,row in enumerate(_lady_rows()[0]):
                variants.append({'id':0,'quiz':{'state':0,'question_id':qid,'correct_answer':row['answer'],
                    'prize':row['prize'],'player_answer':'EC_EMPTY_WORD','author':2,'waiting':False},
                    'source':'src/lilycove_lady.c:287'})
        elif id_==1:
            for fid,row in enumerate(_lady_rows()[1]):
                for item in sorted(row['accepted']):
                    variants.append({'id':1,'favor':{'state':0,'favor_id':fid,'best_item':item,
                        'num_items_given':0,'liked':False,'has_player_name':False,'item':'ITEM_NONE'},
                        'source':'src/lilycove_lady.c:120'})
        elif id_==2:
            for category in range(5):
                variants.append({'id':2,'contest':{'category':category,'given':False,'good':0,'other':0},
                    'source':'src/lilycove_lady.c:590'})
    return variants


@lru_cache(None)
def _battle_stock_rows():
    text=(ROOT/'src/field_specials.c').read_text()
    body=text.split('sEmeraldChampionsBattleItemCategories[]',1)[1].split('};',1)[0]
    categories={}
    for name,array in re.findall(r'\[(EC_BATTLE_ITEM_CATEGORY_\w+)\]\s*=\s*(\w+)',body):
        source=re.search(r'\b'+array+r'\[\]\s*=\s*\{(.*?)\};',text,re.S)
        categories[name]=re.findall(r'ITEM_\w+',source[1].split('ITEM_NONE',1)[0])
    body=text.split('sBadgeStockedBattleItems[]',1)[1].split('};',1)[0]
    badge_stock={item:int(count) for item,count in re.findall(r'\{(ITEM_\w+),\s*(\d+)\}',body)}
    body=text.split('static enum Species GetFormEquipmentSpecies(enum Item item)\n{',1)[1].split('\n}',1)[0]
    equipment=dict(re.findall(r'case\s+(ITEM_\w+):\s*return\s+(SPECIES_\w+)',body))
    item_text=(ROOT/'src/data/items.h').read_text()
    for item,entry in re.findall(r'\[(ITEM_\w+)\]\s*=\s*\{(.*?)\n\s*\}',item_text,re.S):
        if re.search(r'\.sortType\s*=\s*ITEM_TYPE_MEMORY\b',entry):equipment[item]='SPECIES_SILVALLY'
    return {'categories':categories,'badge_stock':badge_stock,'equipment':equipment}


def decoration_trade_possible(state):
    """Trader identity, unused one-time trade and an acquired PC decoration.

    Ordinary preparation can remove placed decorations and free PC storage.
    No character replacement by record mixing is inferred for a fixed save.
    """
    id_=state.get('mauville_man_id');trainer=state.get('trainer_id')
    if id_ is None and trainer is not None:id_=(trainer%10)//2
    if id_!=2 or state.get('trader',{}).get('already_traded',False):return False
    decorations=set(state.get('decorations',()))
    if state.get('preparation_possible'):decorations.update(i for i in state.get('items',()) if i.startswith('DECOR_') and i!='DECOR_NONE')
    in_use=set(state.get('decorations_in_use',()))
    return bool(decorations if state.get('preparation_possible') else decorations-in_use)


def _lady_predicate(name,state,constants):
    """Concrete lady save data, with legal quiz-answer choice only after setup.

    Trainer-id-selected lady and randomized question/favor are explicit state;
    theoretical preparation does not select a different save's lady or prize.
    """
    lady=state.get('lilycove_lady',{});quiz=lady.get('quiz',{});favor=lady.get('favor',{});contest=lady.get('contest',{})
    if name=='QuizPrizeItem':
        prize=quiz.get('prize');qid=quiz.get('question_id')
        if prize is None and qid in range(len(_lady_rows()[0])):prize=_lady_rows()[0][qid]['prize']
        number=_number(prize,constants);return {number} if number is not None else None
    if name in ('GetLilycoveLadyId','Script_GetLilycoveLadyId','SetLilycoveLadyGfx'):
        id_=lady.get('id'); trainer=state.get('trainer_id')
        if id_ is None and trainer is not None:id_=(trainer%6)>>1
        return None if id_ is None else {int(id_==2) if name=='SetLilycoveLadyGfx' else id_}
    if name in ('GetQuizLadyState','GetFavorLadyState'):
        value=(quiz if name=='GetQuizLadyState' else favor).get('state')
        return None if value is None else {value if value in (1,2) else 0}
    bools={'HasAnotherPlayerGivenFavorLadyItem':(favor,'has_player_name'),'DidFavorLadyLikeItem':(favor,'liked'),
           'IsQuizLadyWaitingForChallenger':(quiz,'waiting'),'HasPlayerGivenContestLadyPokeblock':(contest,'given')}
    if name in bools:
        row,field=bools[name];return {int(bool(row[field]))} if field in row else None
    if name in ('GetQuizAuthor','BufferQuizAuthorNameAndCheckIfLady'):
        author=quiz.get('author');return None if author is None else {int(author==2) if name.startswith('Buffer') else author}
    if name=='IsQuizAnswerCorrect':
        correct=quiz.get('correct_answer');answer=quiz.get('player_answer');qid=quiz.get('question_id')
        if correct is None and qid in range(len(_lady_rows()[0])):correct=_lady_rows()[0][qid]['answer']
        if correct is None:return None
        if answer is not None:return {int(answer==correct)}
        return {0,1} if state.get('preparation_possible') and quiz.get('answer_unlocked') else None
    if name in ('Script_DoesFavorLadyLikeItem','FavorLadyGetPrize'):
        id_=favor.get('favor_id')
        if id_ not in range(len(_lady_rows()[1])):return None
        row=_lady_rows()[1][id_]
        if name=='FavorLadyGetPrize':
            number=_number(row['prize'],constants);return {number} if number is not None else None
        item=state.get('selected_item');return {int(item in row['accepted'])} if item is not None else None
    if name=='IsFavorLadyThresholdMet':
        count=favor.get('num_items_given');return {int(count>=5)} if count is not None else None
    if name=='GetContestLadyPokeblockState':
        count=contest.get('good');return None if count is None else {1 if count>=5 else 2 if count==0 else 0}
    if name=='ShouldContestLadyShowGoOnAir':
        good=contest.get('good');other=contest.get('other');return {int(good>=5 or other>=5)} if good is not None and other is not None else None
    return NotImplemented


@lru_cache(None)
def _berry_phrases():
    text=(ROOT/'src/easy_chat.c').read_text().split('sBerryMasterWifePhrases[][2]',1)[1].split('};',1)[0]
    return {name:[word.strip() for word in words.split(',')] for name,words in
            re.findall(r'\[(PHRASE_\w+)\s*-\s*1\]\s*=\s*\{([^}]+)\}',text)}


@lru_cache(None)
def _easy_word_rows():
    header=(ROOT/'include/constants/easy_chat.h').read_text();rows={}
    for path in (ROOT/'src/data/easy_chat').glob('easy_chat_group_*.h'):
        text=path.read_text()
        for word,body in re.findall(r'\[EC_INDEX\((EC_WORD_\w+)\)\]\s*=\s*\{(.*?)\}',text,re.S):
            group=re.search(r'^#define\s+'+word+r'\s+\(\((EC_GROUP_\w+)',header,re.M)
            enabled=re.search(r'\.enabled\s*=\s*(TRUE|FALSE)',body)
            if group and enabled:rows[word]=(group[1],enabled[1]=='TRUE')
    return rows


def _easy_word_available(word,state,constants):
    pokemon=re.fullmatch(r'EC_POKEMON\((\w+)\)',word)
    if pokemon:
        species='SPECIES_'+pokemon[1]
        seen=set(state.get('seen',()))|set(state.get('caught',()))
        if state.get('preparation_possible'):seen.update(state.get('species',()))
        return species in seen
    row=_easy_word_rows().get(word)
    if row is None:return False
    group,enabled=row
    if not enabled:return False
    if group=='EC_GROUP_EVENTS':return _flag(state,'FLAG_SYS_GAME_CLEAR',constants)
    group_id=_number(group,constants)
    if group_id is None:
        text=(ROOT/'include/constants/easy_chat.h').read_text()
        match=re.search(r'^#define\s+'+group+r'\s+(\d+)\b',text,re.M)
        group_id=int(match[1]) if match else None
    return group_id is not None and 1<=group_id<=16


def _scratch_output(args,state,constants):
    if args.get('function')=='GetLevelCapForScriptedGift' and args.get('variable')=='VAR_0x800A':
        return {state['cap']} if 'cap' in state else None
    function=args.get('function');variable=args.get('variable')
    # The player's tile (src/field_specials.c StorePlayerCoordsInVars). Arrival
    # scenes branch on it only for camera framing, e.g. Sootopolis' legends.
    if function=='StorePlayerCoordsInVars' and variable in ('VAR_0x8004','VAR_0x8005'):
        return set(range(256))
    if function=='ShowEasyChatScreen' and variable=='VAR_0x8004':
        mode=_token(_arg(args,'VAR_0x8004',state,constants),'EASY_CHAT_TYPE_',constants)
        # Token can remain literal when constants are not supplied.
        if mode=='EASY_CHAT_TYPE_GOOD_SAYING':
            phrases=_berry_phrases();selected=state.get('easy_chat_phrase')
            allowed={name:words for name,words in phrases.items() if all(_easy_word_available(w,state,constants) for w in words)}
            numbers={name:(_number(name,constants) or list(phrases).index(name)+1) for name in phrases}
            if selected is not None:
                return {next((numbers[name] for name,words in allowed.items() if list(selected)==words),0)}
            return {0}|{numbers[name] for name in allowed} if state.get('preparation_possible') else None
        return None
    if function=='GetContestWinnerId' and variable=='VAR_0x8005':return evaluate('SPECIAL:ContestWinnerId()',state,constants)
    if function=='GetContestPlayerId' and variable=='VAR_0x8004':return evaluate('SPECIAL:ContestPlayerId()',state,constants)
    if function=='BufferQuizPrizeItem' and variable=='VAR_0x8005':return evaluate('SPECIAL:QuizPrizeItem()',state,constants)
    # The Bag picker: any item the player holds, or ITEM_NONE on cancel.
    if variable=='VAR_ITEM_ID' and function in ('Bag_ChooseItem','ChooseItem'):
        items=state.get('items')
        if items is None:return None
        numbers={_number(item,constants) for item in items}
        return ({0}|numbers) if None not in numbers else None
    if function=='ChoosePartyMon' and variable=='VAR_0x8004':
        party=state.get('party')
        if party is not None:return set(range(len(party)))|{255}
        if state.get('preparation_possible'):
            species=state.get('species')
            if species is not None:return set(range(min(6,len(species))))|{255}
        return None
    if function=='FossilToSpecies' and variable=='VAR_0x8006':
        item=_token(_arg(args,'VAR_0x8004',state,constants),'ITEM_',constants)
        text=(ROOT/'src/field_specials.c').read_text().split('sRevivableFossils[]',1)[1].split('};',1)[0]
        rows=dict(re.findall(r'\{(ITEM_\w+),\s*(SPECIES_\w+)\}',text))
        species=rows.get(item)
        if species is None:
            previous=_number(_arg(args,'VAR_0x8006',state,constants),constants)
            return {previous} if item is not None and previous is not None else None
        number=_number(species,constants);return {number} if number is not None else None
    if function=='SetSpeciesAndEggMove':
        text=(ROOT/'src/field_specials.c').read_text().split('void SetSpeciesAndEggMove(void)',1)[1].split('u8 eligible',1)[0]
        values=set();bike=bool(set(state.get('items',())) & {'ITEM_BICYCLE','ITEM_MACH_BIKE','ITEM_ACRO_BIKE'})
        for species,moves,license_,badge,needs in re.findall(r'\{(SPECIES_\w+),\s*\{([^}]+)\},\s*(\w+),\s*(\w+),\s*(TRUE|FALSE)\}',text):
            if license_!='0' and not _flag(state,license_,constants):continue
            if badge!='0' and not _flag(state,badge,constants):continue
            if needs=='TRUE' and not bike:continue
            if variable=='VAR_0x8004':values.add(_number(species,constants))
            elif variable=='VAR_0x8005':values.update(_number(move.strip(),constants) for move in moves.split(','))
        return values-{None} if values else None
    # Output invalidation is intentionally broader than known domains.
    return None


@lru_cache(None)
def _scroll_menu_rows(menu):
    text=(ROOT/'src/field_specials.c').read_text().split('void ShowScrollableMultichoice(void)',1)[1].split('static const u8 *const sScrollableMultichoiceOptions',1)[0]
    # Shared bodies: `case A: case B: case C: body` (the Game Corner starters).
    match=re.search(r'case\s+'+re.escape(menu)+r'\s*:(?:\s*case\s+\w+\s*:)*(.*?)(?:break;|case\s+)',text,re.S)
    count=re.search(r'tNumItems\s*=\s*(\d+)',match[1]) if match else None
    return int(count[1]) if count else None


def _local_discovery(args,state,constants):
    sign=_token(_arg(args,'VAR_0x8004',state,constants),'LEGENDARY_SIGN_',constants)
    maps={'LEGENDARY_SIGN_MELOETTA':'DewfordMeadow','LEGENDARY_SIGN_LANDORUS':'Route111_RuinsExterior','LEGENDARY_SIGN_MARSHADOW':'Route113_GlassWorkshop'}
    if sign not in maps:return {0}
    location=state.get('current_map')
    if location is None:return None
    if location!=maps[sign] or _sign_sets(state,'legend_unlocked',sign) or _sign_sets(state,'legend_caught',sign):return {0}
    row=_gates()[sign.replace('LEGENDARY_SIGN_','SPECIES_')]
    badges=sum(_flag(state,f'FLAG_BADGE{i:02d}_GET',constants) for i in range(1,9))
    if badges<row['badges'] or (row['flag'] and not _flag(state,row['flag'],constants)):return {0}
    if sign=='LEGENDARY_SIGN_MARSHADOW':
        soot=_number(state.get('vars',{}).get('VAR_EC_SOOT_PROGRESS'),constants)
        target=_number('EC_SOOT_MARSHADOW_TARGET',constants);mask=_number('EC_SOOT_TOTAL_MASK',constants)
        if soot is not None and target is not None and mask is not None:return {int((soot&mask)>=target)}
        return {0,1} if state.get('preparation_possible') and state.get('collectable_ash') else None
    if sign=='LEGENDARY_SIGN_LANDORUS':
        if state.get('party') is not None:
            family=_family(state,'SPECIES_CASTFORM',constants);return {int(family)} if family is not None else None
        return {0,1} if state.get('preparation_possible') and 'SPECIES_CASTFORM' in state.get('species',set()) else {0}
    party=state.get('party')
    if party is not None:return {int(any(isinstance(p,dict) and 'MOVE_SING' in p.get('moves',[]) for p in party))}
    return {0,1} if state.get('preparation_possible') and 'MOVE_SING' in state.get('moves_possible',set()) else {0}
