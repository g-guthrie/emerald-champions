"""Bound the visitor's dynamic dialogue using the actual item and Easy Chat tables."""
from pathlib import Path
import re, json
ROOT = Path(__file__).resolve().parents[2]
base = ROOT/'work/dialogue-slateport-20260910/scan_northern_routes.py'
ns = {'__file__': str(base)}
exec(base.read_text().split('paths=')[0], ns)
width = ns['width']
lady = (ROOT/'src/data/lilycove_lady.h').read_text()
items = {}
for key, body in re.findall(r'\[(ITEM_\w+)\]\s*=\s*\{(.*?)(?=\n    \[ITEM_|\Z)', (ROOT/'src/data/items.h').read_text(), re.S):
    name = re.search(r'\.name\s*=\s*ITEM_NAME\("([^"]+)"\)', body)
    if name and not re.search(r'\.importance\s*=\s*[1-9]', body):
        items[key] = name[1]
longest = lambda values: max(values, key=width)
any_item = longest(items.values())
accepted_item = longest(items[key] for key in re.findall(r'\bITEM_\w+', lady) if key in items)
request = longest(re.findall(r'\.request = COMPOUND_STRING\("([^"]+)"\)', lady))
mon = longest(re.findall(r'\.monName = COMPOUND_STRING\("([^"]+)"\)', lady))
category = longest(re.findall(r'\.categoryName = COMPOUND_STRING\("([^"]+)"\)', lady))
groups = '\n'.join(p.read_text() for p in (ROOT/'src/data/easy_chat').glob('easy_chat_group_*.h'))
words = re.findall(r'\.text = COMPOUND_STRING\("([^"]+)"\)', groups)
for kind, files, field, pattern in [
    ('MOVE', [ROOT/'src/data/moves_info.h'], 'name', r'COMPOUND_STRING'),
    ('SPECIES', list((ROOT/'src/data/pokemon/species_info').glob('gen_*_families.h')), 'speciesName', '_'),
]:
    selected = set(re.findall(r'\b'+kind+r'_\w+', groups))
    for path in files:
        blocks = re.findall(r'\[('+kind+r'_\w+)\]\s*=\s*\{(.*?)(?=\n    \['+kind+r'_|\Z)', path.read_text(), re.S)
        for key, body in blocks:
            if key in selected:
                value = re.search(r'\.'+field+r'\s*=\s*'+pattern+r'\("([^"]+)"\)', body)
                if value: words.append(value[1].upper())
answer = longest(words)
source = (ROOT/'data/scripts/lilycove_lady.inc').read_text()
rows = []
for m in re.finditer(r'^(\w+):{1,2}\s*\n((?:\s*\.string[^\n]*\n)+)', source, re.M):
    label = m[1].split('_Text_')[1]
    values = {}
    if label in ['ObsessedWithThing','PlayerGaveMeBadThing','PlayerGaveMeGreatThing','WillYouShareThing','IllTryToCherishIt','IWillCherishThis','IWillTreasureThis']:
        values = {'STR_VAR_1':request, 'STR_VAR_2':any_item if label in ['PlayerGaveMeBadThing','IllTryToCherishIt'] else accepted_item, 'STR_VAR_3':'W'*7}
    if label in ['WaitingForChallenger','YouGotItRightYouveWonPersonsPrize','XReceivedOneY']:
        values = {'STR_VAR_1':'W'*7, 'STR_VAR_2':any_item}
    if label == 'WrongTheCorrectAnswerIs': values = {'STR_VAR_3':answer}
    if label == 'IGetToKeepPrize': values = {'STR_VAR_1':any_item}
    if source[:m.start()].count('\n') >= 700:
        values = {'STR_VAR_1':mon, 'STR_VAR_2':category}
    text = ''.join(re.findall(r'\.string\s+"(.*)"',m[2]))
    for line in re.split(r'\\[nlp]|\$',text):
        if not line: continue
        def expand(x):
            if x[1] == 'PLAYER': return 'W'*7
            if x[1] == 'POKEBLOCK': return 'W'*5
            assert x[1] in values, (label, x[1])
            return values[x[1]]
        rendered = re.sub(r'\{([^}]+)\}',expand,line)
        rows.append({'label':label,'text':line,'expanded':rendered,'width':width(rendered)})
output = ROOT/'work/dialogue-slateport-20260910/lilycove-lady-text-layout.json'
output.write_text(json.dumps(rows,indent=2,ensure_ascii=False)+'\n')
print('Bounds:', {'item':any_item, 'accepted_item':accepted_item, 'answer':answer, 'nickname':mon, 'category':category})
print('Blocks:',len(set(r['label'] for r in rows)), 'lines:',len(rows),'maximum:',max(r['width'] for r in rows))
issues = [r for r in rows if r['width'] > 213]
for r in issues: print(r)
assert not issues
