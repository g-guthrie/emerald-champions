import ast
from collections import Counter, defaultdict, deque
from itertools import groupby
from pathlib import Path
import re

REF = re.compile(r'(?<![\w:])([a-z_]+:[A-Za-z0-9_][A-Za-z0-9_.-]*)')
FIELDS = ('species', 'item', 'ability', 'nature', 'evs', 'moves')


def references(value, path=()):
    if isinstance(value, str):
        for match in REF.finditer(value):
            yield match[0], path
    elif isinstance(value, dict):
        for key, child in value.items():
            yield from references(child, (*path, key))
    elif isinstance(value, (list, tuple)):
        for i, child in enumerate(value):
            yield from references(child, (*path, i))


def set_line(value):
    species, item, ability, nature, evs, moves = (value[x] for x in FIELDS)
    extra = {k: v for k, v in value.items() if k not in FIELDS}
    return ' '.join((species.removeprefix('species:'), '@'+item.removeprefix('item:'),
                     ability.removeprefix('ability:'), nature.removeprefix('nature:'),
                     '/'.join(map(str, evs)))) + ' | ' + ' '.join(x.removeprefix('move:') for x in moves) + ' | ' + repr(extra)


def parse_set(line):
    main, moves, extra = line.split(' | ', 2)
    species, item, ability, nature, evs = main.split()
    return dict(species='species:'+species, item='item:'+item[1:], ability='ability:'+ability,
                nature='nature:'+nature, evs=tuple(map(int, evs.split('/'))),
                moves=tuple('move:'+x for x in moves.split()), **ast.literal_eval(extra))


def learn_line(value):
    def names(moves):
        return ' '.join(x.removeprefix('move:') for x in moves)
    if isinstance(value, dict) and 'base' in value and set(value)<= {'base','add','drop'}:
        return 'pool '+value['base']+' | '+names(value.get('add', ()))+' | '+names(value.get('drop', ()))
    if isinstance(value, tuple) and all(isinstance(x,str) and x.startswith('move:') for x in value):
        return 'moves '+names(value)
    if isinstance(value, tuple) and all(isinstance(x,tuple) and len(x)==2 and isinstance(x[0],int) and isinstance(x[1],str) and x[1].startswith('move:') for x in value):
        return 'levels '+';'.join(str(level)+':'+names(x[1] for x in rows) for level,rows in groupby(value,lambda x:x[0]))


def parse_learn(line):
    def moves(s):
        return tuple('move:'+x for x in s.split())
    mode,body=line.split(' ',1)
    if mode=='moves':
        return moves(body)
    if mode=='levels':
        return tuple((int(level),move) for block in body.split(';') for level,values in [block.split(':',1)] for move in moves(values))
    base,add,drop=body.split(' | ')
    return {'base':base} | ({'add':moves(add)} if add else {}) | ({'drop':moves(drop)} if drop else {})


def dump(records, destination):
    schemas = {}
    groups = defaultdict(list)
    for key, value in records.items():
        groups[key.split(':')[0]].append((key, value))
    for kind, rows in groups.items():
        if kind == 'set':
            continue
        fields = defaultdict(Counter)
        for _, row in rows:
            if isinstance(row, dict):
                for field, value in row.items():
                    fields[field][repr(value)] += 1
        defaults = {}
        for field, counts in fields.items():
            value, count = counts.most_common(1)[0]
            if count > len(rows) // 2:
                defaults[field] = ast.literal_eval(value)
        if defaults:
            schemas[kind] = defaults
    with Path(destination).open('w') as out:
        for kind, defaults in schemas.items():
            out.write('schema:'+kind+'\t'+repr(defaults)+'\n')
        for kind, rows in groups.items():
            for key, value in rows:
                if kind == 'set' and all(x in value for x in FIELDS):
                    encoded = 'showdown '+set_line(value)
                elif kind == 'learn' and (encoded := learn_line(value)) is not None:
                    pass
                elif isinstance(value, dict) and kind in schemas:
                    defaults = schemas[kind]
                    # An absent field differs from a field set to the schema default.
                    missing = tuple(k for k in defaults if k not in value)
                    value = {k: v for k, v in value.items() if k not in defaults or v != defaults[k]}
                    if missing:
                        value['!absent'] = missing
                    encoded = repr(value)
                else:
                    encoded = repr(value)
                out.write(key+'\t'+encoded+'\n')


class Game:
    def __init__(self, path):
        self.path = Path(path)
        self.records = {}
        self.lines = {}
        self.schemas = schemas = {}
        inherited = {}
        for n, line in enumerate(self.path.read_text().splitlines(), 1):
            if not line:
                continue
            key, encoded = line.split('\t', 1)
            kind, name = key.split(':', 1)
            if key in self.lines:
                raise ValueError(f'{self.path}:{n}: duplicate {key}')
            self.lines[key] = n
            if encoded.startswith('showdown '):
                value = parse_set(encoded[9:])
            elif kind == 'learn' and encoded.startswith(('moves ', 'levels ', 'pool ')):
                value = parse_learn(encoded)
            else:
                value = ast.literal_eval(encoded)
            if kind == 'schema':
                schemas[name] = value
                continue
            if isinstance(value, dict) and kind in schemas:
                inherited[key] = tuple(k for k in schemas[kind] if k not in value and k not in value.get('!absent', ()))
                value = schemas[kind] | value
                for field in value.pop('!absent', ()):
                    value.pop(field)
            self.records[key] = value
        self.outgoing = defaultdict(list)
        self.incoming = defaultdict(list)
        for source, value in (self.records | {'schema:'+k:v for k,v in schemas.items()}).items():
            for target, field in references(value):
                relation = self.relation(source, field)
                edge = (source, target, relation, field)
                self.outgoing[source].append(edge)
                self.incoming[target].append(edge)
        for source, fields in inherited.items():
            if fields:
                target = 'schema:'+source.split(':')[0]
                edge = (source, target, 'inherits', fields)
                self.outgoing[source].append(edge)
                self.incoming[target].append(edge)

    @staticmethod
    def relation(source, field):
        kind = source.split(':')[0]
        if kind == 'text':
            return 'claim' if field and field[0] == 'claims' else 'mentions'
        if kind == 'offer':
            if field[0] == 'pay':
                return 'prices_from' if len(field)>2 and field[2]==1 else 'consumes'
            return {'gain':'delivers', 'when':'requires', 'set':'updates'}.get(field[0], str(field[0]))
        if kind == 'set':
            return {'species':'species', 'item':'holds', 'moves':'uses', 'ability':'ability'}.get(field[0], 'references')
        if kind == 'wild':
            return 'habitat' if field[0] == 0 else 'encounters'
        if kind == 'actor':
            return {'script':'interacts', 'flag':'hidden_when'}.get(field[0], 'references')
        if kind in ('event','motion'):
            return 'continues' if field[0] == 1 else 'executes'
        return str(field[0]) if field else 'references'

    def impact(self, key):
        distance = {key:0}
        via = {}
        queue = deque([key])
        while queue:
            target = queue.popleft()
            if target != key and target.startswith('map:'):
                continue
            for source, _, relation, field in self.incoming[target]:
                if source not in distance:
                    distance[source] = distance[target]+1
                    via[source] = (target, relation, field)
                    queue.append(source)
        return distance, via

    def materialize(self):
        result = {}
        active = set()
        def resolve(key):
            if key in result:
                return result[key]
            if key in active:
                raise ValueError(f'cyclic definition: {key}')
            active.add(key)
            value = self.records[key]
            if key.startswith('learn:') and isinstance(value, dict) and 'base' in value:
                moves = (x for x in resolve(value['base']) if x not in value.get('drop', ()))
                value = tuple(dict.fromkeys((*moves, *value.get('add', ()))))
            if key.startswith('species:'):
                value = dict(value)
                pool = resolve(value['pool']) if 'pool' in value else ()
                for field in ('levelUpLearnset', 'eggMoveLearnset'):
                    if field in value:
                        rows = resolve(value[field])
                        value[field] = tuple(row for row in rows if (row[1] if field == 'levelUpLearnset' else row) in pool)
            result[key] = value
            active.remove(key)
            return value
        for key in self.records:
            resolve(key)
        return result

    def economy(self, key):
        rows = []
        for source, _, relation, field in self.incoming[key]:
            if source.startswith(('offer:','set:','gift:','interaction:')):
                places = self.locations(source)
                rows.append((source, relation, places, self.records[source]))
        return rows

    def compatibility(self):
        records = self.materialize()
        issues = []
        for key, build in records.items():
            if not key.startswith('set:'):
                continue
            species = build['species']
            pool = records[species].get('pool')
            allowed = records.get(pool, ())
            for move in dict.fromkeys(build['moves']):
                if move != 'move:NONE' and move not in allowed:
                    issues.append({'set':key, 'species':species, 'move':move, 'pool':pool,
                                   'users':sorted({edge[0] for edge in self.incoming[key]})})
        return issues

    def reconcile(self):
        issues = self.compatibility()
        removed = defaultdict(set)
        for issue in issues:
            removed[issue['set']].add(issue['move'])
        for key, moves in removed.items():
            build = self.records[key]
            build['moves'] = tuple(move for move in build['moves'] if move not in moves)
        return issues

    def locations(self, key):
        seen = {key}
        queue = deque([key])
        while queue:
            target = queue.popleft()
            for source, _, relation, field in self.incoming[target]:
                if not source.startswith(('event:', 'actor:', 'trigger:', 'interaction:', 'shop:', 'map:')):
                    continue
                # Traversal connectivity is not evidence that an offer is located there.
                if source.startswith('map:') and target.startswith('map:'):
                    continue
                if source not in seen:
                    seen.add(source)
                    if not source.startswith('map:'):
                        queue.append(source)
        return sorted(x for x in seen if x.startswith('map:'))

    def display(self, key):
        record = self.records[key]
        body = record['body']
        for slot, expression in record.get('slots', {}).items():
            upper = expression[0] == 'upper'
            if upper:
                expression = expression[1]
            op, value, *path = expression
            if op != 'field':
                raise ValueError(f'preview needs runtime expression: {expression}')
            for field in path:
                if isinstance(value, str) and value in self.records:
                    value = self.records[value]
                value = value[field]
            body = body.replace('{'+slot+'}', str(value).upper() if upper else str(value))
        return body

    def unresolved(self):
        # Report missing definitions, never turn an unknown mechanic into a no-op.
        defined = set(self.records) | {'schema:'+k for k in self.schemas}
        return {target: edges for target, edges in self.incoming.items() if target not in defined}
